/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include "ulog/ulog.h"
#include "aos/kernel.h"
#include "ugraphics.h"
#include "aiconfig.h"
#include "aiagent_service.h"
#include "ucamera_service.h"

#ifdef AOS_OLED_SH1106
#include "sh1106.h"
#endif

#ifdef AOS_USE_BEEPER
#include "beeper.h"
#endif

#define TAG "UCLOUD_AI"
#define CAPTURED_IMAGE "/data/capture.jpg"

// #define USE_CURL
#define LOG printf

#include <httpclient.h>

#ifdef CONFIG_ALICLOUD_FACEBODY_ENABLE
static float g_confidence = 0.0;
static char *p_expression = NULL;
#endif

extern void user_post_event(char type_t);

#ifdef CONFIG_ALICLOUD_FACEBODY_ENABLE
static int facebody_compare_callback(ai_result_t *result)
{
    int ret;
    float confidence;
    int x, y, w, h;
    if (!result)
        return -1;

    confidence = result->facebody.face.confidence;
    x = result->facebody.face.location.x;
    y = result->facebody.face.location.y;
    w = result->facebody.face.location.w;
    h = result->facebody.face.location.h;

    /*judge if boss is coming*/
    if (confidence > 60) {
        LOG("==============================================\n");
        LOG("=================boss is coming===============\n");
        LOG("==============================================\n");
        /*post event to Iot platform*/
        // user_post_event(1);

        LOG("draw string\n");
        ret = ugraphics_draw_string("Warning!!!", 120, 100);
        ret = ugraphics_draw_string("boss is coming!!!", 100, 120);

        /*show string on OLED for HaaS EDK*/
#ifdef AOS_OLED_SH1106
        OLED_Clear();
        OLED_Show_String(24, 12, "Warning!!!", 12, 1);
        OLED_Show_String(12, 36, "Boss is coming!", 12, 1);
        OLED_Refresh_GRAM();
#endif

        /*beeper alarm for HaaS EDK*/
#ifdef AOS_USE_BEEPER
        beeper_start();
#endif
    } else {
#ifdef AOS_OLED_SH1106
        OLED_Clear();
        OLED_Show_String(12, 24, "It's safe now!", 12, 1);
        OLED_Refresh_GRAM();
#endif
        /*stop beeper alarm*/
#ifdef AOS_USE_BEEPER
        beeper_stop();
#endif
    }

    return 0;
}

static int recognize_expression_callback(ai_result_t *result)
{
    int len;
    char *expression = NULL;
    float face_probability;

    if (!result)
        return -1;

    expression = result->facebody.expression.expression;
    face_probability = result->facebody.expression.probability;

    if (!expression)
        return -1;

    ugraphics_draw_image("/data/ai_demo_image/sadness.jpg", 20, 20);
    ugraphics_draw_image("/data/ai_demo_image/happiness.jpg", 80, 20);
    ugraphics_draw_image("/data/ai_demo_image/surprise.jpg", 140, 20);

    LOG("expression: %s\n", expression);
    /*draw image to lcd screen*/
    if (!strcmp(expression, "sadness")) {
        ugraphics_draw_image("/data/ai_demo_image/right.jpg", 20, 70);
    } else if (!strcmp(expression, "happiness")) {
        ugraphics_draw_image("/data/ai_demo_image/right.jpg", 80, 70);
    } else if (!strcmp(expression, "surprise")) {
        ugraphics_draw_image("/data/ai_demo_image/right.jpg", 140, 70);
    } else {
        /*do nothing*/
    }

    return 0;
}

static int generate_human_anime_styple_callback(ai_result_t *result)
{
    int ret;
    int image_len;
    char *image = NULL;

    if (!result)
        return -1;

    /*save human anime style image to file system*/
    image_len = result->facebody.anime.imageLen;
    image = result->facebody.anime.image;
    if (!image) {
        LOGE(TAG, "image is null\n");
        return -1;
    }
    ret = ugraphics_save_image(image, image_len, "/data/humananime.png");
    if (ret < 0) {
        LOGE(TAG, "ugraphics_save_image fail\n");
        return -1;
    }

    /*draw image to lcd screen*/
    ugraphics_draw_image("/data/humananime.png", 0, 0);

    return ret;
}
#endif

