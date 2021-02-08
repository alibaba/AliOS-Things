/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include "ulog/ulog.h"
#include "oss_app.h"
#include "aos/yloop.h"
#include "aos/kernel.h"
#include "ucloud_ai.h"
#include "graphics.h"
#include "sdkconfig.h"

#define TAG "UCLOUD_AI"

#include <httpclient.h>

#define HEAD_SIZE 2048
#define BODY_SZIE 1024*128+1

httpclient_t ai_client = { 0 };
httpclient_data_t ai_client_data = {0};
static ai_model_t ai_model = AI_MODEL_MAX;

void set_ucloud_ai_model(ai_model_t model)
{
    ai_model = model;
}

ai_model_t get_ucloud_ai_model(void)
{
    return ai_model;
}

int32_t ucloud_ai_init(void)
{
    int32_t ret;
    char * customer_header = "Accept: */*\r\n";

    ret = httpclient_prepare(&ai_client_data, HEAD_SIZE, BODY_SZIE);
    if (ret != HTTP_SUCCESS)
        return -1;

    ai_client.is_http = true;
    httpclient_set_custom_header(&ai_client, customer_header);

    return ret;
}

int32_t ucloud_ai_uninit(void)
{
    int32_t ret;

    ret = httpclient_unprepare(&ai_client_data);
    return ret;
}

int32_t ucloud_ai_connect(char *url)
{
    int32_t ret;

    ret = httpclient_conn(&ai_client, (const char *)url);
    if(HTTP_SUCCESS != ret) {
        LOGE(TAG, "http connect failed");
        return -1;
    }
    return ret;
}

void ucloud_ai_disconnect(void)
{
    httpclient_clse(&ai_client);
}

int32_t ucloud_ai_get_stream(char *url)
{
    int ret;
    int recv_len = 0;

    httpclient_reset(&ai_client_data);
    ret = httpclient_send(&ai_client, (const char *)url, HTTP_GET, &ai_client_data);
    if(HTTP_SUCCESS != ret) {
        LOGE(TAG, "http send request failed");
        return -1;
    }
    do {
        ret = httpclient_recv(&ai_client, &ai_client_data);
        if (ret < 0)
            break;
        recv_len = ai_client_data.response_content_len;
    } while (ret == HTTP_EAGAIN);
    return recv_len;
}

int32_t ucloud_ai_save_image(char *path)
{
    FILE *jpeg_file;

    if ((jpeg_file = fopen(path, "wb")) == NULL) {
        LOGE(TAG, "opening output file fail\n");
        return -1;
    }

    if (fwrite(ai_client_data.response_buf, ai_client_data.response_content_len, 1, jpeg_file) < 1) {
        LOGE(TAG, "write buf fail\n");
        return -1;
    }
    fclose(jpeg_file);

    return 0;
}

uint8_t *ucloud_ai_upload_file(char *path)
{
    char *tmp_upload_url, *p_upload_url;

    /*update capture.jpg to oss*/
    tmp_upload_url = upload_local_file(path, OSS_ACCESS_KEY, \
            OSS_ACCESS_SECRET, NULL , OSS_ENDPOINT, OSS_BUCKET);
    if (!tmp_upload_url) {
        LOGE(TAG, "url is null\n");
        return NULL;
    }
    p_upload_url = strdup(tmp_upload_url);
    return p_upload_url;
}