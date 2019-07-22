/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdio.h>

#include "ulocation/ulocation.h"
#include "ulocation_hal.h"

#ifdef DEV_SAL_SIM800
#define ULOCATION_GPS
#endif

#define NORTHARCTIC_LAT 90
#define LATDIFF_ARCTIC  10

int ulocation_hal_get_gps(location_t **gps)
{
    int   rlt = -1;
    float tmp;

#ifdef ULOCATION_GPS
    static location_t l;

    l.outdoor.latitude = NORTHARCTIC_LAT; /* north pole */

    sim800_get_gps(&l.outdoor.latitude, &l.outdoor.longitude, &l.outdoor.altitude);

    tmp = (l.outdoor.latitude - NORTHARCTIC_LAT);

    if (tmp * tmp < LATDIFF_ARCTIC) { /* are you inside north arctic ? */
        rlt = -1;
    } else {
        set_location(&l);
        *gps = &l;
        rlt = 0;
    }
#endif

    return rlt;
}

