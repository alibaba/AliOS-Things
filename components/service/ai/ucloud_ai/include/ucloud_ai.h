/*
 * Copyright (C) 2020-2023 Alibaba Group Holding Limited
 */

#ifndef _UCLOUD_AI_H_
#define _UCLOUD_AI_H_
#include <stdint.h>
#include "ucloud_ai_facebody.h"
#include "ucloud_ai_objectdet.h"


typedef enum _ai_model_t {
    AI_MODEL_COMPARING_FACEBODY,
    AI_MODEL_GENERATE_HUMAN_ANIME_STYLE,
    AI_MODEL_RECOGNIZE_EXPRESSION,
    AI_MODEL_OBJECT_DETECT,
    AI_MODEL_MAX
} ai_model_t;

void set_ucloud_ai_model(ai_model_t model);
ai_model_t get_ucloud_ai_model(void);
int32_t ucloud_ai_init(void);
int32_t ucloud_ai_uninit(void);
int32_t ucloud_ai_connect(char *url);
void ucloud_ai_disconnect(void);
int32_t ucloud_ai_get_stream(char *url);
int32_t ucloud_ai_save_image(char *path);
uint8_t *ucloud_ai_upload_file(char *path);

#endif //_UCLOUD_AI_H_