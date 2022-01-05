/**
 * Copyright (2017) Baidu Inc. All rights reserved.
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
 * Auth: Leliang Zhang(zhangleliang@baidu.com)
 * Desc: device log report
 */

#ifndef BAIDU_DUER_LIGHTDUER_DS_LOG_H
#define BAIDU_DUER_LIGHTDUER_DS_LOG_H

#include "baidu_json.h"
#include "lightduer_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum _duer_ds_log_version_enum {
    DUER_DS_LOG_VERSION_UNKNOWN  = 0x00,
    DUER_DS_LOG_VERSION_1_0      = 0x01,
    DUER_DS_LOG_VERSION_2_0      = 0x02,
    DUER_DS_LOG_VERSION_MAX      = 0x08
} duer_ds_log_version_enum_t;

typedef enum _duer_ds_log_level_enum {
    DUER_DS_LOG_LEVEL_FATAL = 0x01,
    DUER_DS_LOG_LEVEL_ERROR = 0x02,
    DUER_DS_LOG_LEVEL_WARN  = 0x03,
    DUER_DS_LOG_LEVEL_INFO  = 0x04,
    DUER_DS_LOG_LEVEL_DEBUG = 0x05,
    DUER_DS_LOG_LEVEL_MAX   = 0x08
} duer_ds_log_level_enum_t;

typedef enum {
    DUER_DS_LOG_IMPORTANCE_NORMAL      = 0x00,
    DUER_DS_LOG_IMPORTANCE_MUST_REPORT = 0x01,
} duer_ds_log_importance_enum_t;

#ifndef DUER_DS_LOG_DEFAULT_REPORT_LEVEL
#define DUER_DS_LOG_DEFAULT_REPORT_LEVEL DUER_DS_LOG_LEVEL_INFO
#endif

#ifndef DUER_DS_LOG_DEFAULT_CACHE_LEVEL
#define DUER_DS_LOG_DEFAULT_CACHE_LEVEL DUER_DS_LOG_DEFAULT_REPORT_LEVEL
#endif

/*
 * update the report level,
 *        DUER_DS_LOG_LEVEL_FATAL <= log_level <= DUER_DS_LOG_LEVEL_DEBUG
 *        if < DUER_DS_LOG_LEVEL_FATAL, set to DUER_DS_LOG_LEVEL_FATAL
 *        if > DUER_DS_LOG_LEVEL_DEBUG, set to DUER_DS_LOG_LEVEL_DEBUG
 * @param log_level
 * @return DUER_OK on success, other fail
 *
 */
duer_status_t duer_ds_log_set_report_level(duer_ds_log_level_enum_t log_level);

/**
 * init the control point for setting report level
 */
void duer_ds_log_init(void);

typedef enum _duer_ds_log_module_enum {
    DUER_DS_LOG_MODULE_CA       = 0x01,
    DUER_DS_LOG_MODULE_RECORDER = 0x02,
    DUER_DS_LOG_MODULE_MEDIA    = 0x03,
    DUER_DS_LOG_MODULE_HTTP     = 0x04,
    DUER_DS_LOG_MODULE_DCS      = 0x05,
    DUER_DS_LOG_MODULE_APP      = 0x06,
    DUER_DS_LOG_MODULE_WIFICFG  = 0x07,
    DUER_DS_LOG_MODULE_SYSTEM   = 0x08,
    DUER_DS_LOG_MODULE_ANALYSIS = 0x09,
    DUER_DS_LOG_MODULE_BIND     = 0x0A,
    DUER_DS_LOG_MODULE_SONIC    = 0x0B,
    DUER_DS_LOG_MODULE_SMARTCFG = 0x0C,
    DUER_DS_LOG_MODULE_AIRKISS  = 0x0D,
    DUER_DS_LOG_MODULE_PM       = 0x0E,
    DUER_DS_LOG_MODULE_MAX      = 0x20
} duer_ds_log_module_enum_t;

typedef enum _duer_ds_log_family_enum {
    DUER_DS_LOG_FAMILY_UNKNOWN  = 0x00,
    DUER_DS_LOG_FAMILY_MEMORY   = 0x01,
    DUER_DS_LOG_FAMILY_NETWORK  = 0x02,
    DUER_DS_LOG_FAMILY_MAX      = 0x0F
} duer_ds_log_family_enum_t;

