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
#include "ucloud_ai_demo.h"

#ifdef AOS_COMP_LCD
#include "hal_lcd.h"
#endif

#ifdef AOS_OLED_SH1106
#include "sh1106.h"
#endif

#ifdef AOS_USE_BEEPER
#include "beeper.h"
#endif
#ifdef UCLOUD_AI_FACEBODY_CONFIG
#include "facebody.h"
#endif
#ifdef UCLOUD_AI_OBJECTDET_CONFIG
#include "objectdet.h"
#endif

#define TAG "UCLOUD_AI"
#define CAPTURED_IMAGE "/data/capture.jpg"

//#define USE_CURL
//#define LOG

#include <httpclient.h>

#define HEAD_SIZE 2048
#define BODY_SZIE 1024*128+1

#ifdef UCLOUD_AI_FACEBODY_CONFIG
static face_rect_t face_rect;
static float g_confidence = 0.0;
static char *p_expression = NULL;
#endif

static aos_mutex_t g_mutex;

#ifdef UCLOUD_AI_FACEBODY_CONFIG
int facebody_comparing_callback(float confidence, face_rect_t *face_rect)
{
    aos_mutex_lock(&g_mutex, AOS_WAIT_FOREVER);
    g_confidence = confidence;
    aos_mutex_unlock(&g_mutex);
   /*judge if boss is coming*/
    if (confidence > 60) {
        LOG("==============================================\n");
        LOG("=================boss is coming===============\n");
        LOG("==============================================\n");
        /*post event to Iot platform*/
        user_post_event(1);

        /*show string on OLED for HaaS EDK*/
#ifdef AOS_OLED_SH1106
        OLED_Clear();
        OLED_Show_String(24, 12, "Warning!!!", 12, 1);        
        OLED_Show_String(12, 36, "Boss is coming!", 12, 1);
        OLED_Refresh_GRAM();
#endif

        /*beeper alarm for HaaS EDK*/
#ifdef AOS_USE_BEEPER
        beeper_start();
#endif
    } else {
#ifdef AOS_OLED_SH1106
        OLED_Clear();
        OLED_Show_String(12, 24, "It's safe now!", 12, 1);
        OLED_Refresh_GRAM();
#endif
        /*stop beeper alarm*/
#ifdef AOS_USE_BEEPER
        beeper_stop();
#endif
    }
    return 0;
}

void do_comparing_facebody_async(void)
{
    aos_mutex_lock(&g_mutex, AOS_WAIT_FOREVER);
    ucloud_ai_comparing_facebody(CAPTURED_IMAGE, facebody_comparing_callback);
    aos_mutex_unlock(&g_mutex);
}

int do_facebody_comparing_process(void)
{
    aos_post_event(EV_NETWORK, CODE_ON_GOT_WIFICAMERA_IMAGE, 0);

#ifdef AOS_COMP_SDL2
    aos_mutex_lock(&g_mutex, AOS_WAIT_FOREVER);
    if (g_confidence > 60) {
        LOG("draw string\n");
        graphics_draw_string("Warning!!!", 120, 100);
        graphics_draw_string("boss is coming!!!", 100, 120);
    }
    aos_mutex_unlock(&g_mutex);
#endif
    return 0;
}

int recognize_expression_callback(char *expression, float face_probability, face_rect_t *face_rect)
{
    int len;

    if (!expression || !face_rect)
        return -1;

    aos_mutex_lock(&g_mutex, AOS_WAIT_FOREVER);
    p_expression = strdup(expression);
    if (!p_expression) {
        LOGE(TAG, "p_expression strdup fail\n");
        return -1;
    }
    aos_mutex_unlock(&g_mutex);

    return 0;
}

void do_recognize_expression_async(void)
{
    ucloud_ai_recognize_expression(CAPTURED_IMAGE, recognize_expression_callback);
}

int do_facebody_recognize_expression_process(void)
{
    aos_post_event(EV_NETWORK, CODE_ON_GOT_WIFICAMERA_IMAGE, 0);

#ifdef AOS_COMP_SDL2
    aos_mutex_lock(&g_mutex, AOS_WAIT_FOREVER);
    if (p_expression) {
        /*draw image to lcd screen*/
        LOG("p_expression: %s\n", p_expression);
        if (!strcmp(p_expression, "sadness")) {
            graphics_draw_image("/data/sadness.jpg", 20, 20);
        } else if (!strcmp(p_expression, "happiness")) {
            graphics_draw_image("/data/happiness.jpg", 20, 20);
        } else if (!strcmp(p_expression, "surprise")) {
            graphics_draw_image("/data/surprise.jpg", 20, 20);
        } else {
            graphics_draw_string(p_expression, 20, 20);
        }
        free(p_expression);
        p_expression = NULL;
    }
    aos_mutex_unlock(&g_mutex);
#endif
    return 0;
}

