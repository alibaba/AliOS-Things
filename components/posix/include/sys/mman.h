/*
 * Copyright (C) 2021 Alibaba Group Holding Limited
 */

#ifndef _SYS_MMAN_H
#define _SYS_MMAN_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <sys/types.h>

void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
int munmap(void *addr, size_t length);

#ifdef __cplusplus
}
#endif

#endif /* _SYS_MMAN_H */
