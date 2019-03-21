/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef ULOC_HAL_H
#define ULOC_HAL_H

#ifdef __cplusplus
extern "C" {
#endif

int uloc_hal_get_wifi(char **mmac, char **macs);
int uloc_hal_get_gprs(char **cdma, char **bts, char **nearbts);
int uloc_hal_get_gps(location_t **gps);

#ifdef __cplusplus
}
#endif

#endif  /* ULOC_HAL_H */

