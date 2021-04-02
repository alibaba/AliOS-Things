/*
 * Copyright (C) 2021-2023 Alibaba Group Holding Limited
 */

#ifndef _COMMON_H_
#define _COMMON_H_


#include <stdint.h>
#include "model/facebody.h"
#include "model/imageenhan.h"
#include "model/imagerecog.h"
#include "model/imageseg.h"
#include "model/objectdet.h"
#include "model/ocr.h"
#include "model/internal.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef union _AIModelResultStruct {
    FacebodyResultStruct facebody;
    ImageSegResultStruct imageseg;
    ImageEnhanResultStruct imageenhan;
    ImageRecogResultStruct imagerecog;
    ObjectDetResultStruct objectdet;
    OcrResultStruct ocr;
} AIModelResultStruct;


int getResponseBodyByUrl(const char *url, const char **buffer);
void setAccesskeyAndSecret(char *key, char *secret);

#ifdef __cplusplus
}
#endif
#endif // _COMMON_H_
