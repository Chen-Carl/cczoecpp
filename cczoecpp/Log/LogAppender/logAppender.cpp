#include <iostream>
#include "logAppender.h"

namespace cczoe {
namespace logcpp {

LogAppender::LogAppender() : 
    m_hasFormatter(false) 
{ }

void LogAppender::setFormatter(std::shared_ptr<LogFormatter> fmtter)
{
    thread::ScopedLock<thread::Mutex> lock(m_mutex);
    m_formatter = fmtter;
    if (m_formatter)
        m_hasFormatter = true;
    else
        m_hasFormatter = false;
}

StdoutLogAppender::StdoutLogAppender(std::shared_ptr<LogFormatter> fmtter)
{
    m_formatter = fmtter;
}

void StdoutLogAppender::log(std::shared_ptr<Logger> logger, LogLevel::Level level, std::shared_ptr<LogEvent> event)
{
    thread::ScopedLock<thread::Mutex> lock(m_mutex);
    if (m_hasFormatter && level >= m_level)
    {
        std::cout << m_formatter->format(event);
    }
    else if (!m_hasFormatter)
    {
        if (logger->hasFormatter())
        {
            std::cout << logger->getLogFormatter()->format(event);
        }
    }
}

bool FileLogAppender::reopen()
{
    thread::ScopedLock<thread::Mutex> lock(m_mutex);
    if (m_filestream)
    {
        m_filestream.close();
    }
    m_filestream.open(m_filename, std::ios::app);
    return !!m_filestream;
}

FileLogAppender::FileLogAppender(const std::string &filename) :
    m_filename(filename)
{
    reopen();
}

void FileLogAppender::log(std::shared_ptr<Logger> logger, LogLevel::Level level, std::shared_ptr<LogEvent> event)
{
    thread::ScopedLock<thread::Mutex> lock(m_mutex);
    if (m_hasFormatter && level >= m_level)
    {
        m_filestream << m_formatter->format(event);
    }
    else if (!m_hasFormatter)
    {
        
    }
}


}}