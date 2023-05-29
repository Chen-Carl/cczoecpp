#ifndef __CCZOE_NET_HTTP_HTTPPARSER_H__
#define __CCZOE_NET_HTTP_HTTPPARSER_H__

#include <memory>
#include "Net/Http/http.h"
#include "Net/Http/parser/http_parser.h"
#include "Log/Log.h"

namespace cczoe {
namespace http {

class HttpRequestParser
{
private:
    http_parser m_parser;
    http_parser_settings m_settings;
    std::shared_ptr<HttpRequest> m_data;
    bool m_finished = false;
    std::string m_currField;

public:
    HttpRequestParser();

    size_t execute(const char *data, size_t len);

    std::shared_ptr<HttpRequest> getData() const { return m_data; }
    const std::string &getCurrField() const { return m_currField; }
    int error() const { return m_parser.http_errno; }
    bool finished() const { return m_finished; }

    void setCurrField(const std::string &field) { m_currField = field; }
};

class HttpResponseParser
{
private:
    http_parser m_parser;
    http_parser_settings m_settings;
    std::shared_ptr<HttpResponse> m_data;
    std::string m_currField;

public:
    HttpResponseParser();

    size_t execute(const char *data, size_t len);

    std::shared_ptr<HttpResponse> getData() const { return m_data; }
    const std::string &getCurrField() const { return m_currField; }

    void setCurrField(const std::string &field) { m_currField = field; }
};

}
}

#endif