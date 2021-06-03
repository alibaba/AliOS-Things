/*
 * Copyright (c) 2019 Alibaba Group. All rights reserved.
 * License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef __ALIYUN_IOT_SMARTCARD_HAL_H__
#define __ALIYUN_IOT_SMARTCARD_HAL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#include "aliyun_iot_smartcard_export.h"

typedef enum {
    CARRIER_NULL = 0,
    CHINA_MOBILE = 1,
    CHINA_UNICOM,
    CHINA_TELECOM,
    CARRIER_UNKNOWN
} aliyun_iot_smartcard_carrier_type_t;

typedef enum {
    ACT_NULL = 0,
    ACT_2G,
    ACT_3G,
    ACT_4G
} aliyun_iot_smartcard_act_t;

typedef struct {
    aliyun_iot_smartcard_carrier_type_t car_type;   /**< carrier type */
    char *car_name;                                 /**< carrier name */
    int32_t rssi;                                   /**< rssi */
} aliyun_iot_smartcard_carrier_t;

/**
 * @brief hal init func
 *
 * @return  0 on success, -1 on failure.
 */
int aliyun_iot_smartcard_hal_init(void);

/**
 * @brief hal deinit func
 *
 * @return  0 on success, -1 on failure.
 */
int aliyun_iot_smartcard_hal_deinit(void);

/**
 * @brief get rssi signal
 *
 * @param [in] rssi: rssi threshold
 * @return 0 on success, else on failure.
 * @see None.
 * @note None.
 */
int aliyun_iot_smartcard_hal_get_rssi(int32_t *rssi);

/**
 * @brief get iccid
 *
 * @param [out] iccid: pointer to iccid str
 * @param [in] len: iccid str len
 * @return 0 on success, else on failure.
 * @see None.
 * @note None.
 */
int aliyun_iot_smartcard_hal_get_iccid(char *iccid, int len);

/**
 * @brief get imei
 *
 * @param [out] imei: pointer to imei str
 * @param [in] len: imei str len
 * @return 0 on success, else on failure.
 * @see None.
 * @note None.
 */
int aliyun_iot_smartcard_hal_get_imei(char *imei, int len);

/**
 * @brief get access technology
 *
 * @param [out] act: pointer to access technology
 *
 * @return 0 on success, else on failure.
 * @see None.
 * @note None.
 */
int aliyun_iot_smartcard_hal_get_act(aliyun_iot_smartcard_act_t *act);

/**
 * @brief get current carrier type
 *
 * @param [out] car_type: carrier type
 *
 * @return 0 on success, else on failure.
 * @see None.
 * @note None.
 */
int aliyun_iot_smartcard_hal_get_carrier(aliyun_iot_smartcard_carrier_type_t *car_type);

/**
 * @brief reset module
 *
 * @return  0 on success, negative error on failure.
 * @see None.
 * @note None.
 */
int aliyun_iot_smartcard_hal_reset_module(void);

/*!
 * \brief   set phonebook storage to sim
 *
 * \retval  Possible returns are:
 *          -1: get failed.
 *           0: set succesful.
 */
int aliyun_iot_smartcard_hal_cpbs(void);

/*!
 * \brief   read phonebook
 *
 * \param   [IN] slot: slot number
 *          [OUT] name: pointer to name str
 *          [IN] name_length: name str len
 *          [OUT] number: pointer to number str
 *          [IN] number_length: number str len
 *
 * \retval  Possible returns are:
 *          -1: get failed.
 *           0: set succesful.
 */
int aliyun_iot_smartcard_hal_cpbr(int slot, char *name, int name_length, char *number, int number_length);

/*!
 * \brief   update phonebook with specified number
 *
 * \param   [IN] slot: phonbook entry index
 *          [IN] number: pointer to number
 *
 * \retval  Possible returns are:
 *          -1: get failed.
 *           0: set succesful.
 */
int aliyun_iot_smartcard_hal_cpbw(int slot, char *number);
#ifdef __cplusplus
}
#endif

#endif
