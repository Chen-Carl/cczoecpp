#include "fiber.h"

namespace cczoe {
namespace fiber {

// the next fiber id
static std::atomic<uint64_t> s_fiber_id {0};
// the number of fibers
static std::atomic<uint64_t> s_fiber_count {0};
// record current running fiber
static thread_local Fiber *t_fiber = nullptr;
// record main fiber
static thread_local std::shared_ptr<Fiber> t_threadFiber = nullptr;

// fiber stack config, allocate 1M space
static std::shared_ptr<config::ConfigVar<uint32_t>> g_fiber_stack_size(config::Config::lookup("fiber.stack_size", (uint32_t)(1024 * 1024), "fiber stack size"));

void Fiber::setThis(Fiber *fiber)
{
    t_fiber = fiber;
}

std::shared_ptr<Fiber> Fiber::getThis()
{
    if (t_fiber)
    {
        return t_fiber->shared_from_this();
    }
    // create a main fiber
    std::shared_ptr<Fiber> main_fiber(new Fiber);
    CCZOE_ASSERT(t_fiber == main_fiber.get());
    t_threadFiber = main_fiber;
    return t_fiber->shared_from_this();
}

uint64_t Fiber::totalFibers()
{
    return s_fiber_count;
}

Fiber::Fiber()
{
    setThis(this);
    m_state = RUNNING;
    if (getcontext(&m_ucp))
    {
        CCZOE_ASSERT2(false, "getcontext");
    }
    s_fiber_count++;
    m_id = s_fiber_id++;
}

Fiber::Fiber(std::function<void()> cb, size_t stacksize) :
    m_id(s_fiber_id++),
    m_cb(cb)
{
    m_stacksize = stacksize > 0 ? stacksize : g_fiber_stack_size->getValue();
    m_stack = StackAllocator::Alloc(m_stacksize);
    m_state = INIT;
    // save the context restore it when switching to the other fiber
    if (getcontext(&m_ucp))
    {
        CCZOE_ASSERT2(false, "getcontext");
    }
    s_fiber_count++;
    m_ucp.uc_link = nullptr;
    m_ucp.uc_stack.ss_sp = m_stack;
    m_ucp.uc_stack.ss_size = m_stacksize;
    makecontext(&m_ucp, &Fiber::mainFunc, 0);
}

Fiber::~Fiber()
{
    s_fiber_count--;
    // subfiber has a stack
    if (m_stack)
    {
        CCZOE_ASSERT(m_state == INIT || m_state == TERM);
        StackAllocator::Free(m_stack);
    }
    // deconstruct the main fiber
    else
    {
        if (t_fiber == this)
        {
            setThis(nullptr);
        }
    }
}

void Fiber::reset(std::function<void()> callback)
{
    CCZOE_ASSERT(m_state == INIT || m_state == TERM);
    CCZOE_ASSERT(m_stack);
    m_cb = callback;
    m_state = INIT;
    if (getcontext(&m_ucp))
    {
        CCZOE_ASSERT2(false, "getcontext");
    }
    m_ucp.uc_link = nullptr;
    m_ucp.uc_stack.ss_sp = m_stack;
    m_ucp.uc_stack.ss_size = m_stacksize;
    // modify the start point
    makecontext(&m_ucp, &Fiber::mainFunc, 0);
}

uint32_t Fiber::getId() const
{
    return m_id;
}

void Fiber::yield()
{
    if (t_fiber != t_threadFiber.get())
    {
        CCZOE_ASSERT(m_state == RUNNING || m_state == TERM);
        if (m_state == RUNNING)
        {
            m_state = READY;
        }
        setThis(t_threadFiber.get());
        if (swapcontext(&m_ucp, &t_threadFiber->m_ucp))
        {
            CCZOE_ASSERT2(false, "swapcontext");
        }
    }
}

void Fiber::resume()
{
    if (t_fiber == t_threadFiber.get())
    {
        CCZOE_ASSERT(m_state != TERM && m_state != RUNNING);
        setThis(this);
        m_state = RUNNING;
        if (swapcontext(&t_threadFiber->m_ucp, &m_ucp))
        {
            CCZOE_ASSERT2(false, "swapcontext");
        }
    }
}

void Fiber::yieldToReady()
{
    std::shared_ptr<Fiber> curr = getThis();
    curr->yield();
}

void Fiber::initMainFiber()
{
    if (!t_fiber)
    {
        // create a main fiber
        std::shared_ptr<Fiber> main_fiber(new Fiber);
        CCZOE_ASSERT(t_fiber == main_fiber.get());
        t_threadFiber = main_fiber;
    }
}

void Fiber::mainFunc()
{
    std::shared_ptr<Fiber> curr = getThis();
    curr->m_cb();
    curr->m_cb = nullptr;
    curr->m_state = TERM;

    // resume the main fiber, destroy the auto pointer
    auto rawCurr = curr.get();
    // reference number distract 1
    curr.reset();
    rawCurr->yield();
}

void Fiber::setIdle()
{
    m_state = INIT;
}


}
}
