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
 * Desc: SmartConfig related report log
 */

#ifndef BAIDU_DUER_LIGHTDUER_DS_LOG_SMART_CONFIG_H
#define BAIDU_DUER_LIGHTDUER_DS_LOG_SMART_CONFIG_H

#include "baidu_json.h"
#include "lightduer_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum _duer_ds_log_smart_cfg_code {
    DUER_DS_LOG_SMART_CFG_START          = 0x201,
    DUER_DS_LOG_SMART_CFG_LOCK_CHANNEL   = 0x202,
    DUER_DS_LOG_SMART_CFG_LEAD_COMPLETED = 0x203,
    DUER_DS_LOG_SMART_CFG_FINISHED       = 0x204,

    DUER_DS_LOG_SMART_CFG_FAILED         = 0x401,
} duer_ds_log_smart_cfg_code_t;

typedef struct duer_ds_log_smart_cfg_info_s {
    duer_size_t packet_counts;
    duer_size_t data_counts;
    duer_size_t data_invalids;
    duer_size_t decode_counts;
} duer_ds_log_smart_cfg_info_t;

/**
 * common function to report the log of smart config
 */
duer_status_t duer_ds_log_smart_cfg(duer_ds_log_smart_cfg_code_t log_code,
                                    const baidu_json *message);

/**
 * report the log that smart config receive all lead code
 */
duer_status_t duer_ds_log_smart_cfg_lead_completed(duer_size_t data_len);

/**
 * report the log that smart config finished
 */
duer_status_t duer_ds_log_smart_cfg_finished(duer_ds_log_smart_cfg_info_t *info);

#ifdef __cplusplus
}
#endif

#endif // BAIDU_DUER_LIGHTDUER_DS_LOG_SMART_CONFIG_H
