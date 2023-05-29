#include "Net/Server/HttpServer.h"
#include "Net/Http/HttpSession.h"
#include "HttpServer.h"

namespace cczoe {
namespace net {

net::HttpServer::HttpServer(bool keepAlive, IOManager *worker, bool showLogs) :
    TcpServer(worker, showLogs),
    m_isKeepAlive(keepAlive)
{
    m_servletDispatcher = std::make_shared<ServletDispatcher>();
}

void HttpServer::handleClient(std::shared_ptr<Socket> sock)
{
    TcpServer::handleClient(sock);
    std::shared_ptr<http::HttpSession> session = std::make_shared<http::HttpSession>(sock);
    do {
        std::shared_ptr<http::HttpRequest> request = session->recvRequest();
        if (!request)
        {
            break;
        }
        std::shared_ptr<http::HttpResponse> response = std::make_shared<http::HttpResponse>(request->getVersion(), request->isClose() && !m_isKeepAlive);
        response->setBody("http response from cll");
        session->sendResponse(response);
    } while (m_isKeepAlive);
}

bool HttpServer::bind(const std::shared_ptr<Address> addr)
{
    if (m_isKeepAlive)
    {
        CCZOE_LOG_INFO(CCZOE_LOG_ROOT()) << "HttpServer Connection: keep-alive";
    }
    else
    {
        CCZOE_LOG_INFO(CCZOE_LOG_ROOT()) << "HttpServer Connection: close";
    }
    return TcpServer::bind(addr);
}

}
}