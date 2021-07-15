/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <aos/errno.h>

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
static void    *g_stdio_lock_ptr = NULL;
static int32_t  stdio_redirect_fd[3] = {-1, -1, -1}; // 0: stdin, 1: stdout, 2: stderr

int32_t vfs_inode_list(vfs_list_type_t type);
int vfs_inode_get_names(const char *path, char names[][64], uint32_t* size);

#if (CURRENT_WORKING_DIRECTORY_ENABLE > 0)
char g_current_working_directory[VFS_PATH_MAX];
#endif

extern int uring_fifo_push_s(const void* buf, const uint16_t len);
static int32_t write_stdout(const void *buf, uint32_t nbytes)
{
    uring_fifo_push_s(buf, nbytes);
    return nbytes;
}

static int is_stdio_fd(int32_t fd)
{
    return fd == STDIN_FILENO || fd == STDOUT_FILENO || fd == STDERR_FILENO? 1 : 0;
}

static int32_t find_stdio_redirect_fd(int32_t fd)
{
    int32_t rfd = -1;

    if (vfs_lock(g_stdio_lock_ptr) != VFS_OK) {
        return -1;
    }

    if (fd == STDIN_FILENO) {
        rfd = stdio_redirect_fd[0];
    } else if (fd == STDOUT_FILENO) {
        rfd = stdio_redirect_fd[1];
    } else if (fd == STDERR_FILENO) {
        rfd = stdio_redirect_fd[2];
    }

    vfs_unlock(g_stdio_lock_ptr);

    return rfd;
}

static int32_t set_stdio_redirect_fd(int32_t sfd, int32_t rfd)
{
    int ret = 0, real_rfd = rfd - VFS_FD_OFFSET;

    if (vfs_lock(g_stdio_lock_ptr) != VFS_OK) {
        return -1;
    }

    if (real_rfd < 0 || real_rfd >= VFS_MAX_FILE_NUM || !vfs_fd_is_open(rfd)) {
        vfs_unlock(g_vfs_lock_ptr);
        return -1;
    }

    if (sfd == STDIN_FILENO) {
        stdio_redirect_fd[0] = rfd;
    } else if (sfd == STDOUT_FILENO) {
        stdio_redirect_fd[1] = rfd;
    } else if (sfd == STDERR_FILENO) {
        stdio_redirect_fd[2] = rfd;
    } else {
        ret = -1;
    }

    vfs_unlock(g_stdio_lock_ptr);

    return ret;
}

static int clear_stdio_redirect_fd(int fd)
{
    int ret = 0;

    if (vfs_lock(g_stdio_lock_ptr) != VFS_OK) {
        return -1;
    }

    if (fd == STDIN_FILENO) {
        stdio_redirect_fd[0] = -1;
    } else if (fd == STDOUT_FILENO) {
        stdio_redirect_fd[1] = -1;
    } else if (fd == STDERR_FILENO) {
        stdio_redirect_fd[2] = -1;
    } else {
        ret = -1;
    }

    vfs_unlock(g_stdio_lock_ptr);

    return ret;
}

static int32_t vfs_close_without_glock(int32_t fd)
{
    int32_t ret = VFS_OK;
    vfs_file_t *f;
    vfs_inode_t *node;

    f = vfs_file_get(fd);
    if (f == NULL) return VFS_ERR_NOENT;

    node = f->node;
    assert(node != (vfs_inode_t *)(-1));

    if (vfs_lock(node->lock) != VFS_OK) {
        VFS_ERROR("%s failed to lock inode %p!\n\r", __func__, node);
        return VFS_ERR_LOCK;
    }

    if (node->status != VFS_INODE_VALID) {
        vfs_unlock(node->lock);
        VFS_ERROR("%s node %p is invalid now!\n\r", __func__, node);
        goto end;
    }

    if (INODE_IS_FS(node)) {
        if ((node->ops.i_fops->close) != NULL) {
            ret = (node->ops.i_fops->close)(f);
        }
    } else {
        if ((node->ops.i_ops->close) != NULL) {
            ret = (node->ops.i_ops->close)(f);
        }
    }

    vfs_unlock(node->lock);

end:
    vfs_fd_mark_close(fd);
    vfs_file_del(f);
    return ret;
}

static int32_t set_normal_redirect_fd(int32_t oldfd, int32_t newfd)
{
    int ret = -1, realold = oldfd - VFS_FD_OFFSET;
    vfs_file_t *f;

    if (vfs_lock(g_vfs_lock_ptr) != VFS_OK) {
        return -1;
    }

    if (realold < 0 || realold >= VFS_MAX_FILE_NUM || !vfs_fd_is_open(oldfd)) {
        vfs_unlock(g_vfs_lock_ptr);
        return -1;
    }

    if (vfs_fd_is_open(newfd)) {
       if (vfs_close_without_glock(newfd) != VFS_OK) {
           goto end;
       }
    }

    f = vfs_file_get2(newfd);
    if (!f) goto end;

    f->redirect_fd = oldfd;
    /* -1 as *node, just for NULL check, do NOT really use it! */
    f->node = (vfs_inode_t *)(-1);
    vfs_fd_mark_open(newfd);
    ret = newfd;

end:
    vfs_unlock(g_vfs_lock_ptr);
    return ret;
}

