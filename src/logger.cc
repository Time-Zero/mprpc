#include "logger.h"
#include <cstdio>
#include <time.h>
#include <sstream>
#include <iostream>

Logger &Logger::GetInstance()
{
    static Logger logger;
    return logger;
}

void Logger::SetLogLevel(uint32_t log_level)
{
    std::lock_guard<std::mutex> lck(m_mtx_);
    m_loglevel_ = log_level;
}

void Logger::Log(std::string &msg)
{
    m_lckque_.Push(msg);
}

Logger::Logger() : m_loglevel_(LogLevel::INFO)
{
    // TODO:优化日志文件写入逻辑，使得不要频繁的打开文件
    std::thread writeLogTask([&](){
        for(;;)
        {
            // 获取当前的日期，然后取日志信息，写入相应的日志文件中
            time_t now = time(nullptr);
            tm* now_tm = localtime(&now);

            std::stringstream ss;
            ss << now_tm->tm_year + 1900 << "-" << now_tm->tm_mon + 1 << "-" << now_tm->tm_mday << "-log.txt";

            FILE *pf = fopen(ss.str().c_str(), "a+");
            if(pf == nullptr)
            {
                std::cout << "logger file: " << ss.str() << " open error!" << std::endl;
                exit(EXIT_FAILURE);
            }

            std::string msg = m_lckque_.Pop();
            std::stringstream ss_log;
            ss_log << now_tm->tm_hour << ":" << now_tm->tm_min << ":" << now_tm->tm_sec << " => " << msg;

            fputs(ss_log.str().c_str(), pf);
            fclose(pf);
        }
    });


    writeLogTask.detach();
}
