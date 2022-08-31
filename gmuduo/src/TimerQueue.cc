#include <sys/timerfd.h>
#include <unistd.h>
#include <string.h>
#include "Log.h"
#include "TimerQueue.h"
#include "EventLoop.h"
#include "Channel.h"


namespace gmuduo
{
    static int createTimerfd() {
        int res = timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC|TFD_NONBLOCK);
        if(res < 0) {
            LOG_FATAL("timerfd_create failed");
        }
        return res;
    }

    static timespec timeFromNow(const Timestamp& when) {
        auto microseconds = when.microSecondsSinceEpoch() - Timestamp().microSecondsSinceEpoch(); 
        if(microseconds < 100)
            microseconds = 100;
        timespec res;
        res.tv_sec = static_cast<time_t>(microseconds / Timestamp::kMicroSecondsPerSecond);
        res.tv_nsec = static_cast<time_t>((microseconds % Timestamp::kMicroSecondsPerSecond) * 1000);
        return res;
    } 

    TimerQueue::TimerQueue(EventLoop* evetLoop):
            eventLoop_(evetLoop), 
            timerfd_(createTimerfd()), 
            timerChannel_(evetLoop, timerfd_),
            isHandling(false)
    {
        timerChannel_.setReadCallback(std::bind(&TimerQueue::handleRead, this, std::placeholders::_1));
        timerChannel_.enableRead();   // timer应该一直的监听读事件     
    }

    // 新建一个timer，在必要时要重新设置timerfd到期时间
    TimerID TimerQueue::addTimer(std::function<void(void)> callback, Timestamp when, uint64_t interval) {
        auto timer_ptr = std::make_shared<Timer>(callback, when, interval);
        eventLoop_->runInLoop([this, timer_ptr](){
            if(timerSet_.empty() || timerSet_.begin()->first > timer_ptr->expired()) {
                resetTimerfd(timer_ptr->expired());
            }
            timerSet_.insert(Entry(timer_ptr->expired(), timer_ptr));
        });
        return {timer_ptr};
    }

    // 重置timerfd到期时间
    void TimerQueue::resetTimerfd(const Timestamp& when) {
        
        itimerspec newValue;
        memset(&newValue, 0, sizeof(itimerspec));
        newValue.it_value = timeFromNow(when);
        if(timerfd_settime(timerfd_, 0, &newValue, nullptr) < 0 ) {     // 这里使用相对时间，可以控制timer到期间隔，免得太快到期。
            LOG_FATAL("resetTimerfd failed");
        }
    }
    
    // 取得已经到期的timer，并从timerSet中删除
    std::vector<TimerQueue::Entry> TimerQueue::getExpired(const Timestamp& time) {
        std::vector<Entry> expired;
        Entry sentry(time, std::shared_ptr<Timer>());
        auto end = timerSet_.lower_bound(sentry);
        std::copy(timerSet_.begin(), end, back_inserter(expired));
        timerSet_.erase(timerSet_.begin(), end);
        return expired;
    }

    void TimerQueue::handleRead(Timestamp time) {
        uint64_t temp;
        read(timerfd_, &temp, sizeof temp);
        auto expired = getExpired(time);
        isHandling = true;
        for(auto & entry: expired) {
            entry.second->run();
        }
        reset(expired);     // 将周期任务重新添加回去
        isHandling = false;
    }

    void TimerQueue::cancelTimer(const TimerID& timerid) {
        auto timer = timerid.timer_.lock();     // 获取shared_ptr
        if(timer) {
            auto iter = timerSet_.find(Entry(timer->expired(), timer));
            if(iter != timerSet_.end()) {      // eventLoop过了处理channel的阶段，可以直接删除timer
                timerSet_.erase(iter);
            } else {
                cancellingTimers_.insert(timer);
            }
        }
    }

    void TimerQueue::reset(const std::vector<Entry>& expired) {
        for(auto& entry: expired) {
            auto& timer = entry.second;
            // 周期任务没有被取消，重新加回去
            if(timer->isreapt() && cancellingTimers_.find(timer) == cancellingTimers_.end()) {
                timer->reset();
                timerSet_.insert(Entry(timer->expired(), timer));  
            }
        }
        cancellingTimers_.clear();
        // 还有timer的存在，需要重新设置timerfd
        if(!timerSet_.empty()) {
            resetTimerfd(timerSet_.begin()->first);
        }
    }
} // namespace gmuduo
