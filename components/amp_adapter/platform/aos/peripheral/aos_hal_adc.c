/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <aos/errno.h>
#include "aos_hal_adc.h"
#include <vfsdev/adc_dev.h>

int32_t aos_hal_adc_init(adc_dev_t *adc)
{
    uint32_t flags = 0;
    int32_t ret = 0;
    int32_t *p_fd = NULL;
    char name[16] = {0};

    if (!adc || adc->priv)
        return -EINVAL;

    p_fd = (int32_t *)malloc(sizeof(int32_t));
    if (!p_fd)
        return -ENOMEM;
    *p_fd = -1;

    snprintf(name, sizeof(name), "/dev/adc%d", adc->port);

    *p_fd = open(name, 0);
    if (*p_fd < 0) {
        printf ("open %s failed, fd:%d\r\n", name, *p_fd);
        ret = -EIO;
	    goto out;
    }

    ret = ioctl(*p_fd, IOC_ADC_START, adc->config.sampling_cycle);
    if (ret) {
        printf ("start %s failed, ret:%d\r\n", name, ret);
        goto out;
    }

out:
    if (!ret) {
        adc->priv = p_fd;
    } else {
        if (*p_fd >= 0)
            close(*p_fd);
        free(p_fd);
        p_fd = NULL;
    }
    return ret;
}

int32_t aos_hal_adc_value_get(adc_dev_t *adc, uint32_t *output, uint32_t timeout)
{
    int32_t *p_fd = NULL;
    int32_t ret = -1;
    io_adc_arg_t arg;

    if (!adc || !output)
        return -EINVAL;

    p_fd = (int32_t *)adc->priv;

    if (!p_fd || *p_fd < 0)
        return -EIO;

    arg.timeout = timeout;
    ret = ioctl(*p_fd, IOC_ADC_GET_VALUE, (unsigned long)&arg);
    if (ret) {
        printf ("get value of adc%d failed, ret:%d\r\n", adc->port, ret);
    } else {
        *(uint32_t *)output = arg.value;
    }

    return ret;
}

int32_t aos_hal_adc_finalize(adc_dev_t *adc)
{
    int32_t ret = 0;
    int32_t port = 0;
    int32_t *p_fd = NULL;

    if (!adc || !adc->priv)
        return -EINVAL;

    p_fd = (int32_t *)adc->priv;

    if (*p_fd < 0)
        return -EIO;

    if (*p_fd >= 0) {
        ret = ioctl(*p_fd, IOC_ADC_STOP, 0);
        if (ret)
            printf ("adc%d stop failed, ret:%d\r\n", adc->port, ret);
        close(*p_fd);
    } else
        ret = -EALREADY;

    adc->priv = NULL;

    *p_fd = -1;
    free(p_fd);

    return ret;
}
