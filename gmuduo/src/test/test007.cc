#include <thread>
#include <gmuduo/src/Acceptor.h>
#include <gmuduo/src/EventLoop.h>
#include <gmuduo/src/InetAddr.h>
#include <iostream>


using namespace std;
using namespace gmuduo;


// test acceptor

void threadFunc(gmuduo::InetAddr addr) {
    gmuduo::Socket sock;
    sock.connect(addr);
}


int main() {
    gmuduo::EventLoop loop;
    InetAddr addr("127.0.0.1", 8888);
    Acceptor acceptor(&loop, addr, false);
    auto callback = [&loop](int, const InetAddr&) {
        cout << "acceptor \n" << endl;
        loop.quit();
        cout << "acceptor2 \n" << endl;
    };    
    acceptor.setNewConnectionCallback(callback);
    acceptor.listen();
    std::thread t(threadFunc, addr);
    loop.loop();
    
    t.join();


    return 0;
}