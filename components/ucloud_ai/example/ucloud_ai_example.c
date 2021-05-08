/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include "ucloud_ai_common.h"
#include "ulog/ulog.h"
#if AOS_COMP_CLI
#include "aos/cli.h"
#endif

#define TAG "uclud_ai_example"
#define LOG printf

static void ucloud_ai_comp_example(int argc, char **argv)
{
    int model_index = -1;
    int ret = -1;
    char *image = NULL;

    if (argc < 2) {
        LOG("Please test with command: ucloud_ai_example -m [0~14]\n");
        return;
    }

    if (!strncmp(argv[1], "init", 4)) {
        /*init network*/
        event_service_init(NULL);
        netmgr_service_init(NULL);
        LOG("ucloud_ai comp init successfully!\n");
        return;
    } else if (!strncmp(argv[1], "-m", 2)) {
        if (argv[2] < 0 && argv[2] > 14) {
            LOGE(TAG, "range of model value is 0 ~ 14, please try again\n");
            return;
        }
    } else {
        LOG("unkown command\n");
        return;
    }
    model_index = atoi(argv[2]);

    /*ucloud ai component init*/
    ret = ucloud_ai_init();
    if (ret < 0) {
        LOGE(TAG, "ucloud_ai_init failed, ret: %d\n", ret);
        return -1;
    }
    /*Config OSS Information*/
    ucloud_ai_set_key_secret(OSS_ACCESS_KEY, OSS_ACCESS_SECRET);
    ucloud_ai_set_oss_endpoint(OSS_ENDPOINT);
    ucloud_ai_set_oss_bucket(OSS_BUCKET);

    switch (model_index) {
#ifdef CONFIG_ALICLOUD_FACEBODY_ENABLE
        case UCLOUD_AI_MODEL_COMPARING_FACEBODY:
            LOG("Comparing facebody:\n");
            ucloud_ai_facebody_comparing_face(FACE1_IMAGE, FACE2_IMAGE, NULL);
            break;
        case UCLOUD_AI_MODEL_GENERATE_HUMAN_ANIME_STYLE:
            LOG("Generate human anime style:\n");
            ucloud_ai_facebody_generate_human_anime_style(ANIME_IMAGE, NULL);
            break;
        case UCLOUD_AI_MODEL_RECOGNIZE_EXPRESSION:
            LOG("Recognize expression:\n");
            ucloud_ai_facebody_recognize_expression(EXPRESSION_IMAGE, NULL);
            break;
#endif
#ifdef CONFIG_ALICLOUD_OBJECTDET_ENABLE
        case UCLOUD_AI_MODEL_DETECT_OBJECT:
            LOG("Detect object:\n");
            ucloud_ai_objectdet_detect_object(OBJECT_IMAGE, NULL);
            break;
        case UCLOUD_AI_MODEL_DETECT_MAIN_BODY:
            LOG("Detect main body:\n");
            ucloud_ai_objectdet_detect_main_body(MAINBODY_IMAGE, NULL);
            break;
#endif
#ifdef CONFIG_ALICLOUD_IMAGESEG_ENABLE
        case UCLOUD_AI_MODEL_SEGMENT_COMMON_IMAGE:
            LOG("Segment common image:\n");
            ucloud_ai_imageseg_segment_common_image(MAINBODY_IMAGE, NULL);
            break;
        case UCLOUD_AI_MODEL_SEGMENT_FACE:
            LOG("Segment face:\n");
            ucloud_ai_imageseg_segment_face(FACE1_IMAGE, NULL);
            break;
#endif
#ifdef CONFIG_ALICLOUD_OCR_ENABLE
        case UCLOUD_AI_MODEL_RECOGNIZE_IDENTITY_CARD_FACE_SIDE:
            LOG("Recognize identity card face side:\n");
            ucloud_ai_ocr_recognize_identity_card_face_side(CARD_FACE_IMAGE, NULL);
            break;
        case UCLOUD_AI_MODEL_RECOGNIZE_IDENTITY_CARD_BACK_SIDE:
            LOG("Recognize identity card back side:\n");
            ucloud_ai_ocr_recognize_identity_card_back_side(CARD_BACK_IMAGE, NULL);
            break;
        case UCLOUD_AI_MODEL_RECOGNIZE_BANK_CARD:
            LOG("Recognize bank card:\n");
            ucloud_ai_ocr_recognize_bank_card(BANK_CARD_IMAGE, NULL);
            break;
        case UCLOUD_AI_MODEL_RECOGNIZE_CHARACTER:
            LOG("Recognize character:\n");
            ucloud_ai_ocr_recognize_character(CHARACTER_IMAGE, NULL);
            break;
#endif
#ifdef CONFIG_ALICLOUD_IMAGERECOG_ENABLE
        case UCLOUD_AI_MODEL_CLASSIFYING_RUBBISH:
            LOG("Classifying rubbish:\n");
            ucloud_ai_imagerecog_classifying_rubbish(RUBBISH_IMAGE, NULL);
            break;
        case UCLOUD_AI_MODEL_DETECT_FRUITS:
            LOG("Detect fruits:\n");
            ucloud_ai_imagerecog_detect_fruits(FRUITS_IMAGE, NULL);
            break;
#endif
#ifdef CONFIG_ALICLOUD_IMAGEENHAN_ENABLE
        case UCLOUD_AI_MODEL_ERASE_PERSON:
            LOG("Erase person:\n");
            ucloud_ai_imageenhan_erase_person(PERSON_ORG_IMAGE, IMAGEENHAN_ERASE_PERSON_USERMASK_URL, NULL);
            break;
        case UCLOUD_AI_MODEL_EXTEND_IMAGE_STYLE:
            LOG("Extend image style:\n");
            ucloud_ai_imageenhan_extend_image_style(STYLE_IMAGE, IMAGEENHAN_EXTEND_IMAGE_STYLE_URL, NULL);
            break;
#endif
        default:
            break;
    }

    ret = ucloud_ai_uninit();
    if (ret < 0) {
        LOGE(TAG, "ucloud_ai_uninit failed, ret: %d\n", ret);
        return -1;
    }

    return;
}

#if AOS_COMP_CLI
/* reg args: fun, cmd, description*/
ALIOS_CLI_CMD_REGISTER(ucloud_ai_comp_example, ucloud_ai, ucloud_ai component base example)
#endif
