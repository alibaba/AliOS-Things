/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include "fs/vfs_types.h"
#include "fs/vfs_api.h"
#include "fs/vfs_conf.h"

#include "vfs_inode.h"
#include "vfs_file.h"
#include "vfs_adapt.h"

#ifdef IO_NEED_TRAP
    #include "vfs_trap.h"
#endif

volatile static uint8_t  g_vfs_init     = 0;
static void    *g_vfs_lock_ptr = NULL;

#if (CURRENT_WORKING_DIRECTORY_ENABLE > 0)
    char g_current_working_directory[VFS_PATH_MAX];
#endif

int32_t vfs_init(void)
{
#ifdef VFS_CONFIG_ROOTFS
    char *directory = "/";
#else
    char *directory = "/default";
#endif
    if (g_vfs_init == 1) {
        return VFS_OK;
    }

    g_vfs_lock_ptr = vfs_lock_create();
    if (g_vfs_lock_ptr == NULL) {
        return VFS_ERR_NOMEM;
    }

    vfs_inode_init();

#if (CURRENT_WORKING_DIRECTORY_ENABLE > 0)
    /* init current working directory */
    memset(g_current_working_directory, 0, sizeof(g_current_working_directory));
    strncpy(g_current_working_directory, directory, strlen(directory) + 1);
#endif

    g_vfs_init = 1;

    return VFS_OK;
}

int32_t vfs_open(const char *path, int32_t flags)
{
    int32_t len = 0;
    int32_t ret = VFS_OK;

    vfs_file_t  *f;
    vfs_inode_t *node;

    if (path == NULL) {
        return VFS_ERR_INVAL;
    }

    len = strlen(path);
    if (len > VFS_PATH_MAX) {
        return VFS_ERR_NAMETOOLONG;
    }

    if (vfs_lock(g_vfs_lock_ptr) != VFS_OK) {
        return VFS_ERR_LOCK;
    }

    node = vfs_inode_open(path);
    if (node == NULL) {
        vfs_unlock(g_vfs_lock_ptr);

#ifdef IO_NEED_TRAP
        return trap_open(path, flags);
#else
        return VFS_ERR_NOENT;
#endif
    }

    node->i_flags = flags;
    f = vfs_file_new(node);

    if (f == NULL) {
        vfs_unlock(g_vfs_lock_ptr);
        return VFS_ERR_ENFILE;
    }

#ifdef CONFIG_VFS_LSOPEN
    strncpy(f->filename, path, sizeof(f->filename) - 1);
#endif

    if (INODE_IS_FS(node)) {
        if ((node->ops.i_fops->open) != NULL) {
            ret = (node->ops.i_fops->open)(f, path, flags);
        }
    } else {
        if ((node->ops.i_ops->open) != NULL) {
            ret = (node->ops.i_ops->open)(node, f);
        }
    }

    if (ret != VFS_OK) {
        vfs_file_del(f);
    } else {
        ret = vfs_fd_get(f);
        vfs_fd_mark_open(ret);
    }

    vfs_unlock(g_vfs_lock_ptr);

    return ret;
}

int32_t vfs_close(int32_t fd)
{
    int32_t ret = VFS_OK;

    vfs_file_t *f;
    vfs_inode_t *node;

    if (vfs_lock(g_vfs_lock_ptr) != VFS_OK) {
        return VFS_ERR_LOCK;
    }

    f = vfs_file_get(fd);

    if (f == NULL) {
        vfs_unlock(g_vfs_lock_ptr);
#ifdef IO_NEED_TRAP
        return trap_close(fd);
#else
        return VFS_ERR_NOENT;
#endif
    }

    node = f->node;

    if (INODE_IS_FS(node)) {
        if ((node->ops.i_fops->close) != NULL) {
            ret = (node->ops.i_fops->close)(f);
        }
    } else if (INODE_IS_NETWORK(node)) {
        if ((node->ops.i_nops->close) != NULL) {
            ret = (node->ops.i_nops->close)(f->param);
        }
    } else {
        if ((node->ops.i_ops->close) != NULL) {
            ret = (node->ops.i_ops->close)(f);
        }
    }

    vfs_fd_mark_close(fd);
    vfs_file_del(f);

    vfs_unlock(g_vfs_lock_ptr);

    return ret;
}

int32_t vfs_read(int32_t fd, void *buf, uint32_t nbytes)
{
    int32_t nread = -1;

    vfs_file_t  *f;
    vfs_inode_t *node;

    f = vfs_file_get(fd);

    if (f == NULL) {
#ifdef IO_NEED_TRAP
        return trap_read(fd, buf, nbytes);
#else
        return VFS_ERR_NOENT;
#endif
    }

    node = f->node;

    if (INODE_IS_FS(node)) {
        if ((node->ops.i_fops->read) != NULL) {
            nread = (node->ops.i_fops->read)(f, buf, nbytes);
        }
    } else if (INODE_IS_NETWORK(node)) {
        if ((node->ops.i_nops->read) != NULL) {
            nread = (node->ops.i_nops->read)(f->param, buf, nbytes);
        }
    } else {
        if ((node->ops.i_ops->read) != NULL) {
            nread = (node->ops.i_ops->read)(f, buf, nbytes);
        }
    }

    return nread;
}

int32_t vfs_write(int32_t fd, const void *buf, uint32_t nbytes)
{
    int32_t nwrite = -1;

    vfs_file_t  *f;
    vfs_inode_t *node;

    f = vfs_file_get(fd);

    if (f == NULL) {
#ifdef IO_NEED_TRAP
        return trap_write(fd, buf, nbytes);
#else
        return VFS_ERR_NOENT;
#endif
    }

    node = f->node;

    if (INODE_IS_FS(node)) {
        if ((node->ops.i_fops->write) != NULL) {
            nwrite = (node->ops.i_fops->write)(f, buf, nbytes);
        }
    } else if (INODE_IS_NETWORK(node)) {
        if ((node->ops.i_nops->write) != NULL) {
            nwrite = (node->ops.i_nops->write)(f->param, buf, nbytes);
        }
    } else {
        if ((node->ops.i_ops->write) != NULL) {
            nwrite = (node->ops.i_ops->write)(f, buf, nbytes);
        }
    }

    return nwrite;
}

int32_t vfs_ioctl(int32_t fd, int32_t cmd, uint32_t arg)
{
    int32_t ret = VFS_ERR_NOSYS;

    vfs_file_t  *f;
    vfs_inode_t *node;

    if (fd < 0) {
        return VFS_ERR_INVAL;
    }

    f = vfs_file_get(fd);
    if (f == NULL) {
        return VFS_ERR_NOENT;
    }

    node = f->node;

    if (INODE_IS_FS(node)) {
        if ((node->ops.i_fops->ioctl) != NULL) {
            ret = (node->ops.i_fops->ioctl)(f, cmd, arg);
        }
    } else if (INODE_IS_NETWORK(node)) {
        if ((node->ops.i_nops->ioctl) != NULL) {
            ret = (node->ops.i_nops->ioctl)(f->param, cmd, arg);
        }
    } else {
        if ((node->ops.i_ops->ioctl) != NULL) {
            ret = (node->ops.i_ops->ioctl)(f, cmd, arg);
        }
    }

    return ret;
}

