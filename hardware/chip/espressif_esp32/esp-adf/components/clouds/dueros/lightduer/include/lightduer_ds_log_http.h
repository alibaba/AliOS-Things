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
 * Auth: Gang Chen(chengang12@baidu.com)
 * Desc: http module log report
 */

#ifndef BAIDU_DUER_LIGHTDUER_DS_LOG_HTTP_H
#define BAIDU_DUER_LIGHTDUER_DS_LOG_HTTP_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lightduer_types.h"
#include "stdbool.h"
#include "baidu_json.h"

typedef enum {
    DUER_DS_LOG_HTTP_INIT                          =  0x101,
    DUER_DS_LOG_HTTP_DESTROYED                     =  0x102,
    DUER_DS_LOG_HTTP_COMMON_ERROR                  =  0x103,
    DUER_DS_LOG_HTTP_PERSISTENT_CONN_TIMEOUT       =  0x104,
    DUER_DS_LOG_HTTP_DOWNLOAD_STARTED              =  0x105,
    // Attention: 0x106, 0x107, 0x201 must be reserved, as they had been used in old version SDK

    DUER_DS_LOG_HTTP_ZERO_BYTE_DOWNLOAD            =  0x202,
    DUER_DS_LOG_HTTP_DOWNLOAD_STOPPED              =  0x203,
    DUER_DS_LOG_HTTP_DOWNLOAD_FINISHED             =  0x204,

    DUER_DS_LOG_HTTP_SOCKET_CONN_FAILED            =  0x301,
    DUER_DS_LOG_HTTP_URL_PARSE_FAILED              =  0x302,
    DUER_DS_LOG_HTTP_SEND_FAILED                   =  0x303,
    DUER_DS_LOG_HTTP_DOWNLOAD_FAILED               =  0x304,
    DUER_DS_LOG_HTTP_INIT_FAILED                   =  0x305,
    DUER_DS_LOG_HTTP_CONNECT_CLOSED_BY_SERVER      =  0x306,
    DUER_DS_LOG_HTTP_REQUEST_SEND_FAILED           =  0x307,
    DUER_DS_LOG_HTTP_HEADER_SEND_FAILED            =  0x308,
    DUER_DS_LOG_HTTP_RESPONSE_RECEIVE_FAILED       =  0x309,
    DUER_DS_LOG_HTTP_HEADER_RECEIVE_FAILED         =  0x30a,
    DUER_DS_LOG_HTTP_CHUNK_SIZE_RECEIVE_FAILED     =  0x30b,
    DUER_DS_LOG_HTTP_CHUNK_RECEIVE_FAILED          =  0x30c,
    DUER_DS_LOG_HTTP_CONTENT_RECEIVE_FAILED        =  0x30d,
    DUER_DS_LOG_HTTP_MEMORY_ERROR                  =  0x30e,
    DUER_DS_LOG_HTTP_PARAM_ERROR                   =  0x310,
    DUER_DS_LOG_HTTP_RECEIVE_FAILED                =  0x311,
    DUER_DS_LOG_HTTP_REDIRECT_FAILED               =  0x312,
    DUER_DS_LOG_HTTP_DNS_GET_IP                    =  0x313,
    DUER_DS_LOG_HTTP_TOO_MANY_REDIRECT             =  0x314,

    DUER_DS_LOG_HTTP_SOCKET_INIT_FAILED            =  0x401,
    DUER_DS_LOG_HTTP_SOCKET_OPEN_FAILED            =  0x402,
} duer_ds_log_http_code_t;

// used to statistic the download info when download finished/stopped/failed
typedef struct {
    const char *url;
    int        ret_code;
    size_t     upload_size;
    size_t     download_size;
    int        download_speed;
    int        resume_count;
    size_t     content_len;
    bool       is_chunked;
} duer_ds_log_http_statistic_t;

/**
 * Report ds log for http module
 */
duer_status_t duer_ds_log_http(duer_ds_log_http_code_t log_code, const baidu_json *log_message);
/**
 * Report ds log with direction information:
 *        {
 *            "file" : "http.c",
 *            "line" : 100
 *        }
 */
duer_status_t duer_ds_log_http_report_with_dir(duer_ds_log_http_code_t log_code,
                                              const char *file,
                                              int line);
/**
 * Report ds log when http download function is exit, it include the statistic information.
 */
duer_status_t duer_ds_log_http_download_exit(duer_ds_log_http_code_t log_code,
                                             duer_ds_log_http_statistic_t *statistic);
/**
 * Report ds log when http persistent is timeout and be closed.
 */
duer_status_t duer_ds_log_http_persisten_conn_timeout(const char *host);

/**
 * Report ds log with url:
 *        {
 *            "url" : "baidu.com"
 *        }
 */
duer_status_t duer_ds_log_http_report_with_url(duer_ds_log_http_code_t log_code, const char *url);

/**
 * Report ds log with error code:
 *        {
 *            "err_code" : -3007
 *        }
 */
duer_status_t duer_ds_log_http_report_err_code(duer_ds_log_http_code_t log_code, int err_code);

/**
 * Report ds log when redirect failed, it include the redirect location:
 *        {
 *            "location" : "http://test.com"
 *        }
 */
duer_status_t duer_ds_log_http_redirect_fail(const char *location);

/**
 * Report ds log when redirect too many times, it include the redirect count:
 *        {
 *            "redirect_count" : 31
 *        }
 */
duer_status_t duer_ds_log_http_too_many_redirect(int count);

#define DUER_DS_LOG_REPORT_HTTP_MEMORY_ERROR() \
        duer_ds_log_http_report_with_dir(DUER_DS_LOG_HTTP_MEMORY_ERROR, __FILE__, __LINE__)
#define DUER_DS_LOG_REPORT_HTTP_PARAM_ERROR() \
        duer_ds_log_http_report_with_dir(DUER_DS_LOG_HTTP_PARAM_ERROR, __FILE__, __LINE__)
#define DUER_DS_LOG_REPORT_HTTP_COMMON_ERROR() \
        duer_ds_log_http_report_with_dir(DUER_DS_LOG_HTTP_COMMON_ERROR, __FILE__, __LINE__)

#ifdef __cplusplus
}
#endif

#endif // BAIDU_DUER_LIGHTDUER_DS_LOG_HTTP_H
