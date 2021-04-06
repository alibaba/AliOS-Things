/*
 * Copyright (C) 2021-2023 Alibaba Group Holding Limited
 */

#ifndef _CAMERA_SERVICE_H_
#define _CAMERA_SERVICE_H_
#include <stdint.h>
#include "ucamera_device.h"
#include "ucamera_common.h"

#define UCAMERA_PIX_FMT_YYUV    0
#define UCAMERA_PIX_FMT_AYUV32  1
#define UCAMERA_PIX_FMT_ARGB32  2
#define UCAMERA_PIX_FMT_BGRA32  3
#define UCAMERA_PIX_FMT_RGB24   4
#define UCAMERA_PIX_FMT_BGR24   5
#define UCAMERA_PIX_FMT_YUV420  6
#define UCAMERA_PIX_FMT_YUV420SP 7
#define UCAMERA_PIX_FMT_PNG      8
#define UCAMERA_PIX_FMT_JPEG     9


typedef struct _ucamera_context_t {
    const char *name;
    const char *desc;
    int (*available) (void);
    ucamera_device_t *(*create) (int devindex);
} ucamera_context_t;

extern ucamera_context_t wifi_camera;


/** @defgroup ucamera_aos_api ucamera
 * @{
 */

/**
 * Init the ucamera module.
 *
 * @param[in]  dev_name    device name.
 * @param[in]  dev_name    device name.
 *
 * @return  0 on success, negative error on failure.
 */
int32_t ucamera_service_init(const char *dev_name);

/**
 * Uninit the ucamera module.
 *
 * @return  0 on success, negative error on failure.
 */
int32_t ucamera_service_uninit(void);

/**
 * Get one frame of camera device.

 * @return  frame buffer content.
 */
frame_buffer_t *ucamera_service_get_frame(void);

/**
 * Init the ucamera module.
* @param[in]  frame  frame buffer content.
* @param[in]  path  path of saving image.

 * @return  0 on success, negative error on failure.
 */
int ucamera_service_save_frame(frame_buffer_t *frame, const char *path);

/**
 * @}
 */

#endif // _WIFI_CAMERA_H_
