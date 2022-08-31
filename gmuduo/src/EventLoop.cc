#include <iostream>
#include <poll.h>
#include <assert.h>
#include <atomic>
#include <gmuduo/com/Timestamp.h>
#include "EventLoop.h"
#include "Epoll.h"
#include "Channel.h"
#include "Log.h"
#include "TimerQueue.h"
#include <string>
#include <sys/eventfd.h>
#include <unistd.h>


namespace gmuduo{
    __thread EventLoop* t_eventLoop = nullptr;      // 线程局部变量

    // 创建一个eventfd文件描述符
    static int createEventfd() {
        int fd = eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
        if(fd < 0) {
            LOG_FATAL("createEventfd failed");
        }
        return fd;
    }

    EventLoop::EventLoop(): 
            poller_(std::make_unique<Epoll>()),
            timerQueue_(new TimerQueue(this)),
            efd(createEventfd()),
            wakeupChannel_(std::make_unique<Channel>(this, efd))       
    {
        if(t_eventLoop == nullptr) {
            t_eventLoop = this;
        } else {
            LOG_FATAL("当前线程已经创建了一个eventloop");
        }
        
        wakeupChannel_->setReadCallback(std::bind(&EventLoop::handleEfdRead, this, std::placeholders::_1));
        wakeupChannel_->enableRead();
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
            auto time = poller_->poll(pollTimeMS_, activeChannels_);
            for(auto channel : activeChannels_) {
                channel->handleEvent(time);
            }
            doPendingTask();
        }
    }
    void EventLoop::assertInLoopThread() const {
        assert(t_eventLoop == this);
        // if(!isInLoopThread()) {
        //     std::cout << "not in loop thread" << std::endl;
        //     exit(-1);
        // }
    }
    
    bool EventLoop::isInLoopThread() const {
        return t_eventLoop == this;
    }
    void EventLoop::updateChannel(Channel * channel) { poller_->updateChannel(channel); }

        // 定时器
    TimerID EventLoop::runAt(const Timestamp& when, Functor calllbackk) {  // 在固定(无保证)时间执行
        return timerQueue_->addTimer(std::move(calllbackk), when, 0);
    }
    TimerID EventLoop::runEvery(uint64_t interval, Functor callback) {       // 间隔执行
        Timestamp now;
        now += interval;
        return timerQueue_->addTimer(std::move(callback), now, interval);
    }      
    TimerID EventLoop::runAfter(uint64_t delay, Functor callback) {       // 相对延迟执行
        Timestamp now;
        now += delay;
        return runAt(now, std::move(callback));
    }
    // 取消一个timer
    void EventLoop::cancelTimer(const TimerID& timerid) {
        if(isInLoopThread()) {
            // 在io线程中，可直接删除
            timerQueue_->cancelTimer(timerid);
        } else {    // 否则要转移到io线程中。
            queueInLoop([timerid, this](){ cancelTimer(timerid);});
        }
    }

    void EventLoop::handleEfdRead(Timestamp time) {
        uint64_t one = 1;
        ssize_t n = read(efd, &one, sizeof one);
        if (n != sizeof one)
        {
            LOG_FATAL("handleEfdRead failed");
        }        
    }

    void EventLoop::wakeup() {
        uint64_t one = 1;
        auto n = write(efd, &one, sizeof one);
        if (n != sizeof one)
        {
            LOG_FATAL("wakeup failed");
        }
    }

    void EventLoop::doPendingTask() {
        std::vector<Functor> tasks;
        callpending_ = true;
        {
            std::lock_guard<std::mutex> _(mutex_);
            std::swap(tasks, penddingTask_);
        }      
        for(auto & task : tasks) {
            task();
        } 
        callpending_ = false;
    }
    // 线程安全
    void EventLoop::queueInLoop(Functor callback) {
        {
            std::lock_guard<std::mutex> _(mutex_);
            penddingTask_.emplace_back(std::move(callback));
        }
        if(!isInLoopThread() || callpending_) {
            wakeup();
        }
    }
    // 线程安全
    void EventLoop::runInLoop(Functor callback) {
        if(isInLoopThread()) {  // 在io线程中,直接运行
            callback();
        } else {
            queueInLoop(std::move(callback));
        }
    }

    void EventLoop::quit() {
        quited_ = true;
        if(!isInLoopThread()) {
            wakeup();
        }
    }
}