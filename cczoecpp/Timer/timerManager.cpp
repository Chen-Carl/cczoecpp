#include <limits>
#include "Timer/timerManager.h"
#include "utils/getInfo.h"

namespace cczoe {

// TODO: tickle optimization
// TODO: time rollback

TimerManager::TimerManager()
{

}

TimerManager::~TimerManager()
{

}

void TimerManager::addTimer(std::shared_ptr<Timer> timer, RWMutexType &mutex)
{
    mutex.wrlock();
    auto it = m_timers.insert(timer);
    mutex.unlock();
    if (it.second && it.first == m_timers.begin())
    {
        onTimerInsertedAtFront();
    }
}

std::shared_ptr<Timer> TimerManager::addTimer(uint64_t ms, std::function<void()> cb, bool recurring)
{
    // TODO: make_shared cannot access private constructor
    // std::shared_ptr<Timer> timer = std::make_shared<Timer>(ms, cb, recurring, this);
    std::shared_ptr<Timer> timer = std::shared_ptr<Timer>(new Timer(ms, cb, recurring, this));
    addTimer(timer, m_mutex);
    return timer;
}

std::shared_ptr<Timer> TimerManager::addTimer(uint64_t ms, std::function<void()> cb, std::weak_ptr<void> cond, bool recurring)
{
    return addTimer(ms, [&]() {
        std::shared_ptr<void> tmp = cond.lock();
        if (tmp)
        {
            cb();
        }
    }, recurring);
}

uint64_t TimerManager::getNextTimer()
{
    thread::ReadScopedLock<RWMutexType> lock(m_mutex);
    if (m_timers.empty())
    {
        return std::numeric_limits<uint64_t>::max();
    }

    const std::shared_ptr<Timer> &next = *m_timers.begin();
    uint64_t nowMs = getCurrentMs();
    return (nowMs >= next->m_next) ? 0 : next->m_next - nowMs;
}

std::vector<std::function<void()>> TimerManager::getExpiredCb()
{
    uint64_t now = getCurrentMs();
    {
        thread::ReadScopedLock<RWMutexType> lock(m_mutex);
        if (m_timers.empty())
        {
            return {};
        }
    }
    thread::WriteScopedLock<RWMutexType> lock(m_mutex);
    // TODO: cannot use std::make_shared
    auto it = m_timers.lower_bound(std::shared_ptr<Timer>(new Timer(now)));
    while (it != m_timers.end() && (*it)->m_next == now)
    {
        it++;
    }
    std::vector<std::shared_ptr<Timer>> expired(m_timers.begin(), it);
    m_timers.erase(m_timers.begin(), it);
    std::vector<std::function<void()>> res;
    for (auto &timer : expired)
    {
        res.push_back(timer->m_cb);
        if (timer->m_recurring)
        {
            timer->m_next = now + timer->m_ms;
            m_timers.insert(timer);
        }
        else
        {
            timer->m_cb = nullptr;
        }
    }
    return res;
}

bool TimerManager::empty()
{
    thread::ReadScopedLock<RWMutexType> lock(m_mutex);
    return m_timers.empty();
}


}