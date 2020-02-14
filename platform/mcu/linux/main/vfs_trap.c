/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

int trap_open(const char *path, int flags)
{
#ifdef AOS_CONFIG_TCPIP
    return -ENOSYS;
#else
    int fd = open(path, flags);
    if (fd < 0) {
        fd = open(path, O_RDWR | O_CREAT, 0644);
    }
    return fd;
#endif
}

int trap_read(int fd, void *buf, int len)
{
    return read(fd, buf, len);
}

int trap_write(int fd, const void *buf, int len)
{
    return write(fd, buf, len);
}

int trap_close(int fd)
{
    return close(fd);
}

int trap_fcntl(int fd, int cmd, int val)
{
    return fcntl(fd, cmd, val);
}