int32_t vfs_do_pollfd(int32_t fd, int32_t flag, vfs_poll_notify_t notify,
                      void *fds, void *arg)
{
    int32_t ret = VFS_ERR_NOSYS;

    vfs_file_t  *f;
    vfs_inode_t *node;

    f = vfs_file_get(fd);
    if (f == NULL) {
        return VFS_ERR_NOENT;
    }

    node = f->node;
    if (!INODE_IS_FS(node)) {
        if ((node->ops.i_ops->poll) != NULL) {
            ret = (node->ops.i_ops->poll)(f, flag, notify, fds, arg);
        }
    }

    return ret;
}

uint32_t vfs_lseek(int32_t fd, int64_t offset, int32_t whence)
{
    int32_t ret = VFS_ERR_NOSYS;

    vfs_file_t  *f;
    vfs_inode_t *node;

    f = vfs_file_get(fd);
    if (f == NULL) {
        return VFS_ERR_NOENT;
    }

    node = f->node;

    if (INODE_IS_FS(node)) {
        if ((node->ops.i_fops->lseek) != NULL) {
            ret = (node->ops.i_fops->lseek)(f, offset, whence);
        }
    } else {
        if ((node->ops.i_ops->lseek) != NULL) {
            ret = (node->ops.i_ops->lseek)(f, offset, whence);
        }
    }

    return ret;
}

int32_t vfs_fsync(int32_t fd)
{
    int32_t ret = VFS_ERR_NOSYS;

    vfs_file_t  *f;
    vfs_inode_t *node;

    f = vfs_file_get(fd);
    if (f == NULL) {
        return VFS_ERR_NOENT;
    }

    node = f->node;

    if (INODE_IS_FS(node)) {
        if ((node->ops.i_fops->sync) != NULL) {
            ret = (node->ops.i_fops->sync)(f);
        }
    }

    return ret;
}

void vfs_sync(void)
{
    int i;
    int32_t fd;

    /**
     * prevent other threads from closing
     * the file while syncing it
     */
    if (vfs_lock(g_vfs_lock_ptr) != VFS_OK) {
        return;
    }

    for (i = 0; i < VFS_MAX_FILE_NUM; i++) {
        fd = VFS_FD_OFFSET + i;
        if (vfs_fd_is_open(fd)) {
            printf("sync fd %d\r\n", fd);
            vfs_fsync(fd);
        }
    }

    vfs_unlock(g_vfs_lock_ptr);
}

int32_t vfs_stat(const char *path, vfs_stat_t *st)
{
    int32_t ret = VFS_ERR_NOSYS;

    vfs_file_t  *f;
    vfs_inode_t *node;

    if (path == NULL) {
        return VFS_ERR_INVAL;
    }

    if (vfs_lock(g_vfs_lock_ptr) != VFS_OK) {
        return VFS_ERR_LOCK;
    }

    node = vfs_inode_open(path);

    if (node == NULL) {
        vfs_unlock(g_vfs_lock_ptr);
        return VFS_ERR_NODEV;
    }

    f = vfs_file_new(node);

    vfs_unlock(g_vfs_lock_ptr);

    if (f == NULL) {
        return VFS_ERR_NOENT;
    }

#ifdef CONFIG_VFS_LSOPEN
    strncpy(f->filename, path, sizeof(f->filename) - 1);
#endif

    if (INODE_IS_FS(node)) {
        if ((node->ops.i_fops->stat) != NULL) {
            ret = (node->ops.i_fops->stat)(f, path, st);
        }
    } else if (INODE_IS_CHAR(node) || INODE_IS_BLOCK(node)) {
        if ((node->ops.i_ops->stat) != NULL) {
            ret = (node->ops.i_ops->stat)(f, path, st);
        }
    }

    if (vfs_lock(g_vfs_lock_ptr) != VFS_OK) {
        return VFS_ERR_LOCK;
    }

    vfs_file_del(f);
    vfs_unlock(g_vfs_lock_ptr);

    return ret;
}

int32_t vfs_fstat(int fd, vfs_stat_t *st)
{
    int32_t ret = VFS_ERR_NOSYS;

    vfs_file_t  *f;
    vfs_inode_t *node;

    f = vfs_file_get(fd);
    if (f == NULL) {
        return VFS_ERR_NOENT;
    }

    node = f->node;

    if (INODE_IS_FS(node)) {
        if ((node->ops.i_fops->fstat) != NULL) {
            ret = (node->ops.i_fops->fstat)(f, st);
        }
    }

    return ret;
}

void *vfs_mmap(void *start, size_t len, int prot, int flags, int fd, off_t offset)
{
    void *ret = (void *)VFS_ERR_NOSYS;

    vfs_file_t  *f;
    vfs_inode_t *node;

    f = vfs_file_get(fd);
    if (f == NULL) {
        return NULL;
    }

    if (vfs_lock(g_vfs_lock_ptr) != VFS_OK) {
        return NULL;
    }

    node = f->node;

    if (INODE_IS_CHAR(node) || INODE_IS_BLOCK(node)) {
        if ((node->ops.i_ops->mmap) != NULL) {
            ret = (node->ops.i_ops->mmap)(f, len);
        }
    }

    vfs_unlock(g_vfs_lock_ptr);

    return ret;
}

int32_t vfs_link(const char *path1, const char *path2)
{
    int32_t ret = VFS_ERR_NOSYS;

    vfs_file_t  *f;
    vfs_inode_t *node;

    if ((path1 == NULL) || (path2 == NULL)) {
        return VFS_ERR_INVAL;
    }

    if (vfs_lock(g_vfs_lock_ptr) != VFS_OK) {
        return VFS_ERR_LOCK;
    }

    node = vfs_inode_open(path1);

    if (node == NULL) {
        vfs_unlock(g_vfs_lock_ptr);
        return VFS_ERR_NODEV;
    }

    f = vfs_file_new(node);

    vfs_unlock(g_vfs_lock_ptr);

    if (f == NULL) {
        return VFS_ERR_NOENT;
    }

#ifdef CONFIG_VFS_LSOPEN
    strncpy(f->filename, path1, sizeof(f->filename) - 1);
#endif

    if (INODE_IS_FS(node)) {
        if ((node->ops.i_fops->link) != NULL) {
            ret = (node->ops.i_fops->link)(f, path1, path2);
        }
    }

    if (vfs_lock(g_vfs_lock_ptr) != VFS_OK) {
        return VFS_ERR_LOCK;
    }

    vfs_file_del(f);
    vfs_unlock(g_vfs_lock_ptr);

    return ret;
}

