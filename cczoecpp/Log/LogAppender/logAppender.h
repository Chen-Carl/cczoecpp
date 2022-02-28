#ifndef __ZOE_LOG_APPENDER_H__
#define __ZOE_LOG_APPENDER_H__

#include <fstream>
#include "Logger/logger.h"
#include "LogEvent/logEvent.h"
#include "LogFormatter/logFormatter.h"

namespace cczoe {
namespace logcpp {

class LogAppender 
{
protected:
    LogLevel::Level m_level = LogLevel::DEBUG;
    std::shared_ptr<LogFormatter> m_formatter;
    bool m_hasFormatter = false;

public:
    LogAppender();
    LogAppender(std::shared_ptr<LogFormatter> fmtter);
    virtual ~LogAppender() { }

    virtual void setFormatter(std::shared_ptr<LogFormatter> fmtter);
    void setLevel(LogLevel::Level level) { m_level = level; }

    virtual void log(std::shared_ptr<Logger> logger, LogLevel::Level level, std::shared_ptr<LogEvent> event) = 0;
};

class StdoutLogAppender : public LogAppender
{
public:
    StdoutLogAppender() { }
    StdoutLogAppender(std::shared_ptr<LogFormatter> fmtter);
    virtual ~StdoutLogAppender() { }
    virtual void log(std::shared_ptr<Logger> logger, LogLevel::Level level, std::shared_ptr<LogEvent> event);

};

class FileLogAppender : public LogAppender
{
private:
    std::string m_filename;
    std::ofstream m_filestream;

    bool reopen();

public:
    FileLogAppender(const std::string &filename);

    virtual void log(std::shared_ptr<Logger> logger, LogLevel::Level level, std::shared_ptr<LogEvent> event);
};

}}

#endif