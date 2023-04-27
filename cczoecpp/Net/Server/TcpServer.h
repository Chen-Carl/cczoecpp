#ifndef __NET_SERVER_TCPSERVER_H__
#define __NET_SERVER_TCPSERVER_H__

#include <memory>
#include <vector>
#include "Scheduler/iomanager.h"
#include "Net/Socket/socket.h"

namespace cczoe {

namespace net {

class TcpServer : public boost::noncopyable, public std::enable_shared_from_this<TcpServer>
{
private:
    std::string m_name = "cczoe/1.0";
    uint64_t m_readTimeout = 60 * 1000;
    bool m_isStop = true;
    IOManager *m_worker;
    std::vector<std::shared_ptr<Socket>> m_socks;

    void accept(std::shared_ptr<Socket> sock);
    virtual void handleClient(std::shared_ptr<Socket> client);

public:
    TcpServer(IOManager *worker = IOManager::GetThis());
    virtual ~TcpServer();

    virtual bool bind(const std::shared_ptr<Address> addr);
    virtual bool bind(const std::vector<std::shared_ptr<Address>> &addrs, std::vector<bool> &success);
    virtual bool start();
    virtual bool stop();

    std::string getName() const { return m_name; }
    uint64_t getReadTimeout() const { return m_readTimeout; }
    bool isStop() const { return m_isStop; }

    void setName(const std::string &name) { m_name = name; }
    void setReadTimeout(uint64_t timeout) { m_readTimeout = timeout; }
};

}
}

#endif