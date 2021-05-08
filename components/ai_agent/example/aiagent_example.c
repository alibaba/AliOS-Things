/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include "aiagent_service.h"
#include "aiagent_common.h"
#include "ulog/ulog.h"
#if AOS_COMP_CLI
#include "aos/cli.h"
#endif

#define TAG "aiagent_example"
#define LOG printf

#ifdef CONFIG_ALICLOUD_FACEBODY_ENABLE
static int facebody_compare_callback(ai_result_t *result)
{
    float confidence;
    int x, y, w, h;
    if (!result)
        return -1;

    confidence = result->facebody.face.confidence;
    x = result->facebody.face.location.x;
    y = result->facebody.face.location.y;
    w = result->facebody.face.location.w;
    h = result->facebody.face.location.h;
    LOG("Facebody comparing result:\n");
    LOG("confidence: %.1f\n", confidence);
    LOG("location at x: %d, y: %d, w: %d, h: %d\n", x, y, w, h);

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

    if (!expression && strlen(expression) > 0)
        return -1;

    LOG("Recognize expression result:\n");
    LOG("type: %s, probability: %.1f\n", expression, face_probability);

    return 0;
}

static int generate_human_anime_styple_callback(ai_result_t *result)
{
    int ret;
    char *url = NULL;

    if (!result)
        return -1;

    LOG("Generate human anime style result:\n");
    url = result->facebody.anime.url;
    if (!url && strlen(url) > 0)
        return -1;

    LOG("url: %s\n", url);
    return ret;
}
#endif

#ifdef CONFIG_ALICLOUD_OBJECTDET_ENABLE
static int detect_object_callback(ai_result_t *result)
{
    int len = 0;
    int x, y, w, h;
    char *type = NULL;
    float score;

    if (!result)
        return -1;

    LOG("Detect object result:\n");
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

    LOG("type: %s, Score: %.1f, x: %d, y: %d, w: %d, h: %d\n", type, score, x, y, w, h);

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

    LOG("Detect main body result:\n");
    LOG("main body location x: %d, y: %d, w: %d, h: %d\n", x, y, w, h);

    return 0;
}
#endif

#ifdef CONFIG_ALICLOUD_IMAGESEG_ENABLE
static int segment_common_image_callback(ai_result_t *result)
{
    int ret;
    char *url = NULL;
    int x, y, w, h;
    if (!result)
        return -1;

    LOG("Segment common image result:\n");
    url = result->imageseg.common.url;
    if (!url && strlen(url) > 0)
        return -1;

    LOG("url: %s\n", url);

    return 0;
}

