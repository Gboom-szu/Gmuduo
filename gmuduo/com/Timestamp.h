#ifndef GMUDUO_COM_TIMESTAMP_H
#define GMUDUO_COM_TIMESTAMP_H


#include <string>
#include "type.h"


namespace gmuduo
{
    class Timestamp {
    private:
        uint64_t microSecondsSinceEpoch_;    // us from epoch。64位字长，足够了。
    public:
        Timestamp();        // 现在的时间点
        explicit Timestamp(uint64_t microSecondsSinceEpoch) : microSecondsSinceEpoch_(microSecondsSinceEpoch){}
        uint64_t microSecondsSinceEpoch() const {return microSecondsSinceEpoch_;}
        // std::string tostring();     // 字符表示日期
    };

    inline bool operator <(const Timestamp& lts, const Timestamp& rts) {
        return lts.microSecondsSinceEpoch() < rts.microSecondsSinceEpoch();
    }
    inline bool operator ==(const Timestamp& lts, const Timestamp& rts) {
        return lts.microSecondsSinceEpoch() == rts.microSecondsSinceEpoch();
    }
    inline bool operator >(const Timestamp& lts, const Timestamp& rts) {
        return lts.microSecondsSinceEpoch() > rts.microSecondsSinceEpoch();
    }
} // namespace gmuduo




#endif