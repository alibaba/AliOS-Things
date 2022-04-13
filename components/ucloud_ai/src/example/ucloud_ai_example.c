/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include "ucloud_ai_common.h"
#include "ulog/ulog.h"
#ifdef AOS_COMP_CLI
#include "aos/cli.h"
#endif

#define TAG "uclud_ai_example"

#define OSS_ACCESS_KEY "LTAI5tLXQ7Yuu113mf231YNg"
#define OSS_ACCESS_SECRET "ACUHFx92D2KO0x7jabmVGliuYtrlJx"
#define OSS_ENDPOINT "oss-cn-shanghai.aliyuncs.com"
#define OSS_BUCKET "cloud-ai-dev2"

#ifdef CONFIG_ALICLOUD_FACEBODY_ENABLE
static int facebody_compare_callback(ucloud_ai_result_t *result)
{
    float confidence;

    if (!result)
        return -1;

    LOG("Facebody comparing result:\n");
    LOG("confidence: %.1f\n", confidence);

    return 0;
}

static int recognize_expression_callback(ucloud_ai_result_t *result)
{
    int len;
    char *expression, *p_expression;
    float face_probability;

    if (!result)
        return -1;

    expression = result->facebody.expression.expression;
    face_probability = result->facebody.expression.probability;

    if (!expression)
        return -1;

    p_expression = strdup(expression);
    if (!p_expression) {
        LOGE(TAG, "p_expression strdup fail\n");
        return -1;
    }

    LOG("Recognize expression result:\n");
    LOG("type: %s, probability: %.1f\n", p_expression, face_probability);
    free(p_expression);
    p_expression = NULL;

    return 0;
}

static int generate_human_anime_styple_callback(ucloud_ai_result_t *result)
{
    int ret;
    char *url = NULL;

    if (!result)
        return -1;

    url = result->facebody.anime.url;
    LOG("Generate human anime style result:\n");
    LOG("url: %s\n", url);

    return ret;
}
#endif

#ifdef CONFIG_ALICLOUD_OBJECTDET_ENABLE
static int detect_object_callback(ucloud_ai_result_t *result)
{
    int len = 0;
    char *p_type = NULL;
    int x, y, w, h;
    char *type = NULL;
    float score;

    if (!result)
        return -1;

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
    p_type = strdup(type);
    if (!p_type) {
        LOGE(TAG, "p_type strdup fail\n");
        return -1;
    }

    LOG("Detect object result:\n");
    LOG("type: %s, Score: %.1f, x: %d, y: %d, w: %d, h: %d\n", p_type, score, x, y, w, h);
    free(p_type);
    p_type = NULL;

    return 0;
}

static int detect_main_body_callback(ucloud_ai_result_t *result)
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
static int segment_common_image_callback(ucloud_ai_result_t *result)
{
    int ret;

    if (!result)
        return -1;

    LOG("Segment common image result:\n");
    LOG("url: %s\n", result->imageseg.common.url);

    return 0;
}

static int segment_face_callback(ucloud_ai_result_t *result)
{
    int ret;
    int image_len = 0;
    int x, y, w, h;
    int index;
    char *url = NULL;
    char path[32] = {0};

    if (!result)
        return -1;

    index = result->imageseg.face.index;
    url = result->imageseg.face.url;
    x = result->imageseg.face.location.x;
    y = result->imageseg.face.location.y;
    w = result->imageseg.face.location.w;
    h = result->imageseg.face.location.h;

    LOG("Segment face result:\n");
    LOG("face %d image url: %s locating at  x: %d, y: %d, w: %d, h: %d\n", index, url, x, y, w, h);

    return 0;
}
#endif

#ifdef CONFIG_ALICLOUD_OCR_ENABLE
static int recognize_identity_card_face_side_callback(ucloud_ai_result_t *result)
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
    if (address && strlen(address) > 0) {
        LOG("address: %s\n", address);
    }

    /*birthdate string*/
    birthdate = result->ocr.identity.face.birthDate;
    if (birthdate && strlen(birthdate) > 0) {
        LOG("birthdate: %s\n", birthdate);
    }

    /*gender string*/
    gender = result->ocr.identity.face.gender;
    if (gender && strlen(gender) > 0) {
        LOG("gender: %s\n", gender);
    }

    /*nationality string*/
    nationality = result->ocr.identity.face.nationality;
    if (nationality && strlen(nationality) > 0) {
        LOG("nationality: %s\n", nationality);
    }

    /*id number string*/
    id_num = result->ocr.identity.face.iDNumber;
    if (id_num && strlen(id_num) > 0) {
        LOG("id number: %s\n", id_num);
    }

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

