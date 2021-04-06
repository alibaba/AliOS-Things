/*
 * Copyright (C) 2020-2023 Alibaba Group Holding Limited
 */

#ifndef _UCLOUD_AI_OCR_H_
#define _UCLOUD_AI_OCR_H_
#include "ucloud_ai_common.h"

#ifdef __cplusplus
extern "C" {
#endif


/** @addtogroup ucloud_ai_aos_api
 * @{
 */

/**
 * Ucloud ai recognizing identity card face side api.
 *
 * @param[in]  path                identity card face side you want to recognize.
 * @param[in]  cb                  callback function to deal with result.
 *
 */
void ucloud_ai_ocr_recognize_identity_card_face_side(char *path, ucloud_ai_cb_t cb);

/**
 * Ucloud ai recognizing identity card back side api.
 *
 * @param[in]  path                identity card back side you want to recognize.
 * @param[in]  cb                  callback function to deal with result.
 *
 */
void ucloud_ai_ocr_recognize_identity_card_back_side(char *path, ucloud_ai_cb_t cb);

/**
 * Ucloud ai recognizing bank card api.
 *
 * @param[in]  path                bank card you want to recognize.
 * @param[in]  cb                  callback function to deal with result.
 *
 */
void ucloud_ai_ocr_recognize_bank_card(char *path, ucloud_ai_cb_t cb);

/**
 * Ucloud ai recognizing character api.
 *
 * @param[in]  path                character you want to recogize.
 * @param[in]  cb                  callback function to deal with result.
 *
 */
void ucloud_ai_ocr_recognize_character(char *path, ucloud_ai_cb_t cb);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif // _UCLOUD_AI_OCR_H_
