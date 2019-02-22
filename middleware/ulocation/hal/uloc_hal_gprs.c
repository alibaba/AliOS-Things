/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */
 
#include "stdio.h"
#include <ulocation/ulocation.h>
#include <uloc_hal.h>

#ifdef DEV_SAL_SIM800
#define ULOC_GPRS
#endif

int uloc_hal_get_gprs(char ** cdma, char ** bts,  char ** nearbts)
{
    int rlt = -1;
    
#ifdef ULOC_GPRS
    /* test */
    static char bts_buf[128];
    static char nbts_buf[128 * 4];

    sim800_get_gprs_network_info(bts_buf, sizeof(bts_buf), nbts_buf, sizeof(nbts_buf));

    /* currently, only gsm is supported */
    *cdma = "0";         
    *bts     = bts_buf;
    *nearbts = nbts_buf;

    rlt = 0;
#endif

    return rlt;
}

