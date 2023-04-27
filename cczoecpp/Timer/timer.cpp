#include "Timer/timer.h"
#include "Timer/timerManager.h"
#include "utils/getInfo.h"

namespace cczoe {

bool cczoe::Timer::Comparator::operator()(const std::shared_ptr<Timer> &lhs, const std::shared_ptr<Timer> &rhs) const
{
    if (!lhs && !rhs)
    {
        return false;
    }
    if (!lhs)
    {
        return true;
    }
    if (!rhs)
    {
        return false;
    }
    if (lhs->m_next < rhs->m_next)
    {
        return true;
    }
    if (lhs->m_next > rhs->m_next)
    {
        return false;
    }
    return lhs.get() < rhs.get();
}

Timer::Timer(uint64_t ms, std::function<void()> cb, bool recurring, TimerManager *manager) : m_recurring(recurring), m_ms(ms), m_manager(manager), m_cb(cb)
{
    m_next = getCurrentMs() + ms;
}

Timer::Timer(uint64_t now)
{
    m_next = now;
}

bool Timer::cancel()
{
    thread::WriteScopedLock<TimerManager::RWMutexType> lock(m_manager->m_mutex);
    if (m_cb)
    {
        m_cb = nullptr;
        auto it = (m_manager->m_timers).find(shared_from_this());
        m_manager->m_timers.erase(it);
        return true;
    }
    return false;
}

bool Timer::reset(uint64_t ms, bool from_now)
{
    thread::WriteScopedLock<TimerManager::RWMutexType> lock(m_manager->m_mutex);
    if (m_cb)
    {
        auto it = (m_manager->m_timers).find(shared_from_this());
        if (it != m_manager->m_timers.end())
        {
            m_manager->m_timers.erase(it);
            m_ms = ms;
            if (from_now)
            {
                m_next = getCurrentMs() + m_ms;
            }
            m_manager->addTimer(shared_from_this(), m_manager->m_mutex);
            return true;
        }
    }
    return false;
}

bool Timer::refresh()
{
    thread::WriteScopedLock<TimerManager::RWMutexType> lock(m_manager->m_mutex);
    if (m_cb)
    {
        m_cb = nullptr;
        auto it = (m_manager->m_timers).find(shared_from_this());
        if (it != m_manager->m_timers.end())
        {
            m_manager->m_timers.erase(it);
            m_next = getCurrentMs() + m_ms;
            m_manager->m_timers.insert(shared_from_this());
            return true;
        }
    }
    return false;
}

}