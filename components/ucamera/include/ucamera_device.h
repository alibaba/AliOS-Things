/*
 * Copyright (C) 2021-2023 Alibaba Group Holding Limited
 */

#ifndef _CAMERA_DEVICE_H_
#define _CAMERA_DEVICE_H_
#include <stdint.h>
#include <stdbool.h>
#include "wifi_camera.h"
#include "ucamera_service.h"
#include "ucamera_common.h"

typedef struct _ucamera_device_t {
    /* * * */
    /* The name of this camera device */
    const char *name;

    bool is_dummy;

    char *stream_buf;
    int32_t stream_len;

    /*frame size*/
    int16_t width;
    int16_t height;

    /*frame*/
    frame_buffer_t frame;

    /*init camera*/
    int32_t (*camera_init) (struct _ucamera_device_t *dev);

    /*uninit camera*/
    void (*camera_uninit) (struct _ucamera_device_t *dev);

    /*connect to camera device*/
    int32_t (*camera_connect) (struct _ucamera_device_t *dev);

    /*disconnect to camera device, this can be done after camera init*/
    int32_t (*camera_disconnect) (struct _ucamera_device_t *dev);

    /*get camera frame*/
    frame_buffer_t *(*camera_get_frame) (struct _ucamera_device_t *dev);

    void (*camera_free) (struct _ucamera_device_t *dev);
} ucamera_device_t;

ucamera_device_t *ucamera_device_init(const char *dev_name);
void ucamera_device_uninit(void);
const char *ucamera_get_device_name(void);
ucamera_device_t *ucamera_get_device(void);

#endif // _CAMERA_DEVICE_H_
