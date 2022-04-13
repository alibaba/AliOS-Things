/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef VFS_KSYSCALL_H
#define VFS_KSYSCALL_H

#include "vfs_syscall_arg.h"

int sys_vfs_open_stub(vfs_open_syscall_arg_t *arg);

int sys_vfs_close_stub(vfs_close_syscall_arg_t *arg);

ssize_t sys_vfs_read_stub(vfs_read_syscall_arg_t *arg);

ssize_t sys_vfs_write_stub(vfs_write_syscall_arg_t *arg);

int sys_vfs_ioctl_stub(vfs_ioctl_syscall_arg_t *arg);

off_t sys_vfs_lseek_stub(vfs_lseek_syscall_arg_t *arg);

int sys_vfs_fsync_stub(vfs_fsync_syscall_arg_t *arg);

void sys_vfs_sync_stub(void);

int sys_vfs_stat_stub(vfs_stat_syscall_arg_t *arg);

int sys_vfs_fstat_stub(vfs_fstat_syscall_arg_t *arg);

void *sys_vfs_mmap_stub(vfs_mmap_syscall_arg_t *arg);

int sys_vfs_munmap_stub(vfs_munmap_syscall_arg_t *arg);

int sys_vfs_link_stub(vfs_link_syscall_arg_t *arg);

int sys_vfs_unlink_stub(vfs_unlink_syscall_arg_t *arg);

int sys_vfs_remove_stub(vfs_remove_syscall_arg_t *arg);

int sys_vfs_rename_stub(vfs_rename_syscall_arg_t *arg);

vfs_dir_t *sys_vfs_opendir_stub(vfs_opendir_syscall_arg_t *arg);

int sys_vfs_dirfd_stub(vfs_dirfd_syscall_arg_t *arg);

int sys_vfs_closedir_stub(vfs_closedir_syscall_arg_t *arg);

vfs_dirent_t *sys_vfs_readdir_stub(vfs_readdir_syscall_arg_t *arg);

int sys_vfs_mkdir_stub(vfs_mkdir_syscall_arg_t *arg);

int sys_vfs_rmdir_stub(vfs_rmdir_syscall_arg_t *arg);

void sys_vfs_rewinddir_stub(vfs_rewinddir_syscall_arg_t *arg);

long sys_vfs_telldir_stub(vfs_telldir_syscall_arg_t *arg);

void sys_vfs_seekdir_stub(vfs_seekdir_syscall_arg_t *arg);

int sys_vfs_statfs_stub(vfs_statfs_syscall_arg_t *arg);

int sys_vfs_access_stub(vfs_statfs_syscall_arg_t *arg);

int sys_vfs_chdir_stub(vfs_chdir_syscall_arg_t *arg);

char *sys_vfs_getcwd_stub(vfs_getcwd_syscall_arg_t *arg);

long sys_vfs_pathconf_stub(vfs_pathconf_syscall_arg_t *arg);

long sys_vfs_fpathconf_stub(vfs_fpathconf_syscall_arg_t *arg);

int sys_vfs_utime_stub(vfs_utime_syscall_arg_t *arg);

int sys_vfs_fcntl_stub(vfs_fcntl_syscall_arg_t *arg);
#endif /* VFS_KSYSCALL_H */

