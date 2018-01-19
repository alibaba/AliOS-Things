/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include <aos/network.h>
#include <vfs_conf.h>
#include <vfs_err.h>
#include <vfs_inode.h>
#include <vfs.h>
#include <stdio.h>
#include <hal/hal.h>
#include <limits.h>
#include <string.h>

#if defined (__ICCARM__) || defined (__CC_ARM)
#include <sys/select.h>
#endif

#ifndef PATH_MAX
#define PATH_MAX 256
#endif

extern uart_dev_t uart_0;

static uint8_t    g_vfs_init;

aos_mutex_t g_vfs_mutex;

#ifdef IO_NEED_TRAP
static int trap_open(const char *path, int flags)
{
#ifdef WITH_LWIP
    return -ENOSYS;
#else
    int fd = open(path, flags);
    if (fd < 0) {
        fd = open(path, O_RDWR | O_CREAT, 0644);
    }
    return fd;
#endif
}

static int trap_read(int fd, void *buf, int len)
{
    return read(fd, buf, len);
}

static int trap_write(int fd, const void *buf, int len)
{
    return write(fd, buf, len);
}

static int trap_close(int fd)
{
    return close(fd);
}

static int trap_fcntl(int fd, int cmd, int val)
{
    return fcntl(fd, cmd, val);
}

#else
static int trap_open(const char *path, int flags)
{
    return -ENOSYS;
}

static int trap_read(int fd, void *buf, int len)
{
    return -ENOSYS;
}

static int trap_write(int fd, const void *buf, int len)
{
    return -ENOSYS;
}

static int trap_close(int fd)
{
    return -ENOSYS;
}

static int trap_fcntl(int fd, int cmd, int val)
{
    return -ENOSYS;
}
#endif


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

#define MAX_FILE_NUM (AOS_CONFIG_VFS_DEV_NODES * 2)
static file_t files[MAX_FILE_NUM];

static inline int get_fd(file_t *file)
{
    return (file - files) + AOS_CONFIG_VFS_FD_OFFSET;
}

static inline file_t *get_file(int fd)
{
    file_t *f;

    fd -= AOS_CONFIG_VFS_FD_OFFSET;

    if (fd < 0) {
        return NULL;
    }

    if (fd >= MAX_FILE_NUM) {
        return NULL;
    }

    f = &files[fd];
    return f->node ? f : NULL;
}

static file_t *new_file(inode_t *node)
{
    file_t *f;
    int idx;

    for (idx = 0; idx < MAX_FILE_NUM; idx++) {
        f = &files[idx];

        if (f->node == NULL) {
            goto got_file;
        }
    }

    return NULL;

got_file:
    f->node = node;
    f->f_arg = NULL;
    f->offset = 0;
    inode_ref(node);
    return f;
}

