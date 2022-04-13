/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "aos/cli.h"
#include "aos/vfs.h"

#include <drivers/u_ld.h>
#include <hal2vfs/io_pwm.h>
#include <drivers/char/u_device.h>

int vfs_pwm_test(int channel)
{
    int ret = -1;
    int fd = 0;
    int freq = 100;
    float duty_cycle = 0.5;
    char name[16] = {0};

    snprintf(name, sizeof(name), "/dev/pwm%d", channel);
    fd = open(name, 0);
    ddkc_warn("open %s %s, fd:%d\r\n", name, fd >= 0 ? "success" : "fail", fd);

    if (fd >= 0) {

        ret = ioctl(fd, IOC_PWM_FREQ, (unsigned long)freq);
        ddkc_info("set pwm's freq to %d %s, ret:%d\r\n", freq, ret ? "fail" : "succeed", ret);

        ret = ioctl(fd, IOC_PWM_DUTY_CYCLE, (unsigned long)&duty_cycle);
        ddkc_info("set pwm's duty cycle to %f %s, ret:%d\r\n", duty_cycle, ret ? "fail" : "succeed", ret);

        ret = ioctl(fd, IOC_PWM_ON, (unsigned long)0);
        ddkc_info("set pwm on %s, ret:%d\r\n", ret ? "fail" : "succeed", ret);

        sleep(1);
        ret = ioctl(fd, IOC_PWM_OFF, (unsigned long)0);
        ddkc_info("set pwm off %s, ret:%d\r\n", ret ? "fail" : "succeed", ret);
        close(fd);
        ret = 0;
    }
    fd = open(name, 0);
    if (fd >= 0) {
        freq = 1000;
        ret = ioctl(fd, IOC_PWM_FREQ, (unsigned long) freq);
        ddkc_info("change pwm's freq to %d %s, ret:%d\r\n", freq, ret ? "fail" : "succeed", ret);

        duty_cycle = 0.1;
        ret = ioctl(fd, IOC_PWM_DUTY_CYCLE, (unsigned long) &duty_cycle);
        ddkc_info("change pwm's duty cycle to %f %s, ret:%d\r\n", duty_cycle, ret ? "fail" : "succeed", ret);

        ret = ioctl(fd, IOC_PWM_ON, (unsigned long) 0);
        ddkc_info("set pwm on %s, ret:%d\r\n", ret ? "fail" : "succeed", ret);

        sleep(1);
        ret = ioctl(fd, IOC_PWM_OFF, (unsigned long) 0);
        ddkc_info("set pwm off %s, ret:%d\r\n", ret ? "fail" : "succeed", ret);

        close(fd);
        ret = 0;
    }
    return 0;
}

static void vfs_pwm_cli_cmd(char *buf, int len, int argc, char **argv)
{
    int ret = 0;
    int ch = argc > 1 ? atoi(argv[1]) : 1;

    ddkc_info("argc:%d, pwm[%d]\n", argc, ch);
    ret = vfs_pwm_test(ch);

    ddkc_info("vfs pwm test %s, ret:%d\r\n", ret ? "failed" : "success", ret);

    return;
}

struct cli_command vfs_pwm_cli_cmds[] = {
    {"pwmt",         "pwm test", vfs_pwm_cli_cmd},
};

int vfs_pwm_test_cmd_init(void) {
    return aos_cli_register_commands(&vfs_pwm_cli_cmds[0],
                                     sizeof(vfs_pwm_cli_cmds)/sizeof(vfs_pwm_cli_cmds[0]));
}

POST_DRIVER_ENTRY(vfs_pwm_test_cmd_init)
