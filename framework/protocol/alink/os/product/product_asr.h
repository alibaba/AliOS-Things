/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __PRODUCT_ASR_H__
#define __PRODUCT_ASR_H__

#include "product.h"

typedef enum {
    AUDIO_FORMAT_PCM = 0,
    AUDIO_FORMAT_OPUS = 1,
    AUDIO_FORMAT_OGG = 2,
    AUDIO_FORMAT_DEFAULT = 0xff
} audio_format_t;

#define PRODUCT_ASR_APP_KEY_LEN (64 + 1)

/**
 * @brief Get alink asr appkey string.
 *
 * @param[out] app_key @n buffer to store app key string.
 * @return A pointer to the start address of app_key.
 * @see None.
 * @note None.
 */
char *product_asr_get_appkey(char app_key[PRODUCT_ASR_APP_KEY_LEN]);

/**
 * @brief Get alink asr secret string.
 *
 * @param[out] asr_secret  @n buffer to store asr secret string.
 * @return A pointer to the start address of asr secret.
 * @see None.
 * @note None.
 */
char *product_asr_get_secret(char secret_str[PRODUCT_SECRET_LEN]);

/**
 * @brief Get audio format.
 *
 * @param none.
 * @return @audio_format_t
 * @see None.
 * @note None.
 */
audio_format_t product_asr_get_audio_format(void);

/**
 * @brief get alink asr CA (certification authority) addr , if the
 * board supports file system, you should return the file name,
 * otherwise the content of file should be returned.
 * @param none.
 * @return see brief..
 * @see None.
 * @note None.
 */
char *product_asr_get_ca(void);

#endif

