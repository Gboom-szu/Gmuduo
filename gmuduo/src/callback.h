#ifndef GMUDUO_SRC_CALLBACK_H
#define GMUDUO_SRC_CALLBACK_H

#include <functional>
#include <memory>


namespace gmuduo
{
    class TCPConnection;
    class Buffer;

    using TCPConnectionPtr = std::shared_ptr<TCPConnection>;
    using CallbackFunc = std::function<void(TCPConnectionPtr)>;
    using MessageCallback = std::function<void(TCPConnectionPtr, Buffer&)>;
    using WriteCompleteCallback = std::function<void(TCPConnectionPtr)>;
    using HighLevelCallback = std::function<void(TCPConnectionPtr, std::size_t)>;
    using ConnectionCallback = std::function<void(TCPConnectionPtr)>;
    using CloseCallback = std::function<void(TCPConnectionPtr)>;
    
} // namespace gmuduo






#endif