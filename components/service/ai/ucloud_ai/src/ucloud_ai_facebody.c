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
#include "facebody.h"

#define TAG "UCLOUD_AI_FACEBODY"

//#define USE_CURL
//#define LOG

#include <httpclient.h>

#define HEAD_SIZE 2048
#define BODY_SZIE 1024*128+1

static face_rect_t face_rect;
static float g_confidence = 0.0;
static char *p_expression = NULL;

void ucloud_ai_comparing_facebody(char *path, facebody_comparing_cb_t cb)
{
    int len;
    char *p_upload_url;
    char *p_myface_url;

    /*update capture.jpg to oss*/
    p_upload_url = ucloud_ai_upload_file(path);

    /*my face picture*/
    p_myface_url = MYFACE_PATH;

    /*do facebody detection*/
    facebody_comparing_ai(p_upload_url, p_myface_url, cb);
    free(p_upload_url);
}

void ucloud_ai_recognize_expression(char *path, facebody_recognize_expression_cb_t cb)
{
    int ret;
    int len;
    char *p_upload_url;

    /*update capture.jpg to oss*/
    p_upload_url = ucloud_ai_upload_file(path);

    facebody_recognize_expression_ai(p_upload_url, cb);
    free(p_upload_url);
}

void ucloud_ai_facebody_generate_human_anime_style(char *path, facebody_generate_human_anime_stype_cb_t cb)
{
    int ret;
    int len;
    char *p_upload_url;

    /*update capture.jpg to oss*/
    p_upload_url = ucloud_ai_upload_file(path);

    /*do generating human anime style, and get image url*/
    facebody_generate_human_anime_style_ai(p_upload_url, cb);
    free(p_upload_url);
}
