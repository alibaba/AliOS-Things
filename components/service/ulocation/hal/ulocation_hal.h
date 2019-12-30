/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef ULOCATION_HAL_H
#define ULOCATION_HAL_H

#include "ulocation/ulocation.h"
#include "ulog/ulog.h"
#include "sensor/sensor.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BTS_BUF         128
#define NEARBTS_BUF     128*4
#define NORTHARCTIC_LAT 90
#define LATDIFF_ARCTIC  10

/**
 *  from wifi connect infomation get mmac and macs for assist location.
 *  @param[out] mmac  the AP which the device connected.  
 *  @param[out] macs  the AP list which the device scanned.
 *  @retval           0 on success, negative error on failure.
*/
int ulocation_hal_get_wifi(char *mmac, char *macs);

/**
 *  from gprs connect infomation get cdma, bts, nearbts for assist location.
 *  @param[out] cdma    the modulation of communication.
 *  @param[out] bts     the location information of the device accessed.
 *  @param[out] nearbts the location list which the device scanned.
 *  @retval             0 on success, negative error on failure.
 */
int ulocation_hal_get_gprs(char *cdma, char *bts, char *nearbts);

/**
 *  get the gps info.
 *  @param[out] gps  the gps infomation.
 *  @retval          0 on success, negative error on failure.
 */
int ulocation_hal_get_gps(location_t *gps);

/**
 *  gps hal init.
 *  @retval  0 on success, negative error on failure.
 */
int ulocation_hal_gps_init();

/**
 *  ulocation hal init.
 *  @retval  0 on success, negative error on failure.
 */
int ulocation_hal_init();

#ifdef __cplusplus
}
#endif

#endif  /* ULOCATION_HAL_H */
