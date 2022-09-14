#include "TCPServer.h"
#include "EventLoop.h"
#include "EventLoopThreadpool.h"
#include "Acceptor.h"
#include <stdio.h>  // snprintf

namespace gmuduo
{
    TCPServer::TCPServer(EventLoop* loop, const InetAddr& addr, std::string name, bool reuseport): 
        loop_(loop),
        name_(std::move(name)),
        IPPort_(addr.getIPportStr()),
        numthrad_(0),
        conectionID_(0),
        acceptorPtr_(std::make_unique<Acceptor>(loop, addr, reuseport)),
        threadpool_(std::make_unique<EventLoopThreadpool>(loop_))
    {
        acceptorPtr_->setNewConnectionCallback(std::bind(&TCPServer::newConnection, this, std::placeholders::_1, std::placeholders::_2));

    }
    TCPServer::~TCPServer() {

    }

    void TCPServer::start() {
        loop_->assertInLoopThread();
        threadpool_->start();           // 启动线程池
        acceptorPtr_->listen();
    }           

    void TCPServer::setThreadNum(int num) {
        loop_->assertInLoopThread();
        threadpool_->setThreadNum(num);
    }

    // 针对TCPConnection
    void TCPServer::setMessageCallback(const MessageCallback& cb) {
        mesageCallback_ = cb;
    }

    void TCPServer::setWriteCompleteCallback(const WriteCompleteCallback& cb) {
        writeCompleteCallback_ = cb;
    }

    void TCPServer::setConnectionCallback(const ConnectionCallback& cb) {
        connectionCallback_ = cb;
    }   

    void TCPServer::removeConnection(TCPConnectionPtr  tcpcontptr) {
        loop_->runInLoop(std::bind(&TCPServer::removeConnectionInLoop, this, tcpcontptr));
    }  

    void TCPServer::removeConnectionInLoop(TCPConnectionPtr tcpcontptr) {
        loop_->assertInLoopThread();
        TCPPtrMap_.erase(tcpcontptr->getNmae());
    }


    // 这个函数的调用时机一定时在io线程中
    void TCPServer::newConnection(int fd, const InetAddr& peerAddr) {
        char buf[64];
        snprintf(buf, sizeof buf, "-%s#%d", IPPort_.c_str(), conectionID_);
        conectionID_++;
        std::string conName = name_ + buf;
        auto ioLoop = threadpool_->getOneLoop();
        auto newConPtr = std::make_shared<TCPConnection>(ioLoop, fd, peerAddr, conName);
        TCPPtrMap_[conName] = newConPtr;
        newConPtr->setMessageCallback(mesageCallback_);
        newConPtr->setWriteCompleteCallback(writeCompleteCallback_);
        newConPtr->setonConnection(connectionCallback_);
        newConPtr->setCloseCallback(std::bind(&TCPServer::removeConnection, this, newConPtr));

        ioLoop->runInLoop(std::bind(&TCPConnection::onEstablished, newConPtr));
    } 

    
} // namespace gmuduo
