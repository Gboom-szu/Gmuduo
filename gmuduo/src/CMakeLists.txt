set(SRCS 
    EventLoop.cc
    Channel.cc
    Epoll.cc
    TimerQueue.cc
    InetAddr.cc
    Log.cc
    Socket.cc
    Acceptor.cc
    TCPServer.cc
    TCPClient.cc
    TCPConnection.cc
    EventLoopThread.cc
    EventLoopThreadpool.cc
    Buffer.cc
    Timestamp.cc
)

add_library(gmuduo ${SRCS})
target_link_libraries(gmuduo spdlog)

add_subdirectory(test)