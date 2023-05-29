#ifndef __CCZOE_NET_HTTP_HTTPSESSION_H__
#define __CCZOE_NET_HTTP_HTTPSESSION_H__

#include "Net/Socket/socketStream.h"
#include "Net/Http/http.h"

namespace cczoe {
namespace http {

class HttpSession : public net::SocketStream
{
private:

public:
    HttpSession(std::shared_ptr<net::Socket> sock);

    std::shared_ptr<HttpRequest> recvRequest();
    int sendResponse(std::shared_ptr<HttpResponse> response);
};

}}

#endif