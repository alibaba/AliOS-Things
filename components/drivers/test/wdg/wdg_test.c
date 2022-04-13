/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include <fcntl.h>
#include <sys/ioctl.h>

#include "aos/vfs.h"
#include "aos/cli.h"

#include <drivers/u_ld.h>
#include <hal2vfs/io_wdg.h>
#include <drivers/char/u_device.h>

int vfs_wdg_test(int count, int period)
{
    int ret = 0;
    int fd = 0;

    fd = open("/dev/wdg0", 0);
    ddkc_info("open wdg %s, fd:%d\r\n", fd >= 0 ? "success" : "fail", fd);

    if (fd >= 0) {
        while (count--) {
            ret = ioctl(fd, IOC_WDG_RELOAD, 0);
            ddkc_info("wdg reload %s, ret:%d\r\n", ret ? "fail" : "succeed", ret);
            usleep(period);
        }
        close(fd);
    }
    ddkc_info("wdg test done\r\n");
    return 0;
}

static void vfs_wdg_cli_cmd(char *buf, int len, int argc, char **argv)
{
    int ret = 0;
    int count = argc > 1 ? atoi(argv[1]) : 20;
    int period = argc > 2 ? atoi(argv[2]) : 300;

    ddkc_info("argc:%d, count:%d, period:%d ms\r\n", argc, count, period);
    ret = vfs_wdg_test(count, period);

    ddkc_info("vfs wdg test %s, ret:%d\r\n", ret ? "failed" : "success", ret);

    return;
}

struct cli_command vfs_wdg_cli_cmds[] = {
    {"wdgt",         "wdg test", vfs_wdg_cli_cmd},
};

int vfs_wdg_test_cmd_init(void) {
    return aos_cli_register_commands(&vfs_wdg_cli_cmds[0],
                                     sizeof(vfs_wdg_cli_cmds)/sizeof(vfs_wdg_cli_cmds[0]));
}

POST_DRIVER_ENTRY(vfs_wdg_test_cmd_init)
