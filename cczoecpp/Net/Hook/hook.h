#ifndef __NET_HOOK_HOOK_H__
#define __NET_HOOK_HOOK_H__

#include <unistd.h>
#include <fcntl.h>
#include <cstdint>
#include "Net/Hook/fdmanager.h"
#include "Scheduler/iomanager.h"

namespace cczoe {

bool isHookEnable();
void setHookEnable(bool enable = true);
void hookInit();

template <typename OriginFun, typename... Args>
static ssize_t do_io(int fd, OriginFun fun, const char* hookName, uint32_t event, int soTimeout, Args&&... args) 
{
    struct TimeInfo
    {
        int cancelled = 0;
    };

    if (!cczoe::isHookEnable())
    {
        return fun(fd, std::forward<Args>(args)...);
    }

    std::shared_ptr<FdCtx> ctx = FdMgr::GetInstance()->get(fd);
    if (!ctx) 
    {
        return fun(fd, std::forward<Args>(args)...);
    }

    if (ctx->isClosed()) 
    {
        // Bad file number
        errno = EBADF;
        return -1;
    }
    // non-socket or user sets nonblock
    // if user sets nonblock, the original function handles it in nonblock mode
    if (!ctx->isSocket() || ctx->isUserNonblock()) 
    {
        return fun(fd, std::forward<Args>(args)...);
    }

    uint64_t timeout = ctx->getTimeout(soTimeout);
    std::shared_ptr<TimeInfo> timeInfo = std::make_shared<TimeInfo>();

retry:
    // TODO: Broken pipe
    ssize_t n = fun(fd, std::forward<Args>(args)...);
    while (n == -1 && errno == EINTR) 
    {
        n = fun(fd, std::forward<Args>(args)...);
    }
    if (n == -1 && errno == EAGAIN) 
    {
        IOManager* iom = IOManager::GetThis();
        std::shared_ptr<Timer> timer;
        std::weak_ptr<TimeInfo> cancel(timeInfo);
        if (timeout != std::numeric_limits<uint64_t>::max()) 
        {
            // TODO: [=] or [&] will trigger a segmentation fault
            timer = iom->addTimer(timeout, [cancel, fd, iom, event]() {
                auto t = cancel.lock();
                if (t && !t->cancelled) 
                {
                    t->cancelled = ETIMEDOUT;
                    iom->cancelEvent(fd, (IOManager::Event)(event));
                }
            }, cancel);
        }
        // there are 2 wakeup point:
        // 1. the fd is ready
        // 2. the timer is timeout

        // the callback is set to default (nullptr) in addEvent
        // so the fiber will be added to ready list
        if (iom->addEvent(fd, (IOManager::Event)(event)))
        {
            CCZOE_LOG_ERROR(CCZOE_LOG_ROOT()) << std::format("{} addEvent({}, {}) error", hookName, fd, event);
            if (timer) 
            {
                timer->cancel();
            }
            return -1;
        } 
        else 
        {
            fiber::Fiber::GetThis()->yield();
            if(timer) 
            {
                timer->cancel();
            }
            if(timeInfo->cancelled) 
            {
                errno = timeInfo->cancelled;
                return -1;
            }
            goto retry;
        }
    }
    
    return n;
}

}

extern "C" {

// sleep

typedef unsigned int (*sleep_fun)(unsigned int seconds);
extern sleep_fun sleep_f;

typedef int (*usleep_fun)(useconds_t usec);
extern usleep_fun usleep_f;

typedef int (*nanosleep_fun)(const struct timespec *req, struct timespec *rem);
extern nanosleep_fun nanosleep_f;

// socket

typedef int (*socket_fun)(int domain, int type, int protocol);
extern socket_fun socket_f;

typedef int (*connect_fun)(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
extern connect_fun connect_f;

int connectWithTimeout(int fd, const struct sockaddr *addr, socklen_t addrlen, uint64_t timeout);

typedef int (*accept_fun)(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
extern accept_fun accept_f;

// read

typedef ssize_t (*read_fun)(int fd, void *buf, size_t count);
extern read_fun read_f;

typedef ssize_t (*readv_fun)(int fd, const struct iovec *iov, int iovcnt);
extern readv_fun readv_f;

typedef ssize_t (*recv_fun)(int sockfd, void *buf, size_t len, int flags);
extern recv_fun recv_f;

typedef ssize_t (*recvfrom_fun)(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen);
extern recvfrom_fun recvfrom_f;

typedef ssize_t (*recvmsg_fun)(int sockfd, struct msghdr *msg, int flags);
extern recvmsg_fun recvmsg_f;

// write

typedef ssize_t (*write_fun)(int fd, const void *buf, size_t count);
extern write_fun write_f;

typedef ssize_t (*writev_fun)(int fd, const struct iovec *iov, int iovcnt);
extern writev_fun writev_f;

typedef ssize_t (*send_fun)(int sockfd, const void *buf, size_t len, int flags);
extern send_fun send_f;

typedef ssize_t (*sendto_fun)(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen);
extern sendto_fun sendto_f;

typedef ssize_t (*sendmsg_fun)(int sockfd, const struct msghdr *msg, int flags);
extern sendmsg_fun sendmsg_f;

// close

typedef int (*close_fun)(int fd);
extern close_fun close_f;

// control

typedef int (*fcntl_fun)(int fd, int cmd, ... /* arg */ );
extern fcntl_fun fcntl_f;

typedef int (*ioctl_fun)(int fd, unsigned long request, ...);
extern ioctl_fun ioctl_f;

typedef int (*getsockopt_fun)(int sockfd, int level, int optname, void *optval, socklen_t *optlen);
extern getsockopt_fun getsockopt_f;

typedef int (*setsockopt_fun)(int sockfd, int level, int optname, const void *optval, socklen_t optlen);
extern setsockopt_fun setsockopt_f;

}

#endif