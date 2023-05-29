#include "Net/Server/HttpServer.h"
#include "Net/Address/sockaddr.h"

using namespace cczoe;

void run();

int main()
{
    IOManager iom("test_httpServer", 2);
    iom.schedule(run);
    return 0;
}

void run()
{
    auto addrs = net::IPAddress::DNSLookup("0.0.0.0:6054");
    if (addrs.has_value())
    {
        std::shared_ptr<net::IPAddress> addr = addrs.value().front();
        std::shared_ptr<net::HttpServer> server = std::make_shared<net::HttpServer>(IOManager::GetThis());
        while (!server->bind(addr))
        {
            sleep(2);
        }
        server->start();
    }
    else
    {
        CCZOE_LOG_ERROR(CCZOE_LOG_ROOT()) << "DNSLookup fail";
    }
}