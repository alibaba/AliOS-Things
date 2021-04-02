/*
 * Copyright (C) 2021-2023 Alibaba Group Holding Limited
 */

#ifndef _IMAGESEG_H_
#define _IMAGESEG_H_

#include "internal.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _SegmentCommonStruct {
    char *url;
    char *image;
    int imageLen;
} SegmentCommonStruct;

typedef struct _SegmentFaceStruct {
    char *url;
    char *image;
    int imageLen;
    RectStruct location;
} SegmentFaceStruct;

typedef union _ImageSegResultStruct {
    SegmentCommonStruct common;
    SegmentFaceStruct face;
} ImageSegResultStruct;

int segmentCommonImage(char *url, AIModelCBFunc cb);
int segmentFace(char *url, AIModelCBFunc cb);
#ifdef __cplusplus
}
#endif
#endif
