/*
 * Copyright (C) 2021-2023 Alibaba Group Holding Limited
 */

#ifndef _AI_AGENT_COMMON_H_
#define _AI_AGENT_COMMON_H_
#include <stdint.h>


#ifdef CONFIG_UCLOUD_AI_ENGINE_ENABLE
#include "ucloud_ai_common.h"
typedef ucloud_ai_result_t ai_result_t;
#else
typedef void ai_result_t;
#endif

typedef enum _ai_model_t {
    AI_MODEL_COMPARING_FACEBODY,
    AI_MODEL_GENERATE_HUMAN_ANIME_STYLE,
    AI_MODEL_RECOGNIZE_EXPRESSION,
    AI_MODEL_DETECT_OBJECT,
    AI_MODEL_DETECT_MAIN_BODY,
    AI_MODEL_SEGMENT_COMMON_IMAGE,
    AI_MODEL_SEGMENT_FACE,
    AI_MODEL_RECOGNIZE_IDENTITY_CARD_FACE_SIDE,
    AI_MODEL_RECOGNIZE_IDENTITY_CARD_BACK_SIDE,
    AI_MODEL_RECOGNIZE_BANK_CARD,
    AI_MODEL_RECOGNIZE_CHARACTER,
    AI_MODEL_CLASSIFYING_RUBBISH,
    AI_MODEL_DETECT_FRUITS,
    AI_MODEL_ERASE_PERSON,
    AI_MODEL_EXTEND_IMAGE_STYLE,
    AI_MODEL_MAX
} ai_model_t;

typedef int (*ai_engine_cb_t)(ai_result_t *result);

#endif // _AI_AGENT_COMMON_H_
