/*
 * Copyright (C) 2021-2023 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include "ulog/ulog.h"
#include "aos/kernel.h"
#include "aiagent_engine.h"
#include "engine/ucloud_ai_engine.h"
#include "aiagent_service.h"
#include "ucloud_ai_common.h"
#include "aiconfig.h"

#define TAG "UCLOUD_AI_ENGINE"

static void ucloud_ai_engine_delete(aiagent_engine_t *eng)
{
    if (!eng)
        return -1;

    free(eng);
}

static bool ucloud_ai_engine_available(void)
{
    return true;
}

int32_t ucloud_ai_engine_init(aiagent_engine_t *eng)
{
    int32_t ret;

    if (!eng)
        return -1;

    ret = ucloud_ai_init();
    if (ret < 0) {
        LOGE(TAG, "ucloud_ai_init failed, ret: %d\n", ret);
        return -1;
    }

    ucloud_ai_set_key_secret(OSS_ACCESS_KEY, OSS_ACCESS_SECRET);
    ucloud_ai_set_oss_endpoint(OSS_ENDPOINT);
    ucloud_ai_set_oss_bucket(OSS_BUCKET);
}

int32_t ucloud_ai_engine_uninit(aiagent_engine_t *eng)
{
    if (!eng)
        return -1;

    return ucloud_ai_uninit();
}

int ucloud_ai_engine_model_infer(aiagent_engine_t *eng)
{
    if (!eng)
        return -1;

    ucloud_ai_cb_t cb = (ucloud_ai_cb_t)eng->callback;
    switch (eng->model) {
#ifdef CONFIG_ALICLOUD_FACEBODY_ENABLE
        case UCLOUD_AI_MODEL_COMPARING_FACEBODY:
            ucloud_ai_facebody_comparing_face(eng->src1, eng->src2, cb);
            break;
        case UCLOUD_AI_MODEL_GENERATE_HUMAN_ANIME_STYLE:
            ucloud_ai_facebody_generate_human_anime_style(eng->src1, cb);
            break;
        case UCLOUD_AI_MODEL_RECOGNIZE_EXPRESSION:
            ucloud_ai_facebody_recognize_expression(eng->src1, cb);
            break;
#endif
#ifdef CONFIG_ALICLOUD_OBJECTDET_ENABLE
        case UCLOUD_AI_MODEL_DETECT_OBJECT:
            ucloud_ai_objectdet_detect_object(eng->src1, cb);
            break;
        case UCLOUD_AI_MODEL_DETECT_MAIN_BODY:
            ucloud_ai_objectdet_detect_main_body(eng->src1, cb);
            break;
#endif
#ifdef CONFIG_ALICLOUD_IMAGESEG_ENABLE
        case UCLOUD_AI_MODEL_SEGMENT_COMMON_IMAGE:
            ucloud_ai_imageseg_segment_common_image(eng->src1, cb);
            break;
        case UCLOUD_AI_MODEL_SEGMENT_FACE:
            ucloud_ai_imageseg_segment_face(eng->src1, cb);
            break;
#endif
#ifdef CONFIG_ALICLOUD_OCR_ENABLE
        case UCLOUD_AI_MODEL_RECOGNIZE_IDENTITY_CARD_FACE_SIDE:
            ucloud_ai_ocr_recognize_identity_card_face_side(eng->src1, cb);
            break;
        case UCLOUD_AI_MODEL_RECOGNIZE_IDENTITY_CARD_BACK_SIDE:
            ucloud_ai_ocr_recognize_identity_card_back_side(eng->src1, cb);
            break;
        case UCLOUD_AI_MODEL_RECOGNIZE_BANK_CARD:
            ucloud_ai_ocr_recognize_bank_card(eng->src1, cb);
            break;
        case UCLOUD_AI_MODEL_RECOGNIZE_CHARACTER:
            ucloud_ai_ocr_recognize_character(eng->src1, cb);
            break;
#endif
#ifdef CONFIG_ALICLOUD_IMAGERECOG_ENABLE
        case UCLOUD_AI_MODEL_CLASSIFYING_RUBBISH:
            ucloud_ai_imagerecog_classifying_rubbish(eng->src1, cb);
            break;
        case UCLOUD_AI_MODEL_DETECT_FRUITS:
            ucloud_ai_imagerecog_detect_fruits(eng->src1, cb);
            break;
#endif
#ifdef CONFIG_ALICLOUD_IMAGEENHAN_ENABLE
        case UCLOUD_AI_MODEL_ERASE_PERSON:
            ucloud_ai_imageenhan_erase_person(eng->src1, IMAGEENHAN_ERASE_PERSON_USERMASK_URL, cb);
            break;
        case UCLOUD_AI_MODEL_EXTEND_IMAGE_STYLE:
            ucloud_ai_imageenhan_extend_image_style(eng->src1, IMAGEENHAN_EXTEND_IMAGE_STYLE_URL, cb);
            break;
#endif
        default:
            break;
    }
    return 0;
}

static aiagent_engine_t *ucloud_ai_engine_create(int engineid)
{
    aiagent_engine_t *eng;

    /* Malloc ai agent eng*/
    eng = (aiagent_engine_t *) malloc(sizeof(aiagent_engine_t));
    if (!eng) {
        LOGE(TAG, "malloc camera device fail\n");
        return NULL;
    }
    eng->is_dummy = false;

    /* Set the function pointers */
    eng->ai_engine_init = ucloud_ai_engine_init;
    eng->ai_engine_uninit = ucloud_ai_engine_uninit;
    eng->ai_engine_model_infer = ucloud_ai_engine_model_infer;
    // eng->callback = ucloud_callback;
    eng->ai_engine_free = ucloud_ai_engine_delete;

    return eng;
}

aiagent_context_t ucloud_ai_engine = {
    UCLOUD_AI_NAME, "ucloud ai inference engine",
    ucloud_ai_engine_available, ucloud_ai_engine_create
};