static int segment_face_callback(ai_result_t *result)
{
    int ret;
    int image_len = 0;
    int x, y, w, h;
    char *url = NULL;
    char path[32] = {0};

    if (!result)
        return -1;

    LOG("Segment face result:\n");
    url = result->imageseg.face.url;
    x = result->imageseg.face.location.x;
    y = result->imageseg.face.location.y;
    w = result->imageseg.face.location.w;
    h = result->imageseg.face.location.h;

    if (!url && strlen(url) > 0)
        return -1;
    LOG("image url: %s\n", url);
    LOG("location at x: %d, y: %d, w: %d, h: %d\n", x, y, w, h);

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
    int card_x[4], card_y[4], face_x[4], face_y[4];

    if (!result)
        return -1;

    LOG("Recognize identity card face side result:\n");
    /*address string*/
    address = result->ocr.identity.face.address;
    if (address && strlen(address) > 0)
        LOG("address: %s\n", address);

    /*birthdate string*/
    birthdate = result->ocr.identity.face.birthDate;
    if (birthdate && strlen(birthdate) > 0)
        LOG("birthdate: %s\n", birthdate);

    /*gender string*/
    gender = result->ocr.identity.face.gender;
    if (gender && strlen(gender) > 0)
        LOG("gender: %s\n", gender);

    /*nationality string*/
    nationality = result->ocr.identity.face.nationality;
    if (nationality && strlen(nationality) > 0)
        LOG("nationality: %s\n", nationality);

    /*id number string*/
    id_num = result->ocr.identity.face.iDNumber;
    if (id_num && strlen(id_num) > 0)
        LOG("id number: %s\n", id_num);

    /*card box line*/
    memcpy(card_x, result->ocr.identity.face.cardX, 4);
    memcpy(card_y, result->ocr.identity.face.cardY, 4);

    if (card_x && card_y) {
        LOG("card location: x0: %d, y0: %d\n", card_x[0], card_x[0]);
        LOG("card location: x1: %d, y1: %d\n", card_x[1], card_x[1]);
        LOG("card location: x2: %d, y2: %d\n", card_x[2], card_x[2]);
        LOG("card location: x3: %d, y3: %d\n", card_x[3], card_x[3]);
    }

    /*face box line*/
    memcpy(face_x, result->ocr.identity.face.faceX, 4);
    memcpy(face_y, result->ocr.identity.face.faceY, 4);
    if (face_x && face_y) {
        LOG("face location: x0: %d, y0: %d\n", card_x[0], card_x[0]);
        LOG("face location: x1: %d, y1: %d\n", card_x[1], card_x[1]);
        LOG("face location: x2: %d, y2: %d\n", card_x[2], card_x[2]);
        LOG("face location: x3: %d, y3: %d\n", card_x[3], card_x[3]);
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

    LOG("Recognize identity card back side result:\n");
    /*start date of identity card's back side*/
    start_date = result->ocr.identity.back.startDate;
    if (start_date && strlen(start_date) > 0)
        LOG("start date: %s\n", start_date);

    /*issue of identity card's back side*/
    issue = result->ocr.identity.back.issue;
    if (issue && strlen(issue) > 0)
        LOG("issue: %s\n", issue);

    /*end date of identity card's back side*/
    end_date = result->ocr.identity.back.endDate;
    if (end_date && strlen(end_date) > 0)
        LOG("end date: %s\n", end_date);

    return 0;
}

static int recognize_bank_card_callback(ai_result_t *result)
{
    char *bank_name = NULL;
    char *card_number = NULL;
    char *valid_date = NULL;

    if (!result)
        return -1;

    LOG("Recognize bank card result:\n");
    /*bank name of bank card*/
    bank_name = result->ocr.bank.bankName;
    if (bank_name && strlen(bank_name) > 0)
        LOG("bank name: %s\n", bank_name);

    /*card number of bank card*/
    card_number = result->ocr.bank.cardNumber;
    if (card_number && strlen(card_number) > 0)
        LOG("card number: %s\n", card_number);

    /*valid date of bank card*/
    valid_date = result->ocr.bank.validDate;
    if (valid_date && strlen(valid_date) > 0)
        LOG("valid date: %s\n", valid_date);

    return 0;
}

static int recognize_character_callback(ai_result_t *result)
{
    float probability;
    char *text = NULL;
    int left, top;
    int width, height;

    if (!result)
        return -1;

    LOG("Recognize character result:\n");
    text = result->ocr.character.text;
    left = result->ocr.character.left;
    top = result->ocr.character.top;
    width = result->ocr.character.width;
    height = result->ocr.character.height;
    probability = result->ocr.character.probability;
    if (text) {
        LOG("text: %s\n", text);
        LOG("probability: %.1f\n", probability);
        LOG("text area: left: %d, top: %d, weight: %d, height: %d\n", top, left, width, height);
    }

    return 0;
}
#endif

#ifdef CONFIG_ALICLOUD_IMAGERECOG_ENABLE
static int imagerecog_classifying_rubbish_callback(ai_result_t *result)
{
    char *rubbish = NULL;
    char *category = NULL;
    float rubbish_score;
    float category_score;

    if (!result)
        return -1;

    LOG("Recognize rubbish result:\n");
    /*rubbish name*/
    rubbish = result->imagerecog.rubbish.rubbish;
    rubbish_score = result->imagerecog.rubbish.rubbishScore;
    if (rubbish && strlen(rubbish) > 0) {
        LOG("rubbish: %s\n", rubbish);
        LOG("rubbish score: %.1f\n", rubbish_score);
    }

    /*rubbish category*/
    category = result->imagerecog.rubbish.category;
    category_score = result->imagerecog.rubbish.categoryScore;
    if (category && strlen(category) > 0) {
        LOG("category: %s\n", category);
        LOG("category score: %.1f\n", category_score);
    }

    return 0;
}

static int imagerecog_detect_fruits_callback(ai_result_t *result)
{
    char score_str[8];
    char *name = NULL;
    int tmp_y, x, y, w, h;
    float score;

    if (!result)
        return -1;

    LOG("Recognize fruits result:\n");
    /*fruits name and score*/
    name = result->imagerecog.fruits.name;
    score = result->imagerecog.fruits.score;
    x = result->imagerecog.fruits.box.x;
    y = result->imagerecog.fruits.box.y;
    w = result->imagerecog.fruits.box.w;
    h = result->imagerecog.fruits.box.h;
    if (name && strlen(name) > 0) {
        LOG("fruit name: %s\n", name);
        LOG("fruit score: %.1f\n", score);
        LOG("fruit location: x: %d, y: %d, w: %d, h: %d\n", x, y, w, h);
    }

    return 0;
}
#endif

#ifdef CONFIG_ALICLOUD_IMAGEENHAN_ENABLE
static int imageenhan_erase_person_callback(ai_result_t *result)
{
    int ret;
    char *url = NULL;

    if (!result)
        return -1;

    LOG("Erase person result:\n");
    url = result->imageenhan.person.url;
    LOG("url: %s\n", url);

    return 0;
}

static int imageenhan_extend_image_style_callback(ai_result_t *result)
{
    int ret;
    int major_image_len = 0;
    int out_image_len = 0;
    char *major_url = NULL;
    char *out_image_url = NULL;
    char *p_major_url = NULL;
    char *p_out_image_url = NULL;

    if (!result)
        return -1;

    major_url = result->imageenhan.style.majorUrl;
    out_image_url = result->imageenhan.style.outImageUrl;

    LOG("Extend image style result:\n");
    if (major_url && strlen(major_url) > 0) {
        LOG("major url: %s\n", major_url);
    }
    if (out_image_url && strlen(out_image_url) > 0) {
        LOG("out image url: %s\n", out_image_url);
    }

    return 0;
}

#endif // UCLOUD_AI_IMAGERECOG_CONFIG

static void aiagent_comp_example(int argc, char **argv)
{
    int model_index = -1;
    int ret = -1;
    char *image1 = NULL;
    char *image2 = NULL;
    ai_engine_cb_t cb;

    if (argc < 4) {
        LOG("Please test with command: aiagent_example -m [0~14]\n");
        return;
    }

    if (!strncmp(argv[1], "-e", 2)) {
        if (strncmp(argv[2], "ucloud-ai", 9)) {
            LOG("not support ai engine, currently we support ucloud-ai engine\n");
            return;
        }
    } else {
        LOG("unkown %d command\n", argv[1]);
        return;
    }

    if (!strncmp(argv[3], "init", 4)) {
        /*init network*/
        event_service_init(NULL);
        netmgr_service_init(NULL);

        LOG("aiagent init successfully!\n");
        return;
    } else if (!strncmp(argv[3], "-m", 2)) {
        model_index = atoi(argv[4]);
        if (model_index < 0 && model_index > 14) {
            LOGE(TAG, "range of model value is 0 ~ 14, please try again\n");
            return;
        }
    } else {
        LOG("unkown command\n");
        return;
    }

    ret = aiagent_service_init(argv[2], model_index);
    if (ret < 0) {
        LOGE(TAG, "aiagent service init fail, ret:%d\r\n", ret);
        return;
    }

    /*get callback function based on current model*/
    switch (aiagent_service_get_cur_model()) {
#ifdef CONFIG_ALICLOUD_FACEBODY_ENABLE
        case AI_MODEL_COMPARING_FACEBODY:
            cb = facebody_compare_callback;
            image1 = FACE1_IMAGE;
            image2 = FACE2_IMAGE;
            break;
        case AI_MODEL_GENERATE_HUMAN_ANIME_STYLE:
            cb = generate_human_anime_styple_callback;
            image1 = ANIME_IMAGE;
            image2 = NULL;
            break;
        case AI_MODEL_RECOGNIZE_EXPRESSION:
            cb = recognize_expression_callback;
            image1 = EXPRESSION_IMAGE;
            image2 = NULL;
            break;
            break;
#endif
#ifdef CONFIG_ALICLOUD_OBJECTDET_ENABLE
        case AI_MODEL_DETECT_OBJECT:
            cb = detect_object_callback;
            image1 = OBJECT_IMAGE;
            image2 = NULL;
            break;
        case AI_MODEL_DETECT_MAIN_BODY:
            cb = detect_main_body_callback;
            image1 = MAINBODY_IMAGE;
            image2 = NULL;
            break;
#endif
#ifdef CONFIG_ALICLOUD_IMAGESEG_ENABLE
        case AI_MODEL_SEGMENT_COMMON_IMAGE:
            cb = segment_common_image_callback;
            image1 = MAINBODY_IMAGE;
            image2 = NULL;
            break;
        case AI_MODEL_SEGMENT_FACE:
            cb = segment_face_callback;
            image1 = FACE1_IMAGE;
            image2 = NULL;
            break;
#endif
#ifdef CONFIG_ALICLOUD_OCR_ENABLE
        case AI_MODEL_RECOGNIZE_IDENTITY_CARD_FACE_SIDE:
            cb = recognize_identity_card_face_side_callback;
            image1 = CARD_FACE_IMAGE;
            image2 = NULL;
            break;
        case AI_MODEL_RECOGNIZE_IDENTITY_CARD_BACK_SIDE:
            cb = recognize_identity_card_back_side_callback;
            image1 = CARD_BACK_IMAGE;
            image2 = NULL;
            break;
        case AI_MODEL_RECOGNIZE_BANK_CARD:
            cb = recognize_bank_card_callback;
            image1 = BANK_CARD_IMAGE;
            image2 = NULL;
            break;
        case AI_MODEL_RECOGNIZE_CHARACTER:
            cb = recognize_character_callback;
            image1 = CHARACTER_IMAGE;
            image2 = NULL;
            break;
#endif
#ifdef CONFIG_ALICLOUD_IMAGERECOG_ENABLE
        case AI_MODEL_CLASSIFYING_RUBBISH:
            cb = imagerecog_classifying_rubbish_callback;
            image1 = RUBBISH_IMAGE;
            image2 = NULL;
            break;
        case AI_MODEL_DETECT_FRUITS:
            cb = imagerecog_detect_fruits_callback;
            image1 = FRUITS_IMAGE;
            image2 = NULL;
            break;
#endif
#ifdef CONFIG_ALICLOUD_IMAGEENHAN_ENABLE
        case AI_MODEL_ERASE_PERSON:
            cb = imageenhan_erase_person_callback;
            image1 = PERSON_ORG_IMAGE;
            image2 = NULL;
            break;
        case AI_MODEL_EXTEND_IMAGE_STYLE:
            cb = imageenhan_extend_image_style_callback;
            image1 = STYLE_IMAGE;
            image2 = NULL;
            break;
#endif
        default:
            cb = NULL;
            image1 = NULL;
            image2 = NULL;
            break;
    }

    /*do ai model inference*/
    if (cb)
        aiagent_service_model_infer(image1, image2, (ai_engine_cb_t)cb);

    ret = aiagent_service_uninit();
    if (ret < 0) {
        LOGE(TAG, "aiagent_service_uninit fail, ret:%d\r\n", ret);
        return;
    }
    return;
}

#if AOS_COMP_CLI
/* reg args: fun, cmd, description*/
ALIOS_CLI_CMD_REGISTER(aiagent_comp_example, aiagent, aiagent component base example)
#endif
