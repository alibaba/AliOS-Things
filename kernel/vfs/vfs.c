/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include <vfs_conf.h>
#include <vfs_err.h>
#include <vfs_inode.h>
#include <vfs.h>
#include <stdio.h>
#include <hal/hal.h>
#include <limits.h>
#include <string.h>
#include <vfs_file.h>

#ifdef IO_NEED_TRAP
#include <vfs_trap.h>
#endif

#ifndef PATH_MAX
#define PATH_MAX 256
#endif

static uint8_t g_vfs_init;
aos_mutex_t g_vfs_mutex;

int vfs_init(void)
{
    int ret = VFS_SUCCESS;

    if (g_vfs_init == 1) {
        return ret;
    }

    if ((ret = aos_mutex_new(&g_vfs_mutex)) != VFS_SUCCESS) {
        return ret;
    }

    inode_init();

    g_vfs_init = 1;

    return ret;
}

int aos_open(const char *path, int flags)
{
    file_t  *file;
    inode_t *node;
    size_t len = 0;
    int ret = VFS_SUCCESS;

    if (path == NULL) {
        return -EINVAL;
    }

    len = strlen(path);
    if (len > PATH_MAX) {
        return -ENAMETOOLONG;
    }

    if ((ret = aos_mutex_lock(&g_vfs_mutex, AOS_WAIT_FOREVER)) != 0) {
        return ret;
    }

    node = inode_open(path);

    if (node == NULL) {
        aos_mutex_unlock(&g_vfs_mutex);

        #ifdef IO_NEED_TRAP
            return trap_open(path, flags);
        #else
            return -ENOENT;
        #endif
    }

    node->i_flags = flags;
    file = new_file(node);

    aos_mutex_unlock(&g_vfs_mutex);

    if (file == NULL) {
        return -ENFILE;
    }

    if (INODE_IS_FS(node)) {
        if ((node->ops.i_fops->open) != NULL) {
            ret = (node->ops.i_fops->open)(file, path, flags);
        }

    } else {
        if ((node->ops.i_ops->open) != NULL) {
            ret = (node->ops.i_ops->open)(node, file);
        }
    }

    if (ret != VFS_SUCCESS) {
        del_file(file);
        return ret;
    }

    return get_fd(file);
}
AOS_EXPORT(int, aos_open, const char *, int);

int aos_close(int fd)
{
    int ret = VFS_SUCCESS;
    file_t  *f;
    inode_t *node;

    f = get_file(fd);

    if (f == NULL) {
        #ifdef IO_NEED_TRAP
            return trap_close(fd);
        #else
            return -ENOENT;
        #endif
    }

    node = f->node;

    if (INODE_IS_FS(node)) {
        if ((node->ops.i_fops->close) != NULL) {
            ret = (node->ops.i_fops->close)(f);
        }

    } else {

        if ((node->ops.i_ops->close) != NULL) {
            ret = (node->ops.i_ops->close)(f);
        }
    }

    if ((ret = aos_mutex_lock(&g_vfs_mutex, AOS_WAIT_FOREVER)) != 0) {
        return ret;
    }

    del_file(f);

    aos_mutex_unlock(&g_vfs_mutex);

    return ret;
}
AOS_EXPORT(int, aos_close, int);

ssize_t aos_read(int fd, void *buf, size_t nbytes)
{
    ssize_t  nread = -1;
    file_t  *f;
    inode_t *node;

    f = get_file(fd);

    if (f == NULL) {
        #ifdef IO_NEED_TRAP
            return trap_read(fd, buf, nbytes);
        #else
            return -ENOENT;
        #endif
    }

    node = f->node;

    if (INODE_IS_FS(node)) {
        if ((node->ops.i_fops->read) != NULL) {
            nread = (node->ops.i_fops->read)(f, buf, nbytes);
        }
    } else {
        if ((node->ops.i_ops->read) != NULL) {
            nread = (node->ops.i_ops->read)(f, buf, nbytes);
        }
    }

    return nread;
}
AOS_EXPORT(ssize_t, aos_read, int, void *, size_t);

ssize_t aos_write(int fd, const void *buf, size_t nbytes)
{
    ssize_t  nwrite = -1;
    file_t  *f;
    inode_t *node;

    f = get_file(fd);

    if (f == NULL) {
        #ifdef IO_NEED_TRAP
            return trap_write(fd, buf, nbytes);
        #else
            return -ENOENT;
        #endif
    }

    node = f->node;

    if (INODE_IS_FS(node)) {
        if ((node->ops.i_fops->write) != NULL) {
            nwrite = (node->ops.i_fops->write)(f, buf, nbytes);
        }
    } else {
        if ((node->ops.i_ops->write) != NULL) {
            nwrite = (node->ops.i_ops->write)(f, buf, nbytes);
        }
    }

    return nwrite;
}
AOS_EXPORT(ssize_t, aos_write, int, const void *, size_t);

