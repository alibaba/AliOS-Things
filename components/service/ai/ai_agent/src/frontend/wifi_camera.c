/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include "ulog/ulog.h"
#include "aos/yloop.h"
#include "aos/kernel.h"
#include "graphics.h"
#include "hal_lcd.h"

#define TAG "WIFI_CAMERA"

//#define USE_CURL
//#define LOG

#include <httpclient.h>

#define HEAD_SIZE 2048
#define BODY_SZIE 1024*128+1

#ifndef AOS_COMP_SDL2
#ifdef AOS_COMP_LCD
static uint8_t *framebuffer = NULL;
#endif
#endif

httpclient_t wificamera_client = { 0 };
httpclient_data_t wificamera_client_data = {0};


int32_t wificamera_init(void)
{
    int32_t ret;
    char * customer_header = "Accept: */*\r\n";

    ret = httpclient_prepare(&wificamera_client_data, HEAD_SIZE, BODY_SZIE);
    if (ret != HTTP_SUCCESS)
        return -1;

    wificamera_client.is_http = true;
    httpclient_set_custom_header(&wificamera_client, customer_header);

#ifndef AOS_COMP_SDL2
#ifdef AOS_COMP_LCD
    framebuffer = (uint8_t *)malloc(SCREEN_W*SCREEN_H*2);
#endif
#endif
    return ret;
}

int32_t wificamera_uninit(void)
{
    int32_t ret;

#ifndef AOS_COMP_SDL2
#ifdef AOS_COMP_LCD
    free(framebuffer);
#endif
#endif

    ret = httpclient_unprepare(&wificamera_client_data);
    return ret;
}

int32_t wificamera_connect(char *url)
{
    int32_t ret;

    ret = httpclient_conn(&wificamera_client, (const char *)url);
    if(HTTP_SUCCESS != ret) {
        LOGE(TAG, "http connect failed");
        return -1;
    }
    return ret;
}

void wificamera_disconnect(char *url)
{
    httpclient_clse(&wificamera_client);
}

int32_t wificamera_get_stream(char *url)
{
    int ret;
    int recv_len = 0;

    httpclient_reset(&wificamera_client_data);
    ret = httpclient_send(&wificamera_client, (const char *)url, HTTP_GET, &wificamera_client_data);
    if(HTTP_SUCCESS != ret) {
        LOGE(TAG, "http send request failed");
        return -1;
    }
    do {
        ret = httpclient_recv(&wificamera_client, &wificamera_client_data);
        if (ret < 0)
            break;
        recv_len = wificamera_client_data.response_content_len;
    } while (ret == HTTP_EAGAIN);
    return recv_len;
}


int32_t wificamera_save_image(char *path)
{
    FILE *jpeg_file = NULL;

    if ((jpeg_file = fopen(path, "wb")) == NULL) {
        LOGE(TAG, "opening output file fail\n");
        return -1;
    }
    if (fwrite(wificamera_client_data.response_buf, wificamera_client_data.response_content_len, 1, jpeg_file) < 1) {
        LOGE(TAG, "write buf fail\n");
        return -1;
    }
    fclose(jpeg_file);

    return 0;
}

int32_t wificamera_show_image(void)
{
#ifndef AOS_COMP_SDL2
#ifdef AOS_COMP_LCD
    int ret;
    int recv_len = 0;
    uint8_t *rgb_buffer = NULL;
    int rgb_size = 0;
    hal_lcd_t *hal_lcd = get_hal_lcd();

    /*decode jpeg to rgb888*/
    ret = tjpeg2rgb(wificamera_client_data.response_buf, recv_len, &rgb_buffer, &rgb_size);
    if (ret < 0) {
        LOGE(TAG, "tjpeg2rgb fail, ret = %d", ret);
        if (rgb_buffer)
            free(rgb_buffer);
        return -1;
    }
    /*convert rgb888 to rgb565*/
    rgb888torgb565(rgb_buffer, SCREEN_W*SCREEN_H*3, framebuffer, SCREEN_W*SCREEN_H*2);

    /*show picture on lcd screen*/
    hal_lcd->lcd_frame_draw(framebuffer);
    free(rgb_buffer);
    return 0;
#endif
#endif
    return 0;
}