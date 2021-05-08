/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */
#include <poll.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <epoll.h>
#include "aos/vfs.h"
#include <vfsdev/spi_dev.h>
#include <drivers/char/u_device.h>
#include <drivers/u_ld.h>
#if AOS_COMP_CLI
#include "aos/cli.h"
#endif

int vfs_spi_test(int id)
{
    int i = 0;
    char buffer[16];
    int ret = 0;
    int fd = 0;
    int epfd = epoll_create(10 + 1);
    char name[16] = {0};

    snprintf(name, sizeof(name), "/dev/spi%d", id);
    fd = open(name, 0);
    ddkc_info("open %s %s, fd:%d\r\n", name, fd >= 0 ? "success" : "fail", fd);

    if (fd >= 0) {
        int n = 0;
        int nfds = 0;
        struct epoll_event ev;
        struct epoll_event events[10 + 1];

        ev.data.fd = fd;
        ev.events = EPOLLIN | EPOLLOUT;
        ret = epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);
        ddkc_info("epoll_ctl return %d\r\n", ret);

        //ret = ioctl(fd, IOC_SPI_SET_CFLAG, SPI_MODE_0 | SPI_MSB | SPI_MASTER | SPI_TRANSFER_DMA_MODE | SPI_DATA_8BIT);
        ret = ioctl(fd, IOC_SPI_SET_CFLAG, SPI_MODE_0 | SPI_MSB | SPI_MASTER | SPI_DATA_8BIT);
        ddkc_info("write to spi return %d\r\n", ret);

        buffer[0] = 0x90;
        ret = write(fd, buffer, 4);
        ddkc_info("write to spi return %d\r\n", ret);

        memset(buffer, 0, sizeof(buffer));
        ret = read(fd, buffer, sizeof(buffer));
        ddkc_info("read from spi return %d\r\n", ret);

        for (i = 0; i < 4; i++) {
            ddkc_info("buffer[%d] - 0x%x\r\n", i, buffer[i]);
        }

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
        close(fd);
    }
    return 0;
}

static void vfs_spi_cli_cmd(int argc, char **argv)
{
    int ret = 0;
    int index = argc > 1 ? atoi(argv[1]) : 1;

    ddkc_info("argc:%d, index:%d\r\n", argc, index);
    ret = vfs_spi_test(index);

    ddkc_info("vfs spi test %s, ret:%d\r\n", ret ? "failed" : "success", ret);

    return;
}

#if AOS_COMP_CLI
/* reg args: fun, cmd, description*/
ALIOS_CLI_CMD_REGISTER(vfs_spi_cli_cmd, spi_example, spi vfs example)
#endif