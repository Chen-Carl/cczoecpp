#include <format>
#include <dlfcn.h>
#include <stdarg.h>
#include <sys/ioctl.h>
#include "hook.h"
#include "Fiber/fiber.h"
#include "Scheduler/iomanager.h"
#include "Net/Hook/fdmanager.h"

namespace cczoe {

static thread_local bool t_hookEnable = false;

bool isHookEnable()
{
    return t_hookEnable;
}

void setHookEnable(bool enable)
{
    t_hookEnable = enable;
}

void hookInit()
{
    static bool isInit = false;
    if (isInit)
    {
        return;
    }
    sleep_f = (sleep_fun)dlsym(RTLD_NEXT, "sleep");
    usleep_f = (usleep_fun)dlsym(RTLD_NEXT, "usleep");
    nanosleep_f = (nanosleep_fun)dlsym(RTLD_NEXT, "nanosleep");
    socket_f = (socket_fun)dlsym(RTLD_NEXT, "socket");
    connect_f = (connect_fun)dlsym(RTLD_NEXT, "connect");
    accept_f = (accept_fun)dlsym(RTLD_NEXT, "accept");
    read_f = (read_fun)dlsym(RTLD_NEXT, "read");
    readv_f = (readv_fun)dlsym(RTLD_NEXT, "readv");
    recv_f = (recv_fun)dlsym(RTLD_NEXT, "recv");
    recvfrom_f = (recvfrom_fun)dlsym(RTLD_NEXT, "recvfrom");
    recvmsg_f = (recvmsg_fun)dlsym(RTLD_NEXT, "recvmsg");
    write_f = (write_fun)dlsym(RTLD_NEXT, "write");
    writev_f = (writev_fun)dlsym(RTLD_NEXT, "writev");
    send_f = (send_fun)dlsym(RTLD_NEXT, "send");
    sendto_f = (sendto_fun)dlsym(RTLD_NEXT, "sendto");
    sendmsg_f = (sendmsg_fun)dlsym(RTLD_NEXT, "sendmsg");
    close_f = (close_fun)dlsym(RTLD_NEXT, "close");
    fcntl_f = (fcntl_fun)dlsym(RTLD_NEXT, "fcntl");
    ioctl_f = (ioctl_fun)dlsym(RTLD_NEXT, "ioctl");
    getsockopt_f = (getsockopt_fun)dlsym(RTLD_NEXT, "getsockopt");
    setsockopt_f = (setsockopt_fun)dlsym(RTLD_NEXT, "setsockopt");

    isInit = true;
}

struct __HookIniter {
    __HookIniter() {
        hookInit();
    }
};

static __HookIniter s_hookIniter;

}

