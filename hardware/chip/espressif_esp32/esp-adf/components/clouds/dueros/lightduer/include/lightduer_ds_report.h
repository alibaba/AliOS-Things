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
 * Desc: device status report
 */

#ifndef BAIDU_DUER_LIGHTDUER_DS_REPORT_H
#define BAIDU_DUER_LIGHTDUER_DS_REPORT_H

#include "baidu_json.h"
#include "lightduer_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * the return baidu_json* will be handled by the caller
 * so the callee should not try to release the baidu_json*
 */
typedef baidu_json* (*duer_ds_report_function_t)(void);

typedef enum _duer_ds_report_type_enum {
    DUER_DS_ONCE,         // report only once
    DUER_DS_PERIODIC,     // report periodically
    DUER_DS_TOTAL
} duer_ds_report_type_enum_t;

/**
 *  all the register functions will be invoked when try to report the device status.
 *  all the modules which have status to report should call this to register the
 *  functions.
 */
duer_status_t duer_ds_register_report_function(duer_ds_report_function_t report_function);

/**
 *  initiate a device status report, DUER_DS_ONCE or DUER_DS_PERIODIC decided by type
 *  if the periodic timer already started, stop it , restart it with the new interval
 *  @param type, DUER_DS_ONCE or DUER_DS_PERIODIC
 *  @param interval, if the report type is DUER_DS_PERIODIC, then this indicate the interval
 *                   0, means use the default value, now is 5min
 *  @return, DUER_OK on success, or failed
 *  report format:
 *          duer_device_status contains an array
 *          all the info collect from the functions registed is one item in the array.
 *  {
 *      "duer_device_status": [
 *          {
 *              "avg_connect_time": "500ms",
 *              "disconnect_count": 0
 *          },
 *          {
 *              "upload_size": "10Bytes",
 *              "down_size": "20Bytes"
 *          }
 *      ]
 *  }
 */
duer_status_t duer_ds_report_start(duer_ds_report_type_enum_t type, duer_u32_t interval);

/**
 *  try to stop the periodic report.
 */
duer_status_t duer_ds_report_stop(void);

/**
 *  try to destroy report list.
 */
duer_status_t duer_ds_report_destroy(void);

#ifdef __cplusplus
}
#endif

#endif // BAIDU_DUER_LIGHTDUER_DS_REPORT_H
