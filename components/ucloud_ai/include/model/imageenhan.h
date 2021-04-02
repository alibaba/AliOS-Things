/*
 * Copyright (C) 2021-2023 Alibaba Group Holding Limited
 */

#ifndef _IMAGEENHAN_H_
#define _IMAGEENHAN_H_

#include "model/internal.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _ErasePersonStruct {
    char *url;
    char *image;
    int imageLen;
} ErasePersonStruct;

typedef struct _ExtendImageStyleStruct {
    char *majorUrl;
    char *majorImage;
    char *outImageUrl;
    char *outImage;
    int majorImageLen;
    int outImageLen;
} ExtendImageStyleStruct;

typedef union _ImageEnhanResultStruct {
    ErasePersonStruct person;
    ExtendImageStyleStruct style;
} ImageEnhanResultStruct;

int erasePerson(char *imageUrl, char *userMaskUrl, AIModelCBFunc cb);
int extendImageStyle(char *majorUrl, char *styleUrl, AIModelCBFunc cb);
#ifdef __cplusplus
}
#endif
#endif
