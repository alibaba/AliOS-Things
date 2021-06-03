/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <aos/errno.h>
#include "aos_hal_wdg.h"
#include <vfsdev/wdg_dev.h>

int32_t aos_hal_wdg_init(wdg_dev_t *wdg)
{
    uint32_t flags = 0;
    int32_t ret = 0;
    int32_t *p_fd = NULL;
    char name[16] = {0};

    if (!wdg || wdg->priv)
        return -EINVAL;

    p_fd = (int32_t *)malloc(sizeof(int32_t));
    if (!p_fd)
        return -ENOMEM;
    *p_fd = -1;

    snprintf(name, sizeof(name), "/dev/wdg%d", wdg->port);

    *p_fd = open(name, 0);

    if (*p_fd < 0) {
        printf ("open %s failed, fd:%d\r\n", name, *p_fd);
        ret = -EIO;
	    goto out;
    }

out:
    if (!ret) {
        wdg->priv = p_fd;
    } else {
        if (*p_fd >= 0)
            close(*p_fd);
        free(p_fd);
        p_fd = NULL;
    }
    return ret;
}

void aos_hal_wdg_reload(wdg_dev_t *wdg)
{
    int32_t *p_fd = NULL;
    int32_t ret = -1;

    if (!wdg)
        return;

    p_fd = (int32_t *)wdg->priv;

    if (!p_fd || *p_fd < 0)
        return;

    ret = ioctl(*p_fd, IOC_WDG_RELOAD, 0);
    if (ret) {
        printf ("reload wdg%d failed, ret:%d\r\n", wdg->port, ret);
    }

    return;
}

int32_t aos_hal_wdg_finalize(wdg_dev_t *wdg)
{
    int32_t ret = 0;
    int32_t port = 0;
    int32_t *p_fd = NULL;

    if (!wdg || !wdg->priv)
        return -EINVAL;

    p_fd = (int32_t *)wdg->priv;

    if (*p_fd < 0)
        return -EIO;

    if (*p_fd >= 0) {
        close(*p_fd);
    } else
        ret = -EALREADY;

    wdg->priv = NULL;

    *p_fd = -1;
    free(p_fd);

    return ret;
}