#ifdef CONFIG_ALICLOUD_OBJECTDET_ENABLE
static int detect_object_callback(ai_result_t *result)
{
    int len = 0;
    char *p_type = NULL;
    int x, y, w, h;
    char *type = NULL;
    float score;

    if (!result)
        return -1;

    LOG("detect_object_callback\n");
    type = result->objectdet.object.type;
    score = result->objectdet.object.score;
    x = result->objectdet.object.box.x;
    y = result->objectdet.object.box.y;
    w = result->objectdet.object.box.w;
    h = result->objectdet.object.box.h;

    if (!type) {
        LOGE(TAG, "type is null\n");
        return -1;
    }

    /*draw image to lcd screen*/
    LOG("type: %s\n", type);
    ugraphics_draw_string(type, x, y);
    ugraphics_draw_rect(x, y, w, h);
    ugraphics_draw_rect(x + 1, y + 1, w - 2, h - 2);
    ugraphics_draw_rect(x + 2, y + 2, w - 4, h - 4);

    return 0;
}

static int detect_main_body_callback(ai_result_t *result)
{
    int x, y, w, h;

    if (!result)
        return -1;

    x = result->objectdet.mainbody.location.x;
    y = result->objectdet.mainbody.location.y;
    w = result->objectdet.mainbody.location.w;
    h = result->objectdet.mainbody.location.h;

    /*draw rect to lcd screen*/
    ugraphics_draw_rect(x, y, w, h);
    /*fill boarder*/
    ugraphics_draw_rect(x + 1, y + 1, w - 2, h - 2);
    ugraphics_draw_rect(x + 2, y + 2, w - 4, h - 4);

    return 0;
}
#endif

#ifdef CONFIG_ALICLOUD_IMAGESEG_ENABLE
static int segment_common_image_callback(ai_result_t *result)
{
    int ret;
    int image_len = 0;
    char *image = NULL;

    if (!result)
        return -1;

    /*save image to file system*/
    image = result->imageseg.common.image;
    image_len = result->imageseg.common.imageLen;
    if (!image) {
        LOGE(TAG, "image is null\n");
        return -1;
    }
    ret = ugraphics_save_image(image, image_len, "/data/segment_common_image.png");
    if (ret < 0) {
        LOGE(TAG, "ugraphics_save_image fail\n");
        return -1;
    }

    /*draw image to lcd screen*/
    ugraphics_draw_image("/data/segment_common_image.png", 0, 0);
    return 0;
}

static int segment_face_callback(ai_result_t *result)
{
    int ret;
    int image_len = 0;
    int x, y;
    int index;
    char *image = NULL;
    char path[32] = {0};

    if (!result)
        return -1;

    /*clear capture background*/
    ugraphics_clear();

    /*save image*/
    image = result->imageseg.face.image;
    image_len = result->imageseg.face.imageLen;
    ret = ugraphics_save_image(image, image_len, "/data/segment_face.png");
    if (ret < 0) {
        LOGE(TAG, "save_captured_image fail\n");
        return -1;
    }

    /*draw image to lcd screen*/
    x = result->imageseg.face.location.x;
    y = result->imageseg.face.location.y;
    ugraphics_draw_image("/data/segment_face.png", x, y);
    return 0;
}
#endif


