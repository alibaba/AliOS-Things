/*
 * Copyright (C) 2021-2023 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include "ulog/ulog.h"
#include "aos/kernel.h"
#include "ucamera_service.h"
#include <httpclient.h>
#include "wifi_camera.h"
// #include <aiconfig.h>

/*Tested this program with esp32 eye*/

#define TAG "WIFI_CAMERA"

#define HEAD_SIZE 2048
#define BODY_SZIE (1024 * 128 + 1)

static httpclient_t wifi_camera_client = { 0 };
static httpclient_data_t wifi_camera_client_data = {0};

static void wifi_camera_delete(ucamera_device_t *dev)
{
    free(dev);
}

static bool wifi_camera_device_available(void)
{
    return true;
}

int32_t wifi_camera_init(ucamera_device_t *dev)
{
    int32_t ret;
    char *customer_header = "Accept: */*\r\n";

    ret = httpclient_prepare(&wifi_camera_client_data, HEAD_SIZE, BODY_SZIE);
    if (ret != HTTP_SUCCESS)
        return -1;

    wifi_camera_client.is_http = true;
    httpclient_set_custom_header(&wifi_camera_client, customer_header);

    LOG("wifi camera init done\n");
    return ret;
}

int32_t wifi_camera_uninit(ucamera_device_t *dev)
{
    int32_t ret;

    ret = httpclient_unprepare(&wifi_camera_client_data);
    return ret;
}

int32_t wifi_camera_connect(ucamera_device_t *dev)
{
    int32_t ret;

    ret = httpclient_conn(&wifi_camera_client, (const char *)WIFICAMERA_URL);
    if (HTTP_SUCCESS != ret) {
        LOGE(TAG, "http connect failed");
        return -1;
    }

    LOG("connect to wifi camera successfully\n");
    return ret;
}

void wifi_camera_disconnect(ucamera_device_t *dev)
{
    if (!dev)
        return;

    httpclient_clse(&wifi_camera_client);
    LOG("disconnect to wifi camera successfully\n");
}

frame_buffer_t *wifi_camera_get_frame(ucamera_device_t *dev)
{
    int ret;

    if (!dev)
        return NULL;

    httpclient_reset(&wifi_camera_client_data);
    ret = httpclient_send(&wifi_camera_client, (const char *)WIFICAMERA_URL, HTTP_GET, &wifi_camera_client_data);
    if (HTTP_SUCCESS != ret) {
        LOGE(TAG, "http send request failed");
        return NULL;
    }

    do {
        ret = httpclient_recv(&wifi_camera_client, &wifi_camera_client_data);
        if (ret < 0) {
            dev->stream_buf = NULL;
            dev->stream_len = 0;
            break;
        }
        dev->stream_buf = wifi_camera_client_data.response_buf;
        dev->stream_len = wifi_camera_client_data.response_content_len;
    } while (ret == HTTP_EAGAIN);

    /*malloc frame*/
    dev->frame.buf = dev->stream_buf;
    dev->frame.len = dev->stream_len;
    dev->frame.width = WIFI_CAMERA_FRAME_WIDTH;
    dev->frame.height = WIFI_CAMERA_FRAME_HEIGHT;
    dev->frame.format = FRAME_FORMAT_RGB565;

    return &dev->frame;
}

static ucamera_device_t *wifi_camera_device_create(int devindex)
{
    ucamera_device_t *device = NULL;

    /* Initialize all variables that we clean on shutdown */
    device = (ucamera_device_t *) malloc(sizeof(ucamera_device_t));
    if (!device) {
        LOGE(TAG, "malloc camera device fail\n");
        return 0;
    }
    device->is_dummy = false;
    device->width = WIFI_CAMERA_FRAME_WIDTH;
    device->height = WIFI_CAMERA_FRAME_HEIGHT;

    /* Set the function pointers */
    device->camera_init = wifi_camera_init;
    device->camera_uninit = wifi_camera_uninit;
    device->camera_connect = wifi_camera_connect;
    device->camera_disconnect = wifi_camera_disconnect;
    device->camera_get_frame = wifi_camera_get_frame;
    device->camera_free = wifi_camera_delete;

    LOG("create camera device successfully\n");
    return device;
}

ucamera_context_t wifi_camera = {
    WIFI_CAMERA_NAME, "http camera stream over wifi",
    wifi_camera_device_available, wifi_camera_device_create
};
