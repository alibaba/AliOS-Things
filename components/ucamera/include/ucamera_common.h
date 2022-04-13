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
    FRAME_FORMAT_RGB565,
    FRAME_FORMAT_JPEG,
    FRAME_FORMAT_PNG,
    FRAME_FORMAT_YUV,
    FRAME_FORMAT_MAX
} frame_format_t;
#endif // _CAMERA_COMMON_H_
