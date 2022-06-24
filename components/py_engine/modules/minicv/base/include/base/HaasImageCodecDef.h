/*
 * Copyright (C) 2020-2021 Alibaba Group Holding Limited
 */
#ifndef HAAS_IMAGE_CODEC_DEF_H
#define HAAS_IMAGE_CODEC_DEF_H

#include "base/include/base/HaasCommonImage.h"

/**@enum CodecImageType_t
* @brief Codec Image Type \n
*/
typedef enum {
    CODEC_IMAGE_SOURCE_NONE         = -1,
    CODEC_IMAG_SOURCE_IMAGE_PNG     = 0,
    CODEC_IMAG_SOURCE_IMAGE_JPG     = 1,
    CODEC_IMAG_SOURCE_IMAGE_BMP     = 2,
    CODEC_IMAG_SOURCE_IMAGE_GIF     = 3,
    CODEC_IMAG_SOURCE_MAX           = 30,
} CodecImageType_t;
#endif
