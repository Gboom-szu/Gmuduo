#include <gmuduo/src/Buffer.h>
#include <iostream>

using namespace std;
using namespace gmuduo;

int main() {

    Buffer buffer(8);
    cout << "readable : " << buffer.readableBytes() << " writebale: " << buffer.writeableBytes() << " prependable " << endl;
    buffer.append("123");
    cout << "readable : " << buffer.readableBytes() << " writebale: " << buffer.writeableBytes() << " prependable " << endl;
    cout << buffer.peekAllAsString() << endl;
    buffer.prepend("87654321");
    cout << buffer.peekAllAsString() << endl;
    cout << buffer.peekInt16() << endl;
    buffer.readInt16();
    cout << buffer.peekAllAsString() << endl;
    cout << buffer.retrieveAsString(3) << endl;
    cout << buffer.peekAllAsString() << endl;
    
    cout << "----------------------------" << endl;
    Buffer buffer2(5);
    buffer2.append("1234");
    buffer2.readInt16();
    buffer2.append("\r");
    buffer2.append("\n");
    cout << buffer2.peekAllAsString() << endl;
    auto crlf = buffer2.findCRLF();
    cout << int(crlf[0]) << " " << int('\n') << endl;

    cout << "----------------------------" << endl;
    Buffer buffer3(8);
    int saveErrno;
    buffer3.readFd(0, saveErrno);
    cout << buffer3.peekAllAsString() << endl;



    return 0;
}