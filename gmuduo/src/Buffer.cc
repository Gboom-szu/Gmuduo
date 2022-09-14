#include "Buffer.h"
#include <endian.h>
#include <algorithm>
#include <string.h>
#include <assert.h>
#include <sys/uio.h>


namespace gmuduo
{

    Buffer::Buffer(std::size_t size ):
        data_(size + 1),                            // 需要多一位作为写满标记
        writeIndex_(0),
        readIndex_(0)
    {}

    bool Buffer::empty() const {return writeIndex_ == readIndex_;}
    std::size_t Buffer::readableBytes() const{
        return writeIndex_ >= readIndex_ ? 
            writeIndex_ - readIndex_:
            data_.size() - readIndex_ + writeIndex_;
    }
    std::size_t Buffer::writeableBytes() const{
        return data_.size() - readableBytes() - 1;      // 剩余一字节作为缓冲区满的判断
    }
    std::size_t Buffer::prependableBytes() const{       // 所有可写空间都可以作为prepend
        return writeableBytes();
    }

    void Buffer::swap(Buffer& buf) {
        std::swap(buf.data_, data_);
        std::swap(buf.readIndex_, readIndex_);
        std::swap(buf.writeIndex_, writeIndex_);
    }
    void Buffer::ensurePlace(std::size_t len) {
        if(writeableBytes() < len) {
            makePlace(len);
        }
    }

    void Buffer::makePlace(std::size_t len) {       // 多一位作为写满标记
        if(writeIndex_ >= readIndex_) {
            data_.resize(writeIndex_ + len + 1);
        } else {
            Buffer temp;
            temp.ensurePlace(writeableBytes() + len + 1);
            temp.append(retrieveAllAsString());
            swap(temp);
        }
    }

    void Buffer::append(const std::string& message) {
        append(message.data(), message.size());
    }

    void Buffer::append(const char * source, std::size_t len) {
        ensurePlace(len);
        if(writeIndex_ > readIndex_ && writeIndex_ + len >= data_.size()) {
            auto firstPart = data_.size() - writeIndex_;
            std::copy(source, source + firstPart, &data_[writeIndex_]);
            std::copy(source+firstPart, source + len, &data_[0]);
        }
        else {
            std::copy(source, source+len, &data_[writeIndex_]);
        }
        writeIndex_ += len;
        writeIndex_ %= data_.capacity();
    }
    
    void Buffer::append(const void * source, std::size_t len) {
        append(static_cast<const char *>(source), len);
    }

    void Buffer::append(const Buffer& buffer) {
        append(buffer.peekAllAsString());
    }

    void Buffer::appendInt8(int8_t x) {
        append(static_cast<const void *>(&x), sizeof x);
    }
    void Buffer::appendInt16(int16_t x) {
        int16_t t = htobe16(x);
        append(static_cast<const void *>(&t), sizeof t);
    }
    void Buffer::appendInt32(int32_t x) {
        int32_t t = htobe32(x);
        append(static_cast<const void *>(&t), sizeof t);
    }
    void Buffer::appendInt64(int64_t x) {
        int64_t t = htobe64(x);
        append(static_cast<const void *>(&t), sizeof t);
    }
    void Buffer::prepend(const std::string& message) {
        prepend(message.data(), message.size());
    }
    void Buffer::prepend(const char * source, std::size_t len) {
        ensurePlace(len);
        if(writeIndex_ < readIndex_ || readIndex_ > len) {
            readIndex_ -= len;
            std::copy(source, source + len, &data_[readIndex_]);
        } else {
            auto firstPart = readIndex_;
            std::copy(source + len - firstPart, source + len, &data_[0]);
            readIndex_ = data_.size() + readIndex_ - len;
            std::copy(source, source + len - firstPart, &data_[readIndex_]);
        }
    }

    void Buffer::prepend(const void * source, std::size_t len) {
        append(static_cast<const char *>(source), len);
    }

