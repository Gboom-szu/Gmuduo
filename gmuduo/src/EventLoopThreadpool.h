#ifndef GMUDUO_SRC_EVENTLOOPTHREADPOOL_H
#define GMUDUO_SRC_EVENTLOOPTHREADPOOL_H

#include <thread>
#include <vector>
#include "EventLoopThread.h"

namespace gmuduo
{
    class EventLoop;
    class EventLoopThreadpool
    {
    private:
        int numThreads_;
        int loopIndex_;
        bool started_;
        EventLoop* baseLoop_;
        std::vector<std::shared_ptr<EventLoopThread>> threads_;
        std::vector<EventLoop*>     loops_;
    public:
        EventLoopThreadpool(EventLoop* baseLoop, int numThreads = 0);
        void start();
        void stop();
        void setThreadNum(int num);
        EventLoop* getOneLoop();
    };
} // namespace gmuduo

#endif