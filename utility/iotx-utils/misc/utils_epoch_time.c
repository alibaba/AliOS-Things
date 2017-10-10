/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdlib.h>
//#include <memory.h>

#include "utils_httpc.h"
#include "utils_epoch_time.h"
#include "lite-log.h"

#define HTTP_RESP_CONTENT_LEN   (64)

extern int httpclient_common(httpclient_t *client,
                             const char *url,
                             int port,
                             const char *ca_crt,
                             int method,
                             uint32_t timeout_ms,
                             httpclient_data_t *client_data);

uint64_t utils_get_epoch_time(char copy[], int len)
{
    int ret;
    httpclient_t httpclient;
    httpclient_data_t httpclient_data;
    char http_content[HTTP_RESP_CONTENT_LEN];

    memset(&httpclient, 0, sizeof(httpclient_t));
    memset(&httpclient_data, 0, sizeof(httpclient_data));
    memset(&http_content, 0, sizeof(HTTP_RESP_CONTENT_LEN));

    httpclient.header = "Accept: text/xml,text/html,\r\n";

    httpclient_data.response_buf = http_content;
    httpclient_data.response_content_len = HTTP_RESP_CONTENT_LEN;

    ret = httpclient_common(
                      &httpclient,
                      "http://iot-nsauth.alibaba.net/system/time",
                      80,
                      NULL,
                      HTTPCLIENT_GET,
                      5000,
                      &httpclient_data);
    if (0 != ret) {
        log_err("request epoch time from remote server failed.");
        return 0;
    } else {

        char            backup;
        uint64_t        cnv = 0;
        uint64_t        res = 0;

        backup = http_content[10];
        http_content[10] = '\0';
        cnv = atoi(http_content);
        res = cnv * 1000;
        http_content[10] = backup;
        cnv = atoi(http_content + 10);
        res += cnv;
        log_debug("res = %ld", res);

        snprintf(copy, len, "%s", http_content);
        return res;
    }
}
