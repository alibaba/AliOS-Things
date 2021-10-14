/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <reent.h>
#include <sys/errno.h>
#include <sys/unistd.h>
#include <sys/time.h>
#include <stdarg.h>

#include <k_api.h>
#include "aos/kernel.h"
#include "aos/vfs.h"

#include "aos/hal/uart.h"

#include "vfs_conf.h"

#include "sys/socket.h"
#ifdef CONFIG_AOS_LWIP
#ifdef TELNETD_ENABLED
#include "lwip/apps/telnetserver.h"
#endif
#endif

#define FD_VFS_START VFS_FD_OFFSET
#define FD_VFS_END   (FD_VFS_START + VFS_MAX_FILE_NUM - 1)

#ifdef POSIX_DEVICE_IO_NEED
#ifdef CONFIG_AOS_LWIP
#define FD_SOCKET_START FD_AOS_SOCKET_OFFSET
#define FD_SOCKET_END   (FD_AOS_SOCKET_OFFSET + FD_AOS_NUM_SOCKETS - 1)
#define FD_EVENT_START  FD_AOS_EVENT_OFFSET
#define FD_EVENT_END    (FD_AOS_EVENT_OFFSET + FD_AOS_NUM_EVENTS - 1)
#endif
#endif

#define LIBC_CHECK_AOS_RET(ret) do {if ((ret) < 0) {ptr->_errno = -(ret); return -1; } } while (0)

int _execve_r(struct _reent *ptr, const char *name, char *const *argv,
              char *const *env)
{
    ptr->_errno = ENOSYS;
    return -1;
}

int _fcntl_r(struct _reent *ptr, int fd, int cmd, int arg)
{
    int ret;

    if ((fd >= FD_VFS_START) && (fd <= FD_VFS_END)) {
        ret = aos_fcntl(fd, cmd, arg);
        LIBC_CHECK_AOS_RET(ret);
        return ret;
#ifdef POSIX_DEVICE_IO_NEED
#ifdef CONFIG_AOS_LWIP
    } else if ((fd >= FD_SOCKET_START) && (fd <= FD_EVENT_END)) {
        return lwip_fcntl(fd, cmd, arg);
#endif
#endif
    } else {
        ptr->_errno = EBADF;
        return -1;
    }
}

int _fork_r(struct _reent *ptr)
{
    ptr->_errno = ENOSYS;
    return -1;
}

int _getpid_r(struct _reent *ptr)
{
    ptr->_errno = ENOSYS;
    return 0;
}

int _isatty_r(struct _reent *ptr, int fd)
{
    if (fd >= 0 && fd < 3) {
        return 1;
    }

    ptr->_errno = ENOTTY;
    return 0;
}

int _kill_r(struct _reent *ptr, int pid, int sig)
{
    ptr->_errno = ENOSYS;
    return -1;
}

_off_t _lseek_r(struct _reent *ptr, int fd, _off_t pos, int whence)
{
    int ret = aos_lseek(fd, pos, whence);
    LIBC_CHECK_AOS_RET(ret);
    return ret;
}

int _mkdir_r(struct _reent *ptr, const char *name, int mode)
{
    int ret = aos_mkdir(name);
    LIBC_CHECK_AOS_RET(ret);
    return ret;
}

int _open_r(struct _reent *ptr, const char *file, int flags, int mode)
{
    int ret = aos_open(file, flags);
    LIBC_CHECK_AOS_RET(ret);
    return ret;
}

int _close_r(struct _reent *ptr, int fd)
{
    int ret;

    if ((fd >= FD_VFS_START) && (fd <= FD_VFS_END)) {
        ret = aos_close(fd);
        LIBC_CHECK_AOS_RET(ret);
        return ret;
#ifdef POSIX_DEVICE_IO_NEED
#ifdef CONFIG_AOS_LWIP
    } else if ((fd >= FD_SOCKET_START) && (fd <= FD_EVENT_END)) {
        return lwip_close(fd);
#endif
#endif
    } else {
        ptr->_errno = EBADF;
        return -1;
    }
}

_ssize_t _read_r(struct _reent *ptr, int fd, void *buf, size_t nbytes)
{
    int ret;

    if ((fd >= FD_VFS_START) && (fd <= FD_VFS_END)) {
        ret = aos_read(fd, buf, nbytes);
        LIBC_CHECK_AOS_RET(ret);
        return ret;
#ifdef POSIX_DEVICE_IO_NEED
#ifdef CONFIG_AOS_LWIP
    } else if ((fd >= FD_SOCKET_START) && (fd <= FD_EVENT_END)) {
        return lwip_read(fd, buf, nbytes);
#endif
#endif
    } else {
        ptr->_errno = EBADF;
        return -1;
    }
}

