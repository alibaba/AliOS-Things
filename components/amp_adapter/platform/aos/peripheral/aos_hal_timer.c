/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <aos/errno.h>
#include "aos_hal_timer.h"
#include <vfsdev/timer_dev.h>

int32_t aos_hal_timer_init(timer_dev_t *tim)
{
    int32_t ret = 0;
    int32_t port = 0;
    int32_t *p_fd = NULL;
    char name[16] = {0};
    timer_alarm_t alarm;

    if (!tim || tim->priv)
        return -EINVAL;

    port = tim->port;
    p_fd = (int32_t *)malloc(sizeof(int32_t));
    if (!p_fd)
        return -ENOMEM;
    *p_fd = -1;

    snprintf(name, sizeof(name), "/dev/timer%d", port);

    *p_fd = open(name, 0);

    if (*p_fd < 0) {
        printf ("open %s failed, fd:%d\r\n", name, *p_fd);
        ret = -EIO;
        goto out;
    }

    ret = 0;

out:
    if (!ret) {
        tim->priv = p_fd;
    } else {
        if (*p_fd >= 0)
            close(*p_fd);
        free(p_fd);
        p_fd = NULL;
    }
    return ret;
}

int32_t aos_hal_timer_start(timer_dev_t *tim)
{
    int32_t *p_fd = NULL;
    int32_t ret = -1;
    timer_alarm_t alarm;

    if (!tim)
        return -EINVAL;

    p_fd = (int32_t *)tim->priv;

    if (!p_fd || *p_fd < 0)
        return -EIO;

    alarm.arg = tim->config.arg;
    alarm.cb = tim->config.cb;
    alarm.period = tim->config.period;
    alarm.auto_reload = tim->config.reload_mode;

    ret = ioctl(*p_fd, IOC_TIMER_IRQP_SET, (unsigned long)&alarm);

    if (ret)
        printf("ioctl on IOC_TIMER_IRQP_SET failed, ret:%d\r\n", ret);

    ret = ioctl(*p_fd, IOC_TIMER_CONTROL, (unsigned long)IO_TIMER_START);
    if (ret)
        printf("start timer%d failed, ret:%d\r\n", tim->port, ret);

    return ret;
}

void aos_hal_timer_stop(timer_dev_t *tim)
{
        int32_t *p_fd = NULL;
    int32_t ret = -1;

    if (!tim)
        return;

    p_fd = (int32_t *)tim->priv;

    if (!p_fd || *p_fd < 0)
        return;

    ret = ioctl(*p_fd, IOC_TIMER_CONTROL, (unsigned long)IO_TIMER_STOP);
    if (ret)
        printf("stop timer%d failed, ret:%d\r\n", tim->port, ret);

    return;
}

int32_t aos_hal_timer_para_chg(timer_dev_t *tim, timer_config_t para)
{
    int32_t *p_fd = NULL;
    int32_t ret = -1;
    timer_alarm_t alarm;

    if (!tim)
        return -EINVAL;

    p_fd = (int32_t *)tim->priv;

    if (!p_fd || *p_fd < 0)
        return -EIO;

    alarm.arg = para.arg;
    alarm.cb = para.cb;
    alarm.period = para.period;
    alarm.auto_reload = para.reload_mode;

    ret = ioctl(*p_fd, IOC_TIMER_IRQP_SET, (unsigned long)&alarm);
    if (ret)
        printf("change parameter of timer%d failed, ret:%d\r\n", tim->port, ret);

    return ret;
}

int32_t aos_hal_timer_finalize(timer_dev_t *tim)
{
    int32_t ret = 0;
    int32_t port = 0;
    int32_t *p_fd = NULL;

    if (!tim || !tim->priv)
        return -EINVAL;

    p_fd = (int32_t *)tim->priv;

    if (*p_fd < 0)
        return -EIO;

    if (*p_fd >= 0)
        close(*p_fd);
    else
        ret = -EALREADY;

    tim->priv = NULL;

    *p_fd = -1;
    free(p_fd);

    return ret;
}
