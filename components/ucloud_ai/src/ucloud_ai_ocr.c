/*
 * Copyright (C) 2021-2023 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include "ucloud_ai_common.h"

#define TAG "UCLOUD_AI_OCR"

/*recognizing identity card has issue to be resolved*/
void ucloud_ai_ocr_recognize_identity_card_face_side(char *path, ucloud_ai_cb_t cb)
{
    int ret;
    int len;
    char *p_upload_url;

    /*update capture.jpg to oss*/
    p_upload_url = ucloud_ai_upload_file(path);
    if (!p_upload_url)
        return;

    recognizeIdentityCardFaceSide(p_upload_url, cb);
    free(p_upload_url);
}

void ucloud_ai_ocr_recognize_identity_card_back_side(char *path, ucloud_ai_cb_t cb)
{
    int ret;
    int len;
    char *p_upload_url;

    /*update capture.jpg to oss*/
    p_upload_url = ucloud_ai_upload_file(path);
    if (!p_upload_url)
        return;

    recognizeIdentityCardBackSide(p_upload_url, cb);
    free(p_upload_url);
}

void ucloud_ai_ocr_recognize_bank_card(char *path, ucloud_ai_cb_t cb)
{
    int ret;
    int len;
    char *p_upload_url;

    /*update capture.jpg to oss*/
    p_upload_url = ucloud_ai_upload_file(path);
    if (!p_upload_url)
        return;

    recognizeBankCard(p_upload_url, cb);
    free(p_upload_url);
}

void ucloud_ai_ocr_recognize_character(char *path, ucloud_ai_cb_t cb)
{
    int ret;
    int len;
    char *p_upload_url;

    /*update capture.jpg to oss*/
    p_upload_url = ucloud_ai_upload_file(path);
    if (!p_upload_url)
        return;

    recognizeCharacter(p_upload_url, cb);
    free(p_upload_url);
}
