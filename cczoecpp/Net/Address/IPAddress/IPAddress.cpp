#include <sys/types.h>
#include <stdio.h>
#include <ifaddrs.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <bit>
#include "log.h"
#include "IPAddress.h"

namespace cczoe {
namespace net {

std::optional<std::vector<std::shared_ptr<IPAddress>>> IPAddress::DNSLookup(const std::string &host, int family, int type, int protocol)
{
    std::string service = "";
    std::string hostname = host;
    if (size_t pos = host.find_last_of(":"); pos != std::string::npos)
    {
        service = host.substr(pos + 1);
        hostname = host.substr(0, pos);
    }

    // TODO: not completed
    addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_flags = 0;
    hints.ai_family = family;
    hints.ai_socktype = type;
    hints.ai_protocol = protocol;
    hints.ai_addrlen = 0;
    hints.ai_addr = nullptr;
    hints.ai_canonname = nullptr;
    hints.ai_next = nullptr;

    addrinfo *res;
    int rt = getaddrinfo(hostname.c_str(), service.empty() ? nullptr : service.c_str(), &hints, &res);
    if (rt != 0)
    {
        CCZOE_LOG_ERROR(CCZOE_LOG_ROOT()) << "DNSLookup getaddrinfo errno = " << rt << ": " << gai_strerror(rt);
        return std::nullopt;
    }

    std::vector<std::shared_ptr<IPAddress>> addrs;
    addrinfo *node = res;
    while (node != nullptr)
    {
        if (node->ai_family == AF_INET || node->ai_family == AF_UNSPEC)
        {
            addrs.push_back(std::make_shared<IPv4Address>(
                *reinterpret_cast<sockaddr_in *>(node->ai_addr)
            ));
        }
        if (node->ai_family == AF_INET6 || node->ai_family == AF_UNSPEC)
        {
            addrs.push_back(std::make_shared<IPv6Address>(
                *reinterpret_cast<sockaddr_in6 *>(node->ai_addr)
            ));
        }
        node = node->ai_next;
    }
    freeaddrinfo(res);
    if (addrs.empty())
    {
        return std::nullopt;
    }
    return addrs;
}

std::optional<std::multimap<std::string, IPAddress::NICInfo>> IPAddress::GetNICAddresses(int family)
{
    std::multimap<std::string, NICInfo> res;
    ifaddrs *ifap;
    int rt = getifaddrs(&ifap);
    if (rt != 0) 
    {
        CCZOE_LOG_ERROR(CCZOE_LOG_ROOT()) << "IPAddress::GetNICAddresses getifaddrs errno = " << rt << ": " << strerror(rt);
        return std::nullopt;
    }
    ifaddrs *node = ifap;
    while (node != nullptr)
    {
        if (family != AF_UNSPEC && family != node->ifa_addr->sa_family)
        {
            node = node->ifa_next;
            continue;
        }

        const std::string name = node->ifa_name;
        std::shared_ptr<IPAddress> addr = nullptr;
        uint32_t prefixLen = 0;
        switch (node->ifa_addr->sa_family)
        {
            case AF_INET:
            {
                addr = std::make_shared<IPv4Address>(
                    *reinterpret_cast<sockaddr_in *>(node->ifa_addr)
                );
                prefixLen = std::popcount(
                    reinterpret_cast<sockaddr_in *>(node->ifa_netmask)->sin_addr.s_addr
                );
                break;
            }
            case AF_INET6:
            {
                addr = std::make_shared<IPv6Address>(
                    *reinterpret_cast<const sockaddr_in6 *>(node->ifa_addr)
                );
                for (int i = 0; i < 16; i++)
                {
                    prefixLen += std::popcount(
                        reinterpret_cast<sockaddr_in6 *>(node->ifa_netmask)->sin6_addr.s6_addr[i]
                    );
                }
                break;
            }
        }
        if (addr != nullptr)
        {
            res.emplace(name, std::make_pair(addr, prefixLen));
        }
        node = node->ifa_next;
    }
    freeifaddrs(ifap);
    if (res.empty())
    {
        return std::nullopt;
    }
    return res;
}

std::optional<std::vector<IPAddress::NICInfo>> IPAddress::GetNICAddresses(const std::string &iface, int family)
{
    std::vector<NICInfo> res;
    if (iface.empty() || iface == "*") 
    {
        if (family == AF_INET || family == AF_UNSPEC) 
        {
            res.emplace_back(std::make_shared<IPv4Address>(), 0);
        }
        if (family == AF_INET6 || family == AF_UNSPEC) 
        {
            res.emplace_back(std::make_shared<IPv6Address>(), 0);
        }
        return res;
    }

    auto infoMap = GetNICAddresses(family);
    if (infoMap.has_value())
    {
        auto its = infoMap->equal_range(iface);
        while (its.first != its.second)
        {
            res.push_back(its.first->second);
            its.first++;
        }
        return res;
    }
    return std::nullopt;
}

IPv4Address::IPv4Address(uint32_t ip, uint16_t port)
{
    memset(&m_addr, 0, sizeof(m_addr));
    m_addr.sin_family = AF_INET;
    m_addr.sin_addr.s_addr = htonl(ip);
    m_addr.sin_port = htons(port);
}

IPv4Address::IPv4Address(const char *ip, uint16_t port)
{
    memset(&m_addr, 0, sizeof(m_addr));
    m_addr.sin_family = AF_INET;
    m_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, ip, &m_addr.sin_addr) != 1)
    {
        CCZOE_LOG_ERROR(CCZOE_LOG_ROOT()) << "errno = " << errno << ": " << strerror(errno);
        throw std::runtime_error("IPv4Address: invalid ipv4 address");
    }
}

