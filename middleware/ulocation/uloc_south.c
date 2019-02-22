/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#include "stdio.h"

#include <ulocation/ulocation.h>
#include <uloc_hal.h>

int uloc_update_locationinfo(location_t ** gps,
                             char ** mmac, char ** macs,                   /* wifi       */
                             char ** cdma, char ** bts,  char ** nearbts)  /* 2g network */
{
    *gps  = NULL;
    *mmac = NULL;  *macs = NULL;
    *cdma = NULL;  *bts  = NULL; *nearbts = NULL;

    /* get wifi ssid or (gps & cell info) */
    uloc_hal_get_wifi(mmac, macs);
    uloc_hal_get_gprs(cdma, bts, nearbts);
    uloc_hal_get_gps(gps);

    return 0;
}


