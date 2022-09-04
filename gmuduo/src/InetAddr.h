#ifndef GMUDUO_SRC_INETADDR_H
#define GMUDUO_SRC_INETADDR_H

#include <netinet/in.h>
#include <string_view>
#include <string>


namespace gmuduo
{
    // 封装ip地址, 只支持ipv4
    class InetAddr{
    private:
        struct sockaddr_in addr_;
    public:
        InetAddr(std::string_view str);     // 随机分配端口
        InetAddr(std::string_view str, uint16_t port);  
        InetAddr(uint16_t port);            // 不指定ip，则为ADDRANY
        std::string getIPStr();
        std::string getPortStr();
        std::string getIPportStr();
        uint32_t getIpNetEndian() {return addr_.sin_addr.s_addr;} 
        uint16_t getPortNetEndian() {return addr_.sin_port;}
        void setSockaddrin(const struct sockaddr_in& addr) {addr_ = addr;}
    };
    
} // namespace gmuduo




#endif