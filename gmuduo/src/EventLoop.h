#ifndef GMUDUO_SRC_EVENTLOOP_H
#define GMUDUO_SRC_EVENTLOOP_H

#include <memory>
#include <vector>
#include <mutex>
#include <functional>
#include <atomic>
#include <gmuduo/com/Timestamp.h>
#include "TimerID.h"


namespace gmuduo{

    class Channel;
    class Epoll;
    class TimerQueue;
    // one loop per thread类，即一个线程只能创建一个对象，复杂poll
    // EventLoop作为接口类，应该每一个pubilic函数都是线程安全的。
    class EventLoop
    {
        using Functor = std::function<void(void)>;
    private:
        std::atomic_bool quited_{false};            // 是否要退出loop
        int pollTimeMS_ = 5000;   // poll超时时间毫秒
        std::unique_ptr<Epoll> poller_;
        std::vector<Channel*> activeChannels_;      // 可操作的channel列表
        std::unique_ptr<TimerQueue> timerQueue_;    // 定时器
        
        mutable std::mutex mutex_;                  // 保护penddingTask_的锁

        int efd;
        std::unique_ptr<Channel> wakeupChannel_;
        std::vector<Functor> penddingTask_;         // 转移到io线程上执行的任务，会在poll之后执行
        std::atomic_bool callpending_{false};       // 是否正在处理penddingTask_。

        void assertInLoopThread() const;
        void doPendingTask();                       // 执行queuInLoop提交的任务
        void wakeup();
        void handleEfdRead(Timestamp);
    public:
        EventLoop();
        ~EventLoop();
        void loop();    // 开始poll
        bool isInLoopThread() const;
        
        void updateChannel(Channel*);   // 更新Channel监听状态
        void setPollTime(int timeout) {pollTimeMS_ = timeout;}
        void quit();        // 结束loop

        // 向io线程提交任务
        
        // 线程安全
        void runInLoop(Functor callback);
        void queueInLoop(Functor callabck);

        // 定时器
        TimerID runAt(const Timestamp& when, Functor calllbackk);   // 在固定(无保证)时间执行
        TimerID runEvery(uint64_t interval, Functor callback) ;     // 间隔执行
        TimerID runAfter(uint64_t delay, Functor callback);         // 延迟执行
        void cancelTimer(const TimerID& tiemrid);                   // 取消timer
        
    };
    


}



#endif