/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */


#ifdef HTTP2_COMM_ENABLED
#include "sdk-testsuites_internal.h"

#define HTTP2_ONLINE_SERVER_URL_T       "10.101.12.205"
#define HTTP2_ONLINE_SERVER_PORT_T      9999
#define HTTP2_PRODUCT_KEY_T             "a1QWlwJO4Z2"
#define HTTP2_DEVICE_NAME_T             "cHzzrWfxLiRLYHik3sYJ"
#define HTTP2_DEVICE_SECRET_T           "vZxZiUMf3vwTViC3XIxJ2ZHt0zVoZlRa"

CASE(HTTP2_BASIC, http2_client_case1) {
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




SUITE(HTTP2_BASIC) = {
    ADD_CASE(HTTP2_BASIC, http2_client_case1),
    ADD_CASE_NULL
};
#endif
