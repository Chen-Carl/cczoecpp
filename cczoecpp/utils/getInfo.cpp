#include <execinfo.h>
#include "getInfo.h"
#include "Fiber/fiber.h"

namespace cczoe {
    
pid_t getThreadId()
{
    return getpid();
}

uint32_t getFiberId()
{
    return fiber::Fiber::getThis()->getId();
}

}

