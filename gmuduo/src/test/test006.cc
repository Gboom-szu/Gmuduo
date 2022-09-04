#include <gmuduo/src/InetAddr.h>
#include <iostream>


using namespace std;

int main() {
    cout << gmuduo::InetAddr("127.0.0.1").getIPStr() << endl;
    cout << gmuduo::InetAddr(15).getIPportStr() << endl;
    cout << gmuduo::InetAddr("127.5.5.4", 23).getIPportStr() << endl;


    return 0;


}