#include "Log/log.h"
#include "Net/Address/sockaddr.h"

using namespace cczoe;
using namespace net;

void testAddress();

int main()
{
    testAddress();
    return 0;
}

void testAddress()
{
    IPv4Address ipv4("127.0.0.1", 5000);
    IPv6Address ipv6("fe80::abcf:261:28e:b8e6", 5000);
    CCZOE_LOG_INFO(CCZOE_LOG_ROOT()) << ipv4.toString();
    CCZOE_LOG_INFO(CCZOE_LOG_ROOT()) << ipv4.subnetMask(24)->toString();
    CCZOE_LOG_INFO(CCZOE_LOG_ROOT()) << ipv6.toString();
    CCZOE_LOG_INFO(CCZOE_LOG_ROOT()) << ipv6.subnetMask(64)->toString();
}
