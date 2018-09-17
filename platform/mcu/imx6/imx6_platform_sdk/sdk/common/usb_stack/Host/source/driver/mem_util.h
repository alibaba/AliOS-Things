#ifndef __MEM_UTIL_H__
#define __MEM_UTIL_H__

#include "psptypes.h"

#define USB_MEM4_ALIGN(n)  ((n) + (-(n) & 3))

void* USB_mem_alloc_zero(uint_32 size);
void* USB_mem_alloc_word_aligned(uint_32 size);

#endif /* __MEM_UTIL_H__ */