#ifdef CONFIG_ALICLOUD_OCR_ENABLE
static int recognize_identity_card_face_side_callback(ai_result_t *result)
{
    char *address = NULL;
    char *birthdate = NULL;
    char *gender = NULL;
    char *nationality = NULL;
    char *id_num = NULL;
    float card_x[4], card_y[4], face_x[4], face_y[4];

    if (!result)
        return -1;

    /*draw address string*/
    address = result->ocr.identity.face.address;
    if (address && strlen(address) > 0) {
        ugraphics_draw_string(address, 20, 20);
    }

    /*draw birthdate string*/
    birthdate = result->ocr.identity.face.birthDate;
    if (birthdate && strlen(birthdate) > 0) {
        ugraphics_draw_string(birthdate, 20, 40);
    }

    /*draw gender string*/
    gender = result->ocr.identity.face.gender;
    if (gender && strlen(gender) > 0) {
        ugraphics_draw_string(gender, 20, 60);
    }

    /*draw nationality string*/
    nationality = result->ocr.identity.face.nationality;
    if (nationality && strlen(nationality) > 0) {
        ugraphics_draw_string(nationality, 20, 80);
    }

    /*draw id number string*/
    id_num = result->ocr.identity.face.iDNumber;
    if (id_num && strlen(id_num) > 0) {
        ugraphics_draw_string(id_num, 20, 100);
    }

    /*reset card and face postion*/
    memset(card_x, 0, 4 * sizeof(int));
    memset(card_y, 0, 4 * sizeof(int));
    memset(face_x, 0, 4 * sizeof(int));
    memset(face_y, 0, 4 * sizeof(int));

    /*draw card box line*/
    memcpy(card_x, result->ocr.identity.face.cardX, 4 * sizeof(int));
    memcpy(card_y, result->ocr.identity.face.cardY, 4 * sizeof(int));
    if (card_x && card_y) {
        ugraphics_draw_line(card_x[2], card_y[2], card_x[3], card_y[3]);
        ugraphics_draw_line(card_x[2], card_y[2] + 1, card_x[3], card_y[3] + 1);
        ugraphics_draw_line(card_x[2], card_y[2] + 2, card_x[3], card_y[3] + 2);

        ugraphics_draw_line(card_x[3], card_y[3], card_x[0], card_y[0]);
        ugraphics_draw_line(card_x[3] - 1, card_y[3], card_x[0] - 1, card_y[0]);
        ugraphics_draw_line(card_x[3] - 2, card_y[3], card_x[0] - 2, card_y[0]);

        ugraphics_draw_line(card_x[1], card_y[1], card_x[0], card_y[0]);
        ugraphics_draw_line(card_x[1], card_y[1] - 1, card_x[0], card_y[0] - 1);
        ugraphics_draw_line(card_x[1], card_y[1] - 2, card_x[0], card_y[0] - 2);

        ugraphics_draw_line(card_x[2], card_y[2], card_x[1], card_y[1]);
        ugraphics_draw_line(card_x[2] + 1, card_y[2], card_x[1] + 1, card_y[1]);
        ugraphics_draw_line(card_x[2] + 2, card_y[2], card_x[1] + 2, card_y[1]);
    }

    /*draw face box line*/
    memcpy(face_x, result->ocr.identity.face.faceX, 4 * sizeof(int));
    memcpy(face_y, result->ocr.identity.face.faceY, 4 * sizeof(int));
    if (face_x && face_y) {
        /*draw top line*/
        ugraphics_draw_line(face_x[0], face_y[0], face_x[1], face_y[1]);
        /*draw left line*/
        ugraphics_draw_line(face_x[0], face_y[0], face_x[3], face_y[3]);
        /*draw right line*/
        ugraphics_draw_line(face_x[1], face_y[1], face_x[2], face_y[2]);
        /*draw bottom line*/
        ugraphics_draw_line(face_x[3], face_y[3], face_x[2], face_y[2]);
    }

    return 0;
}

static int recognize_identity_card_back_side_callback(ai_result_t *result)
{
    char *start_date = NULL;
    char *issue = NULL;
    char *end_date = NULL;

    if (!result)
        return -1;

    /*draw start date of identity card's back side*/
    start_date = result->ocr.identity.back.startDate;
    if (start_date && strlen(start_date) > 0) {
        ugraphics_draw_string(start_date, 20, 20);
    }

    /*draw issue of identity card's back side*/
    issue = result->ocr.identity.back.issue;
    if (issue && strlen(issue) > 0) {
        ugraphics_draw_string(issue, 20, 40);
    }

    /*draw end date of identity card's back side*/
    end_date = result->ocr.identity.back.endDate;
    if (end_date && strlen(end_date) > 0) {
        ugraphics_draw_string(end_date, 20, 60);
    }

    return 0;
}

static int recognize_bank_card_callback(ai_result_t *result)
{
    char *bank_name = NULL;
    char *card_number = NULL;
    char *valid_date = NULL;

    if (!result)
        return -1;

    /*draw bank name of bank card*/
    bank_name = result->ocr.bank.bankName;
    if (bank_name && strlen(bank_name) > 0) {
        ugraphics_draw_string(bank_name, 20, 0);
    }

    /*draw card number of bank card*/
    card_number = result->ocr.bank.cardNumber;
    if (card_number && strlen(card_number) > 0) {
        ugraphics_draw_string(card_number, 20, 20);
    }

    /*draw valid date of bank card*/
    valid_date = result->ocr.bank.validDate;
    if (valid_date && strlen(valid_date) > 0) {
        ugraphics_draw_string(valid_date, 20, 40);
    }

    return 0;
}

