/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <string.h>

#include "k_api.h"
#include "aos/vfs.h"
#include "aos/errno.h"

#include "fs/vfs_types.h"
#include "fs/vfs_api.h"
#include "vfs_syscall_arg.h"
#include "fsfd.h"

int sys_vfs_open_stub(vfs_open_syscall_arg_t *arg)
{
    ktask_t *cur;
    int      ret;

    ret = vfs_open(arg->path, arg->flags);
    if (ret < 0) {
        return ret;
    }
    cur = krhino_cur_task_get();
    fsfd_ref(cur->pid, ret);

    return ret;
}

int sys_vfs_close_stub(vfs_close_syscall_arg_t *arg)
{
    ktask_t *cur;
    int      ret;

    ret = vfs_close(arg->fd);
    if (ret < 0) {
        return ret;
    }

    cur = krhino_cur_task_get();
    fsfd_deref(cur->pid, arg->fd);

    return ret;
}

ssize_t sys_vfs_read_stub(vfs_read_syscall_arg_t *arg)
{
    return vfs_read(arg->fd, arg->buf, arg->nbytes);
}

ssize_t sys_vfs_write_stub(vfs_write_syscall_arg_t *arg)
{
    return vfs_write(arg->fd, arg->buf, arg->nbytes);
}

int sys_vfs_ioctl_stub(vfs_ioctl_syscall_arg_t *arg)
{
    return vfs_ioctl(arg->fd, arg->cmd, arg->arg);
}

off_t sys_vfs_lseek_stub(vfs_lseek_syscall_arg_t *arg)
{
    return vfs_lseek(arg->fd, arg->offset, arg->whence);
}

int sys_vfs_fsync_stub(vfs_fsync_syscall_arg_t *arg)
{
    return vfs_fsync(arg->fd);
}

void sys_vfs_sync_stub(void)
{
    vfs_sync();
}

int sys_vfs_stat_stub(vfs_stat_syscall_arg_t *arg)
{
    return vfs_stat(arg->path, arg->st);
}

int sys_vfs_fstat_stub(vfs_fstat_syscall_arg_t *arg)
{
    return vfs_fstat(arg->fd, arg->st);
}

void *sys_vfs_mmap_stub(vfs_mmap_syscall_arg_t *arg)
{
    return vfs_mmap(arg->start, arg->len, arg->prot,
                    arg->flags, arg->fd, arg->offset);
}

extern int k_shmm_unmap_area(void *start, size_t len);
int sys_vfs_munmap_stub(vfs_munmap_syscall_arg_t *arg)
{
    return k_shmm_unmap_area(arg->start, arg->len);
}

int sys_vfs_link_stub(vfs_link_syscall_arg_t *arg)
{
    return vfs_link(arg->path1, arg->path2);
}

int sys_vfs_unlink_stub(vfs_unlink_syscall_arg_t *arg)
{
    return vfs_unlink(arg->path);
}

int sys_vfs_remove_stub(vfs_remove_syscall_arg_t *arg)
{
    return vfs_remove(arg->path);
}

int sys_vfs_rename_stub(vfs_rename_syscall_arg_t *arg)
{
    return vfs_rename(arg->oldpath, arg->newpath);
}

vfs_dir_t *sys_vfs_opendir_stub(vfs_opendir_syscall_arg_t *arg)
{
    return vfs_opendir(arg->path);
}

int sys_vfs_dirfd_stub(vfs_dirfd_syscall_arg_t *arg)
{
    return vfs_dirfd(arg->dir);
}

int sys_vfs_closedir_stub(vfs_closedir_syscall_arg_t *arg)
{
    return vfs_closedir(arg->dir);
}

vfs_dirent_t *sys_vfs_readdir_stub(vfs_readdir_syscall_arg_t *arg)
{
    vfs_dirent_t *direntp = vfs_readdir(arg->dir);

    if ((direntp != NULL) && (arg->dir_entry != NULL)) {
        /* Simply copy to user. */
        arg->dir_entry->d_ino = direntp->d_ino;
        arg->dir_entry->d_type = direntp->d_type;
        strncpy(arg->dir_entry->d_name, direntp->d_name, NAME_MAX);
        return arg->dir_entry;

    } else {
        return direntp;
    }
}

int sys_vfs_mkdir_stub(vfs_mkdir_syscall_arg_t *arg)
{
    return vfs_mkdir(arg->path);
}

int sys_vfs_rmdir_stub(vfs_rmdir_syscall_arg_t *arg)
{
    return vfs_rmdir(arg->path);
}

void sys_vfs_rewinddir_stub(vfs_rewinddir_syscall_arg_t *arg)
{
    vfs_rewinddir(arg->dir);
}

long sys_vfs_telldir_stub(vfs_telldir_syscall_arg_t *arg)
{
    return vfs_telldir(arg->dir);
}

void sys_vfs_seekdir_stub(vfs_seekdir_syscall_arg_t *arg)
{
    vfs_seekdir(arg->dir, arg->loc);
}

int sys_vfs_statfs_stub(vfs_statfs_syscall_arg_t *arg)
{
    return vfs_statfs(arg->path, arg->buf);
}

int sys_vfs_access_stub(vfs_access_syscall_arg_t *arg)
{
    return vfs_access(arg->path, arg->amode);
}

int sys_vfs_chdir_stub(vfs_chdir_syscall_arg_t *arg)
{
    return vfs_chdir(arg->path);
}

char *sys_vfs_getcwd_stub(vfs_getcwd_syscall_arg_t *arg)
{
    return vfs_getcwd(arg->buf, arg->size);
}

long sys_vfs_pathconf_stub(vfs_pathconf_syscall_arg_t *arg)
{
    return vfs_pathconf(arg->path, arg->name);
}

long sys_vfs_fpathconf_stub(vfs_fpathconf_syscall_arg_t *arg)
{
    return vfs_fpathconf(arg->fd, arg->name);
}

int sys_vfs_utime_stub(vfs_utime_syscall_arg_t *arg)
{
    return vfs_utime(arg->path, arg->times);
}

int sys_vfs_fcntl_stub(vfs_fcntl_syscall_arg_t *arg)
{
    return vfs_fcntl(arg->fd, arg->cmd, arg->val);
}
