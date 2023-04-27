#ifndef __CCZOE_BYTEARRAY_H__
#define __CCZOE_BYTEARRAY_H__

#include <memory>
#include <string>
#include <vector>
#include <sys/uio.h>
#include "endian.h"

namespace cczoe {
namespace net {

class ByteArray
{
public:
    // single list - definition of memory block
    struct Node
    {
        char *ptr;              // memory pointer
        size_t size;            // block size
        Node *next;             // next block

        Node(size_t size);
        Node();
        ~Node();
    };

public:
    ByteArray(size_t baseSize = 4096);
    ~ByteArray();

    // write fixed length int
    void writeFint8(int8_t value);
    void writeFuint8(uint8_t value);
    void writeFint16(int16_t value);
    void writeFuint16(uint16_t value);
    void writeFint32(int32_t value);
    void writeFuint32(uint32_t value);
    void writeFint64(int64_t value);
    void writeFuint64(uint64_t value);

    // write float and double
    void writeFloat(float value);
    void writeDouble(double value);

    // fixed length string
    void writeStringF16(const std::string &value);
    void writeStringF32(const std::string &value);
    void writeStringF64(const std::string &value);
    void writeStringVint(const std::string &value);

    // write variable length int
    void writeInt32(int32_t value);
    void writeUint32(uint32_t value);
    void writeInt64(int64_t value);
    void writeUint64(uint64_t value);

    // read fixed length int
    int8_t readFint8();
    uint8_t readFuint8();
    int16_t readFint16();
    uint16_t readFuint16();
    int32_t readFint32();
    uint32_t readFuint32();
    int64_t readFint64();
    uint64_t readFuint64();

    // read variable length int
    int32_t readInt32();
    uint32_t readUint32();
    int64_t readInt64();
    uint64_t readUint64();

    float readFloat();
    double readDouble();

    // read fixed length string
    std::string readStringF16();
    std::string readStringF32();
    std::string readStringF64();
    std::string readStringVint();

    void clear();
    void write(const void *buf, size_t size);
    void read(void *buf, size_t size);
    void read(void *buf, size_t size, size_t pos) const;

    // debug methods
    bool writeToFile(const std::string &name) const;
    bool readFromFile(const std::string &name);
    std::string toString() const;
    std::string toHexString() const;

    // get methods
    size_t getBaseSize() const { return m_baseSize; }
    size_t getReadableSize() const { return m_size - m_position; }
    size_t getCapacity() const { return m_capacity - m_position; }
    size_t getSize() const { return m_size; }
    bool isLittleEndian() const { return m_endian == CCZOE_LITTLE_ENDIAN; }

    // set methods
    void setLittleEndian() { m_endian = CCZOE_LITTLE_ENDIAN; }
    void setBigEndian() { m_endian = CCZOE_BIG_ENDIAN; }
    void setPosition(size_t pos);

    // memory operations
    uint64_t addWriteBuffers(std::vector<iovec> &buff, uint64_t len);
    uint64_t addReadBuffers(std::vector<iovec> &buff, uint64_t len) const;

private:
    size_t m_baseSize;      // size of memory block
    size_t m_position;      // current position
    size_t m_capacity;      // size of memory
    size_t m_size;          // size of data
    int8_t m_endian;        // byte sequence, default big endian
    Node *m_root;           // pointer to the first memory block
    Node *m_curr;           // current memory block

    // add new memory
    void addCapacity(size_t size);

    // zigzag encoding
    static uint32_t EncodeZigzag32(int32_t v);
    static uint64_t EncodeZigzag64(int64_t v);
    static int32_t DecodeZigzag32(uint32_t v);
    static int64_t DecodeZigzag64(uint64_t v);
};

}}

#endif