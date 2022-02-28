#include "Log/log.h"
#include "Scheduler/scheduler.h"

using namespace cczoe;

void test_fiber(int i)
{
    CCZOE_LOG_DEBUG(CCZOE_LOG_ROOT()) << "hello world " << thread::Thread::GetName();
}

int main()
{
    fiber::Fiber::getThis();
    Scheduler sc("test", 5);
    for (int i = 0; i < 10; i++)
    {
        std::shared_ptr<fiber::Fiber> f(new fiber::Fiber(std::bind(test_fiber, i)));
        sc.schedule(f);
    }
    sc.start();

    for (int i = 0; i < 10; i++)
    {
        std::shared_ptr<fiber::Fiber> f(new fiber::Fiber(std::bind(test_fiber, i + 10)));
        sc.schedule(f);
    }

    sc.stop();

    return 0;
}