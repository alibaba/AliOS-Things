#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <aos/kernel.h>
#include <aos/vfs.h>
#include <epoll.h>
#include "private/epoll_inner.h"

int aos_epoll_create(int size)
{
    if (size < 0) {
        errno = EINVAL;
        return -1;
    }
    return aos_epoll_create1(0);
}

int aos_epoll_create1(int flag)
{
    int ret = -1;

    if (flag < 0) {
        errno = EINVAL;
        return ret;
    }

    ret = vfs_device_epoll_init();
    if (ret < 0) {
        printf("vfs epoll init failed ,%d\r\n", ret);
        if (ret < -1) {
            errno = -ret;
        }
        return -1;
    }

#ifdef _WIN32
    ret = aos_open("C:\\epoll.bin", flag == EPOLL_CLOEXEC ? O_CLOEXEC : 0);
#else
    ret = aos_open("/dev/epoll", flag == EPOLL_CLOEXEC ? O_CLOEXEC : 0);
#endif
    if (ret < 0) {
        if (ret < -1) {
            errno = -ret;
        }
        return -1;
    }
    return ret;
}

int aos_epoll_ctl(int epid, int op, int fd, struct epoll_event *event)
{

    epoll_ctl_data_t data;
    int ret = -1;

    if (epid < 0 || fd < 0) {
        errno = EINVAL;
        return -1;
    }
    memset(&data, 0, sizeof(epoll_ctl_data_t));
    data.op = op;
    data.fd = fd;
    data.event = event;

    ret = aos_ioctl(epid, EPOLL_VFS_CTL, (uint32_t)&data);
    if (ret < 0) {
        if (ret < -1) {
            errno = -ret;
        }
        return -1;
    }
    return ret;
}


int aos_epoll_wait(int epid, struct epoll_event *events, int maxevents, int timeout)
{
    int ret = -1;
    epoll_wait_data_t data;
    if (events == NULL || epid < 0 || maxevents <= 0) {
        errno = EINVAL;
        return -1;
    }
    memset(&data, 0, sizeof(epoll_wait_data_t));
    data.events = events;
    data.maxevents = maxevents;
    data.timeout = timeout;
    ret = aos_ioctl(epid, EPOLL_VFS_WAIT, (uint32_t)&data);
    if (ret < 0) {
        if (ret < -1) {
            errno = -ret;
        }
        return -1;
    }
    return ret;
}
