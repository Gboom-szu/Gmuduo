#include "Socket.h"
#include "Log.h"
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <string.h>
#include "InetAddr.h"
#include <errno.h>
#include "Log.h"



namespace gmuduo
{
    Socket::Socket(){
        fd_ = ::socket(PF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
        if(fd_ < 0) {
            LOG_ERROR("Socket::Socket() failed");
        }
    }
    Socket::Socket(int sockfd): fd_(sockfd){} 
    Socket::Socket(const InetAddr& addr) {
        fd_ = ::socket(PF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
        if(fd_ < 0) {
            LOG_ERROR("Socket::Socket() failed");
        }
        bind(addr);        
    }
    Socket::~Socket(){ ::close(fd_); }

    void Socket::bind(const InetAddr& addr) {
        auto ret = ::bind(fd_, reinterpret_cast<const sockaddr *>(addr.getSockaddrinPtr()), static_cast<socklen_t>(sizeof(sockaddr_in)));
        if (ret < 0) {
            LOG_ERROR("bind failed");
        }
    }
    
    void Socket::listen(int backlog) {
        auto ret = ::listen(fd_, backlog);
        if(ret < 0) {
            LOG_ERROR("listen failed");
        }
    }
    
    void Socket::listen() {
        auto ret = ::listen(fd_, SOMAXCONN);
        if(ret < 0) {
            LOG_ERROR("listen failed");
        }
    }
    
    bool Socket::connect(const InetAddr& addr) {
        int ret = ::connect(fd_, reinterpret_cast<const sockaddr*>(addr.getSockaddrinPtr()), static_cast<socklen_t>(sizeof(sockaddr_in)));
        return ret == 0 ;
    }

    void Socket::shutdownWrite() {
        auto ret = ::shutdown(fd_, SHUT_WR);
        if(ret < 0) {
            LOG_ERROR("shutdownWrite failed");
        }
    }
    
    int Socket::accept(InetAddr& peerAddr) {   // accept一个新连接
        sockaddr_in addr;
        socklen_t addrLen = sizeof(addr);
        memset(&addr, 0, sizeof addr);
        int sockfd = ::accept4(fd_, reinterpret_cast<sockaddr*>(&addr), &addrLen, SOCK_CLOEXEC | SOCK_NONBLOCK);
        // int sockfd = ::accept(fd_, reinterpret_cast<sockaddr*>(&addr), &addrLen);
        if(sockfd < 0) {
            handleError();
        }
        peerAddr.setSockaddrin(addr);
        return sockfd;
    }                    
    
    InetAddr Socket::getHostInetAddr() const {          // 获取本地地址
        sockaddr_in addr;
        socklen_t addrlen = static_cast<socklen_t>(sizeof addr);
        auto ret = ::getsockname(fd_, reinterpret_cast<sockaddr*>(&addr), &addrlen);
        if(ret < 0) {
            LOG_ERROR("getInetaddr failed");
        }
        return InetAddr(addr);
    }             
    
    InetAddr Socket::getPeerInetAddr() const {      // 获取对端地址
        sockaddr_in addr;
        socklen_t addrlen = static_cast<socklen_t>(sizeof addr);
        auto ret = getpeername(fd_, reinterpret_cast<sockaddr*>(&addr), &addrlen);
        if(ret < 0) {
            LOG_ERROR("getPeerInetAddr failed");
        }
        return InetAddr(addr);
    }         
    
    int Socket::getfd() const { return fd_;}
    
    std::size_t Socket::send(char * source, std::size_t len, int& saveErrno) {
        auto n = ::write(fd_, source, len);
        saveErrno = errno;
        return n;
    }

    void Socket::setNodelay(bool on) {
        int optval = on ? 1 : 0;
        auto ret =  ::setsockopt(fd_, IPPROTO_TCP, TCP_NODELAY, &optval, static_cast<socklen_t>(sizeof optval));
        if(ret < 0) {
            LOG_ERROR("setNodelay failed");
        }
    }
    
    void Socket::setReusePort(bool on) {
        int optval = on ? 1 : 0;
        auto ret =  ::setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &optval, static_cast<socklen_t>(sizeof optval));
        if(ret < 0) {
            LOG_ERROR("setReusePort failed");
        }
    }
    
    void Socket::setReuseAddr(bool on) {
        int optval = on ? 1 : 0;
        auto ret =  ::setsockopt(fd_, SOL_SOCKET, SO_REUSEPORT, &optval, static_cast<socklen_t>(sizeof optval));
        if(ret < 0) {
            LOG_ERROR("setReuseAddr failed");
        }
    }
    
    void Socket::setKeepAlive(bool on) {
        int optval = on ? 1 : 0;
        auto ret =  ::setsockopt(fd_, SOL_SOCKET, SO_KEEPALIVE, &optval, static_cast<socklen_t>(sizeof optval));
        if(ret < 0) {
            LOG_ERROR("setKeepAlive failed");
        }
    }
    
    void Socket::handleError() {
        int savedErrno = errno;
        LOG_INFO(errno);
        LOG_INFO("Socket::handleError");
        switch (savedErrno)
        {
            case EAGAIN:
            case ECONNABORTED:
            case EINTR:
            case EPROTO: // ???
            case EPERM:
            case EMFILE: // per-process lmit of open file desctiptor ???
                // expected errors
                errno = savedErrno;
                break;
            case EBADF:
            case EFAULT:
            case EINVAL:
            case ENFILE:
            case ENOBUFS:
            case ENOMEM:
            case ENOTSOCK:
            case EOPNOTSUPP:
                // unexpected errors
                perror("error");
                LOG_ERROR("unexpected error of ::accept ");
                break;
            default:
                LOG_ERROR("unknown error of ::accept ");
                break;
        }
    }
    


} // namespace gmuduo
