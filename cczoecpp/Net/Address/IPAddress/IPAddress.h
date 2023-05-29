#ifndef __CCZOE_IPADDRESS_H__
#define __CCZOE_IPADDRESS_H__

#include <arpa/inet.h>
#include <optional>
#include <map>
#include <vector>
#include <string>
#include <sstream>
#include "Net/Address/address.h"

namespace cczoe {
namespace net {

class IPAddress : public Address
{
private:
    using NICInfo = std::pair<std::shared_ptr<IPAddress>, uint32_t>;

public:
    IPAddress() { }
    virtual ~IPAddress() { }

    // broadcast address
    virtual std::shared_ptr<IPAddress> broadcastAddress(uint32_t prefixLen) = 0;
    // network address
    virtual std::shared_ptr<IPAddress> networkAddress(uint32_t prefixLen) = 0;
    // subnet mask
    virtual std::shared_ptr<IPAddress> subnetMask(uint32_t prefixLen) = 0;

    virtual uint16_t getPort() const = 0;

    virtual void setPort(uint16_t port) = 0;

    /**
     * @brief search addresses by host name
     * @param host host name, www.baidu.com[:80] ([] is optional)
     * @param family AF_INT, AF_INT6, AF_UNIX
     * @param type socketl type, SOCK_STREAM, SOCK_DGRAM
     * @param protocol IPPROTO_TCP, IPPROTO_UDP
     */
    static std::optional<std::vector<std::shared_ptr<IPAddress>>> DNSLookup(const std::string &host, int family = AF_UNSPEC, int type = 0, int protocol = 0);

    /**
     * @brief get localhost NICs information: NIC name -> (address, subnet mask)
     * @param family AF_INT, AF_INT6, AF_UNIX
     */
    static std::optional<std::multimap<std::string, NICInfo>> GetNICAddresses(int family = AF_INET);
    
    /**
     * @brief get certian NIC's information: array (address, subnet mask)
     * @param[in] iface NIC name
     * @param[in] family AF_INT, AF_INT6, AF_UNIX
     */
    static std::optional<std::vector<NICInfo>> GetNICAddresses(const std::string &iface, int family = AF_INET);
};

class IPv4Address : public IPAddress
{
private:
    sockaddr_in m_addr;

public:
    /**
     * @brief Constructor
     * @param ip    IP address in host byte order
     * @param port  Port number in host byte order
     */
    IPv4Address(uint32_t ip = INADDR_ANY, uint16_t port = 0);
    IPv4Address(const char* ip, uint16_t port);
    IPv4Address(const sockaddr_in &addr);
    virtual ~IPv4Address() { }

    // Address override
    virtual const sockaddr* getAddr() const override;
    virtual sockaddr* getAddr() override { return (sockaddr*)&m_addr; }
    virtual socklen_t getAddrLen() const override;
    virtual std::ostream& insert(std::ostream &os) const override;

    // IPAddress override

    virtual uint16_t getPort() const override;

    virtual void setPort(uint16_t port) override;

    virtual std::shared_ptr<IPAddress> broadcastAddress(uint32_t prefixLen) override;
    virtual std::shared_ptr<IPAddress> networkAddress(uint32_t prefixLen) override;
    virtual std::shared_ptr<IPAddress> subnetMask(uint32_t prefixLen) override;
};

class IPv6Address : public IPAddress
{
private:
    sockaddr_in6 m_addr;

public:
    IPv6Address();
    IPv6Address(const char* ip, uint16_t port);
    IPv6Address(const sockaddr_in6 &addr);
    virtual ~IPv6Address() { }

    // Address override
    virtual const sockaddr* getAddr() const override;
    virtual sockaddr* getAddr() override  { return (sockaddr*)&m_addr; }
    virtual socklen_t getAddrLen() const override;
    virtual std::ostream& insert(std::ostream &os) const override;

    // IPAddress override
    virtual uint16_t getPort() const override;

    virtual void setPort(uint16_t port) override;

    virtual std::shared_ptr<IPAddress> broadcastAddress(uint32_t prefixLen) override;
    virtual std::shared_ptr<IPAddress> networkAddress(uint32_t prefixLen) override;
    virtual std::shared_ptr<IPAddress> subnetMask(uint32_t prefixLen) override;
};


}
}

#endif