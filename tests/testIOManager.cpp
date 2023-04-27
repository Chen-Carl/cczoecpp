#include <format>
#include <sys/types.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include "Scheduler/iomanager.h"
#include "Log/log.h"
#include "Net/Hook/hook.h"

using namespace cczoe;

void testScheduler();
void fiberFunc();
void do_read(int sockfd);
void do_write(int sockfd);

int main()
{
    IOManager iom("testIOManager", 1);
    iom.schedule(fiberFunc);
    testScheduler();
    return 0;
}

void testScheduler()
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    fcntl(sock, F_SETFL, O_NONBLOCK);
    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);
    inet_pton(AF_INET, "180.101.50.242", &addr.sin_addr.s_addr);
    CCZOE_LOG_INFO(CCZOE_LOG_ROOT()) << "add event";

    int rt = connect(sock, (const sockaddr*)&addr, sizeof(addr));
    if (rt != 0)
    {
        CCZOE_LOG_ERROR(CCZOE_LOG_ROOT()) << "connect error";
        return;
    }
    IOManager::GetThis()->addEvent(sock, IOManager::WRITE, std::bind(do_write, sock));
    IOManager::GetThis()->addEvent(sock, IOManager::READ, std::bind(do_read, sock));
}

void fiberFunc()
{
    CCZOE_LOG_INFO(CCZOE_LOG_ROOT()) << "test fiber";
}

void do_read(int sockfd)
{
    CCZOE_LOG_INFO(CCZOE_LOG_ROOT()) << "read callback";
    char buf[2049]; // 修改数组长度为2049
    memset(buf, 0, sizeof(buf));
    int rt = recv(sockfd, buf, sizeof(buf) - 1, 0); // 接收数据时只接收2048个字节
    if (rt < 0)
    {
        CCZOE_LOG_INFO(CCZOE_LOG_ROOT()) << "read fail";
        return;
    }
    if (rt == 0)
    {
        CCZOE_LOG_INFO(CCZOE_LOG_ROOT()) << "peer closed";
        close(sockfd);
        return;
    }
    buf[rt] = '\0'; // 在buf数组的最后一个位置加上空字符
    CCZOE_LOG_INFO(CCZOE_LOG_ROOT()) << std::format("recv: {} bytes, {}", rt, buf);
    // CCZOE_LOG_INFO(CCZOE_LOG_ROOT()) << "rescheduling...";
    // IOManager::GetThis()->addEvent(sockfd, IOManager::READ, std::bind(do_read, sockfd));
}

void do_write(int sockfd)
{
    CCZOE_LOG_INFO(CCZOE_LOG_ROOT()) << "write callback";
    int so_err;
    socklen_t len = sizeof(so_err);
    int rt = getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &so_err, &len);
    if (rt < 0)
    {
        CCZOE_LOG_INFO(CCZOE_LOG_ROOT()) << "getsockopt fail";
        return;
    }
    if (so_err)
    {
        CCZOE_LOG_INFO(CCZOE_LOG_ROOT()) << "connect fail";
        return;
    }
    const char* httpRequest = "GET / HTTP/1.1\r\nConnection: close\r\nHost: www.baidu.com\r\n\r\n";
    send(sockfd, httpRequest, strlen(httpRequest), 0);
    CCZOE_LOG_INFO(CCZOE_LOG_ROOT()) << "write success";
    // IOManager::GetThis()->addEvent(sockfd, IOManager::WRITE, std::bind(do_write, sockfd));
}