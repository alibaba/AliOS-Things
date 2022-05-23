/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2018 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on all ESPRESSIF SYSTEMS products, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include "sdkconfig.h"
#include "audio_mem.h"
#ifdef CONFIG_BLUEDROID_ENABLED
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "esp_log.h"
#include "esp_system.h"

#include "mbedtls/aes.h"
#include "mbedtls/dhm.h"
#include "mbedtls/md5.h"

#if __has_include("esp_idf_version.h")
#include "esp_idf_version.h"
#else
#define ESP_IDF_VERSION_VAL(major, minor, patch) 1
#endif

#if (ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 0, 0))

#ifdef CONFIG_IDF_TARGET_ESP32
#include "esp32/rom/crc.h"
#else
#include "esp32s2beta/rom/crc.h"
#endif // CONFIG_IDF_TARGET_ESP32

#else
#include "rom/crc.h"
#endif //(ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 0, 0))

#define BLUFI_SECURITY_TAG "BLUFI_SECURITY"
/*
   The SEC_TYPE_xxx is for self-defined packet data type in the procedure of "BLUFI negotiate key"
   If using other negotiation procedure to exchange (or generate) a key, the user should redefine the type by yourself.
 */
#define SEC_TYPE_DH_PARAM_LEN   0x00
#define SEC_TYPE_DH_PARAM_DATA  0x01
#define SEC_TYPE_DH_P           0x02
#define SEC_TYPE_DH_G           0x03
#define SEC_TYPE_DH_PUBLIC      0x04

struct blufi_security {
#define DH_SELF_PUB_KEY_LEN     128
#define DH_SELF_PUB_KEY_BIT_LEN (DH_SELF_PUB_KEY_LEN * 8)
    uint8_t  self_public_key[DH_SELF_PUB_KEY_LEN];
#define SHARE_KEY_LEN           128
#define SHARE_KEY_BIT_LEN       (SHARE_KEY_LEN * 8)
    uint8_t  share_key[SHARE_KEY_LEN];
    size_t   share_len;
#define PSK_LEN                 16
    uint8_t  psk[PSK_LEN];
    uint8_t  *dh_param;
    int      dh_param_len;
    uint8_t  iv[16];
    mbedtls_dhm_context dhm;
    mbedtls_aes_context aes;
};
static struct blufi_security *blufi_sec;

static int myrand (void *rng_state, unsigned char *output, size_t len)
{
    size_t i;
    for (i = 0; i < len; ++i) {
        output[i] = esp_random();
    }

    return ESP_OK;
}

