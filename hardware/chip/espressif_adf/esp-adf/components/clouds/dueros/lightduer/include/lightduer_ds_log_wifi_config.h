/**
 * Copyright (2018) Baidu Inc. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/*
 * Auth: Chen Xihao(chenxihao@baidu.com)
 * Desc: wifi config related report log
 */

#ifndef BAIDU_DUER_LIGHTDUER_DS_LOG_WIFI_CONFIG_H
#define BAIDU_DUER_LIGHTDUER_DS_LOG_WIFI_CONFIG_H

#include "baidu_json.h"
#include "lightduer_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum _duer_ds_log_wifi_cfg_code {
    DUER_DS_LOG_WIFI_CFG_START          = 0x101,
    DUER_DS_LOG_WIFI_CFG_SCAN_RESLUTS   = 0x102,
    DUER_DS_LOG_WIFI_CFG_LOCKED         = 0x103,
    DUER_DS_LOG_WIFI_CFG_CONNECTING     = 0x104,
    DUER_DS_LOG_WIFI_CFG_CONNECTED      = 0x105,
    DUER_DS_LOG_WIFI_CFG_FINISHED       = 0x106,

    DUER_DS_LOG_WIFI_CFG_TIMEOUT        = 0x401,
    DUER_DS_LOG_WIFI_CFG_FAILED         = 0x402,
} duer_ds_log_wifi_cfg_code_t;

typedef enum _duer_wifi_cfg_err_code {
    DUER_WIFI_CFG_SUCCESS           = 0,
    DUER_WIFI_CFG_STOP              = 1,
    DUER_WIFI_CFG_ERR_NORMAL        = 2,
    DUER_WIFI_CFG_ERR_NO_MEMORY     = 3,
    DUER_WIFI_CFG_ERR_SSID_INVALID  = 4,
    DUER_WIFI_CFG_ERR_HANDSHARK     = 5,
} duer_wifi_cfg_err_code_t;

typedef enum _duer_wifi_cfg_method {
    DUER_WIFI_CFG_UNKOWN            = 0,
    DUER_WIFI_CFG_AIRKISS           = 1,
    DUER_WIFI_CFG_SMART_CONFIG      = 2,
    DUER_WIFI_CFG_SONIC             = 3,
    DUER_WIFI_CFG_BLUE_TOOTH        = 4,
} duer_wifi_cfg_method_t;

#define CHECK_WIFI_CFG_ID() \
    if (duer_ds_log_wifi_cfg_get_id() == 0) { \
        DUER_LOGE("should call duer_ds_log_wifi_cfg_start first"); \
        return DUER_ERR_FAILED; \
    }

/**
 * common function to report the log of wifi config
 */
duer_status_t duer_ds_log_wifi_cfg(duer_ds_log_wifi_cfg_code_t log_code,
                                   const baidu_json *message);

/**
 * report the log that wifi config start
 */
duer_status_t duer_ds_log_wifi_cfg_start(void);

/**
 * report the message for log code DUER_DS_LOG_WIFI_CFG_SCAN_RESLUTS
 * {
 *     "list": [
 *         "SSID名称1", "SSID名称2", ...
 *     ],
 * }
 */
duer_status_t duer_ds_log_wifi_cfg_scan_resluts(const char **ssids, duer_size_t num);

/**
 * report the log that wifi config finished
 */
duer_status_t duer_ds_log_wifi_cfg_finished(duer_wifi_cfg_err_code_t code,
                                            duer_wifi_cfg_method_t method);

/**
 * report the log that wifi config failed
 */
duer_status_t duer_ds_log_wifi_cfg_failed(duer_wifi_cfg_err_code_t err_code, int rssi);

/**
 * get the id of wifi config
 */
int duer_ds_log_wifi_cfg_get_id(void);

#ifdef __cplusplus
}
#endif

#endif // BAIDU_DUER_LIGHTDUER_DS_LOG_WIFI_CONFIG_H
