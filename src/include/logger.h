#pragma once
#include "lockqueue.h"
#include <cstdint>
#include <cstdio>
#include <atomic>
#include <memory>

enum LogLevel {
    INFO,
    ERROR,
};

struct LogFileDesDel {
    void operator()(FILE *pf) {
        if (pf != nullptr) {
            fclose(pf);
        }
    }
};

// Mprpc框架提供的日志系统
class Logger {
public:
    // 获取单例对象
    static Logger &GetInstance();
    // 设置日志级别
    void SetLogLevel(uint16_t log_level);
    // 获取日志级别
    uint16_t GetLogLevel();
    // 添加日志信息
    void Log(std::string msg);

private:
    Logger();
    Logger(const Logger &) = delete;
    Logger(const Logger &&) = delete;

private:
    std::atomic_uint16_t m_loglevel_;             // 日志级别
    std::unique_ptr<FILE, LogFileDesDel> m_file_; // 日志文件指针
    std::atomic_uint16_t m_today_;                // 记录日期
    LockQueue<std::string> m_lckque_; // 日志缓冲队列
};

#define LOG_INFO(logmsgformat, ...)                              \
    do {                                                         \
        Logger &logger = Logger::GetInstance();                  \
        logger.SetLogLevel(LogLevel::INFO);                      \
        char buf[1024] = {0};                                    \
        snprintf(buf, sizeof(buf), logmsgformat, ##__VA_ARGS__); \
        std::stringstream ss;                                    \
        ss << " [INFO] " << buf;                                 \
        logger.Log(ss.str());                                    \
    } while (0);

#define LOG_ERROR(logmsgformat, ...)                             \
    do {                                                         \
        Logger &logger = Logger::GetInstance();                  \
        logger.SetLogLevel(LogLevel::ERROR);                     \
        char buf[1024] = {0};                                    \
        snprintf(buf, sizeof(buf), logmsgformat, ##__VA_ARGS__); \
        std::stringstream ss;                                    \
        ss << " [ERROR] " << buf;                                \
        logger.Log(ss.str());                                    \
    } while (0);
