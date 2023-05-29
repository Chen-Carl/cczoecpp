#ifndef __CCZOECPP_NET_HTTP_HTTP_H__
#define __CCZOECPP_NET_HTTP_HTTP_H__

#include <cstdint>
#include <cstring>
#include <optional>
#include <string>
#include <map>
#include <boost/lexical_cast.hpp>

namespace cczoe {
namespace http {

/* Request Methods */
#define HTTP_METHOD_MAP(XX)           \
	XX(0,  DELETE,      DELETE)       \
	XX(1,  GET,         GET)          \
	XX(2,  HEAD,        HEAD)         \
	XX(3,  POST,        POST)         \
	XX(4,  PUT,         PUT)          \
	/* pathological */                \
	XX(5,  CONNECT,     CONNECT)      \
	XX(6,  OPTIONS,     OPTIONS)      \
	XX(7,  TRACE,       TRACE)        \
	/* WebDAV */                      \
	XX(8,  COPY,        COPY)         \
	XX(9,  LOCK,        LOCK)         \
	XX(10, MKCOL,       MKCOL)        \
	XX(11, MOVE,        MOVE)         \
	XX(12, PROPFIND,    PROPFIND)     \
	XX(13, PROPPATCH,   PROPPATCH)    \
	XX(14, SEARCH,      SEARCH)       \
	XX(15, UNLOCK,      UNLOCK)       \
	XX(16, BIND,        BIND)         \
	XX(17, REBIND,      REBIND)       \
	XX(18, UNBIND,      UNBIND)       \
	XX(19, ACL,         ACL)          \
	/* subversion */                  \
	XX(20, REPORT,      REPORT)       \
	XX(21, MKACTIVITY,  MKACTIVITY)   \
	XX(22, CHECKOUT,    CHECKOUT)     \
	XX(23, MERGE,       MERGE)        \
	/* upnp */                        \
	XX(24, MSEARCH,     M-SEARCH)     \
	XX(25, NOTIFY,      NOTIFY)       \
	XX(26, SUBSCRIBE,   SUBSCRIBE)    \
	XX(27, UNSUBSCRIBE, UNSUBSCRIBE)  \
	/* RFC-5789 */                    \
	XX(28, PATCH,       PATCH)        \
	XX(29, PURGE,       PURGE)        \
	/* CalDAV */                      \
	XX(30, MKCALENDAR,  MKCALENDAR)   \
	/* RFC-2068, section 19.6.1.2 */  \
	XX(31, LINK,        LINK)         \
	XX(32, UNLINK,      UNLINK)       \
	/* icecast */                     \
	XX(33, SOURCE,      SOURCE)

enum class HttpMethod
{
#define XX(num, name, string) name = num,
  	HTTP_METHOD_MAP(XX)
#undef XX
};

/* Status Codes */
#define HTTP_STATUS_MAP(XX)                                                   \
	XX(100, CONTINUE,                        Continue)                        \
	XX(101, SWITCHING_PROTOCOLS,             Switching Protocols)             \
	XX(102, PROCESSING,                      Processing)                      \
	XX(200, OK,                              OK)                              \
	XX(201, CREATED,                         Created)                         \
	XX(202, ACCEPTED,                        Accepted)                        \
	XX(203, NON_AUTHORITATIVE_INFORMATION,   Non-Authoritative Information)   \
	XX(204, NO_CONTENT,                      No Content)                      \
	XX(205, RESET_CONTENT,                   Reset Content)                   \
	XX(206, PARTIAL_CONTENT,                 Partial Content)                 \
	XX(207, MULTI_STATUS,                    Multi-Status)                    \
	XX(208, ALREADY_REPORTED,                Already Reported)                \
	XX(226, IM_USED,                         IM Used)                         \
	XX(300, MULTIPLE_CHOICES,                Multiple Choices)                \
	XX(301, MOVED_PERMANENTLY,               Moved Permanently)               \
	XX(302, FOUND,                           Found)                           \
	XX(303, SEE_OTHER,                       See Other)                       \
	XX(304, NOT_MODIFIED,                    Not Modified)                    \
	XX(305, USE_PROXY,                       Use Proxy)                       \
	XX(307, TEMPORARY_REDIRECT,              Temporary Redirect)              \
	XX(308, PERMANENT_REDIRECT,              Permanent Redirect)              \
	XX(400, BAD_REQUEST,                     Bad Request)                     \
	XX(401, UNAUTHORIZED,                    Unauthorized)                    \
	XX(402, PAYMENT_REQUIRED,                Payment Required)                \
	XX(403, FORBIDDEN,                       Forbidden)                       \
	XX(404, NOT_FOUND,                       Not Found)                       \
	XX(405, METHOD_NOT_ALLOWED,              Method Not Allowed)              \
	XX(406, NOT_ACCEPTABLE,                  Not Acceptable)                  \
	XX(407, PROXY_AUTHENTICATION_REQUIRED,   Proxy Authentication Required)   \
	XX(408, REQUEST_TIMEOUT,                 Request Timeout)                 \
	XX(409, CONFLICT,                        Conflict)                        \
	XX(410, GONE,                            Gone)                            \
	XX(411, LENGTH_REQUIRED,                 Length Required)                 \
	XX(412, PRECONDITION_FAILED,             Precondition Failed)             \
	XX(413, PAYLOAD_TOO_LARGE,               Payload Too Large)               \
	XX(414, URI_TOO_LONG,                    URI Too Long)                    \
	XX(415, UNSUPPORTED_MEDIA_TYPE,          Unsupported Media Type)          \
	XX(416, RANGE_NOT_SATISFIABLE,           Range Not Satisfiable)           \
	XX(417, EXPECTATION_FAILED,              Expectation Failed)              \
	XX(421, MISDIRECTED_REQUEST,             Misdirected Request)             \
	XX(422, UNPROCESSABLE_ENTITY,            Unprocessable Entity)            \
	XX(423, LOCKED,                          Locked)                          \
	XX(424, FAILED_DEPENDENCY,               Failed Dependency)               \
	XX(426, UPGRADE_REQUIRED,                Upgrade Required)                \
	XX(428, PRECONDITION_REQUIRED,           Precondition Required)           \
	XX(429, TOO_MANY_REQUESTS,               Too Many Requests)               \
	XX(431, REQUEST_HEADER_FIELDS_TOO_LARGE, Request Header Fields Too Large) \
	XX(451, UNAVAILABLE_FOR_LEGAL_REASONS,   Unavailable For Legal Reasons)   \
	XX(500, INTERNAL_SERVER_ERROR,           Internal Server Error)           \
	XX(501, NOT_IMPLEMENTED,                 Not Implemented)                 \
	XX(502, BAD_GATEWAY,                     Bad Gateway)                     \
	XX(503, SERVICE_UNAVAILABLE,             Service Unavailable)             \
	XX(504, GATEWAY_TIMEOUT,                 Gateway Timeout)                 \
	XX(505, HTTP_VERSION_NOT_SUPPORTED,      HTTP Version Not Supported)      \
	XX(506, VARIANT_ALSO_NEGOTIATES,         Variant Also Negotiates)         \
	XX(507, INSUFFICIENT_STORAGE,            Insufficient Storage)            \
	XX(508, LOOP_DETECTED,                   Loop Detected)                   \
	XX(510, NOT_EXTENDED,                    Not Extended)                    \
	XX(511, NETWORK_AUTHENTICATION_REQUIRED, Network Authentication Required)

enum class HttpStatus
{
#define XX(num, name, string) name = num,
  	HTTP_STATUS_MAP(XX)
#undef XX
};

struct CaseInsensitiveLess
{
	bool operator()(const std::string& lhs, const std::string& rhs) const
	{
		return strcasecmp(lhs.c_str(), rhs.c_str()) < 0;
	}
};

class HttpRequest
{
private:
	using MapType = std::map<std::string, std::string, CaseInsensitiveLess>;

