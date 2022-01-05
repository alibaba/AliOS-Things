
/*
 * Copyright (C) 2020-2021 Alibaba Group Holding Limited
 */
#ifndef WRAPPER_IHAAS_IMAGE_CODEC_H
#define WRAPPER_IHAAS_IMAGE_CODEC_H

#include "base/include/base/HaasCommonImage.h"
#include "base/include/base/HaasImageCodecDef.h"
#ifdef __cplusplus
extern "C" {
#endif

    void* ImageCodecCreateInstance(CodecImageType_t type);
    void ImageCodecDestoryInstance(void* instance);
	int ImageCodecImgRead(void* instance, ImageBuffer_t **image, char * filename);
	int ImageCodecImgReadMulti(void* instance, ImageBuffer_t **images, char * filename);
	int ImageCodecImgWrite(void* instance, ImageBuffer_t *image, char * filename);
	int ImageCodecImgWriteMulti(void* instance, ImageBuffer_t **images, char * filename);
	int ImageCodecImgDecode(void* instance, void *addr, ImageBuffer_t **image);
    ImageBuffer_t * ImageCodecImgDecode2(void* instance, const char * filename);
	int ImageCodecImgEncode(void* instance, void *addr, ImageBuffer_t ** image);
	int ImageCodechaveImageReader(void* instance, char * filename);
	int ImageCodechaveImageWriter(void* instance, char * filename);

#ifdef __cplusplus
};
#endif

#endif
