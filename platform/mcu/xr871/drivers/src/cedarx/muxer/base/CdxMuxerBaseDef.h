/*
 * Copyright (c) 2008-2016 Allwinner Technology Co. Ltd.
 * All rights reserved.
 *
 * File : CdxMuxerBaseDef.h
 * Description : Allwinner Muxer Base Definition
 * History :
 *
 */

#ifndef CDX_MUXER_BASE_DEF_H
#define CDX_MUXER_BASE_DEF_H

typedef enum {
    CODEC_TYPE_UNKNOWN   = -1,
    CODEC_TYPE_VIDEO     = 0, //don't change it!
    CODEC_TYPE_AUDIO     = 1, //don't change it!
    CODEC_TYPE_SUBTITLE,
    CODEC_TYPE_DATA
}CodecType;

typedef enum {
    MUX_CODEC_ID_MPEG4   = 32,
    MUX_CODEC_ID_H264    = 33,
    MUX_CODEC_ID_MJPEG   = 108,
    MUX_CODEC_ID_AAC     = 64,
    MUX_CODEC_ID_MP3     = 105,
    MUX_CODEC_ID_PCM,          // 16-bit little-endian, twos-complement
    MUX_CODEC_ID_ADPCM,        // DVI/Intel IMAADPCM-ACM code 17
}MuxCodecID;

#endif /* CDX_MUXER_BASE_DEF_H */
