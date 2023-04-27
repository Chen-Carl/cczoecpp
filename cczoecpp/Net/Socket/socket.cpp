#include <format>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

#include "Socket/socket.h"
#include "Net/Hook/fdmanager.h"
#include "Net/Hook/hook.h"
#include "Net/Address/sockaddr.h"
#include "Net/Socket/socket.h"
#include "socket.h"

namespace cczoe {
namespace net {

Socket::Socket(int family, int type, int protocol) : 
    m_sock(-1),
    m_family(family),
    m_type(type),
    m_protocol(protocol),
    m_isConnected(false) 
{

}

Socket::~Socket()
{
    close();
}

void Socket::init()
{
    int val = 1;
    // listen multiple sockets on the same port
    setOption(SOL_SOCKET, SO_REUSEADDR, &val, sizeof(int));
    if (m_type == SOCK_STREAM) 
    {
        // disable Nagle's algorithm
        setOption(IPPROTO_TCP, TCP_NODELAY, &val, sizeof(int));
    }
}

bool Socket::init(int sock)
{
    std::shared_ptr<FdCtx> ctx = FdMgr::GetInstance()->get(sock);
    if (ctx && ctx->isSocket() && !ctx->isClosed())
    {
        m_sock = sock;
        m_isConnected = true;
        init();
        getLocalAddress();
        getRemoteAddress();
        return true;
    }
    return false;
}

bool Socket::isValid() const
{
    return m_sock != -1;
}

int Socket::getError() const
{
    int error = 0;
    socklen_t len = sizeof(int);
    if (!getOption(SOL_SOCKET, SO_ERROR, &error, &len))
    {
        return errno;
    }
    return error;
}

uint64_t Socket::getSendTimeout() const
{
    std::shared_ptr<FdCtx> ctx = FdMgr::GetInstance()->get(m_sock);
    if (ctx)
    {
        return ctx->getTimeout(SO_SNDTIMEO);
    }
    return std::numeric_limits<uint64_t>::max();
}

uint64_t Socket::getRecvTimeout() const
{
    std::shared_ptr<FdCtx> ctx = FdMgr::GetInstance()->get(m_sock);
    if (ctx)
    {
        return ctx->getTimeout(SO_RCVTIMEO);
    }
    return std::numeric_limits<uint64_t>::max();
}

bool Socket::getOption(int level, int optname, void *optval, socklen_t *optlen) const
{
    int rt = getsockopt(m_sock, level, optname, optval, optlen);
    if (rt)
    {
        CCZOE_LOG_ERROR(CCZOE_LOG_ROOT()) << std::format("syscall error: in getsockopt: sock={}, level={}, optname={}, errno={}, errstr={}", m_sock, level, optname, errno, strerror(errno));
        return false;
    }
    return true;
}

std::shared_ptr<Address> Socket::getRemoteAddress()
{
    if (m_remoteAddress)
    {
        return m_remoteAddress;
    }
    std::shared_ptr<Address> res;
    switch (m_family)
    {
    case AF_INET:
        res = std::make_shared<IPv4Address>();
        break;
    case AF_INET6:
        res = std::make_shared<IPv6Address>();
        break;
    case AF_UNIX:
        res = std::make_shared<UnixAddress>();
        break;
    default:    
        res = std::make_shared<UnknownAddress>(m_family);
        break;
    }
    socklen_t addrlen = res->getAddrLen();
    if (getpeername(m_sock, res->getAddr(), &addrlen))
    {
        CCZOE_LOG_ERROR(CCZOE_LOG_ROOT()) << std::format("syscall error: in getpeername: sock={}, errno={}, errstr={}", m_sock, errno, strerror(errno));
        return std::make_shared<UnknownAddress>(m_family);
    }
    if (m_family == AF_UNIX)
    {
        std::dynamic_pointer_cast<UnixAddress>(res)->setAddrLen(addrlen);
    }
    m_remoteAddress = res;
    return m_remoteAddress;
}

std::shared_ptr<Address> Socket::getLocalAddress()
{
    if (m_localAddress)
    {
        return m_localAddress;
    }
    std::shared_ptr<Address> res;
    switch (m_family)
    {
    case AF_INET:
        res = std::make_shared<IPv4Address>();
        break;
    case AF_INET6:
        res = std::make_shared<IPv6Address>();
        break;
    case AF_UNIX:
        res = std::make_shared<UnixAddress>();
        break;
    default:
        res = std::make_shared<UnknownAddress>(m_family);
        break;
    }
    socklen_t addrlen = res->getAddrLen();
    if (getsockname(m_sock, res->getAddr(), &addrlen))
    {
        CCZOE_LOG_ERROR(CCZOE_LOG_ROOT()) << std::format("syscall error: in getsockname: sock={}, errno={}, errstr={}", m_sock, errno, strerror(errno));
        return std::make_shared<UnknownAddress>(m_family);
    }
    if (m_family == AF_UNIX)
    {
        std::dynamic_pointer_cast<UnixAddress>(res)->setAddrLen(addrlen);
    }
    m_localAddress = res;
    return m_localAddress;
}

bool net::Socket::setSendTimeout(int64_t timeout)
{
    struct timeval tv { timeout / 1000, (int)(timeout % 1000 * 1000) };
    return setOption(SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
}

bool Socket::setRecvTimeout(int64_t timeout)
{
    struct timeval tv { timeout / 1000, (int)(timeout % 1000 * 1000) };
    return setOption(SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
}

bool Socket::setOption(int level, int optname, const void *optval, socklen_t optlen)
{
    int rt = setsockopt(m_sock, level, optname, optval, optlen);
    if (rt)
    {
        CCZOE_LOG_ERROR(CCZOE_LOG_ROOT()) << std::format("syscall error: in setsockopt: sock={}, level={}, optname={}, errno={}, errstr={}", m_sock, level, optname, errno, strerror(errno));
        return false;
    }
    return true;
}

std::shared_ptr<Socket> Socket::accept()
{
    std::shared_ptr<Socket> sk = std::make_shared<Socket>(m_family, m_type, m_protocol);
    int sock = ::accept(m_sock, nullptr, nullptr);
    if (sock == -1)
    {
        CCZOE_LOG_ERROR(CCZOE_LOG_ROOT()) << std::format("syscall error: in accept: sock={}, errno={}, errstr={}", m_sock, errno, strerror(errno));
        return nullptr;
    }
    if (sk->init(sock))
    {
        return sk;
    }
    return nullptr;
}

bool Socket::bind(const std::shared_ptr<Address> addr)
{
    m_localAddress = addr;
    if (!isValid())
    {
        m_sock = socket(m_family, m_type, m_protocol);
        if (m_sock == -1)
        {
            CCZOE_LOG_ERROR(CCZOE_LOG_ROOT()) << std::format("syscall error: in socket: socket create error: errno={}, errstr={}", errno, strerror(errno));
            return false;
        }
        init();
        if (!isValid())
        {
            return false;
        }
    }

    if (getFamily() != addr->getFamily())
    {
        CCZOE_LOG_ERROR(CCZOE_LOG_ROOT()) << std::format("family error: sock family = {} while bind address family = {}, sock={}, addr={}, errno={}, errstr={}", getFamily(), addr->getFamily(), m_sock, addr->toString(), errno, strerror(errno));
        return false;
    }

    if (::bind(m_sock, addr->getAddr(), addr->getAddrLen()))
    {
        CCZOE_LOG_ERROR(CCZOE_LOG_ROOT()) << std::format("syscall error: in bind: sock={}, bind addr={}, errno={}, errstr={}", m_sock, addr->toString(), errno, strerror(errno));
        return false;
    }
    getLocalAddress();
    return true;
}

bool Socket::connect(const std::shared_ptr<Address> addr, uint64_t timeout)
{
    m_remoteAddress = addr;
    if (!isValid())
    {
        m_sock = socket(m_family, m_type, m_protocol);
        if (m_sock == -1)
        {
            CCZOE_LOG_ERROR(CCZOE_LOG_ROOT()) << std::format("syscall error: in socket: socket create error: errno={}, errstr={}", errno, strerror(errno));
            return false;
        }
        init();
        if (!isValid())
        {
            return false;
        }
    }

    if (addr->getFamily() != m_family)
    {
        CCZOE_LOG_ERROR(CCZOE_LOG_ROOT()) << std::format("family error: sock family = {} while bind address family = {}, sock={}, addr={}, errno={}, errstr={}", m_family, addr->getFamily(), m_sock, addr->toString(), errno, strerror(errno));
        return false;
    }

    if (timeout == std::numeric_limits<uint64_t>::max())
    {
        if (::connect(m_sock, addr->getAddr(), addr->getAddrLen()))
        {
            CCZOE_LOG_ERROR(CCZOE_LOG_ROOT()) << std::format("syscall error: in connect: sock={}, addr={}, errno={}, errstr={}", m_sock, addr->toString(), errno, strerror(errno));
            close();
            return false;
        }
    }
    else if (::connectWithTimeout(m_sock, addr->getAddr(), addr->getAddrLen(), timeout))
    {
        CCZOE_LOG_ERROR(CCZOE_LOG_ROOT()) << std::format("syscall error: in connect: connectWithTimeout error: sock={}, addr={}, errno={}, errstr={}", m_sock, addr->toString(), errno, strerror(errno));
        close();
        return false;
    }
    m_isConnected = true;
    getRemoteAddress();
    getLocalAddress();
    return true;
}

bool Socket::listen(int backlog)
{
    if (!isValid())
    {
        CCZOE_LOG_ERROR(CCZOE_LOG_ROOT()) << std::format("invalid socket: in listen: sock={}, errno={}, errstr={}", m_sock, errno, strerror(errno));
        return false;
    }
    if (::listen(m_sock, backlog))
    {
        CCZOE_LOG_ERROR(CCZOE_LOG_ROOT()) << std::format("syscall error: in listen: sock={}, errno={}, errstr={}", m_sock, errno, strerror(errno));
        return false;
    }
    return true;
}

bool Socket::close()
{
    if (!m_isConnected && m_sock == -1)
    {
        return true;
    }
    m_isConnected = false;
    if (m_sock != -1)
    {
        ::close(m_sock);
        m_sock = -1;
    }
    return false;
}

int Socket::send(const void *buf, size_t len, int flags)
{
    if (isConnected())
    {
        return ::send(m_sock, buf, len, flags);
    }
    return -1;    
}

int Socket::send(iovec *buf, size_t len, int flags)
{
    if (isConnected())
    {
        msghdr msg;
        memset(&msg, 0, sizeof(msg));
        msg.msg_iov = buf;
        msg.msg_iovlen = len;
        return ::sendmsg(m_sock, &msg, flags);
    }
    return -1;   
}

int Socket::sendTo(const void *buf, size_t len, const std::shared_ptr<Address> addr, int flags)
{
    if (isConnected())
    {
        return ::sendto(m_sock, buf, len, flags, addr->getAddr(), addr->getAddrLen());
    }
    return -1;
}

int Socket::sendTo(iovec *buf, size_t len, const std::shared_ptr<Address> addr, int flags)
{
    if (isConnected())
    {
        msghdr msg;
        memset(&msg, 0, sizeof(msg));
        msg.msg_iov = buf;
        msg.msg_iovlen = len;
        msg.msg_name = addr->getAddr();
        msg.msg_namelen = addr->getAddrLen();
        return ::sendmsg(m_sock, &msg, flags);
    }
    return -1;
}

int Socket::recv(void *buf, size_t len, int flags)
{
    if (isConnected())
    {
        return ::recv(m_sock, buf, len, flags);
    }
    return -1;
}

int Socket::recv(iovec *buf, size_t len, int flags)
{
    if (isConnected())
    {
        msghdr msg;
        memset(&msg, 0, sizeof(msg));
        msg.msg_iov = buf;
        msg.msg_iovlen = len;
        return ::recvmsg(m_sock, &msg, flags);
    }
    return -1;
}

int Socket::recvFrom(void *buf, size_t len, std::shared_ptr<Address> addr, int flags)
{
    if (isConnected())
    {
        socklen_t addrlen = addr->getAddrLen();
        return ::recvfrom(m_sock, buf, len, flags, addr->getAddr(), &addrlen);
    }
    return -1;
}

int Socket::recvFrom(iovec *buf, size_t len, std::shared_ptr<Address> addr, int flags)
{
    if (isConnected())
    {
        msghdr msg;
        memset(&msg, 0, sizeof(msg));
        msg.msg_iov = buf;
        msg.msg_iovlen = len;
        msg.msg_name = addr->getAddr();
        msg.msg_namelen = addr->getAddrLen();
        return ::recvmsg(m_sock, &msg, flags);
    }
    return -1;
}

bool Socket::cancelRead()
{
    return IOManager::GetThis()->cancelEvent(m_sock, IOManager::READ);
}

bool Socket::cancelWrite()
{
    return IOManager::GetThis()->cancelEvent(m_sock, IOManager::WRITE);
}

bool Socket::cancelAccept()
{
    return IOManager::GetThis()->cancelEvent(m_sock, IOManager::READ);
}

bool Socket::cancelAll()
{
    return IOManager::GetThis()->cancelAll(m_sock);
}

std::ostream &Socket::dump(std::ostream &os) const
{
    os << std::format("Socket[sock={}, is_connected={}, local_address={}, remote_address={}]", m_sock, m_isConnected, m_localAddress->toString(), m_remoteAddress->toString());
    return os;
}

std::shared_ptr<Socket> Socket::CreateTCP(std::shared_ptr<Address> address)
{
    std::shared_ptr<Socket> sock = std::make_shared<Socket>(address->getFamily(), SOCK_STREAM, 0);
    return sock;
}

std::shared_ptr<Socket> Socket::CreateUDP(std::shared_ptr<Address> address)
{
    std::shared_ptr<Socket> sock = std::make_shared<Socket>(address->getFamily(), SOCK_DGRAM, 0);
    return sock;
}

std::shared_ptr<Socket> Socket::CreateTCPSocket()
{
    std::shared_ptr<Socket> sock = std::make_shared<Socket>(AF_INET, SOCK_STREAM, 0);
    return sock;
}

std::shared_ptr<Socket> Socket::CreateUDPSocket()
{
    std::shared_ptr<Socket> sock = std::make_shared<Socket>(AF_INET, SOCK_DGRAM, 0);
    return sock;
}

std::shared_ptr<Socket> Socket::CreateUnixTCPSocket()
{
    std::shared_ptr<Socket> sock = std::make_shared<Socket>(AF_UNIX, SOCK_STREAM, 0);
    return sock;
}

std::shared_ptr<Socket> Socket::CreateUnixUDPSocket()
{
    std::shared_ptr<Socket> sock = std::make_shared<Socket>(AF_UNIX, SOCK_DGRAM, 0);
    return sock;
}

}
}