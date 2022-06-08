/*
 * Copyright (C) 2020-2021 Alibaba Group Holding Limited
 */
#ifndef HAAS_DATA_INPUT_DEF_H
#define HAAS_DATA_INPUT_DEF_H

#include "base/include/base/HaasCommonImage.h"

/**@enum CameraNumber_t
* @brief Camera Number \n
*/
typedef enum {
    DATA_INPUT_CAMERA_NUMBER_NONE          = -1,
    DATA_INPUT_CAMERA_NUMBER_FRONT         = 0,
    DATA_INPUT_CAMERA_NUMBER_BACK          = 1,
    DATA_INPUT_CAMERA_NUMBER_MAX           = 30,
} CameraNumber_t;

/**@enum DataInputType_t
* @brief Input Data Source Type \n
*/
typedef enum {
    DATA_INPUT_SOURCE_NONE          = -1,
    DATA_INPUT_SOURCE_IMAGE_BMP     = 0,
    DATA_INPUT_SOURCE_IMAGE_GIF     = 1,
    DATA_INPUT_SOURCE_IMAGE_PNG     = 2,
    DATA_INPUT_SOURCE_IMAGE_JPG     = 3,
    DATA_INPUT_SOURCE_VIDEO_FILE    = 4,
    DATA_INPUT_SOURCE_VIDEO_RTMP    = 5,
    DATA_INPUT_SOURCE_VIDEO_RTSP    = 6,
    DATA_INPUT_SOURCE_CAMERA_USB    = 7,
    DATA_INPUT_SOURCE_CAMERA_LOCAL  = 8,
    DATA_INPUT_SOURCE_CAMERA_IP     = 9,
    DATA_INPUT_SOURCE_MAX           = 30,
} DataInputType_t;
#endif
