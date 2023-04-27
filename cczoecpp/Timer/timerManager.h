#ifndef __CCZOE_TIMER_TIMERMANAGER_H__
#define __CCZOE_TIMER_TIMERMANAGER_H__

#include <memory>
#include <functional>
#include <set>
#include "Timer/timer.h"
#include "Thread/Mutex/mutex.h"

namespace cczoe {

class TimerManager
{
    friend class Timer;
public:
    using RWMutexType = thread::RWMutex;
    
private:
    RWMutexType m_mutex;
    std::set<std::shared_ptr<Timer>, Timer::Comparator> m_timers;

    void addTimer(std::shared_ptr<Timer> timer, RWMutexType &lock);

public:
    TimerManager();
    virtual ~TimerManager();

    std::shared_ptr<Timer> addTimer(uint64_t ms, std::function<void()> cb, bool recurring = false);

    std::shared_ptr<Timer> addTimer(uint64_t ms, std::function<void()> cb, std::weak_ptr<void> cond, bool recurring = false);

    uint64_t getNextTimer();
    std::vector<std::function<void()>> getExpiredCb();

    bool empty();

protected:
    virtual void onTimerInsertedAtFront() = 0;
};

}

#endif