/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "km.h"
#include "ls_osa.h"
#include "ali_crypto.h"

/* ID2 For Debug, which can be got from id2 console */
#define ID2_ID      "000FFFFFDB1D8DC78DDCB800"
#define ID2_KEY     "e4cac8c54ab2d7f07939466b332736990598a452ffe33a97"

#define ID2_LEN                             24
#define MAX_KEY_LEN                         32

#define KM_DBG_LOG(_f, _a ...)  \
        ls_osa_print("%s %d: "_f,  __FUNCTION__, __LINE__, ##_a)

static uint8_t s_id2[ID2_LEN + 1]           = {0};
static uint8_t s_id2_key[MAX_KEY_LEN + 1]   = {0};

static uint32_t s_id2_len = 0;
static uint32_t s_key_len = 0;

static uint32_t s_km_init = 0;

static int _char_to_hex(char c)
{
    int hex = -1;

    if (c >= '0' && c <= '9') {
        hex = c - '0';
    } else if (c >= 'a' && c <= 'f') {
        hex = c - 'a' + 10;
    } else if (c >= 'A' && c <= 'F') {
        hex = c - 'A' + 10;
    }

    return hex;
}

static int _string_to_hex(char *str, uint32_t str_len, uint8_t *hex, uint32_t hex_len)
{
    size_t i;
    uint8_t h, l;

    if (hex_len * 2 < str_len) {
        return -1;
    }

    for (i = 0; i < str_len; i += 2) {
        h = _char_to_hex(str[i]);
        l = _char_to_hex(str[i + 1]);
        if (h < 0 || l < 0) {
            return -1;
        }

        hex[i >> 1] = (h << 4) | (l & 0x0F);
    }

    return 0;
}

static int _aes_ecb_crypt(uint8_t *key, uint32_t key_len,
                uint8_t *in, uint32_t in_len, uint8_t* out, uint32_t* out_len, uint8_t is_enc)
{
    int ret = 0;
    size_t ctx_size;
    ali_crypto_result result;
    void* aes_ctx = NULL;

    result = ali_aes_get_ctx_size(AES_ECB, &ctx_size);
    if (result != ALI_CRYPTO_SUCCESS) {
        KM_DBG_LOG("get aes ecb ctx fail, 0x%x\n", result);
        return -1;
    }

    aes_ctx = ls_osa_malloc(ctx_size);
    if (aes_ctx == NULL) {
        KM_DBG_LOG("out of mem, %d\n", ctx_size);
        return -1;
    }

    result = ali_aes_init(AES_ECB, is_enc, key, NULL, key_len, NULL, aes_ctx);
    if (result != ALI_CRYPTO_SUCCESS) {
        KM_DBG_LOG("aes ecb init fail, %d\n", result);
        ret = -1;
        goto _out;
    }

    result = ali_aes_process(in, out, in_len, aes_ctx);
    if (result != ALI_CRYPTO_SUCCESS){
        KM_DBG_LOG("aes ecb process fail, %d\n", result);
        ret = -1;
        goto _out;
    }

    result = ali_aes_finish(NULL, 0, NULL, NULL, SYM_NOPAD, aes_ctx);
    if (result != ALI_CRYPTO_SUCCESS) {
        KM_DBG_LOG("aes ecb finish fail, %d\n", result);
        ret = -1;
        goto _out;
    }

    *out_len = in_len;

_out:
    ls_osa_free(aes_ctx);

    return ret;
}

uint32_t km_generate_key(const char* name, const uint32_t name_len, km_key_type key_type, void* arg)
{
    return KM_ERR_NOT_SUPPORTED;
}

uint32_t km_export_key(const char* name, const uint32_t name_len,
                       km_format_t format, uint8_t* export_data, uint32_t* export_data_size)
{
    return KM_ERR_NOT_SUPPORTED;
}

uint32_t km_mac(const char *name, const uint32_t name_len, km_sym_param *mac_params,
        const uint8_t *iv, const uint32_t iv_len, uint8_t *src, size_t src_len,
        uint8_t *mac, uint32_t *mac_len)
{
    return KM_ERR_NOT_SUPPORTED;
}

uint32_t km_delete_key(const char* name, const uint32_t name_len)
{
    return KM_ERR_NOT_SUPPORTED;
}

uint32_t km_delete_all()
{
    return KM_ERR_NOT_SUPPORTED;
}

////////////////////////////////////////////////////////////////////////////////

uint32_t km_envelope_begin(void **ctx, const char *name, const uint32_t name_len,
                           uint8_t *iv, uint16_t iv_len,
                           uint8_t *protected_key, uint32_t *protected_key_len, km_purpose_type is_enc)
{
    return KM_ERR_NOT_SUPPORTED;
}

uint32_t km_envelope_update(void* ctx, uint8_t* src, uint32_t src_len, uint8_t* dest, uint32_t* dest_len)
{
    return KM_ERR_NOT_SUPPORTED;
}

uint32_t km_envelope_finish(void* ctx, uint8_t* src, uint32_t src_len, uint8_t* dest, uint32_t* dest_len)
{
    return KM_ERR_NOT_SUPPORTED;
}

////////////////////////////////////////////////////////////////////////////////

uint32_t km_sign(const char* name, const uint32_t name_len, void* sign_params,
                 const uint8_t* data, const size_t data_len, uint8_t* out, size_t* out_len)
{
    return KM_ERR_NOT_SUPPORTED;
}

uint32_t km_verify(const char* name, const uint32_t name_len, void* sign_params,
                   const uint8_t* data, const size_t data_len, const uint8_t* signature, const size_t signature_len)
{
    return KM_ERR_NOT_SUPPORTED;
}

uint32_t km_asym_encrypt(const char* name, const uint32_t name_len, void* enc_params,
                         const uint8_t* src, const size_t src_len, uint8_t* dest, size_t* dest_len)
{
    return KM_ERR_NOT_SUPPORTED;
}

uint32_t km_asym_decrypt(const char* name, const uint32_t name_len, void* enc_params,
                         const uint8_t* src, const size_t src_len, uint8_t* dest, size_t* dest_len)
{
    return KM_ERR_NOT_SUPPORTED;
}

////////////////////////////////////////////////////////////////////////////////

uint32_t km_init()
{
    int ret = 0;

    KM_DBG_LOG("Demo KM Build Time: %s %s\n", __DATE__, __TIME__);
    KM_DBG_LOG("SE, TEE or Soft KM must be selected for formal product!!!\n\n");

    if (s_km_init == 0) {
        s_id2_len = strlen(ID2_ID);
        if (s_id2_len != ID2_LEN) {
            return KM_ERR_GENERIC;
        }

        memcpy(s_id2, ID2_ID, s_id2_len);

        s_key_len = strlen(ID2_KEY) >> 1;
        ret = _string_to_hex(ID2_KEY, (uint32_t)strlen(ID2_KEY), s_id2_key, s_key_len);
        if (ret < 0) {
            KM_DBG_LOG("string to hex fail\n");
            return KM_ERR_GENERIC;
        }

        s_km_init = 1;
    }

    return KM_SUCCESS;
}

void km_cleanup()
{
    return;
}

uint32_t km_cipher(const char* name, const uint32_t name_len,
                   km_sym_param* km_params, const uint8_t* iv, const uint32_t iv_len,
                   uint8_t* src, size_t src_len, uint8_t* dest, size_t* dest_len)
{
    int ret = 0;
    km_key_type key_type = km_params->key_type;
    km_block_mode_type block_mode = km_params->cipher_param.block_mode;
    km_padding_type padding_type = km_params->cipher_param.padding_type;
    km_purpose_type purpose_type = km_params->cipher_param.purpose_type;
    uint8_t is_enc;
    uint32_t output_len;

    if ((block_mode != KM_ECB) || (padding_type != KM_NO_PADDING)) {
        return KM_ERR_NOT_SUPPORTED;
    }

    is_enc = (purpose_type == KM_PURPOSE_ENCRYPT) ? 1 : 0;

    output_len = *dest_len;
    if (key_type == KM_AES) {
        ret = _aes_ecb_crypt(s_id2_key, s_key_len, src, src_len, dest, &output_len, is_enc);
        if (ret != 0) {
            KM_DBG_LOG("aes ecb crypt fail\n");
            return KM_ERR_GENERIC;
        }
    } else {
        return KM_ERR_NOT_SUPPORTED;
    }

    *dest_len = output_len;

    return KM_SUCCESS;
}

uint32_t km_set_id2(uint8_t* id2, uint32_t len)
{
    if (len != ID2_LEN) {
        return KM_ERR_BAD_PARAMS;
    }

     memcpy(s_id2, id2, len);
     s_id2_len = len;

     return KM_SUCCESS;
}

uint32_t km_import_key(const char* name, const uint32_t name_len,
                       km_format_t format, const km_key_data_t* key_data, const uint32_t key_data_len)
{
    uint8_t key_type;
    uint8_t* pkey;
    uint32_t key_len;

    key_type = key_data->type;

    if ((key_type != KM_DES3) && (key_type != KM_AES)) {
        return KM_ERR_BAD_PARAMS;
    }
    pkey = key_data->sym_key.key;
    key_len = key_data->sym_key.key_bit >> 3;

    memcpy(s_id2_key, pkey, key_len);
    s_key_len = key_len;

    return KM_SUCCESS;
}

uint32_t km_get_id2(uint8_t* id2, uint32_t* len)
{
    if (*len < ID2_LEN) {
        return KM_ERR_SHORT_BUFFER;
    }

    if (s_id2_len == 0) {
        return KM_ERR_ITEM_NOT_FOUND;
    } else {
        memcpy(id2, s_id2, ID2_LEN);
    }

    *len = ID2_LEN;

    return KM_SUCCESS;
}

uint32_t km_get_attestation(uint8_t* id, uint32_t* id_len)
{
    if (id == NULL) {
        *id_len = ID2_LEN;
        return KM_ERR_SHORT_BUFFER;
    } else {
        *id_len = ID2_LEN;
        memset(id, 'A', *id_len);
	return KM_SUCCESS;
    }
}

uint32_t km_get_id2_state(uint32_t *state)
{
    return KM_ERR_ITEM_NOT_FOUND;
}

uint32_t km_set_id2_state(uint32_t state)
{
    return KM_ERR_ITEM_NOT_FOUND;
}

