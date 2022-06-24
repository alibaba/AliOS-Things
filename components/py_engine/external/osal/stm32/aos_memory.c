#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "aos_kernel.h"
#include "aos_errno.h"
#include "py/gc.h"
#include "gccollect.h"

void *aos_malloc(size_t size)
{
    return (void *)gc_alloc(size, false);
}

void *aos_calloc(size_t nitems, size_t size)
{
    return (void *)gc_alloc(nitems * size, false);
}

void *aos_zalloc(size_t size)
{
    return (void *)aos_calloc(1, size);
}

void *aos_realloc(void *mem, size_t size)
{
    return (void *)gc_realloc(mem, size,true);
}

void aos_free(void *mem)
{
    gc_free(mem);
}