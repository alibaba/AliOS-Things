/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef KV_USYSCALL_H
#define KV_USYSCALL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "sys/types.h"

int32_t kv_set(const char *key, const void *value, int len, int sync);

int32_t kv_get(const char *key, void *buffer, int *buffer_len);

int32_t kv_sec_set(const char *key, const void *value, int len, int sync);

int32_t kv_sec_get(const char *key, void *buffer, int *buffer_len);

int32_t kv_del_stub(const char *key);

#ifdef __cplusplus
}
#endif

#endif /* KV_USYSCALL_H */