/**
 *  {
 *      "duer_trace_info": {
 *          "code": [打点信息],
 *          "message": [详细信息], // 可选
 *          "ts": [当前时间戳]
 *      }
 *  }
 * the code format:
 *   0               1               2               3
 *   0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |X| Ver |X| Lel |X X X|  Module | Family|      Code             |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 */

#define MASK_VERSION   0x70000000
#define BITS_VERSION   0x07
#define OFFSET_VERSION 28
#define MASK_LEVEL     0x07000000
#define BITS_LEVEL     0x07
#define OFFSET_LEVEL   24
#define MASK_MODULE    0x001F0000
#define BITS_MODULE    0x1F
#define OFFSET_MODULE  16
#define MASK_FAMILY    0x0000F000
#define BITS_FAMILY    0x0F
#define OFFSET_FAMILY  12
#define MASK_CODE      0x00000FFF
#define BITS_CODE      0x0FFF
#define OFFSET_CODE    0

/**
  * generate the code in the above format from the input parameter
  * @param log_version
  * @param log_level
  * @param log_module
  * @param log_family
  * @param log_code
  * @return return the code in the above format
  */
duer_u32_t duer_ds_log_generate_code(duer_ds_log_version_enum_t log_version,
                                     duer_ds_log_level_enum_t   log_level,
                                     duer_ds_log_module_enum_t  log_module,
                                     duer_ds_log_family_enum_t  log_family,
                                     int log_code);

/**
 * @param log_code the code in the above format which is 32bits
 * @return the version
 */
duer_u32_t duer_ds_log_get_log_version(duer_u32_t log_code);

/**
 * @param log_code the code in the above format which is 32bits
 * @return the level
 */
duer_u32_t duer_ds_log_get_log_level(duer_u32_t log_code);

/**
 * @param log_code the code in the above format which is 32bits
 * @return the module
 */
duer_u32_t duer_ds_log_get_log_module(duer_u32_t log_code);

/**
 * @param log_code the code in the above format which is 32bits
 * @return the family
 */
duer_u32_t duer_ds_log_get_log_family(duer_u32_t log_code);

/**
 * @param log_code the code in the above format which is 32bits
 * @return the code
 */
duer_u32_t duer_ds_log_get_log_code(duer_u32_t log_code);

/**
 *  #param log_version the version of the log
 *  @param log_level the log level, Fatal, Error, Warn, Info, Debug
 *  @param log_module the module code
 *  @param log_family the family of this log
 *  @param log_code the code for this info, maybe use to parse the content of message
 *                  only the low 12bits use
 *  @param log_message the content of the info, in json format
 *         the caller should not try to release this if exist.
 *  @return DUER_OK on success, or fail
 *  an example:
 *  {
 *      "duer_trace_info": {
 *          "code": 2001, //
 *          "message": {
 *              "reason": "REBOOT",
 *              "count": 1
 *          },
 *          "ts": 220509360
 *      }
 *  }
 */
duer_status_t duer_ds_log_v_f(duer_ds_log_version_enum_t log_version,
                              duer_ds_log_level_enum_t   log_level,
                              duer_ds_log_module_enum_t  log_module,
                              duer_ds_log_family_enum_t  log_family,
                              int log_code,
                              const baidu_json *log_message,
                              duer_ds_log_importance_enum_t importance);

duer_status_t duer_ds_log_f(duer_ds_log_level_enum_t   log_level,
                            duer_ds_log_module_enum_t  log_module,
                            duer_ds_log_family_enum_t  log_family,
                            int log_code,
                            const baidu_json *log_message,
                            duer_ds_log_importance_enum_t importance);

duer_status_t duer_ds_log(duer_ds_log_level_enum_t   log_level,
                          duer_ds_log_module_enum_t  log_module,
                          int log_code,
                          const baidu_json *log_message);

duer_status_t duer_ds_log_important(duer_ds_log_level_enum_t   log_level,
                                    duer_ds_log_module_enum_t  log_module,
                                    int log_code,
                                    const baidu_json *log_message);

#ifdef __cplusplus
}
#endif

#endif // BAIDU_DUER_LIGHTDUER_DS_LOG_H