_ssize_t _write_r(struct _reent *ptr, int fd, const void *buf, size_t nbytes)
{
    int ret;
    const char *tmp = buf;
    int         i   = 0;
    uart_dev_t  uart_stdio;

    if (buf == NULL) {
        ptr->_errno = EINVAL;
        return 0;
    }

    memset(&uart_stdio, 0, sizeof(uart_stdio));
    uart_stdio.port = HAL_UART_STDIO_PORT;

    if ((fd >= FD_VFS_START) && (fd <= FD_VFS_END)) {
        ret = aos_write(fd, buf, nbytes);
        LIBC_CHECK_AOS_RET(ret);
        return ret;
#ifdef POSIX_DEVICE_IO_NEED
#ifdef CONFIG_AOS_LWIP
    } else if ((fd >= FD_SOCKET_START) && (fd <= FD_EVENT_END)) {
        return lwip_write(fd, buf, nbytes);
#endif
#endif
    } else if ((fd == STDOUT_FILENO) || (fd == STDERR_FILENO)) {
        for (i = 0; i < nbytes; i++) {
            if (*tmp == '\n') {
#ifdef TELNETD_ENABLED
                TelnetWrite('\r');
#endif
                hal_uart_send(&uart_stdio, (void *)"\r", 1, AOS_WAIT_FOREVER);
            }

#ifdef TELNETD_ENABLED
            TelnetWrite(*tmp);
#endif
            hal_uart_send(&uart_stdio, (void *)tmp, 1, AOS_WAIT_FOREVER);
            tmp++;
        }

        return nbytes;
    } else {
        ptr->_errno = EBADF;
        return -1;
    }
}

int ioctl(int fildes, int request, ... /* arg */)
{
    long    arg  = 0;
    void   *argp = NULL;
    va_list args;
    int ret;

    va_start(args, request);

    if ((fildes >= VFS_FD_OFFSET) &&
        (fildes <= (VFS_FD_OFFSET + VFS_MAX_FILE_NUM - 1))) {
        arg = va_arg(args, int);
        ret = aos_ioctl(fildes, request, arg);
        va_end(args);
        return ret;
#ifdef POSIX_DEVICE_IO_NEED
#ifdef CONFIG_AOS_LWIP
    } else if ((fildes >= FD_AOS_SOCKET_OFFSET) &&
               (fildes <= (FD_AOS_EVENT_OFFSET + FD_AOS_NUM_EVENTS - 1))) {
        argp = va_arg(args, void *);
        ret = lwip_ioctl(fildes, request, argp);
        va_end(args);
        return ret;
#endif
#endif
    } else {
        va_end(args);
        return -1;
    }
}

int _rename_r(struct _reent *ptr, const char *oldname, const char *newname)
{
    int ret = aos_rename(oldname, newname);
    LIBC_CHECK_AOS_RET(ret);
    return ret;
}

void *_sbrk_r(struct _reent *ptr, ptrdiff_t incr)
{
    ptr->_errno = ENOSYS;
    return NULL;
}

int _stat_r(struct _reent *ptr, const char *file, struct stat *pstat)
{
    int ret;
    struct aos_stat stat;

    if ((file == NULL) || (pstat == NULL)) {
        ptr->_errno = EINVAL;
        return -1;
    }

    ret = aos_stat(file, &stat);
    LIBC_CHECK_AOS_RET(ret);

    pstat->st_mode  = stat.st_mode;
    pstat->st_size  = stat.st_size;
    pstat->st_atime = stat.st_actime;
    pstat->st_mtime = stat.st_modtime;

    return ret;
}

int _fstat_r(struct _reent *ptr, int fd, struct stat *buf)
{
    int ret;
    struct aos_stat stat_temp;

    if ((fd < 0) || (buf == NULL)) {
        ptr->_errno = EINVAL;
        return -1;
    }

    ret = aos_fstat(fd, &stat_temp);
    LIBC_CHECK_AOS_RET(ret);

    buf->st_mode = stat_temp.st_mode;
    buf->st_size = stat_temp.st_size;
    buf->st_atime = stat_temp.st_actime;
    buf->st_mtime = stat_temp.st_modtime;

    return ret;
}

_CLOCK_T_ _times_r(struct _reent *ptr, struct tms *ptms)
{
    ptr->_errno = ENOSYS;
    return -1;
}