static int recognize_identity_card_back_side_callback(ucloud_ai_result_t *result)
{
    char *start_date = NULL;
    char *issue = NULL;
    char *end_date = NULL;

    if (!result)
        return -1;

    LOG("Recognize identity card back side result:\n");
    /*start date of identity card's back side*/
    start_date = result->ocr.identity.back.startDate;
    if (start_date && strlen(start_date) > 0) {
        LOG("start date: %s\n", start_date);
    }

    /*issue of identity card's back side*/
    issue = result->ocr.identity.back.issue;
    if (issue && strlen(issue) > 0) {
        LOG("issue: %s\n", issue);
    }

    /*end date of identity card's back side*/
    end_date = result->ocr.identity.back.endDate;
    if (end_date && strlen(end_date) > 0) {
        LOG("end date: %s\n", end_date);
    }

    return 0;
}

static int recognize_bank_card_callback(ucloud_ai_result_t *result)
{
    char *bank_name = NULL;
    char *card_number = NULL;
    char *valid_date = NULL;

    if (!result)
        return -1;

    LOG("Recognize bank card result:\n");
    /*bank name of bank card*/
    bank_name = result->ocr.bank.bankName;
    if (bank_name && strlen(bank_name) > 0) {
        LOG("bank name: %s\n", bank_name);
    }

    /*card number of bank card*/
    card_number = result->ocr.bank.cardNumber;
    if (card_number && strlen(card_number) > 0) {
        LOG("card number: %s\n", card_number);
    }

    /*valid date of bank card*/
    valid_date = result->ocr.bank.validDate;
    if (valid_date && strlen(valid_date) > 0) {
        LOG("valid date: %s\n", valid_date);
    }

    return 0;
}

static int recognize_character_callback(ucloud_ai_result_t *result)
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
static int imagerecog_classifying_rubbish_callback(ucloud_ai_result_t *result)
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
}

static int imagerecog_detect_fruits_callback(ucloud_ai_result_t *result)
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
}
#endif

#ifdef CONFIG_ALICLOUD_IMAGEENHAN_ENABLE
static int imageenhan_erase_person_callback(ucloud_ai_result_t *result)
{
    int ret;
    char *url = NULL;

    if (!result)
        return -1;

    LOG("Recognize fruits result:\n");
    url = result->imageenhan.person.url;
    LOG("url: %s\n", url);

    return 0;
}

static int imageenhan_extend_image_style_callback(ucloud_ai_result_t *result)
{
    int ret;
    int major_image_len = 0;
    int out_image_len = 0;
    char *major_url = NULL;
    char *out_image_url = NULL;

    if (!result)
        return -1;

    LOG("Extend image style result:\n");
    major_url = result->imageenhan.style.majorUrl;
    out_image_url = result->imageenhan.style.outImageUrl;
    LOG("major url: %s\n", major_url);
    LOG("out image url: %s\n", out_image_url);

    return 0;
}

#endif // UCLOUD_AI_IMAGERECOG_CONFIG

