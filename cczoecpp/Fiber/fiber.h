#ifndef __CCZOE_FIBER_H__
#define __CCZOE_FIBER_H__

#include <ucontext.h>
#include <memory>
#include <functional>
#include <atomic>
#include "Thread/thread.h"
#include "Config/config.h"
#include "Allocator/allocator.h"
#include "utils/macro.h"

namespace cczoe {
namespace fiber {

class Fiber : public std::enable_shared_from_this<Fiber>
{
public:
    // fiber states
    enum State
    {
        INIT,       // initial state
        RUNNING,    // running state
        TERM,       // terminal state
        READY       // ready state
    };

    /**
     * @brief the constructor of sub fibers
     * @param cb            start point
     * @param stacksize     stack size
     */
    Fiber(std::function<void()> cb, size_t stacksize = 0);
    ~Fiber();

private:
    /**
     * @brief Main fiber constructor. Main fiber does not have executive stack.
     * @param cb            start point
     * @param stacksize     stack size
     */
    Fiber();
    // set the running fiber
    static void setThis(Fiber *fiber);

    uint64_t m_id = 0;              // fiber id
    uint32_t m_stacksize = 0;       // stack size
    State m_state = INIT;           // fiber state
    /** 
     * m_ucp contains uc_link, uc_stack, uc_mcontext, which is private to each fiber, therefore private members should save these information and cover the old value fetched by getcontext()
     */
    ucontext_t m_ucp;               // fiber context
    void *m_stack = nullptr;        // stack pointer
    std::function<void()> m_cb;     // start point

public:
    /**
     * @brief Reset this fiber. Reuse the resources, avoiding creating another fiber. This method should only be applied to init fibers and terminal fibers.
     * @param callback      reset function 
     */
    void reset(std::function<void()> callback);
    uint32_t getId() const;

    // atom operations of Fiber
    void yield();
    void resume();

    void setIdle();

    State getState() const { return m_state; }

    /**
     * @brief  
     */
    // get the running fiber
    static std::shared_ptr<Fiber> GetThis();
    // the total number of fibers
    static uint64_t totalFibers();

    static void YieldToReady();

    static void mainFunc();
    static void initMainFiber();
};

}
}

#endif