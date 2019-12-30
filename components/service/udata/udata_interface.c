/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdbool.h>
#include <fcntl.h>

#include "aos/kernel.h"
#include "udata_com_desc.h"
#include "service_mgr.h"


int udata_report_publish(udata_type_e type, void *pdata)
{

    if (pdata == NULL) {
        return -1;
    }
    if (type >= UDATA_MAX_CNT)
    {
        return -1;
    }

    return udata_get_report_pkg(type, pdata);
}

int udata_dev_ioctl(udata_t *pkg, uint8_t cmd, void *parm)
{
    /* set the udata_type and related info here */
    // will be supported later
    return 0;
}

int udata_subscribe(udata_type_e type)
{
    int ret = 0;
    ret     = udata_service_subscribe(type);
    if (unlikely(ret)) {
        return -1;
    }

    LOG("%s %s successfully\n", uDATA_STR, __func__);
    return 0;
}

int udata_unsubscribe(udata_type_e type)
{
    int ret = 0;
    ret     = udata_service_unsubscribe(type);
    if (unlikely(ret)) {
        return -1;
    }

    LOG("%s %s successfully\n", uDATA_STR, __func__);
    return 0;
}

