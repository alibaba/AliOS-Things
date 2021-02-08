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
#include "objectdet.h"

#define TAG "UCLOUD_AI_OBJECTDET"

int ucloud_ai_objectdet_object_detect(char *path, objectdet_detect_object_cb_t cb)
{
    int ret;
    int len;
    char *p_upload_url;

    /*update capture.jpg to oss*/
    p_upload_url = ucloud_ai_upload_file(path);

    ret = objectdet_detect_object_ai(p_upload_url, cb);
    if (ret < 0)
        return -1;

    return 0;
}
