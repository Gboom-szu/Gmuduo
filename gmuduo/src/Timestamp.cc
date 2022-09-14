#include <chrono>
#include "Timestamp.h"



namespace gmuduo
{
    Timestamp::Timestamp() {
        microSecondsSinceEpoch_ =  std::chrono::duration_cast<std::chrono::microseconds>( std::chrono::steady_clock::now().time_since_epoch()).count();
    }
    
} // namespace gmuduo
