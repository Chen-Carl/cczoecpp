#include "mutex.h"

namespace cczoe {
namespace thread {

Mutex::Mutex()
{
    pthread_mutex_init(&m_mutex, nullptr);
}

Mutex::~Mutex()
{
    pthread_mutex_destroy(&m_mutex);
}

void Mutex::lock()
{
    pthread_mutex_lock(&m_mutex);
}

void Mutex::unlock()
{
    pthread_mutex_unlock(&m_mutex);
}

RWMutex::RWMutex()
{
    pthread_rwlock_init(&m_mutex, nullptr);
}

RWMutex::~RWMutex()
{
    pthread_rwlock_destroy(&m_mutex);
}

void RWMutex::rdlock()
{
    pthread_rwlock_rdlock(&m_mutex);
}

void RWMutex::wrlock()
{
    pthread_rwlock_wrlock(&m_mutex);
}

void RWMutex::unlock()
{
    pthread_rwlock_unlock(&m_mutex);
}

SpinLock::SpinLock()
{
    pthread_spin_init(&m_mutex, 0);
}

SpinLock::~SpinLock()
{
    pthread_spin_destroy(&m_mutex);
}

void SpinLock::lock()
{
    pthread_spin_lock(&m_mutex);
}

void SpinLock::unlock()
{
    pthread_spin_unlock(&m_mutex);
}

}}