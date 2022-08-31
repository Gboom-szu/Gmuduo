#include <gmuduo/src/EventLoop.h>
#include <iostream>

using namespace std;


// 测试timer功能
int main() {
    gmuduo::EventLoop eventloop;
    eventloop.runAfter(1000000, [](){
        cout << "1 定时器延迟1s" << endl;
    });
    auto timeId2 = eventloop.runEvery(2000000, [](){
        cout << "2 定时器延迟2s" << endl;
    });
    eventloop.runAfter(20000000, [&eventloop, &timeId2](){
        cout << "3 定时器取消定时器2" << endl;
        eventloop.cancelTimer(timeId2);
    });
    eventloop.loop();

    return 0;
}