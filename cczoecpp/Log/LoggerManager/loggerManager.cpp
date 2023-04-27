#include "loggerManager.h"
#include "LogAppender/logAppender.h"

namespace cczoe {
namespace logcpp {

LoggerManager::LoggerManager()
{
    m_root = std::make_shared<Logger>();
    m_root->addAppender(std::make_shared<StdoutLogAppender>());
    m_loggers[m_root->getName()] = m_root;
}

std::shared_ptr<Logger> LoggerManager::getLogger(const std::string &name)
{
    thread::ScopedLock<thread::Mutex> lock(m_mutex);
    auto it = m_loggers.find(name);
    if (it != m_loggers.end())
        return it->second;
    // if not exist, create a new logger
    std::shared_ptr<Logger> logger = std::make_shared<Logger>(name);
    logger->m_root = m_root;
    m_loggers[name] = logger;
    return logger;
}


}}