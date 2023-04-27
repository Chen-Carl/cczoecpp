#include "Log/log.h"
#include "Scheduler/scheduler.h"

using namespace cczoe;

void test_fiber(int i)
{
    CCZOE_LOG_DEBUG(CCZOE_LOG_ROOT()) << "hello world " << thread::Thread::GetName();
}

int main()
{
    fiber::Fiber::GetThis();
    Scheduler sc("test", 1);
    for (int i = 0; i < 5; i++)
    {
        std::shared_ptr<fiber::Fiber> f = std::make_shared<fiber::Fiber>(std::bind(test_fiber, i));
        sc.schedule(f);
    }

    sc.start();

    // for (int i = 0; i < 100; i++)
    // {
    //     std::shared_ptr<fiber::Fiber> f = std::make_shared<fiber::Fiber>(std::bind(test_fiber, i + 100000));
    //     sc.schedule(f);
    // }

    // sc.stop();

    return 0;
}