#include "EventLoopThread.h"
#include "EventLoop.h"


namespace gmuduo
{
    EventLoopThread::EventLoopThread(): 
    thread_(std::thread(&EventLoopThread::threadFunc, this))
    {}

    void EventLoopThread::threadFunc() {
        loop_ = std::make_shared<EventLoop>();
        cond_.notify_one();
        loop_->loop();
    }
    void EventLoopThread::stop() {
        loop_->quit();
    }
    EventLoopThread::~EventLoopThread() {
        thread_.join();
    }

    EventLoop* EventLoopThread::getLoop() {
        std::unique_lock<std::mutex> lock(mutex_);
        cond_.wait(lock, [this](){ return bool(loop_); });
        return loop_.get();
    }
    
} // namespace gmuduo
