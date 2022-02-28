#ifndef __CCZOE_BYTEARRAY_H__
#define __CCZOE_BYTEARRAY_H__

#include <memory>
#include <string>
#include "endian.h"

namespace cczoe {
namespace net {

class ByteArray
{
public:
    // definition of memory block
    struct Node
    {
        char *m_ptr;
        size_t m_size;
        Node *next;

        Node(size_t size);
        Node();
        ~Node();
    };

public:
    ByteArray(size_t baseSize = 4096);
    ~ByteArray();

    // write fixed int data
    // Int = int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t
    template <class Int>
    void writeFint(Int value)
    {
        if (m_endian != CCZOE_BYTE_ORDER)
        {
            value = byteSwap(value);
        }
        write(&value, sizeof(value));
    }

    // write variable length int32_t data
    void writeVint32(int32_t value);

    // write variable length uint32_t data
    void writeVuint32(uint32_t value);

    // write variable length int64_t data
    void writeVint64(int64_t value);

    // write variable length uint64_t data
    void writeVuint64(uint64_t value);

    // write float data
    void writeFloat(float value);

    // write double data
    void writeDouble(double value);

    // write string
    void writeStringF16(const std::string &value);
    void writeStringF32(const std::string &value);
    void writeStringF64(const std::string &value);
    void writeStringVint(const std::string &value);

    // write without length
    void writeString(const std::string &value);

    template <class Int>
    Int readFint()
    {
        Int v;
        read(&v, sizeof(v));
        if (m_endian == CCZOE_BYTE_ORDER)
        {
            return v;
        }
        else
        {
            return byteSwap(v);
        }
    }

    int32_t readVint32();
    uint32_t readVuint32();
    int64_t readVint64();
    uint64_t readVuint64();

    float readFloat();
    double readDouble();

    std::string readStringF16();
    std::string readStringF32();
    std::string readStringF64();
    std::string readStringVint();

    // clear byte array
    void clear();
    
    /** 
     * @brief write data in the "buf" with length = size
     * @param[in] buf buffer pointer
     * @param[in] size data size
     */
    void write(const void *buf, size_t size);

    /** 
     * @brief read data with length = size
     * @param[out] buf buffer pointer
     * @param[in] size data size
     */
    void read(void *buf, size_t size);

    /** 
     * @brief read data with length = size
     * @param[out] buf buffer pointer
     * @param[in] size data size
     * @param[in] position start position
     */
    // void read(void *buf, size_t size, size_t position) const;

    // file operation
    bool writeToFile(const std::string &name) const;
    bool readFile(const std::string &name);

    // set current postiton
    void setPosition(size_t pos);

    // return current position of the byte array
    size_t getPosition() const { return m_position; }

    // get base size of memory block
    size_t getBaseSize() const { return m_baseSize; }

    // get writable memory
    size_t getCapacity() const { return m_capacity - m_position; }

    // get readable size
    size_t getReadSize() const { return m_size - m_position; }

    // get data size
    size_t getSize() const { return m_size; }

    // get endian
    bool isLittleEndian() const { return m_endian == CCZOE_LITTLE_ENDIAN; }

    // set endian
    void setLittleEndian(bool val) { m_endian = val ? CCZOE_LITTLE_ENDIAN : CCZOE_BIG_ENDIAN; }

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

    // swap endian
    template <class Int>
    Int byteSwap(Int value)
    {
        return value;
    }

    // zigzag encoding
    static uint32_t EncodeZigzag32(int32_t v);
    static uint64_t EncodeZigzag64(int64_t v);

    template <class Int>
    Int DecodeZigzag(Int v)
    {
        return (v >> 1) ^ (-(v & 1));
    }
};

}}

#endif