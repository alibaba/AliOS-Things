#include <stdlib.h>
#include "psptypes.h"
#include "mem_util.h"

#define  USB_MEM4_ALIGN(n)          ((n) + (-(n) & 3))

void* USB_mem_alloc_word_aligned(uint_32 size)
{
    void *p = malloc(size + 4);
    return((void*)(USB_MEM4_ALIGN((uint_32)p)));
}

void* USB_mem_alloc_zero(uint_32 size)
{
    void* p;
    p = USB_mem_alloc_word_aligned(size);
    if(p != NULL)
    {
        memset( (p) , (0) , (size));
    }
    return p;
}
