#include "unixAddress.h"

namespace cczoe {
namespace net {

static const size_t UNIX_ADDRESS_LEN = sizeof(((sockaddr_un*)0)->sun_path) - 1;

UnixAddress::UnixAddress()
{
    memset(&m_addr, 0, sizeof(m_addr));
    m_addr.sun_family = AF_UNIX;
    m_addrLen = offsetof(sockaddr_un, sun_path) + UNIX_ADDRESS_LEN;
}

UnixAddress::UnixAddress(const std::string &path)
{
    memset(&m_addr, 0, sizeof(m_addr));
    m_addr.sun_family = AF_UNIX;
    m_addrLen = path.size() + 1;
    if (!path.empty() && path[0] == '\0')
    {
        m_addrLen--;
    }
    if (m_addrLen > sizeof(m_addr.sun_path))
    {
        throw std::runtime_error("UnixAddress: path too long");
    }
    memcpy(m_addr.sun_path, path.c_str(), path.size() + 1);
    m_addrLen += offsetof(sockaddr_un, sun_path);
}

const sockaddr* UnixAddress::getAddr() const
{
    return (const sockaddr*)&m_addr;
}

socklen_t UnixAddress::getAddrLen() const
{
    return m_addrLen;
}

std::ostream& UnixAddress::insert(std::ostream &os) const
{
    return os << m_addr.sun_path;
}

UnknownAddress::UnknownAddress(int family)
{
    memset(&m_addr, 0, sizeof(m_addr));
    m_addr.sa_family = family;
}

const sockaddr* UnknownAddress::getAddr() const
{
    return (const sockaddr*)&m_addr;
}

socklen_t UnknownAddress::getAddrLen() const
{
    return sizeof(m_addr);
}

std::ostream &UnknownAddress::insert(std::ostream &os) const
{
    return os << "UnknownAddress family = " << m_addr.sa_family;
}

}
}