
#include <iostream>
#include <string>


namespace gmuduo{
    #define LOG_INFO(message) {std::cout << (message) << std::endl;}
    #define LOG_FATAL(message) {std::cout << (message) << std::endl; exit(-1);}
}
