/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "km.h"
#include "irot_hal.h"
#include "chip_log.h"
#include "chip_config.h"

#define ID2_KEY_NAME                "id2_key"
#define ID2_KEY_NAME_LEN             7

static void _dump_chip_conf_info()
{
#if defined(CONFIG_CHIP_DEBUG)
    chip_log_info("CONFIG_CHIP_DEBUG is defined!\n");
#else
    chip_log_info("CONFIG_CHIP_DEBUG is not defined!\n");
#endif

    if (CONFIG_CHIP_TYPE == CHIP_TYPE_SE_STD_CMD) {
        chip_log_info("CONFIG_CHIP_TYPE: %s\n", "CHIP_TYPE_SE_STD_CMD");
    } else if (CONFIG_CHIP_TYPE == CHIP_TYPE_SE_MTK_CMD){
        chip_log_info("CONFIG_CHIP_TYPE: %s\n", "CHIP_TYPE_SE_MTK_CMD");
    }

    if (CONFIG_CHIP_KEY_TYPE == CHIP_KEY_TYPE_3DES) {
        chip_log_info("CONFIG_CHIP_KEY_TYPE: %s\n", "CHIP_KEY_TYPE_3DES");
    } else if (CONFIG_CHIP_KEY_TYPE == CHIP_KEY_TYPE_AES){
        chip_log_info("CONFIG_CHIP_KEY_TYPE: %s\n", "CHIP_KEY_TYPE_AES");
    } else if (CONFIG_CHIP_KEY_TYPE == CHIP_KEY_TYPE_RSA) {
        chip_log_info("CONFIG_CHIP_KEY_TYPE: %s\n", "CHIP_KEY_TYPE_RSA");
    }

    chip_log_info("========================================\n");
}

static uint32_t _check_km_key_name(const char* name, const uint32_t name_len)
{
    if (name == NULL || name_len != ID2_KEY_NAME_LEN) {
        return KM_ERR_BAD_PARAMS;
    }

    if (memcmp(ID2_KEY_NAME, name, ID2_KEY_NAME_LEN) != 0) {
        return KM_ERR_BAD_PARAMS;
    }

    return KM_SUCCESS;
}

uint32_t km_init()
{
    irot_result_t result;

    _dump_chip_conf_info();

    result = irot_hal_init();
    if (result != IROT_SUCCESS) {
        return KM_ERR_GENERIC;
    }

    return KM_SUCCESS;
}

void km_cleanup()
{
    irot_hal_cleanup();
}

uint32_t km_get_id2(uint8_t* id2, uint32_t* len)
{
    irot_result_t result;

    result = irot_hal_get_id2(id2, len);
    if(result == IROT_ERROR_ITEM_NOT_FOUND) {
        return KM_ERR_ITEM_NOT_FOUND;
    } else if (result != IROT_SUCCESS) {
        return KM_ERR_GENERIC;
    }

    return KM_SUCCESS;
}

#if (CONFIG_CHIP_KEY_TYPE == CHIP_KEY_TYPE_3DES || \
     CONFIG_CHIP_KEY_TYPE == CHIP_KEY_TYPE_AES  || \
     CONFIG_CHIP_KEY_TYPE == CHIP_KEY_TYPE_SM1  || \
     CONFIG_CHIP_KEY_TYPE == CHIP_KEY_TYPE_SM4)
uint32_t km_cipher(const char* name, const uint32_t name_len,
                   km_sym_param* cipher_params,
                   const uint8_t* iv, const uint32_t iv_len,
                   uint8_t* src, size_t src_len, uint8_t* dest, size_t* dest_len)
{
    km_block_mode_type block_mode = cipher_params->cipher_param.block_mode;
    km_padding_type padding_type = cipher_params->cipher_param.padding_type;
    km_purpose_type purpose_type = cipher_params->cipher_param.purpose_type;
    sym_crypto_param_t sym_param;
    irot_result_t result;
    uint32_t out_len;
    uint32_t km_ret;

    km_ret = _check_km_key_name(name, name_len);
    if (km_ret != KM_SUCCESS) {
        chip_log_error("name is not match, %s\n", name);
        return km_ret;
    }

    if (block_mode != KM_ECB || padding_type != KM_NO_PADDING) {
        chip_log_error("bad block_mode(%d) or padding_type(%d)\n", block_mode, padding_type);
        return KM_ERR_BAD_PARAMS;
    }

    sym_param.block_mode = BLOCK_MODE_ECB;
    sym_param.padding_type = SYM_PADDING_NOPADDING;

    if (purpose_type == KM_PURPOSE_ENCRYPT) {
        sym_param.mode = MODE_ENCRYPT;
    } else if (purpose_type == KM_PURPOSE_DECRYPT) {
        sym_param.mode = MODE_DECRYPT;
    } else {
        chip_log_error("bad purpose type, %d\n", purpose_type);
        return KM_ERR_BAD_PARAMS;
    }

    out_len = *dest_len;
    result = irot_hal_sym_crypto(NULL, KEY_ID_ID2, NULL, 0,
                  src, src_len, dest, &out_len, &sym_param);
    if (result != IROT_SUCCESS) {
        return KM_ERR_GENERIC;
    }

    *dest_len = out_len;

    return KM_SUCCESS;
}

