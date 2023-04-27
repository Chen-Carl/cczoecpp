#include "socketStream.h"

namespace cczoe {
namespace net {

SocketStream::SocketStream(std::shared_ptr<Socket> sock) :
    m_sock(sock)
{

}

SocketStream::~SocketStream()
{

}

int SocketStream::read(void *buffer, size_t length)
{
    if (!m_sock || !m_sock->isConnected())
    {
        return -1;
    }
    return m_sock->recv(buffer, length);
}

int SocketStream::read(std::shared_ptr<ByteArray> barray, size_t length)
{
    if (!m_sock || !m_sock->isConnected())
    {
        return -1;
    }
    std::vector<iovec> iovs;
    barray->addWriteBuffers(iovs, length);
    return m_sock->recv(&iovs[0], iovs.size());
}

}}