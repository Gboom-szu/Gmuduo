#ifndef GMUDUO_SRC_EPOLL_H
#define GMUDUO_SRC_EPOLL_H
#include <unordered_map>
#include <sys/epoll.h>
#include <vector>
#include <unordered_map>
#include "Log.h"

namespace gmuduo{
    class Channel;
    class Epoll{
    private:
        static const int KInitSize = 16;     // activeEvents_初始大小
        int epollFd_;   // epoll实例文件符
        std::unordered_map<int, Channel*> channelMap_;   // fd to channel
        std::vector<epoll_event> activeEvents_;

        void update(uint32_t operation, Channel* channel);      // 更新epoll
        void fillActiveChannels(int num, std::vector<Channel*>& activeChannels);
    public:
        Epoll(): activeEvents_(KInitSize) {
            epollFd_ = epoll_create1(EPOLL_CLOEXEC);
            if(epollFd_ < 0 ) {
                LOG_FATAL("cannot create epoll");
            }
        }
        void updateChannel(Channel* channel);
        // void removeChannel(Channel* channel);

        uint64_t poll(int timeout, std::vector<Channel*>& activeChannels);      // 返回poll结束的时间
    };
}



#endif