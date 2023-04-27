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
auto byteswap(T val)
{
    if constexpr(std::is_same<T, uint64_t>::value)
    {
        return (T)bswap_64((uint64_t)val);
    }
    else if constexpr(std::is_same<T, uint32_t>::value)
    {
        return (T)bswap_32((uint32_t)val);
    }
    else if constexpr(std::is_same<T, uint16_t>::value)
    {
        return (T)bswap_16((uint16_t)val);
    }
    return val;
}

}

#endif