	HttpMethod m_method;
	HttpStatus m_status;
	uint8_t m_version;
	bool m_close;
	std::string m_path;
	std::string m_query;
	std::string m_fragment;
	std::string m_body;
	MapType m_headers;
	MapType m_params;
	MapType m_cookies;

public:
	HttpRequest(uint8_t version = 17, bool close = false);

	const HttpMethod &getMethod() const { return m_method; }
	const HttpStatus &getStatus() const { return m_status; }
	uint8_t getVersion() const { return m_version; }
	bool isClose() const { return m_close; }
	std::string_view getPath() const { return m_path; }
	std::string_view getQuery() const { return m_query; }
	std::string_view getFragment() const { return m_fragment; }
	std::string_view getBody() const { return m_body; }
	const MapType &getHeaders() const { return m_headers; }
	const MapType &getParams() const { return m_params; }
	const MapType &getCookies() const { return m_cookies; }
	std::optional<std::string_view> getHeader(const std::string &key) const;
	std::optional<std::string_view> getParam(const std::string &key) const;
	std::optional<std::string_view> getCookie(const std::string &key) const;

	void setMethod(const HttpMethod &method) { m_method = method; }
	void setStatus(const HttpStatus &status) { m_status = status; }
	void setVersion(uint8_t version) { m_version = version; }
	void setClose(bool close) { m_close = close; }
	void setPath(std::string_view path) { m_path = path; }
	void setQuery(std::string_view query) { m_query = query; }
	void setFragment(std::string_view fragment) { m_fragment = fragment; }
	void setBody(std::string_view body) { m_body = body; }
	void setHeaders(const MapType &headers) { m_headers = headers; }
	void setParams(const MapType &params) { m_params = params; }
	void setCookies(const MapType &cookies) { m_cookies = cookies; }
	void setHeader(const std::string &key, const std::string &value) { m_headers[key] = value; }
	void setParam(const std::string &key, const std::string &value) { m_params[key] = value; }
	void setCookie(const std::string &key, const std::string &value) { m_cookies[key] = value; }
	void delHeader(const std::string &key) { m_headers.erase(key); }
	void delParam(const std::string &key) { m_params.erase(key); }
	void delCookie(const std::string &key) { m_cookies.erase(key); }

