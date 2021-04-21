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
    AI_MODEL_COMPARING_FACEBODY,                  // 人脸比对
    AI_MODEL_GENERATE_HUMAN_ANIME_STYLE,          // 人物动漫化
    AI_MODEL_RECOGNIZE_EXPRESSION,                // 表情识别
    AI_MODEL_DETECT_OBJECT,                       // 目标检测
    AI_MODEL_DETECT_MAIN_BODY,                    // 主体检测
    AI_MODEL_SEGMENT_COMMON_IMAGE,                // 通用分割
    AI_MODEL_SEGMENT_FACE,                        // 人脸分割
    AI_MODEL_RECOGNIZE_IDENTITY_CARD_FACE_SIDE,   // 身份证正面识别
    AI_MODEL_RECOGNIZE_IDENTITY_CARD_BACK_SIDE,   // 身份证背面识别
    AI_MODEL_RECOGNIZE_BANK_CARD,                 // 银行卡识别
    AI_MODEL_RECOGNIZE_CHARACTER,                 // 文本识别
    AI_MODEL_CLASSIFYING_RUBBISH,                 // 垃圾分类
    AI_MODEL_DETECT_FRUITS,                       // 水果检测
    AI_MODEL_ERASE_PERSON,                        // 图像人体擦除
    AI_MODEL_EXTEND_IMAGE_STYLE,                  // 风格迁移
    AI_MODEL_MAX
} ai_model_t;

typedef int (*ai_engine_cb_t)(ai_result_t *result);

#endif // _AI_AGENT_COMMON_H_
