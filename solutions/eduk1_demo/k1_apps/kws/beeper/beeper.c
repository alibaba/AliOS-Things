/*
 * Copyright (C) 2020-2023 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <ulog/ulog.h>
#include <vfsdev/pwm_dev.h>

void beeper_start(uint16_t port, uint16_t frequency, uint16_t duration)
{
    int ret = -1;
    int fd = 0;
    char name[16] = {0};
    float duty_cycle = 0.8;

    snprintf(name, sizeof(name), "/dev/pwm%d", port);
    fd = open(name, 0);
    if (fd >= 0) {
        if (frequency > 0) {
            ret = ioctl(fd, IOC_PWM_FREQ, (unsigned long)frequency);
            ret = ioctl(fd, IOC_PWM_DUTY_CYCLE, (unsigned long)&duty_cycle);
            ret = ioctl(fd, IOC_PWM_ON, (unsigned long)0);
        }
        if (duration != 0) {
            aos_msleep(duration);
        }
        if (frequency > 0 && duration > 0) {
            ret = ioctl(fd, IOC_PWM_OFF, (unsigned long)0);
            close(fd);
        }
    }
}

void beeper_stop(uint16_t port)
{
    int ret = -1;
    int fd = 0;
    char name[16] = {0};
    float duty_cycle = 0.8;
    int freq = 1;
    unsigned int period_s;

    snprintf(name, sizeof(name), "/dev/pwm%d", port);
    fd = open(name, 0);
    if (fd >= 0) {
        ret = ioctl(fd, IOC_PWM_FREQ, (unsigned long)freq);
        ret = ioctl(fd, IOC_PWM_DUTY_CYCLE, (unsigned long)&duty_cycle);
        ret = ioctl(fd, IOC_PWM_ON, (unsigned long)0);
        ret = ioctl(fd, IOC_PWM_OFF, (unsigned long)0);
        close(fd);
    }
}
