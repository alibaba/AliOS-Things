/*
 * Copyright (C) 2021-2023 Alibaba Group Holding Limited
 */

#ifndef _INTERNAL_H_
#define _INTERNAL_H_

#define CLOUD_AI_REGION_ID "cn-shanghai"
#define CLOUD_AI_FACEBODY_ENDPOINT "facebody.cn-shanghai.aliyuncs.com"
#define CLOUD_AI_OBJECTDET_ENDPOINT "objectdet.cn-shanghai.aliyuncs.com"
#define CLOUD_AI_IMAGESEG_ENDPOINT "imageseg.cn-shanghai.aliyuncs.com"
#define CLOUD_AI_IMAGERECOG_ENDPOINT "imagerecog.cn-shanghai.aliyuncs.com"
#define CLOUD_AI_IMAGEENHAN_ENDPOINT "imageenhan.cn-shanghai.aliyuncs.com"
#define CLOUD_AI_OCR_ENDPOINT "ocr.cn-shanghai.aliyuncs.com"
#define OSS_ENDPOINT "oss-cn-shanghai.aliyuncs.com"

#ifdef __cplusplus
extern "C" {
#endif
typedef struct _RectStruct {
    float x;
    float y;
    float w;
    float h;
} RectStruct;

typedef int (*AIModelCBFunc)(void *result);

const char *getAccessKey(void);
const char *getAccessSecret(void);
#ifdef __cplusplus
}
#endif
#endif // _INTERNAL_H_
