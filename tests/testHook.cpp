#include <chrono>
#include "Net/Hook/hook.h"
#include "Scheduler/iomanager.h"
#include "utils/timeCounter.hpp"
#include "Log/log.h"

using namespace cczoe;

void testSleep();

int main()
{
    testSleep();
    return 0;
}

void testSleep()
{
    IOManager iom("testHook", 1);
    auto sleepFor = [](int seconds) {
        sleep(seconds);
        CCZOE_LOG_INFO(CCZOE_LOG_ROOT()) << "sleep " << seconds << "s";
    };
    iom.schedule(std::bind(sleepFor, 5));
    iom.schedule(std::bind(sleepFor, 10));
}