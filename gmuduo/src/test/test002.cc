#include <gmuduo/src/EventLoop.h>
#include <thread>
#include <iostream>


// 测试EventLoop
int main() {
    gmuduo::EventLoop loop;
    auto t = std::thread([&loop]() {
        loop.loop();
    });
    t.join();

    return 0;
}