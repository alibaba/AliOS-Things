/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "aos/vfs.h"
#include "aos/cli.h"

#include <aos/hal/i2c.h>
#include <drivers/u_ld.h>
#include <hal2vfs/io_i2c.h>
#include <drivers/char/u_device.h>

int vfs_i2c_test(int id)
{
    int ret = 0;
    int fd = 0;
    io_i2c_control_u c;
    io_i2c_data_t d;
    unsigned char buff[256];
    char name[16] = {0};

    snprintf(name, sizeof(name), "/dev/i2c%d", id);
    fd = open(name, 0);
    ddkc_info("open %s %s, fd:%d\r\n", name, fd > 0 ? "success" : "fail", fd);

    if (fd >= 0) {
        c.c.addr = 0x55;
        c.c.addr_width = 0;
        c.c.role = 1;
        ret = ioctl(fd, IOC_I2C_SET_CONFIG, (unsigned long)&c);

        c.freq = 100000;
        ret = ioctl(fd, IOC_I2C_SET_FREQ, (unsigned long)&c);

        ddkc_info("ioctl on IOC_I2C_SET_FREQ %s ret:%d\r\n", ret ? "fail" : "succeed", ret);

        d.addr = 0x65;
        d.data = buff;
        d.length = sizeof(buff);
        d.maddr = 0x100;
        d.mlength = 100;
        d.timeout = 100;

        ret = ioctl(fd, IOC_I2C_MASTER_TX, (unsigned long)&d);

        ddkc_info("ioctl on IOC_I2C_MASTER_Tx %s, ret:%d\r\n", ret ? "fail" : "succeed", ret);

        ret = ioctl(fd, IOC_I2C_SLAVE_TX, (unsigned long)&d);

        ddkc_info("ioctl on IOC_I2C_SLAVE_TX %s, ret:%d\r\n", ret ? "fail" : "succeed", ret);

        ret = ioctl(fd, IOC_I2C_MEM_TX, (unsigned long)&d);

        ddkc_info("ioctl on IOC_I2C_MEM_TX %s, ret:%d\r\n", ret ? "fail" : "succeed", ret);

        close(fd);
    }
    return 0;
}

extern int si7006_test(void);

static void vfs_i2c_cli_cmd(char *buf, int len, int argc, char **argv)
{
    int ret = 0;
    int index = argc > 1 ? atoi(argv[1]) : 1;

    ddkc_info("argc:%d, index:%d\r\n", argc, index);
    //ret = vfs_i2c_test(index);

    ret = si7006_test();

    ddkc_info("vfs i2c test %s, ret:%d\r\n", ret ? "failed" : "success", ret);

    return;
}

struct cli_command vfs_i2c_cli_cmds[] = {
    {"i2ct",         "i2c test", vfs_i2c_cli_cmd},
};

int vfs_i2c_test_cmd_init(void) {
    return aos_cli_register_commands(&vfs_i2c_cli_cmds[0],
                                     sizeof(vfs_i2c_cli_cmds)/sizeof(vfs_i2c_cli_cmds[0]));
}

POST_DRIVER_ENTRY(vfs_i2c_test_cmd_init)
