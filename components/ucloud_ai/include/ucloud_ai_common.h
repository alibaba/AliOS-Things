/*
 * Copyright (C) 2020-2023 Alibaba Group Holding Limited
 */

#ifndef _UCLOUD_AI_COMMON_H_
#define _UCLOUD_AI_COMMON_H_
#include "model/common.h"
#include "oss_app.h"

#ifdef __cplusplus
extern "C" {
#endif



#define FACE1_IMAGE "/data/ucloud_ai_image/face1.png"
#define FACE2_IMAGE "/data/ucloud_ai_image/face2.png"
#define ANIME_IMAGE "/data/ucloud_ai_image/expression.jpg"
#define EXPRESSION_IMAGE "/data/ucloud_ai_image/expression.jpg"
#define OBJECT_IMAGE "/data/ucloud_ai_image/object.jpg"
#define MAINBODY_IMAGE "/data/ucloud_ai_image/face1.png"
#define CARD_FACE_IMAGE "/data/ucloud_ai_image/card_face.jpg"
#define CARD_BACK_IMAGE "/data/ucloud_ai_image/card_back.jpg"
#define BANK_CARD_IMAGE "/data/ucloud_ai_image/bank_card.jpg"
#define CHARACTER_IMAGE "/data/ucloud_ai_image/character.jpg"
#define RUBBISH_IMAGE "/data/ucloud_ai_image/rubbish.jpg"
#define FRUITS_IMAGE "/data/ucloud_ai_image/fruits.jpg"
#define PERSON_ORG_IMAGE "/data/ucloud_ai_image/person_org.jpg"
#define STYLE_IMAGE "/data/ucloud_ai_image/person_org.jpg"

// imageenhan style url
#define IMAGEENHAN_ERASE_PERSON_USERMASK_URL "http://viapi-test.oss-cn-shanghai.aliyuncs.com/viapi-3.0domepic/imageenhan/ErasePerson/ErasePerson6.jpg"
#define IMAGEENHAN_EXTEND_IMAGE_STYLE_URL "http://ivpd-demo.oss-cn-shanghai.aliyuncs.com/ivpd-demo/images/ExtendImageStyle/styleUrl.jpeg"

typedef AIModelCBFunc ucloud_ai_cb_t;
typedef AIModelResultStruct ucloud_ai_result_t;

typedef enum _ucloud_ai_model_t {
    UCLOUD_AI_MODEL_COMPARING_FACEBODY, // refer to https://vision.aliyun.com/experience/detail?spm=a211p3.14471187.J_7524944390.59.bc2e797dbbIyZQ&tagName=facebody&children=CompareFace
    UCLOUD_AI_MODEL_GENERATE_HUMAN_ANIME_STYLE, // refer to https://vision.aliyun.com/experience/detail?spm=a211p3.14471187.J_7524944390.15.54ca797dDKkjDk&tagName=facebody&children=GenerateHumanAnimeStyle
    UCLOUD_AI_MODEL_RECOGNIZE_EXPRESSION, // refer to https://vision.aliyun.com/experience/detail?spm=a211p3.14471187.J_7524944390.10.4e1a797dHZHzS5&tagName=facebody&children=RecognizeExpression
    UCLOUD_AI_MODEL_DETECT_OBJECT, // refer to https://vision.aliyun.com/experience/detail?spm=a211p3.14471187.J_7524944390.20.2074797dOvJ4L2&tagName=objectdet&children=DetectObject
    UCLOUD_AI_MODEL_DETECT_MAIN_BODY, // refer to https://vision.aliyun.com/experience/detail?spm=a211p3.14471187.J_7524944390.23.61df797dKC0yXR&tagName=objectdet&children=DetectMainBody
    UCLOUD_AI_MODEL_SEGMENT_COMMON_IMAGE, // refer to https://vision.aliyun.com/experience/detail?spm=a211p3.14471187.J_7524944390.6.7d9e797dL51BIO&tagName=imageseg&children=SegmentCommonImage
    UCLOUD_AI_MODEL_SEGMENT_FACE, // refer to https://vision.aliyun.com/experience/detail?spm=a211p3.14471187.J_7524944390.22.9e3a797deY528B&tagName=imageseg&children=SegmentFace
    UCLOUD_AI_MODEL_RECOGNIZE_IDENTITY_CARD_FACE_SIDE, // refer to https://vision.aliyun.com/experience/detail?spm=a211p3.14471187.J_7524944390.16.7d87797d2vHJJ9&tagName=ocr&children=RecognizeIdentityCard
    UCLOUD_AI_MODEL_RECOGNIZE_IDENTITY_CARD_BACK_SIDE, // refer to https://vision.aliyun.com/experience/detail?spm=a211p3.14471187.J_7524944390.16.7d87797d2vHJJ9&tagName=ocr&children=RecognizeIdentityCard
    UCLOUD_AI_MODEL_RECOGNIZE_BANK_CARD, // refer to https://vision.aliyun.com/experience/detail?spm=a211p3.14471187.J_7524944390.20.63cd797d7APYEE&tagName=ocr&children=RecognizeBankCard
    UCLOUD_AI_MODEL_RECOGNIZE_CHARACTER, // refer to https://vision.aliyun.com/experience/detail?spm=a211p3.14020179.J_7524944390.6.66cd608dJkkY48&tagName=ocr&children=RecognizeCharacter
    UCLOUD_AI_MODEL_CLASSIFYING_RUBBISH, // refer to https://vision.aliyun.com/experience/detail?spm=a211p3.14471187.J_7524944390.13.73f5797dHFaJxR&tagName=imagerecog&children=ClassifyingRubbish
    UCLOUD_AI_MODEL_DETECT_FRUITS, // refer to https://vision.aliyun.com/experience/detail?spm=a211p3.14020179.J_7524944390.44.66cd608dt39mQW&tagName=imagerecog&children=DetectFruits
    UCLOUD_AI_MODEL_ERASE_PERSON, // refer to https://vision.aliyun.com/experience/detail?spm=a211p3.14471187.J_7524944390.6.6c49797dKzzl8D&tagName=imageenhan&children=ErasePerson, have some unknown issue
    UCLOUD_AI_MODEL_EXTEND_IMAGE_STYLE, // refer to https://vision.aliyun.com/experience/detail?spm=a211p3.14471187.J_7524944390.18.6c49797dKzzl8D&tagName=imageenhan&children=ExtendImageStyle
    UCLOUD_AI_MODEL_MAX
} ucloud_ai_model_t;

/** @defgroup ucloud_ai_aos_api ucloud_ai
 * @{
 */


/**
 * Ucloud ai set key and secret.
 * @param[in]  key    alibaba cloud key account.
 * @param[in]  secret    alibaba cloud secret.
 *
 */
void ucloud_ai_set_key_secret(char *key, char *secret);

/**
 * Set oss bucket name.
 *
 * @param[in]  bucket    oss bucket name.
 *
 */
void ucloud_ai_set_oss_bucket(char *bucket);

/**
 * Set oss endpoint.
 *
 * @param[in]  endpoint    oss endpoint.
 *
 */
void ucloud_ai_set_oss_endpoint(char *endpoint);

/**
 * Upload file to oss bucket.
 *
 * @param[in]  path    local file path.
 *
 * @return oss url result.
 */
uint8_t *ucloud_ai_upload_file(char *path);

/**
 * Initialize ucloud ai.
 *
 * @return  0 on success, negative error on failure.
 */
int32_t ucloud_ai_init(void);

/**
 * Uninitialize ucloud ai.
 *
 * @return  0 on success, negative error on failure.
 */
int32_t ucloud_ai_uninit(void);

/**
 * Connect to url server.
 *
 * @param[in]  url  url address.
 *
 * @return  0 on success, negative error on failure.
 */
int32_t ucloud_ai_connect(char *url);

/**
 * Disconnect to url server.
 *
 */
void ucloud_ai_disconnect(void);

/**
 * Get stream by url address.
 *
 */
int32_t ucloud_ai_get_stream(char *url, char **stream);
/**
 * @}
 */

#ifdef __cplusplus
}
#endif
#endif // _UCLOUD_AI_COMMON_H_
