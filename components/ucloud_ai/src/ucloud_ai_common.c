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
        printf("g_oss_endpoint: %s\n", g_oss_endpoint);
    }
}

uint8_t *ucloud_ai_upload_file(char *path)
{
    char *tmp_upload_url, *p_upload_url;
    char *access_key = getAccessKey();
    char *access_secret = getAccessSecret();

    printf("access_key: %s, access_secret: %s\n", access_key, access_secret);
    printf("g_oss_bucket: %s\n", g_oss_bucket);
    printf("g_oss_endpoint: %s\n", g_oss_endpoint);
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
