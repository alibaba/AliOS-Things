
/*
 * Copyright (C) 2020-2021 Alibaba Group Holding Limited
 */
#ifndef WRAPPER_IHAAS_VIDEO_CODEC_H
#define WRAPPER_IHAAS_VIDEO_CODEC_H

#include "base/include/base/HaasCommonVideo.h"
#include "base/include/base/HaasCommonImage.h"
#include "base/include/base/HaasVideoCodecDef.h"
#ifdef __cplusplus
extern "C" {
#endif

    void* VideoCodecCreateInstance(VideoCodecType_t type);
    void VideoCodecDestoryInstance(void* instance);
	int VideoCodecOpen(void* instance);
	int VideoCodecClose(void* instance);
	int VideoCodecDecodeConfig(void* instance, DecodeConfig_t* config);
	int VideoCodecStartDecode(void* instance);
	int VideoCodecStopDecode(void* instance);
	int VideoCodecGetDecodeImageData(void* instance, void *pkt, ImageBuffer_t** image);
	int VideoCodecEncodeConfig(void* instance, EncodeConfig_t* config);
	int VideoCodecStartEncode(void* instance);
	int VideoCodecStopEncode(void* instance);
	int VideoCodecGetEncodePacketData(void* instance, VideoPacket_t **data);

#ifdef __cplusplus
};
#endif

#endif
