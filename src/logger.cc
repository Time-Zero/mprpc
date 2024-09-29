#include "logger.h"
#include <cstdio>
#include <pthread.h>
#include <time.h>
#include <sstream>
#include <iostream>
#include <thread>

Logger &Logger::GetInstance()
{
    static Logger logger;
    return logger;
}

void Logger::SetLogLevel(uint16_t log_level)
{
    m_loglevel_.store(log_level);
}

void Logger::Log(std::string msg)
{
    m_lckque_.Push(msg);
}

Logger::Logger() : m_loglevel_(LogLevel::INFO), m_file_(nullptr), m_today_(0)
{
    // 启用一个线程来负责日志的写入
    std::thread writeLogTask([&](){
        for(;;)
        {
            // 获取当前的日期，然后取日志信息，写入相应的日志文件中
            time_t now = time(nullptr);
            tm* now_tm = localtime(&now);

            //  如果没有日志文件句柄或者当前日期和日志日期不一致
            if(m_file_.get() == nullptr || m_today_.load() != now_tm->tm_mday)
            {
                m_today_.store(now_tm->tm_mday);
                std::stringstream ss;
                ss << now_tm->tm_year + 1900 << "-" << now_tm->tm_mon + 1 << "-" << now_tm->tm_mday << "-log.log";

                m_file_.reset(fopen(ss.str().c_str(), "a+"));
                if(m_file_.get() == nullptr)
                {
                    std::cout << "logger file: " << ss.str() << " open error!" << std::endl;
                    exit(EXIT_FAILURE);
                }
            }

            std::string msg = m_lckque_.Pop();
            std::stringstream ss_log;
            ss_log << now_tm->tm_year + 1900 << ":" << now_tm->tm_hour << ":" << now_tm->tm_min << ":" 
            << now_tm->tm_sec << " => " << msg << std::endl;

            fputs(ss_log.str().c_str(), m_file_.get());
            fflush(m_file_.get());
        }
    });


    writeLogTask.detach();
}

uint16_t Logger::GetLogLevel()
{
    return m_loglevel_.load();
}