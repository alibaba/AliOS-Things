/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef ULOCATION_HAL_H
#define ULOCATION_HAL_H

#ifdef __cplusplus
extern "C" {
#endif

int ulocation_hal_get_wifi(char **mmac, char **macs);

int ulocation_hal_get_gprs(char **cdma, char **bts, char **nearbts);

int ulocation_hal_get_gps(location_t **gps);

#ifdef __cplusplus
}
#endif

#endif  /* ULOCATION_HAL_H */

