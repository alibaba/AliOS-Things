/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */


#ifdef HTTP2_COMM_ENABLED
#include "sdk-testsuites_internal.h"
#include "cut.h"
#include "ut_debug.h"
#include "iot_export_http2.h"
#include "utils_httpc.h"
#include "iot_export.h"
#include "iot_export_file_uploader.h"

#define HTTP2_ONLINE_SERVER_URL_T       "10.101.12.205"
#define HTTP2_ONLINE_SERVER_PORT_T      9999
#define HTTP2_PRODUCT_KEY_T             "a1QWlwJO4Z2"
#define HTTP2_DEVICE_NAME_T             "cHzzrWfxLiRLYHik3sYJ"
#define HTTP2_DEVICE_SECRET_T           "vZxZiUMf3vwTViC3XIxJ2ZHt0zVoZlRa"

CASE(HTTP2, http2_client_case1) {
    httpclient_t client;
    http2_connection_t *handler = NULL;

    memset(&client, 0, sizeof(httpclient_t));
    handler = iotx_http2_client_connect((void *)&client, "http://10.101.12.205/", HTTP2_ONLINE_SERVER_PORT_T);
    if (handler == NULL) {
        ut_err("handler is NULL, can't connect\n");
    } else {
        ut_info("connected\n");
    }
    ASSERT_NULL(handler);

}

CASE(HTTP2, http2_client_case2) {
    httpclient_t client;
    http2_connection_t *handler = NULL;
    file_sync_info file_info;
    char file_id[MAX_HTTP2_FILE_ID_LEN] = {0};
    char store_id[MAX_HTTP2_FILE_STORE_LEN] = {0};
    int pos = 1;
    int ret = -1;

    memset(&client, 0, sizeof(httpclient_t));
    handler = iotx_http2_client_connect((void *)&client, HTTP2_ONLINE_SERVER_URL_T, HTTP2_ONLINE_SERVER_PORT_T);
    if (handler == NULL) {
        ut_err("handler is NULL, can't connect\n");
    } else {
        ut_info("connected\n");
    }

    file_info.file_name = (char *)"/home/robin/codebase/release/iotx-sdk-c/README.md";
    file_info.file_id = file_id;
    file_info.store_id = store_id;
    file_info.current_pos = &pos;
    file_info.type = 0;
    ret = iotx_upload_file(handler, &file_info);
    ut_info("ret = %d\n", ret);
    ASSERT_NE(ret, HTTP2_UPLOAD_FILE_RET_OK);
}

CASE(HTTP2, http2_client_case3) {
    httpclient_t client;
    http2_connection_t *handler = NULL;
    file_sync_info file_info;
    char file_id[MAX_HTTP2_FILE_ID_LEN] = {0};
    char store_id[MAX_HTTP2_FILE_STORE_LEN] = {0};
    int pos = 1;
    int ret = -1;

    memset(&client, 0, sizeof(httpclient_t));
    handler = iotx_http2_client_connect((void *)&client, HTTP2_ONLINE_SERVER_URL_T, HTTP2_ONLINE_SERVER_PORT_T);
    if (handler == NULL) {
        ut_err("handler is NULL, can't connect\n");
    } else {
        ut_info("connected\n");
    }
    ASSERT_NOT_NULL(handler);

    iotx_http2_set_device_info(HTTP2_PRODUCT_KEY_T, HTTP2_DEVICE_NAME_T, HTTP2_DEVICE_SECRET_T);

    file_info.file_name = (char *)"/home/robin/codebase/release/iotx-sdk-c/README.md";
    file_info.file_id = file_id;
    file_info.store_id = store_id;
    file_info.current_pos = &pos;
    file_info.type = 0;
    ret = iotx_upload_file(handler, &file_info);
    ut_info("ret = %d\n", ret);
    ASSERT_EQ(ret, HTTP2_UPLOAD_FILE_RET_OK);
}


SUITE(HTTP2) = {
    ADD_CASE(HTTP2, http2_client_case1),
    ADD_CASE(HTTP2, http2_client_case2),
    ADD_CASE(HTTP2, http2_client_case3),
    ADD_CASE_NULL
};
#endif