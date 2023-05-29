#include <format>
#include "Net/Http/HttpParser.h"
#include "Log/Log.h"
#include "HttpParser.h"

namespace cczoe {
namespace http {

HttpRequestParser::HttpRequestParser()
{
    // request init
    m_data = std::make_shared<HttpRequest>();
    // http parser init
    http_parser_init(&m_parser, HTTP_REQUEST);
    m_parser.data = this;
    // http parser settings init
    http_parser_settings_init(&m_settings);
    m_settings.on_url = +[](http_parser* parser, const char *at, size_t length) {
        http_parser_url urlParser;
        http_parser_url_init(&urlParser);
        int rt = http_parser_parse_url(at, length, 0, &urlParser);
        if (rt != 0) 
        {
            CCZOE_LOG_ERROR(CCZOE_LOG_ROOT()) << "http_parser_parse_url error: " << rt;
            return 1;
        }
        HttpRequestParser *p = static_cast<HttpRequestParser*>(parser->data);
        if (urlParser.field_set & (1 << UF_PATH)) 
        {
            p->getData()->setPath(std::string(at + urlParser.field_data[UF_PATH].off, urlParser.field_data[UF_PATH].len));
        }
        if (urlParser.field_set & (1 << UF_QUERY)) 
        {
            p->getData()->setQuery(std::string(at + urlParser.field_data[UF_QUERY].off, urlParser.field_data[UF_QUERY].len));
        }
        if (urlParser.field_set & (1 << UF_FRAGMENT)) 
        {
            p->getData()->setFragment(std::string(at + urlParser.field_data[UF_FRAGMENT].off, urlParser.field_data[UF_FRAGMENT].len));
        }
        return 0;
    };
    m_settings.on_status = +[](http_parser* parser, const char *at, size_t length) {
        return 0;
    };
    m_settings.on_header_field = +[](http_parser* parser, const char *at, size_t length) {
        HttpRequestParser *p = static_cast<HttpRequestParser*>(parser->data);
        p->setCurrField(std::string(at, length));
        return 0;
    };
    m_settings.on_header_value = +[](http_parser* parser, const char *at, size_t length) {
        HttpRequestParser *p = static_cast<HttpRequestParser*>(parser->data);
        p->m_data->setHeader(p->getCurrField(), std::string(at, length));
        return 0;
    };
    m_settings.on_body = +[](http_parser* parser, const char *at, size_t length) {
        HttpRequestParser *p = static_cast<HttpRequestParser*>(parser->data);
        p->m_data->setBody(std::string(at, length));
        return 0;
    };
    m_settings.on_message_begin = +[](http_parser* parser) {
        return 0;
    };
    m_settings.on_headers_complete = +[](http_parser* parser) {
        HttpRequestParser *p = static_cast<HttpRequestParser *>(parser->data);
        p->getData()->setVersion(((parser->http_major) << 0x4) | (parser->http_minor));
        p->getData()->setMethod((HttpMethod)(parser->method));
        return 0;
    };
    m_settings.on_message_complete = +[](http_parser* parser) {
        HttpRequestParser *p = static_cast<HttpRequestParser *>(parser->data);
        p->m_finished = true;
        return 0;
    };
    m_settings.on_chunk_header = +[](http_parser* parser) {
        return 0;
    };
    m_settings.on_chunk_complete = +[](http_parser* parser) {
        return 0;
    };
}

size_t HttpRequestParser::execute(const char *data, size_t len)
{
    size_t nparsed = http_parser_execute(&m_parser, &m_settings, data, len);
    return nparsed;
}

HttpResponseParser::HttpResponseParser()
{
    m_data = std::make_shared<HttpResponse>();
    http_parser_init(&m_parser, HTTP_RESPONSE);
    m_parser.data = this;
    http_parser_settings_init(&m_settings);
    m_settings.on_url = +[](http_parser* parser, const char *at, size_t length) {
        return 0;
    };
    m_settings.on_status = +[](http_parser* parser, const char *at, size_t length) {
        HttpResponseParser *p = static_cast<HttpResponseParser*>(parser->data);
        p->getData()->setStatus(HttpStatus(parser->status_code));
        return 0;
    };
    m_settings.on_header_field = +[](http_parser* parser, const char *at, size_t length) {
        HttpResponseParser *p = static_cast<HttpResponseParser*>(parser->data);
        p->setCurrField(std::string(at, length));
        return 0;
    };
    m_settings.on_header_value = +[](http_parser* parser, const char *at, size_t length) {
        HttpResponseParser *p = static_cast<HttpResponseParser*>(parser->data);
        p->getData()->setHeader(p->getCurrField(), std::string(at, length));
        return 0;
    };
    m_settings.on_body = +[](http_parser* parser, const char *at, size_t length) {
        HttpResponseParser *p = static_cast<HttpResponseParser*>(parser->data);
        p->getData()->setBody(std::string(at, length));
        return 0;
    };
    m_settings.on_message_begin = +[](http_parser* parser) {
        return 0;
    };
    m_settings.on_headers_complete = +[](http_parser* parser) {
        HttpRequestParser *p = static_cast<HttpRequestParser *>(parser->data);
        p->getData()->setVersion(((parser->http_major) << 0x4) | (parser->http_minor));
        p->getData()->setMethod((HttpMethod)(parser->method));
        return 0;
    };
    m_settings.on_message_complete = +[](http_parser* parser) {
        return 0;
    };
    m_settings.on_chunk_header = +[](http_parser* parser) {
        return 0;
    };
    m_settings.on_chunk_complete = +[](http_parser* parser) {
        return 0;
    };
}

size_t HttpResponseParser::execute(const char *data, size_t len)
{
    size_t nparsed = http_parser_execute(&m_parser, &m_settings, data, len);
    return nparsed;
}

}
}