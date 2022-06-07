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
#ifndef AOS_BOARD_HAAS700
#include <aos/pwm.h>
#endif

#define NS_PER_SEC (1000000000UL)
#define DIV_ROUND_CLOSEST(x, divisor)(          \
{                                               \
    typeof(x) __x = x;                          \
    typeof(divisor) __d = divisor;              \
    (((typeof(x))-1) > 0 ||                     \
     ((typeof(divisor))-1) > 0 ||               \
     (((__x) > 0) == ((__d) > 0))) ?            \
        (((__x) + ((__d) / 2)) / (__d)) :       \
        (((__x) - ((__d) / 2)) / (__d));        \
}                                               \
)

int32_t aos_hal_pwm_init(pwm_dev_t *pwm)
{
#ifndef AOS_BOARD_HAAS700
    int ret = 0;
    aos_pwm_ref_t *ref = NULL;
    aos_pwm_attr_t attr;

    ref = (aos_pwm_ref_t *)malloc(sizeof(aos_pwm_ref_t));
    if (ref == NULL) {
        return -ENOMEM;
    }
    memset(&attr, 0, sizeof(aos_pwm_attr_t));
    ret = aos_pwm_get(ref, pwm->port);
    if (ret < 0) {
        goto out;
    }

    attr.period = pwm->config.freq > 0 ? DIV_ROUND_CLOSEST(NS_PER_SEC, pwm->config.freq) : 0;
    attr.duty_cycle = pwm->config.duty_cycle > 0 ? (attr.period * pwm->config.duty_cycle) / 100 : 0;
    attr.enabled = 1;
    attr.polarity = 0;

    ret = aos_pwm_set_attr(ref, &attr);
    if (ret) {
        goto out;
    }

out:
    if (!ret) {
        pwm->priv = ref;
    } else {
        if (ref != NULL)
            aos_pwm_put(ref);
        free(ref);
        ref = NULL;
    }
    return ret;
#else
    return -1;
#endif
}

int32_t aos_hal_pwm_stop(pwm_dev_t *pwm)
{
#ifndef AOS_BOARD_HAAS700
    int ret = 0;
    aos_pwm_ref_t *ref = NULL;
    aos_pwm_attr_t attr;

    if (!pwm)
        return -EINVAL;

    ref = (aos_pwm_ref_t *)pwm->priv;

    if (!ref)
        return -EIO;

    attr.enabled = 0;
    ret = aos_pwm_set_attr(ref, &attr);
    return ret;
#else
    return -1;
#endif
}


int32_t aos_hal_pwm_start(aos_hal_pwm_dev_t *pwm)
{
#ifndef AOS_BOARD_HAAS700
    return 0;
#else
    return -1;
#endif
}

int32_t aos_hal_pwm_para_chg(pwm_dev_t *pwm, pwm_config_t para)
{
#ifndef AOS_BOARD_HAAS700
    int ret = 0;
    aos_pwm_ref_t *ref = NULL;
    aos_pwm_attr_t attr;
    if (!pwm)
        return -EINVAL;

    ref = (aos_pwm_ref_t *)pwm->priv;

    if (!ref)
        return -EIO;

    attr.period = para.freq > 0 ? DIV_ROUND_CLOSEST(NS_PER_SEC, para.freq) : 0;
    attr.duty_cycle = para.duty_cycle > 0 ? (attr.period * para.duty_cycle) / 100 : 0;
    attr.enabled = 1;
    attr.polarity = 0;
    ret = aos_pwm_set_attr(ref, &attr);
    return ret;
#else
    return -1;
#endif
}

int32_t aos_hal_pwm_finalize(pwm_dev_t *pwm)
{
#ifndef AOS_BOARD_HAAS700
    int ret;
    aos_pwm_ref_t *ref = NULL;
    aos_pwm_attr_t attr;
    if (!pwm)
        return -EINVAL;

    ref = (aos_pwm_ref_t *)pwm->priv;

    if (!ref)
        return -EIO;

    attr.enabled = 0;
    ret = aos_pwm_set_attr(ref, &attr);
    if (ret) {
        goto out;
    }
    aos_pwm_put(ref);
    free(ref);
    return 0;
out:
    return ret;
#else
    return -1;
#endif
}



