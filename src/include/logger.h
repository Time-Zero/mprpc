#pragma once
#include "lockqueue.h"
#include <cstdint>
#include <cstdio>
#include <mutex>
#include <sstream>

enum LogLevel {
  INFO,
  ERROR,
};

// Mprpc框架提供的日志系统
class Logger {
public:
  // 获取单例对象
  static Logger &GetInstance();
  // 设置日志级别
  void SetLogLevel(uint32_t log_level);
  // 添加日志信息
  void Log(std::string msg);

private:
  Logger();
  Logger(const Logger &) = delete;
  Logger(const Logger &&) = delete;

private:
  uint32_t m_loglevel_; // 日志级别
  std::mutex m_mtx_;
  LockQueue<std::string> m_lckque_; // 日志缓冲队列
};

#define LOG_INFO(logmsgformat, ...)                                            \
  do {                                                                         \
    Logger &logger = Logger::GetInstance();                                    \
    logger.SetLogLevel(LogLevel::INFO);                                        \
    char buf[1024] = {0};                                                      \
    snprintf(buf, sizeof(buf), logmsgformat, ##__VA_ARGS__);                   \
    std::stringstream ss;                                                      \
    ss << " [INFO] " << buf;                                                   \
    logger.Log(ss.str());                                                      \
  } while (0);

#define LOG_ERROR(logmsgformat, ...)                                           \
  do {                                                                         \
    Logger &logger = Logger::GetInstance();                                    \
    logger.SetLogLevel(LogLevel::ERROR);                                       \
    char buf[1024] = {0};                                                      \
    snprintf(buf, sizeof(buf), logmsgformat, ##__VA_ARGS__);                   \
    std::stringstream ss;                                                      \
    ss << " [ERROR] " << buf;                                                  \
    logger.Log(ss.str());                                                      \
  } while (0);
