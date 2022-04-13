
/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef KIPC_UTILS_H
#define KIPC_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

uint32_t aos_ipc_strlen(const char *str);
int aos_ipc_strcmp(const char *src, const char *dst);
void aos_ipc_strcpy(char *dst, const char *src);
void *aos_ipc_memcpy(void *dest, const void *src, size_t count);

#ifdef __cplusplus
}
#endif

#endif  /* end of KIPC_UTILS_H */

