/*
 * Copyright (C) 2021-2023 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include "ucloud_ai_common.h"

#define TAG "UCLOUD_AI_IMAGERECOG"

void ucloud_ai_imagerecog_classifying_rubbish(char *path, ucloud_ai_cb_t cb)
{
    int len;
    char *p_upload_url;

    /*update capture.jpg to oss*/
    p_upload_url = ucloud_ai_upload_file(path);
    if (!p_upload_url)
        return;

    /*do imageseg detection*/
    classifyingRubbish(p_upload_url, cb);
    free(p_upload_url);
}

void ucloud_ai_imagerecog_detect_fruits(char *path, ucloud_ai_cb_t cb)
{
    int len;
    char *p_upload_url;

    /*update capture.jpg to oss*/
    p_upload_url = ucloud_ai_upload_file(path);
    if (!p_upload_url)
        return;

    /*do imageseg detection*/
    detectFruits(p_upload_url, cb);
    free(p_upload_url);
}
