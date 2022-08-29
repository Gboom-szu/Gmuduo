#include <gmuduo/src/EventLoop.h>
#include <thread>
#include <iostream>

 
// 对EventLoop进行单元测试

int main() {
    gmuduo::EventLoop loop;
    auto t = std::thread([](){
        gmuduo::EventLoop loop;
        loop.loop();
    });
    loop.loop();
    t.join();
    std::cout << "两个线程, 两个loop" << std::endl;

    gmuduo::EventLoop loop2;
    
    
    return 0;
}