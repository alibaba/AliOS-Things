/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <reent.h>
#include <sys/errno.h>
#include <sys/unistd.h>
#include <stdarg.h>
#include <k_api.h>
#include "aos/kernel.h"
#include <aos/hal/uart.h>

#include <umm.h>

int _execve_r(struct _reent *ptr, const char *name, char *const *argv,
              char *const *env)
{
    ptr->_errno = ENOTSUP;
    return -1;
}

int _fcntl_r(struct _reent *ptr, int fd, int cmd, int arg)
{
    return -1;
}

int _fork_r(struct _reent *ptr)
{
    ptr->_errno = ENOTSUP;
    return -1;
}

int _getpid_r(struct _reent *ptr)
{
    ptr->_errno = ENOTSUP;
    return 0;
}

int _isatty_r(struct _reent *ptr, int fd)
{
    if (fd >= 0 && fd < 3) {
        return 1;
    }

    ptr->_errno = ENOTSUP;
    return -1;
}

int _kill_r(struct _reent *ptr, int pid, int sig)
{
    ptr->_errno = ENOTSUP;
    return -1;
}

int _link_r(struct _reent *ptr, const char *old, const char *new)
{
    ptr->_errno = ENOTSUP;
    return -1;
}

_off_t _lseek_r(struct _reent *ptr, int fd, _off_t pos, int whence)
{
    return -1;
}

int _mkdir_r(struct _reent *ptr, const char *name, int mode)
{
    return -1;
}

int _open_r(struct _reent *ptr, const char *file, int flags, int mode)
{
    return -1;
}

int _close_r(struct _reent *ptr, int fd)
{
     return -1;
}

_ssize_t _read_r(struct _reent *ptr, int fd, void *buf, size_t nbytes)
{
    return -1;
}

_ssize_t _write_r(struct _reent *ptr, int fd, const void *buf, size_t nbytes)
{
    const char *tmp = buf;
    int         i   = 0;
    uart_dev_t  uart_stdio;

    memset(&uart_stdio, 0, sizeof(uart_stdio));
    uart_stdio.port = 0;

    hal_uart_send(&uart_stdio, buf, nbytes, AOS_WAIT_FOREVER);

    return nbytes;
}

int _fstat_r(struct _reent *ptr, int fd, struct stat *pstat)
{
    ptr->_errno = ENOTSUP;
    return -1;
}

int _rename_r(struct _reent *ptr, const char *old, const char *new)
{
    ptr->_errno = ENOTSUP;
    return 0;
}

void *_sbrk_r(struct _reent *ptr, ptrdiff_t incr)
{
    ptr->_errno = ENOTSUP;
    return NULL;
}

int _stat_r(struct _reent *ptr, const char *file, struct stat *pstat)
{
    return aos_stat(file, pstat);
}

_CLOCK_T_ _times_r(struct _reent *ptr, struct tms *ptms)
{
    ptr->_errno = ENOTSUP;
    return -1;
}

int _unlink_r(struct _reent *ptr, const char *file)
{
    return aos_unlink(file);
}

int _wait_r(struct _reent *ptr, int *status)
{
    ptr->_errno = ENOTSUP;
    return -1;
}

void *_malloc_r(struct _reent *ptr, size_t size)
{
    void *mem;

    mem = umm_alloc(size);

    return mem;
}

void *_realloc_r(struct _reent *ptr, void *old, size_t newlen)
{
    void *mem;

    mem = umm_realloc(old, newlen);

    return mem;
}

void *_calloc_r(struct _reent *ptr, size_t size, size_t len)
{
    void *mem;

    mem = umm_alloc(size * len);
    if (mem) {
        bzero(mem, size * len);
    }

    return mem;
}

void _free_r(struct _reent *ptr, void *addr)
{
    umm_free(addr);
}

void _exit(int status)
{
    while (1)
        ;
}

void _system(const char *s)
{
    return;
}

void abort(void)
{
    while (1)
        ;
}
