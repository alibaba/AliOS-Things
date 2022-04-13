/*
 * Copyright (C) 2021-2023 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include "ucloud_ai_common.h"

#define TAG "UCLOUD_AI_IMAGEENHAN"

/*recognizing identity card has issue to be resolved*/
void ucloud_ai_imageenhan_erase_person(char *image_path, char *user_mask_path, ucloud_ai_cb_t cb)
{
    int ret;
    int len;
    char *p_upload_url;

    /*update capture.jpg to oss*/
    p_upload_url = ucloud_ai_upload_file(image_path);
    if (!p_upload_url)
        return;

    /*do erasing person*/
    erasePerson(p_upload_url, user_mask_path, cb);
    free(p_upload_url);
}

void ucloud_ai_imageenhan_extend_image_style(char *major_path, char *style_path, ucloud_ai_cb_t cb)
{
    int ret;
    int len;
    char *p_upload_url;

    /*update capture.jpg to oss*/
    p_upload_url = ucloud_ai_upload_file(major_path);
    if (!p_upload_url)
        return;

    /*do extending image style*/
    extendImageStyle(p_upload_url, style_path, cb);
    free(p_upload_url);
}
