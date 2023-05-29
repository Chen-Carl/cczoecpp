#ifndef __NET_SOCKET_STREAM_H__
#define __NET_SOCKET_STREAM_H__

#include "Net/ByteArray/bytearray.h"

namespace cczoe {
namespace net {

class Stream
{
public:
    virtual ~Stream() { }

    virtual int read(void *buffer, size_t length) = 0;
    virtual int read(std::shared_ptr<ByteArray> barray, size_t length) = 0;
    virtual int readFixSize(void *buffer, size_t length);
    virtual int readFixSize(std::shared_ptr<ByteArray> barray, size_t length);

    virtual int write(const void *buffer, size_t length) = 0;
    virtual int write(std::shared_ptr<ByteArray> barray, size_t length) = 0;
    virtual int writeFixSize(const void *buffer, size_t length);
    virtual int writeFixSize(std::shared_ptr<ByteArray> barray, size_t length);

    virtual int close() = 0;
};

}
}

#endif