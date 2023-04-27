#include "Net/Socket/socket.h"
#include "Net/Address/sockaddr.h"

using namespace cczoe;

void testSocket();

int main()
{
    testSocket();
    return 0;
}

void testSocket()
{
    auto addrs = net::IPAddress::DNSLookup("www.baidu.com");
    if (addrs.has_value())
    {
        std::shared_ptr<net::IPAddress> addr = addrs.value().front();
        std::shared_ptr<net::Socket> sock = net::Socket::CreateTCP(addr);
        addr->setPort(80);
        if (!sock->connect(addr))
        {
            return;
        }
        const char buff[] = "GET / HTTP/1.1\r\nHost: www.baidu.com\r\nConnection: close\r\n\r\n";
        int rt = sock->send(buff, sizeof(buff));
        if (rt <= 0)
        {
            std::cout << "send fail" << std::endl;
            return;
        }
        std::string buffs(4096, '\0');
        rt = sock->recv(&buffs[0], buffs.size());
        if (rt <= 0)
        {
            std::cout << "recv fail" << std::endl;
            return;
        }

        buffs.resize(rt);
        std::cout << buffs << std::endl;
    }
}
