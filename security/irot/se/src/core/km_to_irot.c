/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <string.h>
#include "chip_config.h"
#include "km.h"
#include "irot_hal.h"
#include "log/chiplog.h"

#define ID2_LEN 24
#define ID2_KEY_NAME "id2_key"
#define ID2_KEY_NAME_LEN (sizeof(ID2_KEY_NAME) - 1)

static void dump_chip_conf_info()
{
    switch (CHIP_TYPE_CONFIG) {
        case CHIP_TYPE_SECURE_MCU: {
            chip_log_debug("CHIP_TYPE_CONFIG           : %s\n",
                           "CHIP_TYPE_SECURE_MCU");
        } break;
        case CHIP_TYPE_SE_STD_CMD: {
            chip_log_debug("CHIP_TYPE_CONFIG           : %s\n",
                           "CHIP_TYPE_SE_STD_CMD");
        } break;
        case CHIP_TYPE_SE_MTK_CMD: {
            chip_log_debug("CHIP_TYPE_CONFIG           : %s\n",
                           "CHIP_TYPE_SE_MTK_CMD");
        } break;
        default: { } break; }
    switch (CHIP_CRYPTO_TYPE_CONFIG) {
        case CHIP_CRYPTO_TYPE_3DES: {
            chip_log_debug("CHIP_CRYPTO_TYPE_CONFIG    : %s\n",
                           "CHIP_CRYPTO_TYPE_3DES");
        } break;
        case CHIP_CRYPTO_TYPE_AES: {
            chip_log_debug("CHIP_CRYPTO_TYPE_CONFIG    : %s\n",
                           "CHIP_CRYPTO_TYPE_AES");
        } break;
        case CHIP_CRYPTO_TYPE_RSA: {
            chip_log_debug("CHIP_CRYPTO_TYPE_CONFIG    : %s\n",
                           "CHIP_CRYPTO_TYPE_RSA");
        } break;
        default: { } break; }
    chip_log_debug("========================================\n");
}

uint32_t km_init()
{
    irot_result_t ret;
    dump_chip_conf_info();
    ret = irot_hal_init();
    if (ret == IROT_SUCCESS) {
        return KM_SUCCESS;
    } else {
        return KM_ERR_GENERIC;
    }
}

void km_cleanup()
{
    irot_hal_cleanup();
}

uint32_t km_get_id2(uint8_t *id2, uint32_t *len)
{
    irot_result_t ret;
    ret = irot_hal_get_id2(id2, len);
    if (ret == IROT_ERROR_ITEM_NOT_FOUND) {
        return KM_ERR_ITEM_NOT_FOUND;
    } else if (ret != IROT_SUCCESS) {
        return KM_ERR_GENERIC;
    } else {
        return KM_SUCCESS;
    }
}


static uint32_t check_km_key_name(const char *name, const uint32_t name_len)
{
    if ((name == NULL) || (name_len != ID2_KEY_NAME_LEN)) {
        return KM_ERR_BAD_PARAMS;
    }
    if (memcmp(ID2_KEY_NAME, name, ID2_KEY_NAME_LEN) != 0) {
        return KM_ERR_BAD_PARAMS;
    }
    return KM_SUCCESS;
}

#if (CHIP_CRYPTO_TYPE_CONFIG == CHIP_CRYPTO_TYPE_3DES || \
     CHIP_CRYPTO_TYPE_CONFIG == CHIP_CRYPTO_TYPE_AES)
