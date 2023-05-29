#include "Net/Server/servlet.h"
#include "servlet.h"

namespace cczoe {
namespace net {

int32_t FunctionServlet::handle(std::shared_ptr<http::HttpRequest> req, std::shared_ptr<http::HttpResponse> resp, std::shared_ptr<http::HttpSession> session)
{
    return m_callback(req, resp, session);
}

void ServletDispatcher::addServlet(const std::string &uri, std::shared_ptr<Servlet> servlet, bool fuzzy)
{
    thread::WriteScopedLock<RWMutexType> lock(m_mutex);
    if (!fuzzy)
    {
        m_exact[uri] = servlet;
    }
    else
    {

    }
}

void ServletDispatcher::addServlet(const std::string &uri, std::function<int32_t(std::shared_ptr<http::HttpRequest> req, std::shared_ptr<http::HttpResponse> resp, std::shared_ptr<http::HttpSession> session)> callback, bool fuzzy)
{
    thread::WriteScopedLock<RWMutexType> lock(m_mutex);
    if (!fuzzy)
    {
        m_exact[uri] = std::make_shared<FunctionServlet>(callback);
    }
    else
    {

    }
}

void ServletDispatcher::delServlet(const std::string &uri, bool fuzzy)
{
    thread::WriteScopedLock<RWMutexType> lock(m_mutex);
    if (!fuzzy)
    {
        m_exact.erase(uri);
    }
    else
    {

    }
}

std::vector<std::shared_ptr<Servlet>> ServletDispatcher::getServlet(const std::string &uri, bool fuzzy)
{
    return std::vector<std::shared_ptr<Servlet>>();
}

int32_t ServletDispatcher::handle(std::shared_ptr<http::HttpRequest> req, std::shared_ptr<http::HttpResponse> resp, std::shared_ptr<http::HttpSession> session)
{
    findServlet(req->getPath())->handle(req, resp, session);
    return 0;
}

std::shared_ptr<Servlet> ServletDispatcher::findServlet(std::string_view uri) const
{
    return std::shared_ptr<Servlet>();
}

}
}