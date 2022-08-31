#include <gmuduo/src/EventLoop.h>
#include <iostream>


int main() {
    gmuduo::EventLoop eventLoop;
    eventLoop.runAfter(3000000, [&eventLoop](){
        std::cout << "timer set queueInLoop" << std::endl;
        eventLoop.queueInLoop([](){
            std::cout << "queueInLoop" << std::endl;
        });
    });
    eventLoop.runEvery(1000000, [](){
        std::cout << "heart beats" << std::endl;
    });
    eventLoop.runAfter(6000000,[&eventLoop](){
        std::cout << "loop exits at 6s" << std::endl;
        eventLoop.quit();
    });
    eventLoop.loop();

    return 0;
}