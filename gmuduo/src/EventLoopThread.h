#ifndef GMUDUO_SRC_EVENTLOOPTHREAD_H
#define GMUDUO_SRC_EVENTLOOPTHREAD_H

#include <thread>
#include <mutex>
#include <condition_variable>
#include <memory>

namespace gmuduo
{
    class EventLoop;
    class EventLoopThread
    {
    private:
        std::mutex mutex_;
        std::condition_variable cond_;
        std::thread thread_;
        std::shared_ptr<EventLoop> loop_;
        void threadFunc();

    public:
        EventLoopThread();
        ~EventLoopThread();
        EventLoop* getLoop();
        void stop();

    };
    
    
    
} // namespace gmuduo



#endif