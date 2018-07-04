/*
 *  Copyright (C) 2015 YunOS Project. All rights reserved.
 */

#include "lite-utils.h"

void* pal_memory_malloc(size_t size)
{
    return LITE_malloc((int)size);
}

void pal_memory_free(void* ptr)
{
    LITE_free(ptr);
    return;
}
