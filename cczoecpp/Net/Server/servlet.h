#ifndef __CCZOE_NET_SERVER_SERVLET_H__
#define __CCZOE_NET_SERVER_SERVLET_H__

#include <functional>
#include <vector>
#include <unordered_map>
#include "Net/Http/http.h"
#include "Net/Http/HttpSession.h"

namespace cczoe {
namespace net {

class Servlet
{
public:
    Servlet() {}
    virtual ~Servlet() {}

    virtual int32_t handle(
        std::shared_ptr<http::HttpRequest> req, 
        std::shared_ptr<http::HttpResponse> resp,
        std::shared_ptr<http::HttpSession> session
    ) = 0;
};

class FunctionServlet : public Servlet
{
private:
    std::function<int32_t(
        std::shared_ptr<http::HttpRequest> req, 
        std::shared_ptr<http::HttpResponse> resp,
        std::shared_ptr<http::HttpSession> session
    )> m_callback;

public:
    FunctionServlet(
        std::function<int32_t(
            std::shared_ptr<http::HttpRequest> req, 
            std::shared_ptr<http::HttpResponse> resp,
            std::shared_ptr<http::HttpSession> session
        )> callback
    ) : m_callback(callback) {}

    int32_t handle(
        std::shared_ptr<http::HttpRequest> req, 
        std::shared_ptr<http::HttpResponse> resp,
        std::shared_ptr<http::HttpSession> session
    ) override;
};

// class NotFoundServlet : public Servlet
// {

// };

class ServletDispatcher : public Servlet
{
    using RWMutexType = thread::RWMutex;
private:
    std::unordered_map<std::string, std::shared_ptr<Servlet>> m_exact;
    std::vector<std::pair<std::string, std::shared_ptr<Servlet>>> m_fuzzy;
    RWMutexType m_mutex;

public:
    ServletDispatcher() { }

    void addServlet(const std::string &uri, std::shared_ptr<Servlet> servlet, bool fuzzy = false);
    void addServlet(const std::string &uri, std::function<int32_t(
        std::shared_ptr<http::HttpRequest> req, 
        std::shared_ptr<http::HttpResponse> resp,
        std::shared_ptr<http::HttpSession> session
    )> callback, bool fuzzy = false);
    void delServlet(const std::string &uri, bool fuzzy = false);
    std::vector<std::shared_ptr<Servlet>> getServlet(const std::string &uri, bool fuzzy = false);

    virtual int32_t handle(
        std::shared_ptr<http::HttpRequest> req, 
        std::shared_ptr<http::HttpResponse> resp,
        std::shared_ptr<http::HttpSession> session
    ) override;

    std::shared_ptr<Servlet> findServlet(std::string_view uri) const;

    std::unordered_map<std::string, std::shared_ptr<Servlet>> getExactServlet(const std::string &uri) const { return m_exact; }
    std::vector<std::pair<std::string, std::shared_ptr<Servlet>>> getFuzzyServlet(const std::string &uri) const { return m_fuzzy; }
};

}
}

#endif