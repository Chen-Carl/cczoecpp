#ifndef __CCZOE_ADDRESS_H__
#define __CCZOE_ADDRESS_H__

#include <sys/types.h>
#include <sys/socket.h>
#include <iostream>
#include <memory>

namespace cczoe {
namespace net {

class Address
{
public:
    Address() { }
    virtual ~Address() { }

    virtual const sockaddr* getAddr() const = 0;
    virtual socklen_t getAddrLen() const = 0;
    virtual int getFamily() const;
    virtual sockaddr* getAddr() = 0;
    
    virtual std::ostream& insert(std::ostream &os) const = 0;

    virtual std::string toString() const;

    bool operator<(const Address &rhs) const;
    bool operator==(const Address &rhs) const;
    bool operator!=(const Address &rhs) const;
};

}}

#endif