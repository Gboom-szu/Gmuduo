#ifndef GMUDUO_SRC_EVENTLOOP_H
#define GMUDUO_SRC_EVENTLOOP_H

#include <memory>
#include <vector>

namespace gmuduo{

    class Channel;
    class Epoll;
    // one loop per thread类，即一个线程只能创建一个对象，复杂poll
    class EventLoop
    {
    private:
        bool quited_ = false;
        std::unique_ptr<Epoll> poller_;
        std::vector<Channel*> activeChannels_;      // 可操作的channel列表

        bool isInLoopThread();
    public:
        EventLoop();
        ~EventLoop();
        void loop();    // 开始poll
        void assertInLoopThread();
        void updateChannel(Channel*);   // 更新Channel监听状态
    };
    


}



#endif