int generate_human_anime_styple_callback(char *out_url)
{
    int ret;
    long size;
    int recv_len = 0;

    if (!out_url) {
        LOGE(TAG, "out_url is null\n");
        return -1;
    }

    LOG("out_url: %s\n", out_url);
    /*get image by url*/
    ret = ucloud_ai_connect(out_url);
    if(HTTP_SUCCESS != ret) {
        LOGE(TAG, "ucloud_ai_connect failed, ret: %d\n", ret);
        return -1;
    }

    recv_len = ucloud_ai_get_stream(out_url);
    LOG("image recv_len: %d", recv_len);
    ucloud_ai_disconnect();
    if (!recv_len) {
        LOGE(TAG, "recv_len is %d\n", recv_len);
        return -1;
    }

    /*save human anime style image*/
    ret = ucloud_ai_save_image("/data/humananime.png");
    if (ret < 0) {
        LOGE(TAG, "save_captured_image fail\n");
        return -1;
    }

#ifdef AOS_COMP_SDL2
    /*draw image to lcd screen*/
    graphics_draw_image("/data/humananime.png", 0, 0);
#endif
    return ret;
}

int do_facebody_generate_human_anime_style_process(void)
{
    ucloud_ai_facebody_generate_human_anime_style(CAPTURED_IMAGE, generate_human_anime_styple_callback);
    return 0;
}
#endif

#ifdef UCLOUD_AI_OBJECTDET_CONFIG
int object_detect_callback(char *type, float score, object_rect_t *rect)
{
    int len = 0;
    char *p_type = NULL;

    if (!type || !rect) {
        LOGE(TAG, "type or rect is null\n");
        return -1;
    }
    p_type = strdup(type);
    if (!p_type) {
        LOGE(TAG, "p_type strdup fail\n");
        return -1;
    }
#ifdef AOS_COMP_SDL2
    /*draw image to lcd screen*/
    if (p_type) {
        LOG("type: %s\n", p_type);
        graphics_draw_string(p_type, rect->x, rect->y);
        free(p_type);
        p_type = NULL;
    }
#endif
    return 0;
}

int do_objectdet_object_detect_process(void)
{
    return ucloud_ai_objectdet_object_detect(CAPTURED_IMAGE, object_detect_callback);
}
#endif

int ucloud_ai_demo_main(void *p)
{
    int ret = 0;
    int recv_len = 0;
    char *url = WIFICAMERA_URL;
    char *upload_url = NULL;

    ret = aos_mutex_new(&g_mutex);
    if (ret != 0) {
        return -1;
    }

    LOG("start ucloud_ai_main\n");
    /*for wificamera client*/
    ret = wificamera_init();
    if(HTTP_SUCCESS != ret) {
        LOGE(TAG, "http_app_init failed");
        return -1;
    }

    ret = wificamera_connect(url);
    if(HTTP_SUCCESS != ret) {
        LOGE(TAG, "http connect failed");
        return -1;
    }

    ret = ucloud_ai_init();
    if(HTTP_SUCCESS != ret) {
        LOGE(TAG, "ucloud_ai_init failed");
        return -1;
    }

    while (1) {
        recv_len = wificamera_get_stream(url);
        if (recv_len <= 0) {
            LOGE(TAG, "recv_len : %d\n", recv_len);
            continue;
        }
        /*save file to data folder*/
        aos_mutex_lock(&g_mutex, AOS_WAIT_FOREVER);
        ret = wificamera_save_image(CAPTURED_IMAGE);
        aos_mutex_unlock(&g_mutex);
        if (ret < 0) {
            LOGE(TAG, "save_captured_image fail\n");
            continue;
        }
#ifdef AOS_COMP_SDL2
        /*draw wifi camera frame*/
        graphics_draw_image(CAPTURED_IMAGE, 0, 0);
#endif
        switch (get_ucloud_ai_model()) {
#ifdef UCLOUD_AI_FACEBODY_CONFIG
            case AI_MODEL_COMPARING_FACEBODY:
                do_facebody_comparing_process();
                break;
            case AI_MODEL_GENERATE_HUMAN_ANIME_STYLE:
                do_facebody_generate_human_anime_style_process();
                break;
            case AI_MODEL_RECOGNIZE_EXPRESSION:
                do_facebody_recognize_expression_process();
                break;
#endif
#ifdef UCLOUD_AI_OBJECTDET_CONFIG
            case AI_MODEL_OBJECT_DETECT:
                do_objectdet_object_detect_process();
                break;
#endif
            default:
                break;
        }
#ifdef AOS_COMP_SDL2
        /*draw image to lcd screen*/
        graphics_flip();
#else
        wificamera_show_image();
#endif
    }

    wificamera_disconnect();
    wificamera_uninit();
    ucloud_ai_uninit();
    aos_mutex_free(&g_mutex);
    LOG("wificamera_main end\n");
    return 0;
}
