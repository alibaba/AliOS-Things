
/*
 * Copyright (C) 2020-2021 Alibaba Group Holding Limited
 */
#ifndef WRAPPER_IHAAS_IMAGE_PROC_H
#define WRAPPER_IHAAS_IMAGE_PROC_H

#include "base/include/base/HaasCommonImage.h"
#include "base/include/base/HaasImageProcDef.h"
#ifdef __cplusplus
extern "C" {
#endif

    void* ImageProcCreateInstance(ImageProcType_t type);
    void ImageProcDestoryInstance(void* instance);
    int ImageProcOpen(void* instance);
    int ImageProcClose(void* instance);
    int ImageProcCvtColor(void* instance, const ImageBuffer_t* src, PixelFmt_t dst_format,
            ImageBuffer_t** dst);
    int ImageProcResize(void* instance, const ImageBuffer_t* src, const ImageSize_t dst_size,
            ImageBuffer_t** dst);
    int ImageProcImgCopy(void* instance, const ImageBuffer_t* src, ImageBuffer_t** dst);
    int ImageProcRectangle(void* instance, const ImageBuffer_t* src, int32_t left, int32_t top,
            int32_t right, int32_t bottom);
    int ImageProcCircle(void* instance, const ImageBuffer_t* src, int32_t center_x, int32_t center_y,
            int32_t radius);
    int ImageProcDrawMarker(void* instance, const ImageBuffer_t* src, Point_t *pt, int32_t type);
    int ImageProcFillPoly(void* instance, const ImageBuffer_t* src, Point_t** pts, int32_t color);
    int ImageProcPutText(void* instance, const ImageBuffer_t* src, char* text, int32_t left,
            int32_t top);
    int ImageProcDilateErode(void* instance, const ImageBuffer_t* src, ImageBuffer_t* dst,
            int32_t iMode, int32_t iThresh);
    void ImageProcImageAdd(void* instance, const ImageBuffer_t* src, ImageBuffer_t* added,
            ImageBuffer_t* dst);
    void ImageProcImageMinus(void* instance, const ImageBuffer_t* src, ImageBuffer_t* minused,
            ImageBuffer_t* dst);
    void ImageProcImageElementMultiply(void* instance, const ImageBuffer_t* src,
            ImageBuffer_t* multiplied, ImageBuffer_t* dst);
    void ImageProcImageElementDivide(void* instance, const ImageBuffer_t* src,
            ImageBuffer_t* divied, ImageBuffer_t* dst);
    int ImageProcWriteImageToFile(void* instance, const ImageBuffer_t* src, char* file_name);

#ifdef __cplusplus
};
#endif

#endif
