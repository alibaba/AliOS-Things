/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#ifdef _WIN32
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#endif
#define TAG "vfsTrap"
int trap_open(const char *path, int flags)
{
#ifdef CONFIG_AOS_LWIP
    return -ENOSYS;
#else
    int fd = _open(path, flags);
    if (fd < 0) {
        fd = _open(path, O_RDWR | O_CREAT, 0644);
    }
    return fd;
#endif
}

int trap_read(int fd, void *buf, int len)
{
    return _read(fd, buf, len);
}

int trap_write(int fd, const void *buf, int len)
{
    return _write(fd, buf, len);
}

int trap_close(int fd)
{
    return _close(fd);
}

int trap_fcntl(int fd, int cmd, int val)
{
    if(F_GETFL==cmd)
    {
        return 0;
    }
    else if(F_SETFL==cmd)
    {
        return ioctlsocket(fd, cmd, &val);
    }
    else
    {
        LOGE(TAG,"Unsupport mode %d",cmd);
    }
    return -1;
}