int _link_r(struct _reent *ptr, const char *oldpath, const char *newpath)
{
    int ret = aos_link(oldpath, newpath);
    LIBC_CHECK_AOS_RET(ret);
    return ret;
}

int _unlink_r(struct _reent *ptr, const char *file)
{
    int ret = aos_unlink(file);
    LIBC_CHECK_AOS_RET(ret);
    return ret;
}

int _wait_r(struct _reent *ptr, int *status)
{
    ptr->_errno = ENOSYS;
    return -1;
}

int _gettimeofday_r(struct _reent *ptr, struct timeval *tv, void *__tzp)
{
    uint64_t t;
    struct timezone *tz = __tzp;

    if (tv) {
        t = aos_calendar_time_get();
        tv->tv_sec  = t / 1000;
        tv->tv_usec = (t % 1000) * 1000;
    }

    if (tz) {
        /* Not supported. */
        tz->tz_minuteswest = 0;
        tz->tz_dsttime = 0;
    }

    return 0;
}

long timezone = 0; /* global variable */

struct tm* localtime_r(const time_t* t, struct tm* r)
{
    time_t time_tmp;
    time_tmp = *t + timezone;
    return gmtime_r(&time_tmp, r);
}

struct tm* localtime(const time_t* t)
{
    struct tm* timeinfo;
    static struct tm tm_tmp;

    timeinfo = localtime_r(t, &tm_tmp);

    return timeinfo;
}

void *_malloc_r(struct _reent *ptr, size_t size)
{
    void *mem;

#if (RHINO_CONFIG_MM_DEBUG > 0u)
    mem = aos_malloc(size | AOS_UNSIGNED_INT_MSB);
    aos_alloc_trace(mem, (size_t)__builtin_return_address(0));
#else
    mem = aos_malloc(size);
#endif

    return mem;
}

void *_realloc_r(struct _reent *ptr, void *old, size_t newlen)
{
    void *mem;

#if (RHINO_CONFIG_MM_DEBUG > 0u)
    mem = aos_realloc(old, newlen | AOS_UNSIGNED_INT_MSB);
    aos_alloc_trace(mem, (size_t)__builtin_return_address(0));
#else
    mem = aos_realloc(old, newlen);
#endif

    return mem;
}

void *_calloc_r(struct _reent *ptr, size_t size, size_t len)
{
    void *mem;

#if (RHINO_CONFIG_MM_DEBUG > 0u)
    mem = aos_malloc((size * len) | AOS_UNSIGNED_INT_MSB);
    aos_alloc_trace(mem, (size_t)__builtin_return_address(0));
#else
    mem = aos_malloc(size * len);
#endif

    if (mem) {
        bzero(mem, size * len);
    }

    return mem;
}

void _free_r(struct _reent *ptr, void *addr)
{
    aos_free(addr);
}

void _exit(int status)
{
    while (1)
        ;
}

void exit(int status)
{
    aos_task_exit(status);
    /* This function declares the noreturn attribute, and execution should not return */
    while (1)
        ;
}

__attribute__((weak)) void _fini()
{
    return;
}

void _system(const char *s)
{
    return;
}

void abort(void)
{
    k_err_proc(RHINO_SYS_FATAL_ERR);
    /* This function declares the noreturn attribute, and execution should not return */
    while (1)
        ;
}

struct _reent *__getreent(void)
{
#if (RHINO_CONFIG_NEWLIBC_REENT > 0)
    CPSR_ALLOC();

    ktask_t *cur_task = NULL;

    RHINO_CRITICAL_ENTER();
    /* If in the interrupt context, use the global reent structure */
    if (g_intrpt_nested_level[cpu_cur_get()] > 0u) {
        RHINO_CRITICAL_EXIT();
        goto ret_impure_ptr;
    }
    RHINO_CRITICAL_EXIT();

    cur_task = krhino_cur_task_get();
    if (cur_task == NULL)
        goto ret_impure_ptr;

    if (cur_task->newlibc_reent == NULL) {
        cur_task->newlibc_reent = (struct _reent *)krhino_mm_alloc(sizeof(struct _reent));
        if (cur_task->newlibc_reent == NULL)
            goto ret_impure_ptr;
        _REENT_INIT_PTR(cur_task->newlibc_reent);
    }
    return cur_task->newlibc_reent;
#endif

ret_impure_ptr:
    if (_impure_ptr->__sdidinit == 0) {
        __sinit(_impure_ptr);
    }
    return _impure_ptr;
}
