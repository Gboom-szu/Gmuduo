#ifndef GMUDUO_SRC_CHANNEL_H
#define GMUDUO_SRC_CHANNEL_H

#include <functional>
#include <sys/epoll.h>
#include "Timestamp.h"

namespace gmuduo{
    class EventLoop;
    class Timestamp;
    class Channel{      // Channel所有接口都只能在loop线程中调用
        using Eventcallback = std::function<void()>;    // 非读事件回调函数
        using Readcallback  = std::function<void(Timestamp)>;  // 读事件回调函数

    private:
        EventLoop* eventloop_; // eventloop对象指针
        int fd_;     // 文件描述符，只使用，不持有
        uint32_t events_ = 0;    // poll事件
        uint32_t revents_ = 0;   // 触发的事件

        int indx_ = -1;    // for poller

        // 事件回调处理函数
        Readcallback readcallback_;      // 读回调函数
        Eventcallback writecallback_;    // 写回调函数
        Eventcallback closecallback_;    // 关闭连接回调函数
        Eventcallback errorcallback_;    // 错误回调

        // 注册事件字段
        static const uint32_t kNoneEvent;
        static const uint32_t kReadEvent;
        static const uint32_t kWriteEvent;

    public:
        Channel(EventLoop* eventloop, int fd) : eventloop_(eventloop), fd_(fd) {}

        // 设置各事件回调函数
        void setReadCallback(Readcallback&& rd) {readcallback_ =  std::move(rd); }
        void setWriteCallback(Eventcallback&& wr) {writecallback_ = std::move(wr);}
        void setCloseCallback(Eventcallback&& cl) {closecallback_ = std::move(cl);}
        void setErrorCallback(Eventcallback&& er) {errorcallback_ = std::move(er);}
        void setReadCallback(const Readcallback& rd) {readcallback_ = rd; }
        void setWriteCallback(const Eventcallback& wr) {writecallback_ = wr;}
        void setCloseCallback(const Eventcallback& cl) {closecallback_ = cl;}
        void setErrorCallback(const Eventcallback& er) {errorcallback_ = er;}

        // 设置监听事件
        void enableRead();
        void disableRead();
        void enableWrite();
        void disableWrite();
        void disableAll();
        void update();  // 更新监听状态
        void remove();
        int indx(){return indx_;}
        void setIndx(int indx) {indx_ = indx;}
        uint32_t getEvent() {return events_;}
        void setrEvent(uint32_t event) {revents_ = event;}

        int fd()  {return fd_;}
        bool hasNoEvent() {return events_ == kNoneEvent;}

        void handleEvent(Timestamp receiveTime);
    };

}



#endif