static void del_file(file_t *file)
{
    inode_unref(file->node);
    file->node = NULL;
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
        return trap_open(path, flags);
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
        return trap_close(fd);
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
        return trap_read(fd, buf, nbytes);
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
        return trap_write(fd, buf, nbytes);
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

#if (AOS_CONFIG_VFS_POLL_SUPPORT>0)

#if !defined(WITH_LWIP) && !defined(WITH_SAL)&& defined(VCALL_RHINO)
#define NEED_WAIT_IO
#endif

#include <aos/network.h>

#ifdef CONFIG_NO_TCPIP

struct poll_arg {
    aos_sem_t sem;
};

static void setup_fd(int fd)
{
}

static void teardown_fd(int fd)
{
}

static void vfs_poll_notify(struct pollfd *fd, void *arg)
{
    struct poll_arg *parg = arg;
    aos_sem_signal(&parg->sem);
}

static int wait_io(int maxfd, fd_set *rfds, struct poll_arg *parg, int timeout)
{
    timeout = timeout >= 0 ? timeout : AOS_WAIT_FOREVER;
    aos_sem_wait(&parg->sem, timeout);
    return 0;
}

static int init_parg(struct poll_arg *parg)
{
    aos_sem_new(&parg->sem,  0);
    return 0;
}

static void deinit_parg(struct poll_arg *parg)
{
    aos_sem_free(&parg->sem);
}
#elif defined(NEED_WAIT_IO)

#include <sys/syscall.h>
#define gettid() syscall(SYS_gettid)

struct poll_arg {
    aos_sem_t sem;
};

static void setup_fd(int fd)
{
    int f = fcntl(fd, F_GETFL) | O_ASYNC;
    if (fcntl(fd, F_SETFL, f) < 0) {
        perror("fcntl setup");
    }
    if (fcntl(fd, F_SETOWN, gettid()) < 0) {
        perror("fcntl setown");
    }
}

static void teardown_fd(int fd)
{
    int f = fcntl(fd, F_GETFL) & ~O_ASYNC;
    if (fcntl(fd, F_SETFL, f) < 0) {
        perror("fcntl teardown");
    }
}

static int wait_io(int maxfd, fd_set *rfds, struct poll_arg *parg, int timeout)
{
    struct timeval tv = { 0 };
    int ret;
    fd_set saved_fds = *rfds;

    /* check if already data available */
    ret = select(maxfd + 1, rfds, NULL, NULL, &tv);
    if (ret > 0) {
        return ret;
    }

    timeout = timeout >= 0 ? timeout : AOS_WAIT_FOREVER;
    ret = aos_sem_wait(&parg->sem, timeout);
    if (ret != VFS_SUCCESS) {
        return 0;
    }

    *rfds = saved_fds;
    ret = select(maxfd + 1, rfds, NULL, NULL, &tv);
    return ret;
}

static void vfs_poll_notify(struct pollfd *fd, void *arg)
{
    struct poll_arg *parg = arg;
    aos_sem_signal(&parg->sem);
}

static void vfs_io_cb(int fd, void *arg)
{
    struct poll_arg *parg = arg;
    aos_sem_signal(&parg->sem);
}

void cpu_io_register(void (*f)(int, void *), void *arg);
void cpu_io_unregister(void (*f)(int, void *), void *arg);
static int init_parg(struct poll_arg *parg)
{
    cpu_io_register(vfs_io_cb, parg);
    aos_sem_new(&parg->sem,  0);
    return 0;
}

static void deinit_parg(struct poll_arg *parg)
{
    aos_sem_free(&parg->sem);
    cpu_io_unregister(vfs_io_cb, parg);
}

#else
struct poll_arg {
    int efd;
};

static void vfs_poll_notify(struct pollfd *fd, void *arg)
{
    struct poll_arg *parg = arg;
    uint64_t val = 1;
    write(parg->efd, &val, sizeof val);
}

static void setup_fd(int fd)
{
}

static void teardown_fd(int fd)
{
}

static int init_parg(struct poll_arg *parg)
{
    int efd;
    efd = eventfd(0, 0);

    if (efd < 0) {
        return -1;
    }

    parg->efd = efd;

    return 0;
}

static void deinit_parg(struct poll_arg *parg)
{
    close(parg->efd);
}

static int wait_io(int maxfd, fd_set *rfds, struct poll_arg *parg, int timeout)
{
    struct timeval tv = {
        .tv_sec  = timeout / 1024,
        .tv_usec = (timeout % 1024) * 1024,
    };

    FD_SET(parg->efd, rfds);
    maxfd = parg->efd > maxfd ? parg->efd : maxfd;
    return select(maxfd + 1, rfds, NULL, NULL, timeout >= 0 ? &tv : NULL);
}
#endif

static int pre_poll(struct pollfd *fds, int nfds, fd_set *rfds, void *parg)
{
    int i;
    int maxfd = 0;

    for (i = 0; i < nfds; i++) {
        struct pollfd *pfd = &fds[i];

        pfd->revents = 0;
    }

    for (i = 0; i < nfds; i++) {
        file_t  *f;
        struct pollfd *pfd = &fds[i];

        if (pfd->fd < AOS_CONFIG_VFS_FD_OFFSET) {
            setup_fd(pfd->fd);
            FD_SET(pfd->fd, rfds);
            maxfd = pfd->fd > maxfd ? pfd->fd : maxfd;
            continue;
        }

        f = get_file(pfd->fd);

        if (f == NULL) {
            return -1;
        }

        pfd = &fds[i];
        (f->node->ops.i_ops->poll)(f, true, vfs_poll_notify, pfd, parg);
    }

    return maxfd;
}

static int post_poll(struct pollfd *fds, int nfds)
{
    int j;
    int ret = 0;

    for (j = 0; j < nfds; j++) {
        file_t  *f;
        struct pollfd *pfd = &fds[j];

        if (pfd->fd < AOS_CONFIG_VFS_FD_OFFSET) {
            teardown_fd(pfd->fd);
            continue;
        }


        f = get_file(pfd->fd);

        if (f == NULL) {
            continue;
        }

        (f->node->ops.i_ops->poll)(f, false, NULL, NULL, NULL);

        if (pfd->revents) {
            ret ++;
        }
    }

    return ret;
}

int aos_poll(struct pollfd *fds, int nfds, int timeout)
{
    fd_set rfds;

    int ret = VFS_SUCCESS;
    int nset = 0;
    struct poll_arg parg;

    if (init_parg(&parg) < 0) {
        return -1;
    }

    FD_ZERO(&rfds);
    ret = pre_poll(fds, nfds, &rfds, &parg);

    if (ret < 0) {
        goto check_poll;
    }

    ret = wait_io(ret, &rfds, &parg, timeout);

    if (ret >= 0) {
        int i;

        for (i = 0; i < nfds; i++) {
            struct pollfd *pfd = fds + i;

            if (FD_ISSET(pfd->fd, &rfds)) {
                pfd->revents |= POLLIN;
            }
        }

        nset += ret;
    }

check_poll:
    nset += post_poll(fds, nfds);

    deinit_parg(&parg);

    return ret < 0 ? 0 : nset;
}
AOS_EXPORT(int, aos_poll, struct pollfd *, int, int);
#endif

int aos_fcntl(int fd, int cmd, int val)
{
    if (fd < 0) {
        return -EINVAL;
    }

    if (fd < AOS_CONFIG_VFS_FD_OFFSET) {
        return trap_fcntl(fd, cmd, val);
    }

    return 0;
}
AOS_EXPORT(int, aos_fcntl, int, int, int);

int aos_ioctl_in_loop(int cmd, unsigned long arg)
{
    int      err;
    int      fd;

    for (fd = AOS_CONFIG_VFS_FD_OFFSET;
         fd < AOS_CONFIG_VFS_FD_OFFSET + AOS_CONFIG_VFS_DEV_NODES; fd++) {
        file_t  *f;
        inode_t *node;

        if ((err = aos_mutex_lock(&g_vfs_mutex, AOS_WAIT_FOREVER)) != 0) {
            return err;
        }

        f = get_file(fd);

        if (f == NULL) {
            aos_mutex_unlock(&g_vfs_mutex);
            return -ENOENT;
        }

        if ((err = aos_mutex_unlock(&g_vfs_mutex)) != 0) {
            return err;
        }

        node = f->node;

        if ((node->ops.i_ops->ioctl) != NULL) {
            err = (node->ops.i_ops->ioctl)(f, cmd, arg);

            if (err != VFS_SUCCESS) {
                return err;
            }
        }
    }

    return VFS_SUCCESS;
}

int32_t aos_uart_send(void *data, uint32_t size, uint32_t timeout)
{
    return hal_uart_send(&uart_0, data, size, timeout);
}
AOS_EXPORT(int32_t, aos_uart_send, void *, uint32_t, uint32_t);

