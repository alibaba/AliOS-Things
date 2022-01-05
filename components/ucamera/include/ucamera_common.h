/*
 * Copyright (C) 2021-2023 Alibaba Group Holding Limited
 */

#ifndef _CAMERA_COMMON_H_
#define _CAMERA_COMMON_H_
#include <stdint.h>

typedef struct _frame_buffer_t {
    uint8_t *buf;
    uint32_t len;
    uint16_t width;
    uint16_t height;
    uint32_t format;
} frame_buffer_t;

typedef enum {
    SIZE_160X120 = 1,
    SIZE_240X240 = 4,
    SIZE_320X240 = 5,
    SIZE_640X480 = 8,
    SIZE_800X600 = 9,
} size_id_t;

typedef struct _frame_size_t {
    uint16_t width;
    uint16_t height;
    size_id_t id;
} frame_size_t;

typedef enum {
    FRAME_FORMAT_RGB565,
    FRAME_FORMAT_JPEG,
    FRAME_FORMAT_BMP,
    FRAME_FORMAT_PNG,
    FRAME_FORMAT_YUV,
    FRAME_FORMAT_MAX
} frame_format_t;

typedef enum {
    // for wifi camera only
    UCAMERA_CMD_SET_URL,
    UCAMERA_CMD_SET_CONTROL_URL,

    // for uart camera only
    UCAMERA_CMD_SET_UART_PARAMS,
    UCAMERA_CMD_SET_UPLOAD_TIME,
    UCAMERA_CMD_SET_WIFI_SSID_PWD,
    UCAMERA_CMD_SET_LED_BRIGHTNESS,
    UCAMERA_CMD_SET_MODE,
    UCAMERA_CMD_GET_TOKEN,

    // common command
    UCAMERA_CMD_SET_FRAME_SIZE,
    UCAMERA_CMD_SET_MIRROR,
    UCAMERA_CMD_SET_FLIP,
    UCAMERA_CMD_SET_POSITION,
    UCAMERA_CMD_MAX
} command_t;
#endif // _CAMERA_COMMON_H_
