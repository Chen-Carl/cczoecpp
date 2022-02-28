#ifndef __ZOE_THREAD_H__
#define __ZOE_THREAD_H__

#include <pthread.h>
#include <thread>
#include <functional>
#include <string>
#include "Semaphore/sem.h"

namespace cczoe {
namespace thread {

class Thread
{
private:
    Thread(const Thread &) = delete;
    Thread(const Thread &&) = delete;
    Thread &operator=(const Thread &) = delete;

    static void *run(void *arg);

    pid_t m_id = -1;            // process id
    pthread_t m_thread = 0;     // thread id
    std::function<void()> m_callback;
    std::string m_name;
    Semaphore m_semaphore;

public:
    Thread(std::function<void()> callback, const std::string &name);
    ~Thread();

    pid_t getId() const { return m_id; }
    const std::string &getName() const { return m_name; }
    void setName(const std::string &name) { m_name = name; }
    void join();

    static Thread* GetThis();
    static const std::string &GetName();
    static void SetName(const std::string &name);
};

}}

#endif