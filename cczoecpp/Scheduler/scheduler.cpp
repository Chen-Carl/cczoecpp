#include "scheduler.h"

namespace cczoe {

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

}

Scheduler::~Scheduler()
{

}

void Scheduler::start()
{
    m_threads.resize(m_threadCount);
    for (size_t i = 0; i < m_threadCount; i++)
    {
        m_threads[i].reset(new thread::Thread(std::bind(&Scheduler::run, this), m_name + "_" + std::to_string(i)));
    }
}

void Scheduler::run()
{
    fiber::Fiber::getThis();
    std::shared_ptr<fiber::Fiber> task;
    thread::ScopedLock<MutexType> lock(m_mutex);
    std::shared_ptr<fiber::Fiber> idleFiber(new fiber::Fiber(std::bind(&Scheduler::idle, this)));
    while (true)
    {
        task.reset();
        {
            thread::ScopedLock<MutexType> lock(m_mutex);
            auto it = m_tasks.begin();
            if (it != m_tasks.end())
            {
                task = *it;
                m_tasks.erase(it++);
            }
        }
        if (!task)
        {
            idleFiber->setIdle();
            idleFiber->resume();
            continue;
        }
        task->resume();
    }
}

void Scheduler::stop()
{
    while (!m_tasks.empty());
    m_stopCommand = true;
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
        fiber::Fiber::getThis()->yield();
    }
}

}