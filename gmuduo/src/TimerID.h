#ifndef GMUDUO_SRC_TIMERID_H
#define GMUDUO_SRC_TIMERID_H

#include <memory>


namespace gmuduo
{
    class Timer;
    class TimerID{
    public:
        TimerID(const TimerID&) = default;
        TimerID(TimerID&&) = default;
    private:
        TimerID(const std::shared_ptr<Timer>& timer): timer_(timer) {}      // 禁止自己创建TimerID
        std::weak_ptr<Timer> timer_;    // timer指针

        friend class TimerQueue;
    };
} // namespace gmuduo




#endif