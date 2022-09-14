#include "Acceptor.h"
#include "InetAddr.h"
#include "EventLoop.h"
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>



namespace gmuduo
{
    Acceptor::Acceptor(EventLoop* loop, const InetAddr& addr, bool reuseport) : 
        loop_(loop),
        socket_(),
        channel_(loop, socket_.getfd()),
        nullfd_(::open("/dev/null",  O_RDONLY | O_CLOEXEC)),
        isListening_(false)
    {
        socket_.setReuseAddr(true);
        socket_.setReusePort(reuseport);
        socket_.bind(addr);
        channel_.setReadCallback(std::bind(&Acceptor::handleRead, this));
    }
    Acceptor::~Acceptor(){
        channel_.disableAll();
        channel_.remove();
        ::close(nullfd_);
    }

    void Acceptor::listen(){
        loop_->assertInLoopThread();
        isListening_ = true;
        channel_.enableRead();
        socket_.listen();
    }
    void Acceptor::listen(int backof){
        loop_->assertInLoopThread();
        isListening_ = true;
        channel_.enableRead();
        socket_.listen(backof);
    }


    bool Acceptor::isListening() const { return isListening_;}

    void Acceptor::setNewConnectionCallback(const NewConnectionCallBack& cb) { callback_ = cb;}

    void Acceptor::handleRead() {
        int fd ;
        InetAddr peerAddr;
        while(true) {
            fd = socket_.accept(peerAddr) ; 
            if(fd >= 0) {
                if(callback_) {
                    callback_(fd, peerAddr);
                } else {
                    ::close(fd);
                }
            } else if(errno == EMFILE){
                // 文件描述符耗尽
                ::close(nullfd_);
                nullfd_ = ::accept(socket_.getfd(), NULL, NULL);
                ::close(nullfd_);
                nullfd_ = ::open("/dev/null",  O_RDONLY | O_CLOEXEC);
            } else {    // EAGAIN
                break;
            }
        }
       
    }
    
} // namespace gmuduo
