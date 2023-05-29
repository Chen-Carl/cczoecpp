#include <format>
#include <sys/fcntl.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <memory>
#include "iomanager.h"
#include "utils/macro.h"

namespace cczoe {

IOManager::FdContext::EventContext &IOManager::FdContext::getEventContext(IOManager::Event event)
{
    switch (event)
    {
    case READ:
        return read;
    case WRITE:
        return write;
    default:
        CCZOE_ASSERT2(false, "FdContext::getContext returns a NONE event");
    }
    throw std::invalid_argument("FdContext::getContext invalid event");
}

void IOManager::FdContext::resetContext(IOManager::FdContext::EventContext &eventCtx)
{
    eventCtx.scheduler = nullptr;
    eventCtx.fiber.reset();
    eventCtx.cb = nullptr;
}

void IOManager::FdContext::triggerEvent(IOManager::Event event)
{
    events = (Event)(events & ~event);
    EventContext &eventCtx = getEventContext(event);
    if (eventCtx.cb)
    {
        eventCtx.scheduler->schedule(eventCtx.cb);
    }
    else
    {
        eventCtx.scheduler->schedule(eventCtx.fiber);
    }
    eventCtx.scheduler = nullptr;
}

IOManager::IOManager(std::string name, size_t threadCount) :
    Scheduler(name, threadCount)
{
    m_epfd = epoll_create(1000);
    CCZOE_ASSERT(m_epfd > 0);
    int rt = pipe(m_tickleFds);
    CCZOE_ASSERT(rt == 0);
    epoll_event event;
    memset(&event, 0, sizeof(epoll_event));
    // EPOLLET: epoll edge-triggered
    event.events = EPOLLIN | EPOLLET;
    event.data.fd = m_tickleFds[0];
    // set flag to non-block
    rt = fcntl(m_tickleFds[0], F_SETFL, O_NONBLOCK);
    rt = epoll_ctl(m_epfd, EPOLL_CTL_ADD, m_tickleFds[0], &event);
    CCZOE_ASSERT(rt == 0);
    start();
}

#include <dlfcn.h>

IOManager::~IOManager()
{
    while (!m_stopCommand)
    {
        auto sleepFunc = reinterpret_cast<void (*)(unsigned int)>(dlsym(RTLD_NEXT, "sleep"));
        sleepFunc(5);
    }
    for (size_t i = 0; i < m_threadCount; i++)
    {
        tickle();
    }
    // close handler
    close(m_epfd);
    close(m_tickleFds[0]);
    close(m_tickleFds[1]);
    // delete fd contexts pointers
    for (size_t i = 0; i < m_fdContexts.size(); i++)
    {
        if (m_fdContexts[i])
            delete m_fdContexts[i];
    }
}

int IOManager::addEvent(int fd, Event event, std::function<void()> cb)
{
    // set event fd
    FdContext *fdCtx = nullptr;
    thread::ReadScopedLock<RWMutexType> lock(m_mutex);
    if (m_fdContexts.size() > size_t(fd))
    {
        fdCtx = m_fdContexts[fd];
        lock.unlock();
    }
    else
    {
        lock.unlock();
        thread::WriteScopedLock<RWMutexType> lock(m_mutex);
        fdContextsResize(fd * 1.5);
        fdCtx = m_fdContexts[fd];
    }

    thread::ScopedLock<MutexType> lock2(fdCtx->mutex);
    // a same event cannot be added again
    if (fdCtx->events & event)
    {
        CCZOE_LOG_ERROR(CCZOE_LOG_ROOT()) << "addEvent error: fd = " << fd << ", event = " << event << ", already exists " << fdCtx->events; 
        CCZOE_ASSERT(!(fdCtx->events & event));
    }
    // op: add an event or create an event?
    int op = fdCtx->events ? EPOLL_CTL_MOD : EPOLL_CTL_ADD;
    // bind the event to m_epfd
    epoll_event epEvent;
    epEvent.events = EPOLLET | fdCtx->events | event;
    epEvent.data.ptr = fdCtx;
    int rt = epoll_ctl(m_epfd, op, fd, &epEvent);
    if (rt)
    {
        CCZOE_LOG_ERROR(CCZOE_LOG_ROOT()) << "epoll_ctl(" << m_epfd << ", " << op << ", " << fd << ", " << epEvent.events << "): failed, errno = " << errno << ", " << strerror(errno);
        return 1;
    }
    m_pendingEventCount++;
    // update fdContexts record
    fdCtx->events = (Event)(fdCtx->events | event);
    FdContext::EventContext &eventCtx = fdCtx->getEventContext(event);
    // CCZOE_ASSERT(!eventCtx.scheduler);
    // CCZOE_ASSERT(!eventCtx.fiber);
    // CCZOE_ASSERT(!eventCtx.cb);
    eventCtx.scheduler = Scheduler::GetThis();
    if (cb)
    {
        eventCtx.cb.swap(cb);
    }
    // if cb is nullptr, set the current fiber as a callback function
    else
    {
        eventCtx.fiber = fiber::Fiber::GetThis();
        CCZOE_ASSERT(eventCtx.fiber->getState() == fiber::Fiber::RUNNING);
    }
    return 0;
}

int IOManager::delEvent(int fd, Event event)
{
    // set event fd
    thread::ReadScopedLock<RWMutexType> lock(m_mutex);
    if (m_fdContexts.size() <= size_t(fd))
    {
        return 1;
    }
    FdContext *fdCtx = m_fdContexts[fd];
    lock.unlock();
    // lock fd
    thread::ScopedLock<MutexType> lock2(fdCtx->mutex);
    if (!(fdCtx->events & event))
    {
        return 1;
    }
    Event newEvent = (Event)(fdCtx->events & ~event);
    int op = newEvent ? EPOLL_CTL_MOD : EPOLL_CTL_DEL;
    epoll_event epEvent;
    epEvent.events = EPOLLET | newEvent;
    epEvent.data.ptr = fdCtx;
    int rt = epoll_ctl(m_epfd, op, fd, &epEvent);
    if (rt)
    {
        CCZOE_LOG_ERROR(CCZOE_LOG_ROOT()) << "epoll_ctl(" << m_epfd << ", " << op << ", " << fd << ", " << epEvent.events << "): failed, errno = " << errno << ", " << strerror(errno);
        return 1;
    }

    m_pendingEventCount--;
    fdCtx->events = newEvent;
    FdContext::EventContext &eventCtx = fdCtx->getEventContext(event);
    // clear callback function
    fdCtx->resetContext(eventCtx);
    return 0;
}

int IOManager::cancelEvent(int fd, Event event)
{
    thread::ReadScopedLock<RWMutexType> lock(m_mutex);
    if (m_fdContexts.size() <= size_t(fd))
    {
        return 1;
    }
    FdContext *fdCtx = m_fdContexts[fd];
    lock.unlock();
    // lock fd
    thread::ScopedLock<MutexType> lock2(fdCtx->mutex);
    if (!(fdCtx->events & event))
    {
        return 1;
    }
    Event newEvent = (Event)(fdCtx->events & ~event);
    int op = newEvent ? EPOLL_CTL_MOD : EPOLL_CTL_DEL;
    epoll_event epEvent;
    epEvent.events = EPOLLET | newEvent;
    epEvent.data.ptr = fdCtx;
    int rt = epoll_ctl(m_epfd, op, fd, &epEvent);
    if (rt)
    {
        CCZOE_LOG_ERROR(CCZOE_LOG_ROOT()) << "epoll_ctl(" << m_epfd << ", " << op << ", " << fd << ", " << epEvent.events << "): failed, errno = " << errno << ", " << strerror(errno);
        return 1;
    }

    m_pendingEventCount--;
    fdCtx->events = newEvent;
    // event is triggered before delete
    fdCtx->triggerEvent(event);
    return 0;
}

int IOManager::cancelAll(int fd)
{
    thread::ReadScopedLock<RWMutexType> lock(m_mutex);
    if (m_fdContexts.size() <= size_t(fd))
    {
        return 1;
    }
    FdContext *fdCtx = m_fdContexts[fd];
    lock.unlock();
    // lock fd
    thread::ScopedLock<MutexType> lock2(fdCtx->mutex);
    if (!fdCtx->events)
    {
        return 1;
    }

    epoll_event epEvent;
    epEvent.events = 0;
    epEvent.data.ptr = fdCtx;
    int rt = epoll_ctl(m_epfd, EPOLL_CTL_DEL, fd, &epEvent);
    if (rt)
    {
        CCZOE_LOG_ERROR(CCZOE_LOG_ROOT()) << "epoll_ctl(" << m_epfd << ", EPOLL_CTL_DEL" << ", " << fd << ", " << epEvent.events << "): failed, errno = " << errno << ", " << strerror(errno);
        return 1;
    }

    if (fdCtx->events & READ)
    {
        fdCtx->triggerEvent(READ);
        m_pendingEventCount--;
    }
    if (fdCtx->events & WRITE)
    {
        fdCtx->triggerEvent(WRITE);
        m_pendingEventCount--;
    }
    CCZOE_ASSERT(fdCtx->events == 0);
    return 0;
}

void IOManager::fdContextsResize(size_t size)
{
    m_fdContexts.resize(size);
    for (size_t i = 0; i < m_fdContexts.size(); i++)
    {
        if (!m_fdContexts[i])
        {
            m_fdContexts[i] = new FdContext;
            m_fdContexts[i]->fd = i;
        }
    }
}

void IOManager::tickle()
{
    if (m_idleThreadCount > 0)
    {
        int rt = write(m_tickleFds[0], "T", 1);
        CCZOE_ASSERT(rt);
    }
}

void IOManager::idle()
{
    // maximum events to process is 64
    epoll_event *events = new epoll_event[64];
    std::shared_ptr<epoll_event> sharedEvents(events, [](epoll_event *ptr){
        delete [] ptr;
    });

    int rt = 0;
    while (!m_stopCommand || hasTimer())
    {
        static const uint64_t maxTimeout = 3000;
        uint64_t nextTimeout = getNextTimer();
        if (nextTimeout == ~0ull)
        {
            nextTimeout = std::min(maxTimeout, nextTimeout);
        }
        else
        {
            nextTimeout = maxTimeout;
        }
        // epoll_wait registed fd
        while (1)
        {
            rt = epoll_wait(m_epfd, events, 64, nextTimeout);
            if (!(rt < 0 && errno == EINTR))
                break;
        }

        std::vector<std::function<void()>> cbs = getExpiredCb();
        if (!cbs.empty())
        {
            for (auto it = cbs.begin(); it != cbs.end(); it++)
            {
                schedule(*it);
            }
            cbs.clear();
        }

        // epoll_wait timeout
        for (int i = 0; i < rt; i++)
        {
            epoll_event &event = events[i];
            // process wakeup event
            if (event.data.fd == m_tickleFds[0])
            {
                uint8_t dummy;
                while (read(m_tickleFds[0], &dummy, 1) == 1);
            }
            // other events
            else
            {
                FdContext *fdCtx = (FdContext*)event.data.ptr;
                thread::ScopedLock<MutexType> lock(fdCtx->mutex);
                // if EPOLLERR or EPOLLHUP, trigger read and write event
                if (event.events & (EPOLLERR | EPOLLHUP))
                {
                    event.events |= EPOLLIN | EPOLLOUT;
                }
                // collect events
                int e = NONE;
                // READ event
                if (event.events & EPOLLIN)
                {
                    e |= READ;
                }
                // WRITE event
                if (event.events & EPOLLOUT)
                {
                    e |= WRITE;
                }
                // delete occuring events, and epoll_wait left events
                if ((fdCtx->events & e) != NONE)
                {
                    // delete these events e
                    int leftEvents = (fdCtx->events & ~e);
                    int op = leftEvents ? EPOLL_CTL_MOD : EPOLL_CTL_DEL;
                    event.events = EPOLLET | leftEvents;
                    // add left events
                    int rt2 = epoll_ctl(m_epfd, op, fdCtx->fd, &event);
                    if (rt2)
                    {
                        CCZOE_LOG_ERROR(CCZOE_LOG_ROOT()) << "epoll_ctl(" << m_epfd << ", " << op << ", " << fdCtx->fd << ", " << event.events << "): error, errno = " << errno << ", " << strerror(errno);
                    }
                    // trigger e(READ event)
                    if (e & READ)
                    {
                        fdCtx->triggerEvent(READ);
                        m_pendingEventCount--;
                    }
                    // trigger e(WRITE event)
                    if (e & WRITE)
                    {
                        fdCtx->triggerEvent(WRITE);
                        m_pendingEventCount--;
                    }
                }
            }
        }

        // all events processed, returning to Scheduler::run()
        // it will check the task queue and process the task
        std::shared_ptr<fiber::Fiber> curr = fiber::Fiber::GetThis();
        auto rawPtr = curr.get();
        curr.reset();
        rawPtr->yield();
    }
}

bool IOManager::hasTimer()
{
    return !empty();
}

void IOManager::onTimerInsertedAtFront()
{
    tickle();
}

IOManager *IOManager::GetThis()
{
    return dynamic_cast<IOManager *>(Scheduler::GetThis());
}

};