
/*
 * Copyright (C) 2020-2021 Alibaba Group Holding Limited
 */
#ifndef HAAS_COMMON_VIDEO_H
#define HAAS_COMMON_VIDEO_H

#include "base/include/base/HaasCommonImage.h"

/**@enum VideoCodecID_t
* @brief Video Codec ID Value \n
*/
typedef enum {
    VIDEO_CODEC_ID_NONE,
    VIDEO_CODEC_ID_H261,
    VIDEO_CODEC_ID_H263,
    VIDEO_CODEC_ID_MJPEG,
    VIDEO_CODEC_ID_LJPEG,
    VIDEO_CODEC_ID_H264,
    VIDEO_CODEC_ID_H265,
} VideoCodecID_t;

/**@struct VideoPacket_t
* @brief Video Packet information \n
*/
typedef struct {
    int64_t     pts;
    int64_t     dts;
    uint8_t		*data;
    int         size;
    int64_t     duration;
    int64_t     pos;
} VideoPacket_t;

/**@struct DecodeConfig_t
* @brief Decode Config information \n
*/
typedef struct {
    int          type;
    int         format;
    ImageSize_t *size;
    VideoPacket_t *data;
} DecodeConfig_t;


/**@struct ImageBuffer_t
* @brief Encode Config information \n
*/
typedef struct {
    int          type;
    int         format;
    ImageSize_t *size;
    ImageBuffer_t *data;
} EncodeConfig_t;

#endif
