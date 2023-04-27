#include "logger.h"
#include "LogAppender/logAppender.h"

namespace cczoe {
namespace logcpp {

const char *LogLevel::toString(LogLevel::Level level)
{
    switch (level)
    {
    case LogLevel::DEBUG:
        return "DEBUG";
        break;
    case LogLevel::INFO:
        return "INFO";
        break;
    case LogLevel::WARN:
        return "WARN";
        break;
    case LogLevel::ERROR:
        return "ERROR";
        break;
    case LogLevel::FATAL:
        return "FATAL";
        break;
    default:
        return "UNKNOWN";
    }
    return "UNKNOWN";
}

LogLevel::Level LogLevel::fromString(const std::string &str)
{
    #define XX(level, s) \
        if (str == #s) \
        { \
            return LogLevel::level; \
        }

        XX(DEBUG, debug);
        XX(INFO, info);
        XX(WARN, warn);
        XX(ERROR, error);
        XX(FATAL, fatal);
        
        XX(DEBUG, DEBUG);
        XX(INFO, INFO);
        XX(WARN, WARN);
        XX(ERROR, ERROR);
        XX(FATAL, FATAL);
    #undef XX
    return LogLevel::UNKNOWN;
}

Logger::Logger(const std::string &name) :
    m_name(name)
{
    m_level = LogLevel::DEBUG;
    m_formatter = std::make_shared<LogFormatter>("%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n");
}

void Logger::addAppender(std::shared_ptr<LogAppender> appender)
{
    thread::ScopedLock<thread::Mutex> lock(m_mutex);
    m_appenders.push_back(appender);
}

void Logger::delAppender(std::shared_ptr<LogAppender> appender)
{
    thread::ScopedLock<thread::Mutex> lock(m_mutex);
    for (auto it = m_appenders.begin(); it != m_appenders.end(); it++)
    {
        if (*it == appender)
        {
            m_appenders.erase(it);
            break;
        }
    }
}

void Logger::clearAppender()
{
    thread::ScopedLock<thread::Mutex> lock(m_mutex);
    m_appenders.clear();
}

void Logger::log(LogLevel::Level level, std::shared_ptr<LogEvent> event)
{
    if (level >= m_level)
    {
        auto self = shared_from_this();
        if (!m_appenders.empty())
        {
            for (auto appender : m_appenders)
            {
                appender->log(self, level, event);
            }
        }
        // if m_appenders is empty, root logger will output the result
        else if (m_root)
        {
            m_root->log(level, event);
        }

    }
}

void Logger::debug(std::shared_ptr<LogEvent> event)
{
    log(LogLevel::DEBUG, event);
}

void Logger::info(std::shared_ptr<LogEvent> event)
{
    log(LogLevel::INFO, event);
}

void Logger::warn(std::shared_ptr<LogEvent> event)
{
    log(LogLevel::WARN, event);
}

void Logger::error(std::shared_ptr<LogEvent> event)
{
    log(LogLevel::ERROR, event);
}

void Logger::fatal(std::shared_ptr<LogEvent> event)
{
    log(LogLevel::FATAL, event);
}

}
}