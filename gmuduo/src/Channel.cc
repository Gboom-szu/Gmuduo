#include <sys/epoll.h>
#include "Channel.h"
#include "Log.h"
#include "EventLoop.h"

namespace gmuduo{
    // 定义事件
    const uint32_t Channel::kNoneEvent = 0;
    const uint32_t Channel::kReadEvent = EPOLLIN;
    const uint32_t Channel::kWriteEvent = EPOLLOUT;

    void Channel::enableRead() {events_ |= kReadEvent; update();}
    void Channel::disableRead() {events_ &= ~kReadEvent; update();}
    void Channel::enableWrite() {events_ |= kWriteEvent; update();}
    void Channel::disableWrite() {events_ &= ~kWriteEvent; update();}
    void Channel::disableAll() {events_ = kNoneEvent;}
    void Channel::update() {eventloop_->updateChannel(this);}

    void Channel::handleEvent(uint64_t receiveTime) {
        if(revents_ & kReadEvent) {
            LOG_INFO("读事件");
            readcallback_(receiveTime);
        }
        if(revents_ & kWriteEvent) {
            LOG_INFO("写事件");
            writecallback_();
        }
        if(revents_ & EPOLLRDHUP) {
            LOG_INFO("连接关闭");
            closecallback_();
        }
    }
}