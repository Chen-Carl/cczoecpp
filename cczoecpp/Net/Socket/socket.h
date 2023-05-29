#ifndef __CCZOE_NET_SOCKET_H__
#define __CCZOE_NET_SOCKET_H__

#include <sys/types.h>
#include <sys/socket.h>
#include <memory>
#include <boost/noncopyable.hpp>
#include "Net/Address/address.h"
#include "Scheduler/iomanager.h"

namespace cczoe {
namespace net {

class Socket : public boost::noncopyable
{
private:
    using FdContext = IOManager::FdContext;
private:
    int m_sock;         // socket fd, -1 means invalid
    int m_family;
    int m_type;
    int m_protocol;
    int m_isConnected;

    std::shared_ptr<Address> m_localAddress;
    std::shared_ptr<Address> m_remoteAddress;

    void init();

public:
    Socket(int family, int type, int protocol = 0);
    ~Socket();

    bool init(int sock);

    int getFamily() const { return m_family; }
    int getType() const { return m_type; }
    int getProtocol() const { return m_protocol; }
    int getSocket() const { return m_sock; }
    bool isConnected() const { return m_isConnected; }
    bool isValid() const;
    int getError() const;

    uint64_t getSendTimeout() const;
    uint64_t getRecvTimeout() const;
    bool getOption(int level, int optname, void *optval, socklen_t *optlen) const;
    std::shared_ptr<Address> getRemoteAddress();
    std::shared_ptr<Address> getLocalAddress();

    bool setSendTimeout(int64_t timeout);
    bool setRecvTimeout(int64_t timeout);
    bool setOption(int level, int optname, const void *optval, socklen_t optlen);

    std::shared_ptr<Socket> accept();
    bool bind(const std::shared_ptr<Address> addr);
    bool connect(const std::shared_ptr<Address> addr, uint64_t timeout = -1);
    bool listen(int backlog = SOMAXCONN);
    bool close();

    int send(const void *buf, size_t len, int flags = 0);
    int send(iovec *buf, size_t len, int flags = 0);
    int sendTo(const void *buf, size_t len, const std::shared_ptr<Address> addr, int flags = 0);
    int sendTo(iovec *buf, size_t len, const std::shared_ptr<Address> addr, int flags = 0);

    int recv(void *buf, size_t len, int flags = 0);
    int recv(iovec *buf, size_t len, int flags = 0);
    int recvFrom(void *buf, size_t len, std::shared_ptr<Address> addr, int flags = 0);
    int recvFrom(iovec *buf, size_t len, std::shared_ptr<Address> addr, int flags = 0);

    bool cancelRead();
    bool cancelWrite();
    bool cancelAccept();
    bool cancelAll();

    std::ostream &dump(std::ostream &os) const;

    friend std::ostream &operator<<(std::ostream &os, const Socket &sock)
    {
        return sock.dump(os);
    }

    static std::shared_ptr<Socket> CreateTCP(std::shared_ptr<Address> address);
    static std::shared_ptr<Socket> CreateUDP(std::shared_ptr<Address> address);
    static std::shared_ptr<Socket> CreateTCPSocket();
    static std::shared_ptr<Socket> CreateUDPSocket();
    static std::shared_ptr<Socket> CreateUnixTCPSocket();
    static std::shared_ptr<Socket> CreateUnixUDPSocket();
};


}
}

#endif