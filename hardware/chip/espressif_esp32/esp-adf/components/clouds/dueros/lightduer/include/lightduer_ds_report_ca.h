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
 * Desc: device status ca report
 */

#ifndef BAIDU_DUER_LIGHTDUER_DS_REPORT_CA_H
#define BAIDU_DUER_LIGHTDUER_DS_REPORT_CA_H

#include "baidu_json.h"
#include "lightduer_types.h"

#ifdef __cplusplus
extern "C" {
#endif

duer_status_t duer_ds_report_ca_update_avg_latency(duer_u32_t al);

duer_status_t duer_ds_report_ca_update_send_max_packet(duer_u32_t smp);

duer_status_t duer_ds_report_ca_inc_valid_recv(void);

duer_status_t duer_ds_report_ca_inc_recv_count(void);

duer_status_t duer_ds_report_ca_update_recv_max_packet(duer_u32_t rmp);

duer_status_t duer_ds_report_ca_inc_error_mbedtls(void);

duer_status_t duer_ds_report_ca_inc_error_tcpheader(void);

duer_status_t duer_ds_report_ca_inc_error_coap(void);

duer_status_t duer_ds_report_ca_inc_error_connect(void);


/**
 * generate the ca status report
 * this function used in @duer_ds_register_report_function
 * "ca_status": {
 *     "connection": {
 *         "disconnect_time": 100,
 *         "max_connect": 200,
 *         "reconnect_count": 2
 *     },
 *     "send": {
 *         "avg_latency": 20,
 *         "max_packet": 1900
 *     },
 *     "recv": {
 *         "valid_recv": 800,
 *         "recv_count": 1000,
 *         "max_packet": 1900
 *     },
 *     "error": {
 *         "mbedtls": 0,
 *         "tcp_header": 0,
 *         "coap": 0,
 *         "connect_fail": 0
 *     }
 * }
 */
baidu_json* duer_ds_report_ca_status(void);

#ifdef __cplusplus
}
#endif

#endif // BAIDU_DUER_LIGHTDUER_DS_REPORT_CA_H
