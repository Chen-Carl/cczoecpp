#ifndef __CCZOE_HOOK_FDMANAGER_H__
#define __CCZOE_HOOK_FDMANAGER_H__

#include <sys/socket.h>
#include <memory>
#include <vector>
#include "Scheduler/iomanager.h"
#include "Thread/Mutex/mutex.h"

namespace cczoe {

class FdCtx
{
private:
    bool m_isInit : 1 = false;          // if the fd is initialized
    bool m_isSocket : 1 = false;        // if the fd is a socket
    bool m_sysNonblock : 1 = false;     // if the fd is nonblock in system
    bool m_userNonblock : 1 = false;    // if the fd is nonblock in user
    bool m_isClosed : 1 = false;        // if the fd is closed
    int m_fd;

    // socket timeout, max means no timeout
    uint64_t m_recvTimeout = std::numeric_limits<uint64_t>::max();
    uint64_t m_sendTimeout = std::numeric_limits<uint64_t>::max();

public:
    FdCtx(int fd);
    ~FdCtx();

    bool isInit() const { return m_isInit; }
    bool isSocket() const { return m_isSocket; }
    bool isClosed() const { return m_isClosed; }
    bool isUserNonblock() const { return m_userNonblock; }
    bool isSysNonblock() const { return m_sysNonblock; }
    int getFd() const { return m_fd; }
    uint64_t getTimeout(int type) const { return type == SO_RCVTIMEO ? m_recvTimeout : m_sendTimeout; }

    void setUserNonblock(bool v) { m_userNonblock = v; }
    void setSysNonblock(bool v) { m_sysNonblock = v; }
    void setTimeout(int type, uint64_t val) { type == SO_RCVTIMEO ? m_recvTimeout = val : m_sendTimeout = val; }
};

class FdManager
{
    using RWMutexType = thread::RWMutex;
private:
    RWMutexType m_mutex;
    std::vector<std::shared_ptr<FdCtx>> m_data;
public:
    FdManager();

    std::shared_ptr<FdCtx> set(int fd);
    std::shared_ptr<FdCtx> get(int fd, bool autoCreate = false);
    void del(int fd);
};

typedef Singleton<FdManager> FdMgr;

}

#endif