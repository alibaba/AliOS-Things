/*
 * Copyright (C) 2021-2023 Alibaba Group Holding Limited
 */

#ifndef _IMAGERECOG_H_
#define _IMAGERECOG_H_

#include "internal.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _ClassifyingRubbishStruct {
    char *rubbish;
    char *category;
    float categoryScore;
    float rubbishScore;
} ClassifyingRubbishStruct;

typedef struct _DetectFruitsStruct {
    char *name;
    float score;
    RectStruct box;
} DetectFruitsStruct;

typedef union _ImageRecogResultStruct {
    ClassifyingRubbishStruct rubbish;
    DetectFruitsStruct fruits;
} ImageRecogResultStruct;

int classifyingRubbish(char *url, AIModelCBFunc cb);
int detectFruits(char *url, AIModelCBFunc cb);

#ifdef __cplusplus
}
#endif
#endif