extern "C" {

sleep_fun sleep_f;
usleep_fun usleep_f;
nanosleep_fun nanosleep_f;
socket_fun socket_f;
connect_fun connect_f;
accept_fun accept_f;
read_fun read_f;
readv_fun readv_f;
recv_fun recv_f;
recvfrom_fun recvfrom_f;
recvmsg_fun recvmsg_f;
write_fun write_f;
writev_fun writev_f;
send_fun send_f;
sendto_fun sendto_f;
sendmsg_fun sendmsg_f;
close_fun close_f;
fcntl_fun fcntl_f;
ioctl_fun ioctl_f;
getsockopt_fun getsockopt_f;
setsockopt_fun setsockopt_f;

// sleep

unsigned int sleep(unsigned int seconds)
{
    if (!cczoe::t_hookEnable)
    {
        return sleep_f(seconds);
    }

    std::shared_ptr<cczoe::fiber::Fiber> fiber = cczoe::fiber::Fiber::GetThis();
    cczoe::IOManager *iom = cczoe::IOManager::GetThis();
    iom->addTimer(seconds * 1000, [&](){
        iom->schedule(fiber);
    }, false);
    cczoe::fiber::Fiber::YieldToReady();
    return 0;
}

int usleep(useconds_t usec)
{
    if (!cczoe::t_hookEnable)
    {
        return usleep_f(usec);
    }

    std::shared_ptr<cczoe::fiber::Fiber> fiber = cczoe::fiber::Fiber::GetThis();
    cczoe::IOManager *iom = cczoe::IOManager::GetThis();
    iom->addTimer(usec / 1000, [&](){
        iom->schedule(fiber);
    }, false);
    cczoe::fiber::Fiber::YieldToReady();
    return 0;
}

int nanosleep(const struct timespec *req, struct timespec *rem)
{
    if (!cczoe::t_hookEnable)
    {
        return nanosleep_f(req, rem);
    }

    std::shared_ptr<cczoe::fiber::Fiber> fiber = cczoe::fiber::Fiber::GetThis();
    cczoe::IOManager *iom = cczoe::IOManager::GetThis();
    iom->addTimer(req->tv_sec * 1000 + req->tv_nsec / 1000000, [&](){
        iom->schedule(fiber);
    }, false);
    cczoe::fiber::Fiber::YieldToReady();
    return 0;
}

// socket

int socket(int domain, int type, int protocol)
{
    if (!cczoe::t_hookEnable)
    {
        return socket_f(domain, type, protocol);
    }

    int fd = socket_f(domain, type, protocol);
    if (fd == -1)
    {
        return fd;
    }

    cczoe::FdMgr::GetInstance()->get(fd, true);
    return fd;
}

int connectWithTimeout(int fd, const struct sockaddr *addr, socklen_t addrlen, uint64_t timeout)
{
    if (!cczoe::t_hookEnable)
    {
        return connect_f(fd, addr, addrlen);
    }

    std::shared_ptr<cczoe::FdCtx> ctx = cczoe::FdMgr::GetInstance()->get(fd);
    if (!ctx || ctx->isClosed())
    {
        errno = EBADF;
        return -1;
    }
    if (!ctx->isSocket())
    {
        return connect_f(fd, addr, addrlen);
    }
    if (ctx->isUserNonblock())
    {
        return connect_f(fd, addr, addrlen);
    }

    struct TimeInfo
    {
        int cancelled = 0;
    };
    int n = connect_f(fd, addr, addrlen);
    if (n == 0)
    {
        return 0;
    }
    if (n != -1 || errno != EINPROGRESS)
    {
        return n;
    }

    // add timer and yield
    cczoe::IOManager *iom = cczoe::IOManager::GetThis();
    std::shared_ptr<cczoe::Timer> timer = nullptr;
    std::shared_ptr<TimeInfo> timeInfo = std::make_shared<TimeInfo>();
    if (timeout != std::numeric_limits<uint64_t>::max())
    {
        std::weak_ptr<TimeInfo> cancell(timeInfo);
        timer = iom->addTimer(timeout, [&]() { 
            auto ob = cancell.lock();
            if (ob && !ob->cancelled)
            {
                ob->cancelled = ETIMEDOUT;
                iom->cancelEvent(fd, cczoe::IOManager::WRITE);
            }
        }, cancell);
    }
    if (int rt = iom->addEvent(fd, cczoe::IOManager::WRITE); rt) 
    {
        if (timer)
        {
            timer->cancel();
        }
        throw std::logic_error(std::format("connect addEvent({}, cczoe::IOManager::WRITE) error", fd));
        return -1;
    }
    cczoe::fiber::Fiber::YieldToReady();
    if (timer)
    {
        timer->cancel();
    }
    if (timeInfo->cancelled)
    {
        errno = timeInfo->cancelled;
        return -1;
    }
    
    int error = 0;
    socklen_t len = sizeof(int);
    if (getsockopt(fd, SOL_SOCKET, SO_ERROR, &error, &len) == -1)
    {
        return -1;
    }
    if (!error)
    {
        return 0;
    }
    errno = error;
    return -1;
}

int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
    if (!cczoe::t_hookEnable)
    {
        return connect_f(sockfd, addr, addrlen);
    }
    return 0;
}

int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
    int fd = cczoe::do_io(sockfd, accept_f, "accept", cczoe::IOManager::READ, SO_RCVTIMEO, addr, addrlen);
    if (fd >= 0)
    {
        cczoe::FdMgr::GetInstance()->get(fd, true);
    }
    return fd;
}

// read

ssize_t read(int fd, void *buf, size_t count)
{
    return do_io(fd, read_f, "read", cczoe::IOManager::READ, SO_RCVTIMEO, buf, count);
}

ssize_t readv(int fd, const struct iovec *iov, int iovcnt)
{
    return do_io(fd, readv_f, "readv", cczoe::IOManager::READ, SO_RCVTIMEO, iov, iovcnt);
}

ssize_t recv(int sockfd, void *buf, size_t len, int flags)
{
    return do_io(sockfd, recv_f, "recv", cczoe::IOManager::READ, SO_RCVTIMEO, buf, len, flags);
}

ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen)
{
    return do_io(sockfd, recvfrom_f, "recvfrom", cczoe::IOManager::READ, SO_RCVTIMEO, buf, len, flags, src_addr, addrlen);
}

ssize_t recvmsg(int sockfd, struct msghdr *msg, int flags)
{
    return  do_io(sockfd, recvmsg_f, "recvmsg", cczoe::IOManager::READ, SO_RCVTIMEO, msg, flags);
}

// write

ssize_t write(int fd, const void *buf, size_t count)
{
    return do_io(fd, write_f, "write", cczoe::IOManager::WRITE, SO_SNDTIMEO, buf, count);
}

ssize_t writev(int fd, const struct iovec *iov, int iovcnt)
{
    return do_io(fd, writev_f, "writev", cczoe::IOManager::WRITE, SO_SNDTIMEO, iov, iovcnt);
}

ssize_t send(int sockfd, const void *buf, size_t len, int flags)
{
    return do_io(sockfd, send_f, "send", cczoe::IOManager::WRITE, SO_SNDTIMEO, buf, len, flags);
}

