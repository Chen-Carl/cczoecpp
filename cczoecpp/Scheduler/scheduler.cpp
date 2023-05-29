#include <dlfcn.h>
#include "scheduler.h"
#include "Net/Hook/hook.h"

namespace cczoe {

static thread_local Scheduler *t_scheduler = nullptr;

Scheduler::ScheduleTask::ScheduleTask()
{
    // unknown construction restrains the thread id to -1
    m_threadId = -1;
}

Scheduler::ScheduleTask::ScheduleTask(std::shared_ptr<fiber::Fiber> f, int thr)
{
    m_fiber = f;
    m_threadId = thr;
}

Scheduler::ScheduleTask::ScheduleTask(std::function<void()> f, int thr)
{
    m_cb = f;
    m_threadId = thr;
}

Scheduler::Scheduler(std::string name, size_t threadCount) : 
    m_name(name), m_threadCount(threadCount)
{
    t_scheduler = this;
}

Scheduler::~Scheduler()
{
    while (!m_stopCommand)
    {
        auto sleepFunc = reinterpret_cast<void (*)(unsigned int)>(dlsym(RTLD_NEXT, "sleep"));
        sleepFunc(5);
    }
    stop();
}

void Scheduler::start()
{
    m_threads.resize(m_threadCount);
    for (size_t i = 0; i < m_threadCount; i++)
    {
        m_threads[i] = std::make_shared<thread::Thread>(m_name + "_" + std::to_string(i), std::bind(&Scheduler::run, this));
    }
}

void Scheduler::run()
{
    fiber::Fiber::GetThis();
    setThis();
    // setHookEnable(true);
    std::shared_ptr<fiber::Fiber> task;
    std::shared_ptr<fiber::Fiber> idleFiber = std::make_shared<fiber::Fiber>(std::bind(&Scheduler::idle, this));
    while (true)
    {
        task.reset();
        {
            thread::ScopedLock<MutexType> lock(m_mutex);
            auto it = m_tasks.begin();
            if (it != m_tasks.end())
            {
                task = *it;
                it = m_tasks.erase(it);
            }
        }
        if (!task)
        {
            m_idleThreadCount++;
            idleFiber->setIdle();
            idleFiber->resume();
            m_idleThreadCount--;
            continue;
        }
        task->resume();
    }
}

void Scheduler::stop()
{
    m_stopCommand = true;
    while (true)
    {
        thread::ScopedLock<MutexType> lock(m_mutex);
        if (m_tasks.empty())
        {
            break;
        }
    }
    std::vector<std::shared_ptr<thread::Thread>> thrs;
    {
        thread::ScopedLock<MutexType> lock(m_mutex);
        thrs.swap(m_threads);
    }
    for (auto &i : thrs)
    {
        i->join();
    }
}

void Scheduler::idle()
{
    while (!m_stopCommand)
    {
        CCZOE_LOG_INFO(CCZOE_LOG_ROOT()) << "idle";
        fiber::Fiber::GetThis()->yield();
    }
}

void Scheduler::setThis() 
{
    t_scheduler = this;
}

Scheduler *Scheduler::GetThis()
{
    return t_scheduler;
}

}