#include <format>
#include "Net/Server/TcpServer.h"
#include "Log/log.h"
#include "TcpServer.h"

namespace cczoe {
namespace net {

TcpServer::TcpServer(IOManager *worker) : m_worker(worker)
{

}

TcpServer::~TcpServer()
{
    for (auto &sock : m_socks)
    {
        sock->close();
    }
    m_socks.clear();
}

void TcpServer::accept(std::shared_ptr<Socket> sock)
{
    while (!m_isStop)
    {
        std::shared_ptr<Socket> client = sock->accept();
        if (client)
        {
            client->setRecvTimeout(1000 * 60 * 2);
            m_worker->schedule(std::bind(&TcpServer::handleClient, shared_from_this(), client));
        }
        else
        {
            CCZOE_LOG_ERROR(CCZOE_LOG_ROOT()) << std::format("accept fail: errno={}, strerr={}", errno, strerror(errno));
        }
    }
}

void TcpServer::handleClient(std::shared_ptr<Socket> client)
{
    CCZOE_LOG_INFO(CCZOE_LOG_ROOT()) << "handle client: " << *client;
}

bool TcpServer::bind(const std::shared_ptr<Address> addr)
{
    std::vector<std::shared_ptr<Address>> addrs;
    std::vector<bool> success;
    addrs.push_back(addr);
    return bind(addrs, success);
}

bool TcpServer::bind(const std::vector<std::shared_ptr<Address>> &addrs, std::vector<bool> &success)
{
    success.resize(addrs.size(), true);
    for (size_t i = 0; i < addrs.size(); i++)
    {
        std::shared_ptr<Socket> sock = Socket::CreateTCP(addrs[i]);
        if (!sock->bind(addrs[i]))
        {
            CCZOE_LOG_ERROR(CCZOE_LOG_ROOT()) << std::format("socket bind fail: addr={}, errno={}, strerr={}", addrs[i]->toString(), errno, strerror(errno));
            success[i] = false;
            continue;
        }
        if (!sock->listen())
        {
            CCZOE_LOG_ERROR(CCZOE_LOG_ROOT()) << std::format("socket listen fail: addr={}, errno={}, strerr={}", addrs[i]->toString(), errno, strerror(errno));
            success[i] = false;
            continue;
        }
        m_socks.push_back(sock);
        CCZOE_LOG_INFO(CCZOE_LOG_ROOT()) << std::format("socket listen success: addr={}", addrs[i]->toString());
    }
    return std::all_of(success.begin(), success.end(), [](bool i) { return i; });
}

bool TcpServer::start()
{
    if (!m_isStop)
    {
        return true;
    }
    m_isStop = false;
    for (auto &sock : m_socks)
    {
        m_worker->schedule(std::bind(&TcpServer::accept, shared_from_this(), sock));
    }
    return true;
}

bool TcpServer::stop()
{
    m_isStop = true;
    auto self = shared_from_this();
    m_worker->schedule([self]() {
        for (auto &sock : self->m_socks)
        {
            sock->cancelAll();
            sock->close();
        }
        self->m_socks.clear();
    });
    return true;
}

}
}