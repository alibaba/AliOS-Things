/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef MM_H
#define MM_H

#ifdef __cplusplus
extern "C" {
#endif

#include <tlsf.h>

tlsf_t tlsf_create_with_pool(void* mem, size_t bytes);
void* tlsf_malloc(tlsf_t tlsf, size_t bytes);
void* tlsf_realloc(tlsf_t tlsf, void* ptr, size_t size);
void tlsf_free(tlsf_t tlsf, void* ptr);

#ifdef __cplusplus
}
#endif

#endif /* MM_H */
