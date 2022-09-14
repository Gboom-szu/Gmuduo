#ifndef GMUDUO_SRC_BUFFER_H
#define GMUDUO_SRC_BUFFER_H
#include <vector>
#include <string>
#include "type.h"

namespace gmuduo
{
    // 环形缓冲区
    class Buffer{
    public:
        static const std::size_t kInitialSize = 1024;
        static constexpr  char kCRLF[] = "\r\n";
    private: 
        std::vector<char>  data_;
        std::size_t writeIndex_;                // writeIndex == readIndex 为空，writeIndex + 1 == readIndex 为满
        std::size_t readIndex_;

    public:
        explicit Buffer(std::size_t size = kInitialSize);

        bool empty() const;
        std::size_t readableBytes() const;
        std::size_t writeableBytes() const;         // 剩余空间，包括prependable
        std::size_t prependableBytes() const;
        void swap(Buffer& buf);

        // 写数据
        void append(const std::string& message);
        void append(const char * source, std::size_t len);
        void append(const void * source, std::size_t len);
        void append(const Buffer& buff);
        void appendInt8(int8_t);
        void appendInt16(int16_t);
        void appendInt32(int32_t);
        void appendInt64(int64_t);

        void prepend(const std::string& message);
        void prepend(const char * source, std::size_t len);
        void prepend(const void * source, std::size_t len);
        void prepend(const Buffer& buffer);
        void prependInt8(int8_t);
        void prependInt16(int16_t);
        void prependInt32(int32_t);
        void prependInt64(int64_t);             

        // 读
        void peek(char* dest, std::size_t len) const;   // 复制len字节到dest
        void peek(void* dest, std::size_t len) const;
        std::string peekAsString(std::size_t len) const;        // 作为字符串返回
        std::string peekAllAsString() const;                            

        int8_t peekInt8() const;              // 只读取不删除
        int16_t peekInt16() const;
        int32_t peekInt32() const;
        int64_t peekInt64() const;

        int8_t readInt8();              // 读取并删除
        int16_t readInt16();
        int32_t readInt32();
        int64_t readInt64();  

        void retrieve(size_t len);
        void retrieveAll();
        void retrieveUntil(const char* end);

        std::string retrieveAsString(size_t len);
        std::string retrieveAllAsString();

        const char* findCRLF() const;                   // 从起始位置开始查找 \r\n
        const char* findCRLF(const char* start) const;  // 从start位置开始查找 \r\n
        const char* findEOL() const;                    // 查找 \n
        const char* findEOL(const char* start) const;


        // 这两个不是用户接口
        // 从socket读数据
        std::size_t readFd(int fd, int& savedErrno);
        std::size_t writeFd(int fd, int& savedErrno);

    private:
        void ensurePlace(std::size_t len);
        void makePlace(std::size_t len);

    };
    
} // namespace gmuduo



#endif