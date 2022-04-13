/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <umm.h>

void *operator new[](size_t size)
{
    return umm_alloc(size);
}

void *operator new(size_t size)
{
    return umm_alloc(size);
}

void operator delete[](void *ptr)
{
    umm_free(ptr);
}

void operator delete(void *ptr)
{
    umm_free(ptr);
}