int32_t vfs_unlink(const char *path)
{
    int32_t ret = VFS_ERR_NOSYS;

    vfs_file_t  *f;
    vfs_inode_t *node;

    if (path == NULL) {
        return VFS_ERR_INVAL;
    }

    if (vfs_lock(g_vfs_lock_ptr) != VFS_OK) {
        return VFS_ERR_LOCK;
    }

    node = vfs_inode_open(path);

    if (node == NULL) {
        vfs_unlock(g_vfs_lock_ptr);
        return VFS_ERR_NODEV;
    }

    f = vfs_file_new(node);

    vfs_unlock(g_vfs_lock_ptr);

    if (f == NULL) {
        return VFS_ERR_NOENT;
    }

#ifdef CONFIG_VFS_LSOPEN
    strncpy(f->filename, path, sizeof(f->filename) - 1);
#endif

    if (INODE_IS_FS(node)) {
        if ((node->ops.i_fops->unlink) != NULL) {
            ret = (node->ops.i_fops->unlink)(f, path);
        }
    }

    if (vfs_lock(g_vfs_lock_ptr) != VFS_OK) {
        return VFS_ERR_LOCK;
    }

    vfs_file_del(f);
    vfs_unlock(g_vfs_lock_ptr);

    return ret;
}

int32_t vfs_remove(const char *path)
{
    int32_t ret = VFS_ERR_NOSYS;

    vfs_file_t  *f;
    vfs_inode_t *node;

    if (path == NULL) {
        return VFS_ERR_INVAL;
    }

    if (vfs_lock(g_vfs_lock_ptr) != VFS_OK) {
        return VFS_ERR_LOCK;
    }

    node = vfs_inode_open(path);

    if (node == NULL) {
        vfs_unlock(g_vfs_lock_ptr);
        return VFS_ERR_NODEV;
    }

    f = vfs_file_new(node);

    vfs_unlock(g_vfs_lock_ptr);

    if (f == NULL) {
        return VFS_ERR_NOENT;
    }

#ifdef CONFIG_VFS_LSOPEN
    strncpy(f->filename, path, sizeof(f->filename) - 1);
#endif

    if (INODE_IS_FS(node)) {
        if ((node->ops.i_fops->remove) != NULL) {
            ret = (node->ops.i_fops->remove)(f, path);
        }
    }

    if (vfs_lock(g_vfs_lock_ptr) != VFS_OK) {
        return VFS_ERR_LOCK;
    }

    vfs_file_del(f);
    vfs_unlock(g_vfs_lock_ptr);

    return ret;
}

int32_t vfs_rename(const char *oldpath, const char *newpath)
{
    int32_t ret = VFS_ERR_NOSYS;

    vfs_file_t  *f;
    vfs_inode_t *node;

    if ((oldpath == NULL) || (newpath == NULL)) {
        return VFS_ERR_INVAL;
    }

    if (vfs_lock(g_vfs_lock_ptr) != VFS_OK) {
        return VFS_ERR_LOCK;
    }

    node = vfs_inode_open(oldpath);

    if (node == NULL) {
        vfs_unlock(g_vfs_lock_ptr);
        return VFS_ERR_NODEV;
    }

    f = vfs_file_new(node);

    vfs_unlock(g_vfs_lock_ptr);

    if (f == NULL) {
        return VFS_ERR_NOENT;
    }

#ifdef CONFIG_VFS_LSOPEN
    strncpy(f->filename, oldpath, sizeof(f->filename) - 1);
#endif

    if (INODE_IS_FS(node)) {
        if ((node->ops.i_fops->rename) != NULL) {
            ret = (node->ops.i_fops->rename)(f, oldpath, newpath);
        }
    }

    if (vfs_lock(g_vfs_lock_ptr) != VFS_OK) {
        return VFS_ERR_LOCK;
    }

    vfs_file_del(f);
    vfs_unlock(g_vfs_lock_ptr);

    return ret;
}

vfs_dir_t *vfs_opendir(const char *path)
{
    vfs_dir_t *dp = NULL;

    vfs_file_t  *f;
    vfs_inode_t *node;

    if (path == NULL) {
        return NULL;
    }

    if (vfs_lock(g_vfs_lock_ptr) != VFS_OK) {
        return NULL;
    }

    node = vfs_inode_open(path);

    if (node == NULL) {
        vfs_unlock(g_vfs_lock_ptr);
        return NULL;
    }

    f = vfs_file_new(node);

    vfs_unlock(g_vfs_lock_ptr);

    if (f == NULL) {
        return NULL;
    }

#ifdef CONFIG_VFS_LSOPEN
    strncpy(f->filename, path, sizeof(f->filename) - 1);
#endif

    if (INODE_IS_FS(node)) {
        if ((node->ops.i_fops->opendir) != NULL) {
            dp = (node->ops.i_fops->opendir)(f, path);
        }
    }

    if (dp == NULL) {
        if (vfs_lock(g_vfs_lock_ptr) != VFS_OK) {
            return NULL;
        }

        vfs_file_del(f);

        vfs_unlock(g_vfs_lock_ptr);
        return NULL;
    }

    dp->dd_vfs_fd = vfs_fd_get(f);

    return dp;
}

int32_t vfs_dirfd(vfs_dir_t *dir)
{
    vfs_file_t  *f;

    if (dir == NULL) {
        return VFS_ERR_INVAL;
    }

    f = vfs_file_get(dir->dd_vfs_fd);
    if (f == NULL) {
        return VFS_ERR_NOENT;
    }

    return dir->dd_vfs_fd;
}

int32_t vfs_closedir(vfs_dir_t *dir)
{
    int32_t ret = VFS_ERR_NOSYS;

    vfs_file_t  *f;
    vfs_inode_t *node;

    if (dir == NULL) {
        return VFS_ERR_INVAL;
    }

    f = vfs_file_get(dir->dd_vfs_fd);
    if (f == NULL) {
        return VFS_ERR_NOENT;
    }

    node = f->node;

    if (INODE_IS_FS(node)) {
        if ((node->ops.i_fops->closedir) != NULL) {
            ret = (node->ops.i_fops->closedir)(f, dir);
        }
    }

    if (vfs_lock(g_vfs_lock_ptr) != VFS_OK) {
        return VFS_ERR_LOCK;
    }

    vfs_file_del(f);

    vfs_unlock(g_vfs_lock_ptr);

    return ret;
}

vfs_dirent_t *vfs_readdir(vfs_dir_t *dir)
{
    vfs_dirent_t *dirent = NULL;

    vfs_file_t  *f;
    vfs_inode_t *node;

    if (dir == NULL) {
        return NULL;
    }

    f = vfs_file_get(dir->dd_vfs_fd);
    if (f == NULL) {
        return NULL;
    }

    node = f->node;

    if (INODE_IS_FS(node)) {
        if ((node->ops.i_fops->readdir) != NULL) {
            dirent = (node->ops.i_fops->readdir)(f, dir);
        }
    }

    return dirent;
}

