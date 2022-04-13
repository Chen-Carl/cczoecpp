#ifndef __CCZOE_IOMANAGER_H__
#define __CCZOE_IOMANAGER_H__

#include <sys/epoll.h>
#include "Thread/Mutex/mutex.h"
#include "scheduler.h"

namespace cczoe {

class IOManager : public Scheduler
{
    using MutexType = thread::Mutex;
    using RWMutexType = thread::RWMutex;
public:
    enum Event 
    {
        NONE = 0x0,
        READ = 0x1,         // EPOLLIN
        WRITE = 0x4         // EPOLLOUT
    };

private:
    /**
     * @brief schedule task descriptor - (fd, type, callback)
     * @param fd descriptor
     * @param event event type
     * @param cb call back function
     */
    struct FdContext
    {
        struct EventContext
        {
            Scheduler *scheduler = nullptr;         // host fiber scheduler
            std::shared_ptr<fiber::Fiber> fiber;    // call back fiber
            std::function<void()> cb;               // call back function
        };

        int fd = 0;                 // event descriptor
        Event events = NONE;        // event type
        EventContext read;          // read event
        EventContext write;         // write event
        MutexType mutex;

        /**
         * @brief get EventContext(read or write) according to event type
         * @param event event type
         * @return reference to the EventContext(read or write)
         */
        EventContext &getEventContext(Event event);
        void resetContext(EventContext &eventCtx);
        void triggerEvent(Event event);
    };

public:
    IOManager(std::string name = "default_iom", size_t threadCount = 1);
    ~IOManager();

    /** 
     * @brief add an event to fd
     * @param fd    socked fd
     * @param event event type
     * @param cb    callback
     * @return 0: success   1: failed
     */
    int addEvent(int fd, Event event, std::function<void()> cb = nullptr);
    int delEvent(int fd, Event event);
    /**
     * @brief similar to delEvent, cancelEvent will execute the callback function and then delete the event
     * @return 0: success   1: failed
     */ 
    int cancelEvent(int fd, Event event);
    int cancelAll(int fd);

    void tickle();
    virtual void idle() override;
    virtual void stop() override;

private:
    int m_epfd = 0;             // epoll descriptor
    int m_tickleFds[2];         // pipe fds: fd[0] read, fd[1] write
    RWMutexType m_mutex;        // mutex for iomanager
    std::atomic<size_t> m_pendingEventCount = {0};  // pending event number
    std::vector<FdContext*> m_fdContexts;           // fd contexts

    void fdContextsResize(size_t size);
};

}

#endif