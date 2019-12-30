/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdio.h>

#include "ulocation/ulocation.h"
#include "ulocation_hal.h"

int ulocation_hal_get_gprs(char *cdma, char *bts, char *nearbts)
{
    int ret = -1;

    ret = sim800_get_gprs_network_info(bts, BTS_BUF, nearbts, NEARBTS_BUF);
    if (ret != 0) {
        LOGE("uLocation", "sim800 get gprs net work info failed!");
        return ret;
    }
    /* currently, only gsm is supported */
    memcpy(cdma, "0", sizeof(cdma));
    return ret;
}

