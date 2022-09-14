#ifndef GMUDUO_SRC_ACCEPTRO_H
#define GMUDUO_SRC_ACCEPTOR_H


#include <functional>
#include "Channel.h"
#include "Socket.h"

namespace gmuduo
{
    class EventLoop;
    class InetAddr;

    class Acceptor{     // 非接口类
    private:
        using NewConnectionCallBack = std::function<void(int fd, const InetAddr& peerAddr)>;; 
        EventLoop* loop_;     // 所属于的loop
        Socket socket_;
        Channel channel_;
        int nullfd_;        // 用于处理文件描述符耗尽的情况
        bool isListening_;
        NewConnectionCallBack callback_;
        void handleRead();


    public:
        Acceptor(EventLoop* loop, const InetAddr& addr, bool reuseport);
        ~Acceptor();
        void listen();
        void listen(int backof);
        bool isListening() const ;
        void setNewConnectionCallback(const NewConnectionCallBack& cb);       // 设置新连接回调函数
        
    };
    
} // namespace gmuduo




#endif