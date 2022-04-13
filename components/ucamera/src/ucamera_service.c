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
    int32_t ret;

    ucamera_device_t *dev = ucamera_device_init(dev_name);
    if (!dev) {
        LOGE(TAG, "ucamera service init fail\n");
        return -1;
    }

    ret = dev->camera_connect(dev);
    if (ret < 0)
        LOGE(TAG, "ucamera device connect fail\n");
    return ret;
}

/*
 * Uninitialize the ucamera service
 */
int32_t ucamera_service_uninit(void)
{
    int32_t ret;

    ucamera_device_t *dev = ucamera_get_device();
    if (!dev) {
        LOGE(TAG, "ucamera get device name fail\n");
        return -1;
    }
    ret = dev->camera_disconnect(dev);
    if (ret < 0)
        LOGE(TAG, "ucamera device disconnect fail\n");

    ucamera_device_uninit();

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
    FILE *jpeg_file = NULL;

    jpeg_file = fopen(path, "wb");
    if (jpeg_file == NULL) {
        LOGE(TAG, "opening output file fail\n");
        return -1;
    }
    if (fwrite(frame->buf, frame->len, 1, jpeg_file) < 1) {
        LOGE(TAG, "write buf fail\n");
        fclose(jpeg_file);
        return -1;
    }

    fclose(jpeg_file);
    return 0;
}
