#include <gmuduo/src/TCPServer.h>
#include <gmuduo/src/EventLoop.h>
#include <gmuduo/src/Socket.h>
#include <iostream>
#include <future>


using namespace gmuduo;
using namespace std;

int main() {
    EventLoop loop;
    InetAddr addr(8888);
    TCPServer server(&loop, addr, "TCPServerTest", false);
    server.setConnectionCallback([](std::shared_ptr<TCPConnection> tp){
        cout << "new connection" << tp->getNmae() << endl;
        tp->getLoop()->quit();
    });
    server.start();

    auto t =  std::async(std::launch::async, [addr](){
        Socket socket;
        socket.connect(addr);   
    });


    loop.loop();
    cout << "quit" << endl;
    t.wait();

    return 0;
}