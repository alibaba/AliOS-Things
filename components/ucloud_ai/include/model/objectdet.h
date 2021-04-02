/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef _OBJECTDET_H_
#define _OBJECTDET_H_

#include "model/internal.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _DetectObjectStruct {
    char *type;
    float score;
    RectStruct box;
} DetectObjectStruct;

typedef struct _DetectMainBodyStruct {
    RectStruct location;
} DetectMainBodyStruct;

typedef union _ObjectDetResultStruct {
    DetectObjectStruct object;
    DetectMainBodyStruct mainbody;
} ObjectDetResultStruct;

int detectObject(char *url, AIModelCBFunc cb);
int detectMainBody(char *url, AIModelCBFunc cb);

#ifdef __cplusplus
}
#endif
#endif // _OBJECTDET_H_
