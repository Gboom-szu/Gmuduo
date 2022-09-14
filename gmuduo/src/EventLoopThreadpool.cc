#include "EventLoopThreadpool.h"
#include "EventLoop.h"

namespace gmuduo
{
    EventLoopThreadpool::EventLoopThreadpool(EventLoop* baseloop, int numthreads):
        numThreads_(numthreads),
        loopIndex_(0),
        started_(false),
        baseLoop_(baseloop)

    {
        loops_.emplace_back(baseLoop_);
    }

    void EventLoopThreadpool::start() {
        baseLoop_->assertInLoopThread();
        if(started_) return;
        started_ = true;
        for(int i = 0 ; i < numThreads_; i++) {
            auto thread = std::make_shared<EventLoopThread>();
            threads_.emplace_back(thread);
            loops_.emplace_back(thread->getLoop());
        }
    }
    void EventLoopThreadpool::stop() {
        for(auto& eventThread: threads_)
            eventThread->stop();
    }
    void EventLoopThreadpool::setThreadNum(int num) {
        baseLoop_->assertInLoopThread();
        numThreads_  = num;
    }

    EventLoop* EventLoopThreadpool::getOneLoop() {
        baseLoop_->assertInLoopThread();
        if(loopIndex_ == loops_.size())
            loopIndex_ = 0;
        return loops_[loopIndex_++];
    }
    
} // namespace gmuduo
