/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef UMM_H
#define UMM_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

void *umm_init(void *mem, size_t size);

void *umm_alloc(size_t size);

void *umm_realloc(void *ptr, size_t size);

void umm_free(void *ptr);

size_t umm_total_size(void);

size_t umm_free_size(void);

size_t umm_used_size(void);

size_t umm_max_free_block_size(void);

size_t umm_free_size_min(void);

void umm_show(void);

#ifdef __cplusplus
}
#endif

#endif /* UMM_H */

