#ifndef __ZOE_LOGGERMANAGER_H__
#define __ZOE_LOGGERMANAGER_H__

#include <map>
#include <memory>
#include "Logger/logger.h"
#include "singleton.h"


namespace cczoe {
namespace logcpp {
class LoggerManager 
{
private:
    std::map<std::string, std::shared_ptr<Logger>> m_loggers;
    std::shared_ptr<Logger> m_root;

public:
    LoggerManager();
    
    std::shared_ptr<Logger> getRoot() const { return m_root; }
    std::shared_ptr<Logger> getLogger(const std::string &name);
};

typedef Singleton<LoggerManager> LoggerMgr;

}}

#endif