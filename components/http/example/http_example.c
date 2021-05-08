/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include "httpclient.h"
#if AOS_COMP_CLI
#include "aos/cli.h"
#endif
#include <string.h>
#include <stdio.h>

/* @brief http request buffer */
#define REQ_BUF_SIZE 2048
static char req_buf[REQ_BUF_SIZE];

/* @brief http response buffer */
#define RSP_BUF_SIZE 2048
static char rsp_buf[RSP_BUF_SIZE];

static void http_comp_example(int argc, char **argv)
{
    httpclient_t client = {0};
    httpclient_data_t client_data = {0};
    char * req_url = "http://www.baidu.com/";
    int ret;
    char * customer_header = "Accept: */*\r\n";

    if (argc > 1) {
        req_url = argv[1];
    }

    memset(req_buf, 0, sizeof(req_buf));
    client_data.header_buf = req_buf;
    client_data.header_buf_len = sizeof(req_buf);

    memset(rsp_buf, 0, sizeof(rsp_buf));
    client_data.response_buf = rsp_buf;
    client_data.response_buf_len = sizeof(rsp_buf);

    printf("http request %s\r\n", req_url);

    httpclient_set_custom_header(&client, customer_header);
    ret = httpclient_get(&client, req_url, &client_data);
    if( ret >= 0 ) {
        printf("Data received: %s \r\n", client_data.response_buf);

        printf("http comp test success!\r\n");
    }

    return;
}

#if AOS_COMP_CLI
/* reg args: fun, cmd, description*/
ALIOS_CLI_CMD_REGISTER(http_comp_example, http_example, http component base example)
#endif
