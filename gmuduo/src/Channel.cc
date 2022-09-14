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
    void Channel::disableAll() {events_ = kNoneEvent; update();}
    void Channel::update() {eventloop_->updateChannel(this);}
    void Channel::remove(){
        eventloop_->removeChannel(this);
    }
    void Channel::handleEvent(Timestamp receiveTime) {
        // ref : https://blog.csdn.net/halfclear/article/details/78061771
        if(revents_ & EPOLLERR) {
            if(errorcallback_) errorcallback_();
        }

        if(revents_ & EPOLLIN) {
            if(readcallback_) readcallback_(receiveTime);
        }

        if(revents_ & EPOLLHUP) {  // 对端关闭了读写，本端继续发数据而收到RST。这时候就不应该再发数据了。 EPOLLIN+EPOLLRDHUP+EPOLLHUP+EPOLLERR
            if(closecallback_) closecallback_();
        } 

        if(revents_ & EPOLLOUT) {
            if(writecallback_) writecallback_();
        }

    }
}