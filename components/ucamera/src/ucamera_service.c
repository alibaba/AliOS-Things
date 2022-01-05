/*
 * Copyright (C) 2021-2023 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include "ulog/ulog.h"
#include "aos/kernel.h"
#include "ucamera_service.h"
#include "wifi_camera.h"

#define TAG "UCAMERA_SERVICE"

/*
 * Initialize the ucamera service
 */
int32_t ucamera_service_init(const char *dev_name)
{
    ucamera_device_t *dev = ucamera_device_init(dev_name);
    if (!dev) {
        LOGE(TAG, "ucamera service init fail\n");
        return -1;
    }

    return 0;
}

/*
 * Uninitialize the ucamera service
 */
int32_t ucamera_service_uninit(void)
{
    ucamera_device_t *dev = ucamera_get_device();
    if (!dev) {
        LOGE(TAG, "ucamera get device name fail\n");
        return -1;
    }

    ucamera_device_uninit();

    return 0;
}

/*
 * Config the ucamera service
 */
int32_t ucamera_service_config(command_t cmd, void *params)
{
    int32_t ret;

    ucamera_device_t *dev = ucamera_get_device();
    if (!dev) {
        LOGE(TAG, "ucamera get device name fail\n");
        return -1;
    }

    dev->cmd = cmd;
    if (dev->type == CAM_WIFI_TYPE) {
        switch (cmd) {
        case UCAMERA_CMD_SET_URL:
            dev->params.wifi.control_url = (const char *)params;
            break;

        case UCAMERA_CMD_SET_CONTROL_URL:
        case UCAMERA_CMD_SET_FRAME_SIZE:
        case UCAMERA_CMD_SET_MIRROR:
        case UCAMERA_CMD_SET_FLIP:
            dev->params.wifi.control_url = (const char *)params;
            break;

        default:
            break;
        }
    } else if (dev->type == CAM_UART_TYPE) {
        switch (cmd) {
        case UCAMERA_CMD_SET_UART_PARAMS:
            dev->params.uart.bandrate = ((uart_cam_params_t *)params)->bandrate;
            dev->params.uart.rx = ((uart_cam_params_t *)params)->rx;
            dev->params.uart.tx = ((uart_cam_params_t *)params)->tx;
            break;

        case UCAMERA_CMD_SET_MODE:
            dev->params.uart.mode = ((uart_cam_params_t *)params)->mode;
            break;

        case UCAMERA_CMD_SET_FRAME_SIZE:
            dev->frame_size.id = ((frame_size_t *)params)->id;
            if (dev->frame_size.id == SIZE_800X600) {
                dev->frame_size.width = 800;
                dev->frame_size.height = 600;
            } else if (dev->frame_size.id == SIZE_640X480) {
                dev->frame_size.width = 640;
                dev->frame_size.height = 480;
            } else if (dev->frame_size.id == SIZE_320X240) {
                dev->frame_size.width = 320;
                dev->frame_size.height = 240;
            } else if (dev->frame_size.id == SIZE_240X240) {
                dev->frame_size.width = 240;
                dev->frame_size.height = 240;
            } else if (dev->frame_size.id == SIZE_160X120) {
                dev->frame_size.width = 120;
                dev->frame_size.height = 120;
            }
            break;

        case UCAMERA_CMD_SET_LED_BRIGHTNESS:
            dev->params.uart.led_brightness = ((uart_cam_params_t *)params)->led_brightness;
            break;

        case UCAMERA_CMD_SET_WIFI_SSID_PWD:
            dev->params.uart.wifi_ssid = ((uart_cam_params_t *)params)->wifi_ssid;
            dev->params.uart.wifi_pwd = ((uart_cam_params_t *)params)->wifi_pwd;
            break;

        case UCAMERA_CMD_SET_UPLOAD_TIME:
            dev->params.uart.upload_time = ((uart_cam_params_t *)params)->upload_time;
            break;
        default:
            break;
        }
    }
    ret = dev->camera_config(dev);
    if (ret < 0)
        LOGE(TAG, "ucamera_config fail\n");

    return ret;
}

/*
 * Start the ucamera service
 */
int32_t ucamera_service_connect(const char *url)
{
    int32_t ret;

    ucamera_device_t *dev = ucamera_get_device();
    if (!dev) {
        LOGE(TAG, "ucamera service start fail\n");
        return -1;
    }
    dev->params.wifi.url = url;
    ret = dev->camera_connect(dev);
    if (ret < 0)
        LOGE(TAG, "ucamera device connect fail\n");

    return ret;
}


/*
 * Stop the ucamera service
 */
int32_t ucamera_service_disconnect(void)
{
    int32_t ret;

    ucamera_device_t *dev = ucamera_get_device();
    if (!dev) {
        LOGE(TAG, "ucamera service stop fail\n");
        return -1;
    }

    ret = dev->camera_disconnect(dev);
    if (ret < 0)
        LOGE(TAG, "ucamera device disconnect fail\n");
    return ret;
}

/*
 * Get frame of camera device
 */
frame_buffer_t *ucamera_service_get_frame(void)
{
    frame_buffer_t *frame = NULL;
    ucamera_device_t *dev = ucamera_get_device();
    if (!dev) {
        LOGE(TAG, "ucamera get device name fail\n");
        return NULL;
    }

    frame = dev->camera_get_frame(dev);
    return frame;
}

/*
 * Save frame of camera
 */
int ucamera_service_save_frame(frame_buffer_t *frame, const char *path)
{
    FILE *file = NULL;

    file = fopen(path, "wb");
    if (file == NULL) {
        LOGE(TAG, "opening output file fail\n");
        return -1;
    }
    if (fwrite(frame->buf, frame->len, 1, file) < 1) {
        LOGE(TAG, "write buf fail\n");
        fclose(file);
        return -1;
    }

    fclose(file);
    return 0;
}
