/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <tlsf.h>

static tlsf_t tlsf_ptr = NULL;

void* app_mm_init(void *mem, size_t size)
{
    tlsf_ptr = tlsf_create_with_pool(mem, size);

    return tlsf_ptr;
}

void* app_mm_malloc(size_t size)
{
    if (tlsf_ptr == NULL) {
        return NULL;
    }

    return tlsf_malloc(tlsf_ptr, size);
}

void* app_mm_realloc(void *ptr, size_t size)
{
    if (tlsf_ptr == NULL) {
        return NULL;
    }

    return tlsf_realloc(tlsf_ptr, ptr, size);
}

void app_mm_free(void *ptr)
{
    if (tlsf_ptr == NULL) {
        return;
    }

    tlsf_free(tlsf_ptr, ptr);
}
