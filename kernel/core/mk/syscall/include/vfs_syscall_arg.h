/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef VFS_SYSCALL_H
#define VFS_SYSCALL_H

#include "aos/vfs.h"
#include "fs/vfs_types.h"

typedef struct {
    const char *path;
    int flags;
} vfs_open_syscall_arg_t;

typedef struct {
    int fd;
} vfs_close_syscall_arg_t;

typedef struct {
    int fd;
    void *buf;
    size_t nbytes;
} vfs_read_syscall_arg_t;

typedef struct {
    int fd;
    const void *buf;
    size_t nbytes;
} vfs_write_syscall_arg_t;

typedef struct {
    int fd;
    int cmd;
    unsigned long arg;
} vfs_ioctl_syscall_arg_t;

typedef struct {
    int fd;
    int64_t offset;
    int whence;
} vfs_lseek_syscall_arg_t;

typedef struct {
    int fd;
} vfs_fsync_syscall_arg_t;

typedef struct {
    const char *path;
    vfs_statfs_t *st;
} vfs_stat_syscall_arg_t;

typedef struct {
    int fd;
    vfs_statfs_t *st;
} vfs_fstat_syscall_arg_t;

typedef struct {
    void *start;
    size_t len;
    int prot;
    int flags;
    int fd;
    off_t offset;
} vfs_mmap_syscall_arg_t;

typedef struct {
    void *start;
    size_t len;
} vfs_munmap_syscall_arg_t;

typedef struct {
    const char *path1;
    const char *path2;
} vfs_link_syscall_arg_t;

typedef struct {
    const char *path;
} vfs_unlink_syscall_arg_t;

typedef struct {
    const char *path;
} vfs_remove_syscall_arg_t;

typedef struct {
    const char *oldpath;
    const char *newpath;
} vfs_rename_syscall_arg_t;

typedef struct {
    const char *path;
} vfs_opendir_syscall_arg_t;

typedef struct {
    vfs_dir_t *dir;
} vfs_dirfd_syscall_arg_t;

typedef struct {
    vfs_dir_t *dir;
} vfs_closedir_syscall_arg_t;

typedef struct {
    vfs_dir_t *dir;
    vfs_dirent_t *dir_entry;
} vfs_readdir_syscall_arg_t;

typedef struct {
    const char *path;
} vfs_mkdir_syscall_arg_t;

typedef struct {
    const char *path;
} vfs_rmdir_syscall_arg_t;

typedef struct {
    vfs_dir_t *dir;
} vfs_rewinddir_syscall_arg_t;

typedef struct {
    vfs_dir_t *dir;
} vfs_telldir_syscall_arg_t;

typedef struct {
    vfs_dir_t *dir;
    long loc;
} vfs_seekdir_syscall_arg_t;

typedef struct {
    const char *path;
    vfs_statfs_t *buf;
} vfs_statfs_syscall_arg_t;

typedef struct {
    const char *path;
    int amode;
} vfs_access_syscall_arg_t;

typedef struct {
    const char *path;
} vfs_chdir_syscall_arg_t;

typedef struct {
    char *buf;
    size_t size;
} vfs_getcwd_syscall_arg_t;

typedef struct {
    const char *path;
    int name;
} vfs_pathconf_syscall_arg_t;

typedef struct {
    int fd;
    int name;
} vfs_fpathconf_syscall_arg_t;

typedef struct {
    const char *path;
    const vfs_utimbuf_t *times;
} vfs_utime_syscall_arg_t;

typedef struct {
    int fd;
    int cmd;
    void *val;
} vfs_fcntl_syscall_arg_t;

#endif /* VFS_SYSCALL_H */

