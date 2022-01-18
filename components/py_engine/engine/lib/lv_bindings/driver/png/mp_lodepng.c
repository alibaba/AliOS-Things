#include "py/obj.h"

/**
 * Implementation of memory allocation functions for lodepng, which delegate gc allocation
 * This allows lodepng use the ram reserved for Micropython gc
 */

#ifndef NO_QSTR

#ifndef LODEPNG_NO_COMPILE_ALLOCATORS
   #error LodePNG must be compiled with LODEPNG_NO_COMPILE_ALLOCATORS, which should be passed by the build system, but was not.
#endif

#endif

void* lodepng_malloc(size_t size)
{
    return m_malloc(size);
}

void* lodepng_realloc(void* ptr, size_t new_size)
{
    return m_realloc(ptr, new_size);
}

void lodepng_free(void* ptr)
{
    m_free(ptr);
}


