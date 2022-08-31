#ifndef GMUDUO_SRC_TIMERQUEUE_H
#define GMUDUO_SRC_TIMERQUEUE_H

#include <functional>
#include <set>
#include <memory>
#include <vector>
#include <gmuduo/com/Timestamp.h>
#include "Channel.h"
#include "Timer.h"
#include "TimerID.h"


namespace gmuduo
{
    class EventLoop;


    // TimerQueue无线程安全问题，因为所有操作都必须在io线程中，EventLoop会提供保证.
    // TimerQueue作为内部实现类，不会向用户暴露接口，因此所有成员都是私有的，即所有操作只能通过EventLoop
    class TimerQueue{
    private:
        using Entry = std::pair<Timestamp, std::shared_ptr<Timer>>;
        
        EventLoop* eventLoop_;      // 所属的eventloop
        int timerfd_;       // timerfd文件描述符
        Channel timerChannel_;      // 与timerfd_相关的channel
        std::set<Entry> timerSet_;
        std::set<std::shared_ptr<Timer>> cancellingTimers_;
        bool isHandling;      // 是否正在处理timer超时。由于无线程安全问题，无需原子变量


    private:
        TimerQueue(EventLoop*);
        void resetTimerfd(const Timestamp&);   // 重新设置timerfd到期时间
        TimerID addTimer(std::function<void(void)> callback, Timestamp timeout, uint64_t interval);     // 新建Timer
        void cancelTimer(const TimerID&);    // 取消一个Timer
        std::vector<Entry> getExpired(const Timestamp& time);   // 清除并返回到期的timer
        void handleRead(Timestamp time);    // timerfd到期时，处理timer
        void reset(const std::vector<Entry>& expired);      // 将周期timer重新加回去，并重新设置timerfd

        friend class EventLoop;     // 只能通过EventLoop访问TimerQueue
    };
    
} // namespace gmuduo


#endif