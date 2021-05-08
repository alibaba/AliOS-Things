/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include "aos/vfs.h"

#include <vfsdev/timer_dev.h>
#include <drivers/char/u_device.h>
#include <drivers/u_ld.h>

#if AOS_COMP_CLI
#include "aos/cli.h"
#endif

void vfs_timer_handler (void *arg) {
    ddkc_info("enter %s arg:%p, fd = %d\r\n", __func__, arg, *((int *)arg));

    return;
}

int vfs_timer_test(int id)
{
    int ret = 0;
    int fd = 0;
    timer_alarm_t alarm;
    timer_alarm_t ralarm;
    char name[16] = {0};

    snprintf(name, sizeof(name), "/dev/timer%d", id);
    fd = open(name, 0);
    ddkc_info("open %s %s, fd:%d\r\n", name, fd >= 0 ? "success" : "fail", fd);

    if (fd >= 0) {
        // correct parameters
        alarm.arg = &fd;
        alarm.cb = vfs_timer_handler;
        alarm.period = 1000000;
        alarm.auto_reload = true;

        ret = ioctl(fd, IOC_TIMER_IRQP_SET, (unsigned long)&alarm);

        ddkc_info("ioctl on IOC_TIMER_IRQP_SET %s, ret:%d\r\n", ret ? "fail" : "succeed", ret);

        ret = ioctl(fd, IOC_TIMER_CONTROL, (unsigned long)IO_TIMER_START);
        ddkc_info("start timer %s, ret:%d\r\n", ret ? "fail" : "succeed", ret);
        sleep(5);

        ret = ioctl(fd, IOC_TIMER_RELOAD, (unsigned long)false);
        ddkc_info("disable timer reload %s, ret:%d\r\n", ret ? "fail" : "succeed", ret);
        sleep(3);

        ret = ioctl(fd, IOC_TIMER_RELOAD, (unsigned long)true);
        ddkc_info("enable timer reload %s, ret:%d\r\n", ret ? "fail" : "succeed", ret);
        sleep(5);

        alarm.period = 500000;
        ret = ioctl(fd, IOC_TIMER_IRQP_SET, (unsigned long)&alarm);
        ddkc_info("ioctl on IOC_TIMER_IRQP_SET %s, ret:%d\r\n", ret ? "fail" : "succeed", ret);
        sleep(5);

        ret = ioctl(fd, IOC_TIMER_CONTROL, (unsigned long)IO_TIMER_STOP);
        ddkc_info("stop timer %s, ret:%d\r\n", ret ? "fail" : "succeed", ret);

        ret = ioctl(fd, IOC_TIMER_IRQP_GET, (unsigned long)&ralarm);
        ddkc_info("ioctl on IOC_TIMER_IRQP_GET %s, ret:%d\r\n", ret ? "fail" : "succeed", ret);

        ddkc_info("ralarm period:%ld, cb:%p, arg:%p, auto_reload:%d\r\n",
               ralarm.period, ralarm.cb, ralarm.arg, ralarm.auto_reload);

        close(fd);
    }
    return 0;
}

static void vfs_timer_cli_cmd(int argc, char **argv)
{
    int ret = 0;
    int index = argc > 1 ? atoi(argv[1]) : 1;

    ddkc_info("argc:%d, index:%d\r\n", argc, index);
    ret = vfs_timer_test(index);

    ddkc_info("vfs timer test %s, ret:%d\r\n", ret ? "failed" : "success", ret);

    return;
}


#if AOS_COMP_CLI
/* reg args: fun, cmd, description*/
ALIOS_CLI_CMD_REGISTER(vfs_timer_cli_cmd, hal_timer_example, timer vfs example)
#endif