int32_t vfs_mkdir(const char *path)
{
    int32_t ret = VFS_ERR_NOSYS;

    vfs_file_t  *f;
    vfs_inode_t *node;

    if (path == NULL) {
        return VFS_ERR_INVAL;
    }

    if (vfs_lock(g_vfs_lock_ptr) != VFS_OK) {
        return VFS_ERR_LOCK;
    }

    node = vfs_inode_open(path);

    if (node == NULL) {
        vfs_unlock(g_vfs_lock_ptr);
        return VFS_ERR_NODEV;
    }

    f = vfs_file_new(node);

    vfs_unlock(g_vfs_lock_ptr);

    if (f == NULL) {
        return VFS_ERR_NOENT;
    }

#ifdef CONFIG_VFS_LSOPEN
    strncpy(f->filename, path, sizeof(f->filename) - 1);
#endif

    if (INODE_IS_FS(node)) {
        if ((node->ops.i_fops->mkdir) != NULL) {
            ret = (node->ops.i_fops->mkdir)(f, path);
        }
    }

    if (vfs_lock(g_vfs_lock_ptr) != VFS_OK) {
        return VFS_ERR_LOCK;
    }

    vfs_file_del(f);
    vfs_unlock(g_vfs_lock_ptr);

    return ret;
}

int32_t vfs_rmdir(const char *path)
{
    int32_t ret = VFS_ERR_NOSYS;

    vfs_file_t  *f;
    vfs_inode_t *node;

    if (path == NULL) {
        return VFS_ERR_INVAL;
    }

    if (vfs_lock(g_vfs_lock_ptr) != VFS_OK) {
        return VFS_ERR_LOCK;
    }

    node = vfs_inode_open(path);

    if (node == NULL) {
        vfs_unlock(g_vfs_lock_ptr);
        return VFS_ERR_NODEV;
    }

    f = vfs_file_new(node);

    vfs_unlock(g_vfs_lock_ptr);

    if (f == NULL) {
        return VFS_ERR_NOENT;
    }

#ifdef CONFIG_VFS_LSOPEN
    strncpy(f->filename, path, sizeof(f->filename) - 1);
#endif

    if (INODE_IS_FS(node)) {
        if ((node->ops.i_fops->rmdir) != NULL) {
            ret = (node->ops.i_fops->rmdir)(f, path);
        }
    }

    if (vfs_lock(g_vfs_lock_ptr) != VFS_OK) {
        return VFS_ERR_LOCK;
    }

    vfs_file_del(f);
    vfs_unlock(g_vfs_lock_ptr);

    return ret;
}

void vfs_rewinddir(vfs_dir_t *dir)
{
    vfs_file_t  *f;
    vfs_inode_t *node;

    if (dir == NULL) {
        return;
    }

    f = vfs_file_get(dir->dd_vfs_fd);
    if (f == NULL) {
        return;
    }

    node = f->node;

    if (INODE_IS_FS(node)) {
        if ((node->ops.i_fops->rewinddir) != NULL) {
            (node->ops.i_fops->rewinddir)(f, dir);
        }
    }

    return;
}

int32_t vfs_telldir(vfs_dir_t *dir)
{
    vfs_file_t  *f;
    vfs_inode_t *node;

    int32_t ret = 0;

    if (dir == NULL) {
        return VFS_ERR_INVAL;
    }

    f = vfs_file_get(dir->dd_vfs_fd);
    if (f == NULL) {
        return VFS_ERR_NOENT;
    }

    node = f->node;

    if (INODE_IS_FS(node)) {
        if ((node->ops.i_fops->telldir) != NULL) {
            ret = (node->ops.i_fops->telldir)(f, dir);
        }
    }

    return ret;
}

void vfs_seekdir(vfs_dir_t *dir, int32_t loc)
{
    vfs_file_t  *f;
    vfs_inode_t *node;

    if (dir == NULL) {
        return;
    }

    f = vfs_file_get(dir->dd_vfs_fd);
    if (f == NULL) {
        return;
    }

    node = f->node;

    if (INODE_IS_FS(node)) {
        if ((node->ops.i_fops->seekdir) != NULL) {
            (node->ops.i_fops->seekdir)(f, dir, loc);
        }
    }

    return;
}

int32_t vfs_statfs(const char *path, vfs_statfs_t *buf)
{
    int32_t ret = VFS_ERR_NOSYS;

    vfs_file_t  *f;
    vfs_inode_t *node;

    if (path == NULL) {
        return VFS_ERR_INVAL;
    }

    if (vfs_lock(g_vfs_lock_ptr) != VFS_OK) {
        return VFS_ERR_LOCK;
    }

    node = vfs_inode_open(path);

    if (node == NULL) {
        vfs_unlock(g_vfs_lock_ptr);
        return VFS_ERR_NODEV;
    }

    f = vfs_file_new(node);

    vfs_unlock(g_vfs_lock_ptr);

    if (f == NULL) {
        return VFS_ERR_NOENT;
    }

#ifdef CONFIG_VFS_LSOPEN
    strncpy(f->filename, path, sizeof(f->filename) - 1);
#endif

    if (INODE_IS_FS(node)) {
        if ((node->ops.i_fops->statfs) != NULL) {
            ret = (node->ops.i_fops->statfs)(f, path, buf);
        }
    }

    if (vfs_lock(g_vfs_lock_ptr) != VFS_OK) {
        return VFS_ERR_LOCK;
    }

    vfs_file_del(f);
    vfs_unlock(g_vfs_lock_ptr);

    return ret;
}

int32_t vfs_access(const char *path, int32_t amode)
{
    int32_t ret = VFS_ERR_NOSYS;

    vfs_file_t  *f;
    vfs_inode_t *node;

    if (path == NULL) {
        return VFS_ERR_INVAL;
    }

    if (vfs_lock(g_vfs_lock_ptr) != VFS_OK) {
        return VFS_ERR_LOCK;
    }

    node = vfs_inode_open(path);

    if (node == NULL) {
        vfs_unlock(g_vfs_lock_ptr);
        return VFS_ERR_NODEV;
    }

    f = vfs_file_new(node);

    vfs_unlock(g_vfs_lock_ptr);

    if (f == NULL) {
        return VFS_ERR_NOENT;
    }

#ifdef CONFIG_VFS_LSOPEN
    strncpy(f->filename, path, sizeof(f->filename) - 1);
#endif

    if (INODE_IS_FS(node)) {
        if ((node->ops.i_fops->access) != NULL) {
            ret = (node->ops.i_fops->access)(f, path, amode);
        }
    }

    if (vfs_lock(g_vfs_lock_ptr) != VFS_OK) {
        return VFS_ERR_LOCK;
    }

    vfs_file_del(f);
    vfs_unlock(g_vfs_lock_ptr);

    return ret;
}

