/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef VFS_USYSCALL_H
#define VFS_USYSCALL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "sys/types.h"
#include "dirent.h"
#include "errno.h"
#include "limits.h"

int u_vfs_open(const char *path, int flags);

int u_vfs_close(int fd);

int32_t u_vfs_read(int32_t fd, void *buf, uint32_t nbytes);

int32_t u_vfs_write(int32_t fd, const void *buf, uint32_t nbytes);

int u_vfs_ioctl(int fd, int cmd, unsigned long arg);

uint32_t u_vfs_lseek(int32_t fd, int64_t offset, int32_t whence);

int u_vfs_fsync(int fd);

void u_vfs_sync(void);

int u_vfs_stat(const char *path, vfs_statfs_t *st);

int u_vfs_fstat(int fd, vfs_statfs_t *st);

void *u_vfs_mmap(void *start, size_t len, int prot, int flags, int fd, off_t offset);

int u_vfs_munmap(void *start, size_t len);

int u_vfs_link(const char *path1, const char *path2);

int u_vfs_unlink(const char *path);

int u_vfs_remove(const char *path);

int u_vfs_rename(const char *oldpath, const char *newpath);

vfs_dir_t *u_vfs_opendir(const char *path);

int u_vfs_dirfd(vfs_dir_t *dir);

int u_vfs_closedir(vfs_dir_t *dir);

vfs_dirent_t *u_vfs_readdir(vfs_dir_t *dir);

int u_vfs_mkdir(const char *path);

int u_vfs_rmdir(const char *path);

void u_vfs_rewinddir(vfs_dir_t *dir);

long u_vfs_telldir(vfs_dir_t *dir);

void u_vfs_seekdir(vfs_dir_t *dir, long loc);

int u_vfs_statfs(const char *path, vfs_statfs_t *buf);

int u_vfs_access(const char *path, int amode);

int u_vfs_chdir(const char *path);

char *u_vfs_getcwd(char *buf, size_t size);

long u_vfs_pathconf(const char *path, int name);

long u_vfs_fpathconf(int fd, int name);

int u_vfs_utime(const char *path, const vfs_utimbuf_t *times);

int u_vfs_fcntl(int fd, int cmd, int val);

#ifdef __cplusplus
}
#endif

#endif /* VFS_USYSCALL_H */