ssize_t sendto(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen)
{
    return do_io(sockfd, sendto_f, "sendto", cczoe::IOManager::WRITE, SO_SNDTIMEO, buf, len, flags, dest_addr, addrlen);
}

ssize_t sendmsg(int sockfd, const struct msghdr *msg, int flags)
{
    return do_io(sockfd, sendmsg_f, "sendmsg", cczoe::IOManager::WRITE, SO_SNDTIMEO, msg, flags);
}

// close

int close(int fd)
{
    if (!cczoe::t_hookEnable)
    {
        return close_f(fd);
    }
    std::shared_ptr<cczoe::FdCtx> ctx = cczoe::FdMgr::GetInstance()->get(fd);
    if (ctx)
    {
        auto iom = cczoe::IOManager::GetThis();
        if (iom)
        {
            iom->cancelAll(fd);
        }
        cczoe::FdMgr::GetInstance()->del(fd);
    }
    return close_f(fd);
}

// control

int fcntl(int fd, int cmd, ...)
{
    if (!cczoe::t_hookEnable)
    {
        return fcntl_f(fd, cmd);
    }

    va_list va;
    va_start(va, cmd);
    switch (cmd)
    {
        case F_SETFL:
        {
            int arg = va_arg(va, int);
            va_end(va);
            std::shared_ptr<cczoe::FdCtx> ctx = cczoe::FdMgr::GetInstance()->get(fd);
            if (!ctx || ctx->isClosed() || !ctx->isSocket())
            {
                return fcntl_f(fd, cmd, arg);
            }
            ctx->setUserNonblock(arg & O_NONBLOCK);
            if (ctx->isSysNonblock())
            {
                arg |= O_NONBLOCK;
            }
            else
            {
                arg &= ~O_NONBLOCK;
            }
            return fcntl_f(fd, cmd, arg);
        }
        case F_GETFL:
        {
            va_end(va);
            int arg = fcntl_f(fd, cmd);
            std::shared_ptr<cczoe::FdCtx> ctx = cczoe::FdMgr::GetInstance()->get(fd);
            if (!ctx || ctx->isClosed() || !ctx->isSocket())
            {
                return arg;
            }
            if (ctx->isUserNonblock())
            {
                return arg | O_NONBLOCK;
            }
            else
            {
                return arg & ~O_NONBLOCK;
            }
        }
        case F_DUPFD:
        case F_DUPFD_CLOEXEC:
        case F_SETFD:
        case F_SETOWN:
        case F_SETSIG:
        case F_SETLEASE:
        case F_NOTIFY:
        case F_SETPIPE_SZ:
        {
            int arg = va_arg(va, int);
            va_end(va);
            return fcntl_f(fd, cmd, arg);
        }
        case F_GETFD:
        case F_GETOWN:
        case F_GETSIG:
        case F_GETLEASE:
        case F_GETPIPE_SZ:
        {
            va_end(va);
            return fcntl_f(fd, cmd);
        }
        case F_SETLK:
        case F_SETLKW:
        case F_GETLK:
        {
            struct flock *arg = va_arg(va, struct flock *);
            va_end(va);
            return fcntl_f(fd, cmd, arg);
        }
        case F_GETOWN_EX:
        case F_SETOWN_EX:
        {
            struct f_owner_exlock *arg = va_arg(va, struct f_owner_exlock *);
            va_end(va);
            return fcntl_f(fd, cmd, arg);
        }
        default:
        {
            va_end(va);
            return fcntl_f(fd, cmd);
        }
    }
}

int ioctl(int fd, unsigned long request, ...)
{
    va_list va;
    va_start(va, request);
    void *arg = va_arg(va, void *);
    va_end(va);

    if (FIONBIO == request)    
    {
        bool userNonblock = !!*(int *)arg;
        std::shared_ptr<cczoe::FdCtx> ctx = cczoe::FdMgr::GetInstance()->get(fd);
        if (!ctx || ctx->isClosed() || !ctx->isSocket())
        {
            return ioctl_f(fd, request, arg);
        }
        ctx->setUserNonblock(userNonblock);
    }
    return ioctl_f(fd, request, arg);
}

int getsockopt(int sockfd, int level, int optname, void *optval, socklen_t *optlen)
{
    return getsockopt_f(sockfd, level, optname, optval, optlen);
}

int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen)
{
    if (!cczoe::t_hookEnable)
    {
        return setsockopt_f(sockfd, level, optname, optval, optlen);
    }
    if (level == SOL_SOCKET)
    {
        if (optname == SO_RCVTIMEO || optname == SO_SNDTIMEO)
        {
            std::shared_ptr<cczoe::FdCtx> ctx = cczoe::FdMgr::GetInstance()->get(sockfd);
            if (ctx)
            {
                const timeval *tv = (const timeval *)optval;
                ctx->setTimeout(optname, tv->tv_sec * 1000 + tv->tv_usec / 1000);
            }
        }
    }
    return setsockopt_f(sockfd, level, optname, optval, optlen);
}

}
