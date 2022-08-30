#include <gmuduo/src/EventLoop.h>
#include <gmuduo/src/Channel.h>
#include <gmuduo/com/Timestamp.h>
#include <iostream>
#include <unistd.h>
#include <future>


// 验证epoll channel 和eventloop

int fd[2];
int main() {
    if( pipe(fd) == -1) {
        std::cout << "pipe error" << std::endl;
        return -1;
    }

    gmuduo::EventLoop eventLoop;
    auto channel = new gmuduo::Channel(&eventLoop, fd[0]);
    channel->enableRead();
    channel->setReadCallback([](gmuduo::Timestamp time) {
        char ch[20];
        read(fd[0], ch, 19);
        ch[19] = '\0';
        std::cout << ch << std::endl;
    });

    auto p = std::async([](){
        sleep(2);
        std::cout << "thread write pipe: hello world" << std::endl;
        const char* ch= "hello world";
        write(fd[1], ch, 12);
    });

    std::cout << "start loop" << std::endl;
    eventLoop.loop();
    p.wait();

    return 0;
}
