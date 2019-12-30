/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "vfs_types.h"
#include "vfs_api.h"
#include "vfs_conf.h"

#include "vfs_inode.h"
#include "vfs_file.h"
#include "vfs_adapt.h"

#ifdef IO_NEED_TRAP
#include "vfs_trap.h"
#endif

static uint8_t  g_vfs_init     = 0;
static void    *g_vfs_lock_ptr = NULL;

#if (CURRENT_WORKING_DIRECTORY_ENABLE > 0)
char g_current_working_directory[VFS_PATH_MAX];
#endif

int32_t vfs_init(void)
{
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
    strcpy(g_current_working_directory, "/default");
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

    vfs_unlock(g_vfs_lock_ptr);

    if (f == NULL) {
        return VFS_ERR_ENFILE;
    }

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
        return ret;
    }

    return vfs_fd_get(f);
}

int32_t vfs_close(int32_t fd)
{
    int32_t ret = VFS_OK;

    vfs_file_t *f;
    vfs_inode_t *node;

    f = vfs_file_get(fd);

    if (f == NULL) {
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
    } else {
        if ((node->ops.i_ops->close) != NULL) {
            ret = (node->ops.i_ops->close)(f);
        }
    }

    if (vfs_lock(g_vfs_lock_ptr) != VFS_OK) {
        return VFS_ERR_LOCK;
    }

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
    }

    return ret;
}

int32_t vfs_sync(int32_t fd)
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

    if (INODE_IS_FS(node)) {
        if ((node->ops.i_fops->stat) != NULL) {
            ret = (node->ops.i_fops->stat)(f, path, st);
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

int32_t vfs_link(const char *path1, const char *path2)
{
    int32_t ret = VFS_ERR_NOSYS;

    vfs_file_t  *f;
    vfs_inode_t *node;

    if ((path1 == NULL)||(path2 == NULL)) {
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
    if ((path == NULL) || (strlen(path) > VFS_PATH_MAX)){
        return VFS_ERR_NAMETOOLONG;
    }

    if (vfs_lock(g_vfs_lock_ptr) != VFS_OK) {
        return VFS_ERR_LOCK;
    }

    memset(g_current_working_directory, 0, sizeof(g_current_working_directory));
    strcpy(g_current_working_directory, path);

    vfs_unlock(g_vfs_lock_ptr);

    return VFS_OK;
#else
    return VFS_ERR_INVAL;
#endif
}

char *vfs_getcwd(char *buf, size_t size)
{
#if (CURRENT_WORKING_DIRECTORY_ENABLE > 0)
    if ((buf == NULL) || (size < strlen(g_current_working_directory))) {
        return NULL;
    }

    strcpy(buf, g_current_working_directory);

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

int32_t vfs_register_fs(const char *path, vfs_filesystem_ops_t* ops, void *arg)
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
