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
 * Desc: ca related trace log
 */

#ifndef BAIDU_DUER_LIGHTDUER_DS_LOG_CA_H
#define BAIDU_DUER_LIGHTDUER_DS_LOG_CA_H

#include "baidu_json.h"
#include "lightduer_ds_log.h"
#include "lightduer_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * tracelog for CA module
 * @param log_level
 * @param log_code
 * @param log_message
 * @return
 * more info pls see lightduer_ds_log.h
 */
duer_status_t duer_ds_log_ca(duer_ds_log_level_enum_t log_level,
                             int log_code,
                             const baidu_json *log_message);

/*
 * log code.
 */
typedef enum _duer_ds_log_ca_enum {
    DUER_DS_LOG_CA_INIT                   = 0x001,
    DUER_DS_LOG_CA_START                  = 0x002,
    DUER_DS_LOG_CA_START_STATE_CHANGE     = 0x003,
    DUER_DS_LOG_CA_START_CONNECT          = 0x004,
    DUER_DS_LOG_CA_STARTED                = 0x005,
    DUER_DS_LOG_CA_REGISTER_CONTROL_POINT = 0x006,
    DUER_DS_LOG_CA_CALL_CONTROL_POINT     = 0x007,
    DUER_DS_LOG_CA_STOPPED                = 0x008,
    DUER_DS_LOG_CA_CLEAR_DATA             = 0x009,
    DUER_DS_LOG_CA_HTTP_DNS               = 0x00A,
    DUER_DS_LOG_CA_ERR                    = 0x300,
    DUER_DS_LOG_CA_START_PROFILE_ERR      = 0x301,
    DUER_DS_LOG_CA_CONNECT_DNS_ERR        = 0x302,
    DUER_DS_LOG_CA_CONNECT_IP_ERR         = 0x303,
    DUER_DS_LOG_CA_START_STATE_REG_FAIL   = 0x304,
    DUER_DS_LOG_CA_SEND_ERROR             = 0x305,
    DUER_DS_LOG_CA_RECV_ERROR             = 0x306,
    DUER_DS_LOG_CA_MBEDTLS_ERR            = 0x307,
    DUER_DS_LOG_CA_TCPHEADER_ERR          = 0x308,
    DUER_DS_LOG_CA_COAP_PARSE_ERR         = 0x309,
    DUER_DS_LOG_CA_MALLOC_FAIL            = 0x400,
    DUER_DS_LOG_CA_LOG_CODE_MAX           = 0xFFF
} duer_ds_log_ca_enum_t;

/**
 * tracelog for CA module with FATAL log level
 * @param log_code
 * @param log_message
 * @return
 * more info pls see lightduer_ds_log.h
 */
duer_status_t duer_ds_log_ca_fatal(int log_code,
                                   const baidu_json *log_message);

/**
 * tracelog for CA module with ERROR log level
 * @param log_code
 * @param log_message
 * @return
 * more info pls see lightduer_ds_log.h
 */
duer_status_t duer_ds_log_ca_error(int log_code,
                                   const baidu_json *log_message);

/**
 * tracelog for CA module with WARN log level
 * @param log_code
 * @param log_message
 * @return
 * more info pls see lightduer_ds_log.h
 */
duer_status_t duer_ds_log_ca_warn(int log_code,
                                  const baidu_json *log_message);

/**
 * tracelog for CA module with INFO log level
 * @param log_code
 * @param log_message
 * @return
 * more info pls see lightduer_ds_log.h
 */
duer_status_t duer_ds_log_ca_info(int log_code,
                                  const baidu_json *log_message);

/**
 * tracelog for CA module with DEBUG log level
 * @param log_code
 * @param log_message
 * @return
 * more info pls see lightduer_ds_log.h
 */
duer_status_t duer_ds_log_ca_debug(int log_code,
                                   const baidu_json *log_message);

/**
 * try to cache the log_code
 * @param log_code the error code
 */
duer_status_t duer_ds_log_ca_cache_error(int log_code);

/**
 * @param from old state
 * @param to new state
 * report the message for log code DUER_DS_LOG_CA_START_STATE_CHANGE
 *  {
 *     "from": [旧状态],
 *     "to": [新的状态]
 *  }
 */
duer_status_t duer_ds_log_ca_start_state_change(int from, int to);

/**
 * report the message for log code DUER_DS_LOG_CA_START_CONNECT
 *  {
 *     "host": [连接主机],
 *     "port": [连接端口]
 *  }
 */

duer_status_t duer_ds_log_ca_start_connect(const char *host, int port);

/**
 * report the message for log code DUER_DS_LOG_CA_STARTED
 *  {
 *     "reason": [启动原因],
 *     "count": [启动次数],
 *     "connect_time:" [成功连接花费时间 ms]
 *  }
 */
duer_status_t duer_ds_log_ca_started(duer_status_t reason, int count, duer_u32_t connect_time);

/*
 * control point type
 */
typedef enum _duer_ds_log_ca_controlpoint_type_enum {
    DUER_CA_CP_TYPE_STATIC   = 0x000,
    DUER_CA_CP_TYPE_DYNAMIC  = 0x001
} duer_ds_log_ca_controlpoint_type_enum_t;
/**
 * report the message for log code DUER_DS_LOG_CA_REGISTER_CONTROL_POINT
 *  {
 *     "name": [制点名称],
 *     "type": [STATIC|DYNAMIC]
 *  }
 */
duer_status_t duer_ds_log_ca_register_cp(const char *name,
                                         duer_ds_log_ca_controlpoint_type_enum_t type);

/**
 * report the message for log code DUER_DS_LOG_CA_CALL_CONTROL_POINT
 *  {
 *     "name": [制点名称]
 *  }
 */
duer_status_t duer_ds_log_ca_call_cp(const char *name);

/**
 * report the message for log code DUER_DS_LOG_CA_MBEDTLS_ERR
 *  {
 *     "code": [错误代码]
 *  }
 */
duer_status_t duer_ds_log_ca_mbedtls_error(duer_s32_t code);

/**
 * report the message for log code DUER_DS_LOG_CA_MALLOC_FAIL
 *  {
 *     "file": [出错文件],
       "line": [出错行号]
 *  }
 */
duer_status_t duer_ds_log_ca_malloc_error(const char *file, duer_u32_t line);

#define DUER_DS_LOG_CA_MEMORY_OVERFLOW() duer_ds_log_ca_malloc_error(__FILE__, __LINE__)

#ifdef __cplusplus
}
#endif

#endif // BAIDU_DUER_LIGHTDUER_DS_LOG_CA_H
