#include <format>
#include <functional>
#include <string>
#include "Net/Server/HttpServer.h"
#include "Net/Address/sockaddr.h"

using namespace cczoe;

void run(std::string_view ip, uint16_t port, bool keepAlive, bool showLogs);

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        std::cout << "usage: " << argv[0] << " IP PORT [-t threads] [-k]" << std::endl;
        std::cout << "where IP is the listening ip address" << std::endl;
        std::cout << "where PORT is the listening port" << std::endl;
        std::cout << "[-t threads] threads number to handle client requests" << std::endl;
        std::cout << "[-k] set to keep alive" << std::endl;
        std::cout << "[-log] set to print logs" << std::endl;
        return 0;
    }
    int threads = 1;
    bool keepAlive = false;
    bool showLogs = false;
    for (int i = 3; i < argc; i++)
    {
        if (strcmp(argv[i], "-t") == 0)
        {
            threads = std::atoi(argv[++i]);
            continue;
        }
        if (strcmp(argv[i], "-k") == 0)
        {
            keepAlive = true;
            continue;
        }
        if (strcmp(argv[i], "-log") == 0)
        {
            showLogs = true;
            continue;
        }
    }
    std::string ip = argv[1];
    uint16_t port = std::atoi(argv[2]);
    IOManager iom("test_httpServer", threads);
    iom.schedule(std::bind(run, ip, port, keepAlive, showLogs));
    return 0;
}

void run(std::string_view ip, uint16_t port, bool keepAlive, bool showLogs)
{
    std::string host = std::format("{}:{}", ip, port);
    auto addrs = net::IPAddress::DNSLookup(host);
    if (addrs.has_value())
    {
        std::shared_ptr<net::IPAddress> addr = addrs.value().front();
        std::shared_ptr<net::HttpServer> server = std::make_shared<net::HttpServer>(keepAlive, IOManager::GetThis(), showLogs);
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