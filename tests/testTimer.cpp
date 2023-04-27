#include <iostream>
#include "Scheduler/iomanager.h"

using namespace cczoe;

void testTimerManager();

int main()
{
    testTimerManager();
    return 0;
}

void testTimerManager()
{
    IOManager iom("test_timer", 100);
    std::shared_ptr<Timer> timer = iom.addTimer(3000, [&timer]() { 
        static int i = 0;
        CCZOE_LOG_INFO(CCZOE_LOG_ROOT()) << "recurring = " << i++; 
        if (i == 6)
        {
            timer->cancel();
        }
    }, true);
    iom.addTimer(3000, []() { CCZOE_LOG_INFO(CCZOE_LOG_ROOT()) << "time = 3000"; }, false);
    iom.addTimer(3000, []() { CCZOE_LOG_INFO(CCZOE_LOG_ROOT()) << "time = 3000"; }, false);
    iom.addTimer(3000, []() { CCZOE_LOG_INFO(CCZOE_LOG_ROOT()) << "time = 3000"; }, false);
    iom.addTimer(10000, []() { CCZOE_LOG_INFO(CCZOE_LOG_ROOT()) << "time = 10000"; }, false);
    iom.addTimer(20000, []() { CCZOE_LOG_INFO(CCZOE_LOG_ROOT()) << "time = 20000"; }, false);

    iom.stop();
}