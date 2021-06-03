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
 * module_common.h: module common api
 *
 */

#ifndef __MODULE_COMMON_H__
#define __MODULE_COMMON_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "log.h"

#include "aliyun_iot_smartcard_export.h"

#define MODULE_COMMON_TAG "[MODULE]"

#define HOST_DRIVER_TYPE_QMI_WWAN    0
#define HOST_DRIVER_TYPE_QMI_GOBINET 1
#define HOST_DRIVER_TYPE_ECM         2
#define HOST_DRIVER_TYPE_RNDIS       3
#define HOST_USBNET_TYPE             0

#define DEFAULT_HOST_QMICM_NAME "MN-CM"
#define DEFAULT_HOST_ETH_INTERFACE_NAME "wwan0"
#define DEFAULT_MODULE_AT_PORT "/dev/ttyUSB2"
#define DEFAULT_MODULE_AT_BAUDRATE 115200

#ifdef HOST_ANDROID_ENABLED
#define LOG_DIR "/sdcard/MNLog"
#define SMARTCARD_CONF_FILE "/sdcard/ali_smartcard.json"
#define HOST_CMD_PREFIX "/system/bin/busybox "
//#define HOST_RILD_ENABLED
#define HOST_AM_BOARDCAST
#else
#define HOST_CMD_PREFIX ""
#define LOG_MODULE_NAME "MNLog"
#define SMARTCARD_CONF_FILE "U:/ali_smartcard.json"
#endif

#define module_log_err(fmt, ...)   log_e(MODULE_COMMON_TAG, fmt"\r\n", ##__VA_ARGS__)
#define module_log_fatal(fmt, ...) log_f(MODULE_COMMON_TAG, fmt"\r\n", ##__VA_ARGS__)
#define module_log_warn(fmt, ...)  log_w(MODULE_COMMON_TAG, fmt"\r\n", ##__VA_ARGS__)
#define module_log_info(fmt, ...)  log_i(MODULE_COMMON_TAG, fmt"\r\n", ##__VA_ARGS__)
#define module_log_dbg(fmt, ...)   log_d(MODULE_COMMON_TAG, fmt"\r\n", ##__VA_ARGS__)

#define ARGC_LIMIT 16

/* at cmd error code definitions */
#define MODULE_AT_SELECT_ERR (-1)
#define MODULE_AT_TIMEOUT    (-2)
#define MODULE_AT_READ_ERR   (-3)
#define MODULE_AT_EXEC_ERR   (-4)
#define MODULE_AT_WRITE_ERR  (-5)

/**
 * @brief send at command
 *
 * @param [in] at_cmd: at cmd str
 * @param [in] final_rsp:  final resp str
 * @param [out] str_resp: resp str
 * @param [in] resp_len: resp str len
 * @param [in] timeout: command timeout in ms
 * @return  0 on success, negative error on failure.
 * @see None.
 * @note None.
 */
int aliyun_iot_smartcard_hal_send_at(const char *at_cmd, const char *final_rsp, char *str_resp, int resp_len, long timeout_ms);
#ifdef __cplusplus
}
#endif

#endif
