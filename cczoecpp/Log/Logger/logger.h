#ifndef __ZOE_LOGGER_H__
#define __ZOE_LOGGER_H__

#include <list>
#include <string>
#include <memory>

namespace cczoe {
namespace logcpp {

class LogAppender;
class LogEvent;
class LogFormatter;

class LogLevel
{
public:
    enum Level
    {
        UNKNOWN = 0,
        DEBUG = 1,
        INFO = 2,
        WARN = 3,
        ERROR = 4,
        FATAL = 5
    };

    static const char *toString(LogLevel::Level level);
    static LogLevel::Level fromString(const std::string &str);
};

class Logger : public std::enable_shared_from_this<Logger>
{
    friend class LoggerManager;
private:
    std::string m_name;
    LogLevel::Level m_level;
    std::list<std::shared_ptr<LogAppender>> m_appenders;
    std::shared_ptr<LogFormatter> m_formatter;
    std::shared_ptr<Logger> m_root;

public:
    Logger(const std::string &name = "root");

    const std::string getName() const { return m_name; }
    const LogLevel::Level getLevel() const { return m_level; }
    const std::shared_ptr<LogFormatter> getLogFormatter() const { return m_formatter; }
    bool hasFormatter() const { return !!m_formatter; }

    void setLevel(LogLevel::Level level) { m_level = level; }
    void addAppender(std::shared_ptr<LogAppender> appender);
    void delAppender(std::shared_ptr<LogAppender> appender);
    void clearAppender();

    void log(LogLevel::Level level, std::shared_ptr<LogEvent> event);
    void debug(std::shared_ptr<LogEvent> event);
    void info(std::shared_ptr<LogEvent> event);
    void warn(std::shared_ptr<LogEvent> event);
    void error(std::shared_ptr<LogEvent> event);
    void fatal(std::shared_ptr<LogEvent> event);
};

}}

#endif