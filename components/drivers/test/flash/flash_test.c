/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <epoll/epoll.h>
#include <poll.h>
#include <errno.h>

#include <string.h>
#include <sys/ioctl.h>
#include <aos/vfs.h>
#include <aos/hal/flash.h>
#include <hal2vfs/io_flash.h>
#include <drivers/char/u_device.h>
#include <drivers/u_ld.h>

#include "aos/cli.h"

int vfs_flash_test(unsigned int flash_id)
{
    char buffer[16];
    int ret = 0;
    int fd = -1;
    char dev_str[16];
#if 0
    int epfd = epoll_create(10 + 1);
#endif
    hal_logic_partition_t part_info;

    snprintf(dev_str,16-1,"/dev/flash%d", flash_id);

    fd = open(dev_str, 0);
    ddkc_info("open flash %s %s, fd:%d\r\n", dev_str, fd >= 0 ? "success" : "fail", fd);

    if (fd >= 0) {
#if 0
        int n = 0;
        int nfds = 0;
        struct epoll_event ev;
        struct epoll_event events[10 + 1];

        ev.data.fd = fd;
        ev.events = EPOLLIN | EPOLLOUT;
        ret = epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);
        ddkc_info("epoll_ctl return %d\r\n", ret);
#endif
        ret = lseek(fd,0,SEEK_SET);
        ddkc_info("lseek of flash return %d\r\n", ret);
        ret = write(fd, "test", strlen("test"));
        ddkc_info("write to flash return %d\r\n", ret);
        if (ret != strlen("test")) {
            ddkc_err("write to flash failed, ret:%d, target len:%d\r\n", ret, strlen("test"));
            goto err;
        }

        lseek(fd, 0, SEEK_SET);
        ret = read(fd, buffer, sizeof(buffer));
        ddkc_info("read from flash return %d\r\n", ret);
        if(ret != sizeof(buffer)){
            ddkc_err("read from flash failed, ret:%d, target size:%d\r\n", ret, sizeof(buffer));
            goto err;
        }
        buffer[15] = '\0';
        ret = strncmp("test", buffer, strlen("test"));
        if(ret){
            ddkc_err("flash read failed, buffer:%s\r\n", buffer);
            goto err;
        }
#if 0
        nfds = epoll_wait(epfd, events, 10, 1000);
        if (nfds > 0) {
            if (events[n].events & EPOLLIN) {
                ddkc_info("EPOLLIN received\r\n");
                ret = read(fd, buffer, sizeof(buffer));
                ddkc_info("read from ttyUART return %d\r\n", ret);

            } else if (events[n].events & EPOLLOUT) {
                ddkc_info("EPOLLOUT received\r\n");
            }
        }
#endif
        ddkc_info("ioctl with IOC_FLASH_INFO_GET\r\n");
        ioctl(fd, IOC_FLASH_INFO_GET, (unsigned long)&part_info);
        ddkc_info("ioctl on flash get partition info, description:%s\r\n", part_info.partition_description ? part_info.partition_description : "NULL");
        ddkc_info("ioctl on flash get partition info, start_addr:0x%x\r\n", part_info.partition_start_addr);
        ddkc_info("ioctl on flash get partition info, length:0x%x\r\n", part_info.partition_length);
        ddkc_info("ioctl with IOC_FLASH_GET_BOOT_TYPE\r\n");
        ret = ioctl(fd, IOC_FLASH_GET_BOOT_TYPE, 0);
        ddkc_info("ioctl with IOC_FLASH_GET_BOOT_TYPE ret:%d\r\n", ret);


        close(fd);

        return 0;
    } else {
        ddkc_err("flash open failed, buffer:%s\r\n", buffer);
        ret = -EIO;
    }

err:
    if (fd >= 0)
        close(fd);

    return ret;
}

void flash_unit_test(char *buf, int len, int argc, char **argv) {
    int ret = 0;
    unsigned int loops = 1;
    unsigned int flash_id = HAL_PARTITION_OTA_TEMP;

    if (argc <= 1) {
        ddkc_err("usage: flashut <loops>\r\n");
        return;
    }

    flash_id = argc > 1 ? atoi(argv[1]) : HAL_PARTITION_OTA_TEMP;
    loops = argc > 2 ? atoi(argv[2]) : 1;
    ddkc_info("flash_id:%d, loops:%d\r\n", flash_id, loops);

    while (loops--) {
        ret = vfs_flash_test(flash_id);
        if (ret) {
            ddkc_err("vfs_flash_test return %d\r\n", ret);
        }
    }

    return;
}

static struct cli_command flash_test_cmd[] = {
    { "flashut","flash unit test", flash_unit_test },
};

int flash_vfs_test(void) {

#ifdef AOS_COMP_CLI
    aos_cli_register_commands(&flash_test_cmd[0], sizeof(flash_test_cmd) / sizeof(struct cli_command));
#endif

    ddkc_info("%s: starts\n", __func__);

    return 0;
}

POST_DRIVER_ENTRY(flash_vfs_test)
