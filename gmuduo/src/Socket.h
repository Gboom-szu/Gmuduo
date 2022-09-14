#ifndef GMUDUO_SRC_SOCKET_H
#define GMUDUO_SRC_SOCKET_H

#include "InetAddr.h"
#include "type.h"


namespace gmuduo
{
    class InetAddr;
    class Socket : public noncopyable{      // noncopyable: 禁止复制，确保只有一个对象可以管理fd的生命周期
    private:
        int fd_;

        static void handleError();

    public:
        Socket();                               // 创建一个新的socket
        explicit Socket(int sockfd);     // 管理一个socket描述符，通常是accept返回的
        explicit Socket(const InetAddr& addr);
        ~Socket();
        void bind(const InetAddr& addr);
        bool connect(const InetAddr& addr);
        void listen(int backlog);
        void listen();
        void shutdownWrite();
        int accept(InetAddr& peerAddr);                             // accept一个新连接.这里直接返回一直文件描述符，而不是Socket对象，为了可以让Socket管理fd生命周期
        InetAddr getHostInetAddr() const;             // 获取本地地址
        InetAddr getPeerInetAddr() const;         // 获取对端地址
        int getfd() const;

        std::size_t send(char * source, std::size_t len, int& saveErrno);       // 发送数据

        void setNodelay(bool on);
        void setReusePort(bool on);
        void setReuseAddr(bool on);             // SOL_REUSEADDR的作用：允许0.0.0.0与其它ip绑定到同一个端口；即使处于time_wait也可以重新绑定（一般用于服务器）。
        void setKeepAlive(bool on);

    };
    
} // namespace gmuduo



#endif