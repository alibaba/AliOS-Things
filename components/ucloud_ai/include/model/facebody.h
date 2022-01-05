/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef _FACEBODY_H_
#define _FACEBODY_H_

#include "internal.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _ComparingFaceStruct {
    float confidence;
    RectStruct location;
} ComparingFaceStruct;

typedef struct _RecognizeExpressionStruct {
    char *expression;
    float probability;
    RectStruct location;
} RecognizeExpressionStruct;

typedef struct _GenerateHumanAnimeStyleStruct {
    char *url;
    char *image;
    int imageLen;
} GenerateHumanAnimeStyleStruct;

typedef struct _DetectPedestrianStruct {
    char *type;
    float score;
    RectStruct box;
} DetectPedestrianStruct;

typedef union _FacebodyResultStruct {
    ComparingFaceStruct face;
    RecognizeExpressionStruct expression;
    GenerateHumanAnimeStyleStruct anime;
    DetectPedestrianStruct pedestrian;
} FacebodyResultStruct;

int compareFace(char *urlA, char *urlB, AIModelCBFunc cb);
int recognizeExpression(char *url, AIModelCBFunc cb);
int generateHumanAnimeStyle(char *url, AIModelCBFunc cb);
int detectPedestrian(char *url, AIModelCBFunc cb);
#ifdef __cplusplus
}
#endif
#endif
