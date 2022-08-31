set(SRCS 
    EventLoop.cc
    Channel.cc
    Epoll.cc
    TimerQueue.cc
)

add_library(gmuduo ${SRCS})
target_link_libraries(gmuduo gmuduo_com)

add_subdirectory(test)