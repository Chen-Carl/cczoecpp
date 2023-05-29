#ifndef __CCZOE_THREAD_H__
#define __CCZOE_THREAD_H__

#include <pthread.h>
#include <thread>
#include <functional>
#include <string>
#include <stdexcept>
#include "Semaphore/sem.h"
// #include "Log/log.h"

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
    std::string m_name;
    std::function<void()> m_callback;
    Semaphore m_semaphore;

public:
    template <typename F, typename... Args>
    Thread(const std::string &name, F &&f, Args &&... args):
        m_name(name),
        m_callback(std::bind(std::forward<F>(f), std::forward<Args>(args)...))
    {
        if (name.empty())
        {
            m_name = "default name";
        }
        // when calling constructor, create a new thread
        // callback() invoked in run()
        int rt = pthread_create(&m_thread, nullptr, &Thread::run, this);
        if (rt)
        {
            // CCZOE_LOG_ERROR(CCZOE_LOG_NAME("system")) << "pthread_create fail, rt = " << rt << ", name = " << m_name;
            throw std::logic_error("pthread_create error, name = " + m_name);
        }
    }
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