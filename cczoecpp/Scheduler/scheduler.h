#ifndef __CCZOE_SCHEDULER_H__
#define __CCZOE_SCHEDULER_H__

#include <list>
#include <atomic>
#include "Thread/Thread.h"
#include "Thread/Mutex/mutex.h"
#include "Fiber/Fiber.h"

namespace cczoe {

class Scheduler
{
    using MutexType = thread::Mutex;
private:
    // schedule task includes fibers and functions
    struct ScheduleTask
    {
        std::shared_ptr<fiber::Fiber> m_fiber;
        std::function<void()> m_cb;
        // record the thread that runs the fiber/function
        int m_threadId;

        ScheduleTask();
        ScheduleTask(std::shared_ptr<fiber::Fiber> f, int thr = -1);
        ScheduleTask(std::function<void()> f, int thr = -1);

        void reset() { m_fiber = nullptr; m_cb = nullptr; m_threadId = -1; }
    };

public:
    Scheduler(std::string name = "default_sch", size_t threadCount = 1);
    virtual ~Scheduler();

    virtual void start();
    virtual void run();
    virtual void stop();

    template <class Task>
    void schedule(Task task)
    {
        // if (m_stopCommand)
        // {
        //     CCZOE_LOG_WARN(CCZOE_LOG_ROOT()) << "Schedule task failed, the scheduler is stopped.";
        //     return;
        // }
        ScheduleTask cbOrFiber(task);
        if (cbOrFiber.m_fiber)
        {
            if (cbOrFiber.m_fiber->getState() == fiber::Fiber::State::RUNNING)
            {
                CCZOE_LOG_WARN(CCZOE_LOG_ROOT()) << "Schedule task may fail, the fiber is running.";
            }
            if (cbOrFiber.m_fiber->getState() == fiber::Fiber::State::RUNNING)
            {
                CCZOE_LOG_ERROR(CCZOE_LOG_ROOT()) << "Schedule task failed, the fiber is still running.";
            }
            thread::ScopedLock<MutexType> lock(m_mutex);
            m_tasks.push_back(cbOrFiber.m_fiber);
        }
        else if (cbOrFiber.m_cb)
        {
            thread::ScopedLock<MutexType> lock(m_mutex);
            m_tasks.push_back(std::make_shared<fiber::Fiber>(cbOrFiber.m_cb));
        }
    }

private:
    // task list
    std::list<std::shared_ptr<fiber::Fiber>> m_tasks;
    // mutex to lock private members
    MutexType m_mutex;
    // threads pool
    std::vector<std::shared_ptr<thread::Thread>> m_threads;

    virtual void idle();

protected:
    // name
    std::string m_name;
    // working thread
    size_t m_threadCount = 0;
    // stop command
    bool m_stopCommand = false;
    // idle thread
    std::atomic<size_t> m_idleThreadCount = {0};

    void setThis();

    static Scheduler *GetThis();
};

}

#endif