#include "InetAddr.h"
#include <arpa/inet.h>
#include <string.h>



namespace gmuduo
{
    InetAddr::InetAddr() {
        memset(&addr_, 0, sizeof addr_);
    }
    InetAddr::InetAddr(const sockaddr_in& addr) {
        memcpy(&addr_, &addr, sizeof addr_);
    }
    InetAddr::InetAddr(std::string_view str) {
        memset(&addr_, 0, sizeof addr_);
        addr_.sin_family = AF_INET;
        ::inet_pton(AF_INET, str.data(), &addr_.sin_addr.s_addr);
    }

    InetAddr::InetAddr(std::string_view str, uint16_t port) {
        memset(&addr_, 0, sizeof addr_);
        addr_.sin_family = AF_INET;
        addr_.sin_port = htons(port);
        ::inet_pton(AF_INET, str.data(), &addr_.sin_addr.s_addr);
    }
    InetAddr::InetAddr(uint16_t port) {
        memset(&addr_, 0, sizeof addr_);
        addr_.sin_family = AF_INET;
        addr_.sin_port = htons(port);
        addr_.sin_addr.s_addr = INADDR_ANY;
    }
    std::string InetAddr::getIPStr() const {
        char buf[16];
        inet_ntop(AF_INET, &addr_.sin_addr.s_addr, buf, static_cast<socklen_t> (sizeof (buf)));
        return buf;
    }
    std::string InetAddr::getPortStr() const {
        unsigned port = ntohs(addr_.sin_port);
        return std::to_string(port);
    }
    std::string InetAddr::getIPportStr() const {
        char buf[16];
        inet_ntop(AF_INET, &addr_.sin_addr.s_addr, buf, static_cast<socklen_t>(sizeof (buf)));
        unsigned port = ntohs(addr_.sin_port);
        return std::string(buf) + " : " +  std::to_string(port);
    }
         
} // namespace gmuduo
