#include "bytearray.h"
#include "Log/log.h"
#include <string.h>
#include <fstream>

namespace cczoe {
namespace net {

ByteArray::Node::Node(size_t size) : 
    m_ptr(new char(size)), m_size(size), next(nullptr)
{

}

ByteArray::Node::Node() : 
    m_ptr(nullptr), m_size(0), next(nullptr)
{

}

ByteArray::Node::~Node()
{
    if (m_ptr)
    {
        delete [] m_ptr;
    }
}

ByteArray::ByteArray(size_t baseSize) :
    m_baseSize(baseSize),
    m_position(0),
    m_capacity(baseSize),
    m_size(0),
    m_endian(CCZOE_BIG_ENDIAN),
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

// write variable length int32_t data
void ByteArray::writeVint32(int32_t value)
{
    writeFint(EncodeZigzag32(value));
}

// write variable length uint32_t data
// google compress algorithm: the last bit of each 8 bits being 1 suggests the extra data
void ByteArray::writeVuint32(uint32_t value)
{
    uint8_t tmp[5];
    uint8_t i = 0;
    while (value >= 0x80)
    {
        tmp[i++] = (value & 0x7F) | 0x80;
        value >>= 7;
    }
    tmp[i++] = value;
    write(tmp, i);
}

// write variable length int64_t data
void ByteArray::writeVint64(int64_t value)
{
    writeFint(EncodeZigzag64(value));
}

// write variable length uint64_t data
void ByteArray::writeVuint64(uint64_t value)
{
    uint8_t tmp[10];
    uint8_t i = 0;
    while (value >= 0x80)
    {
        tmp[i++] = (value & 0x7F) | 0x80;
        value >>= 7;
    }
    tmp[i++] = value;
    write(tmp, i);
}

// write float data
void ByteArray::writeFloat(float value)
{
    uint32_t v;
    memcpy(&v, &value, sizeof(value));
    writeFint(v);
}

// write double data
void ByteArray::writeDouble(double value)
{
    uint64_t v;
    memcpy(&v, &value, sizeof(value));
    writeFint(v);
}

// write string
void ByteArray::writeStringF16(const std::string &value)
{
    writeFint((uint16_t)value.size());
    write(value.c_str(), value.size());
}

void ByteArray::writeStringF32(const std::string &value)
{
    writeFint((uint32_t)value.size());
    write(value.c_str(), value.size());
}

void ByteArray::writeStringF64(const std::string &value)
{
    writeFint((uint64_t)value.size());
    write(value.c_str(), value.size());
}

void ByteArray::writeStringVint(const std::string &value)
{
    writeFint((uint64_t)value.size());
    write(value.c_str(), value.size());
}

// write without length
void ByteArray::writeString(const std::string &value)
{
    write(value.c_str(), value.size());
}

template int8_t ByteArray::readFint<int8_t>();
template uint8_t ByteArray::readFint<uint8_t>();
template int16_t ByteArray::readFint<int16_t>();
template uint16_t ByteArray::readFint<uint16_t>();
template int32_t ByteArray::readFint<int32_t>();
template uint32_t ByteArray::readFint<uint32_t>();
template int64_t ByteArray::readFint<int64_t>();
template uint64_t ByteArray::readFint<uint64_t>();

int32_t ByteArray::readVint32()
{
    return DecodeZigzag(readVuint32());
}

uint32_t ByteArray::readVuint32()
{
    uint32_t result = 0;
    for (int i = 0; i < 32; i += 7)
    {
        uint8_t b = readFint<uint8_t>();
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

int64_t ByteArray::readVint64()
{
    return DecodeZigzag(readVuint64());
}

uint64_t ByteArray::readVuint64()
{
    uint64_t result = 0;
    for (int i = 0; i < 32; i += 7)
    {
        uint8_t b = readFint<uint8_t>();
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
    uint32_t v = readFint<uint32_t>();
    float result;
    memcpy(&result, &v, sizeof(v));
    return result;
}

double ByteArray::readDouble()
{
    uint64_t v = readFint<uint64_t>();
    float result;
    memcpy(&result, &v, sizeof(v));
    return result;
}

std::string ByteArray::readStringF16()
{
    uint16_t len = readFint<uint16_t>();
    std::string buf;
    buf.resize(len);
    read(&buf[0], len);
    return buf;
}
std::string ByteArray::readStringF32()
{
    uint32_t len = readFint<uint32_t>();
    std::string buf;
    buf.resize(len);
    read(&buf[0], len);
    return buf;
}

std::string ByteArray::readStringF64()
{
    uint64_t len = readFint<uint64_t>();
    std::string buf;
    buf.resize(len);
    read(&buf[0], len);
    return buf;
}

std::string ByteArray::readStringVint()
{
    uint64_t len = readVint64();
    std::string buf;
    buf.resize(len);
    read(&buf[0], len);
    return buf;
}

// clear byte array
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
    size_t npos = m_position % m_baseSize;  // position in current block
    size_t ncap = m_curr->m_size - npos;    // memory left in current block
    size_t bpos = 0;
    while (size > 0)
    {
        if (ncap >= size)
        {
            memcpy(m_curr->m_ptr + npos, (char*)buf + bpos, size);
            if (m_curr->m_size == npos + size)
            {
                m_curr = m_curr->next;
            }
            m_position += size;
            bpos += size;
            size = 0;
        }
        else
        {
            memcpy(m_curr->m_ptr + npos, (char*)buf + bpos, ncap); // fill current block
            m_position += ncap;
            bpos += ncap;
            size -= ncap;
            m_curr = m_curr->next;
            ncap = m_curr->m_size;
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
    if (size > getReadSize())
    {
        throw std::out_of_range("read out of range");
    }
    size_t npos = m_position % m_baseSize;
    size_t ncap = m_curr->m_size - npos;
    size_t bpos = 0;
    while (size > 0)
    {
        if (ncap >= size)
        {
            memcpy((char*)buf + bpos, m_curr->m_ptr + npos, size);
            if (m_curr->m_size == npos + size)
            {
                m_curr = m_curr->next;
            }
            m_position += size;
            bpos += size;
            size = 0;
        }
        else
        {
            memcpy((char*)buf + bpos, m_curr->m_ptr + npos, ncap);
            m_position += ncap;
            bpos += ncap;
            size -= ncap;
            m_curr = m_curr->next;
            ncap = m_curr->m_size;
            npos = 0;
        }
    }
}

// file operation
bool ByteArray::writeToFile(const std::string &name) const
{
    std::ofstream ofs;
    ofs.open(name, std::ios::trunc | std::ios::binary);
    if(!ofs) {
        CCZOE_LOG_ERROR(CCZOE_LOG_ROOT()) << "writeToFile name=" << name
            << " error , errno=" << errno << " errstr=" << strerror(errno);
        return false;
    }

    int64_t readSize = getReadSize();
    int64_t pos = m_position;
    Node *cur = m_curr;

    while(readSize > 0) 
    {
        int diff = pos % m_baseSize;
        int64_t len = (readSize + diff > (int64_t)m_baseSize ? m_baseSize - diff : readSize);
        ofs.write(cur->m_ptr + diff, len);
        cur = cur->next;
        pos += len;
        readSize -= len;
    }
    return true;
}

bool ByteArray::readFile(const std::string &name)
{
    return false;
}

// set current postiton
void ByteArray::setPosition(size_t pos)
{
    if (pos > m_size)
    {
        throw std::out_of_range("setPosition out of range");
    }
    m_position = pos;
    m_curr = m_root;
    while (pos >= m_curr->m_size)
    {
        pos -= m_curr->m_size;
        m_curr = m_curr->next;
    }
}

uint32_t ByteArray::EncodeZigzag32(int32_t v)
{
    if (v < 0)
    {
        return ((uint32_t)(-v)) * 2 - 1;
    }
    else
    {
        return v * 2;
    }
}

uint64_t ByteArray::EncodeZigzag64(int64_t v)
{
    if (v < 0)
    {
        return ((uint64_t)(-v)) * 2 - 1;
    }
    else
    {
        return v * 2;
    }
}

}}