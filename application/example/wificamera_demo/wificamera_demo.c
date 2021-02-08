/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include "ulog/ulog.h"
#include "aos/yloop.h"
#include "aos/kernel.h"
#include "graphics.h"
#include <httpclient.h>
#include "sdkconfig.h"
#ifdef AOS_COMP_LCD
#include "hal_lcd.h"
#endif

#define TAG "WIFICAMERA"

#define USE_HTTPCLIENT
//#define USE_CURL
#define LOG

#define TAG "WIFI_CAMERA"
#define CAPTURED_IMAGE "/data/capture.jpg"

int wificamera_main(void *p)
{
    int ret = 0;
    int recv_len = 0;
    char *url = WIFICAMERA_URL;
    char *upload_url = NULL;

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

    while (1) {
        recv_len = wificamera_get_stream(url);
        if (recv_len <= 0) {
            LOGE(TAG, "recv_len : %d\n", recv_len);
            continue;
        }
        /*save file to data folder*/
        ret = wificamera_save_image(CAPTURED_IMAGE);
        if (ret < 0) {
            LOGE(TAG, "save_captured_image fail\n");
            continue;
        }
#ifdef AOS_COMP_SDL2
        /*draw wifi camera frame*/
        graphics_draw_image(CAPTURED_IMAGE, 0, 0);
#endif
#ifdef AOS_COMP_SDL2
        /*draw image to lcd screen*/
        graphics_flip();
#else
        wificamera_show_image();
#endif
    }

    wificamera_disconnect();
    wificamera_uninit();
    LOG("wificamera_main end\n");
    return 0;
}