int vfs_chdir(const char *path)
{
#if (CURRENT_WORKING_DIRECTORY_ENABLE > 0)
    if ((path == NULL) || (strlen(path) > VFS_PATH_MAX)) {
        return VFS_ERR_NAMETOOLONG;
    }

    if (vfs_access(path, F_OK) != VFS_OK) {
        return VFS_ERR_INVAL;
    }

    if (vfs_lock(g_vfs_lock_ptr) != VFS_OK) {
        return VFS_ERR_LOCK;
    }

    memset(g_current_working_directory, 0, sizeof(g_current_working_directory));
    strncpy(g_current_working_directory, path, strlen(path) + 1);

    vfs_unlock(g_vfs_lock_ptr);

    return VFS_OK;
#else
    return VFS_ERR_INVAL;
#endif
}

char *vfs_getcwd(char *buf, size_t size)
{
#if (CURRENT_WORKING_DIRECTORY_ENABLE > 0)
    if ((buf == NULL) || (size <= strlen(g_current_working_directory))) {
        return NULL;
    }

    strncpy(buf, g_current_working_directory, strlen(g_current_working_directory) + 1);

    return buf;
#else
    return NULL;
#endif
}

int32_t vfs_pathconf(const char *path, int name)
{
    int32_t ret = VFS_ERR_NOSYS;

    vfs_file_t  *f;
    vfs_inode_t *node;

    if (path == NULL) {
        return VFS_ERR_INVAL;
    }

    if (vfs_lock(g_vfs_lock_ptr) != VFS_OK) {
        return VFS_ERR_LOCK;
    }

    node = vfs_inode_open(path);

    if (node == NULL) {
        vfs_unlock(g_vfs_lock_ptr);
        return VFS_ERR_NODEV;
    }

    f = vfs_file_new(node);

    vfs_unlock(g_vfs_lock_ptr);

    if (f == NULL) {
        return VFS_ERR_NOENT;
    }

#ifdef CONFIG_VFS_LSOPEN
    strncpy(f->filename, path, sizeof(f->filename) - 1);
#endif

    if (INODE_IS_FS(node)) {
        if ((node->ops.i_fops->pathconf) != NULL) {
            ret = (node->ops.i_fops->pathconf)(f, path, name);
        }
    }

    if (vfs_lock(g_vfs_lock_ptr) != VFS_OK) {
        return VFS_ERR_LOCK;
    }

    vfs_file_del(f);
    vfs_unlock(g_vfs_lock_ptr);

    return ret;
}

int32_t vfs_fpathconf(int fd, int name)
{
    int32_t ret = VFS_ERR_NOSYS;

    vfs_file_t  *file;
    vfs_inode_t *node;

    file = vfs_file_get(fd);
    if (file == NULL) {
        return VFS_ERR_NOENT;
    }

    node = file->node;

    if (INODE_IS_FS(node)) {
        if ((node->ops.i_fops->fpathconf) != NULL) {
            ret = (node->ops.i_fops->fpathconf)(file, name);
        }
    }

    return ret;
}

int vfs_utime(const char *path, const vfs_utimbuf_t *times)
{
    int32_t ret = VFS_ERR_NOSYS;

    vfs_file_t  *f;
    vfs_inode_t *node;

    if (path == NULL) {
        return VFS_ERR_INVAL;
    }

    if (vfs_lock(g_vfs_lock_ptr) != VFS_OK) {
        return VFS_ERR_LOCK;
    }

    node = vfs_inode_open(path);

    if (node == NULL) {
        vfs_unlock(g_vfs_lock_ptr);
        return VFS_ERR_NODEV;
    }

    f = vfs_file_new(node);

    vfs_unlock(g_vfs_lock_ptr);

    if (f == NULL) {
        return VFS_ERR_NOENT;
    }

#ifdef CONFIG_VFS_LSOPEN
    strncpy(f->filename, path, sizeof(f->filename) - 1);
#endif

    if (INODE_IS_FS(node)) {
        if ((node->ops.i_fops->utime) != NULL) {
            ret = (node->ops.i_fops->utime)(f, path, times);
        }
    }

    if (vfs_lock(g_vfs_lock_ptr) != VFS_OK) {
        return VFS_ERR_LOCK;
    }

    vfs_file_del(f);
    vfs_unlock(g_vfs_lock_ptr);

    return ret;
}

int32_t vfs_fd_offset_get(void)
{
    return VFS_FD_OFFSET;
}

int32_t vfs_socket(const char *path, int32_t domain, int32_t type, int32_t protocol)
{
    int32_t len = 0;
    int32_t ret = VFS_OK;

    vfs_file_t  *f;
    vfs_inode_t *node;

    len = strlen(path);
    if (len > VFS_PATH_MAX) {
        return VFS_ERR_NAMETOOLONG;
    }

    if (vfs_lock(g_vfs_lock_ptr) != VFS_OK) {
        return VFS_ERR_LOCK;
    }

    node = vfs_inode_open(path);
    if (node == NULL) {
        vfs_unlock(g_vfs_lock_ptr);

#ifdef IO_NEED_TRAP
        return trap_open(path, flags);
#else
        return VFS_ERR_NOENT;
#endif
    }

    f = vfs_file_new(node);

    if (f == NULL) {
        vfs_unlock(g_vfs_lock_ptr);
        return VFS_ERR_ENFILE;
    }

#ifdef CONFIG_VFS_LSOPEN
    strncpy(f->filename, path, sizeof(f->filename) - 1);
#endif

    if (INODE_IS_NETWORK(node)) {
        if ((node->ops.i_nops->socket) != NULL) {
            ret = (node->ops.i_nops->socket)(domain, type, protocol);
            f->param = ret;
        }
    }

    if (ret < 0) {
        vfs_file_del(f);
    } else {
        ret = vfs_fd_get(f);
        vfs_fd_mark_open(ret);
    }

    vfs_unlock(g_vfs_lock_ptr);

    return ret;
}

vfs_inode_t *get_node(int32_t fd)
{
    vfs_file_t  *f;

    if (fd < 0) {
        return NULL;
    }

    f = vfs_file_get(fd);
    if (f == NULL) {
        return NULL;
    }

    if (INODE_IS_NETWORK(f->node)) {
        return f->node;
    }
    return NULL;
}