#elif (CONFIG_CHIP_KEY_TYPE == CHIP_KEY_TYPE_RSA)

uint32_t km_sign(const char* name, const uint32_t name_len, void* sign_params,
                 const uint8_t* data, const size_t data_len, uint8_t* out, size_t* out_len)
{
    km_sign_param* km_params = (km_sign_param*)sign_params;
    asym_sign_verify_t type;
    irot_result_t result;
    uint32_t output_len;
    uint32_t km_ret;

    km_ret = _check_km_key_name(name, name_len);
    if (km_ret != KM_SUCCESS) {
        chip_log_error("name is not match, %s\n", name);
        return km_ret;
    }

    if (km_params->padding_type != KM_PKCS1) {
        return KM_ERR_BAD_PARAMS;
    }

    if (km_params->digest_type == KM_SHA1) {
        type = ASYM_TYPE_RSA_SHA1_PKCS1;
    } else if (km_params->digest_type == KM_SHA256) {
        type = ASYM_TYPE_RSA_SHA256_PKCS1;
    } else {
        return KM_ERR_BAD_PARAMS;
    }

    output_len = *out_len;
    result = irot_hal_asym_priv_sign(NULL, 0,
                  data, data_len, out, &output_len, type);
    if (result != IROT_SUCCESS) {
        return KM_ERR_GENERIC;
    }

    *out_len = output_len;

    return KM_SUCCESS;
}

uint32_t km_asym_decrypt(const char* name, const uint32_t name_len, void* enc_params,
                 const uint8_t* src, const size_t src_len, uint8_t* dest, size_t* dest_len)
{
    km_enc_param* km_params = (km_enc_param *)enc_params;
    irot_result_t result;
    uint32_t out_len;
    uint32_t km_ret;

    km_ret = _check_km_key_name(name, name_len);
    if (km_ret != KM_SUCCESS) {
        chip_log_error("name is not match, %s\n", name);
        return km_ret;
    }

    if (km_params->padding_type != KM_PKCS1) {
        chip_log_error("bad padding type, %d\n", km_params->padding_type);
        return KM_ERR_BAD_PARAMS;
    }

    out_len = *dest_len;
    result = irot_hal_asym_priv_decrypt(NULL, 0,
                  src, src_len, dest, &out_len, ASYM_PADDING_PKCS1);
    if (result != IROT_SUCCESS) {
        return KM_ERR_GENERIC;
    }

    *dest_len = out_len;

    return KM_SUCCESS;
}

#endif  /* CONFIG_CHIP_KEY_TYPE */

uint32_t km_set_id2(uint8_t* id2, uint32_t len)
{
    return KM_ERR_NOT_SUPPORTED;
}

uint32_t km_generate_key(const char *name, const uint32_t name_len,
                         km_key_type key_type, void *arg)
{
    return KM_ERR_NOT_SUPPORTED;
}

uint32_t km_import_key(const char* name, const uint32_t name_len,
             km_format_t format, const km_key_data_t* key_data, const uint32_t key_data_len)
{
    return KM_ERR_NOT_SUPPORTED;
}

uint32_t km_get_attestation(uint8_t* id, uint32_t* id_len)
{
    return KM_ERR_NOT_SUPPORTED;
}

uint32_t km_delete_key(const char *name, const uint32_t name_len)
{
    return KM_ERR_NOT_SUPPORTED;
}

uint32_t km_envelope_begin(void **ctx, const char *name, const uint32_t name_len,
        uint8_t *iv, uint16_t iv_len,
        uint8_t *protected_key, uint32_t *protected_key_len, km_purpose_type is_enc)
{
    return KM_ERR_NOT_SUPPORTED;
}

uint32_t km_envelope_update(void *ctx, uint8_t *src, uint32_t src_len,
        uint8_t *dest, uint32_t *dest_len)
{
    return KM_ERR_NOT_SUPPORTED;
}

uint32_t km_envelope_finish(void *ctx, uint8_t *src, uint32_t src_len,
        uint8_t *dest, uint32_t *dest_len)
{
    return KM_ERR_NOT_SUPPORTED;
}

uint32_t km_set_id2_state(uint32_t state)
{
    return KM_ERR_NOT_SUPPORTED;
}

uint32_t km_get_id2_state(uint32_t *state)
{
    return KM_ERR_NOT_SUPPORTED;
}

