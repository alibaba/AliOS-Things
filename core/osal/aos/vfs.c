/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <string.h>

#include "aos/vfs.h"
#include "aos/errno.h"

#include "vfs_types.h"
#include "vfs_api.h"

static int _vfs_to_aos_res(int res)
{
    switch (res) {
        case VFS_OK:
            return 0;
        case VFS_ERR_NOMEM:
            return -ENOMEM;
        case VFS_ERR_INVAL:
            return -EINVAL;
        case VFS_ERR_NOENT:
            return -ENOENT;
        case VFS_ERR_NAMETOOLONG:
            return -ENAMETOOLONG;
        case VFS_ERR_NOSYS:
            return -ENOSYS;
        case VFS_ERR_ENFILE:
            return -ENFILE;
        case VFS_ERR_NODEV:
            return -ENODEV;
        case VFS_ERR_LOCK:
            return -EIO;
        case VFS_ERR_BUSY:
            return -EBUSY;
        default:
            return res;
    }
}

int aos_vfs_init(void)
{
    return _vfs_to_aos_res(vfs_init());
}

int aos_open(const char *path, int flags)
{
    return _vfs_to_aos_res(vfs_open(path, flags));
}

int aos_close(int fd)
{
    return _vfs_to_aos_res(vfs_close(fd));
}

ssize_t aos_read(int fd, void *buf, size_t nbytes)
{
    return _vfs_to_aos_res(vfs_read(fd, buf, nbytes));
}

ssize_t aos_write(int fd, const void *buf, size_t nbytes)
{
    return _vfs_to_aos_res(vfs_write(fd, buf, nbytes));
}

int aos_ioctl(int fd, int cmd, unsigned long arg)
{
    return _vfs_to_aos_res(vfs_ioctl(fd, cmd, arg));
}

int aos_do_pollfd(int fd, int flag, poll_notify_t notify, void *fds, void *arg)
{
    return _vfs_to_aos_res(vfs_do_pollfd(fd, flag, (vfs_poll_notify_t)notify, fds, arg));
}

off_t aos_lseek(int fd, off_t offset, int whence)
{
    return _vfs_to_aos_res(vfs_lseek(fd, offset, whence));
}

int aos_sync(int fd)
{
    return _vfs_to_aos_res(vfs_sync(fd));
}

int aos_stat(const char *path, struct aos_stat *st)
{
    return _vfs_to_aos_res(vfs_stat(path, (vfs_stat_t *)st));
}

int aos_fstat(int fh, struct aos_stat *st)
{
    return _vfs_to_aos_res(vfs_fstat(fh, (vfs_stat_t *)st));
}

int aos_link(const char *path1, const char *path2)
{
    return _vfs_to_aos_res(vfs_link(path1, path2));
}

int aos_unlink(const char *path)
{
    return _vfs_to_aos_res(vfs_unlink(path));
}

int aos_remove(const char *path)
{
    return _vfs_to_aos_res(vfs_remove(path));
}

int aos_rename(const char *oldpath, const char *newpath)
{
    return _vfs_to_aos_res(vfs_rename(oldpath, newpath));
}

aos_dir_t *aos_opendir(const char *path)
{
    return (aos_dir_t *)vfs_opendir(path);
}

int aos_closedir(aos_dir_t *dir)
{
    return _vfs_to_aos_res(vfs_closedir((vfs_dir_t *)dir));
}

aos_dirent_t *aos_readdir(aos_dir_t *dir)
{
    return (aos_dirent_t *)vfs_readdir((vfs_dir_t *)dir);
}

int aos_mkdir(const char *path)
{
    return _vfs_to_aos_res(vfs_mkdir(path));
}

int aos_rmdir(const char *path)
{
    return _vfs_to_aos_res(vfs_rmdir(path));
}

void aos_rewinddir(aos_dir_t *dir)
{
    vfs_rewinddir((vfs_dir_t *)dir);
}

long aos_telldir(aos_dir_t *dir)
{
    return _vfs_to_aos_res(vfs_telldir((vfs_dir_t *)dir));
}

void aos_seekdir(aos_dir_t *dir, long loc)
{
    vfs_seekdir((vfs_dir_t *)dir, loc);
}

int aos_statfs(const char *path, struct aos_statfs *buf)
{
    return _vfs_to_aos_res(vfs_statfs(path, (vfs_statfs_t *)buf));
}

int aos_access(const char *path, int amode)
{
    return _vfs_to_aos_res(vfs_access(path, amode));
}

int aos_chdir(const char *path)
{
    return _vfs_to_aos_res(vfs_chdir(path));
}

char *aos_getcwd(char *buf, size_t size)
{
    return vfs_getcwd(buf, size);
}

int aos_vfs_fd_offset_get(void)
{
    return vfs_fd_offset_get();
}

long aos_pathconf(const char *path, int name)
{
    return vfs_pathconf(path, name);
}

long aos_fpathconf(int fh, int name)
{
    return vfs_fpathconf(fh, name);
}

int aos_utime(const char *path, const struct aos_utimbuf *times)
{
    return _vfs_to_aos_res(vfs_utime(path, (vfs_utimbuf_t *)times));
}

int aos_register_driver(const char *path, file_ops_t *ops, void *arg)
{
    return _vfs_to_aos_res(vfs_register_driver(path, (vfs_file_ops_t *)ops, arg));
}

int aos_unregister_driver(const char *path)
{
    return _vfs_to_aos_res(vfs_unregister_driver(path));
}

int aos_register_fs(const char *path, fs_ops_t* ops, void *arg)
{
    return _vfs_to_aos_res(vfs_register_fs(path, (vfs_filesystem_ops_t *)ops, arg));
}

int aos_unregister_fs(const char *path)
{
    return _vfs_to_aos_res(vfs_unregister_fs(path));
}
