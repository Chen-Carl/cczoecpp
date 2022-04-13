#include <sys/types.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include "Scheduler/iomanager.h"
#include "Log/log.h"

using namespace cczoe;

void testScheduler();
void fiberFunc();

int main()
{
    testScheduler();
    return 0;
}

void testScheduler()
{
    IOManager iom("ioManager", 3);
    iom.schedule(&fiberFunc);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    fcntl(sock, F_SETFL, O_NONBLOCK);
    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);
    inet_pton(AF_INET, "180.97.34.94", &addr.sin_addr.s_addr);

    iom.addEvent(sock, IOManager::WRITE, [](){
        CCZOE_LOG_INFO(CCZOE_LOG_ROOT()) << "connected";
    });
    connect(sock, (const sockaddr*)&addr, sizeof(addr));
    while (1);
    iom.stop();
}

void fiberFunc()
{
    CCZOE_LOG_INFO(CCZOE_LOG_ROOT()) << "test fiber";
}