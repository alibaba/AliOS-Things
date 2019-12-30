/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef U_LOCATION_API__H
#define U_LOCATION_API__H
#include <stdint.h>
#include <stdbool.h>
#include "ulocation_hal.h"
#ifdef __cplusplus
extern "C" {
#endif

int ulocation_init(ulocation_scenario_e scen, int update_inv);

int ulocation_update_gpsinfo(location_t *gps);

int ulocation_update_wifiinfo(char *mmac, char *macs);

int ulocation_update_gprsinfo(char *cdma, char *bts, char *nearbts);

#ifdef __cplusplus
}
#endif

#endif /*U_LOG_API__H*/

