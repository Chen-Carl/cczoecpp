#include <list>
#include <atomic>
#include "Thread/thread.h"
#include "Thread/Mutex/mutex.h"
#include "Fiber/fiber.h"

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
        ScheduleTask cbOrFiber(task);
        if (cbOrFiber.m_fiber)
        {
            thread::ScopedLock<MutexType> lock(m_mutex);
            m_tasks.push_back(cbOrFiber.m_fiber);
        }
        else if (cbOrFiber.m_cb)
        {
            thread::ScopedLock<MutexType> lock(m_mutex);
            m_tasks.push_back(std::shared_ptr<fiber::Fiber>(new fiber::Fiber(cbOrFiber.m_cb)));
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

    static Scheduler *GetThis();
};

}