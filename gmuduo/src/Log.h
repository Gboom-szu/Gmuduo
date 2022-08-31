
#include <iostream>
#include <string>


namespace gmuduo{
    #define LOG_INFO(message) {std::cout << "LOG_INFO: " << (message) << std::endl;}
    #define LOG_FATAL(message) {std::cout << "LOG_FATAL: " << (message) << std::endl; exit(-1);}
}
