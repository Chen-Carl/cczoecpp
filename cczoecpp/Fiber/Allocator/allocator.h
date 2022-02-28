#ifndef __CCZOE_ALLOCATOR_H__
#define __CCZOE_ALLOCATOR_H__

#include <malloc.h>

namespace cczoe {

class MallocStackAllocator
{
private:

public:
    static void *Alloc(size_t size)
    {
        return malloc(size);
    }

    static void Free(void *vp)
    {
        return free(vp);
    }
};

// static allocator
// set MallocStackAllocator as a default allocator
using StackAllocator = MallocStackAllocator;

}

#endif