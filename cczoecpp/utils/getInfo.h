#ifndef __ZOE_GETINFO_H__
#define __ZOE_GETINFO_H__

#include <pthread.h>
#include <stdint.h>
#include <unistd.h>

namespace cczoe {
    
pid_t getThreadId();
uint32_t getFiberId();

}

#endif