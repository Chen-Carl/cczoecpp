#ifndef __ZOE_LOG_H__
#define __ZOE_LOG_H__

#include "LogAppender/logAppender.h"
#include "LogEvent/logEvent.h"
#include "Logger/logger.h"
#include "LogFormatter/logFormatter.h"
#include "LoggerManager/loggerManager.h"
#include "Thread/thread.h"
#include "getInfo.h"

#define CCZOE_LOG_ROOT() cczoe::logcpp::LoggerMgr::getInstance()->getRoot()
#define CCZOE_LOG_NAME(name) cczoe::logcpp::LoggerMgr::getInstance()->getLogger(name)

// stream out
#define CCZOE_LOG_LEVEL(logger, level) \
    if (level >= logger->getLevel()) \
        cczoe::logcpp::LogEventWrap(std::shared_ptr<cczoe::logcpp::LogEvent>(new cczoe::logcpp::LogEvent( \
            cczoe::thread::Thread::GetName(), __FILE__, __LINE__, 0, cczoe::getThreadId(), cczoe::getFiberId(), time(0), \
            logger, level \
        ))).getEvent()->getSS()

#define CCZOE_LOG_FATAL(logger) CCZOE_LOG_LEVEL(logger, cczoe::logcpp::LogLevel::FATAL)
#define CCZOE_LOG_ERROR(logger) CCZOE_LOG_LEVEL(logger, cczoe::logcpp::LogLevel::ERROR)
#define CCZOE_LOG_WARN(logger) CCZOE_LOG_LEVEL(logger, cczoe::logcpp::LogLevel::WARN)
#define CCZOE_LOG_INFO(logger) CCZOE_LOG_LEVEL(logger, cczoe::logcpp::LogLevel::INFO)
#define CCZOE_LOG_DEBUG(logger) CCZOE_LOG_LEVEL(logger, cczoe::logcpp::LogLevel::DEBUG)

// format out
#define CCZOE_LOG_FMT_LEVEL(logger, level, fmt, ...) \
    if (level <= logger->getLevel()) \
    cczoe::logcpp::LogEventWrap(std::shared_ptr<cczoe::logcpp::LogEvent>(new cczoe::logcpp::LogEvent( \
        cczoe::thread::Thread::GetName(), __FILE__, __LINE__, 0, getThreadId(), getFiberId(), time(0), \
        logger, level \
    ))).getEvent()->format(fmt, __VA_ARGS__)

#define CCZOE_LOG_FMT_DEBUG(logger, fmt, ...) CCZOE_LOG_FMT_LEVEL(logger, LogLevel::DEBUG, fmt, __VA_ARGS__)
#define CCZOE_LOG_FMT_INFO(logger, fmt, ...) CCZOE_LOG_FMT_LEVEL(logger, LogLevel::INFO, fmt, __VA_ARGS__)
#define CCZOE_LOG_FMT_WARN(logger, fmt, ...) CCZOE_LOG_FMT_LEVEL(logger, LogLevel::WARN, fmt, __VA_ARGS__)
#define CCZOE_LOG_FMT_ERROR(logger, fmt, ...) CCZOE_LOG_FMT_LEVEL(logger, LogLevel::ERROR, fmt, __VA_ARGS__)
#define CCZOE_LOG_FMT_FATAL(logger, fmt, ...) CCZOE_LOG_FMT_LEVEL(logger, LogLevel::FATAL, fmt, __VA_ARGS__)

#endif