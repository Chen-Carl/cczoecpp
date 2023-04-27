#ifndef __CCZOE_LOG_EVENT_H__
#define __CCZOE_LOG_EVENT_H__

#include <string>
#include <sstream>
#include "Logger/logger.h"

namespace cczoe {
namespace logcpp {
class LogEvent
{
private:
    std::string m_threadName;       // thread name
    const char *m_file = nullptr;   // file name
    int32_t m_line = 0;             // line number
    uint32_t m_elapse = 0;          // running time
    uint32_t m_tid = 0;             // thread id
    uint32_t m_fid = 0;             // fiber id
    uint64_t m_time = 0;            // current time
    std::stringstream m_ss;         // log content

    std::shared_ptr<Logger> m_logger;
    LogLevel::Level m_level;

public:
    LogEvent(const std::string threadName, const char *file, int32_t line, uint32_t elapse, uint32_t tid, uint32_t fid, uint64_t time, std::shared_ptr<Logger> logger, LogLevel::Level level);

    const std::string &getThreadName() const { return m_threadName; }
    const char *getFile() const { return m_file; }
    int32_t getLine() const { return m_line; }
    uint32_t getElapse() const { return m_elapse; }
    uint32_t getTid() const { return m_tid; }
    uint32_t getFid() const { return m_fid; }
    uint64_t getTime() const { return m_time; }
    const std::string getContent() const { return m_ss.str(); }
    const LogLevel::Level getLevel() const { return m_level; }
    const std::shared_ptr<Logger> getLogger() const { return m_logger; }
    // stream out
    std::stringstream &getSS() { return m_ss; }
    // format out
    void format(const char *fmt, ...);
    void format(const char *fmt, va_list al);
};

class LogEventWrap
{
private:
    std::shared_ptr<LogEvent> m_event;

public:
    LogEventWrap(std::shared_ptr<LogEvent> event);
    ~LogEventWrap();

    std::shared_ptr<LogEvent> getEvent() const { return m_event; }
};

}}

#endif