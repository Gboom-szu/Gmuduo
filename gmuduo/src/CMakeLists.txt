set(SRCS 
    EventLoop.cc
    Channel.cc
    Epoll.cc
)

add_library(gmuduo ${SRCS})

add_subdirectory(test)