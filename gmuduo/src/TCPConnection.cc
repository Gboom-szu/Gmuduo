#include "TCPConnection.h"
#include "EventLoop.h"
#include "Socket.h"
#include "Channel.h"
#include "Log.h"
#include <unistd.h>
#include <memory>

namespace gmuduo
{
    TCPConnection::TCPConnection(EventLoop* loop, int fd, const InetAddr& peerAddr, std::string name):
    socket_(std::make_unique<Socket>(fd)),
    channel_(new Channel(loop, fd)),
    loop_(loop),
    fd_(fd),
    peerAddr_(peerAddr),
    hostAddr_(socket_->getHostInetAddr()),
    name_(std::move(name)),
    status_(kConnecting),
    highLevelThreshold_(64*1024*1024)
    {
        channel_->setReadCallback(std::bind(&TCPConnection::handleRead, this, std::placeholders::_1));
        channel_->setWriteCallback(std::bind(&TCPConnection::handleWrite, this));
        channel_->setErrorCallback(std::bind(&TCPConnection::handleError, this));
        channel_->setCloseCallback(std::bind(&TCPConnection::handleClose, this));
        socket_->setKeepAlive(true);
    }

    void TCPConnection::setMessageCallback(MessageCallback cb) {
        messageCallback_ = std::move(cb);
    }
    void TCPConnection::setWriteCompleteCallback(WriteCompleteCallback cb) {
        writeCompleteCallback_ = std::move(cb);
    }
    void TCPConnection::setonConnection(ConnectionCallback cb) {
        connectionCallback_ = std::move(cb);
    }
    void TCPConnection::setCloseCallback(CloseCallback cb) {
        closeCallback_ = std::move(cb);
    }
    void TCPConnection::setHighLevelCallback(HighLevelCallback cb) {
        highLevelCallback_ = std::move(cb);
    }

    // 接口
    void TCPConnection::send(const std::string& message) {
        if(status_ == kConnected) {
            if(loop_->isInLoopThread()) {
                sendInLoop(message);
            } else {
                loop_->queueInLoop([this, str = message](){
                    this->sendInLoop(str);
                });
            }
        }
    }
    
    void TCPConnection::send(const void* source, std::size_t len) {
        if(status_ != kConnected) return;
        if(loop_->isInLoopThread()) {
            sendInLoop(source, len);
        } else {
            loop_->queueInLoop([this, mess = std::string(static_cast<const char*>(source), len)](){
                this->sendInLoop(mess);
            });
        }
    }

    void TCPConnection::send(Buffer& buf) {
        if(status_ != kConnected) return;
        if(loop_->isInLoopThread() && outputBuffer_.empty()) {
            int saveErrno;
            auto n = buf.writeFd(fd_, saveErrno); 
            if(n < 0) {
                LOG_WARN("TCPConnection::send error");
                errno = saveErrno;
                handleError();
            } else {
                buf.retrieve(n);
            }
        }
        if(!buf.empty()) {              // 还有剩的
            loop_->queueInLoop([this, message = buf.retrieveAllAsString()](){
                this->sendInLoop(message);
            });
        }
    }
   
    void TCPConnection::sendInLoop(const char* source, std::size_t len) {
        loop_->assertInLoopThread();
        if(status_ != kConnected) return;
        std::size_t n = 0;
        if(outputBuffer_.empty()) {        // 发送区缓冲区为空，可以直接发送
            n = ::write(fd_, source, len);
            if(n < 0) {
                LOG_WARN("TCPConnection::send errno: {0}", errno);
                handleClose();
            }
        }
        if(n != len) {
            auto previousLen = outputBuffer_.readableBytes();
            len -= n;
            outputBuffer_.append(source + n, len);
            channel_->enableWrite();
            if(previousLen < highLevelThreshold_ && outputBuffer_.readableBytes() >= highLevelThreshold_) {
                if(highLevelCallback_) highLevelCallback_(shared_from_this(), outputBuffer_.readableBytes());
            }
        }        
    }

    void TCPConnection::sendInLoop(const void* source, std::size_t len) { 
        sendInLoop(static_cast<const char*>(source), len);
    } 

    void TCPConnection::sendInLoop(std::string_view message) {
        sendInLoop(message.data(), message.size());
    }

    void TCPConnection::handleRead(Timestamp achieveTime) {
        loop_->assertInLoopThread();
        int saveErrono;
        auto n = inputBuffer_.readFd(fd_, saveErrono);
        if(n > 0) {
            messageCallback_(shared_from_this(), inputBuffer_);
        } else if(n == 0) {                                         // 读空，对端关闭
            handleClose();
        } else {
            errno = saveErrono;
            handleError();
        }    
    }

    void TCPConnection::handleWrite(){
        LOG_INFO("TCPConnection::handleWrite fd:{0}", fd_);
        if(!outputBuffer_.empty()) {
            int saveErrno;
            auto n = outputBuffer_.writeFd(fd_, saveErrno);
            if(n >= 0) {
                outputBuffer_.retrieve(n);
                if(outputBuffer_.empty()) {            // 已经没有数据要写了
                    channel_->disableWrite();
                    if(writeCompleteCallback_) 
                        writeCompleteCallback_(shared_from_this());
                    if(status_ == kDisConnecting) {    // 缓冲区的数据全部发送完了，可以关闭了
                        shutdownInLoop();
                    }
                }
            } else {
                errno = saveErrno;
                LOG_WARN("TCPConnection::handleWrite errno:{0}", errno);
                handleError();
            }
        }
    }

    void TCPConnection::handleError(){
        LOG_WARN("TCPConnection::handleError errno{0}", errno);
    }

    void TCPConnection::handleClose(){
        loop_->assertInLoopThread();
        LOG_INFO("TCPConnection fd:{0} cloesd", fd_);
        if(status_ != kDisConnected) {
            setStatus(kDisConnected);
            channel_->disableAll();
            channel_->remove();                             // 这个函数不会有延迟，因为handleclose就是在io线程中
            if(connectionCallback_)                         // 先调用用户的
                connectionCallback_(shared_from_this());    // 这个函数执行完之后，对象就会析构
            if(closeCallback_)
                closeCallback_(shared_from_this());
        }

    }

    void TCPConnection::onEstablished() {
        loop_->assertInLoopThread();
        LOG_TRACE("TCP connected");
        setStatus(kConnected);
        channel_->enableRead();
        if(connectionCallback_) {
            connectionCallback_(shared_from_this());
        }
    }
   
    void TCPConnection::onDestroyed() {
        loop_->assertInLoopThread();
        channel_->remove();
    }

    void TCPConnection::setStatus(Status s) {loop_->assertInLoopThread(); status_ = s;}

    void TCPConnection::shutdown() {
        loop_->runInLoop(std::bind(&TCPConnection::shutdownInLoop, this));      
    }

    void TCPConnection::shutdownInLoop() {
        loop_->assertInLoopThread();
        LOG_INFO("tcp shutdown");
        if(status_ == kConnected) {
            setStatus(kDisConnecting);
        } else if(status_ == kDisConnecting) {
            if(outputBuffer_.empty()) {
                socket_->shutdownWrite();
            }
        }     
    }

    void TCPConnection::forceClose() {
        loop_->runInLoop(std::bind(&TCPConnection::forceCloseInLoop, this));
    }
    
    void TCPConnection::forceCloseInLoop() {
        loop_->assertInLoopThread();
        if(status_ != kDisConnected)
            handleClose();
    }

} // namespace gmuduo
