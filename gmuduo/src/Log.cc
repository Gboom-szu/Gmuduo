#include "Log.h"
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/async_logger.h>
#include <iostream>
#include <vector>


namespace gmuduo
{
    MyLogger::MyLogger() {
        isIninted = false;
        writeToConsole = true;
        writeToFile = true;
    }

    MyLogger& MyLogger::instance() {
        static MyLogger logger;
        return logger;
    }
    void MyLogger::init(std::string filename, std::string loggername, spdlog::level::level_enum level) {
        if(isIninted) return;     // 已经初始化
        try
        {
            std::vector<spdlog::sink_ptr> sinks;
            if(writeToConsole)  
                sinks.emplace_back(std::make_shared<spdlog::sinks::stderr_color_sink_mt>());
            if(writeToFile)
                sinks.push_back(std::make_shared<spdlog::sinks::rotating_file_sink_mt>(filename, 1024*1024*8, 3));
            auto logger = std::make_shared<spdlog::logger>(loggername, sinks.begin(), sinks.end());
            spdlog::set_default_logger(logger);
            spdlog::set_pattern("%Y-%m-%d %H:%M:%S:%e [%l] [tid %t] [<%s>:<%#>:<%!>] %v");              // 设置logger格式
            spdlog::set_level(level);
            spdlog::flush_on(spdlog::level::warn);
            spdlog::flush_every(std::chrono::seconds(3));
        }
        catch(const spdlog::spdlog_ex& e)
        {
            std::cerr << e.what() << std::endl;
            std::cerr << "default logger will be used" << std::endl;
        }
        
    }

    void MyLogger::onlyToConsole() {
        writeToConsole = true;
        writeToFile = false;
    }
    void MyLogger::onlyToFile() {
        writeToConsole = false;
        writeToFile = true;
    }
    
} // namespace gmuduo
