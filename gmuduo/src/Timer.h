#ifndef GMUDUO_SRC_TIMER_H
#define GMUDUO_SRC_TIMER_H

#include <functional>
#include "Timestamp.h"

namespace gmuduo
{
    //  一个定时器类，主要注册到期时间以及对应的处理函数
    class Timer{
    private:
        std::function<void(void)> callback_;  // 到期处理函数
        Timestamp    timeout_;                 // 到期时间(ms)，从1970起
        uint64_t    interval_;                // 间隔周期。0为不重复
    public:
        Timer(std::function<void(void)> callback, Timestamp timeout, uint64_t interval) : 
            callback_(std::move(callback)), timeout_(timeout), interval_(interval){}
        void run() const {callback_();}
        Timestamp expired() const {return timeout_;}
        bool isreapt() const {return interval_ > 0;}
        void reset() {timeout_ += interval_;}
    };


} // namespace gmuduo



#endif