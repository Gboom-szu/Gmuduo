#ifndef GMUDUO_SRC_LOG_H
#define GMUDUO_SRC_LOG_H

#include <iostream>
#include <string>
#include <spdlog/spdlog.h>
#include <atomic>
#include "type.h"


namespace gmuduo{
    class MyLogger: public noncopyable {
    public:
        static MyLogger& instance();
        void init(std::string filename = "logs/log.txt", 
            std::string loggername = "Mylogger", 
            spdlog::level::level_enum level = spdlog::level::info);
        void onlyToConsole();
        void onlyToFile();
    private:
        MyLogger();
        std::atomic_bool isIninted;
        bool writeToFile;
        bool writeToConsole;
    };

    #define LOGGER(...) MyLogger::instance().init(__VA_ARGS__);
    #define LOGGER_WITH_NAME(name) MyLogger::instance().init("logs/log.txt", name);
    #define ONLY_TO_CONSOLE MyLogger::instance().onlyToConsole();
    #define ONLY_TO_FILE MyLogger::instance().onlyToFile();

    // 如果没有先调用LOGGER()，将会使用默认logger
    #define LOG_TRACE(...) SPDLOG_TRACE(__VA_ARGS__)
    #define LOG_DEBUG(...) SPDLOG_DEBUG(__VA_ARGS__)
    #define LOG_INFO(...) SPDLOG_INFO(__VA_ARGS__)
    #define LOG_WARN(...) SPDLOG_WARN(__VA_ARGS__)
    #define LOG_ERROR(...) {SPDLOG_ERROR(__VA_ARGS__); abort();}
    #define LOG_CRIRICAL(...) {SPDLOG_CRITICAL(__VA_ARGS__); abort();}
}
#endif