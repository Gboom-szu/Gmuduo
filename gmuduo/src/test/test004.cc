#include <gmuduo/src/EventLoop.h>
#include <iostream>

using namespace std;


// 测试timer功能
int main() {
    gmuduo::EventLoop eventloop;
    eventloop.runAfter(1000, [](){
        cout << "1 定时器延迟1s" << endl;
    });
    eventloop.runAfter(2000, [](){
        cout << "2 定时器延迟1s" << endl;
    });
    eventloop.runEvery(4000, [](){
        cout << "3 定时器每隔2s" << endl;
    });
    eventloop.loop();

    return 0;
}