int aos_ioctl(int fd, int cmd, unsigned long arg)
{
    int ret = -ENOSYS;
    file_t  *f;
    inode_t *node;

    if (fd < 0) {
        return -EINVAL;
    }

    f = get_file(fd);

    if (f == NULL) {
        return -ENOENT;
    }

    node = f->node;

    if (INODE_IS_FS(node)) {
        if ((node->ops.i_fops->ioctl) != NULL) {
            ret = (node->ops.i_fops->ioctl)(f, cmd, arg);
        }
    } else {
        if ((node->ops.i_ops->ioctl) != NULL) {
            ret = (node->ops.i_ops->ioctl)(f, cmd, arg);
        }
    }

    return ret;
}
AOS_EXPORT(int, aos_ioctl, int, int, unsigned long);

off_t aos_lseek(int fd, off_t offset, int whence)
{
    file_t *f;
    inode_t *node;
    int ret = -ENOSYS;

    f = get_file(fd);

    if (f == NULL) {
        return -ENOENT;
    }

    node = f->node;

    if (INODE_IS_FS(node)) {
        if ((node->ops.i_fops->lseek) != NULL) {
            ret = (node->ops.i_fops->lseek)(f, offset, whence);
        }
    }

    return ret;
}
AOS_EXPORT(off_t, aos_lseek, int, off_t, int);

int aos_sync(int fd)
{
    file_t  *f;
    inode_t *node;
    int ret = -ENOSYS;

    f = get_file(fd);

    if (f == NULL) {
        return -ENOENT;
    }

    node = f->node;

    if (INODE_IS_FS(node)) {
        if ((node->ops.i_fops->sync) != NULL) {
            ret = (node->ops.i_fops->sync)(f);
        }
    }

    return ret;
}
AOS_EXPORT(int, aos_sync, int);

int aos_stat(const char *path, struct stat *st)
{
    file_t  *file;
    inode_t *node;
    int err, ret = -ENOSYS;

    if (path == NULL) {
        return -EINVAL;
    }

    if ((err = aos_mutex_lock(&g_vfs_mutex, AOS_WAIT_FOREVER)) != 0) {
        return err;
    }

    node = inode_open(path);

    if (node == NULL) {
        aos_mutex_unlock(&g_vfs_mutex);
        return -ENODEV;
    }

    file = new_file(node);

    aos_mutex_unlock(&g_vfs_mutex);

    if (file == NULL) {
        return -ENOENT;
    }

    if (INODE_IS_FS(node)) {
        if ((node->ops.i_fops->stat) != NULL) {
            ret = (node->ops.i_fops->stat)(file, path, st);
        }
    }

    if ((err = aos_mutex_lock(&g_vfs_mutex, AOS_WAIT_FOREVER)) != 0) {
        return err;
    }

    del_file(file);

    aos_mutex_unlock(&g_vfs_mutex);
    return ret;
}
AOS_EXPORT(int, aos_stat, const char *, struct stat *);

int aos_unlink(const char *path)
{
    file_t  *f;
    inode_t *node;
    int err, ret = -ENOSYS;

    if (path == NULL) {
        return -EINVAL;
    }

    if ((err = aos_mutex_lock(&g_vfs_mutex, AOS_WAIT_FOREVER)) != 0) {
        return err;
    }

    node = inode_open(path);

    if (node == NULL) {
        aos_mutex_unlock(&g_vfs_mutex);
        return -ENODEV;
    }

    f = new_file(node);

    aos_mutex_unlock(&g_vfs_mutex);

    if (f == NULL) {
        return -ENOENT;
    }

    if (INODE_IS_FS(node)) {
        if ((node->ops.i_fops->unlink) != NULL) {
            ret = (node->ops.i_fops->unlink)(f, path);
        }
    }

    if ((err = aos_mutex_lock(&g_vfs_mutex, AOS_WAIT_FOREVER)) != 0) {
        return err;
    }

    del_file(f);

    aos_mutex_unlock(&g_vfs_mutex);
    return ret;
}
AOS_EXPORT(int, aos_unlink, const char *);

int aos_rename(const char *oldpath, const char *newpath)
{
    file_t  *f;
    inode_t *node;
    int err, ret = -ENOSYS;

    if (oldpath == NULL || newpath == NULL) {
        return -EINVAL;
    }

    if ((err = aos_mutex_lock(&g_vfs_mutex, AOS_WAIT_FOREVER)) != 0) {
        return err;
    }

    node = inode_open(oldpath);

    if (node == NULL) {
        aos_mutex_unlock(&g_vfs_mutex);
        return -ENODEV;
    }

    f = new_file(node);

    aos_mutex_unlock(&g_vfs_mutex);

    if (f == NULL) {
        return -ENOENT;
    }

    if (INODE_IS_FS(node)) {
        if ((node->ops.i_fops->rename) != NULL) {
            ret = (node->ops.i_fops->rename)(f, oldpath, newpath);
        }
    }

    if ((err = aos_mutex_lock(&g_vfs_mutex, AOS_WAIT_FOREVER)) != 0) {
        return err;
    }

    del_file(f);

    aos_mutex_unlock(&g_vfs_mutex);
    return ret;
}
AOS_EXPORT(int, aos_rename, const char *, const char *);

