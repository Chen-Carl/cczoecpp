#ifndef __CCZOE_ENDIAN_H__
#define __CCZOE_ENDIAN_H__

#include <byteswap.h>

#define CCZOE_LITTLE_ENDIAN 1
#define CCZOE_BIG_ENDIAN 2

#if BYTE_ORDER== BIG_ENDIAN
#define CCZOE_BYTE_ORDER CCZOE_BIG_ENDIAN
#else
#define CCZOE_BYTE_ORDER CCZOE_LITTLE_ENDIAN
#endif

namespace cczoe {

}

#endif