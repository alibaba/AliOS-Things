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
// Author: Su Hao (suhao@baidu.com)
//
// Description: The common type definitions.

#ifndef BAIDU_IOT_TINYDU_IOT_OS_SRC_IOT_BAIDU_CA_INCLUDE_BAIDU_CA_TYPES_H
#define BAIDU_IOT_TINYDU_IOT_OS_SRC_IOT_BAIDU_CA_INCLUDE_BAIDU_CA_TYPES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Declaration for local varaible & function.
#define DUER_LOC         static
// Definition for local varaible & function.
#define DUER_LOC_IMPL    static

// Declaration for internal varaible & function.
#define DUER_INT         extern
// Definition for internal varaible & function.
#define DUER_INT_IMPL

// Declaration for external varaible & function.
#define DUER_EXT         extern
// Definition for external varaible & function.
#define DUER_EXT_IMPL

/*
 * The ca handler
 */
typedef void*          duer_handler;

typedef void*          duer_context;
typedef int            duer_status_t;
#ifdef __GNUC__
typedef size_t   duer_size_t;
#else
typedef unsigned int   duer_size_t;
#endif
typedef unsigned int   duer_u32_t;
typedef signed int     duer_s32_t;
typedef unsigned short duer_u16_t;
typedef signed int     duer_s16_t;
typedef unsigned char  duer_u8_t;
typedef signed char    duer_s8_t;
typedef char           duer_bool;

typedef duer_status_t (*duer_data_callback)(duer_context, const void *, duer_size_t);

enum _baidu_ca_bool_e {
    DUER_FALSE,
    DUER_TRUE
};

/*
 * The error codes.
 */
typedef enum _duer_errcode_enum {
    DUER_OK,
    DUER_ERR_FAILED                  = -0x0001,
    DUER_ERR_CONNECT_TIMEOUT         = -0x0002,
    DUER_CANCEL                      = -0x0003,
    DUER_INF_TRANS_IP_BY_HTTP_DNS    = -0x0004, // IP got through http dns

    /* Generic Errors */
    DUER_ERR_INVALID_PARAMETER       = -0x0010,
    DUER_ERR_MEMORY_OVERLOW          = -0x0011,
    DUER_ERR_PROFILE_INVALID         = -0x0012,
    DUER_ERR_HAS_STARTED             = -0x0013,
    DUER_ERR_CA_NOT_CONNECTED        = -0x0014,
    DUER_ERR_HAS_STOPPED             = -0x0015,

    /* Network Errors */
    DUER_ERR_TRANS_INTERNAL_ERROR    = -0x0030,
    DUER_ERR_TRANS_WOULD_BLOCK       = -0x0031,
    DUER_ERR_TRANS_TIMEOUT           = -0x0032, // send timeout
    DUER_ERR_REG_FAIL                = -0x0033,
    DUER_ERR_TRANS_DNS_FAIL          = -0x0034,
    DUER_ERR_WIFI_SIGNAL_WEAK        = -0x0035,
    DUER_ERR_WIFI_DISCONNECTED       = -0x0036,

    DUER_ERR_MBEDTLS_NET_MAX         = -0x0040,
    // -0x0040 - -0x0060 reserved for mbed net error code , see mbedtls/net.h for more info
    DUER_ERR_MBEDTLS_NET_MIN         = -0x0060,
    DUER_ERR_MBEDTLS_SSL_MAX         = -0x6000,
    // -0x6000 - -0x8000 reserved for ssl error code , see ssl.h for more info
    DUER_ERR_MBEDTLS_SSL_MIN         = -0x8000,

    DUER_ERR_REPORT_FAILED           = -0x070000,
    DUER_ERR_REPORT_FAILED_BEGIN     = DUER_ERR_REPORT_FAILED + 0x10000
} duer_errcode_t;

#endif // BAIDU_IOT_TINYDU_IOT_OS_SRC_IOT_BAIDU_CA_INCLUDE_BAIDU_CA_TYPES_H