	std::ostream &dump(std::ostream &os) const;
};

class HttpResponse
{
private:
	using MapType = std::map<std::string, std::string, CaseInsensitiveLess>;

	HttpStatus m_status;
	uint8_t m_version;
	bool m_close;
	std::string m_body;
	std::string m_reason;
	MapType m_headers;
public:
	HttpResponse(uint8_t version = 17, bool close = false);

	const HttpStatus &getStatus() const { return m_status; }
	uint8_t getVersion() const { return m_version; }
	bool isClose() const { return m_close; }
	std::string_view getBody() const { return m_body; }
	std::string_view getReason() const { return m_reason; }
	const MapType &getHeaders() const { return m_headers; }
	std::optional<std::string_view> getHeader(const std::string &key) const;

	void setStatus(const HttpStatus &status) { m_status = status; }
	void setVersion(uint8_t version) { m_version = version; }
	void setClose(bool close) { m_close = close; }
	void setBody(std::string_view body) { m_body = body; }
	void setReason(std::string_view reason) { m_reason = reason; }
	void setHeaders(const MapType &headers) { m_headers = headers; }
	void setHeader(const std::string &key, const std::string &value) { m_headers[key] = value; }
	void delHeader(const std::string &key) { m_headers.erase(key); }

	std::ostream &dump(std::ostream &os) const;
};

template <typename T, class MapType = std::map<std::string, std::string, CaseInsensitiveLess>>
std::optional<T> getAs(const MapType &map, const std::string &key)
{
	if (map.count(key))
	{
		try {
			return boost::lexical_cast<T>(map.at(key));
		}
		catch (const boost::bad_lexical_cast &) {
			return std::nullopt;
		}
	}
	return std::nullopt;
}

HttpMethod stringToHttpMethod(const std::string& method);
HttpMethod stringToHttpMethod(const char* method);
const std::string httpMethodToString(HttpMethod method);
const std::string httpStatusToString(HttpStatus status);

}
}

#endif