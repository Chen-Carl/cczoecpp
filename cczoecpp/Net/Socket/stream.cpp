#include "stream.h"

namespace cczoe {
namespace net {

int Stream::readFixSize(void *buffer, size_t length)
{
    if (length <= 0)
    {
        return 0;
    }
    size_t offset = 0;
    while (offset < length)
    {
        int rt = read((char *)buffer + offset, length - offset);
        if (rt <= 0)
        {
            return rt;
        }
        offset += rt;
    }
    return offset;
}

int Stream::readFixSize(std::shared_ptr<ByteArray> barray, size_t length)
{
    if (length <= 0)
    {
        return 0;
    }
    size_t offset = 0;
    while (offset < length)
    {
        int rt = read(barray, length - offset);
        if (rt <= 0)
        {
            return rt;
        }
        offset += rt;
    }
    return offset;
}

int Stream::writeFixSize(const void *buffer, size_t length)
{
    if (length <= 0)
    {
        return 0;
    }
    size_t offset = 0;
    while (offset < length)
    {
        int rt = write((const char *)buffer + offset, length - offset);
        if (rt <= 0)
        {
            return rt;
        }
        offset += rt;
    }
    return offset;
}

int Stream::writeFixSize(std::shared_ptr<ByteArray> barray, size_t length)
{
    if (length <= 0)
    {
        return 0;
    }
    size_t offset = 0;
    while (offset < length)
    {
        int rt = write(barray, length - offset);
        if (rt <= 0)
        {
            return rt;
        }
        offset += rt;
    }
    return offset;
}

}}