IPv4Address::IPv4Address(const sockaddr_in &addr)
{
    m_addr = addr;
}

const sockaddr* IPv4Address::getAddr() const
{
    return reinterpret_cast<const sockaddr*>(&m_addr);
}

socklen_t IPv4Address::getAddrLen() const
{
    return sizeof(m_addr);
}

std::ostream &IPv4Address::insert(std::ostream &os) const
{
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &m_addr.sin_addr, ip, sizeof(ip));
    return os << ip << ":" << ntohs(m_addr.sin_port);
}

uint16_t IPv4Address::getPort() const
{
    return ntohs(m_addr.sin_port);
}

void IPv4Address::setPort(uint16_t port)
{
    m_addr.sin_port = htons(port);
}

std::shared_ptr<IPAddress> IPv4Address::broadcastAddress(uint32_t prefixLen)
{
    if (prefixLen > 32)
    {
        return nullptr;
    }

    sockaddr_in baddr(m_addr);
    baddr.sin_addr.s_addr |= htonl(0xffffffff << (32 - prefixLen));
    return std::make_shared<IPv4Address>(baddr);
}

std::shared_ptr<IPAddress> IPv4Address::networkAddress(uint32_t prefixLen)
{
    if (prefixLen > 32)
    {
        return nullptr;
    }

    sockaddr_in saddr(m_addr);
    saddr.sin_addr.s_addr &= htonl(0xffffffff << (32 - prefixLen));
    return std::make_shared<IPv4Address>(saddr);
}

std::shared_ptr<IPAddress> IPv4Address::subnetMask(uint32_t prefixLen)
{
    if (prefixLen > 32)
    {
        return nullptr;
    }

    sockaddr_in saddr(m_addr);
    saddr.sin_addr.s_addr = htonl(0xffffffff << (32 - prefixLen));
    return std::make_shared<IPv4Address>(saddr);
}

IPv6Address::IPv6Address()
{
    memset(&m_addr, 0, sizeof(m_addr));
    m_addr.sin6_family = AF_INET6;
}

IPv6Address::IPv6Address(const char *ip, uint16_t port)
{
    memset(&m_addr, 0, sizeof(m_addr));
    m_addr.sin6_family = AF_INET6;
    m_addr.sin6_port = htons(port);
    if (inet_pton(AF_INET6, ip, &m_addr.sin6_addr) != 1)
    {
        CCZOE_LOG_ERROR(CCZOE_LOG_ROOT()) << "errno = " << errno << ": " << strerror(errno);
        throw std::runtime_error("IPv6Address: invalid ipv6 address");
    }
}

IPv6Address::IPv6Address(const sockaddr_in6 &addr)
{
    m_addr = addr;
}

const sockaddr *IPv6Address::getAddr() const
{
    return reinterpret_cast<const sockaddr*>(&m_addr);
}

socklen_t IPv6Address::getAddrLen() const
{
    return sizeof(m_addr);
}

std::ostream& IPv6Address::insert(std::ostream &os) const
{
    char ip[INET6_ADDRSTRLEN];
    inet_ntop(AF_INET6, &m_addr.sin6_addr, ip, sizeof(ip));
    return os << ip << ":" << ntohs(m_addr.sin6_port);
}

uint16_t IPv6Address::getPort() const
{
    return ntohs(m_addr.sin6_port);
}

void IPv6Address::setPort(uint16_t port)
{
    m_addr.sin6_port = htons(port);
}

std::shared_ptr<IPAddress> IPv6Address::broadcastAddress(uint32_t prefixLen)
{
    if (prefixLen > 128)
    {
        return nullptr;
    }

    sockaddr_in6 baddr(m_addr);
    baddr.sin6_addr.s6_addr[prefixLen / 8] |= (0xff >> (prefixLen % 8));
    for (int i = prefixLen / 8 + 1; i < 16; i++) 
    {
        baddr.sin6_addr.s6_addr[i] = 0xff;
    }
    return std::make_shared<IPv6Address>(baddr);
}

std::shared_ptr<IPAddress> IPv6Address::networkAddress(uint32_t prefixLen)
{
    if (prefixLen > 128)
    {
        return nullptr;
    }

    sockaddr_in6 naddr(m_addr);
    for (int i = 0; i < 16; ++i) {
        if (prefixLen > 8) 
        {
            naddr.sin6_addr.s6_addr[i] &= 0xff;
            prefixLen -= 8;
        }
        else
        {
            naddr.sin6_addr.s6_addr[i] &= 0xff << (8 - prefixLen);
            prefixLen = 0;
        }
    }
    return std::make_shared<IPv6Address>(naddr);
}

std::shared_ptr<IPAddress> IPv6Address::subnetMask(uint32_t prefixLen)
{
    if (prefixLen > 128)
    {
        return nullptr;
    }

    sockaddr_in6 saddr(m_addr);
    for (int i = 0; i < 16; ++i) 
    {
        if (prefixLen > 8) 
        {
            saddr.sin6_addr.s6_addr[i] = 0xff;
            prefixLen -= 8;
        }
        else
        {
            saddr.sin6_addr.s6_addr[i] = 0xff << (8 - prefixLen);
            prefixLen = 0;
        }
    }
    return std::make_shared<IPv6Address>(saddr);
}

}
}