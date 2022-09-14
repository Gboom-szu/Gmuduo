#ifndef GMUDUO_SRC_TCPSERVER_H
#define GMUDUO_SRC_TCPSERVER_H

#include <unordered_map>
#include <memory>
#include <string>
#include <functional>
#include "TCPConnection.h"
#include "InetAddr.h"
#include "callback.h"


namespace gmuduo
{
    class Acceptor;
    class EventLoop;
    class TCPConnection;
    class EventLoopThreadpool;
    class TCPServer{  // 接口类，TCPServer所有的函数也应该只在
    private:
        
        EventLoop* loop_;                   // 所属的loop。本来还想一个server就是一个线程的，但是想到可能多个server公用一个线程。
        const std::string name_;
        const std::string IPPort_;          // IP:Port 字符串
        int numthrad_;                      // 线程数  
        int conectionID_;   
        std::unique_ptr<Acceptor> acceptorPtr_;
        std::unordered_map<std::string, TCPConnectionPtr>  TCPPtrMap_;
        std::unique_ptr<EventLoopThreadpool> threadpool_;

        // 回调函数
        MessageCallback mesageCallback_;
        WriteCompleteCallback writeCompleteCallback_;
        ConnectionCallback connectionCallback_;


    public:
        TCPServer(EventLoop* loop, const InetAddr& addr, std::string name, bool reuseport);
        ~TCPServer();        
        
        void start();                                               // 启动服务器
        void setThreadNum(int num);
        
        // 针对TCPConnection
        void setMessageCallback(const MessageCallback& cb);
        void setWriteCompleteCallback(const WriteCompleteCallback& cb);
        void setConnectionCallback(const ConnectionCallback& cb);         // 连接刚建立时的回调函数
        void removeConnection(TCPConnectionPtr  tcpcontptr);        // 删除TCP连接,可以在非io线程中调用
        void removeConnectionInLoop(TCPConnectionPtr  tcpcontptr);  // 必须在io线程中调用




    private:
        void newConnection(int fd, const InetAddr& addr);                  // acceptor可读时调用，主要是创建新连接
    };
    
} // namespace gmuduo



#endif