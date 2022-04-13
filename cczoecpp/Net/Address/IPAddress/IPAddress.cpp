#include <string.h>
#include "IPAddress.h"

namespace cczoe {
namespace net {

std::string IPAddress::toString() const
{
    std::stringstream ss;
    toString(ss);
    return ss.str();
}

IPv4Address::IPv4Address(uint32_t ip, uint32_t port)
{
    memset(&m_addr, 0, sizeof(m_addr));
    m_addr.sin_family = AF_INET;
    m_addr.sin_addr.s_addr = htonl(ip);
    m_addr.sin_port = htons(port);
}

IPv4Address::IPv4Address(const std::string &ip, uint32_t port)
{
    memset(&m_addr, 0, sizeof(m_addr));
    m_addr.sin_family = AF_INET;
    m_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, ip.c_str(), &m_addr.sin_addr) != 1)
    {
        throw std::runtime_error("IPv4Address: invalid ip address");
    }
}

uint32_t IPv4Address::getPort() const
{
    return ntohs(m_addr.sin_port);
}

const sockaddr* IPv4Address::getAddr() const
{
    return reinterpret_cast<const sockaddr*>(&m_addr);
}

socklen_t IPv4Address::getAddrLen() const
{
    return sizeof(m_addr);
}

void IPv4Address::setPort(uint32_t port)
{
    m_addr.sin_port = htons(port);
}

std::string IPv4Address::toString() const
{
    std::stringstream ss;
    toString(ss);
    return ss.str();
}

std::shared_ptr<IPAddress> IPv4Address::broadcastAddress(uint32_t prefixLen)
{
    if (prefixLen > 32)
    {
        return nullptr;
    }

    sockaddr_in saddr(m_addr);
    saddr.sin_addr.s_addr |= htonl(0xffffffff << (32 - prefixLen));
    return std::make_shared<IPv4Address>(ntohl(saddr.sin_addr.s_addr), 0);
}

std::shared_ptr<IPAddress> IPv4Address::networkAddress(uint32_t prefixLen)
{
    if (prefixLen > 32)
    {
        return nullptr;
    }

    sockaddr_in saddr(m_addr);
    saddr.sin_addr.s_addr &= htonl(0xffffffff << (32 - prefixLen));
    return std::make_shared<IPv4Address>(ntohl(saddr.sin_addr.s_addr), 0);
}

std::shared_ptr<IPAddress> IPv4Address::subnetMask(uint32_t prefixLen)
{
    if (prefixLen > 32)
    {
        return nullptr;
    }

    sockaddr_in saddr(m_addr);
    saddr.sin_addr.s_addr = htonl(0xffffffff << (32 - prefixLen));
    return std::make_shared<IPv4Address>(ntohl(saddr.sin_addr.s_addr), 0);
}

std::ostream& IPv4Address::toString(std::ostream &os) const
{
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &m_addr.sin_addr, ip, sizeof(ip));
    return os << ip << ":" << ntohs(m_addr.sin_port);
}

IPv6Address::IPv6Address()
{
    memset(&m_addr, 0, sizeof(m_addr));
    m_addr.sin6_family = AF_INET6;
}

IPv6Address::IPv6Address(const char *ip, uint32_t port)
{
    memset(&m_addr, 0, sizeof(m_addr));
    m_addr.sin6_family = AF_INET6;
    m_addr.sin6_port = htons(port);
    if (inet_pton(AF_INET6, ip, &m_addr.sin6_addr) != 1)
    {
        throw std::runtime_error("IPv6Address: invalid ip address");
    }
}

