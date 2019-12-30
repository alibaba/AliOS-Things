/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include "aos/kernel.h"
#include "ulog/ulog.h"
#include "ulocation/ulocation.h"

#define MIN_INTERVAL 5000

static ulocation_scenario_e location_scen = ULOCATION_SCENARIO_OUTDOOR;

static int retrive_gps_interval = MIN_INTERVAL;

int ulocation_init(ulocation_scenario_e scen, int update_inv)
{
    int ret = -1;

    if (scen >= ULOCATION_SCENARIO_COUNT) {
        LOGE("uLocation", "invalid scenario!");
        return ret;
    }
    location_scen = scen;
    if (update_inv > MIN_INTERVAL) {
       retrive_gps_interval = update_inv;
    }

#ifdef ULOCATION_GPS
    ret = ulocation_hal_init();
    if (ret != 0) {
       LOGE("uLocation", "ulocation hal gps init failed!"); 
    }
#endif

    return 0;
}
