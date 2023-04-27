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

public:
    UnixAddress();
    UnixAddress(const std::string &path);
    virtual ~UnixAddress() { }

    virtual const sockaddr* getAddr() const override;
    virtual sockaddr* getAddr() override { return (sockaddr*)&m_addr; }
    virtual socklen_t getAddrLen() const override;
    virtual std::ostream& insert(std::ostream &os) const override;

    virtual void setAddrLen(socklen_t len) { m_addrLen = len; }
};

class UnknownAddress : public Address
{
private:
    sockaddr m_addr;

public:
    UnknownAddress(int family = AF_UNIX);
    virtual ~UnknownAddress() { }

    virtual const sockaddr* getAddr() const override;
    virtual sockaddr* getAddr() override  { return (sockaddr*)&m_addr; }
    virtual socklen_t getAddrLen() const override;
    virtual std::ostream& insert(std::ostream &os) const override;
};

}
}

#endif