/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <reent.h>
#include <sys/errno.h>
#include <sys/unistd.h>
#include <sys/time.h>
#include <stdarg.h>
#include <csi_config.h>
#ifndef CONFIG_KERNEL_NONE
#include <csi_kernel.h>
#endif

#include <drv_usart.h>
#include <k_api.h>

extern usart_handle_t console_handle;

int _execve_r(struct _reent *ptr, const char *name, char *const *argv,
              char *const *env)
{
    ptr->_errno = ENOTSUP;
    return -1;
}

int _fcntl_r(struct _reent *ptr, int fd, int cmd, int arg)
{
	ptr->_errno = ENOTSUP;
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
    ptr->_errno = ENOTSUP;
    return -1;
}

int _mkdir_r(struct _reent *ptr, const char *name, int mode)
{
    ptr->_errno = ENOTSUP;
    return -1;
}

int _open_r(struct _reent *ptr, const char *file, int flags, int mode)
{
    ptr->_errno = ENOTSUP;
    return -1;
}

int _close_r(struct _reent *ptr, int fd)
{
    ptr->_errno = ENOTSUP;
    return -1;
}

_ssize_t _read_r(struct _reent *ptr, int fd, void *buf, size_t nbytes)
{
    ptr->_errno = ENOTSUP;
    return -1;
}

/*
 * implement _write_r here
 */
_ssize_t _write_r(struct _reent *ptr, int fd, const void *buf, size_t nbytes)
{
    const char *tmp = buf;
    int         i   = 0;

    if (buf == NULL) {
        return 0;
    }
    if ((fd == STDOUT_FILENO) || (fd == STDERR_FILENO)) {
        for (i = 0; i < nbytes; i++) {
            if (*tmp == '\n') {
                csi_usart_putchar(console_handle, '\r');
            }
			csi_usart_putchar(console_handle, *tmp);
            tmp++;
        }

        return nbytes;
    } else {
        return -1;
    }
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
    ptr->_errno = ENOTSUP;
    return 0;
}

_CLOCK_T_ _times_r(struct _reent *ptr, struct tms *ptms)
{
    ptr->_errno = ENOTSUP;
    return -1;
}

int _unlink_r(struct _reent *ptr, const char *file)
{
    ptr->_errno = ENOTSUP;
    return 0;
}

int _wait_r(struct _reent *ptr, int *status)
{
    ptr->_errno = ENOTSUP;
    return -1;
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

