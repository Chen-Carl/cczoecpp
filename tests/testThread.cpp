#include <iostream>
#include <vector>
#include <memory>
#include "Thread/Thread.h"
#include "Log/Log.h"

using namespace cczoe;

int count = 0;

void testThread();
void func1();

int main()
{
    testThread();
    return 0;
}

void testThread()
{
    CCZOE_LOG_INFO(CCZOE_LOG_ROOT()) << "thread test begin";
    std::vector<std::shared_ptr<thread::Thread>> thrs;
    for(int i = 0; i < 5; i++) 
    {
        std::shared_ptr<thread::Thread> thr(new thread::Thread("name_" + std::to_string(i), func1));
        thrs.push_back(thr);
    }

    for(size_t i = 0; i < thrs.size(); ++i) 
    {
        thrs[i]->join();
    }
    CCZOE_LOG_INFO(CCZOE_LOG_ROOT()) << "thread test end";
    CCZOE_LOG_INFO(CCZOE_LOG_ROOT()) << "count = " << count;
}

void func1()
{
    CCZOE_LOG_INFO(CCZOE_LOG_ROOT()) << "name: " << thread::Thread::GetName()
                                     << " this.name: " << thread::Thread::GetThis()->getName()
                                     << " id: " << getThreadId()
                                     << " this.id: " << thread::Thread::GetThis()->getId();

    for(int i = 0; i < 10000; ++i) 
    {
        ++count;
    }
}
