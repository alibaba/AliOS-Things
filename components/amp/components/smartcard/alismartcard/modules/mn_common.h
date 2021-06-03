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
 * Description:
 * mn_common.h: module common header file
 *
 */
#ifndef __MN_COMMON_H__
#define __MN_COMMON_H__

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

#include "log.h"
#include "aliyun_iot_smartcard_export.h"
#include "aliyun_iot_smartcard_internal.h"
#include "aliyun_iot_smartcard_os_hal.h"

#define MN_COMMON_TAG "[MN]"

#define mn_log_err(fmt, ...)   log_e(MN_COMMON_TAG, fmt"\r\n", ##__VA_ARGS__)
#define mn_log_fatal(fmt, ...) log_f(MN_COMMON_TAG, fmt"\r\n", ##__VA_ARGS__)
#define mn_log_warn(fmt, ...)  log_w(MN_COMMON_TAG, fmt"\r\n", ##__VA_ARGS__)
#define mn_log_info(fmt, ...)  log_i(MN_COMMON_TAG, fmt"\r\n", ##__VA_ARGS__)
#define mn_log_dbg(fmt, ...)   log_d(MN_COMMON_TAG, fmt"\r\n", ##__VA_ARGS__)

/*!
 * \brief   switch to next card and update sim info
 *
 * \retval  Possible returns are:
 *          MN_ERROR_REG_FAIL: reg fail
 *          MN_ERROR_SAME_CARD: slot is same after switch
 *          -1: get failed.
 *           0: set succesful.
 *
 * \attention  This function will reboot module if necessary
 *
 */
int mn_hal_switch(aliyun_iot_smartcard_type_t switch_type);

/*!
 * \brief   parse MN configs
 *
 * \param   [OUT] changed flag
 *
 * \retval  Possible returns are:
 *          -1: get failed.
 *           0: set succesful.
 */
int mn_parse_configs(const char *file_path);

/*!
 * \brief   get log dir
 *
 * \retval  Possible returns are:
 *          pointer to log dir
 */
char *mn_get_log_dir(void);

/*!
 * \brief  get carrier type and carrier name from iccid
 *
 * \param   [IN] iccid
 * \param   [OUT] car_type: carrier type
 * \param   [OUT] car_name: carrier name
 *
 * \retval  Possible returns are:
 *          -1: get failed.
 *           0: set succesful.
 */
int mn_parse_carrier_from_iccid(const char *iccid, aliyun_iot_smartcard_carrier_type_t *car_type, char *car_name);
#endif /* MN_COMMON_H */
