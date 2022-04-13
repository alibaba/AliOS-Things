/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "sys/types.h"
#include "syscall.h"
#include "vfs_syscall_arg.h"
#include "syscall_no.h"
#include "dirent.h"
#include "errno.h"
#include "limits.h"
#include "aos/kernel.h"
#include "fs/vfs_types.h"

int u_vfs_open(const char *path, int flags)
{
    vfs_open_syscall_arg_t arg;

    arg.path = path;
    arg.flags = flags;

    return (int)SYSCALL(SYS_VFS_OPEN, &arg);
}


int u_vfs_close(int fd)
{
    vfs_close_syscall_arg_t arg;

    arg.fd = fd;

    return (int)SYSCALL(SYS_VFS_CLOSE, &arg);
}

int32_t u_vfs_read(int32_t fd, void *buf, uint32_t nbytes)
{
    vfs_read_syscall_arg_t arg;

    arg.fd = fd;
    arg.buf = buf;
    arg.nbytes = nbytes;

    return SYSCALL(SYS_VFS_READ, &arg);
}

int32_t u_vfs_write(int32_t fd, const void *buf, uint32_t nbytes)
{
    vfs_write_syscall_arg_t arg;

    arg.fd = fd;
    arg.buf = buf;
    arg.nbytes = nbytes;

    return SYSCALL(SYS_VFS_WRITE, &arg);
}

int u_vfs_ioctl(int fd, int cmd, unsigned long arg)
{
    vfs_ioctl_syscall_arg_t _arg;

    _arg.fd = fd;
    _arg.cmd = cmd;
    _arg.arg = arg;

    return (int)SYSCALL(SYS_VFS_IOCTL, &_arg);
}

uint32_t u_vfs_lseek(int32_t fd, int64_t offset, int32_t whence)
{
    vfs_lseek_syscall_arg_t arg;

    arg.fd = fd;
    arg.offset = offset;
    arg.whence = whence;

    return SYSCALL(SYS_VFS_LSEEK, &arg);
}

int u_vfs_fsync(int fd)
{
    vfs_fsync_syscall_arg_t arg;

    arg.fd = fd;

    return (int)SYSCALL(SYS_VFS_FSYNC, &arg);
}

void u_vfs_sync(void)
{
    SYSCALL(SYS_VFS_SYNC, NULL);
}

int u_vfs_stat(const char *path, vfs_statfs_t *st)
{
    vfs_stat_syscall_arg_t arg;

    arg.path = path;
    arg.st = st;

    return (int)SYSCALL(SYS_VFS_STAT, &arg);
}

int u_vfs_fstat(int fd, vfs_statfs_t *st)
{
    vfs_fstat_syscall_arg_t arg;

    arg.fd = fd;
    arg.st = st;

    return (int)SYSCALL(SYS_VFS_FSTAT, &arg);
}

void *u_vfs_mmap(void *start, size_t len, int prot, int flags, int fd, off_t offset)
{
    vfs_mmap_syscall_arg_t arg;

    arg.start  = start;
    arg.len    = len;
    arg.prot   = prot;
    arg.flags  = flags;
    arg.fd     = fd;
    arg.offset = offset;

    return (void *)SYSCALL(SYS_VFS_MMAP, &arg);
}

int u_vfs_munmap(void *start, size_t len)
{
    vfs_munmap_syscall_arg_t arg;

    arg.start = start;
    arg.len = len;

    return (int)SYSCALL(SYS_VFS_MUNMAP, &arg);
}

int u_vfs_link(const char *path1, const char *path2)
{
    vfs_link_syscall_arg_t arg;

    arg.path1 = path1;
    arg.path2 = path2;

    return (int)SYSCALL(SYS_VFS_LINK, &arg);
}

int u_vfs_unlink(const char *path)
{
    vfs_unlink_syscall_arg_t arg;

    arg.path = path;

    return (int)SYSCALL(SYS_VFS_UNLINK, &arg);
}

int u_vfs_remove(const char *path)
{
    vfs_remove_syscall_arg_t arg;

    arg.path = path;

    return (int)SYSCALL(SYS_VFS_REMOVE, &arg);
}

int u_vfs_rename(const char *oldpath, const char *newpath)
{
    vfs_rename_syscall_arg_t arg;

    arg.oldpath = oldpath;
    arg.newpath = newpath;

    return (int)SYSCALL(SYS_VFS_RENAME, &arg);
}