uint32_t km_cipher(const char *name, const uint32_t name_len,
                   km_sym_param *cipher_params, const uint8_t *iv,
                   const uint32_t iv_len, uint8_t *src, size_t src_len,
                   uint8_t *dest, size_t *dest_len)
{
    km_block_mode_type block_mode   = cipher_params->cipher_param.block_mode;
    km_padding_type    padding_type = cipher_params->cipher_param.padding_type;
    km_purpose_type    purpose_type = cipher_params->cipher_param.purpose_type;
    sym_crypto_param_t sym_crypto_param;
    uint32_t           out_len;
    irot_result_t      ret;
    uint32_t           kmret;

    kmret = check_km_key_name(name, name_len);
    if (kmret != KM_SUCCESS) {
        return kmret;
    }

    if ((block_mode != KM_ECB) || (padding_type != KM_NO_PADDING)) {
        return KM_ERR_BAD_PARAMS;
    }

    sym_crypto_param.block_mode   = BLOCK_MODE_ECB;
    sym_crypto_param.padding_type = SYM_PADDING_NOPADDING;

    if (purpose_type == KM_PURPOSE_ENCRYPT) {
        sym_crypto_param.mode = MODE_ENCRYPT;
    } else if (purpose_type == KM_PURPOSE_DECRYPT) {
        sym_crypto_param.mode = MODE_DECRYPT;
    } else {
        return KM_ERR_BAD_PARAMS;
    }

    out_len = *dest_len;
    ret     = irot_hal_sym_crypto(NULL, KEY_ID_ID2, NULL, 0, src, src_len, dest,
                              &out_len, &sym_crypto_param);
    if (ret != IROT_SUCCESS) {
        return KM_ERR_GENERIC;
    }
    *dest_len = out_len;
    return KM_SUCCESS;
}
#endif

#if (CHIP_CRYPTO_TYPE_CONFIG == CHIP_CRYPTO_TYPE_RSA)
uint32_t km_sign(const char *name, const uint32_t name_len, void *sign_params,
                 const uint8_t *data, const size_t data_len, uint8_t *out,
                 size_t *out_len)
{
    km_sign_param *    kmsign_params = (km_sign_param *)sign_params;
    uint32_t           output_len;
    asym_sign_verify_t asym_sign_verify_type;
    irot_result_t      ret;
    uint32_t           kmret;

    kmret = check_km_key_name(name, name_len);
    if (kmret != KM_SUCCESS) {
        return kmret;
    }

    if (kmsign_params->padding_type != KM_PKCS1) {
        return KM_ERR_BAD_PARAMS;
    }

    if (kmsign_params->digest_type == KM_SHA1) {
        asym_sign_verify_type = ASYM_TYPE_RSA_SHA1_PKCS1;
    } else if (kmsign_params->digest_type == KM_SHA256) {
        asym_sign_verify_type = ASYM_TYPE_RSA_SHA256_PKCS1;
    } else {
        return KM_ERR_BAD_PARAMS;
    }

    output_len = *out_len;
    ret = irot_hal_asym_priv_sign(NULL, 0, data, data_len, out, &output_len,
                                  asym_sign_verify_type);
    if (ret != IROT_SUCCESS) {
        return KM_ERR_GENERIC;
    }
    *out_len = output_len;
    return KM_SUCCESS;
}

uint32_t km_asym_decrypt(const char *name, const uint32_t name_len,
                         void *enc_params, const uint8_t *src,
                         const size_t src_len, uint8_t *dest, size_t *dest_len)
{
    km_enc_param *kmenc_params = enc_params;
    uint32_t      out_len;
    irot_result_t ret;
    uint32_t      kmret;

    kmret = check_km_key_name(name, name_len);
    if (kmret != KM_SUCCESS) {
        return kmret;
    }

    if (kmenc_params->padding_type != KM_PKCS1) {
        return KM_ERR_BAD_PARAMS;
    }

    out_len = *dest_len;
    ret     = irot_hal_asym_priv_decrypt(NULL, 0, src, src_len, dest, &out_len,
                                     ASYM_PADDING_PKCS1);
    if (ret != IROT_SUCCESS) {
        return KM_ERR_GENERIC;
    }
    *dest_len = out_len;
    return KM_SUCCESS;
}

#endif


uint32_t km_set_id2(uint8_t *id2, uint32_t len)
{
    return KM_ERR_NOT_SUPPORTED;
}

uint32_t km_import_key(const char *name, const uint32_t name_len,
                       km_format_t format, const km_key_data_t *key_data,
                       const uint32_t key_data_len)
{
    return KM_ERR_NOT_SUPPORTED;
}

uint32_t km_get_attestation(uint8_t *id, uint32_t *id_len)
{
    return KM_ERR_NOT_SUPPORTED;
}
