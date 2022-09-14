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
        sockaddr_in addr_;
    public:
        InetAddr(); 
        explicit InetAddr(uint16_t port);            // 不指定ip，则为ADDRANY
        explicit InetAddr(const sockaddr_in& addr);
        explicit InetAddr(std::string_view str);     // 随机分配端口
        InetAddr(std::string_view str, uint16_t port);


         
        std::string getIPStr() const;
        std::string getPortStr() const;
        std::string getIPportStr() const;
        uint32_t getIpNetEndian() const {return addr_.sin_addr.s_addr;} 
        uint16_t getPortNetEndian() const {return addr_.sin_port;}
        void setSockaddrin(const struct sockaddr_in& addr) {addr_ = addr;}
        const sockaddr_in* getSockaddrinPtr() const {return &addr_;}
    };
    
} // namespace gmuduo




#endif