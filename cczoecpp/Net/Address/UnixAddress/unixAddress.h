#ifndef __CCZOE_UNIXADDRESS_H__
#define __CCZOE_UNIXADDRESS_H__

#include <sys/un.h>
#include "Address/address.h"

namespace cczoe {
namespace net {

class UnixAddress : public Address
{
private:
    sockaddr_un m_addr;
    socklen_t m_addrLen;

    virtual std::ostream& toString(std::ostream &os) const override;

public:
    UnixAddress();
    UnixAddress(const std::string &path);
    virtual ~UnixAddress() { }

    const sockaddr* getAddr() const override;
    socklen_t getAddrLen() const override;
};

class UnknownAddress : public Address
{
private:
    sockaddr_storage m_addr;
    socklen_t m_addrLen;

    virtual std::ostream& toString(std::ostream &os) const override;

public:
    UnknownAddress(int family = AF_UNIX);
    virtual ~UnknownAddress() { }

    const sockaddr* getAddr() const override;
    socklen_t getAddrLen() const override;
};

}
}

#endif