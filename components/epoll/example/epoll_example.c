/*
 * Copyright (C) 2020-2021 Alibaba Group Holding Limited
 */
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include "aos/vfs.h"
#include "epoll.h"
#if AOS_COMP_CLI
#include "aos/cli.h"
#endif

extern int epoll_vfs_test_device_init(void);
extern int epoll_vfs_test_device_deinit(void);

static void epoll_example()
{
    int ret;
    int test_fd = -1;
    int epoll_fd = -1;
    int val = 2;
    int read_val = 0;
    struct epoll_event event;
    struct epoll_event wait_event[2] = {0};

    test_fd = aos_open("/dev/test", O_RDWR);
    if(test_fd < 0) {
       printf("open test device err\r\n");
       return;
    }

    epoll_fd = epoll_create(0);
    if (epoll_fd < 0) {
        printf("epoll create failed\r\n");
        goto err;
    }

    event.events = EPOLLIN;
    event.data.fd = test_fd;
    ret = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, test_fd, &event);
    if (ret < 0) {
        printf("epoll_ctl:add fd failed\r\n");
        goto err;
    }
    ret = epoll_wait(epoll_fd, wait_event, 2, 1000);
    printf("epoll wait timeout:ret = %d\r\n", ret);

    aos_write(test_fd, &val, sizeof(val));

    ret = epoll_wait(epoll_fd, wait_event, 2, 1000);
    printf("epoll_wait,ret = %d\r\n", ret);
    printf("epoll_wait,get event, fd = %d, event = %d\r\n", wait_event[0].data.fd, wait_event[0].events);

    aos_read(test_fd, &read_val, sizeof(read_val));
    printf(" read fd =%d, data = %d \r\n", test_fd, read_val);
err:
    aos_close(test_fd);
    aos_close(epoll_fd);
}


static void epoll_comp_example(int argc, char **argv)
{
    /* init a test vfs device for poll and select */
    epoll_vfs_test_device_init();

    epoll_example();

    epoll_vfs_test_device_deinit();
    return;
}

#if AOS_COMP_CLI
/* reg args: fun, cmd, description*/
ALIOS_CLI_CMD_REGISTER(epoll_comp_example, epoll_example, epoll API base example)
#endif
