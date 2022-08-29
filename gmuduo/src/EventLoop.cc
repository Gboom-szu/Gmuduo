#include <iostream>
#include <poll.h>
#include <assert.h>
#include "EventLoop.h"
#include "Epoll.h"
#include "Channel.h"
#include "Log.h"


namespace gmuduo{
    __thread EventLoop* t_eventLoop = nullptr;


    EventLoop::EventLoop(): poller_(std::make_unique<Epoll>()) {
        if(t_eventLoop == nullptr) {
            t_eventLoop = this;
        } else {
            LOG_FATAL("当前线程已经创建了一个eventloop");
        }
    }

    EventLoop::~EventLoop() {
        t_eventLoop = nullptr;
    }

    void EventLoop::loop() {
        assertInLoopThread();   // loop必须在loop线程中
        quited_ = false;

        while (!quited_)
        {
            activeChannels_.clear();
            auto time = poller_->poll(5000, activeChannels_);
            for(auto channel : activeChannels_) {
                channel->handleEvent(time);
            }
            break;
        }
    }
    void EventLoop::assertInLoopThread() {
        assert(t_eventLoop == this);
        // if(!isInLoopThread()) {
        //     std::cout << "not in loop thread" << std::endl;
        //     exit(-1);
        // }
    }
    
    bool EventLoop::isInLoopThread() {
        return t_eventLoop == this;
    }
    void EventLoop::updateChannel(Channel * channel) { poller_->updateChannel(channel); }


}