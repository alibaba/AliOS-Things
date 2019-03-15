/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef AOS_VFS_TRAP_H
#define AOS_VFS_TRAP_H

int trap_open(const char *path, int flags);
int trap_read(int fd, void *buf, int len);
int trap_write(int fd, const void *buf, int len);
int trap_close(int fd);
int trap_fcntl(int fd, int cmd, int val);

#endif