static int clear_normal_redirect_fd(int32_t fd)
{
    int ret = -1;
    vfs_file_t *f;

    if (vfs_lock(g_vfs_lock_ptr) != VFS_OK) {
        return VFS_ERR_LOCK;
    }

    if (!vfs_fd_is_open(fd)) {
        goto end;
    }

    f = vfs_file_get2(fd);
    if (!f) goto end;

    f->redirect_fd = -1;
    /* -1 as *node, just for NULL check, do NOT really use it! */
    f->node = NULL;
    vfs_fd_mark_close(fd);
    vfs_file_del(f);
    ret = 0;

end:
    vfs_unlock(g_vfs_lock_ptr);
    return ret;
}

int32_t vfs_init(void)
{
    if (g_vfs_init == 1) {
        return VFS_OK;
    }

    g_vfs_lock_ptr = vfs_lock_create();
    if (g_vfs_lock_ptr == NULL) {
        return VFS_ERR_NOMEM;
    }

    g_stdio_lock_ptr = vfs_lock_create();
    if (g_stdio_lock_ptr == NULL) {
        vfs_lock_free(g_vfs_lock_ptr);
        return VFS_ERR_NOMEM;
    }

    vfs_inode_init();

#if (CURRENT_WORKING_DIRECTORY_ENABLE > 0)
    /* init current working directory */
    memset(g_current_working_directory, 0, sizeof(g_current_working_directory));
#ifdef VFS_CONFIG_ROOTFS
    strcpy(g_current_working_directory, "/");
#else
    strncpy(g_current_working_directory, "/default", strlen("/default") + 1);
#endif
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

    f = vfs_file_new(node);

    vfs_unlock(g_vfs_lock_ptr);

    if (f == NULL) {
        return VFS_ERR_ENFILE;
    }

#ifdef CONFIG_VFS_LSOPEN
    strncpy(f->filename, path, sizeof(f->filename) - 1);
#endif

    if (vfs_lock(node->lock) != VFS_OK) {
        VFS_ERROR("%s failed to lock inode %p!\n\r", __func__, node);
        ret = VFS_ERR_LOCK;
        goto end;
    }

    if (node->status != VFS_INODE_VALID) {
        vfs_unlock(node->lock);
        VFS_ERROR("%s node %p is invalid now, %d!\n\r", __func__, node, node->status);
        ret = VFS_ERR_NOENT;
        goto end;
    }

    node->i_flags = flags;

    if (INODE_IS_FS(node)) {
        if ((node->ops.i_fops->open) != NULL) {
            ret = (node->ops.i_fops->open)(f, path, flags);
        }
    } else {
        if ((node->ops.i_ops->open) != NULL) {
            ret = (node->ops.i_ops->open)(node, f);
        }
    }

    vfs_unlock(node->lock);

end:
    if (vfs_lock(g_vfs_lock_ptr) != VFS_OK) {
        return VFS_ERR_LOCK;
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

    /* handle special case forstdio */
    if (is_stdio_fd(fd)) {
        return clear_stdio_redirect_fd(fd) == 0 ? VFS_OK : VFS_ERR_GENERAL;
    }

    if (vfs_lock(g_vfs_lock_ptr) != VFS_OK) {
        return VFS_ERR_LOCK;
    }

    /* check if redirect or not first */
    f = vfs_file_get2(fd);
    if (f == NULL) {
        vfs_unlock(g_vfs_lock_ptr);
        return VFS_ERR_NOENT;
    } else {
        if (f->redirect_fd >= 0) {
            vfs_unlock(g_vfs_lock_ptr);
            return clear_normal_redirect_fd(fd) == 0 ? VFS_OK : VFS_ERR_GENERAL;
        }
    }

    f = vfs_file_get(fd);

    vfs_unlock(g_vfs_lock_ptr);

    if (f == NULL) {
#ifdef IO_NEED_TRAP
        return trap_close(fd);
#else
        return VFS_ERR_NOENT;
#endif
    }

    node = f->node;

    if (vfs_lock(node->lock) != VFS_OK) {
        VFS_ERROR("%s failed to lock inode %p!\n\r", __func__, node);
        return VFS_ERR_LOCK;
    }

    if (node->status != VFS_INODE_VALID) {
        vfs_unlock(node->lock);
        VFS_ERROR("%s node %p is invalid now!\n\r", __func__, node);
        goto end;
    }

    if (INODE_IS_FS(node)) {
        if ((node->ops.i_fops->close) != NULL) {
            ret = (node->ops.i_fops->close)(f);
        }
    } else {
        if ((node->ops.i_ops->close) != NULL) {
            ret = (node->ops.i_ops->close)(f);
        }
    }

    vfs_unlock(node->lock);

end:
    if (vfs_lock(g_vfs_lock_ptr) != VFS_OK) {
        return VFS_ERR_LOCK;
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

    if(node == NULL) {
        return VFS_ERR_NOENT;
    }

    if (vfs_lock(node->lock) != VFS_OK) {
        VFS_ERROR("%s failed to lock inode %p!\n\r", __func__, node);
        return VFS_ERR_LOCK;
    }

    if (node->status != VFS_INODE_VALID) {
        vfs_unlock(node->lock);
        VFS_ERROR("%s node %p is invalid now!\n\r", __func__, node);
        goto ret;
    }

    if (INODE_IS_FS(node)) {
        if ((node->ops.i_fops->read) != NULL) {
            nread = (node->ops.i_fops->read)(f, buf, nbytes);
        }
    } else {
        if ((node->ops.i_ops->read) != NULL) {
            nread = (node->ops.i_ops->read)(f, buf, nbytes);
        }
    }

ret:
    if (vfs_unlock(node->lock) != VFS_OK) {
        VFS_ERROR("%s failed to unlock inode %p!\n\r", __func__, node);
        return VFS_ERR_LOCK;
    }

    return nread;
}

int32_t vfs_write(int32_t fd, const void *buf, uint32_t nbytes)
{
    int32_t nwrite = -1, rfd = -1;

    vfs_file_t  *f;
    vfs_inode_t *node;

    /* handle special case for stdout and stderr */
    if ((fd == STDOUT_FILENO) || (fd == STDERR_FILENO)) {
        if ((rfd = find_stdio_redirect_fd(fd)) >= 0) {
            fd = rfd;
        } else {
            return write_stdout(buf, nbytes);
        }
    }

    f = vfs_file_get(fd);

    if (f == NULL) {
#ifdef IO_NEED_TRAP
        return trap_write(fd, buf, nbytes);
#else
        return VFS_ERR_NOENT;
#endif
    }

    node = f->node;

    if(node == NULL) {
        return VFS_ERR_NOENT;
    }

    if (vfs_lock(node->lock) != VFS_OK) {
        VFS_ERROR("%s failed to lock inode %p!\n\r", __func__, node);
        return VFS_ERR_LOCK;
    }

    if (node->status != VFS_INODE_VALID) {
        vfs_unlock(node->lock);
        VFS_ERROR("%s node %p is invalid now!\n\r", __func__, node);
        goto ret;
    }

    if (INODE_IS_FS(node)) {
        if ((node->ops.i_fops) != NULL) {
            if ((node->ops.i_fops->write) != NULL) {
                nwrite = (node->ops.i_fops->write)(f, buf, nbytes);
            }
        }
    } else {
        if ((node->ops.i_ops) != NULL) {
            if ((node->ops.i_ops->write) != NULL) {
                nwrite = (node->ops.i_ops->write)(f, buf, nbytes);
            }
        }
    }

ret:
    if (vfs_unlock(node->lock) != VFS_OK) {
        VFS_ERROR("%s failed to unlock inode %p!\n\r", __func__, node);
        return VFS_ERR_LOCK;
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

    if(node == NULL) {
        return VFS_ERR_NOENT;
    }

    if (vfs_lock(node->lock) != VFS_OK) {
        VFS_ERROR("%s failed to lock inode %p!\n\r", __func__, node);
        return VFS_ERR_LOCK;
    }

    if (node->status != VFS_INODE_VALID) {
        vfs_unlock(node->lock);
        VFS_ERROR("%s node %p is invalid now!\n\r", __func__, node);
        goto ret;
    }

    if (INODE_IS_FS(node)) {
        if ((node->ops.i_fops->ioctl) != NULL) {
            ret = (node->ops.i_fops->ioctl)(f, cmd, arg);
        }
    } else {
        if ((node->ops.i_ops->ioctl) != NULL) {
            ret = (node->ops.i_ops->ioctl)(f, cmd, arg);
        }
    }

ret:
    if (vfs_unlock(node->lock) != VFS_OK) {
        VFS_ERROR("%s failed to unlock inode %p!\n\r", __func__, node);
        return VFS_ERR_LOCK;
    }

    return ret;
}

int32_t vfs_do_pollfd(int32_t fd, int32_t flag, vfs_poll_notify_t notify,
                      void *fds, void *arg)
{
    int32_t ret = VFS_ERR_NOSYS;

    vfs_file_t  *f;
    vfs_inode_t *node;

#if 0
#ifdef WITH_LWIP
    if ((fd >= FD_AOS_SOCKET_OFFSET) &&
        (fd <= (FD_AOS_EVENT_OFFSET + FD_AOS_NUM_EVENTS - 1))) {
        return lwip_poll(fd, flag, notify, fds, arg);
    }

    if ((fd >= FD_UDS_SOCKET_OFFSET) &&
        (fd < FD_UDS_SOCKET_OFFSET + FD_UDS_SOCKET_NUM)) {
        return uds_poll(fd, flag, notify, fds, arg);
    }
#endif
#endif
    f = vfs_file_get(fd);
    if (f == NULL) {
        return VFS_ERR_NOENT;
    }

    node = f->node;

    if (vfs_lock(node->lock) != VFS_OK) {
        VFS_ERROR("%s failed to lock inode %p!\n\r", __func__, node);
        return VFS_ERR_LOCK;
    }

    if (node->status != VFS_INODE_VALID) {
        vfs_unlock(node->lock);
        VFS_ERROR("%s node %p is invalid now!\n\r", __func__, node);
        return VFS_ERR_NOENT;
    }

    if (!INODE_IS_FS(node)) {
        if ((node->ops.i_ops->poll) != NULL) {
            ret = (node->ops.i_ops->poll)(f, flag, notify, fds, arg);
        }
    }

    if (vfs_unlock(node->lock) != VFS_OK) {
        VFS_ERROR("%s failed to unlock inode %p!\n\r", __func__, node);
        return VFS_ERR_LOCK;
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

    if (vfs_lock(node->lock) != VFS_OK) {
        VFS_ERROR("%s failed to lock inode %p!\n\r", __func__, node);
        return VFS_ERR_LOCK;
    }

    if (node->status != VFS_INODE_VALID) {
        vfs_unlock(node->lock);
        VFS_ERROR("%s node %p is invalid now!\n\r", __func__, node);
        return VFS_ERR_NOENT;
    }

    if (INODE_IS_FS(node)) {
        if ((node->ops.i_fops->lseek) != NULL) {
            ret = (node->ops.i_fops->lseek)(f, offset, whence);
        }
    } else {
        if ((node->ops.i_ops->lseek) != NULL) {
            ret = (node->ops.i_ops->lseek)(f, offset, whence);
        }
    }

    if (vfs_unlock(node->lock) != VFS_OK) {
        VFS_ERROR("%s failed to unlock inode %p!\n\r", __func__, node);
        return VFS_ERR_LOCK;
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

    if (vfs_lock(node->lock) != VFS_OK) {
        VFS_ERROR("%s failed to lock inode %p!\n\r", __func__, node);
        return VFS_ERR_LOCK;
    }

    if (node->status != VFS_INODE_VALID) {
        vfs_unlock(node->lock);
        VFS_ERROR("%s node %p is invalid now!\n\r", __func__, node);
        return VFS_ERR_NOENT;
    }

    if (INODE_IS_FS(node)) {
        if ((node->ops.i_fops->sync) != NULL) {
            ret = (node->ops.i_fops->sync)(f);
        }
    }

    if (vfs_unlock(node->lock) != VFS_OK) {
        VFS_ERROR("%s failed to unlock inode %p!\n\r", __func__, node);
        return VFS_ERR_LOCK;
    }

    return ret;
}

void vfs_allsync(void)
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
            vfs_sync(fd);
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

    if (vfs_lock(node->lock) != VFS_OK) {
        VFS_ERROR("%s failed to lock inode %p!\n\r", __func__, node);
        ret = VFS_ERR_LOCK;
        goto end;
    }

    if (node->status != VFS_INODE_VALID) {
        vfs_unlock(node->lock);
        VFS_ERROR("%s node %p is invalid now!\n\r", __func__, node);
        ret = VFS_ERR_NOENT;
        goto end;
    }

    if (INODE_IS_FS(node)) {
        if ((node->ops.i_fops->stat) != NULL) {
            ret = (node->ops.i_fops->stat)(f, path, st);
        }
    }
    #if 0
    else if (INODE_IS_CHAR(node) || INODE_IS_BLOCK(node)) {
        if ((node->ops.i_ops->stat) != NULL) {
            ret = (node->ops.i_ops->stat)(f, path, st);
        } else {
            ret = VFS_OK;

            if (INODE_IS_CHAR(node)) {
                st->st_mode &= ~S_IFMT;
                st->st_mode |= S_IFCHR;
            }
        }
    }
    #endif
    if (vfs_unlock(node->lock) != VFS_OK) {
        VFS_ERROR("%s failed to unlock inode %p!\n\r", __func__, node);
        ret = VFS_ERR_LOCK;
        goto end;
    }

end:
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

    if (vfs_lock(node->lock) != VFS_OK) {
        VFS_ERROR("%s failed to lock inode %p!\n\r", __func__, node);
        return VFS_ERR_LOCK;
    }

    if (node->status != VFS_INODE_VALID) {
        vfs_unlock(node->lock);
        VFS_ERROR("%s node %p is invalid now!\n\r", __func__, node);
        return VFS_ERR_NOENT;
    }

    if (INODE_IS_FS(node)) {
        if ((node->ops.i_fops->fstat) != NULL) {
            ret = (node->ops.i_fops->fstat)(f, st);
        }
    }

    if (vfs_unlock(node->lock) != VFS_OK) {
        VFS_ERROR("%s failed to unlock inode %p!\n\r", __func__, node);
        return VFS_ERR_LOCK;
    }

    return ret;
}

#ifdef AOS_PROCESS_SUPPORT
void *vfs_mmap(void *start, size_t len, int prot, int flags, int fd, off_t offset)
{
    void *ret = (void *)VFS_ERR_NOSYS;

    vfs_file_t  *f;
    vfs_inode_t *node;

    f = vfs_file_get(fd);
    if (f == NULL) {
        return NULL;
    }

    node = f->node;

    if (vfs_lock(node->lock) != VFS_OK) {
        VFS_ERROR("%s failed to lock inode %p!\n\r", __func__, node);
        return NULL;
    }

    if (node->status != VFS_INODE_VALID) {
        vfs_unlock(node->lock);
        VFS_ERROR("%s node %p is invalid now!\n\r", __func__, node);
        return NULL;
    }

    if (INODE_IS_CHAR(node) || INODE_IS_BLOCK(node)) {
        if ((node->ops.i_ops->mmap) != NULL) {
            ret = (node->ops.i_ops->mmap)(f, start, len, prot, flags, fd, offset);
        }
    }

    if (vfs_unlock(node->lock) != VFS_OK) {
        VFS_ERROR("%s failed to unlock inode %p!\n\r", __func__, node);
        return NULL;
    }

    return ret;
}
#endif

int32_t vfs_link(const char *oldpath, const char *newpath)
{
    int32_t ret = VFS_ERR_NOSYS;

    vfs_file_t  *f;
    vfs_inode_t *node;

    if ((oldpath == NULL)||(newpath == NULL)) {
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

    if (vfs_lock(node->lock) != VFS_OK) {
        VFS_ERROR("%s failed to lock inode %p!\n\r", __func__, node);
        ret = VFS_ERR_LOCK;
        goto end;
    }

    if (node->status != VFS_INODE_VALID) {
        vfs_unlock(node->lock);
        VFS_ERROR("%s node %p is invalid now!\n\r", __func__, node);
        ret = VFS_ERR_NOENT;
        goto end;
    }

    if (INODE_IS_FS(node)) {
        if ((node->ops.i_fops->link) != NULL) {
            ret = (node->ops.i_fops->link)(f, oldpath, newpath);
        }
    }

    if (vfs_unlock(node->lock) != VFS_OK) {
        VFS_ERROR("%s failed to unlock inode %p!\n\r", __func__, node);
        ret = VFS_ERR_LOCK;
        goto end;
    }

end:
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

    if (vfs_lock(node->lock) != VFS_OK) {
        VFS_ERROR("%s failed to lock inode %p!\n\r", __func__, node);
        ret = VFS_ERR_LOCK;
        goto end;
    }

    if (node->status != VFS_INODE_VALID) {
        vfs_unlock(node->lock);
        VFS_ERROR("%s node %p is invalid now!\n\r", __func__, node);
        ret = VFS_ERR_NOENT;
        goto end;
    }

    if (INODE_IS_FS(node)) {
        if ((node->ops.i_fops->unlink) != NULL) {
            ret = (node->ops.i_fops->unlink)(f, path);
        }
    }

    if (vfs_unlock(node->lock) != VFS_OK) {
        VFS_ERROR("%s failed to unlock inode %p!\n\r", __func__, node);
        ret = VFS_ERR_LOCK;
        goto end;
    }

end:
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

    if (vfs_lock(node->lock) != VFS_OK) {
        VFS_ERROR("%s failed to lock inode %p!\n\r", __func__, node);
        ret = VFS_ERR_LOCK;
        goto end;
    }

    if (node->status != VFS_INODE_VALID) {
        vfs_unlock(node->lock);
        VFS_ERROR("%s node %p is invalid now!\n\r", __func__, node);
        ret = VFS_ERR_NOENT;
        goto end;
    }

    if (INODE_IS_FS(node)) {
        if ((node->ops.i_fops->remove) != NULL) {
            ret = (node->ops.i_fops->remove)(f, path);
        }
    }

    if (vfs_unlock(node->lock) != VFS_OK) {
        VFS_ERROR("%s failed to unlock inode %p!\n\r", __func__, node);
        ret = VFS_ERR_LOCK;
        goto end;
    }

end:
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

    if (vfs_lock(node->lock) != VFS_OK) {
        VFS_ERROR("%s failed to lock inode %p!\n\r", __func__, node);
        ret = VFS_ERR_LOCK;
        goto end;
    }

    if (node->status != VFS_INODE_VALID) {
        vfs_unlock(node->lock);
        VFS_ERROR("%s node %p is invalid now!\n\r", __func__, node);
        ret = VFS_ERR_NOENT;
        goto end;
    }

    if (INODE_IS_FS(node)) {
        if ((node->ops.i_fops->rename) != NULL) {
            ret = (node->ops.i_fops->rename)(f, oldpath, newpath);
        }
    }

    if (vfs_unlock(node->lock) != VFS_OK) {
        VFS_ERROR("%s failed to unlock inode %p!\n\r", __func__, node);
        ret = VFS_ERR_LOCK;
        goto end;
    }

end:
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

    if (vfs_lock(node->lock) != VFS_OK) {
        VFS_ERROR("%s failed to lock inode %p!\n\r", __func__, node);
        goto end;
    }

    if (node->status != VFS_INODE_VALID) {
        vfs_unlock(node->lock);
        VFS_ERROR("%s node %p is invalid now!\n\r", __func__, node);
        goto end;
    }

    if (INODE_IS_FS(node)) {
        if ((node->ops.i_fops->opendir) != NULL) {
            dp = (node->ops.i_fops->opendir)(f, path);
        }
    }

    if (vfs_unlock(node->lock) != VFS_OK) {
        if (dp && (node->ops.i_fops->closedir) != NULL) {
            (node->ops.i_fops->closedir)(f, dp);
        }

        VFS_ERROR("%s failed to unlock inode %p!\n\r", __func__, node);
        dp = NULL;
        goto end;
    }

end:
    if (vfs_lock(g_vfs_lock_ptr) != VFS_OK) {
        return NULL;
    }

    if (dp == NULL) {
        vfs_file_del(f);

        vfs_unlock(g_vfs_lock_ptr);
        return NULL;
    }

    dp->dd_vfs_fd = vfs_fd_get(f);
    vfs_fd_mark_open(dp->dd_vfs_fd);

    vfs_unlock(g_vfs_lock_ptr);

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

    if (vfs_lock(node->lock) != VFS_OK) {
        VFS_ERROR("%s failed to lock inode %p!\n\r", __func__, node);
        return VFS_ERR_LOCK;
    }

    if (node->status != VFS_INODE_VALID) {
        vfs_unlock(node->lock);
        VFS_ERROR("%s node %p is invalid now!\n\r", __func__, node);
        return VFS_ERR_NOENT;
    }

    if (INODE_IS_FS(node)) {
        if ((node->ops.i_fops->closedir) != NULL) {
            ret = (node->ops.i_fops->closedir)(f, dir);
        }
    }

    if (vfs_unlock(node->lock) != VFS_OK) {
        VFS_ERROR("%s failed to unlock inode %p!\n\r", __func__, node);
        return VFS_ERR_LOCK;
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

    if (vfs_lock(node->lock) != VFS_OK) {
        VFS_ERROR("%s failed to lock inode %p!\n\r", __func__, node);
        return NULL;
    }

    if (node->status != VFS_INODE_VALID) {
        vfs_unlock(node->lock);
        VFS_ERROR("%s node %p is invalid now!\n\r", __func__, node);
        return NULL;
    }

    if (INODE_IS_FS(node)) {
        if ((node->ops.i_fops->readdir) != NULL) {
            dirent = (node->ops.i_fops->readdir)(f, dir);
        }
    }

    if (vfs_unlock(node->lock) != VFS_OK) {
        VFS_ERROR("%s failed to unlock inode %p!\n\r", __func__, node);
        return NULL;
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

    if (vfs_lock(node->lock) != VFS_OK) {
        VFS_ERROR("%s failed to lock inode %p!\n\r", __func__, node);
        ret = VFS_ERR_LOCK;
        goto end;
    }

    if (node->status != VFS_INODE_VALID) {
        vfs_unlock(node->lock);
        VFS_ERROR("%s node %p is invalid now!\n\r", __func__, node);
        ret = VFS_ERR_NOENT;
        goto end;
    }

    if (INODE_IS_FS(node)) {
        if ((node->ops.i_fops->mkdir) != NULL) {
            ret = (node->ops.i_fops->mkdir)(f, path);
        }
    }

    if (vfs_unlock(node->lock) != VFS_OK) {
        VFS_ERROR("%s failed to unlock inode %p!\n\r", __func__, node);
        ret = VFS_ERR_LOCK;
        goto end;
    }

end:
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

    if (vfs_lock(node->lock) != VFS_OK) {
        VFS_ERROR("%s failed to lock inode %p!\n\r", __func__, node);
        ret = VFS_ERR_LOCK;
        goto end;
    }

    if (node->status != VFS_INODE_VALID) {
        vfs_unlock(node->lock);
        VFS_ERROR("%s node %p is invalid now!\n\r", __func__, node);
        ret = VFS_ERR_NOENT;
        goto end;
    }

    if (INODE_IS_FS(node)) {
        if ((node->ops.i_fops->rmdir) != NULL) {
            ret = (node->ops.i_fops->rmdir)(f, path);
        }
    }

    if (vfs_unlock(node->lock) != VFS_OK) {
        VFS_ERROR("%s failed to unlock inode %p!\n\r", __func__, node);
        ret = VFS_ERR_LOCK;
        goto end;
    }

end:
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

    if (vfs_lock(node->lock) != VFS_OK) {
        VFS_ERROR("%s failed to lock inode %p!\n\r", __func__, node);
        return;
    }

    if (node->status != VFS_INODE_VALID) {
        vfs_unlock(node->lock);
        VFS_ERROR("%s node %p is invalid now!\n\r", __func__, node);
        return;
    }

    if (INODE_IS_FS(node)) {
        if ((node->ops.i_fops->rewinddir) != NULL) {
            (node->ops.i_fops->rewinddir)(f, dir);
        }
    }

    if (vfs_unlock(node->lock) != VFS_OK) {
        VFS_ERROR("%s failed to unlock inode %p!\n\r", __func__, node);
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

    if (vfs_lock(node->lock) != VFS_OK) {
        VFS_ERROR("%s failed to lock inode %p!\n\r", __func__, node);
        return VFS_ERR_LOCK;
    }

    if (node->status != VFS_INODE_VALID) {
        vfs_unlock(node->lock);
        VFS_ERROR("%s node %p is invalid now!\n\r", __func__, node);
        return VFS_ERR_NOENT;
    }

    if (INODE_IS_FS(node)) {
        if ((node->ops.i_fops->telldir) != NULL) {
            ret = (node->ops.i_fops->telldir)(f, dir);
        }
    }

    if (vfs_unlock(node->lock) != VFS_OK) {
        VFS_ERROR("%s failed to unlock inode %p!\n\r", __func__, node);
        return VFS_ERR_LOCK;
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

    if (vfs_lock(node->lock) != VFS_OK) {
        VFS_ERROR("%s failed to lock inode %p!\n\r", __func__, node);
        return;
    }

    if (node->status != VFS_INODE_VALID) {
        vfs_unlock(node->lock);
        VFS_ERROR("%s node %p is invalid now!\n\r", __func__, node);
        return;
    }

    if (INODE_IS_FS(node)) {
        if ((node->ops.i_fops->seekdir) != NULL) {
            (node->ops.i_fops->seekdir)(f, dir, loc);
        }
    }

    if (vfs_unlock(node->lock) != VFS_OK) {
        VFS_ERROR("%s failed to unlock inode %p!\n\r", __func__, node);
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

    if (vfs_lock(node->lock) != VFS_OK) {
        VFS_ERROR("%s failed to lock inode %p!\n\r", __func__, node);
        ret = VFS_ERR_LOCK;
        goto end;
    }

    if (node->status != VFS_INODE_VALID) {
        vfs_unlock(node->lock);
        VFS_ERROR("%s node %p is invalid now!\n\r", __func__, node);
        ret = VFS_ERR_NOENT;
        goto end;
    }

    if (INODE_IS_FS(node)) {
        if ((node->ops.i_fops->statfs) != NULL) {
            ret = (node->ops.i_fops->statfs)(f, path, buf);
        }
    }

    if (vfs_unlock(node->lock) != VFS_OK) {
        VFS_ERROR("%s failed to unlock inode %p!\n\r", __func__, node);
        ret = VFS_ERR_LOCK;
        goto end;
    }

end:
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

    if (vfs_lock(node->lock) != VFS_OK) {
        VFS_ERROR("%s failed to lock inode %p!\n\r", __func__, node);
        ret = VFS_ERR_LOCK;
        goto end;
    }

    if (node->status != VFS_INODE_VALID) {
        vfs_unlock(node->lock);
        VFS_ERROR("%s node %p is invalid now!\n\r", __func__, node);
        ret = VFS_ERR_NOENT;
        goto end;
    }

    if (INODE_IS_FS(node)) {
        if ((node->ops.i_fops->access) != NULL) {
            ret = (node->ops.i_fops->access)(f, path, amode);
        }
    } else {
        ret = VFS_OK; // always OK for devices
    }

    if (vfs_unlock(node->lock) != VFS_OK) {
        VFS_ERROR("%s failed to unlock inode %p!\n\r", __func__, node);
        ret = VFS_ERR_LOCK;
        goto end;
    }

end:
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
    if ((buf == NULL) || (size < strlen(g_current_working_directory))) {
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

    if (vfs_lock(node->lock) != VFS_OK) {
        VFS_ERROR("%s failed to lock inode %p!\n\r", __func__, node);
        ret = VFS_ERR_LOCK;
        goto end;
    }

    if (node->status != VFS_INODE_VALID) {
        vfs_unlock(node->lock);
        VFS_ERROR("%s node %p is invalid now!\n\r", __func__, node);
        ret = VFS_ERR_NOENT;
        goto end;
    }

    if (INODE_IS_FS(node)) {
        if ((node->ops.i_fops->pathconf) != NULL) {
            ret = (node->ops.i_fops->pathconf)(f, path, name);
        }
    }

    if (vfs_unlock(node->lock) != VFS_OK) {
        VFS_ERROR("%s failed to unlock inode %p!\n\r", __func__, node);
        ret = VFS_ERR_LOCK;
        goto end;
    }

end:
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

    if (vfs_lock(node->lock) != VFS_OK) {
        VFS_ERROR("%s failed to lock inode %p!\n\r", __func__, node);
        return VFS_ERR_LOCK;
    }

    if (node->status != VFS_INODE_VALID) {
        vfs_unlock(node->lock);
        VFS_ERROR("%s node %p is invalid now!\n\r", __func__, node);
        return VFS_ERR_NOENT;
    }

    if (INODE_IS_FS(node)) {
        if ((node->ops.i_fops->fpathconf) != NULL) {
            ret = (node->ops.i_fops->fpathconf)(file, name);
        }
    }

    if (vfs_unlock(node->lock) != VFS_OK) {
        VFS_ERROR("%s failed to unlock inode %p!\n\r", __func__, node);
        return VFS_ERR_LOCK;
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

    if (vfs_lock(node->lock) != VFS_OK) {
        VFS_ERROR("%s failed to lock inode %p!\n\r", __func__, node);
        ret = VFS_ERR_LOCK;
        goto end;
    }

    if (node->status != VFS_INODE_VALID) {
        vfs_unlock(node->lock);
        VFS_ERROR("%s node %p is invalid now!\n\r", __func__, node);
        ret = VFS_ERR_NOENT;
        goto end;
    }

    if (INODE_IS_FS(node)) {
        if ((node->ops.i_fops->utime) != NULL) {
            ret = (node->ops.i_fops->utime)(f, path, times);
        }
    }

    if (vfs_unlock(node->lock) != VFS_OK) {
        VFS_ERROR("%s failed to unlock inode %p!\n\r", __func__, node);
        ret = VFS_ERR_LOCK;
        goto end;
    }

end:
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

int vfs_fcntl(int fd, int cmd, int val)
{
    if (fd < 0) {
        return -EINVAL;
    }

    if (fd < vfs_fd_offset_get()) {
#ifdef IO_NEED_TRAP
        return trap_fcntl(fd, cmd, val);
#else
        return -ENOENT;
#endif
    }

    return 0;
}

int32_t vfs_register_driver(const char *path, vfs_file_ops_t *ops, void *arg)
{
    int32_t ret = VFS_ERR_NOSYS;

    vfs_inode_t *node = NULL;

    if (vfs_lock(g_vfs_lock_ptr) != VFS_OK) {
        return VFS_ERR_LOCK;
    }

    node = vfs_inode_open(path);
    if(NULL != node)
    {
        VFS_ERROR("%s failed to register, the path has exist %s, %d!\n\r", __func__, path, node->status);
        vfs_unlock(g_vfs_lock_ptr);
        return VFS_ERR_INVAL;
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

    node = vfs_inode_open(path);
    if(NULL != node)
    {
        VFS_ERROR("%s failed to register, the path has exist %s, %d!\n\r", __func__, path, node->status);
        vfs_unlock(g_vfs_lock_ptr);
        return VFS_ERR_INVAL;
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

int32_t vfs_get_node_name(const char *path, char names[][64], uint32_t* size)
{
    return vfs_inode_get_names(path, names, size);
}
int vfs_dup(int oldfd)
{
    int ret = VFS_ERR_GENERAL, realold = oldfd - VFS_FD_OFFSET;;
    vfs_file_t *f;

    if (vfs_lock(g_vfs_lock_ptr) != VFS_OK) {
        return VFS_ERR_LOCK;
    }

    if (realold < 0 || realold >= VFS_MAX_FILE_NUM || !vfs_fd_is_open(oldfd)) {
        vfs_unlock(g_vfs_lock_ptr);
        return VFS_ERR_GENERAL;
    }

    /* -1 as *node, just for NULL check, do NOT really use it! */
    f = vfs_file_new((vfs_inode_t *)(-1));
    if (f) {
        ret = vfs_fd_get(f);
        vfs_fd_mark_open(ret);
        f->redirect_fd = oldfd;
    } else {
        ret = VFS_ERR_GENERAL;
    }

    vfs_unlock(g_vfs_lock_ptr);

    return ret;
}

int vfs_dup2(int oldfd, int newfd)
{
    if (is_stdio_fd(newfd)) {
        return set_stdio_redirect_fd(newfd, oldfd) == 0 ? VFS_OK : VFS_ERR_GENERAL;
    } else {
        return set_normal_redirect_fd(oldfd, newfd) >= 0 ? newfd : VFS_ERR_GENERAL;
    }
}
