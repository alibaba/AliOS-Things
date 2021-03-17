/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef _OCR_H_
#define _OCR_H_

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct _OCRResult
    {
        int left;
        int angle;
        int top;
        int height;
        int width;
        float probability;
        char* text;
    } OCRResult;

    typedef int (*recognizeCharacter_cb_t)(OCRResult *result);

    int recognizeCharacter_config_ai(char *key, char *secret, char *region_id,
            char *endpoint, char *url, recognizeCharacter_cb_t cb);
#ifdef __cplusplus
}
#endif
#endif
