#include <sstream>
#include <string.h>
#include "address.h"

namespace cczoe {
namespace net {

int Address::getFamily() const
{
    return getAddr()->sa_family;
}

std::string Address::toString() const
{
    std::stringstream ss;
    toString(ss);
    return ss.str();
}

bool Address::operator<(const Address &rhs) const
{
    return toString() < rhs.toString();
}

bool Address::operator==(const Address &rhs) const
{
    return toString() == rhs.toString();
}

bool Address::operator!=(const Address &rhs) const
{
    return !(*this == rhs);
}

}
}