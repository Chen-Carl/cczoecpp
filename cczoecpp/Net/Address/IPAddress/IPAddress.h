#ifndef __CCZOE_IPADDRESS_H__
#define __CCZOE_IPADDRESS_H__

#include <arpa/inet.h>
#include <string>
#include <sstream>
#include "Address/address.h"

namespace cczoe {
namespace net {

class IPAddress : public Address
{
private:
    virtual std::ostream& toString(std::ostream &os) const = 0;

public:
    IPAddress() { }
    virtual ~IPAddress() { }

    // broadcastAddress(prefixLen)
    virtual std::shared_ptr<IPAddress> broadcastAddress(uint32_t prefixLen) = 0;
    // network number
    virtual std::shared_ptr<IPAddress> networkAddress(uint32_t prefixLen) = 0;
    // subnet mask
    virtual std::shared_ptr<IPAddress> subnetMask(uint32_t prefixLen) = 0;

    virtual uint32_t getPort() const = 0;

    virtual void setPort(uint32_t port) = 0;

    virtual std::string toString() const;
};

class IPv4Address : public IPAddress
{
private:
    sockaddr_in m_addr;

    virtual std::ostream& toString(std::ostream &os) const override;

public:
    /**
     * @brief Constructor
     * @param ip    IP address in host byte order
     * @param port  Port number in host byte order
     */
    IPv4Address(uint32_t ip = 0, uint32_t port = 0);
    IPv4Address(const std::string &ip, uint32_t port);
    virtual ~IPv4Address() { }

    virtual uint32_t getPort() const override;
    virtual const sockaddr* getAddr() const override;
    virtual socklen_t getAddrLen() const override;

    virtual void setPort(uint32_t port) override;

    std::string toString() const;

    virtual std::shared_ptr<IPAddress> broadcastAddress(uint32_t prefixLen) override;
    virtual std::shared_ptr<IPAddress> networkAddress(uint32_t prefixLen) override;
    virtual std::shared_ptr<IPAddress> subnetMask(uint32_t prefixLen) override;
};

class IPv6Address : public IPAddress
{
private:
    sockaddr_in6 m_addr;

    virtual std::ostream& toString(std::ostream &os) const override;

public:
    IPv6Address();
    IPv6Address(const char *ip, uint32_t port);
    virtual ~IPv6Address() { }

    virtual std::shared_ptr<IPAddress> broadcastAddress(uint32_t prefixLen) override;
    virtual std::shared_ptr<IPAddress> networkAddress(uint32_t prefixLen) override;
    virtual std::shared_ptr<IPAddress> subnetMask(uint32_t prefixLen) override;

    virtual uint32_t getPort() const override;
    virtual const sockaddr* getAddr() const override;
    virtual socklen_t getAddrLen() const override;

    virtual void setPort(uint32_t port) override;
    virtual std::string toString() const override;
};

}
}

#endif