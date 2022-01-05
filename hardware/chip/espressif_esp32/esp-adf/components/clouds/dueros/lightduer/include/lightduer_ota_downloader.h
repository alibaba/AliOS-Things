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
/**
 * File: lightduer_ota_downloader.h
 * Auth: Zhong Shuai (zhongshuai@baidu.com)
 * Desc: OTA Downloader Head File
 */

#ifndef BAIDU_DUER_LIGHTDUER_INCLUDE_LIGHTDUER_OTA_DOWNLOADER_H
#define BAIDU_DUER_LIGHTDUER_INCLUDE_LIGHTDUER_OTA_DOWNLOADER_H

#include "lightduer_mutex.h"
#include "lightduer_http_client.h"

#ifndef URL_LEN
#define URL_LEN 301
#endif

#define ERR_MSG_LEN (60)

typedef enum _duer_downloader_Protocol {
    HTTP = 0,
    COAP = 1,
    LOCAL = 2,
    MAX_PROTOCOL_COUNT = 3,
} duer_downloader_protocol;

typedef struct _duer_ota_downloader_s {
    duer_downloader_protocol dp;
    void *private_data;              // Set the private data you want to pass
    duer_mutex_t lock;
    char url[URL_LEN + 1];
    struct _duer_ota_downloader_ops_s *ops;
    char err_msg[ERR_MSG_LEN];
} duer_ota_downloader_t ;

typedef int (*data_handler)(
        void* private_data,
        const char *buf,
        size_t len);

/*
 * We need to consider unblock & timeout situation (TBD)
 * connect_server callback function requires you that
 * connect to server and receive the data immediately
 * I think the abstract is not very good. Refactor (TBD)
 */
typedef struct _duer_ota_downloader_ops_s {
    int (*init)(duer_ota_downloader_t *downloader);
    int (*register_data_notify)(duer_ota_downloader_t *downloader, data_handler handler, void *private_data);
    int (*connect_server)(duer_ota_downloader_t *downloader, const char *url);
    int (*disconnect_server)(duer_ota_downloader_t *downloader);
    int (*destroy)(duer_ota_downloader_t *downloader);
} duer_ota_downloader_ops_t;

/*
 * Initialise OTA Downloader Module
 *
 * @param void:
 *
 * @return int: Success: DUER_OK
 *              Failed:  Other
 */
extern int duer_init_ota_downloader(void);

/*
 * Uninitialise OTA Downloader Module
 *
 * @param void:
 *
 * @return int: Success: DUER_OK
 *              Failed:  Other
 */
extern int duer_uninit_ota_downloader(void);
/*
 * Create OTA Downloader
 *
 * @param void:
 *
 * @return duer_ota_downloader_t *: Success: duer_ota_downloader_t *
 *                                  Failed:  NULL
 */
extern duer_ota_downloader_t *duer_ota_downloader_create_downloader(void);

/*
 * Initialise OTA Downloader
 *
 * @param downloader: OTA Downloader object
 *
 * @return int: Success: DUER_OK
 *              Failed:  Other
 */
extern int duer_ota_downloader_init_downloader(duer_ota_downloader_t *downloader);

/*
 * Register a OTA Downloader to OTA Downloader module
 *
 * @param downloader: OTA Downloader object
 *
 * @param dp: Download protocol which the downloader support
 *
 * @return int: Success: DUER_OK
 *              Failed:  Other
 */
extern int duer_ota_downloader_register_downloader(
        duer_ota_downloader_t *downloader,
        duer_downloader_protocol dp);

/*
 * Unregister a OTA Downloader from OTA Downloader module
 *
 * @param dp: Download protocol
 *
 * @return int: Success: DUER_OK
 *              Failed:  Other
 */
extern int duer_ota_downloader_unregister_downloader(duer_downloader_protocol dp);

/*
 * Get a OTA Downloader from OTA Downloader module
 *
 * @param dp: Download protocol which you want
 *
 * @return duer_ota_downloader_t *: Success: duer_ota_downloader_t *
 *                                  Failed:  NULL
 */
extern duer_ota_downloader_t *duer_ota_downloader_get_downloader(duer_downloader_protocol dp);

/*
 * Destroy a OTA Downloader, free the memory and unregister it from OTA Downloader module
 *
 * @param downloader: OTA downloader object
 *
 * @return int: Success: DUER_OK
 *              Failed:  Other
 */
extern int duer_ota_downloader_destroy_downloader(duer_ota_downloader_t *downloader);

/*
 * Register the downloader operations to a Downloader object
 * If you want to support a new downloader protocol, you need to implement duer_ota_downloader_ops
 *
 * @param downloader: OTA Downloader object
 *
 * @param downloader_ops:  OTA Downloader operations
 *
 * @return int: Success: DUER_OK
 *              Failed:  Other
 */
extern int duer_ota_downloader_register_downloader_ops(
        duer_ota_downloader_t *downloader,
        duer_ota_downloader_ops_t *downloader_ops);

/*
 * Connect to server to get the data
 * Note: Once you call it, the OTADownloader will download the data from server and pass it to you
 *
 * @param downloader: OTA Downloader object
 *
 * @param url: URL
 *
 * @return int: Success: DUER_OK
 *              Failed:  Other
 */
extern int duer_ota_downloader_connect_server(duer_ota_downloader_t *downloader, const char *url);

/*
 * Disconnect server
 * Close the socket
 *
 * @param downloader: OTA Downloader object
 *
 * @return int: Success: DUER_OK
 *              Failed:  Other
 */
extern int duer_ota_downloader_disconnect_server(duer_ota_downloader_t *downloader);

/*
 * Register a callback functions "hancler" to receive the data which get from URL
 *
 * @param downloader: OTA Downloader object
 *
 * @param handler: Callback function to receive the data which get form URL
 *
 * @param private: param which you want to pass to handler
 *
 * @return int: Success: DUER_OK
 *              Failed:  Other
 */
extern int duer_ota_downloader_register_data_notify(
        duer_ota_downloader_t *downloader,
        data_handler handler,
        void *private_data);

/*
 * Set the private data you want pass to the OTA downloader OPS
 *
 * @param downloader: OTA Downloader object
 *
 * @param private_data: data which you want to pass
 *
 * @return int: Success: DUER_OK
 *              Failed:  Other
 */
extern int duer_ota_downloader_set_private_data(
        duer_ota_downloader_t *downloader,
        void *private_data);

/*
 * Get the private data
 *
 * @param downloader: OTA Downloader object
 *
 * @return void *: Success: Other
 *                 Failed:  NULL
 */
extern void *duer_ota_downloader_get_private_data(duer_ota_downloader_t *downloader);

/*
 * Report error message
 *
 * @param downloader: OTA Downloader
 *           err_msg: error message
 *          err_code: error code
 *
 * @return void:
 */
extern void duer_ota_downloader_report_err(
        duer_ota_downloader_t *downloader,
        char const *err_msg,
        int err_code);
/*
 * Get error message
 *
 * @param downloader: downloader object
 *
 * @return: Success: error message
 *           Failed: NULL
 */
extern char const *duer_ota_downloader_get_err_msg(duer_ota_downloader_t const *downloader);

/*
 * Check wheater the error message exists
 *
 * @param downloader: downloader object
 *
 * @return: Success: 1
 *           Failed: -1
 */
extern int duer_ota_downloader_check_err_msg(duer_ota_downloader_t *downloader);

#endif // BAIDU_DUER_LIGHTDUER_INCLUDE_LIGHTDUER_OTA_DOWNLOADER_H
