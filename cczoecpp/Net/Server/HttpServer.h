#ifndef __CCZOE_NET_SERVER_HTTPSERVER_H__
#define __CCZOE_NET_SERVER_HTTPSERVER_H__

#include "Net/Server/TcpServer.h"
#include "Net/Server/servlet.h"
#include "Net/Http/HttpSession.h"

namespace cczoe {
namespace net {

class HttpServer : public TcpServer
{
private:
    bool m_isKeepAlive;
    std::shared_ptr<ServletDispatcher> m_servletDispatcher;

public:
    HttpServer(bool keepAlive = false, IOManager *worker = IOManager::GetThis(), bool showLogs = false);

    virtual void handleClient(std::shared_ptr<Socket> sock) override;

    virtual bool bind(const std::shared_ptr<Address> addr) override;

    std::shared_ptr<ServletDispatcher> getServletDispatcher() const { return m_servletDispatcher; }

    void setServletDispatcher(std::shared_ptr<ServletDispatcher> servletDispatcher) { m_servletDispatcher = servletDispatcher; }
};

}
}

#endif