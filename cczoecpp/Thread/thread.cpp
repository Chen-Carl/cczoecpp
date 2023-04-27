#include "Thread/thread.h"
#include "utils/getInfo.h"
#include "Log/log.h"

namespace cczoe {
namespace thread {

static std::shared_ptr<logcpp::Logger> g_logger = CCZOE_LOG_NAME("system");
static thread_local Thread *t_thread = nullptr;
static thread_local std::string t_thread_name = "default name";

Thread::~Thread()
{
    if (m_thread)
    {
        // waiting for sub thread and recover the sub thread resources
        pthread_detach(m_thread);
    }
}

void Thread::join()
{
    if (m_thread)
    {
        // main thread waits for sub thread
        int rt = pthread_join(m_thread, nullptr);
        if (rt)
        {
            CCZOE_LOG_ERROR(g_logger) << "pthread_join thread fail, name = " << m_name;
            throw std::logic_error("pthread_join error");
        }
        // sub thread exit, reset m_thread
        m_thread = 0;
    }
}

void *Thread::run(void *arg)
{
    // set t_thread to current thread
    Thread *thread = (Thread*)arg;
    t_thread = thread;
    t_thread_name = thread->getName();
    // set private attribute of Thread object
    thread->m_id = getThreadId();
    // set pthread name
    pthread_setname_np(pthread_self(), thread->m_name.substr(0, 15).c_str());

    
    std::function<void()> callback;
    // shared_ptr in callback reference minus 1
    callback.swap(thread->m_callback);
    callback();
    return nullptr;
}


Thread* Thread::GetThis()
{
    return t_thread;
}

const std::string &Thread::GetName()
{
    return t_thread_name;
}

void Thread::SetName(const std::string &name)
{
    if (t_thread)
    {
        t_thread->setName(name);
    }
    t_thread_name = name;
}

}}