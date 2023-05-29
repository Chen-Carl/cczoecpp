#include "Net/Http/HttpSession.h"
#include "Net/Http/HttpParser.h"
#include "HttpSession.h"

namespace cczoe {
namespace http {

HttpSession::HttpSession(std::shared_ptr<net::Socket> sock) :
    net::SocketStream(sock)
{

}

std::shared_ptr<HttpRequest> HttpSession::recvRequest()
{
    std::shared_ptr<HttpRequestParser> parser = std::make_shared<HttpRequestParser>();

    const uint64_t buffSize = 4096;
    std::shared_ptr<char> buffer(new char[buffSize], [](char *p) {
        delete[] p; 
    });

    int total = 0;
    while (!parser->finished())
    {
        int len = read(buffer.get() + total, buffSize - total);
        if (len <= 0)
        {
            close();
            return nullptr;
        }
        total += len;
        size_t nparse = parser->execute(buffer.get(), total);
        if (parser->error() || nparse <= 0) 
        {
            close();
            return nullptr;
        }
    }
    return parser->getData();
}

int HttpSession::sendResponse(std::shared_ptr<HttpResponse> response)
{
    std::stringstream ss;
    response->dump(ss);
    std::string data = ss.str();
    return writeFixSize(data.c_str(), data.size());
}

}
}