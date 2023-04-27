#ifndef __CCZOE_SEMAPHORE_H__
#define __CCZOE_SEMAPHORE_H__

#include <pthread.h>
#include <semaphore.h>
#include <stdint.h>

namespace cczoe {
namespace thread {

class Semaphore
{
private:
    Semaphore(const Semaphore &) = delete;
    Semaphore(const Semaphore &&) = delete;
    Semaphore &operator=(const Semaphore &) = delete;

    sem_t m_semaphore;

public:
    Semaphore(uint32_t count = 0);
    void wait();
    void notify();
};

}
}

#endif