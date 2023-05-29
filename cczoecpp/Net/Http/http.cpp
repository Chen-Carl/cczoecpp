#include "Net/Http/http.h"
#include "http.h"

namespace cczoe {
namespace http {

HttpRequest::HttpRequest(uint8_t version, bool close) : 
    m_version(version),
    m_close(close)
{
    m_method = HttpMethod::GET;
    m_path = "/";
}

std::optional<std::string_view> HttpRequest::getHeader(const std::string &key) const
{
    if (m_headers.count(key))
    {
        return m_headers.at(key);
    }
    return std::nullopt;
}

std::optional<std::string_view> HttpRequest::getParam(const std::string &key) const
{
    if (m_params.count(key))
    {
        return m_params.at(key);
    }
    return std::nullopt;
}

std::optional<std::string_view> HttpRequest::getCookie(const std::string &key) const
{
    if (m_cookies.count(key))
    {
        return m_cookies.at(key);
    }
    return std::nullopt;
}

std::ostream &HttpRequest::dump(std::ostream &os) const
{
    os << httpMethodToString(m_method) << " " << m_path;
    if (!m_query.empty())
    {
        os << "?" << m_query;
    }
    if (!m_fragment.empty())
    {
        os << "#" << m_fragment;
    }
    os << " HTTP/" << (uint32_t)(m_version >> 4) << "." << (uint32_t)(m_version & 0x0F) << "\r\n";
    if (m_close)
    {
        os << "connection: close\r\n";
    }
    else
    {
        os << "connection: keep-alive\r\n";
    }
    for (auto &header : m_headers)
    {
        os << header.first << ": " << header.second << "\r\n";
    }
    if (!m_body.empty())
    {
        if (!m_headers.count("Content-Length"))
        {
            os << "Content-Length: " << m_body.size() << "\r\n\r\n" << m_body;
        }
        else
        {
            os << "\r\n" << m_body;
        }
    }
    else
    {
        os << "\r\n";
    }
    return os;
}

HttpResponse::HttpResponse(uint8_t version, bool close) :
    m_version(version),
    m_close(close)
{
    m_status = HttpStatus::OK;
}

std::optional<std::string_view> HttpResponse::getHeader(const std::string &key) const
{
    if (m_headers.count(key))
    {
        return m_headers.at(key);
    }
    return std::nullopt;
}

std::ostream &HttpResponse::dump(std::ostream &os) const
{
    os << "HTTP/" << (uint32_t)(m_version >> 4) << "." << (uint32_t)(m_version & 0x0F) << " "
       << (uint32_t)m_status << " " << httpStatusToString(m_status) << "\r\n";
    if (!m_headers.count("connection"))
    {
        if (m_close)
        {
            os << "Connection: close\r\n";
        }
        else
        {
            os << "Connection: keep-alive\r\n";
        }
    }
    for (auto &header : m_headers)
    {
        os << header.first << ": " << header.second << "\r\n";
    }
    if (!m_body.empty())
    {
        os << "content-length: " << m_body.size() << "\r\n\r\n" << m_body;
    }
    else
    {
        os << "\r\n";
    }
    return os;
}

HttpMethod stringToHttpMethod(const std::string& method)
{
#define XX(num, name, string) \
    if (method == #string) \
    { \
        return HttpMethod::name; \
    }
    HTTP_METHOD_MAP(XX)
#undef XX
    throw std::invalid_argument("invalid http method");
}

HttpMethod stringToHttpMethod(const char* method)
{
#define XX(num, name, string) \
    if (strcmp(method, #string) == 0) \
    { \
        return HttpMethod::name; \
    }
    HTTP_METHOD_MAP(XX)
#undef XX
    throw std::invalid_argument("invalid http method");
}

const std::string httpMethodToString(HttpMethod method)
{
#define XX(num, name, string) \
    if (method == HttpMethod::name) \
    { \
        return #string; \
    }
    HTTP_METHOD_MAP(XX)
#undef XX
    throw std::invalid_argument("invalid http method");
}

const std::string httpStatusToString(HttpStatus status)
{
#define XX(num, name, string) \
    if (status == HttpStatus::name) \
    { \
        return #string; \
    }
    HTTP_STATUS_MAP(XX)
#undef XX
    throw std::invalid_argument("invalid http status");
}

}
}