    void Buffer::prepend(const Buffer& buffer) {
        prepend(buffer.peekAllAsString());
    }

    void Buffer::prependInt8(int8_t x) {
        prepend(static_cast<const void *>(&x), sizeof x);
    }

    void Buffer::prependInt16(int16_t x) {
        int16_t t = htobe16(x);
        prepend(static_cast<const void *>(&t), sizeof t);
    }

    void Buffer::prependInt32(int32_t x) {
        int32_t t = htobe32(x);
        prepend(static_cast<const void *>(&t), sizeof t);
    }

    void Buffer::prependInt64(int64_t x) {
        int64_t t = htobe64(x);
        prepend(static_cast<const void *>(&t), sizeof t);
    }  

    void Buffer::peek(char* dest, std::size_t len) const{
        assert(readableBytes() >= len);
        if(readIndex_ + len < data_.size()) {
            ::memcpy(dest, &data_[readIndex_], len);
        } else {
            auto firstPart = data_.size() - readIndex_;
            std::copy(&data_[readIndex_], &data_[readIndex_] + firstPart, dest);
            std::copy(&data_[0], &data_[0] + len - firstPart + 1, dest + firstPart);
        }
    }
    
    void Buffer::peek(void* dest, std::size_t len) const{
        peek(static_cast<char*>(dest), len);
    }

    std::string Buffer::peekAsString(std::size_t len) const {
        std::string res;
        if(writeIndex_ > readIndex_ || readIndex_ + len <= data_.size()) {
            res.assign(&data_[readIndex_], len);
        } else {
            res.assign(&data_[readIndex_], data_.size() - readIndex_);
            res.append(&data_[0], len - data_.size() + readIndex_);
        }
        return  res;
    }  
    std::string Buffer::peekAllAsString() const {
        return peekAsString(readableBytes());
    }
    int8_t Buffer::peekInt8() const {
        int8_t t;
        peek(static_cast<void*>(&t), sizeof t);
        return t;
    }              
    int16_t Buffer::peekInt16() const {
        int16_t t;
        peek(static_cast<void*>(&t), sizeof t);
        return be16toh(t);
    }
    int32_t Buffer::peekInt32() const {
        int32_t t;
        peek(static_cast<void*>(&t), sizeof t);
        return be32toh(t);
    }
    int64_t Buffer::peekInt64() const {
        int64_t t;
        peek(static_cast<void*>(&t), sizeof t);
        return be64toh(t);
    }
    int8_t Buffer::readInt8() {
        auto res = peekInt8();
        retrieve(sizeof res);
        return res;
    }
    int16_t Buffer::readInt16() {
        auto res = peekInt16();
        retrieve(sizeof res);
        return res;
    }
    int32_t Buffer::readInt32() {
        auto res = peekInt32();
        retrieve(sizeof res);
        return res;
    }
    int64_t Buffer::readInt64() {
        auto res = peekInt64();
        retrieve(sizeof res);
        return res;
    } 

    void Buffer::retrieve(size_t len) {
        readIndex_ += len;
        readIndex_ %= data_.capacity();
    }
    void Buffer::retrieveAll() {
        readIndex_ = writeIndex_ = 0;
    }
    void Buffer::retrieveUntil(const char* end) {
        auto index = end - &data_[0];
        retrieve(index + 1);
    }

    std::string Buffer::retrieveAsString(size_t len) {
        auto res = peekAsString(len);
        retrieve(len);
        return res;
    }
    std::string Buffer::retrieveAllAsString() {
        auto res = peekAllAsString();
        retrieveAll();
        return res;
    }

