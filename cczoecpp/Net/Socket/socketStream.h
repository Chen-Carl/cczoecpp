#ifndef __NET_SOCKET_SOCKETSTREAM_H__
#define __NET_SOCKET_SOCKETSTREAM_H__

#include "Net/Socket/socket.h"
#include "Net/Socket/stream.h"

namespace cczoe {
namespace net {

class SocketStream : public Stream
{
private:
    std::shared_ptr<Socket> m_sock;

public:
    SocketStream(std::shared_ptr<Socket> sock);
    virtual ~SocketStream();

    virtual int read(void *buffer, size_t length) override;
    virtual int read(std::shared_ptr<ByteArray> barray, size_t length) override;

    virtual int write(const void *buffer, size_t length) override;
    virtual int write(std::shared_ptr<ByteArray> barray, size_t length) override;

    std::shared_ptr<Socket> getSocket() const { return m_sock; }
};

}}

#endif