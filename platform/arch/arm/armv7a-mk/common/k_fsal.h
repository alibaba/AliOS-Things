/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef K_FSAL_H
#define K_FSAL_H

#include <stdint.h>

#define AFS_SEEK_SET    0
#define AFS_SEEK_CUR    1
#define AFS_SEEK_END    2

#define AFS_O_RDONLY    (00)
#define AFS_O_WRONLY    (01)
#define AFS_O_RDWR      (02)
#define AFS_O_CREATE    (0100)
#define AFS_O_TRUNC     (01000)

int afs_open(const char *path, int flags);

int afs_close(int fd);

int32_t afs_lseek(int fd, int32_t offset, int whence);

int32_t afs_read(int fd, void *buf, uint32_t count);

int32_t afs_write(int fd, const void *buf, uint32_t count);

#endif /* K_FSAL_H */