int32_t vfs_accept(const char *path, int32_t fd, struct sockaddr *addr, socklen_t *addrlen)
{
    int32_t len = 0;
    int32_t ret = VFS_OK;

    vfs_file_t  *f;
    vfs_inode_t *node;
    vfs_file_t  *socket_file;

    socket_file = vfs_file_get(fd);
    if (socket_file == NULL) {
        return VFS_ERR_NOENT;
    }

    len = strlen(path);
    if (len > VFS_PATH_MAX) {
        return VFS_ERR_NAMETOOLONG;
    }

    if (vfs_lock(g_vfs_lock_ptr) != VFS_OK) {
        return VFS_ERR_LOCK;
    }

    node = vfs_inode_open(path);
    if (node == NULL) {
        vfs_unlock(g_vfs_lock_ptr);

#ifdef IO_NEED_TRAP
        return trap_open(path, flags);
#else
        return VFS_ERR_NOENT;
#endif
    }

    f = vfs_file_new(node);

    if (f == NULL) {
        vfs_unlock(g_vfs_lock_ptr);
        return VFS_ERR_ENFILE;
    }
#ifdef CONFIG_VFS_LSOPEN
    strncpy(f->filename, path, sizeof(f->filename) - 1);
#endif
    vfs_unlock(g_vfs_lock_ptr);

    if (INODE_IS_NETWORK(node)) {
        if ((node->ops.i_nops->accept) != NULL) {
            ret = (node->ops.i_nops->accept)(socket_file->param, addr, addrlen);
            f->param = ret;
        }
    }
    if (vfs_lock(g_vfs_lock_ptr) != VFS_OK) {
        return VFS_ERR_LOCK;
    }
    if (ret < 0) {
        vfs_file_del(f);
    } else {
        ret = vfs_fd_get(f);
        vfs_fd_mark_open(ret);
    }

    vfs_unlock(g_vfs_lock_ptr);

    return ret;
}

int32_t vfs_bind(int fd, const struct sockaddr *name, socklen_t namelen)
{
    int32_t ret = VFS_ERR_NOSYS;
    vfs_inode_t *node = get_node(fd);
    vfs_file_t  *file;

    file = vfs_file_get(fd);
    if (file == NULL) {
        return VFS_ERR_NOENT;
    }

    if (node != NULL) {
        if ((node->ops.i_nops->bind) != NULL) {
            ret = (node->ops.i_nops->bind)(file->param, name, namelen);
        }
    }

    return ret;
}

int32_t vfs_shutdown(int32_t fd, int32_t how)
{
    int32_t ret = VFS_ERR_NOSYS;
    vfs_inode_t *node = get_node(fd);
    vfs_file_t  *file;

    file = vfs_file_get(fd);
    if (file == NULL) {
        return VFS_ERR_NOENT;
    }

    if (node != NULL) {
        if ((node->ops.i_nops->shutdown) != NULL) {
            ret = (node->ops.i_nops->shutdown)(file->param, how);
        }
    }

    return ret;
}

int32_t vfs_getpeername(int32_t fd, struct sockaddr *name, socklen_t *namelen)
{
    int32_t ret = VFS_ERR_NOSYS;
    vfs_inode_t *node = get_node(fd);
    vfs_file_t  *file;

    file = vfs_file_get(fd);
    if (file == NULL) {
        return VFS_ERR_NOENT;
    }

    if (node != NULL) {
        if ((node->ops.i_nops->getpeername) != NULL) {
            ret = (node->ops.i_nops->getpeername)(file->param, name, namelen);
        }
    }

    return ret;
}

int32_t vfs_getsockname(int32_t fd, struct sockaddr *name, socklen_t *namelen)
{
    int32_t ret = VFS_ERR_NOSYS;
    vfs_inode_t *node = get_node(fd);
    vfs_file_t  *file;

    file = vfs_file_get(fd);
    if (file == NULL) {
        return VFS_ERR_NOENT;
    }

    if (node != NULL) {
        if ((node->ops.i_nops->getsockname) != NULL) {
            ret = (node->ops.i_nops->getsockname)(file->param, name, namelen);
        }
    }

    return ret;
}

int32_t vfs_getsockopt(int32_t fd, int32_t level, int32_t optname, void *optval, socklen_t *optlen)
{
    int32_t ret = VFS_ERR_NOSYS;
    vfs_inode_t *node = get_node(fd);
    vfs_file_t  *file;

    file = vfs_file_get(fd);
    if (file == NULL) {
        return VFS_ERR_NOENT;
    }

    if (node != NULL) {
        if ((node->ops.i_nops->getsockopt) != NULL) {
            ret = (node->ops.i_nops->getsockopt)(file->param, level, optname, optval, optlen);
        }
    }

    return ret;
}

int32_t vfs_setsockopt(int32_t fd, int32_t level, int32_t optname,const void *optval, socklen_t optlen)
{
    int32_t ret = VFS_ERR_NOSYS;
    vfs_inode_t *node = get_node(fd);
    vfs_file_t  *file;

    file = vfs_file_get(fd);
    if (file == NULL) {
        return VFS_ERR_NOENT;
    }

    if (node != NULL) {
        if ((node->ops.i_nops->setsockopt) != NULL) {
            ret = (node->ops.i_nops->setsockopt)(file->param, level, optname, optval, optlen);
        }
    }

    return ret;
}

int32_t vfs_connect(int32_t fd,const struct sockaddr *name, socklen_t namelen)
{
    int32_t ret = VFS_ERR_NOSYS;
    vfs_inode_t *node = get_node(fd);
    vfs_file_t  *file;

    file = vfs_file_get(fd);
    if (file == NULL) {
        return VFS_ERR_NOENT;
    }

    if (node != NULL) {
        if ((node->ops.i_nops->connect) != NULL) {
            ret = (node->ops.i_nops->connect)(file->param, name, namelen);
        }
    }

    return ret;
}

int32_t vfs_listen(int32_t fd, int32_t backlog)
{
    int32_t ret = VFS_ERR_NOSYS;
    vfs_inode_t *node = get_node(fd);
    vfs_file_t  *file;

    file = vfs_file_get(fd);
    if (file == NULL) {
        return VFS_ERR_NOENT;
    }

    if (node != NULL) {
        if ((node->ops.i_nops->listen) != NULL) {
            ret = (node->ops.i_nops->listen)(file->param, backlog);
        }
    }
    return ret;
}

int32_t vfs_recv(int32_t fd, void *mem, size_t len, int32_t flags)
{
    int32_t ret = VFS_ERR_NOSYS;
    vfs_inode_t *node = get_node(fd);
    vfs_file_t  *file;

    file = vfs_file_get(fd);
    if (file == NULL) {
        return VFS_ERR_NOENT;
    }
    if (node != NULL) {
        if ((node->ops.i_nops->recv) != NULL) {
            ret = (node->ops.i_nops->recv)(file->param, mem, len, flags);
        }
    }
    return ret;
}

int32_t vfs_recvfrom(int32_t fd, void *mem, size_t len, int32_t flags,
        struct sockaddr *from, socklen_t *fromlen)
{
    int32_t ret = VFS_ERR_NOSYS;
    vfs_inode_t *node = get_node(fd);
    vfs_file_t  *file;

