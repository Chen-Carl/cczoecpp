#include <execinfo.h>
#include "getInfo.h"
#include "Fiber/Fiber.h"

namespace cczoe {
    
pid_t getThreadId()
{
    return gettid();
    // return getpid();
}

uint32_t getFiberId()
{
    return fiber::Fiber::GetThis()->getId();
}

uint64_t getCurrentMs()
{
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    return tv.tv_sec * 1000ul + tv.tv_usec / 1000;
}

uint64_t getCurrentUs()
{
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    return tv.tv_sec * 1000ul * 1000ul + tv.tv_usec;
}

}

