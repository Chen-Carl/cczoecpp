#include "Log/Log.h"
#include "Net/Address/sockaddr.h"

using namespace cczoe;
using namespace net;

void testAddress();
void testDNSLookup();
void testNICInfo();

int main()
{
    testAddress();
    testDNSLookup();
    testNICInfo();
    return 0;
}

void testAddress()
{
    IPv4Address ipv4("127.0.0.1", 5000);
    IPv6Address ipv6("fe80::18af:46ff:fe7f:2237", 5000);
    CCZOE_LOG_INFO(CCZOE_LOG_ROOT()) << "ipv4 address = " << ipv4.toString();
    CCZOE_LOG_INFO(CCZOE_LOG_ROOT()) << "ipv4 subnet mask = " << ipv4.subnetMask(24)->toString();
    CCZOE_LOG_INFO(CCZOE_LOG_ROOT()) << "ipv4 broadcast address = " << ipv4.broadcastAddress(24)->toString();
    CCZOE_LOG_INFO(CCZOE_LOG_ROOT()) << "ipv4 network address = " << ipv4.networkAddress(24)->toString();
    CCZOE_LOG_INFO(CCZOE_LOG_ROOT()) << "ipv6 address = " << ipv6.toString();
    CCZOE_LOG_INFO(CCZOE_LOG_ROOT()) << "ipv6 subnet mask = " << ipv6.subnetMask(64)->toString();
    CCZOE_LOG_INFO(CCZOE_LOG_ROOT()) << "ipv6 network address = " << ipv6.networkAddress(64)->toString();
    CCZOE_LOG_INFO(CCZOE_LOG_ROOT()) << "ipv6 broadcast address = " << ipv6.broadcastAddress(64)->toString();
}

void testDNSLookup()
{
    auto res = IPAddress::DNSLookup("www.baidu.com:http", AF_INET);
    if (res.has_value())
    {
        for (auto &addr : res.value())
        {
            CCZOE_LOG_INFO(CCZOE_LOG_ROOT()) << "DNS Lookup = " << addr->toString();
        }
    }
}

void testNICInfo()
{
    auto res = IPAddress::GetNICAddresses(AF_INET);
    if (res.has_value())
    {
        for (auto &info : res.value())
        {
            CCZOE_LOG_INFO(CCZOE_LOG_ROOT()) << "NIC " << info.first
            << " --> address = " << info.second.first->toString()
            << "/" << info.second.second;
        }
    }

    auto eno1Info = IPAddress::GetNICAddresses("eno1", AF_UNSPEC);
    if (eno1Info.has_value())
    {
        for (auto &info : eno1Info.value())
        {
            CCZOE_LOG_INFO(CCZOE_LOG_ROOT()) << "NIC eno1"
            << " --> address = " << info.first->toString()
            << "/" << info.second;
        }
    }
}