static int recognize_character_callback(ai_result_t *result)
{
    char probability_str[8];
    float probability;
    char *text = NULL;
    int left, top;

    if (!result)
        return -1;

    /*draw character text*/
    text = result->ocr.character.text;
    left = result->ocr.character.left;
    top = result->ocr.character.top;
    probability = result->ocr.character.probability;
    if (text) {
        sprintf(probability_str, "%.2f", probability);
        ugraphics_draw_string(text, left, top);
        ugraphics_draw_string(probability_str, left, top + 20);
    }

    return 0;
}
#endif

#ifdef CONFIG_ALICLOUD_IMAGERECOG_ENABLE
static int imagerecog_classifying_rubbish_callback(ai_result_t *result)
{
    char rubbish_score_str[16], category_score_str[16];
    char *rubbish = NULL;
    char *category = NULL;
    float rubbish_score;
    float category_score;

    if (!result)
        return -1;

    /*draw rubbish name*/
    rubbish = result->imagerecog.rubbish.rubbish;
    rubbish_score = result->imagerecog.rubbish.rubbishScore;
    if (rubbish && strlen(rubbish) > 0) {
        sprintf(rubbish_score_str, "%.2f", rubbish_score);
        ugraphics_draw_string(rubbish, 20, 20);
        // ugraphics_draw_string(rubbish_score_str, 20, 40);
    }

    /*draw rubbish category*/
    category = result->imagerecog.rubbish.category;
    category_score = result->imagerecog.rubbish.categoryScore;
    if (category && strlen(category) > 0) {
        sprintf(category_score_str, "%.2f", category_score);
        ugraphics_draw_string(category, 20, 60);
        // ugraphics_draw_string(category_score_str, 20, 80);
    }
}

static int imagerecog_detect_fruits_callback(ai_result_t *result)
{
    char score_str[8];
    char *name = NULL;
    int tmp_y, x, y;
    float score;

    if (!result)
        return -1;

    /*draw fruits name and score*/
    name = result->imagerecog.fruits.name;
    score = result->imagerecog.fruits.score;
    x = result->imagerecog.fruits.box.x;
    y = result->imagerecog.fruits.box.y;
    if (name && strlen(name) > 0) {
        sprintf(score_str, "%.2f", score);
        if (y < 60)
            tmp_y = y;
        else
            tmp_y = y - 60;
        ugraphics_draw_string(name, x, tmp_y);
        ugraphics_draw_string(score_str, x, tmp_y + 40);
    }
}
#endif

#ifdef CONFIG_ALICLOUD_IMAGEENHAN_ENABLE
static int imageenhan_erase_person_callback(ai_result_t *result)
{
    int ret;
    int image_len = 0;
    char *image = NULL;

    if (!result)
        return -1;

    /*save image to file system*/
    image = result->imageenhan.person.image;
    image_len = result->imageenhan.person.imageLen;
    if (!image) {
        LOGE(TAG, "image is null\n");
        return -1;
    }
    ret = ugraphics_save_image(image, image_len, "/data/erase_person.png");
    if (ret < 0) {
        LOGE(TAG, "ugraphics_save_image fail\n");
        return -1;
    }

    /*draw image to lcd screen*/
    ugraphics_draw_image("/data/erase_person.png", 0, 0);
    return 0;
}

static int imageenhan_extend_image_style_callback(ai_result_t *result)
{
    int ret;
    int major_image_len = 0;
    int out_image_len = 0;
    char *major_image = NULL;
    char *out_image = NULL;

    if (!result)
        return -1;

    /*save image*/
    major_image = result->imageenhan.style.majorImage;
    major_image_len = result->imageenhan.style.majorImageLen;
    out_image = result->imageenhan.style.outImage;
    out_image_len = result->imageenhan.style.outImageLen;

    if (major_image) {
        /*save image*/
        ret = ugraphics_save_image(major_image, major_image_len, "/data/major_image.png");
        if (ret < 0) {
            LOGE(TAG, "ugraphics_save_image fail\n");
            return -1;
        }

        /*draw image to lcd screen*/
        ugraphics_draw_image("/data/major_image.png", 0, 0);
    } else if (out_image) {
        /*save image*/
        ret = ugraphics_save_image(out_image, out_image_len, "/data/final_image.png");
        if (ret < 0) {
            LOGE(TAG, "ugraphics_save_image fail\n");
            return -1;
        }
        /*draw image to lcd screen*/
        ugraphics_draw_image("/data/final_image.png", 0, 0);
    }
    return 0;
}

