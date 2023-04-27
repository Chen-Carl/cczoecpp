#include <sstream>
#include <fstream>
#include <iomanip>
#include <string.h>
#include "bytearray.h"
#include "Log/log.h"

namespace cczoe {
namespace net {

ByteArray::Node::Node() :
    ptr(nullptr), size(0), next(nullptr)
{

}

ByteArray::Node::Node(size_t s) :
    ptr(new char[s]), size(s), next(nullptr)
{

}

ByteArray::Node::~Node()
{
    if (ptr)
    {
        delete [] ptr;
    }
}

ByteArray::ByteArray(size_t baseSize) : 
    m_baseSize(baseSize),
    m_position(0),
    m_capacity(baseSize),
    m_size(0),
    m_endian(CCZOE_LITTLE_ENDIAN),
    m_root(new Node(baseSize)),
    m_curr(m_root)
{

}

ByteArray::~ByteArray()
{
    Node *tmp = m_root;
    while (tmp)
    {
        m_curr = tmp;
        tmp = tmp->next;
        delete m_curr;
    }
}

// write fixed length int
void ByteArray::writeFint8(int8_t value)
{
    write(&value, sizeof(value));
}

void ByteArray::writeFuint8(uint8_t value)
{
    write(&value, sizeof(value));
}

void ByteArray::writeFint16(int16_t value)
{
    if (m_endian != CCZOE_BYTE_ORDER)
    {
        value = byteswap(value);
    }
    write(&value, sizeof(value));
}

void ByteArray::writeFuint16(uint16_t value)
{
    if (m_endian != CCZOE_BYTE_ORDER)
    {
        value = byteswap(value);
    }
    write(&value, sizeof(value));
}

void ByteArray::writeFint32(int32_t value)
{
    if (m_endian != CCZOE_BYTE_ORDER)
    {
        value = byteswap(value);
    }
    write(&value, sizeof(value));
}

void ByteArray::writeFuint32(uint32_t value)
{
    if (m_endian != CCZOE_BYTE_ORDER)
    {
        value = byteswap(value);
    }
    write(&value, sizeof(value));
}

void ByteArray::writeFint64(int64_t value)
{
    if (m_endian != CCZOE_BYTE_ORDER)
    {
        value = byteswap(value);
    }
    write(&value, sizeof(value));
}

void ByteArray::writeFuint64(uint64_t value)
{
    if (m_endian != CCZOE_BYTE_ORDER)
    {
        value = byteswap(value);
    }
    write(&value, sizeof(value));
}

// write float and double
void ByteArray::writeFloat(float value)
{
    uint32_t v;
    memcpy(&v, &value, sizeof(value));
    writeFuint32(v);
}

void ByteArray::writeDouble(double value)
{
    uint64_t v;
    memcpy(&v, &value, sizeof(value));
    writeFuint64(v);
}

// fixed length string
void ByteArray::writeStringF16(const std::string &value)
{
    writeFuint16(value.size());
    write(value.c_str(), value.size());
}

void ByteArray::writeStringF32(const std::string &value)
{
    writeFuint32(value.size());
    write(value.c_str(), value.size());
}

void ByteArray::writeStringF64(const std::string &value)
{
    writeFuint64(value.size());
    write(value.c_str(), value.size());
}

void ByteArray::writeStringVint(const std::string &value)
{
    writeFuint64(value.size());
    write(value.c_str(), value.size());
}

// write variable length int
void ByteArray::writeInt32(int32_t value)
{
    writeUint32(EncodeZigzag32(value));
}

void ByteArray::writeUint32(uint32_t value)
{
    uint8_t tmp[5];
    uint8_t i = 0;
    while (value >= 0x80)
    {
        tmp[i++] = (value & 0x7f) | 0x80;
        value >>= 7;
    }
    tmp[i++] = value;
    write(tmp, i);
}

void ByteArray::writeInt64(int64_t value)
{
    writeUint64(EncodeZigzag64(value));
}

void ByteArray::writeUint64(uint64_t value)
{
    uint8_t tmp[10];
    uint8_t i = 0;
    while (value >= 0x80)
    {
        tmp[i++] = (value & 0x7f) | 0x80;
        value >>= 7;
    }
    tmp[i++] = value;
    write(tmp, i);
}

// read fixed length int
int8_t ByteArray::readFint8()
{
    int8_t v;
    read(&v, sizeof(v));
    return v;
}

uint8_t ByteArray::readFuint8()
{
    uint8_t v;
    read(&v, sizeof(v));
    return v;
}

#define XX(type) \
    type v; \
    read(&v, sizeof(v)); \
    if (m_endian == CCZOE_BYTE_ORDER) \
    { \
        return v; \
    } \
    return byteswap(v)

int16_t ByteArray::readFint16()
{
    XX(int16_t);
}

uint16_t ByteArray::readFuint16()
{
    XX(uint16_t);
}

int32_t ByteArray::readFint32()
{
    XX(int32_t);
}

uint32_t ByteArray::readFuint32()
{
    XX(uint32_t);
}

int64_t ByteArray::readFint64()
{
    XX(int64_t);
}

uint64_t ByteArray::readFuint64()
{
    XX(uint64_t);
}

#undef XX

// read variable length int
int32_t ByteArray::readInt32()
{
    return DecodeZigzag32(readUint32());
}

uint32_t ByteArray::readUint32()
{
    uint32_t result = 0;
    for (int i = 0; i < 32; i += 7)
    {
        uint8_t b = readFuint8();
        if (b < 0x80)
        {
            result |= ((uint32_t)b) << i;
            break;
        }
        else
        {
            result |= (((uint32_t)(b & 0x7f)) << i);
        }
    }
    return result;
}

int64_t ByteArray::readInt64()
{
    return DecodeZigzag64(readUint64());
}

uint64_t ByteArray::readUint64()
{
    uint64_t result = 0;
    for (int i = 0; i < 32; i += 7)
    {
        uint8_t b = readFuint8();
        if (b < 0x80)
        {
            result |= ((uint64_t)b) << i;
            break;
        }
        else
        {
            result |= (((uint64_t)(b & 0x7f)) << i);
        }
    }
    return result;
}

float ByteArray::readFloat()
{
    uint32_t v = readFuint32();
    float result;
    memcpy(&result, &v, sizeof(v));
    return result;
}

double ByteArray::readDouble()
{
    uint64_t v = readFuint64();
    double result;
    memcpy(&result, &v, sizeof(v));
    return result;
}

// read fixed length string
std::string ByteArray::readStringF16()
{
    uint16_t len = readFuint16();
    std::string buff;
    buff.resize(len);
    read(&buff[0], len);
    return buff;
}

std::string ByteArray::readStringF32()
{
    uint32_t len = readFuint32();
    std::string buff;
    buff.resize(len);
    read(&buff[0], len);
    return buff;
}

std::string ByteArray::readStringF64()
{
    uint64_t len = readFuint64();
    std::string buff;
    buff.resize(len);
    read(&buff[0], len);
    return buff;
}

std::string ByteArray::readStringVint()
{
    uint64_t len = readFuint64();
    std::string buff;
    buff.resize(len);
    read(&buff[0], len);
    return buff;
}

void ByteArray::clear()
{
    m_position = 0;
    m_size = 0;
    m_capacity = m_baseSize;
    Node *tmp = m_root->next;
    while (tmp)
    {
        m_curr = tmp;
        tmp = tmp->next;
        delete m_curr;
    }
    m_curr = m_root;
    m_root->next = nullptr;
}

void ByteArray::write(const void *buf, size_t size)
{
    if (size <= 0)
    {
        return;
    }
    addCapacity(size);
    size_t npos = m_position % m_baseSize;
    size_t ncap = m_baseSize - npos;
    size_t bpos = 0;
    while (size > 0)
    {
        if (ncap >= size)
        {
            memcpy(m_curr->ptr + npos, (char*)buf + bpos, size);
            if (npos + size == m_curr->size)
            {
                m_curr = m_curr->next;
            }
            m_position += size;
            bpos += size;
            size = 0;
        }
        else
        {
            memcpy(m_curr->ptr + npos, (char*)buf + bpos, ncap);
            m_position += ncap;
            bpos += ncap;
            size -= ncap;
            m_curr = m_curr->next;
            ncap = m_curr->size;
            npos = 0;
        }
    }

    if (m_position > m_size)
    {
        m_size = m_position;
    }
}

void ByteArray::read(void *buf, size_t size)
{
    if (size > getReadableSize())
    {
        throw std::out_of_range("read size out of range");
    }

    size_t npos = m_position % m_baseSize;
    size_t ncap = m_curr->size - npos;
    size_t bpos = 0;
    while (size > 0)
    {
        if (ncap >= size)
        {
            memcpy((char*)buf + bpos, m_curr->ptr + npos, size);
            if (m_curr->size == npos + size)
            {
                m_curr = m_curr->next;
            }
            m_position += size;
            bpos += size;
            size = 0;
        }
        else
        {
            memcpy((char*)buf + bpos, m_curr->ptr + npos, ncap);
            m_position += ncap;
            bpos += ncap;
            size -= ncap;
            m_curr = m_curr->next;
            ncap = m_curr->size;
            npos = 0;
        }
    }
}

void ByteArray::read(void *buf, size_t size, size_t pos) const
{
    if (size > getReadableSize())
    {
        throw std::out_of_range("read size out of range");
    }

    size_t npos = m_position % m_baseSize;
    size_t ncap = m_curr->size - npos;
    size_t bpos = 0;
    Node *curr = m_curr;
    while (size > 0)
    {
        if (ncap >= size)
        {
            memcpy((char*)buf + bpos, curr->ptr + npos, size);
            if (curr->size == npos + size)
            {
                curr = curr->next;
            }
            pos += size;
            bpos += size;
            size = 0;
        }
        else
        {
            memcpy((char*)buf + bpos, curr->ptr + npos, ncap);
            pos += ncap;
            bpos += ncap;
            size -= ncap;
            curr = curr->next;
            ncap = curr->size;
            npos = 0;
        }
    }
}

// debug methods
bool ByteArray::writeToFile(const std::string &name) const
{
    std::ofstream ofs;
    ofs.open(name, std::ios::trunc | std::ios::binary);
    if (!ofs)
    {
        CCZOE_LOG_ERROR(CCZOE_LOG_ROOT()) << "write to file name = " << name << " error, errno = " << errno << " errstr = " << strerror(errno);
        return false;
    }
    int64_t readSize = getReadableSize();
    int64_t pos = m_position;
    Node *curr = m_curr;
    while (readSize > 0)
    {
        int npos = pos % m_baseSize;
        int64_t len = (readSize > (int64_t)m_baseSize ? m_baseSize : readSize) - npos;
        ofs.write(curr->ptr + npos, len);
        curr = curr->next;
        pos += len;
        readSize -= len;
    }
    return true;
}

bool ByteArray::readFromFile(const std::string &name)
{
    std::ifstream ifs;
    ifs.open(name, std::ios::binary);
    if (!ifs)
    {
        CCZOE_LOG_ERROR(CCZOE_LOG_ROOT()) << "read from file name = " << name << " error, errno = " << errno << " errstr = " << strerror(errno);
        return false;
    }
    std::shared_ptr<char> buff(new char[m_baseSize], [](char *ptr) { delete [] ptr; });
    while (!ifs.eof())
    {
        ifs.read(buff.get(), m_baseSize);
        write(buff.get(), ifs.gcount());
    }
    return true;
}

std::string ByteArray::toString() const
{
    std::string res;
    res.resize(getReadableSize());
    if (res.size() > 0)
    {
        read(&res[0], res.size(), m_position);
    }
    return res;
}

std::string ByteArray::toHexString() const
{
    std::string str = toString();
    std::stringstream ss;
    for (size_t i = 0; i < str.size(); i++)
    {
        if (i > 0 && i % 32 == 0)
        {
            ss << std::endl;
        }
        ss << std::setw(2) << std::setfill('0') << std::hex << (int)(uint8_t)str[i] << " ";
    }
    return ss.str();
}

void ByteArray::addCapacity(size_t size)
{
    if (size == 0)
    {
        return;
    }
    size_t oldCap = getCapacity();
    if (oldCap >= size)
    {
        return;
    }
    size_t count = (size / m_baseSize) + ((size % m_baseSize > oldCap) ? 1 : 0);
    Node *tmp = m_root;
    while (tmp->next)
    {
        tmp = tmp->next;
    }
    Node *first = nullptr;
    for (size_t i = 0; i < count; i++)
    {
        tmp->next = new Node(m_baseSize);
        if (first == nullptr)
        {
            first = tmp;
        }
        tmp = tmp->next;
        m_capacity += m_baseSize;
    }
    if (oldCap == 0)
    {
        m_curr = first;
    }
}

uint64_t ByteArray::addWriteBuffers(std::vector<iovec> &buff, uint64_t len)
{
    if (len <= 0)
    {
        return 0;
    }
    addCapacity(len);
    uint64_t size = len;
    size_t npos = m_position % m_baseSize;
    size_t ncap = m_curr->size - npos;
    Node *curr = m_curr;
    while (len > 0)
    {
        iovec iov;
        if (ncap > len)
        {
            iov.iov_base = curr->ptr + npos;
            iov.iov_len = len;
            m_size += len;
            len = 0;
        }
        else
        {
            iov.iov_base = curr->ptr + npos;
            iov.iov_len = ncap;
            m_size += ncap;
            len -= ncap;
            curr = curr->next;
            ncap = curr->size;
            npos = 0;
        }
        buff.push_back(iov);
    }
    return size;
}

uint64_t ByteArray::addReadBuffers(std::vector<iovec> &buff, uint64_t len) const
{
    len = len > getReadableSize() ? getReadableSize() : len;
    if (len == 0)
    {
        return 0;
    }

    uint64_t size = len;
    size_t npos = m_position % m_baseSize;
    size_t ncap = m_curr->size - npos;
    Node *curr = m_curr;
    iovec iov;

    while (len > 0)
    {
        if (ncap >= len)
        {
            iov.iov_base = curr->ptr + npos;
            iov.iov_len = len;
            len = 0;
        }
        else
        {
            iov.iov_base = curr->ptr + npos;
            iov.iov_len = ncap;
            len -= ncap;
            curr = curr->next;
            ncap = curr->size;
            npos = 0;
        }
        buff.push_back(iov);
    }
    return size;
}

// zigzag encoding
uint32_t ByteArray::EncodeZigzag32(int32_t v)
{
    if (v < 0)
    {
        return ((uint32_t)(-v)) * 2 - 1;
    }
    return v * 2;
}

uint64_t ByteArray::EncodeZigzag64(int64_t v)
{
    if (v < 0)
    {
        return ((uint64_t)(-v)) * 2 - 1;
    }
    return v * 2;
}

int32_t ByteArray::DecodeZigzag32(uint32_t v)
{
    return (v >> 1) ^ (-(v & 1));
}

int64_t ByteArray::DecodeZigzag64(uint64_t v)
{
    return (v >> 1) ^ (-(v & 1));
}

void ByteArray::setPosition(size_t pos)
{
    if (pos > m_size)
    {
        throw std::out_of_range("setPosition out of range");
    }
    m_position = pos;
    while (pos >= m_curr->size)
    {
        pos -= m_curr->size;
        m_curr = m_curr->next;
    }
}

}}