aos_dir_t *aos_opendir(const char *path)
{
    file_t  *file;
    inode_t *node;
    aos_dir_t *dp = NULL;

    if (path == NULL) {
        return NULL;
    }

    if (aos_mutex_lock(&g_vfs_mutex, AOS_WAIT_FOREVER) != 0) {
        return NULL;
    }

    node = inode_open(path);

    if (node == NULL) {
        aos_mutex_unlock(&g_vfs_mutex);
        return NULL;
    }

    file = new_file(node);

    aos_mutex_unlock(&g_vfs_mutex);

    if (file == NULL) {
        return NULL;
    }

    if (INODE_IS_FS(node)) {
        if ((node->ops.i_fops->opendir) != NULL) {
            dp = (node->ops.i_fops->opendir)(file, path);
        }
    }

    if (dp == NULL) {
        if (aos_mutex_lock(&g_vfs_mutex, AOS_WAIT_FOREVER) != 0) {
            return NULL;
        }

        del_file(file);

        aos_mutex_unlock(&g_vfs_mutex);
        return NULL;
    }

    dp->dd_vfs_fd = get_fd(file);
    return dp;
}
AOS_EXPORT(aos_dir_t *, aos_opendir, const char *);

int aos_closedir(aos_dir_t *dir)
{
    file_t  *f;
    inode_t *node;
    int err, ret = -ENOSYS;

    if (dir == NULL) {
        return -EINVAL;
    }

    f = get_file(dir->dd_vfs_fd);

    if (f == NULL) {
        return -ENOENT;
    }

    node = f->node;

    if (INODE_IS_FS(node)) {
        if ((node->ops.i_fops->closedir) != NULL) {
            ret = (node->ops.i_fops->closedir)(f, dir);
        }
    }

    if ((err = aos_mutex_lock(&g_vfs_mutex, AOS_WAIT_FOREVER)) != 0) {
        return err;
    }

    del_file(f);

    aos_mutex_unlock(&g_vfs_mutex);

    return ret;
}
AOS_EXPORT(int, aos_closedir, aos_dir_t *);

aos_dirent_t *aos_readdir(aos_dir_t *dir)
{
    file_t *f;
    inode_t *node;
    aos_dirent_t *ret = NULL;

    if (dir == NULL) {
        return NULL;
    }

    f = get_file(dir->dd_vfs_fd);
    if (f == NULL) {
        return NULL;
    }

    node = f->node;

    if (INODE_IS_FS(node)) {
        if ((node->ops.i_fops->readdir) != NULL) {
            ret = (node->ops.i_fops->readdir)(f, dir);
        }
    }

    if (ret != NULL) {
        return ret;
    }

    return NULL;
}
AOS_EXPORT(aos_dirent_t *, aos_readdir, aos_dir_t *);

int aos_mkdir(const char *path)
{
    file_t  *file;
    inode_t *node;
    int err, ret = -ENOSYS;

    if (path == NULL) {
        return -EINVAL;
    }

    if ((err = aos_mutex_lock(&g_vfs_mutex, AOS_WAIT_FOREVER)) != 0) {
        return err;
    }

    node = inode_open(path);

    if (node == NULL) {
        aos_mutex_unlock(&g_vfs_mutex);
        return -ENODEV;
    }

    file = new_file(node);

    aos_mutex_unlock(&g_vfs_mutex);

    if (file == NULL) {
        return -ENOENT;
    }

    if (INODE_IS_FS(node)) {
        if ((node->ops.i_fops->mkdir) != NULL) {
            ret = (node->ops.i_fops->mkdir)(file, path);
        }
    }

    if ((err = aos_mutex_lock(&g_vfs_mutex, AOS_WAIT_FOREVER)) != 0) {
        return err;
    }

    del_file(file);

    aos_mutex_unlock(&g_vfs_mutex);
    return ret;
}
AOS_EXPORT(int, aos_mkdir, const char *);

extern uart_dev_t uart_0;

int32_t aos_uart_send(void *data, uint32_t size, uint32_t timeout)
{
    return hal_uart_send(&uart_0, data, size, timeout);
}
AOS_EXPORT(int32_t, aos_uart_send, void *, uint32_t, uint32_t);

int32_t aos_uart_recv(void *data, uint32_t expect_size, uint32_t *recv_size, uint32_t timeout)
{
    return hal_uart_recv_II(&uart_0, data,  expect_size, recv_size,  timeout);
}
AOS_EXPORT(int32_t, aos_uart_recv, void *, uint32_t , uint32_t *, uint32_t );