std::shared_ptr<IPAddress> IPv6Address::broadcastAddress(uint32_t prefixLen)
{
    if (prefixLen > 128)
    {
        return nullptr;
    }

    sockaddr_in6 saddr(m_addr);
    uint64_t mask = 0xffffffffffffffff;
    mask = mask << (128 - prefixLen);
    saddr.sin6_addr.s6_addr[0] |= htonl(mask >> 56);
    saddr.sin6_addr.s6_addr[1] |= htonl(mask >> 48);
    saddr.sin6_addr.s6_addr[2] |= htonl(mask >> 40);
    saddr.sin6_addr.s6_addr[3] |= htonl(mask >> 32);
    saddr.sin6_addr.s6_addr[4] |= htonl(mask >> 24);
    saddr.sin6_addr.s6_addr[5] |= htonl(mask >> 16);
    saddr.sin6_addr.s6_addr[6] |= htonl(mask >> 8);
    saddr.sin6_addr.s6_addr[7] |= htonl(mask);
    return std::make_shared<IPv6Address>((const char*)saddr.sin6_addr.s6_addr, 0);
}

std::shared_ptr<IPAddress> IPv6Address::networkAddress(uint32_t prefixLen)
{
    if (prefixLen > 128)
    {
        return nullptr;
    }

    sockaddr_in6 saddr(m_addr);
    uint64_t mask = 0xffffffffffffffff;
    mask = mask << (128 - prefixLen);
    saddr.sin6_addr.s6_addr[0] &= htonl(mask >> 56);
    saddr.sin6_addr.s6_addr[1] &= htonl(mask >> 48);
    saddr.sin6_addr.s6_addr[2] &= htonl(mask >> 40);
    saddr.sin6_addr.s6_addr[3] &= htonl(mask >> 32);
    saddr.sin6_addr.s6_addr[4] &= htonl(mask >> 24);
    saddr.sin6_addr.s6_addr[5] &= htonl(mask >> 16);
    saddr.sin6_addr.s6_addr[6] &= htonl(mask >> 8);
    saddr.sin6_addr.s6_addr[7] &= htonl(mask);
    return std::make_shared<IPv6Address>((const char*)saddr.sin6_addr.s6_addr, 0);
}

std::shared_ptr<IPAddress> IPv6Address::subnetMask(uint32_t prefixLen)
{
    if (prefixLen > 128)
    {
        return nullptr;
    }

    sockaddr_in6 saddr(m_addr);
    uint64_t mask = 0xffffffffffffffff;
    mask = mask << (128 - prefixLen);
    saddr.sin6_addr.s6_addr[0] = htonl(mask >> 56);
    saddr.sin6_addr.s6_addr[1] = htonl(mask >> 48);
    saddr.sin6_addr.s6_addr[2] = htonl(mask >> 40);
    saddr.sin6_addr.s6_addr[3] = htonl(mask >> 32);
    saddr.sin6_addr.s6_addr[4] = htonl(mask >> 24);
    saddr.sin6_addr.s6_addr[5] = htonl(mask >> 16);
    saddr.sin6_addr.s6_addr[6] = htonl(mask >> 8);
    saddr.sin6_addr.s6_addr[7] = htonl(mask);
    // TODO: ipv6 to string, here is a bug
    return std::make_shared<IPv6Address>((const char*)saddr.sin6_addr.s6_addr, 0);
}

uint32_t IPv6Address::getPort() const
{
    return ntohs(m_addr.sin6_port);
}

const sockaddr* IPv6Address::getAddr() const
{
    return reinterpret_cast<const sockaddr*>(&m_addr);
}

socklen_t IPv6Address::getAddrLen() const
{
    return sizeof(m_addr);
}

void IPv6Address::setPort(uint32_t port)
{
    m_addr.sin6_port = htons(port);
}

std::string IPv6Address::toString() const
{
    std::stringstream ss;
    toString(ss);
    return ss.str();
}

std::ostream& IPv6Address::toString(std::ostream &os) const
{
    char ip[INET6_ADDRSTRLEN];
    inet_ntop(AF_INET6, &m_addr.sin6_addr, ip, sizeof(ip));
    return os << ip << ":" << ntohs(m_addr.sin6_port);
}

}
}