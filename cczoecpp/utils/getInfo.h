#ifndef __CCZOE_GETINFO_H__
#define __CCZOE_GETINFO_H__

#include <sys/time.h>
#include <pthread.h>
#include <stdint.h>
#include <unistd.h>

namespace cczoe {
    
pid_t getThreadId();
uint32_t getFiberId();
uint64_t getCurrentMs();
uint64_t getCurrentUs();

}

#endif