#endif // UCLOUD_AI_IMAGERECOG_CONFIG


/*
*** ucloud ai demo main thread
*/
int ucloud_ai_demo_main(void *p)
{
    int ret = 0;
    char *upload_url = NULL;
    char *image1 = CAPTURED_IMAGE;
    char *image2 = NULL;
    ai_engine_cb_t cb;

    LOG("start ucloud_ai_demo_main\n");
    /*init ucamera service*/
    ret = ucamera_service_init(WIFI_CAMERA_NAME);
    if (HTTP_SUCCESS != ret) {
        LOGE(TAG, "http_app_init failed");
        return -1;
    }

    /*init ai agent service*/
    ret = aiagent_service_init("ucloud-ai", AI_MODEL);
    if (ret < 0) {
        LOGE(TAG, "aiagent_service_init failed");
        return -1;
    }

    while (1) {
        frame_buffer_t *frame = ucamera_service_get_frame();
        if (!frame) {
            LOGE(TAG, "frame is null\n");
            continue;
        }
        /*save file to data folder*/
        ret = ugraphics_save_image(frame->buf, frame->len, image1);
        if (ret < 0) {
            LOGE(TAG, "ugraphics_save_image fail\n");
            continue;
        }

        /*draw camera frame*/
        ugraphics_draw_image(image1, 0, 0);

        /*get callback function based on current model*/
        switch (aiagent_service_get_cur_model()) {
#ifdef CONFIG_ALICLOUD_FACEBODY_ENABLE
            case AI_MODEL_COMPARING_FACEBODY:
                cb = facebody_compare_callback;
                image2 = MYFACE_PATH;
                break;
            case AI_MODEL_GENERATE_HUMAN_ANIME_STYLE:
                cb = generate_human_anime_styple_callback;
                break;
            case AI_MODEL_RECOGNIZE_EXPRESSION:
                cb = recognize_expression_callback;
                break;
#endif
#ifdef CONFIG_ALICLOUD_OBJECTDET_ENABLE
            case AI_MODEL_DETECT_OBJECT:
                cb = detect_object_callback;
                LOG("cb = detect_object_callback\n");
                break;
            case AI_MODEL_DETECT_MAIN_BODY:
                cb = detect_main_body_callback;
                break;
#endif
#ifdef CONFIG_ALICLOUD_IMAGESEG_ENABLE
            case AI_MODEL_SEGMENT_COMMON_IMAGE:
                cb = segment_common_image_callback;
                break;
            case AI_MODEL_SEGMENT_FACE:
                cb = segment_face_callback;
                break;
#endif
#ifdef CONFIG_ALICLOUD_OCR_ENABLE
            case AI_MODEL_RECOGNIZE_IDENTITY_CARD_FACE_SIDE:
                cb = recognize_identity_card_face_side_callback;
                break;
            case AI_MODEL_RECOGNIZE_IDENTITY_CARD_BACK_SIDE:
                cb = recognize_identity_card_back_side_callback;
                break;
            case AI_MODEL_RECOGNIZE_BANK_CARD:
                cb = recognize_bank_card_callback;
                break;
            case AI_MODEL_RECOGNIZE_CHARACTER:
                cb = recognize_character_callback;
                break;
#endif
#ifdef CONFIG_ALICLOUD_IMAGERECOG_ENABLE
            case AI_MODEL_CLASSIFYING_RUBBISH:
                cb = imagerecog_classifying_rubbish_callback;
                break;
            case AI_MODEL_DETECT_FRUITS:
                cb = imagerecog_detect_fruits_callback;
                break;
#endif
#ifdef CONFIG_ALICLOUD_IMAGEENHAN_ENABLE
            case AI_MODEL_ERASE_PERSON:
                cb = imageenhan_erase_person_callback;
                break;
            case AI_MODEL_EXTEND_IMAGE_STYLE:
                cb = imageenhan_extend_image_style_callback;
                break;
#endif
            default:
                cb = NULL;
                break;
        }

        /*do ai model inference*/
        if (cb)
            aiagent_service_model_infer(image1, image2, (ai_engine_cb_t)cb);

        /*flip image to lcd screen*/
        ugraphics_flip();
    }

    ucamera_service_uninit();
    aiagent_service_uninit();
    LOG("ucloud_ai_demo_main end\n");
    return 0;
}
