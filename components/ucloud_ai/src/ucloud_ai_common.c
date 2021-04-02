/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include "ulog/ulog.h"
#include "aos/kernel.h"
#include "ucloud_ai_common.h"

#define TAG "UCLOUD_AI_SERVICE"

#include <httpclient.h>

#define HEAD_SIZE 2048
#define BODY_SZIE (1024 * 128 + 1)

httpclient_t ai_client = { 0 };
httpclient_data_t ai_client_data = {0};
static ucloud_ai_model_t ai_model = UCLOUD_AI_MODEL_MAX;
static char g_oss_bucket[64];
static char g_oss_endpoint[64];

void ucloud_ai_set_oss_bucket(char *bucket)
{
    int32_t len;

    if (bucket) {
        strcpy(g_oss_bucket, bucket);
        len = strlen(bucket);
        g_oss_bucket[len] = '\0';
        printf("g_oss_bucket: %s\n", g_oss_bucket);
    }
}

void ucloud_ai_set_oss_endpoint(char *endpoint)
{
    int32_t len;

    if (endpoint) {
        strcpy(g_oss_endpoint, endpoint);
        len = strlen(endpoint);
        g_oss_endpoint[len] = '\0';
    }
}

uint8_t *ucloud_ai_upload_file(char *path)
{
    char *tmp_upload_url, *p_upload_url;
    char *access_key = getAccessKey();
    char *access_secret = getAccessSecret();

    /*update capture.jpg to oss*/
    tmp_upload_url = oss_upload_local_file(access_key, \
            (char *)access_secret, g_oss_endpoint, g_oss_bucket, path);
    if (!tmp_upload_url) {
        LOGE(TAG, "url is null\n");
        return NULL;
    }
    p_upload_url = strdup(tmp_upload_url);
    return p_upload_url;
}

void ucloud_ai_set_key_secret(char *key, char *secret)
{
    setAccesskeyAndSecret(key, secret);
}

int32_t ucloud_ai_init(void)
{
    int32_t ret;
    char *customer_header = "Accept: */*\r\n";

    ret = httpclient_prepare(&ai_client_data, HEAD_SIZE, BODY_SZIE);
    if (ret != HTTP_SUCCESS)
        return -1;

    ai_client.is_http = true;
    httpclient_set_custom_header(&ai_client, customer_header);

    return ret;
}

int32_t ucloud_ai_uninit(void)
{
    return httpclient_unprepare(&ai_client_data);
}

int32_t ucloud_ai_connect(char *url)
{
    int32_t ret;

    ret = httpclient_conn(&ai_client, (const char *)url);
    if (HTTP_SUCCESS != ret) {
        LOGE(TAG, "http connect failed");
        return -1;
    }
    return ret;
}

void ucloud_ai_disconnect(void)
{
    httpclient_clse(&ai_client);
}

int32_t ucloud_ai_get_stream(char *url, char **stream)
{
    int ret;
    int recv_len = 0;

    httpclient_reset(&ai_client_data);
    ret = httpclient_send(&ai_client, (const char *)url, HTTP_GET, &ai_client_data);
    if (HTTP_SUCCESS != ret) {
        LOGE(TAG, "http send request failed");
        return -1;
    }
    do {
        ret = httpclient_recv(&ai_client, &ai_client_data);
        if (ret < 0)
            break;
        *stream = ai_client_data.response_buf;
        recv_len = ai_client_data.response_content_len;
    } while (ret == HTTP_EAGAIN);
    return recv_len;
}

