#include <vector>
#include <format>
#include "Net/Server/TcpServer.h"
#include "Net/Socket/socket.h"
#include "Net/ByteArray/bytearray.h"
#include "Net/Address/sockaddr.h"

using namespace cczoe;

class EchoServer : public net::TcpServer
{
private:
    int m_type;
    std::shared_ptr<net::TcpServer> m_server;  

public:
    EchoServer(int type = 1) : m_type(type)
    {
        m_server = std::make_shared<net::TcpServer>();
    }

    virtual void handleClient(std::shared_ptr<net::Socket> client) override
    {
        CCZOE_LOG_INFO(CCZOE_LOG_ROOT()) << "handle client: " << *client;
        std::shared_ptr<net::ByteArray> ba = std::make_shared<net::ByteArray>();
        while (1)
        {
            ba->clear();
            std::vector<iovec> iovs;
            ba->addWriteBuffers(iovs, 1024 * 1024);
            int rt = client->recv(&iovs[0], iovs.size());
            if (rt == 0)
            {
                CCZOE_LOG_INFO(CCZOE_LOG_ROOT()) << "client close: " << *client;
                break;
            }
            else if (rt < 0)
            {
                CCZOE_LOG_ERROR(CCZOE_LOG_ROOT()) << std::format("client error: errno={}, strerr={}", errno, strerror(errno));
                break;
            }
            ba->setPosition(0);
            if (m_type == 1)
            {
                CCZOE_LOG_INFO(CCZOE_LOG_ROOT()) << "recv: " << ba->toString().substr(0, rt);
                client->send(ba->toString().substr(0, rt).c_str(), strlen(ba->toString().substr(0, rt).c_str()));
            }
            else
            {
                CCZOE_LOG_INFO(CCZOE_LOG_ROOT()) << "recv length: " << rt << " data: " << ba->toHexString();
            }
        }
    }
};

void run(std::string_view ip, uint16_t port);

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        std::cout << "usage: " << argv[0] << " IP PORT" << std::endl;
        std::cout << "where IP is the listening ip address" << std::endl;
        std::cout << "where PORT is the listening port" << std::endl;
        return 0;
    }
    std::string ip = argv[1];
    uint16_t port = std::atoi(argv[2]);
    IOManager iom("default", 1);
    iom.schedule(std::bind(run, ip, port));
    return 0;
}

void run(std::string_view ip, uint16_t port)
{
    std::string host = std::format("{}:{}", ip, port);
    auto addrs = net::IPAddress::DNSLookup(host);
    if (addrs.has_value())
    {
        std::shared_ptr<net::IPAddress> addr = addrs.value().front();
        std::shared_ptr<EchoServer> es = std::make_shared<EchoServer>();
        while (!es->bind(addr))
        {
            sleep(2);
        }
        es->start();
    }
    else
    {
        CCZOE_LOG_ERROR(CCZOE_LOG_ROOT()) << "DNSLookup fail";
    }
}