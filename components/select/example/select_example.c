/*
 * Copyright (C) 2020-2021 Alibaba Group Holding Limited
 */
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include "select.h"
#include "aos/vfs.h"
#if AOS_COMP_CLI
#include "aos/cli.h"
#endif

extern int vfs_test_device_init(void);
extern int vfs_test_device_deinit(void);

static void select_example()
{
    int ret;
    int test_fd;
    int maxfd;
    int write_val = 1;
    fd_set readset;
    fd_set writeset;
    fd_set exceptset;

    struct timeval tv = {
        .tv_sec  = 2,
        .tv_usec = 0,
    };

    FD_ZERO(&readset);
    FD_ZERO(&writeset);
    FD_ZERO(&exceptset);

    test_fd = aos_open("/dev/test", O_RDWR);
    if(test_fd < 0) {
       printf("open test device err\r\n");
       return;
    }

    FD_SET(test_fd, &readset);
    maxfd = test_fd + 1;

    ret = aos_select(maxfd, &readset, &writeset, &exceptset, &tv);
    printf("aos_select timeout:ret = %d\r\n", ret);
    aos_write(test_fd, &write_val, sizeof(write_val));
    FD_SET(test_fd, &readset);
    ret = aos_select(maxfd, &readset, &writeset, &exceptset, &tv);
    printf("aos_select ret = %d\r\n", ret);
    if (FD_ISSET(test_fd, &readset)) {
        int data = 0;
        int len = aos_read(test_fd, &data, sizeof(data));
        if(len > 0) {
            printf(" read fd =%d,data = %d\r\n", test_fd, data);
        } else {
            printf("err: read no data\r\n");
        }
    }
    aos_close(test_fd);
}

static void poll_example()
{
    int ret;
    int test_fd;
    int write_val = 1;
    struct pollfd poll_array[2] = {0};

    test_fd = aos_open("/dev/test", O_RDWR);
    if(test_fd < 0) {
       printf("open test device err\r\n");
       return;
    }

    poll_array[0].events = POLLIN;
    poll_array[0].fd = test_fd;


    ret = aos_poll(poll_array, 2, 1000);

    printf("aos_poll timeout, ret = %d \r\n", ret);

    /* test write event1 */
    ret = aos_write(test_fd, &write_val, sizeof(write_val));
    ret = aos_poll(poll_array, 2, 1000);
    printf("aos_poll ret = %d \r\n", ret);
    if (poll_array[0].revents & POLLIN) {
        int data = 0;
        int len = aos_read(test_fd, &data, sizeof(data));
        if(len > 0) {
            printf(" read fd =%d,data = %d\r\n", test_fd, data);
        } else {
            printf("err: read no data\r\n");
        }
    }
    aos_close(test_fd);
}


static void select_comp_example(int argc, char **argv)
{
    /* init a test vfs device for poll and select */
    vfs_test_device_init();

    /* select test */
    printf("+++++++select example+++++++++\r\n");
    select_example();

    /* poll test */
    printf("+++++++poll example+++++++++\r\n");
    poll_example();

    vfs_test_device_deinit();
    return;
}

#if AOS_COMP_CLI
/* reg args: fun, cmd, description*/
ALIOS_CLI_CMD_REGISTER(select_comp_example, select_example, select and poll API base example)
#endif