static void ucloud_ai_comp_example(int argc, char **argv)
{
    int model_index = -1;
    int ret = -1;
    char *image = NULL;

    if (argc < 3) {
        LOG("Please test with command: ucloud_ai_example -m [0~14]\n");
        return;
    }

    if (strncmp(argv[1], "-m", 2) == 0) {
        if (argv[2] < 0 || argv[2] > 14) {
            LOG("range of model value is 0 ~ 14, please try again\n");
            return;
        }
    } else {
        LOG("unkown command\n");
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
            ucloud_ai_facebody_comparing_face("/data/image/face1.png", "/data/image/face2.png", facebody_compare_callback);
            break;
        case UCLOUD_AI_MODEL_GENERATE_HUMAN_ANIME_STYLE:
            printf("Generate human anime style:\n");
            ucloud_ai_facebody_generate_human_anime_style("/data/image/anime.png", generate_human_anime_styple_callback);
            break;
        case UCLOUD_AI_MODEL_RECOGNIZE_EXPRESSION:
            printf("Recognize expression:\n");
            ucloud_ai_facebody_recognize_expression("/data/image/expression.jpg", recognize_expression_callback);
            break;
#endif
#ifdef CONFIG_ALICLOUD_OBJECTDET_ENABLE
        case UCLOUD_AI_MODEL_DETECT_OBJECT:
            printf("Detect object:\n");
            ucloud_ai_objectdet_detect_object("/data/image/object.jpg", detect_object_callback);
            break;
        case UCLOUD_AI_MODEL_DETECT_MAIN_BODY:
            printf("Detect main body:\n");
            ucloud_ai_objectdet_detect_main_body("/data/image/mainbody.jpg", detect_main_body_callback);
            break;
#endif
#ifdef CONFIG_ALICLOUD_IMAGESEG_ENABLE
        case UCLOUD_AI_MODEL_SEGMENT_COMMON_IMAGE:
            printf("Segment common image:\n");
            ucloud_ai_imageseg_segment_common_image("/data/image/face1.png", segment_common_image_callback);
            break;
        case UCLOUD_AI_MODEL_SEGMENT_FACE:
            printf("Segment face:\n");
            ucloud_ai_imageseg_segment_face("/data/image/mainbody.jpg", segment_face_callback);
            break;
#endif
#ifdef CONFIG_ALICLOUD_OCR_ENABLE
        case UCLOUD_AI_MODEL_RECOGNIZE_IDENTITY_CARD_FACE_SIDE:
            printf("Recognize identity card face side:\n");
            ucloud_ai_ocr_recognize_identity_card_face_side("/data/image/card_face.jpg", recognize_identity_card_face_side_callback);
            break;
        case UCLOUD_AI_MODEL_RECOGNIZE_IDENTITY_CARD_BACK_SIDE:
            printf("Recognize identity card back side:\n");
            ucloud_ai_ocr_recognize_identity_card_back_side("/data/image/card_back.jpg", recognize_identity_card_back_side_callback);
            break;
        case UCLOUD_AI_MODEL_RECOGNIZE_BANK_CARD:
            printf("Recognize bank card:\n");
            ucloud_ai_ocr_recognize_bank_card("/data/image/bank_card.jpg", recognize_bank_card_callback);
            break;
        case UCLOUD_AI_MODEL_RECOGNIZE_CHARACTER:
            printf("Recognize character:\n");
            ucloud_ai_ocr_recognize_character("/data/image/character.jpg", recognize_character_callback);
            break;
#endif
#ifdef CONFIG_ALICLOUD_IMAGERECOG_ENABLE
        case UCLOUD_AI_MODEL_CLASSIFYING_RUBBISH:
            printf("Classifying rubbish:\n");
            ucloud_ai_imagerecog_classifying_rubbish("/data/image/rubbish.jpg", imagerecog_classifying_rubbish_callback);
            break;
        case UCLOUD_AI_MODEL_DETECT_FRUITS:
            printf("Detect fruits:\n");
            ucloud_ai_imagerecog_detect_fruits("/data/image/fruits.jpg", imagerecog_detect_fruits_callback);
            break;
#endif
#ifdef CONFIG_ALICLOUD_IMAGEENHAN_ENABLE
        case UCLOUD_AI_MODEL_ERASE_PERSON:
            printf("Erase person:\n");
            ucloud_ai_imageenhan_erase_person("/data/image/person_org.jpg", IMAGEENHAN_ERASE_PERSON_USERMASK_URL, imageenhan_erase_person_callback);
            break;
        case UCLOUD_AI_MODEL_EXTEND_IMAGE_STYLE:
            printf("Extend image style:\n");
            ucloud_ai_imageenhan_extend_image_style("/data/image/extend_style.jpg", IMAGEENHAN_EXTEND_IMAGE_STYLE_URL, imageenhan_extend_image_style_callback);
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
