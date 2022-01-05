/*
 * Copyright (C) 2020-2021 Alibaba Group Holding Limited
 */
#ifndef HAAS_VIDEOCODEC_DEF_H
#define HAAS_VIDEOCODEC_DEF_H

/**@enum VideoCodecType_t
* @brief Video Codec Type \n
*/
typedef enum {
    VIDEO_CODEC_NONE        = -1,
    VIDEO_CODEC_FFMPEG      = 0,
    VIDEO_CODEC_HAL         = 1,
    VIDEO_CODEC_MAX         = 30,
} VideoCodecType_t;
#endif
