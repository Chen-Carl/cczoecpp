#include "Net/Socket/socket.h"
#include "Net/Address/sockaddr.h"

using namespace cczoe;

class TcpClient
{
private:
    std::shared_ptr<net::Address> m_localAddr = nullptr;
    std::shared_ptr<net::Address> m_remoteAddr = nullptr;
    std::shared_ptr<net::Socket> m_sock;

public:
    TcpClient(const std::shared_ptr<net::Address> remote, const std::shared_ptr<net::Address> local = nullptr) : 
        m_localAddr(local),
        m_remoteAddr(remote)
    {
        m_sock = net::Socket::CreateTCP(local);
        if (m_localAddr != nullptr)
        {
            m_sock->bind(m_localAddr);
        }
    }

    ~TcpClient()
    {
        m_sock->close();
    }

    bool connect()
    {
        return m_sock->connect(m_remoteAddr);
    }

    bool send(size_t length)
    {
        std::string data(length, 'a');
        return m_sock->send(data.c_str(), data.size());
    }
};

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        std::cout << "usage: " << argv[0] << " IP PORT [-l] [ip[:port]] [-c] [size]" << std::endl;
        std::cout << "where IP is the server ip address" << std::endl;
        std::cout << "where PORT is the server port" << std::endl;
        std::cout << "-l [ip[:port]] identify the local ip address and port" << std::endl;
        std::cout << "-c [size] send size bytes data to server" << std::endl;
        return 0;
    }

    std::string localIp;
    uint16_t localPort = 0;
    size_t size = 10;

    for (int i = 3; i < argc; ++i)
    {
        std::string arg = argv[i];
        if (arg == "-l")
        {
            arg = argv[++i];
            if (arg.find(':') != std::string::npos)
            {
                localIp = arg.substr(0, arg.find(':'));
                localPort = std::atoi(arg.substr(arg.find(':') + 1).c_str());
            }
            else
            {
                localIp = arg;
            }
        }
        else if (arg == "-c")
        {
            arg = argv[++i];
            size = std::atoi(arg.c_str());
        }
        else
        {
            std::cout << "invalid argument: " << arg << std::endl;
            return 0;
        }
    }

    std::string ip = argv[1];
    uint16_t port = std::atoi(argv[2]);
    std::shared_ptr<net::IPv4Address> remoteAddr = std::make_shared<net::IPv4Address>(ip.c_str(), port);
    std::shared_ptr<net::IPv4Address> localAddr = nullptr;

    if (!localIp.empty())
    {
        localAddr = std::make_shared<net::IPv4Address>(localIp.c_str(), localPort);
    }
    TcpClient client(remoteAddr, localAddr);
    client.connect();
    client.send(size);
    return 0;
}