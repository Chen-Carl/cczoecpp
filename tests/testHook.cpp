#include <chrono>
#include "Net/Hook/hook.h"
#include "Net/Socket/socket.h"
#include "Net/Address/sockaddr.h"
#include "Scheduler/iomanager.h"
#include "utils/timeCounter.hpp"
#include "Log/Log.h"

using namespace cczoe;

void testSocket();
void testSleep();

int main()
{
    testSleep();
    testSocket();
    return 0;
}

void testSleep()
{
    IOManager iom("testHook", 1);
    auto sleepFor = [](int seconds) {
        sleep(seconds);
        CCZOE_LOG_INFO(CCZOE_LOG_ROOT()) << "sleep " << seconds << "s";
    };
    iom.schedule(std::bind(sleepFor, 5));
    iom.schedule(std::bind(sleepFor, 10));
}

void testSocket()
{
    std::shared_ptr<net::IPAddress> addr = std::make_shared<net::IPv4Address>("180.101.50.188", 80);
    std::shared_ptr<net::Socket> sock = net::Socket::CreateTCP(addr);
    addr->setPort(80);
    if (!sock->connect(addr))
    {
        return;
    }
    const char buff[] = "GET / HTTP/1.1\r\nHost: www.baidu.com\r\nConnection: close\r\n\r\n";
    int rt = sock->send(buff, sizeof(buff));
    if (rt <= 0)
    {
        std::cout << "send fail" << std::endl;
        return;
    }
    std::string buffs(4096, '\0');
    rt = sock->recv(&buffs[0], buffs.size());
    if (rt <= 0)
    {
        std::cout << "recv fail" << std::endl;
        return;
    }

    buffs.resize(rt);
    std::cout << buffs << std::endl;
}
