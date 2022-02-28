#ifndef __ZOE_MUTEX_H__
#define __ZOE_MUTEX_H__

#include <pthread.h>
#include <atomic>

namespace cczoe {
namespace thread {

// mutex lock
class Mutex
{
private:
    pthread_mutex_t m_mutex;
public:
    Mutex();
    ~Mutex();

    void lock();
    void unlock();
};

// read lock and write lock
class RWMutex
{
private:
    pthread_rwlock_t m_mutex;
public:
    RWMutex();
    ~RWMutex();

    void rdlock();
    void wrlock();
    void unlock();
};

// spin lock
class SpinLock
{
private:
    pthread_spinlock_t m_mutex;
public:
    SpinLock();
    ~SpinLock();

    void lock();
    void unlock();
};

// CAS lock
class CASLock
{
private:
    volatile std::atomic_flag m_mutex;
};

// scope lock
template <class T = Mutex>
class ScopedLock
{
private:
    bool m_locked;
    T m_mutex;

    void lock()
    {
        if (!m_locked)
        {
            m_mutex.lock();
            m_locked = true;
        }
    }

    void unlock()
    {
        if (m_locked)
        {
            m_mutex.unlock();
            m_locked = false;
        }
    }

public:
    ScopedLock() = delete;
    ScopedLock(T &mutex) : m_mutex(mutex)
    {
        lock();
        m_locked = true;
    }

    ~ScopedLock()
    {
        unlock();
    }
};

}}

#endif