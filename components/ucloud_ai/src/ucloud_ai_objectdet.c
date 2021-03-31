/*
 * Copyright (C) 2021-2023 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include "ulog/ulog.h"
#include "ucloud_ai_common.h"

#define TAG "UCLOUD_AI_OBJECTDET"

void ucloud_ai_objectdet_detect_object(char *path, ucloud_ai_cb_t cb)
{
    int ret;
    int len;
    char *p_upload_url = NULL;

    /*update capture.jpg to oss*/
    p_upload_url = ucloud_ai_upload_file(path);

    if (p_upload_url) {
        detectObject(p_upload_url, cb);
        free(p_upload_url);
    }
}

void ucloud_ai_objectdet_detect_main_body(char *path, ucloud_ai_cb_t cb)
{
    int ret;
    int len;
    char *p_upload_url = NULL;

    /*update capture.jpg to oss*/
    p_upload_url = ucloud_ai_upload_file(path);

    if (p_upload_url) {
        detectMainBody(p_upload_url, cb);
        free(p_upload_url);
    }
}
