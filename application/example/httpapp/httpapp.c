/**
 * Copyright (C) 2018 Alibaba.inc, All rights reserved.
 *
 */

#include <stdio.h>
#include <string.h>
#include "aos/kernel.h"
#include "aos/yloop.h"
#include "aos/cli.h"
#include "http.h"

#include "ulog/ulog.h"

#define TAG "HTTPAPP"
#define BUF_SIZE 1024

char *geturl    = "https://iot-auth.cn-shanghai.aliyuncs.com/";
char *posturl   = "https://api.mediatek.com/mcs/v2/devices/D0n2yhrl/datapoints.csv";
char *puturl    = "https://ec2-52-76-74-57.ap-southeast-1.compute.amazonaws.com/mcs/test/ok/200";
char *deleteurl = "https://ec2-52-76-74-57.ap-southeast-1.compute.amazonaws.com/mcs/test/ok/200";

/* httpc get method demo */
int httpc_get_demo(char* url)
{
    httpclient_t client = {0};
    httpclient_data_t client_data = {0};
    char *buf = NULL;
    int ret;

    buf = malloc(BUF_SIZE);
    if (buf == NULL) {
        printf("Malloc failed.\r\n");
        return -1;
    }
    memset(buf, 0, BUF_SIZE);
    client_data.response_buf = buf;  //Sets a buffer to store the result.
    client_data.response_buf_len = BUF_SIZE;  //Sets the buffer size.
    ret = httpc_get(&client, url, &client_data);
    if( ret == 0 ) {
        printf("Data received: %s\r\n", client_data.response_buf);
    }
    return ret;
}

/* httpc post method demo */
int httpc_post_demo(char* url)
{
    char *header = "deviceKey:FZoo0S07CpwUHcrt\r\n";
    char *content_type = "text/csv";
    char *post_data = "1,,I am string!";
    httpclient_t client = {0};
    httpclient_data_t client_data = {0};
    char *buf = NULL;
    int ret;

    buf = malloc(BUF_SIZE);
    if (buf == NULL) {
        printf("Malloc failed.\r\n");
        return -1;
    }
    memset(buf, 0, BUF_SIZE);
    client_data.response_buf = buf;  //Sets a buffer to store the result.
    client_data.response_buf_len = BUF_SIZE;  //Sets the buffer size.
    httpc_set_custom_header(&client, header);  //Sets the custom header if needed.
    client_data.post_buf = post_data;  //Sets the user data to be posted.
    client_data.post_buf_len = strlen(post_data);  //Sets the post data length.
    client_data.post_content_type = content_type;  //Sets the content type.
    ret = httpc_post(&client, url, &client_data);
    if( ret == 0 ) {
        printf("Data received: %s\r\n", client_data.response_buf);
    }
    return ret;
}

/* httpc put method demo */
int httpc_put_demo(char* url)
{
    char *content_type = "text/csv";
    char *put_data = "1,,I am string!";
    httpclient_t client = {0};
    httpclient_data_t client_data = {0};
    char *buf = NULL;
    int ret = 0;

    buf = malloc(BUF_SIZE);
    if (buf == NULL) {
        printf("Malloc failed.\r\n");
        return -1;
    }
    memset(buf, 0, BUF_SIZE);
    client_data.response_buf = buf;  //Sets a buffer to store the result.
    client_data.response_buf_len = BUF_SIZE;  //Sets the buffer size.
    client_data.post_buf = put_data;  //Sets the user data to be put.
    client_data.post_buf_len = strlen(put_data);  //Sets the put data length.
    client_data.post_content_type = content_type;  //Sets the content type.
    ret = httpc_put(&client, url, &client_data);
    if( ret == 0 ) {
        printf("Data received: %s\r\n", client_data.response_buf);
    }
    return ret;
}

/* httpc put delete demo */
int httpc_delete_demo(char* url)
{
    httpclient_t client = {0};
    httpclient_data_t client_data = {0};
    char *buf = NULL;
    int ret;

    buf = malloc(BUF_SIZE);
    if (buf == NULL) {
        printf("Malloc failed.\r\n");
        return -1;
    }
    memset(buf, 0, BUF_SIZE);
    client_data.response_buf = buf;  //Sets a buffer to store the result.
    client_data.response_buf_len = BUF_SIZE;  //Sets the buffer size.
    ret = httpc_delete(&client, url, &client_data);
    if( ret == 0 ) {
        printf("Data received: %s\r\n", client_data.response_buf);
    }
    return ret;
}

static void httpc_app()
{
    int ret;

    if(0 !=(ret = httpc_get_demo(geturl))) {
        printf("httpc get demo run failed ret=%d\n", ret);
    }

    if(0 !=(ret = httpc_post_demo(posturl))) {
        printf("httpc post demo run failed ret=%d\n", ret);
    }

    if(0 !=(ret = httpc_put_demo(puturl))) {
        printf("httpc put demo run failed ret=%d\n", ret);
    }

    if(0 !=(ret = httpc_delete_demo(deleteurl))) {
        printf("httpc delete demo run failed ret=%d\n", ret);
    }
    
}

static void httpc_app_task(void) {
    aos_task_new("httpc_app", httpc_app, NULL, 20 * 1024);
}

static struct cli_command httpc_app_commands[] = {
    {"httpc", "httpc", httpc_app_task},
};

int application_start(int argc, char *argv[])
{
    LOG("application started.");

    aos_cli_register_commands(httpc_app_commands,
            sizeof(httpc_app_commands) / sizeof(struct cli_command));
    aos_loop_run();

    return 0;
}


