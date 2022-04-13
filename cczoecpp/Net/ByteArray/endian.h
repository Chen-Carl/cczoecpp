#ifndef __CCZOE_ENDIAN_H__
#define __CCZOE_ENDIAN_H__

#include <byteswap.h>

#define CCZOE_LITTLE_ENDIAN 1
#define CCZOE_BIG_ENDIAN 2

#if BYTE_ORDER == BIG_ENDIAN
#define CCZOE_BYTE_ORDER CCZOE_BIG_ENDIAN
#else
#define CCZOE_BYTE_ORDER CCZOE_LITTLE_ENDIAN
#endif

namespace cczoe {

template <class T>
typename std::enable_if<sizeof(T) == sizeof(uint64_t), T>::type
byteSwap(T value)
{
    return (T)bswap_64((uint64_t)value);
}

template <class T>
typename std::enable_if<sizeof(T) == sizeof(uint32_t), T>::type
byteSwap(T value) 
{
    return (T)bswap_32((uint32_t)value);
}

template <class T>
typename std::enable_if<sizeof(T) == sizeof(uint16_t), T>::type
byteSwap(T value) 
{
    return (T)bswap_16((uint16_t)value);
}

}

#endif