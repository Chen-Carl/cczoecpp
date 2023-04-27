#ifndef __CCZOE_TIMER_TIMER_H__
#define __CCZOE_TIMER_TIMER_H__

#include <functional>
#include <memory>

namespace cczoe {

class TimerManager;

class Timer : public std::enable_shared_from_this<Timer>
{
    friend class TimerManager;
private:
    Timer(uint64_t next = 0);
    Timer(uint64_t ms, std::function<void()> cb, bool recurring, TimerManager *manager);

    bool m_recurring = false;
    uint64_t m_ms = 0;          // time interval
    uint64_t m_next = 0;        // absolute next time to trigger
    TimerManager *m_manager = nullptr;
    std::function<void()> m_cb;

    struct Comparator
    {
        bool operator()(const std::shared_ptr<Timer> &lhs, const std::shared_ptr<Timer> &rhs) const;
    };

public:
    bool cancel();
    /**
     * @brief reset timer
     * @param ms time interval
     * @param from_now if true, reset from now, else reset from last trigger time 
     */
    bool reset(uint64_t ms, bool from_now = false);
    bool refresh();
};

}

#endif