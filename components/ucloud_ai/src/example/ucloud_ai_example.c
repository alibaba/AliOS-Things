/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include "ucloud_ai_common.h"
#include "ulog/ulog.h"
#ifdef AOS_COMP_CLI
#include "aos/cli.h"
#endif

#define TAG "uclud_ai_example"


static void ucloud_ai_comp_example(int argc, char **argv)
{
    int model_index = -1;
    int ret = -1;
    char *image = NULL;

    if (argc < 3) {
        printf("Please test with command: ucloud_ai_example -m [0~14]\n");
        return;
    }

    if (strncmp(argv[1], "-m", 2) == 0) {
        if (argv[2] < 0 && argv[2] > 14) {
            printf("range of model value is 0 ~ 14, please try again\n");
            return;
        }
    } else {
        printf("unkown command\n");
        return;
    }
    model_index = atoi(argv[2]);

    /*Config OSS Information*/
    ucloud_ai_set_key_secret(OSS_ACCESS_KEY, OSS_ACCESS_SECRET);
    ucloud_ai_set_oss_endpoint(OSS_ENDPOINT);
    ucloud_ai_set_oss_bucket(OSS_BUCKET);

    switch (model_index) {
#ifdef CONFIG_ALICLOUD_FACEBODY_ENABLE
        case UCLOUD_AI_MODEL_COMPARING_FACEBODY:
            printf("Comparing facebody:\n");
            ucloud_ai_facebody_comparing_face(FACE1_IMAGE, FACE2_IMAGE, NULL);
            break;
        case UCLOUD_AI_MODEL_GENERATE_HUMAN_ANIME_STYLE:
            printf("Generate human anime style:\n");
            ucloud_ai_facebody_generate_human_anime_style(ANIME_IMAGE, NULL);
            break;
        case UCLOUD_AI_MODEL_RECOGNIZE_EXPRESSION:
            printf("Recognize expression:\n");
            ucloud_ai_facebody_recognize_expression(EXPRESSION_IMAGE, NULL);
            break;
#endif
#ifdef CONFIG_ALICLOUD_OBJECTDET_ENABLE
        case UCLOUD_AI_MODEL_DETECT_OBJECT:
            printf("Detect object:\n");
            ucloud_ai_objectdet_detect_object(OBJECT_IMAGE, NULL);
            break;
        case UCLOUD_AI_MODEL_DETECT_MAIN_BODY:
            printf("Detect main body:\n");
            ucloud_ai_objectdet_detect_main_body(MAINBODY_IMAGE, NULL);
            break;
#endif
#ifdef CONFIG_ALICLOUD_IMAGESEG_ENABLE
        case UCLOUD_AI_MODEL_SEGMENT_COMMON_IMAGE:
            printf("Segment common image:\n");
            ucloud_ai_imageseg_segment_common_image(MAINBODY_IMAGE, NULL);
            break;
        case UCLOUD_AI_MODEL_SEGMENT_FACE:
            printf("Segment face:\n");
            ucloud_ai_imageseg_segment_face(FACE1_IMAGE, NULL);
            break;
#endif
#ifdef CONFIG_ALICLOUD_OCR_ENABLE
        case UCLOUD_AI_MODEL_RECOGNIZE_IDENTITY_CARD_FACE_SIDE:
            printf("Recognize identity card face side:\n");
            ucloud_ai_ocr_recognize_identity_card_face_side(CARD_FACE_IMAGE, NULL);
            break;
        case UCLOUD_AI_MODEL_RECOGNIZE_IDENTITY_CARD_BACK_SIDE:
            printf("Recognize identity card back side:\n");
            ucloud_ai_ocr_recognize_identity_card_back_side(CARD_BACK_IMAGE, NULL);
            break;
        case UCLOUD_AI_MODEL_RECOGNIZE_BANK_CARD:
            printf("Recognize bank card:\n");
            ucloud_ai_ocr_recognize_bank_card(BANK_CARD_IMAGE, NULL);
            break;
        case UCLOUD_AI_MODEL_RECOGNIZE_CHARACTER:
            printf("Recognize character:\n");
            ucloud_ai_ocr_recognize_character(CHARACTER_IMAGE, NULL);
            break;
#endif
#ifdef CONFIG_ALICLOUD_IMAGERECOG_ENABLE
        case UCLOUD_AI_MODEL_CLASSIFYING_RUBBISH:
            printf("Classifying rubbish:\n");
            ucloud_ai_imagerecog_classifying_rubbish(RUBBISH_IMAGE, NULL);
            break;
        case UCLOUD_AI_MODEL_DETECT_FRUITS:
            printf("Detect fruits:\n");
            ucloud_ai_imagerecog_detect_fruits(FRUITS_IMAGE, NULL);
            break;
#endif
#ifdef CONFIG_ALICLOUD_IMAGEENHAN_ENABLE
        case UCLOUD_AI_MODEL_ERASE_PERSON:
            printf("Erase person:\n");
            ucloud_ai_imageenhan_erase_person(PERSON_ORG_IMAGE, IMAGEENHAN_ERASE_PERSON_USERMASK_URL, NULL);
            break;
        case UCLOUD_AI_MODEL_EXTEND_IMAGE_STYLE:
            printf("Extend image style:\n");
            ucloud_ai_imageenhan_extend_image_style(STYLE_IMAGE, IMAGEENHAN_EXTEND_IMAGE_STYLE_URL, NULL);
            break;
#endif
        default:
            break;
    }
    printf("ucloud_ai comp test success!\r\n");
    return;
}

#ifdef AOS_COMP_CLI
/* reg args: fun, cmd, description*/
ALIOS_CLI_CMD_REGISTER(ucloud_ai_comp_example, ucloud_ai, ucloud_ai component base example)
#endif
