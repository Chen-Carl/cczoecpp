#include <memory>
#include <iostream>
#include "Net/Http/http.h"
#include "Net/Http/HttpParser.h"

using namespace cczoe;
using namespace http;

void testRequest();
void testResponse();
void testRequestParser();
void testResponseParser();

int main()
{
    // testRequest();
    // testResponse();
    // testRequestParser();
    testResponseParser();
    return 0;
}

void testRequest()
{
    std::shared_ptr<HttpRequest> req = std::make_shared<HttpRequest>();
    req->setHeader("host", "www.baidu.com");
    req->setBody("hello http");
    req->dump(std::cout) << std::endl;
}

void testResponse()
{
    std::shared_ptr<HttpResponse> res = std::make_shared<HttpResponse>();
    res->setBody("hello http");
    res->dump(std::cout) << std::endl;
}

void testRequestParser()
{
    std::string req = R"EOF(POST /login?aa=bb#sss HTTP/1.1
Host: www.baidu.top
Content-Length: 10

1234567890
    )EOF";
    std::cout << "request: " << std::endl << req << std::endl;
    HttpRequestParser parser;
    parser.execute(req.c_str(), req.length());
    auto parsedReq = parser.getData();
    std::cout << "========== parsing result ========== " << std::endl;
    parsedReq->dump(std::cout) << std::endl;
}

void testResponseParser()
{
    std::string resp = R"EOF(HTTP/1.1 200 OK
Accept-Ranges: bytes
Cache-Control: no-cache
Connection: keep-alive
Content-Length: 9508
Content-Security-Policy: frame-ancestors 'self' https://chat.baidu.com https://fj-chat.baidu.com https://hba-chat.baidu.com https://hbe-chat.baidu.com https://njjs-chat.baidu.com https://nj-chat.baidu.com https://hna-chat.baidu.com https://hnb-chat.baidu.com;
Content-Type: text/html
Date: Sun, 28 May 2023 06:02:55 GMT
P3p: CP=" OTI DSP COR IVA OUR IND COM "
P3p: CP=" OTI DSP COR IVA OUR IND COM "
Pragma: no-cache
Server: BWS/1.1
Set-Cookie: BAIDUID=5E4B05AECCFA11E345F8D509B00D8078:FG=1; expires=Thu, 31-Dec-37 23:55:55 GMT; max-age=2147483647; path=/; domain=.baidu.com
Set-Cookie: BIDUPSID=5E4B05AECCFA11E345F8D509B00D8078; expires=Thu, 31-Dec-37 23:55:55 GMT; max-age=2147483647; path=/; domain=.baidu.com
Set-Cookie: PSTM=1685253775; expires=Thu, 31-Dec-37 23:55:55 GMT; max-age=2147483647; path=/; domain=.baidu.com
Set-Cookie: BAIDUID=5E4B05AECCFA11E396BAE4D632845B10:FG=1; max-age=31536000; expires=Mon, 27-May-24 06:02:55 GMT; domain=.baidu.com; path=/; version=1; comment=bd
Traceid: 168525377506240788587633582380958965070
Vary: Accept-Encoding
X-Ua-Compatible: IE=Edge,chrome=1

<!DOCTYPE html>)EOF";
    std::cout << "response: " << std::endl << resp << std::endl;
    HttpResponseParser parser;
    parser.execute(resp.c_str(), resp.length());
    auto parsedResp = parser.getData();
    std::cout << "========== parsing result ========== " << std::endl;
    parsedResp->dump(std::cout) << std::endl;
}