    file = vfs_file_get(fd);
    if (file == NULL) {
        return VFS_ERR_NOENT;
    }

    if (node != NULL) {
        if ((node->ops.i_nops->recvfrom) != NULL) {
            ret = (node->ops.i_nops->recvfrom)(file->param, mem, len, flags, from, fromlen);
        }
    }
    return ret;
}

int32_t vfs_send(int32_t fd, const void *dataptr, size_t size, int32_t flags)
{
    int32_t ret = VFS_ERR_NOSYS;
    vfs_inode_t *node = get_node(fd);
    vfs_file_t  *file;

    file = vfs_file_get(fd);
    if (file == NULL) {
        return VFS_ERR_NOENT;
    }

    if (node != NULL) {
        if ((node->ops.i_nops->send) != NULL) {
            ret = (node->ops.i_nops->send)(file->param, dataptr, size, flags);
        }
    }
    return ret;
}

int32_t vfs_sendmsg(int32_t fd, const struct msghdr *message, int32_t flags)
{
    int32_t ret = VFS_ERR_NOSYS;
    vfs_inode_t *node = get_node(fd);
    vfs_file_t  *file;

    file = vfs_file_get(fd);
    if (file == NULL) {
        return VFS_ERR_NOENT;
    }

    if (node != NULL) {
        if ((node->ops.i_nops->sendmsg) != NULL) {
            ret = (node->ops.i_nops->sendmsg)(file->param, message, flags);
        }
    }
    return ret;
}

int32_t vfs_writev(int32_t fd, const struct iovec *iov, int32_t iovcnt)
{
    int32_t ret = VFS_ERR_NOSYS;
    vfs_inode_t *node = get_node(fd);
    vfs_file_t  *file;

    file = vfs_file_get(fd);
    if (file == NULL) {
        return VFS_ERR_NOENT;
    }

    if (node != NULL) {
        if ((node->ops.i_nops->writev) != NULL) {
            ret = (node->ops.i_nops->writev)(file->param, iov, iovcnt);
        }
    }
    return ret;
}

int32_t vfs_eventfd(uint32_t initval, int32_t flags)
{
    //TODO
    return -1;
}

int32_t vfs_try_wakeup(int32_t fd, int32_t rcvevent, int32_t sendevent, int32_t errevent)
{
    int32_t ret = VFS_ERR_NOSYS;
    vfs_inode_t *node = get_node(fd);
    vfs_file_t  *file;

    file = vfs_file_get(fd);
    if (file == NULL) {
        return VFS_ERR_NOENT;
    }

    if (node != NULL) {
        if ((node->ops.i_nops->try_wakeup) != NULL) {
            ret = (node->ops.i_nops->try_wakeup)(file->param, rcvevent, sendevent, errevent);
        }
    }
    return ret;
}
int32_t vfs_sendto(int32_t fd, const void *dataptr, size_t size, int32_t flags,
        const struct sockaddr *to, socklen_t tolen)
{
    int32_t ret = VFS_ERR_NOSYS;
    vfs_inode_t *node = get_node(fd);
    vfs_file_t  *file;

    file = vfs_file_get(fd);
    if (file == NULL) {
        return VFS_ERR_NOENT;
    }

    if (node != NULL) {
        if ((node->ops.i_nops->sendto) != NULL) {
            ret = (node->ops.i_nops->sendto)(file->param, dataptr, size, flags, to, tolen);
        }
    }
    return ret;
}

struct hostent *vfs_gethostbyname(const char *path, const char *name)
{
    struct hostent *ret = NULL;
    vfs_inode_t *node;

    if (path == NULL) {
        return ret;
    }

    if (vfs_lock(g_vfs_lock_ptr) != VFS_OK) {
        return NULL;
    }

    node = vfs_inode_open(path);

    if (node == NULL) {
        vfs_unlock(g_vfs_lock_ptr);
        return NULL;
    }
    vfs_unlock(g_vfs_lock_ptr);

    if (INODE_IS_NETWORK(node)) {
        if ((node->ops.i_nops->gethostbyname) != NULL) {
            ret = (node->ops.i_nops->gethostbyname)(name);
        }
    }

    return ret;
}

int vfs_gethostbyname_r(const char *path, const char *name, struct hostent *ret, char *buf,
                size_t buflen, struct hostent **result, int *h_errnop)
{
    int32_t ret_value = VFS_ERR_NOSYS;
    vfs_inode_t *node;

    if (path == NULL) {
        return VFS_ERR_INVAL;
    }

    if (vfs_lock(g_vfs_lock_ptr) != VFS_OK) {
        return VFS_ERR_LOCK;
    }

    node = vfs_inode_open(path);

    if (node == NULL) {
        vfs_unlock(g_vfs_lock_ptr);
        return VFS_ERR_NODEV;
    }
    vfs_unlock(g_vfs_lock_ptr);

    if (INODE_IS_NETWORK(node)) {
        if ((node->ops.i_nops->gethostbyname_r) != NULL) {
            ret_value = (node->ops.i_nops->gethostbyname_r)(name, ret, buf, buflen, result, h_errnop);
        }
    }

    return ret_value;
}

void vfs_freeaddrinfo(const char *path, struct addrinfo *ai)
{
    vfs_inode_t *node;

    if (path == NULL) {
        return;
    }

    if (vfs_lock(g_vfs_lock_ptr) != VFS_OK) {
        return;
    }

    node = vfs_inode_open(path);

    if (node == NULL) {
        vfs_unlock(g_vfs_lock_ptr);
        return;
    }
    vfs_unlock(g_vfs_lock_ptr);

    if (INODE_IS_NETWORK(node)) {
        if ((node->ops.i_nops->freeaddrinfo) != NULL) {
            (node->ops.i_nops->freeaddrinfo)(ai);
        }
    }

    return;
}

int vfs_getaddrinfo(const char *path, const char *nodename,
       const char *servname,
       const struct addrinfo *hints,
       struct addrinfo **res)
{
    int32_t ret = VFS_ERR_NOSYS;
    vfs_inode_t *node;

    if (path == NULL) {
        return VFS_ERR_INVAL;
    }

    if (vfs_lock(g_vfs_lock_ptr) != VFS_OK) {
        return VFS_ERR_LOCK;
    }

    node = vfs_inode_open(path);

    if (node == NULL) {
        vfs_unlock(g_vfs_lock_ptr);
        return VFS_ERR_NODEV;
    }
    vfs_unlock(g_vfs_lock_ptr);

    if (INODE_IS_NETWORK(node)) {
        if ((node->ops.i_nops->getaddrinfo) != NULL) {
            ret = (node->ops.i_nops->getaddrinfo)(nodename, servname, hints, res);
        }
    }

    return ret;
}