    const char* Buffer::findCRLF() const {
        const char* res;
        if(writeIndex_ > readIndex_) {
            res = std::search(&data_[readIndex_], &data_[writeIndex_], kCRLF, kCRLF+2);
        } else {
            res = std::search(&data_[readIndex_], &*data_.end(), kCRLF, kCRLF+2);
            if(res == &*data_.end()) {
                if(data_[data_.size() -1] == '\r' && data_[0] == '\n') {
                    res = &data_[0];
                } else {
                    res = std::search(&data_[0], &data_[writeIndex_], kCRLF, kCRLF + 2);
                    if(res == & data_[writeIndex_]) res = nullptr;
                }
            }
        }
        return res;
    }                   
    const char* Buffer::findCRLF(const char* start) const {
        const char* res;
        if(writeIndex_ > readIndex_ || start < &data_[readIndex_]) {
            res = std::search(start, &data_[writeIndex_], kCRLF, kCRLF + 2);
        } else {
            if(data_[data_.size() -1] == '\r' && data_[0] == '\n') {
                res = &data_[0];
            } else {
                res = std::search(start, &data_[writeIndex_], kCRLF, kCRLF + 2);
            }
        }
        return res == &data_[writeIndex_] ? nullptr : res;
    }  
    const char* Buffer::findEOL() const {
        const char* res;
        if(writeIndex_ > readIndex_) {
            res = std::find(&data_[readIndex_], &data_[writeIndex_], '\n');
        } else {
            res = std::find(&data_[readIndex_], &data_[data_.size() - 1], '\n');
            if(res == &data_[data_.size() - 1]) {
                res = std::find(&data_[0], &data_[writeIndex_], '\n');
            }
        }
        return res == &data_[writeIndex_] ? nullptr : res;
    }                    
    const char* Buffer::findEOL(const char* start) const {
        const char* res;
        if(writeIndex_ > readIndex_ || start < &data_[readIndex_]) {
            res = std::find(start, &data_[writeIndex_], '\n');
        } else {
            res = std::find(start, &data_[data_.size() - 1], '\n');
            if(res == &data_[data_.size() - 1]) {
                res = std::find(&data_[0], &data_[writeIndex_], '\n');
            }            
        }
        return res == &data_[writeIndex_] ? nullptr : res;
    }

    std::size_t Buffer::readFd(int fd, int& savedErrno) {

        char extrabuf[65536];
        struct iovec vec[3];        // 本身可以分为两段, 外加一段栈空间
        auto writable = writeableBytes();
        if(writeIndex_ >= readIndex_) {
            vec[0].iov_base = &data_[writeIndex_];
            vec[0].iov_len = writable - readIndex_;
            vec[1].iov_base = &data_[0];
            vec[1].iov_len = readIndex_;            
        } else {
            vec[0].iov_base = &data_[writeIndex_];
            vec[0].iov_len = writable;
            vec[1].iov_base = nullptr;
            vec[1].iov_len = 0;
        }
        vec[2].iov_base = extrabuf;
        vec[2].iov_len = sizeof extrabuf;

        auto n = readv(fd, vec, 3);
        if(n < -1) {
            savedErrno = errno;
        } else {
            if(n <= writable) {
                writeIndex_ += n;
                writeIndex_ %= data_.capacity();
            } else {
                writeIndex_ += writable;
                writeIndex_ %= data_.capacity();
                append(extrabuf, n - writable);
            }
        }
        return n;    
    }

    std::size_t Buffer::writeFd(int fd, int& savedErrno) {
        struct iovec vec[2];
        if(readIndex_ <= writeIndex_) {
            vec[0].iov_base = &data_[readIndex_];
            vec[0].iov_len = readableBytes();
            vec[1].iov_base = nullptr;
            vec[1].iov_len = 0;
        } else {
            vec[0].iov_base = &data_[readIndex_];
            vec[0].iov_len = readableBytes() - writeIndex_;
            vec[1].iov_base = &data_[0];
            vec[1].iov_len = writeIndex_;            
        }
        auto n = writev(fd, vec, 2);

        savedErrno = errno;


        return n;
    }

} // namespace gmuduo