vfs_dir_t *u_vfs_opendir(const char *path)
{
    vfs_opendir_syscall_arg_t arg;

    arg.path = path;

    return (vfs_dir_t *)SYSCALL(SYS_VFS_OPENDIR, &arg);

}

int u_vfs_dirfd(vfs_dir_t *dir)
{
    vfs_dirfd_syscall_arg_t arg;

    arg.dir = dir;

    return (int)SYSCALL(SYS_VFS_DIRFD, &arg);
}


int u_vfs_closedir(vfs_dir_t *dir)
{
    vfs_closedir_syscall_arg_t arg;

    arg.dir = dir;

    return (int)SYSCALL(SYS_VFS_CLOSEDIR, &arg);
}

vfs_dirent_t *u_vfs_readdir(vfs_dir_t *dir)
{
    vfs_readdir_syscall_arg_t arg;
    static struct dirent *dir_entry = NULL; /* Not reentrant. readdir_r is reentrant.*/

    if (dir_entry == NULL) {
        dir_entry = aos_malloc(sizeof(struct dirent) + NAME_MAX + 1);
        if (dir_entry == NULL) {
            errno = ENOMEM;
            return NULL;
        }
    }

    arg.dir = dir;
    arg.dir_entry = (vfs_dirent_t *)dir_entry;

    return (vfs_dirent_t *)SYSCALL(SYS_VFS_READDIR, &arg);
}

int u_vfs_mkdir(const char *path)
{
    vfs_mkdir_syscall_arg_t arg;

    arg.path = path;

    return (int)SYSCALL(SYS_VFS_MKDIR, &arg);
}

int u_vfs_rmdir(const char *path)
{
    vfs_rmdir_syscall_arg_t arg;

    arg.path = path;

    return (int)SYSCALL(SYS_VFS_RMDIR, &arg);
}

void u_vfs_rewinddir(vfs_dir_t *dir)
{
    vfs_rewinddir_syscall_arg_t arg;

    arg.dir = dir;

    SYSCALL(SYS_VFS_REWINDDIR, &arg);
}

long u_vfs_telldir(vfs_dir_t *dir)
{
    vfs_telldir_syscall_arg_t arg;

    arg.dir = dir;

    return (long)SYSCALL(SYS_VFS_TELLDIR, &arg);
}

void u_vfs_seekdir(vfs_dir_t *dir, long loc)
{
    vfs_seekdir_syscall_arg_t arg;

    arg.dir = dir;
    arg.loc = loc;

    SYSCALL(SYS_VFS_SEEKDIR, &arg);
}

int u_vfs_statfs(const char *path, vfs_statfs_t *buf)
{
    vfs_statfs_syscall_arg_t arg;

    arg.path = path;
    arg.buf = buf;

    return (int)SYSCALL(SYS_VFS_STATFS, &arg);
}

int u_vfs_access(const char *path, int amode)
{
    vfs_access_syscall_arg_t arg;

    arg.path = path;
    arg.amode = amode;

    return (int)SYSCALL(SYS_VFS_ACCESS, &arg);
}

int u_vfs_chdir(const char *path)
{
    vfs_chdir_syscall_arg_t arg;

    arg.path = path;

    return (int)SYSCALL(SYS_VFS_CHDIR, &arg);
}

char *u_vfs_getcwd(char *buf, size_t size)
{
    vfs_getcwd_syscall_arg_t arg;

    arg.buf = buf;
    arg.size = size;

    return (char *)SYSCALL(SYS_VFS_GETCWD, &arg);
}

long u_vfs_pathconf(const char *path, int name)
{
    vfs_pathconf_syscall_arg_t arg;

    arg.path = path;
    arg.name = name;

    return (long)SYSCALL(SYS_VFS_PATHCONF, &arg);
}

long u_vfs_fpathconf(int fd, int name)
{
    vfs_fpathconf_syscall_arg_t arg;

    arg.fd = fd;
    arg.name = name;

    return (long)SYSCALL(SYS_VFS_FPATHCONF, &arg);
}

int u_vfs_utime(const char *path, const vfs_utimbuf_t *times)
{
    vfs_utime_syscall_arg_t arg;

    arg.path = path;
    arg.times = times;

    return (int)SYSCALL(SYS_VFS_UTIME, &arg);
}

int u_vfs_fcntl(int fd, int cmd, int val)
{
    vfs_fcntl_syscall_arg_t arg;

    arg.fd = fd;
    arg.cmd = cmd;
    arg.val = (void *)val;

    return (int)SYSCALL(SYS_VFS_FCNTL, &arg);
}