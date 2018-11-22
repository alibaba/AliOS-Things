/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef UMM_H
#define UMM_H

#ifdef __cplusplus
extern "C" {
#endif

void *umm_init(void *mem, size_t size);

void *umm_malloc(size_t size);

void *umm_realloc(void *ptr, size_t size);

void umm_free(void *ptr);

#ifdef __cplusplus
}
#endif

#endif /* UMM_H */

