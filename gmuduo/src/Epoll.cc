#include "Epoll.h"
#include "Channel.h"
#include "Log.h"
#include <errno.h>
#include <string.h>


namespace gmuduo{
    const int kNotListening = -1;    // 没有监听（包括新建的）
    const int kListening = 1;       // 正在监听

    void Epoll::updateChannel(Channel *channel) {
        auto fd = channel->fd();
        auto status = channel->indx();
        if(status == kNotListening) {
            if(channelMap_.find(fd) == channelMap_.end()) {
                channelMap_[fd] = channel;
            }
            if(!channel->hasNoEvent()) {
                LOG_INFO("EPOLL_CTL_ADD");
                update(EPOLL_CTL_ADD, channel);
                channel->setIndx(kListening);
            }
        } else {
            if(channel->hasNoEvent()) {
                LOG_INFO("EPOLL_CTL_DEL");
                update(EPOLL_CTL_DEL, channel);
                channel->setIndx(kNotListening);
            } else
            {
                LOG_INFO("EPOLL_CTL_MOD");
                update(EPOLL_CTL_MOD, channel);                
            }
        }
    }

    void Epoll::update(uint32_t operation, Channel* channel) {
        epoll_event event;
        event.events = channel->getEvent();
        event.data.ptr = static_cast<void *>(channel);
        if(epoll_ctl(epollFd_, operation, channel->fd(), &event) < 0) {
            LOG_INFO(strerror(errno));
            LOG_FATAL("failed to epoll_ctl");
        }
    }

    void Epoll::fillActiveChannels(int num, std::vector<Channel*>& activeChannels) {
        for(int i = 0; i < num; i++) {
            auto channel = static_cast<Channel*>(activeEvents_[i].data.ptr);
            channel->setrEvent(activeEvents_[i].events);
            activeChannels.emplace_back(channel);
        }
    }

    Timestamp Epoll::poll(int timeout, std::vector<Channel*>& activeChannels) {
        int num = epoll_wait(epollFd_, &(*activeEvents_.begin()), activeEvents_.capacity(), timeout);
        Timestamp now;  // 获取当前时间

        if(num < 0) {
            LOG_FATAL("epoll_wait failed");
        }
        if(num) {
            fillActiveChannels(num, activeChannels);
            if(num == activeEvents_.capacity()) {
                activeEvents_.reserve(2 * activeEvents_.capacity());
            }
        }


        return now;
    }
}