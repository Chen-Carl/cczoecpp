#include <vector>
#include <iostream>
#include "Thread/thread.h"
#include "Log/log.h"
#include "Fiber/fiber.h"

using namespace cczoe;

void func();
void testFiber();
void fiberFunc();

int main()
{
    std::cout << "------ test fiber ------" << std::endl;
    thread::Thread::SetName("main");
    testFiber();
    return 0;
}

void testFiber()
{
    std::vector<std::shared_ptr<thread::Thread>> thrs;

    for (int i = 0; i < 2; i++)
    {
        thrs.push_back(std::make_shared<thread::Thread>("name_" + std::to_string(i), func));
    }
    for (auto &x : thrs)
    {
        x->join();
    }
}

void fiberFunc()
{
    CCZOE_LOG_INFO(CCZOE_LOG_ROOT()) << "subfiber function before yield";
    fiber::Fiber::YieldToReady();
    CCZOE_LOG_INFO(CCZOE_LOG_ROOT()) << "subfiber function after yield";
    fiber::Fiber::YieldToReady();
    CCZOE_LOG_INFO(CCZOE_LOG_ROOT()) << "subfiber function end";
}

void func()
{
    // Outputing logs invokes the default fiber constructor.
    // That's because logs contain the Fiber id, where getFiberId() is applied.
    CCZOE_LOG_INFO(CCZOE_LOG_ROOT()) << "fiber function begin";
    {
        std::shared_ptr<fiber::Fiber> fiber = std::make_shared<fiber::Fiber>(fiberFunc);
        CCZOE_LOG_INFO(CCZOE_LOG_ROOT()) << "main thread fiber output 1";
        fiber->resume();
        CCZOE_LOG_INFO(CCZOE_LOG_ROOT()) << "main thread fiber output 2";
        fiber->resume();
        CCZOE_LOG_INFO(CCZOE_LOG_ROOT()) << "main thread fiber output 3";
        fiber->resume();
    }   
    CCZOE_LOG_INFO(CCZOE_LOG_ROOT()) << "fiber function end";
}

