/*
 * Copyright (C) 2021-2023 Alibaba Group Holding Limited
 */

#ifndef _CAMERA_DEVICE_H_
#define _CAMERA_DEVICE_H_
#include <stdint.h>
#include <stdbool.h>
#include "wifi_camera.h"
#include "uart_camera.h"
#include "ucamera_service.h"
#include "ucamera_common.h"

typedef enum {
    CAM_WIFI_STA_MODE,
    CAM_UART_MODE,
    CAM_TIMING_MODE
} cam_mode_t;

typedef enum {
    CAM_WIFI_TYPE,
    CAM_UART_TYPE,
    CAM_USB_TYPE,
} cam_dev_type_t;

typedef struct _wifi_cam_params_t {
    const char *url;
    const char *control_url;
} wifi_cam_params_t;

typedef struct _uart_cam_params_t {
    uint32_t bandrate;
    uint32_t rx;
    uint32_t tx;
    cam_mode_t mode;
    uint32_t led_brightness;
    uint32_t upload_time;
    char *wifi_ssid;
    char *wifi_pwd;
} uart_cam_params_t;

typedef union {
    uart_cam_params_t uart;
    wifi_cam_params_t wifi;
} ucamera_dev_params_t;

typedef struct _ucamera_device_t {
    /* * * */
    /* The name of this camera device */
    const char *name;

    bool is_dummy;

    char *stream_buf;
    int32_t stream_len;

    /*frame size*/
    frame_size_t frame_size;

    cam_dev_type_t type;
    /*params*/
    ucamera_dev_params_t params;

    command_t cmd;

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

    /*config camera device, this should be done after camera init, before connect*/
    int32_t (*camera_config) (struct _ucamera_device_t *dev);

    /*get camera frame*/
    frame_buffer_t *(*camera_get_frame) (struct _ucamera_device_t *dev);

    void (*camera_free) (struct _ucamera_device_t *dev);
} ucamera_device_t;

ucamera_device_t *ucamera_device_init(const char *dev_name);
void ucamera_device_uninit(void);
const char *ucamera_get_device_name(void);
ucamera_device_t *ucamera_get_device(void);

#endif // _CAMERA_DEVICE_H_
