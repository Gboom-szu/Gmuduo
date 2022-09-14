#ifndef GMUDUO_COM_TYPE_H
#define GMUDUO_COM_TYPE_H

#include <cstdint>  // 引入各种整数类型别名，比如uint64_t, uint32_t等。

namespace gmuduo{
    class noncopyable      // 不可复制，以为着不可以作为容器元素
    {
        public:
        noncopyable(const noncopyable&) = delete;
        void operator=(const noncopyable&) = delete;

        protected:
        noncopyable() = default;
        ~noncopyable() = default;
    };

}


#endif