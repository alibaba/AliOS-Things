/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */
#include <stdlib.h>
#include <fcntl.h>
#include <poll.h>
#include <sys/ioctl.h>

#if AOS_COMP_CLI
#include "aos/cli.h"
#endif
#include "aos/vfs.h"

#include <vfsdev/gpio_dev.h>
#include <drivers/char/u_device.h>
#include <drivers/u_ld.h>

int vfs_gpio_test(int id)
{
    int ret = 0;
    int fd = 0;
    char buffer[16];
    struct gpio_io_config config;

    fd = open("/dev/gpio", 0);
    ddkc_info("open gpio %s, fd:%d\r\n", fd >= 0 ? "success" : "fail", fd);

    if (fd >= 0) {
        config.id = id;
        config.config = GPIO_IO_INPUT | GPIO_IO_INPUT_PU;
        config.data = 0;
        ret = ioctl(fd, IOC_GPIO_GET,  (unsigned long)&config);
        ddkc_info("gpio read %d return %d\r\n", id, ret);

        config.id = id;
        config.config = GPIO_IO_OUTPUT | GPIO_IO_OUTPUT_PP;
        config.data = id%2;
        ret = ioctl(fd, IOC_GPIO_SET, (unsigned long)&config);
        ddkc_info("gpio write %d return %d\r\n", config.data, ret);
        usleep(1000000);
        config.config = GPIO_IO_OUTPUT_TOGGLE;
        config.data = 0;
        ret = ioctl(fd, IOC_GPIO_SET, (unsigned long)&config);
        ddkc_info("gpio toggle return %d\r\n", ret);

        close(fd);
    }
    return 0;
}

#if AOS_COMP_CLI
static void vfs_gpio_cli_cmd(char *buf, int len, int argc, char **argv)
{
    int ret = 0;
    int id = argc > 1 ? atoi(argv[1]) : 1;

    ddkc_info("argc:%d, gpio[%d]\n", argc, id);
    ret = vfs_gpio_test(id);

    ddkc_info("vfs gpio test %s, ret:%d\r\n", ret ? "failed" : "success", ret);

    return;
}

struct cli_command vfs_gpio_cli_cmds[] = {
    { "gpiot", "gpio test", vfs_gpio_cli_cmd, },
};

int vfs_gpio_test_cmd_init(void)
{
    return aos_cli_register_commands(&vfs_gpio_cli_cmds[0], sizeof(vfs_gpio_cli_cmds) / sizeof(vfs_gpio_cli_cmds[0]));
}

POST_DRIVER_ENTRY(vfs_gpio_test_cmd_init)
#endif /* AOS_COMP_CLI */
