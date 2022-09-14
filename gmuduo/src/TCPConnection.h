#ifndef GMUDUO_SRC_TCPCONNECTION_H
#define GMUDUO_SRC_TCPCONNECTION_H

#include <memory>
#include <string>
#include <functional>
#include <atomic>
#include "Timestamp.h"
#include "InetAddr.h"
#include "callback.h"
#include "Buffer.h"


namespace gmuduo
{
    class Socket;
    class EventLoop;
    class Channel;
    class TCPConnection : public std::enable_shared_from_this<TCPConnection> {        // 接口类
    public:
        enum Status{
            kConnecting = 0,
            kConnected,
            kDisConnecting,
            kDisConnected
        };      // 连接的状态
    private:
        using TCPConnectionPtr = std::shared_ptr<TCPConnection>;
        using CallbackFunc = std::function<void(TCPConnectionPtr)>;

        std::unique_ptr<Socket> socket_;
        std::unique_ptr<Channel> channel_;
        EventLoop* const loop_;
        const int fd_;
        const InetAddr peerAddr_;
        const InetAddr hostAddr_;
        const std::string name_;
        std::atomic_int32_t status_;       // 连接状态
        std::size_t highLevelThreshold_;        // 高水位阈值

        Buffer inputBuffer_;
        Buffer outputBuffer_;

        MessageCallback messageCallback_;               // 消息到达时
        CloseCallback closeCallback_;                   // 对端关闭连接时
        ConnectionCallback connectionCallback_;         // 连接刚建立时
        WriteCompleteCallback writeCompleteCallback_;   // 低水位回调
        HighLevelCallback   highLevelCallback_;         // 高水位回调
        
    public:
        TCPConnection(EventLoop* loop, int fd, const InetAddr& addr, std::string name);

        // 这几个不是接口
        void setMessageCallback(MessageCallback cb);
        void setCloseCallback(CloseCallback cb);
        void setWriteCompleteCallback(WriteCompleteCallback cb);
        void setHighLevelCallback(HighLevelCallback cb);
        void setStatus(Status s);
        int getStatus() {return status_;}

        void onEstablished();       // tcp连接刚建立是，需要做一些工作，比如channel。
        void onDestroyed();

        // 这个是接口, 用户通过这个ConnectionCallback处理连接的状态变化
        void setonConnection(ConnectionCallback cb);
        void setHighLevel(std::size_t threshold) {highLevelThreshold_ = threshold;}
        void shutdown();                        // 关闭写
        void forceClose();                      // 调用close

        // 接口
        void send(const std::string& message);
        void send(const void* source, std::size_t len);
        void send(Buffer& buf);
        std::string getNmae() const {return name_;}
        EventLoop* getLoop() const {return loop_;}
    
    private:
        void handleRead(Timestamp);
        void handleWrite();
        void handleError();
        void handleClose();
        void sendInLoop(const void* source, std::size_t len);
        void sendInLoop(const char* source, std::size_t len);
        void sendInLoop(std::string_view message);
        void shutdownInLoop();
        void forceCloseInLoop();
    };
    
} // namespace gmuduo



#endif