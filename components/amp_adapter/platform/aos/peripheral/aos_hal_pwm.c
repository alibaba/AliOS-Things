/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <aos/errno.h>
#include "aos_hal_pwm.h"
#include <vfsdev/pwm_dev.h>

int32_t aos_hal_pwm_init(pwm_dev_t *pwm)
{
    uint32_t flags = 0;
    int32_t ret = 0;
    int32_t *p_fd = NULL;
    char name[16] = {0};

    if (!pwm || pwm->priv)
        return -EINVAL;

    p_fd = (int32_t *)malloc(sizeof(int32_t));
    if (!p_fd)
        return -ENOMEM;
    *p_fd = -1;

    snprintf(name, sizeof(name), "/dev/pwm%d", pwm->port);

    *p_fd = open(name, 0);

    if (*p_fd < 0) {
        printf ("open %s failed, fd:%d\r\n", name, *p_fd);
        ret = -EIO;
	    goto out;
    }

    ret = ioctl(*p_fd, IOC_PWM_FREQ, (unsigned long)pwm->config.freq);
    if (ret) {
        printf ("set freq to %d on %s failed, ret:%d\r\n", pwm->config.freq, name, ret);
        goto out;
    }

    ret = ioctl(*p_fd, IOC_PWM_DUTY_CYCLE, (unsigned long)&pwm->config.duty_cycle);
    if (ret) {
        printf ("set duty cycle to %d on %s failed, ret:%d\r\n", pwm->config.duty_cycle, name, ret);
        goto out;
    }

out:
    if (!ret) {
        pwm->priv = p_fd;
    } else {
        if (*p_fd >= 0)
            close(*p_fd);
        free(p_fd);
        p_fd = NULL;
    }
    return ret;
}

int32_t aos_hal_pwm_start(pwm_dev_t *pwm)
{
    int32_t *p_fd = NULL;
    int32_t ret = -1;

    if (!pwm)
        return -EINVAL;

    p_fd = (int32_t *)pwm->priv;

    if (!p_fd || *p_fd < 0)
        return -EIO;

    ret = ioctl(*p_fd, IOC_PWM_ON, NULL);
    if (ret) {
        printf ("pwm%d start failed, ret:%d\r\n", pwm->port, ret);
    }

    return ret;
}

int32_t aos_hal_pwm_stop(pwm_dev_t *pwm)
{
    int32_t *p_fd = NULL;
    int32_t ret = -1;

    if (!pwm)
        return -EINVAL;

    p_fd = (int32_t *)pwm->priv;

    if (!p_fd || *p_fd < 0)
        return -EIO;

    ret = ioctl(*p_fd, IOC_PWM_OFF, NULL);
    if (ret) {
        printf ("pwm%d start failed, ret:%d\r\n", pwm->port, ret);
    }

    return ret;
}

int32_t aos_hal_pwm_para_chg(pwm_dev_t *pwm, pwm_config_t para)
{
    int32_t *p_fd = NULL;
    int32_t ret = -1;

    if (!pwm)
        return -EINVAL;

    p_fd = (int32_t *)pwm->priv;

    if (!p_fd || *p_fd < 0)
        return -EIO;

    ret = ioctl(*p_fd, IOC_PWM_FREQ, (unsigned long)para.freq);
    if (ret) {
        printf ("set freq to %d on pwm%d failed, ret:%d\r\n", para.freq, pwm->port, ret);
    }

    ret = ioctl(*p_fd, IOC_PWM_DUTY_CYCLE, (unsigned long)&para.duty_cycle);
    if (ret) {
        printf ("set duty cycle to %d on pwm%d failed, ret:%d\r\n", para.duty_cycle, pwm->port, ret);
    }

    return ret;
}

int32_t aos_hal_pwm_finalize(pwm_dev_t *pwm)
{
    int32_t ret = 0;
    int32_t port = 0;
    int32_t *p_fd = NULL;

    if (!pwm || !pwm->priv)
        return -EINVAL;

    p_fd = (int32_t *)pwm->priv;

    if (*p_fd < 0)
        return -EIO;

    if (*p_fd >= 0)
        close(*p_fd);
    else
        ret = -EALREADY;

    pwm->priv = NULL;

    *p_fd = -1;
    free(p_fd);

    return ret;
}



