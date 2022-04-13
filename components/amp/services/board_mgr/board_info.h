/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef BE_BOARD_INFO_H
#define BE_BOARD_INFO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define DEVICE_KEY_TAG "DEVICE_KEY"
#define DEVICE_NAME_TAG "DEVICE_NAME"
#define DEVICE_SECRET_TAG "DEVICE_SECRET"

/**
 * set config info of the board
 *
 * @param[in]  deviceKey   the key of a product
 * @param[in]  deviceName  the name of a product
 *@param[in]  deviceSecret the secret of a product
 * @return  0 is ok, others fail
 */

int8_t board_setDeviceInfo(char *deviceKey, char *deviceName,
                           char *deviceSecret);

/**
 * get config info of the board
 *
 * @param[out]  deviceKey   the key of a product
 * @param[out]  deviceName  the name of a product
 *@param[out]  deviceSecret the secret of a product
 * @return  0 is ok, others fail
 */

int8_t board_getDeviceInfo(char **productKey, char **deviceName,
                           char **deviceSecret);

#ifdef __cplusplus
}
#endif

#endif /* BE_BOARD_INFO_H */