int32_t vfs_register_network(const char *path, vfs_network_ops_t *ops)
{
    int32_t ret = VFS_ERR_NOSYS;

    vfs_inode_t *node = NULL;

    if (vfs_lock(g_vfs_lock_ptr) != VFS_OK) {
        return VFS_ERR_LOCK;
    }

    ret = vfs_inode_reserve(path, &node);
    if (ret == VFS_OK) {
        INODE_SET_NETWORK(node);

        node->ops.i_nops = ops;
        node->i_arg      = NULL;
    }

    if (vfs_unlock(g_vfs_lock_ptr) != VFS_OK) {
        if (node->i_name != NULL) {
            vfs_free(node->i_name);
        }

        memset(node, 0, sizeof(vfs_inode_t));
        return VFS_ERR_LOCK;
    }

    return ret;
}

int32_t vfs_unregister_network(const char *path)
{
    int32_t ret;

    if (vfs_lock(g_vfs_lock_ptr) != VFS_OK) {
        return VFS_ERR_LOCK;
    }

    ret = vfs_inode_release(path);

    if (vfs_unlock(g_vfs_lock_ptr) != VFS_OK) {
        return VFS_ERR_LOCK;
    }

    return ret;
}

int32_t vfs_register_driver(const char *path, vfs_file_ops_t *ops, void *arg)
{
    int32_t ret = VFS_ERR_NOSYS;

    vfs_inode_t *node = NULL;

    if (vfs_lock(g_vfs_lock_ptr) != VFS_OK) {
        return VFS_ERR_LOCK;
    }

    ret = vfs_inode_reserve(path, &node);
    if (ret == VFS_OK) {
        INODE_SET_CHAR(node);

        node->ops.i_ops = ops;
        node->i_arg     = arg;
    }

    if (vfs_unlock(g_vfs_lock_ptr) != VFS_OK) {
        if (node->i_name != NULL) {
            vfs_free(node->i_name);
        }

        memset(node, 0, sizeof(vfs_inode_t));
        return VFS_ERR_LOCK;
    }

    return ret;
}

int32_t vfs_unregister_driver(const char *path)
{
    int32_t ret;

    if (vfs_lock(g_vfs_lock_ptr) != VFS_OK) {
        return VFS_ERR_LOCK;
    }

    ret = vfs_inode_release(path);

    if (vfs_unlock(g_vfs_lock_ptr) != VFS_OK) {
        return VFS_ERR_LOCK;
    }

    return ret;
}

int32_t vfs_register_fs(const char *path, vfs_filesystem_ops_t *ops, void *arg)
{
    int32_t ret;

    vfs_inode_t *node = NULL;

    if (vfs_lock(g_vfs_lock_ptr) != VFS_OK) {
        return VFS_ERR_LOCK;
    }

    ret = vfs_inode_reserve(path, &node);
    if (ret == VFS_OK) {
        INODE_SET_FS(node);

        node->ops.i_fops = ops;
        node->i_arg      = arg;
    }

    if (vfs_unlock(g_vfs_lock_ptr) != VFS_OK) {
        if (node->i_name != NULL) {
            vfs_free(node->i_name);
        }

        memset(node, 0, sizeof(vfs_inode_t));
        return VFS_ERR_LOCK;
    }

    return ret;
}

int32_t vfs_unregister_fs(const char *path)
{
    int32_t ret;

    if (vfs_lock(g_vfs_lock_ptr) != VFS_OK) {
        return VFS_ERR_LOCK;
    }

    ret = vfs_inode_release(path);

    if (vfs_unlock(g_vfs_lock_ptr) != VFS_OK) {
        return VFS_ERR_LOCK;
    }

    return ret;
}

int vfs_fcntl(int fd, int cmd, int val)
{
    int32_t ret = VFS_ERR_NOSYS;
    vfs_file_t  *file;
    vfs_inode_t *node;

    if (fd < 0) {
        return VFS_ERR_INVAL;
    }

    file = vfs_file_get(fd);
    if (file == NULL) {
        return VFS_ERR_NOENT;
    }

    node = file->node;

    if (INODE_IS_NETWORK(node)) {
        if ((node->ops.i_nops->fcntl) != NULL) {
            ret = (node->ops.i_nops->fcntl)(file->param, cmd, val);
            return ret;
        }
    }

    if (fd < vfs_fd_offset_get()) {
#ifdef IO_NEED_TRAP
        return trap_fcntl(fd, cmd, val);
#else
        return VFS_ERR_NOENT;
#endif
    }

    return 0;
}

int32_t vfs_get_file_param(int32_t fd)
{
    vfs_file_t  *socket_file;

    socket_file = vfs_file_get(fd);
    if (socket_file == NULL) {
            return VFS_ERR_NOENT;
    }

    return socket_file->param;
}

int vfs_init_eventfd(const char *path, unsigned int initval, int flags)
{
    int32_t ret = VFS_ERR_NOSYS;
    vfs_inode_t *node;

    if (path == NULL) {
        return VFS_ERR_INVAL;
    }

    if (vfs_lock(g_vfs_lock_ptr) != VFS_OK) {
        return VFS_ERR_LOCK;
    }

    node = vfs_inode_open(path);

    if (node == NULL) {
        vfs_unlock(g_vfs_lock_ptr);
        return VFS_ERR_NODEV;
    }
    vfs_unlock(g_vfs_lock_ptr);

    if (INODE_IS_NETWORK(node)) {
        if ((node->ops.i_nops->eventfd) != NULL) {
            ret = (node->ops.i_nops->eventfd)(initval, flags);
        }
    }

    return ret;
}

int vfs_select(const char *path, int maxfdp1, fd_set *readset, fd_set *writeset, fd_set *exceptset, struct timeval *timeout)
{
    int32_t ret = VFS_ERR_NOSYS;
    vfs_inode_t *node;

    if (path == NULL) {
        return VFS_ERR_INVAL;
    }

    if (vfs_lock(g_vfs_lock_ptr) != VFS_OK) {
        return VFS_ERR_LOCK;
    }

    node = vfs_inode_open(path);

    if (node == NULL) {
        vfs_unlock(g_vfs_lock_ptr);
        return VFS_ERR_NODEV;
    }
    vfs_unlock(g_vfs_lock_ptr);

    if (INODE_IS_NETWORK(node)) {
        if ((node->ops.i_nops->select) != NULL) {
            ret = (node->ops.i_nops->select)(maxfdp1, readset, writeset, exceptset, timeout);
        }
    }

    return ret;
}

#ifdef CONFIG_VFS_LSOPEN
void vfs_dump_open()
{
    vfs_lock(g_vfs_lock_ptr);
    vfs_file_open_dump();
    vfs_unlock(g_vfs_lock_ptr);
}
#endif

int32_t vfs_list(vfs_list_type_t t)
{
    return vfs_inode_list(t);
}