void blufi_dh_negotiate_data_handler(uint8_t *data, int len, uint8_t **output_data, int *output_len, bool *need_free)
{
    int ret;
    uint8_t type = data[0];

    if (blufi_sec == NULL) {
        ESP_LOGE(BLUFI_SECURITY_TAG, "BLUFI Security is not initialized");
        return;
    }

    switch (type) {
    case SEC_TYPE_DH_PARAM_LEN:
        blufi_sec->dh_param_len = ((data[1]<<8)|data[2]);
        if (blufi_sec->dh_param) {
            audio_free(blufi_sec->dh_param);
            blufi_sec->dh_param = NULL;
        }
        blufi_sec->dh_param = (uint8_t *)audio_calloc(1, blufi_sec->dh_param_len);
        if (blufi_sec->dh_param == NULL) {
            ESP_LOGE(BLUFI_SECURITY_TAG, "%s, Malloc failed", __func__);
            return;
        }
        break;
    case SEC_TYPE_DH_PARAM_DATA:{
        if (blufi_sec->dh_param == NULL) {
            ESP_LOGE(BLUFI_SECURITY_TAG, "%s, Blufi_sec->dh_param == NULL", __func__);
            return;
        }
        uint8_t *param = blufi_sec->dh_param;
        memcpy(blufi_sec->dh_param, &data[1], blufi_sec->dh_param_len);
        ret = mbedtls_dhm_read_params(&blufi_sec->dhm, &param, &param[blufi_sec->dh_param_len]);
        if (ret) {
            ESP_LOGE(BLUFI_SECURITY_TAG, "%s Read param failed %d", __func__, ret);
            return;
        }
        audio_free(blufi_sec->dh_param);
        blufi_sec->dh_param = NULL;
        ret = mbedtls_dhm_make_public(&blufi_sec->dhm, (int) mbedtls_mpi_size( &blufi_sec->dhm.P ), blufi_sec->self_public_key, blufi_sec->dhm.len, myrand, NULL);
        if (ret) {
            ESP_LOGE(BLUFI_SECURITY_TAG, "%s Make public failed %d", __func__, ret);
            return;
        }

        mbedtls_dhm_calc_secret( &blufi_sec->dhm,
                blufi_sec->share_key,
                SHARE_KEY_BIT_LEN,
                &blufi_sec->share_len,
                NULL, NULL);

        mbedtls_md5(blufi_sec->share_key, blufi_sec->share_len, blufi_sec->psk);

        mbedtls_aes_setkey_enc(&blufi_sec->aes, blufi_sec->psk, 128);

        /* Alloc output data */
        *output_data = &blufi_sec->self_public_key[0];
        *output_len = blufi_sec->dhm.len;
        *need_free = false;

    }
        break;
    case SEC_TYPE_DH_P:
        break;
    case SEC_TYPE_DH_G:
        break;
    case SEC_TYPE_DH_PUBLIC:
        break;
    }
}

int blufi_aes_encrypt(uint8_t iv8, uint8_t *crypt_data, int crypt_len)
{
    int ret;
    size_t iv_offset = 0;
    uint8_t iv0[16];

    memcpy(iv0, blufi_sec->iv, sizeof(blufi_sec->iv));
    /* Set iv8 as the iv0[0] */
    iv0[0] = iv8;

    ret = mbedtls_aes_crypt_cfb128(&blufi_sec->aes, MBEDTLS_AES_ENCRYPT, crypt_len, &iv_offset, iv0, crypt_data, crypt_data);
    if (ret) {
        return ESP_FAIL;
    }

    return crypt_len;
}

int blufi_aes_decrypt(uint8_t iv8, uint8_t *crypt_data, int crypt_len)
{
    int ret;
    size_t iv_offset = 0;
    uint8_t iv0[16];

    memcpy(iv0, blufi_sec->iv, sizeof(blufi_sec->iv));
    /* Set iv8 as the iv0[0] */
    iv0[0] = iv8;

    ret = mbedtls_aes_crypt_cfb128(&blufi_sec->aes, MBEDTLS_AES_DECRYPT, crypt_len, &iv_offset, iv0, crypt_data, crypt_data);
    if (ret) {
        return ESP_FAIL;
    }

    return crypt_len;
}

uint16_t blufi_crc_checksum(uint8_t iv8, uint8_t *data, int len)
{
    /* This iv8 ignore, not used */
    return crc16_be(0, data, len);
}

esp_err_t blufi_security_init(void)
{
    blufi_sec = (struct blufi_security *)audio_calloc(1, sizeof(struct blufi_security));
    if (blufi_sec == NULL) {
        return ESP_FAIL;
    }

    mbedtls_dhm_init(&blufi_sec->dhm);
    mbedtls_aes_init(&blufi_sec->aes);

    memset(blufi_sec->iv, 0x0, 16);
    return ESP_OK;
}

esp_err_t blufi_security_deinit(void)
{
    if (blufi_sec == NULL) {
        return ESP_FAIL;
    }
    if (blufi_sec->dh_param){
        audio_free(blufi_sec->dh_param);
        blufi_sec->dh_param = NULL;
    }
    mbedtls_dhm_free(&blufi_sec->dhm);
    mbedtls_aes_free(&blufi_sec->aes);

    memset(blufi_sec, 0x0, sizeof(struct blufi_security));

    audio_free(blufi_sec);
    blufi_sec = NULL;

    return ESP_OK;
}

#endif
