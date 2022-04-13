/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include <fcntl.h>
#include <sys/ioctl.h>

#include <aos/vfs.h>
#include <hal2vfs/io_dac.h>
#include <drivers/char/u_device.h>

void vfs_dac_handler (void *arg) {
    ddkc_info("enter %s arg:%p, fd = %d\r\n", __func__, arg, *((int *)arg));

    return;
}

int vfs_dac_test(int id, int channel)
{
    int ret = 0;
    int fd = 0;
    int value = 0;
    char name[16] = {0};

    snprintf(name, sizeof(name), "/dev/dac%d", channel);

    fd = open(name, 0);
    ddkc_info("open %s %s, fd:%d\r\n", name, fd >= 0 ? "success" : "fail", fd);

    if (fd >= 0) {
        // correct parameters

        ret = ioctl(fd, IOC_DAC_START, channel);
        ddkc_info("start dac %s, ret:%d\r\n", ret ? "fail" : "succeed", ret);

        sleep(1);

        value = 0xAA;

        ret = ioctl(fd, IOC_DAC_SET_VALUE, MAKE_DAC_ARG(channel, value));
        ddkc_info("set value:%d to channel:%d %s, ret:%d\r\n", value, channel, ret ? "fail" : "succeed", ret);
        sleep(2);

        value = 0;
        ret = ioctl(fd, IOC_DAC_GET_VALUE, MAKE_DAC_ARG(channel, value));
        ddkc_info("get value of channel:%d returns %d\r\n", ret);
        sleep(3);

        ret = ioctl(fd, IOC_DAC_STOP, channel);

        close(fd);
    }
    return 0;
}

static void vfs_dac_cli_cmd(char *buf, int len, int argc, char **argv)
{
    int ret = 0;
    int id = argc > 1 ? atoi(argv[1]) : 0;
    int ch = argc > 2 ? atoi(argv[2]) : 1;

    ddkc_info("argc:%d, id[%d], dac[%d]\n", argc, id, ch);
    ret = vfs_dac_test(id, ch);

    ddkc_info("vfs dac test %s, ret:%d\r\n", ret ? "failed" : "success", ret);

    return;
}

struct cli_command vfs_dac_cli_cmds[] = {
    {"dact",         "dac test", vfs_dac_cli_cmd},
};

int vfs_dac_test_cmd_init(void) {
    return aos_cli_register_commands(&vfs_dac_cli_cmds[0],
                                     sizeof(vfs_dac_cli_cmds)/sizeof(vfs_dac_cli_cmds[0]));
}

POST_DRIVER_ENTRY(vfs_dac_test_cmd_init)