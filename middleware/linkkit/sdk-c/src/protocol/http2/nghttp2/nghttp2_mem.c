/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */


#include "nghttp2_mem.h"
#include "iotx_utils.h"

static void *default_malloc(size_t size, void *mem_user_data)
{
    (void)mem_user_data;

    return LITE_malloc(size, MEM_MAGIC, "nghttp2");
}

static void default_free(void *ptr, void *mem_user_data)
{
    (void)mem_user_data;
    if (ptr != NULL) {
        LITE_free(ptr);
    }
}

static void *default_calloc(size_t nmemb, size_t size, void *mem_user_data)
{
    (void)mem_user_data;

    return LITE_calloc(nmemb, size, MEM_MAGIC, "nghttp2");
}

static void *default_realloc(void *ptr, size_t size, void *mem_user_data)
{
    (void)mem_user_data;

    return LITE_realloc(ptr, size, MEM_MAGIC, "nghttp2");
}

static nghttp2_mem mem_default = {NULL, default_malloc, default_free,
                                  default_calloc, default_realloc
                                 };

nghttp2_mem *nghttp2_mem_default(void)
{
    return &mem_default;
}

void *nghttp2_mem_malloc(nghttp2_mem *mem, size_t size)
{
    return mem->malloc(size, mem->mem_user_data);
}

void nghttp2_mem_free(nghttp2_mem *mem, void *ptr)
{
    mem->free(ptr, mem->mem_user_data);
}

void nghttp2_mem_free2(nghttp2_free free_func, void *ptr, void *mem_user_data)
{
    free_func(ptr, mem_user_data);
}

void *nghttp2_mem_calloc(nghttp2_mem *mem, size_t nmemb, size_t size)
{
    return mem->calloc(nmemb, size, mem->mem_user_data);
}

void *nghttp2_mem_realloc(nghttp2_mem *mem, void *ptr, size_t size)
{
    return mem->realloc(ptr, size, mem->mem_user_data);
}
