#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include "Net/Hook/fdmanager.h"
#include "Net/Hook/hook.h"

namespace cczoe {

FdCtx::FdCtx(int fd) : 
    m_fd(fd)
{
    struct stat fdStat;
    if (fstat(m_fd, &fdStat) == 0)
    {
        m_isInit = true;
        m_isSocket = S_ISSOCK(fdStat.st_mode);
        if (m_isSocket)
        {
            int flags = fcntl_f(m_fd, F_GETFL, 0);
            if (!(flags & O_NONBLOCK))
            {
                fcntl_f(m_fd, F_SETFL, flags | O_NONBLOCK);
            }
            m_sysNonblock = true;
        }
    }
}

FdCtx::~FdCtx()
{

}

FdManager::FdManager()
{
    m_data.resize(64);
}

std::shared_ptr<FdCtx> FdManager::set(int fd)
{
    thread::WriteScopedLock<RWMutexType> lock(m_mutex);
    std::shared_ptr<FdCtx> ctx = std::make_shared<FdCtx>(fd);
    if(static_cast<size_t>(fd) >= m_data.size()) {
        m_data.resize(fd * 1.5);
    }
    m_data[fd] = ctx;
    return ctx;
}

std::shared_ptr<FdCtx> FdManager::get(int fd, bool autoCreate)
{
    thread::ReadScopedLock<RWMutexType> lock(m_mutex);
    if (static_cast<size_t>(fd) >= m_data.size() && !autoCreate)
    {
        return nullptr;
    }
    else if (m_data[fd] || !autoCreate)
    {
        return m_data[fd];
    }
    lock.unlock();

    return set(fd);
}

void FdManager::del(int fd)
{
    thread::WriteScopedLock<RWMutexType> lock(m_mutex);
    if (static_cast<size_t>(fd) < m_data.size())
    {
        m_data[fd].reset();
    }
}

}