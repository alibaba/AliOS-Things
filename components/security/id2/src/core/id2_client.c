/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "id2_config.h"
#include "id2_plat.h"
#include "id2_priv.h"
#include "id2_client.h"
#include "km.h"

#define ID2_CLIENT_VERSION_NUMBER   0x00020000

/* id2 auth code type */
#define TYPE_CLIENT_CHALLENGE               0
#define TYPE_CLIENT_TIMESTAMP               1
#define TYPE_CLIENT_CHALLENGE_WITH_EXTRA    2
#define TYPE_CLIENT_TIMESTAMP_WITH_EXTRA    3

#define TYPE_SERVER_CHALLENGE               10
#define TYPE_SERVER_CHALLENGE_WITH_EXTRA    11

#define MAX_SIGN_LEN                        128
#define MAX_HASH_LEN                        64
#define MAX_SYMM_BLOCK_LEN                  16

#if (CONFIG_ID2_KEY_TYPE == ID2_KEY_TYPE_3DES || \
     CONFIG_ID2_KEY_TYPE == ID2_KEY_TYPE_AES  || \
     CONFIG_ID2_KEY_TYPE == ID2_KEY_TYPE_SM1  || \
     CONFIG_ID2_KEY_TYPE == ID2_KEY_TYPE_SM4)
#define ID2_AUTH_CODE_BUF_LEN               256
#else
#define ID2_AUTH_CODE_BUF_LEN               512
#endif

static uint8_t s_id2_id[ID2_ID_LEN + 1] = {0};
static uint8_t s_id2_id_len = 0;
static uint8_t s_id2_client_inited = 0;

static uint8_t s_device_random[ID2_MAX_DEVICE_RANDOM_LEN];
static uint8_t s_device_random_len = 0;

static void _dump_id2_conf_info(void)
{
    ls_osa_print("ID2 Client Version: 0x%08x\n", ID2_CLIENT_VERSION_NUMBER);
    ls_osa_print("ID2 Client Build Time: %s %s\n", __DATE__, __TIME__);

    ls_osa_print("-------------------------------------------\n");

#if defined(CONFIG_ID2_DEBUG)
    ls_osa_print("CONFIG_ID2_DEBUG is defined!\n");
#else
    ls_osa_print("CONFIG_ID2_DEBUG is not defined!\n");
#endif

#if defined(CONFIG_ID2_OTP)
    ls_osa_print("CONFIG_ID2_OTP is defined!\n");
#else
    ls_osa_print("CONFIG_ID2_OTP is not defined!\n");
#endif

    if (CONFIG_ID2_KEY_TYPE == ID2_KEY_TYPE_3DES) {
        ls_osa_print("CONFIG_ID2_KEY_TYPE: %s\n", "ID2_KEY_TYPE_3DES");
    } else if (CONFIG_ID2_KEY_TYPE == ID2_KEY_TYPE_AES){
        ls_osa_print("CONFIG_ID2_KEY_TYPE: %s\n", "ID2_KEY_TYPE_AES");
    } else if (CONFIG_ID2_KEY_TYPE == ID2_KEY_TYPE_RSA) {
        ls_osa_print("CONFIG_ID2_KEY_TYPE: %s\n", "ID2_KEY_TYPE_RSA");
    }

    ls_osa_print("-------------------------------------------\n");
}

static void _id2_inverse(uint8_t* id2)
{
    int i = 0;
    int hex = 0;

    for (i = 0; i < ID2_ID_LEN; ++i) {
        hex = char_to_hex(*id2);
        hex = 0x0F - hex;
        *id2++ = hex_to_char(hex);
    }
}

static irot_result_t _id2_hash(uint8_t* data, uint32_t data_len,
                               uint8_t* hash, uint32_t* hash_len)
{
    int ret;
    digest_t type;

#if CONFIG_ID2_HASH_TYPE == ID2_HASH_TYPE_SHA256
    type = DIGEST_TYPE_SHA256;
#elif CONFIG_ID2_HASH_TYPE == ID2_HASH_TYPE_SM3
    type = DIGEST_TYPE_SM3;
#endif

    ret = id2_plat_hash_sum(data, data_len, hash, hash_len, type);
    if (ret < 0) {
        return IROT_ERROR_GENERIC;
    }

    return IROT_SUCCESS;
}

#if (CONFIG_ID2_KEY_TYPE == ID2_KEY_TYPE_3DES || \
     CONFIG_ID2_KEY_TYPE == ID2_KEY_TYPE_AES  || \
     CONFIG_ID2_KEY_TYPE == ID2_KEY_TYPE_SM1  || \
     CONFIG_ID2_KEY_TYPE == ID2_KEY_TYPE_SM4)
static irot_result_t _id2_sym_cipher(uint8_t* in, uint32_t in_len,
                              uint8_t* out, uint32_t* out_len, int is_enc)
{
    uint32_t km_ret;
    uint32_t block_len;
    uint32_t i, padding;
    size_t output_len;
    km_sym_param km_param;

#if (CONFIG_ID2_KEY_TYPE == ID2_KEY_TYPE_3DES)
    block_len = DES_BLOCK_SIZE;
    km_param.key_type = KM_DES3;
#else
    block_len = AES_BLOCK_SIZE;
    km_param.key_type = KM_AES;
#endif

    id2_log_hex_dump("id2 cipher input:", in, in_len);

    if (in_len % block_len != 0) {
        id2_log_error("invalid input length, %d.\n", in_len);
        return IROT_ERROR_BAD_PARAMETERS;
    }

    km_param.cipher_param.block_mode = KM_ECB;
    km_param.cipher_param.padding_type = KM_NO_PADDING;
    km_param.cipher_param.purpose_type = is_enc ? KM_PURPOSE_ENCRYPT : KM_PURPOSE_DECRYPT;

    output_len = *out_len;
    km_ret = km_cipher(ID2_KEY_NAME, ID2_KEY_NAME_LEN,
                       &km_param, NULL, 0, (uint8_t*)in, in_len, out, &output_len);
    if (km_ret != KM_SUCCESS) {
        id2_log_error("km cipher error, 0x%x.\n",km_ret);
        return IROT_ERROR_GENERIC;
    }

    if (output_len < block_len || output_len % block_len != 0) {
        id2_log_error("invalid output length, %d.\n", output_len);
        return IROT_ERROR_GENERIC;
    }

    id2_log_hex_dump("id2 cipher output:", out, output_len);

    if (is_enc) {
        *out_len = output_len;
    } else {
        id2_log_debug("id2 cipher pkcs5 unpadding.\n");

        padding = out[output_len - 1];
        if (padding > block_len) {
            id2_log_error("invalid pkcs5 padding, %d\n", padding);
            return IROT_ERROR_GENERIC;
        }

        for (i = padding; i > 0; i--) {
            if (out[output_len - i] != padding) {
                id2_log_error("id2 pkcs5 unpadding fail.\n");
                return IROT_ERROR_GENERIC;
            }
        }

        *out_len = output_len - padding;
    }

    return IROT_SUCCESS;
}

static irot_result_t _id2_sign(uint8_t* sign_in, uint32_t sign_in_len,
                               uint8_t* sign_out, uint32_t* sign_out_len)
{
    irot_result_t ret = IROT_SUCCESS;
    uint8_t hash_buf[MAX_HASH_LEN + MAX_SYMM_BLOCK_LEN];
    uint32_t hash_len = sizeof(hash_buf);
    uint32_t input_len;
    uint32_t block_len;
    uint32_t i, padding;

    id2_log_hex_dump("id2 sign input:", sign_in, sign_in_len);

    ret = _id2_hash(sign_in, sign_in_len, hash_buf, &hash_len);
    if (ret != IROT_SUCCESS) {
        id2_log_error("id2 hash fail, %d.\n", ret);
        return ret;
    }

#if (CONFIG_ID2_KEY_TYPE == ID2_KEY_TYPE_3DES)
    block_len = DES_BLOCK_SIZE;
#else
    block_len = AES_BLOCK_SIZE;
#endif

    padding = block_len - hash_len % block_len;
    input_len = hash_len + padding;

    for (i = 0; i < padding; i++) {
        hash_buf[hash_len + i] = padding;
    }

    ret = _id2_sym_cipher(hash_buf, input_len, sign_out, sign_out_len, 1);
    if (ret != IROT_SUCCESS) {
        id2_log_error("id2 sym cipher fail, %d.\n", ret);
        return ret;
    }

    id2_log_hex_dump("id2 sign output:", sign_out, *sign_out_len);

    return IROT_SUCCESS;
}

#elif (CONFIG_ID2_KEY_TYPE == ID2_KEY_TYPE_RSA)

irot_result_t _id2_asym_decrypt(uint8_t* in, uint32_t in_len,
                                uint8_t* out, uint32_t* out_len)
{
    irot_result_t ret = IROT_SUCCESS;
    km_enc_param enc_params;
    uint32_t km_ret;
    uint32_t out_buf_len = *out_len;
    uint32_t out_data_len = 0;

    if (in_len % RSA_BLOCK_SIZE != 0) {
        id2_log_error("invalid input length, %d\n", in_len);
        return IROT_ERROR_BAD_PARAMETERS;
    }

    enc_params.padding_type = KM_PKCS1;
    while (in_len > 0) {
        *out_len = out_buf_len;

        /* decrypt one block each time */
        km_ret = km_asym_decrypt(ID2_KEY_NAME, ID2_KEY_NAME_LEN,
                                 &enc_params, in, in_len, out, out_len);
        if (km_ret != KM_SUCCESS) {
            id2_log_error("km asym decrypt error, 0x%08x.\n", km_ret);
            ret = IROT_ERROR_GENERIC;
            break;
        }

        in += RSA_BLOCK_SIZE;
        out += *out_len;
        out_data_len += *out_len;

        in_len -= RSA_BLOCK_SIZE;
        out_buf_len -= *out_len;
    }

    *out_len = out_data_len;

    return ret;
}

static irot_result_t _id2_sign(uint8_t* sign_in, uint32_t sign_in_len,
                               uint8_t* sign_out, uint32_t* sign_out_len)
{
    km_sign_param sign_params;
    uint32_t km_ret;

    sign_params.digest_type = KM_SHA256;
    sign_params.padding_type = KM_PKCS1;

    id2_log_hex_dump("id2 sign input:", sign_in, sign_in_len);

    km_ret = km_sign(ID2_KEY_NAME, ID2_KEY_NAME_LEN,
                     &sign_params, sign_in, sign_in_len, sign_out, sign_out_len);
    if (km_ret != KM_SUCCESS) {
        id2_log_error("km sign fail, 0x%08x\n", km_ret);
        return IROT_ERROR_GENERIC;
    }

    id2_log_hex_dump("id2 sign output:", sign_out, *sign_out_len);

    return IROT_SUCCESS;
}
#endif

#ifdef CONFIG_ID2_AUTH_CODE_HARDEN
static irot_result_t id2_auth_code_encode(uint8_t *auth_code, uint32_t in_len, uint32_t *out_len)
{
    irot_result_t result;
    uint32_t block_len;
    uint32_t auth_data_len;
    uint8_t *auth_data = NULL;
    int ret;

    if (CONFIG_ID2_KEY_TYPE == ID2_KEY_TYPE_AES) {
        block_len = AES_BLOCK_SIZE;
    } else if (CONFIG_ID2_KEY_TYPE == ID2_KEY_TYPE_3DES) {
        block_len = DES_BLOCK_SIZE;
    } else {
        id2_log_error("not support this type, %d\n", CONFIG_ID2_KEY_TYPE);
        return IROT_ERROR_NOT_SUPPORTED;
    }

    auth_data_len = in_len + (block_len - in_len % block_len);
    auth_data_len = 2 + ((auth_data_len + 2) / 3) * 4;

    if (*out_len <= auth_data_len) {
        id2_log_error("short buffer %d %d\n", *out_len, auth_data_len);
        return IROT_ERROR_SHORT_BUFFER;
    }

    ret = pkcs5_pading(auth_code, in_len, &in_len, block_len);
    if (ret != 0) {
        id2_log_error("pkcs5 padding data error\n");
        return IROT_ERROR_GENERIC;
    }

    auth_data = (uint8_t *)ls_osa_malloc(in_len);
    if (auth_data == NULL) {
        id2_log_error("out of mem, %d\n", in_len);
        return IROT_ERROR_OUT_OF_MEMORY;
    }

    result = _id2_sym_cipher(auth_code, in_len, auth_data, &auth_data_len, 1);
    if (result != IROT_SUCCESS) {
        id2_log_error("id2 sym cipher error.\n");
        return result;
    }

    memset(auth_code, 0, *out_len);
    auth_code[0] = '9';
    auth_code[1] = '9';
    auth_code[2] = '~';

    ret = id2_plat_base64_encode(auth_data, auth_data_len, auth_code + 3, out_len);
    if (ret < 0) {
        id2_log_error("sym cipher encrypt error.\n");
        result = IROT_ERROR_GENERIC;
        goto _out;
    }

    *out_len = *out_len + 3;

_out:
    if (auth_data != NULL) {
        ls_osa_free(auth_data);
    }

    return result;
}

static irot_result_t id2_auth_code_decode(uint8_t *auth_code, uint32_t *auth_code_len)
{
    irot_result_t result;
    uint32_t auth_data_len;
    uint8_t *auth_data = NULL;
    int ret;

    if (auth_code[0] != '9' || auth_code[1] != '9') {
        id2_log_error("invalid auth code type(%d%d)\n", auth_code[0], auth_code[1]);
        return IROT_ERROR_GENERIC;
    }

    auth_data = (uint8_t *)ls_osa_malloc(*auth_code_len);
    if (auth_data == NULL) {
        id2_log_error("out of mem, %d\n", *auth_code_len);
        return IROT_ERROR_OUT_OF_MEMORY;
    }

    ret = id2_plat_base64_decode(auth_code + 3, *auth_code_len - 3, auth_data, &auth_data_len);
    if (ret < 0) {
        id2_log_error("base64 decode error.\n");
        result = IROT_ERROR_GENERIC;
        goto _out;
    }

    memset(auth_code, 0, *auth_code_len);

    result = _id2_sym_cipher(auth_data, auth_data_len, auth_code, auth_code_len, 0);
    if (result != IROT_SUCCESS) {
        id2_log_error("id2 sym cipher error.\n");
        goto _out;
    }

_out:
    if (auth_data != NULL) {
        ls_osa_free(auth_data);
    }

    return result;
}
#endif

static irot_result_t _id2_gen_auth_code(uint8_t auth_code_type, const char* server_random,
                                        const uint8_t* extra, uint32_t extra_len,
                                        uint8_t* auth_code, uint32_t* auth_code_len)
{
    irot_result_t ret = IROT_SUCCESS;
    uint32_t id2_len = ID2_ID_LEN;
    uint32_t id2_auth_len = 0;
    uint32_t device_random[2];
    uint8_t id2_str[ID2_ID_LEN + 1] = {0};
    char device_random_str[ID2_MAX_DEVICE_RANDOM_LEN + 1] = {0};

    uint8_t* sign_in = NULL;
    uint8_t* sign_out = NULL;
    uint8_t* sign_base64 = NULL;
    uint32_t sign_in_len = 0;
    uint32_t sign_out_len = 0;
    uint32_t sign_base64_len = 0;

    if (auth_code == NULL || auth_code_len == NULL ||
        server_random == NULL) {
        id2_log_error("invalid input args.\n");
        return IROT_ERROR_BAD_PARAMETERS;
    }

    if (strlen(server_random) == 0 ||
        strlen(server_random) > ID2_MAX_SERVER_RANDOM_LEN) {
        id2_log_error("invalid server random length, %d.\n", strlen(server_random));
        return IROT_ERROR_BAD_PARAMETERS;
    }

    /* check extra */
    if (extra_len > ID2_MAX_EXTRA_LEN) {
        id2_log_error("invalid extra data length, %d.\n", extra_len);
        return IROT_ERROR_EXCESS_DATA;
    }

    /* check authcode */
    if (*auth_code_len < ID2_AUTH_CODE_BUF_LEN) {
        id2_log_error("auth code short buffer, %d %d.\n",
                       *auth_code_len, ID2_AUTH_CODE_BUF_LEN);
        *auth_code_len = ID2_AUTH_CODE_BUF_LEN;
        return IROT_ERROR_SHORT_BUFFER;
    }

    /* get ID */
    ret = id2_client_get_id(id2_str, &id2_len);
    if (ret != IROT_SUCCESS) {
        id2_log_error("get ID error.\n");
        return ret;
    }

    /* device random */
    id2_plat_get_random((uint8_t*)&device_random, sizeof(device_random));
    if (ls_osa_snprintf(device_random_str, sizeof(device_random_str),
                        "%08X%08X", device_random[0], device_random[1]) < 0) {
        id2_log_error("device random format exchange error.\n");
        return IROT_ERROR_GENERIC;
    }

    /* signature input = ID2 + device random + server random + extra */
    sign_in_len = ID2_ID_LEN + strlen(device_random_str) + strlen(server_random) + extra_len;
    sign_in = (uint8_t*)ls_osa_malloc(sign_in_len + 1);
    if (sign_in == NULL) {
        id2_log_error("out of mem, %d.\n", sign_in_len + 1);
        return IROT_ERROR_OUT_OF_MEMORY;
    } else {
        memset(sign_in, 0x00, sign_in_len + 1);
    }

    if (extra == NULL) {
        extra = (const uint8_t *)"";
    }

    if (ls_osa_snprintf((char*)sign_in, sign_in_len + 1, "%s%s%s%s",
               id2_str, device_random_str, server_random, extra) < 0) {
        id2_log_error("sign data format exchange error.\n");
        ret = IROT_ERROR_GENERIC;
        goto _out;
    }

    sign_out = (uint8_t*)ls_osa_malloc(MAX_SIGN_LEN);
    if (sign_out == NULL) {
        id2_log_error("out of mem, %d.\n", MAX_SIGN_LEN);
        ret = IROT_ERROR_OUT_OF_MEMORY;
        goto _out;
    } else {
        memset(sign_out, 0x00, MAX_SIGN_LEN);
    }

    /* ID2 signature */
    sign_out_len = MAX_SIGN_LEN;
    ret = _id2_sign(sign_in, sign_in_len, sign_out, &sign_out_len);
    if (ret != IROT_SUCCESS) {
        id2_log_error("id2 sign error.\n");
        goto _out;
    }

    /* base64 encode */
    sign_base64_len = sign_out_len * 2;
    sign_base64 = (uint8_t*)ls_osa_malloc(sign_base64_len);
    if (sign_base64 == NULL) {
        id2_log_error("out of mem, %d.\n", sign_base64_len);
        ret = IROT_ERROR_OUT_OF_MEMORY;
        goto _out;
    }

    memset(sign_base64, 0x00, sign_base64_len);
    ret = id2_plat_base64_encode(sign_out, sign_out_len, sign_base64, &sign_base64_len);
    if (ret != IROT_SUCCESS) {
        id2_log_error("base64 encode error.\n");
        goto _out;
    }

    id2_log_debug("base64 output: %s\n", sign_base64);

    /* auth_code = auth type ~ hash type ~ device random ~ server random ~ sign base64 */
    id2_auth_len = (uint32_t)(1 + 1 + strlen(device_random_str) + strlen(server_random) + sign_base64_len + 4);
    if (*auth_code_len <= id2_auth_len) {
        id2_log_error("short buffer: %d %d.\n", *auth_code_len, id2_auth_len);
        ret = IROT_ERROR_SHORT_BUFFER;
        goto _out;
    }

    if (ls_osa_snprintf((char*)auth_code, id2_auth_len + 1, "%d~%d~%s~%s~%s",
               auth_code_type, CONFIG_ID2_HASH_TYPE, device_random_str, server_random, sign_base64) < 0) {
        id2_log_error("auth code format exchange error.\n");
        ret = IROT_ERROR_GENERIC;
        goto _out;
    }

#ifdef CONFIG_ID2_AUTH_CODE_HARDEN
    if (id2_auth_code_encode((uint8_t *)auth_code, id2_auth_len, auth_code_len) < 0) {
        id2_log_error("auth code encode error.\n");
        ret = IROT_ERROR_GENERIC;
        goto _out;
    }
#else
    *auth_code_len = id2_auth_len;
#endif

    id2_log_debug("client authcode: [%d] %s\n", strlen((char *)auth_code), auth_code);

_out:
    if (sign_in) {
        ls_osa_free(sign_in);
    }
    if (sign_out) {
        ls_osa_free(sign_out);
    }
    if (sign_base64) {
        ls_osa_free(sign_base64);
    }

    return ret;
}

int is_id2_client_inited(void)
{
    return s_id2_client_inited;
}

irot_result_t id2_client_init(void)
{
    uint32_t km_ret;

    id2_log_debug("[id2_client_init enter.]\n");

    _dump_id2_conf_info();

    km_ret = km_init();
    if (km_ret != KM_SUCCESS) {
        id2_log_error("km init fail, %08x.\n", km_ret);
        return IROT_ERROR_GENERIC;
    }

    s_id2_id_len = 0;
    memset(s_id2_id, 0x00, ID2_ID_LEN + 1);

    s_device_random_len = 0;
    memset(s_device_random, 0x00, ID2_MAX_DEVICE_RANDOM_LEN);

    s_id2_client_inited = 1;

    return IROT_SUCCESS;
}

irot_result_t id2_client_cleanup(void)
{
    id2_log_debug("[id2_client_cleanup enter.]\n");

    km_cleanup();

    s_id2_client_inited = 0;

    return IROT_SUCCESS;
}

irot_result_t id2_client_get_version(uint32_t* version)
{
    id2_log_debug("[id2_client_get_version enter.]\n");

    if (!is_id2_client_inited()) {
        id2_log_error("id2 not inited.\n");
        return IROT_ERROR_GENERIC;
    }

    *version = ID2_CLIENT_VERSION_NUMBER;

    return IROT_SUCCESS;
}

irot_result_t id2_client_get_id(uint8_t* id, uint32_t* len)
{
    uint32_t km_ret;

    id2_log_debug("[id2_client_get_id enter.]\n");

    if (!is_id2_client_inited()) {
        id2_log_error("id2 not inited.\n");
        return IROT_ERROR_GENERIC;
    }

    if (id == NULL || len == NULL) {
        id2_log_error("id or len is NULL.\n");
        return IROT_ERROR_BAD_PARAMETERS;
    }

    if (*len < ID2_ID_LEN) {
        id2_log_error("short buffer, %d.\n", *len);
        *len = ID2_ID_LEN;
        return IROT_ERROR_SHORT_BUFFER;
    }

    if (s_id2_id_len == 0) {
        km_ret = km_get_id2(id, len);
        if (km_ret == KM_ERR_ITEM_NOT_FOUND) {
            id2_log_debug("item not found\n");
            return IROT_ERROR_ITEM_NOT_FOUND;
        } else if (km_ret != KM_SUCCESS) {
            id2_log_error("km get id fail, 0x%08x.\n", km_ret);
            return IROT_ERROR_GENERIC;
        }

        /* hex to ASCII if needed */
        if (*len == (ID2_ID_LEN >> 1)) {
            hex_to_string(id, *len, id, len);
            *len = ID2_ID_LEN;
        }

        if (*len != ID2_ID_LEN) {
            id2_log_error("invalid id length, %d.\n", *len);
            return IROT_ERROR_GENERIC;
        }

        /* cache the id2 */
        memcpy(s_id2_id, id, ID2_ID_LEN);
        s_id2_id_len = ID2_ID_LEN;

        id2_log_info("ID2: %s\n", s_id2_id);
    } else {
        memcpy(id, s_id2_id, ID2_ID_LEN);
        *len = ID2_ID_LEN;
    }

    return IROT_SUCCESS;
}

irot_result_t id2_client_get_challenge_auth_code(const char* server_random,
                                                 const uint8_t* extra, uint32_t extra_len,
                                                 uint8_t* auth_code, uint32_t* auth_code_len)
{
    uint8_t auth_code_type;

    id2_log_debug("[id2_client_get_challenge_auth_code enter.]\n");

    if (!is_id2_client_inited()) {
        id2_log_error("id2 not inited.\n");
        return IROT_ERROR_GENERIC;
    }

    auth_code_type = (extra == NULL) ? TYPE_CLIENT_CHALLENGE: TYPE_CLIENT_CHALLENGE_WITH_EXTRA;
    return _id2_gen_auth_code(auth_code_type, server_random,
                              extra, extra_len, auth_code, auth_code_len);
}

irot_result_t id2_client_get_timestamp_auth_code(const char* timestamp,
                                                 const uint8_t* extra, uint32_t extra_len,
                                                 uint8_t* auth_code, uint32_t* auth_code_len)
{
    uint8_t auth_code_type;

    id2_log_debug("[id2_client_get_timestamp_auth_code enter.]\n");

    if (!is_id2_client_inited()) {
        id2_log_error("id2 not inited.\n");
        return IROT_ERROR_GENERIC;
    }

    auth_code_type = (extra == NULL) ? TYPE_CLIENT_TIMESTAMP : TYPE_CLIENT_TIMESTAMP_WITH_EXTRA;
    return _id2_gen_auth_code(auth_code_type, timestamp,
                              extra, extra_len, auth_code, auth_code_len);
}

irot_result_t id2_client_decrypt(const uint8_t* in, uint32_t in_len, uint8_t* out, uint32_t* out_len)
{
    irot_result_t ret;
    uint8_t* data = NULL;
    uint32_t data_len;

    id2_log_debug("[id2_client_decrypt enter.]\n");

    if (!is_id2_client_inited()) {
        id2_log_error("id2 not inited.\n");
        return IROT_ERROR_GENERIC;
    }

    if (in == NULL || in_len == 0 || out == NULL || out_len == NULL) {
        id2_log_error("invalid input args.\n");
        return IROT_ERROR_BAD_PARAMETERS;
    }

    if (in_len > ID2_MAX_CRYPT_LEN) {
        id2_log_error("invalid input data length, %d.\n", in_len);
        return IROT_ERROR_EXCESS_DATA;
    }

    data_len = in_len;
    data = (uint8_t*)ls_osa_malloc(data_len);
    if (data == NULL) {
        id2_log_error("out of mem, %d.\n", data_len);
        return IROT_ERROR_OUT_OF_MEMORY;
    }

#if (CONFIG_ID2_KEY_TYPE == ID2_KEY_TYPE_3DES || \
     CONFIG_ID2_KEY_TYPE == ID2_KEY_TYPE_AES  || \
     CONFIG_ID2_KEY_TYPE == ID2_KEY_TYPE_SM1  || \
     CONFIG_ID2_KEY_TYPE == ID2_KEY_TYPE_SM4)
    ret = _id2_sym_cipher((uint8_t *)in, in_len, data, &data_len, 0);
    if (ret != IROT_SUCCESS) {
        id2_log_error("id2 sym decrypt fail, %d\n", ret);
        goto _out;
    }
#else
    ret = _id2_asym_decrypt((uint8_t *)in, in_len, data, &data_len);
    if (ret != IROT_SUCCESS) {
        id2_log_error("id2 asym decrypt fail, %d\n", ret);
        goto _out;
    }
#endif

    if (*out_len < data_len) {
        id2_log_error("short buffer, %d %d.\n", *out_len, data_len);
        ret = IROT_ERROR_SHORT_BUFFER;
        goto _out;
    }

    memcpy(out, data, data_len);
    *out_len = data_len;

_out:
    if (data != NULL) {
        ls_osa_free(data);
    }

    return ret;
}

irot_result_t id2_client_get_device_challenge(uint8_t* random, uint32_t* random_len)
{
    uint32_t device_random[2];

    id2_log_debug("[id2_client_get_device_challenge enter.]\n");

    if (!is_id2_client_inited()) {
        id2_log_error("id2 not inited.\n");
        return IROT_ERROR_GENERIC;
    }

    if (random == NULL || random_len == NULL || *random_len == 0) {
        id2_log_error("invalid input args.\n");
        return IROT_ERROR_BAD_PARAMETERS;
    }

    id2_plat_get_random((uint8_t*)&device_random, sizeof(device_random));
    if (ls_osa_snprintf((char*)random, ID2_MAX_DEVICE_RANDOM_LEN + 1,
               "%08X%08X", device_random[0], device_random[1]) < 0) {
        id2_log_error("device random format exchange error.\n");
        return IROT_ERROR_GENERIC;
    }

    if (*random_len > ID2_MAX_DEVICE_RANDOM_LEN) {
        *random_len = ID2_MAX_DEVICE_RANDOM_LEN;
    }

    memset(s_device_random, 0x00, ID2_MAX_DEVICE_RANDOM_LEN);
    memcpy(s_device_random, random, *random_len);
    s_device_random_len = *random_len;

    return IROT_SUCCESS;
}

irot_result_t id2_client_verify_server(
                         const uint8_t* auth_code, uint32_t auth_code_len,
                         const uint8_t* device_random, uint32_t device_random_len,
                         const uint8_t* server_extra, uint32_t server_extra_len)
{
    irot_result_t ret = IROT_ERROR_GENERIC;
    uint8_t auth_code_type;
    uint8_t hash_type;
    uint32_t length;
    uint32_t id2_len = ID2_ID_LEN;
    const uint8_t* head = NULL;
    const uint8_t* tail = NULL;

    uint8_t* sign_in = NULL;
    uint8_t* sign_out = NULL;
    uint8_t* sign_base64 = NULL;
    uint32_t sign_in_len;
    uint32_t sign_out_len;
    uint32_t sign_base64_len;

    id2_log_debug("[id2_client_verify_server enter.]\n");

    if (!is_id2_client_inited()) {
        id2_log_error("id2 not inited.\n");
        return IROT_ERROR_GENERIC;
    }

    if (auth_code == NULL || auth_code_len == 0) {
        id2_log_error("invalid input args.\n");
        return IROT_ERROR_BAD_PARAMETERS;
    }

    if (device_random != NULL && device_random_len == 0) {
        id2_log_error("invalid device random length.\n");
        return IROT_ERROR_BAD_PARAMETERS;
    }

    if (server_extra_len > ID2_MAX_EXTRA_LEN || (
        server_extra != NULL && server_extra_len == 0)) {
        id2_log_error("invalid server extra length, %d.\n", server_extra_len);
        return IROT_ERROR_EXCESS_DATA;
    }

    /* get the real device random */
    if (device_random == NULL) {
        device_random = s_device_random;
        device_random_len = s_device_random_len;
    }

#ifdef CONFIG_ID2_AUTH_CODE_HARDEN
    if (id2_auth_code_decode((uint8_t *)auth_code, &auth_code_len) < 0) {
        id2_log_error("auth code decode error.\n");
        return IROT_ERROR_GENERIC;
    }
#endif

    id2_log_debug("server authcode: [%d] %s", auth_code_len, auth_code);

    /* auth code type */
    head = auth_code;
    auth_code_type = (head[0] - '0') * 10 + (head[1] - '0');
    if (auth_code_type != TYPE_SERVER_CHALLENGE &&
        auth_code_type != TYPE_SERVER_CHALLENGE_WITH_EXTRA) {
        id2_log_error("invalid auth code type, %d.\n", auth_code_type);
        return IROT_ERROR_GENERIC;
    }

    id2_log_hex_dump("device random", device_random, device_random_len);
    id2_log_hex_dump("server extra", server_extra, server_extra_len);
    id2_log_hex_dump("server authcode", auth_code, auth_code_len);

    /* hash type */
    head += 3;
    hash_type = *head++ - '0';
    if (hash_type != CONFIG_ID2_HASH_TYPE) {
        id2_log_error("invalid hash type, %d.\n", hash_type);
        return IROT_ERROR_GENERIC;
    }

    /* compare device random */
    head++;
    tail = (uint8_t*)strchr((const char*)head, '~');
    if (tail == NULL) {
        id2_log_error("auth code format error.\n");
        return IROT_ERROR_GENERIC;
    }

    length = tail - head;
    if (device_random_len != length) {
        id2_log_error("device random length mismatch, %d %d.\n", device_random_len, length);
        return IROT_ERROR_GENERIC;
    }

    if (memcmp(head, device_random, device_random_len) != 0) {
        id2_log_error("device random mismatch.\n");
        return IROT_ERROR_GENERIC;
    }

    /* id2 signature */
    head = tail + 1;
    sign_in_len = ID2_ID_LEN + device_random_len + server_extra_len;
    sign_in = (uint8_t*)ls_osa_malloc(sign_in_len + 1);
    if (sign_in == NULL) {
        id2_log_error("out of mem, %d.\n", sign_in_len + 1);
        return IROT_ERROR_OUT_OF_MEMORY;
    }

    memset(sign_in, 0x00, sign_in_len + 1);
    ret = id2_client_get_id(sign_in, &id2_len);
    if (ret != IROT_SUCCESS) {
        id2_log_error("get ID fail, %d.\n", ret);
        goto _out;
    } else {
        /* inverse id2 */
        _id2_inverse(sign_in);
    }

    memcpy(sign_in + ID2_ID_LEN, device_random, device_random_len);
    if (auth_code_type == TYPE_SERVER_CHALLENGE_WITH_EXTRA) {
        if (server_extra == NULL) {
            id2_log_error("no server extra.\n");
            ret = IROT_ERROR_GENERIC;
            goto _out;
        }
        memcpy(sign_in + ID2_ID_LEN + device_random_len, server_extra, server_extra_len);
    }

    /* signature output buffer */
    sign_out = (uint8_t*)ls_osa_malloc(MAX_SIGN_LEN);
    if (sign_out == NULL) {
        id2_log_error("out of mem, %d.\n", MAX_SIGN_LEN);
        ret = IROT_ERROR_OUT_OF_MEMORY;
        goto _out;
    } else {
        memset(sign_out, 0x00, MAX_SIGN_LEN);
    }

    /* id2 sign */
    sign_out_len = MAX_SIGN_LEN;
    ret = _id2_sign(sign_in, sign_in_len, sign_out, &sign_out_len);
    if (ret != 0) {
        id2_log_error("id2 sign error.\n");
        goto _out;
    }

    /* base64 encode */
    sign_base64_len = sign_out_len * 2;
    sign_base64 = (uint8_t*)ls_osa_malloc(sign_base64_len);
    if (sign_base64 == NULL) {
        id2_log_error("out of mem, %d.\n", sign_base64_len);
        ret = IROT_ERROR_OUT_OF_MEMORY;
        goto _out;
    }
    memset(sign_base64, 0x00, sign_base64_len);
    id2_plat_base64_encode(sign_out, sign_out_len, sign_base64, &sign_base64_len);

    id2_log_debug("base64 output: %s\n", sign_base64);

    length = auth_code_len - (head - auth_code);
    if (sign_base64_len != length) {
        id2_log_error("invalid base64 length, %d %d.\n", sign_base64_len, length);
        ret = IROT_ERROR_GENERIC;
        goto _out;
    }

    if (memcmp(sign_base64, head, length) != 0) {
        id2_log_error("signature compare error.\n");
        ret = IROT_ERROR_GENERIC;
        goto _out;
    }

    ret = IROT_SUCCESS;

_out:
    if (sign_in) {
        ls_osa_free(sign_in);
    }
    if (sign_out) {
        ls_osa_free(sign_out);
    }
    if (sign_base64) {
        ls_osa_free(sign_base64);
    }

    return ret;
}

irot_result_t id2_client_get_secret(const char* seed, uint8_t* secret, uint32_t* secret_len)
{
    irot_result_t ret = IROT_SUCCESS;
    uint32_t in_len;
    uint8_t sign_buf[0x80];
    uint8_t out_buf[32];
    uint32_t sign_len;
    uint32_t out_len;

    id2_log_debug("[id2_client_get_secret enter.]\n");

    if (seed == NULL || secret == NULL || secret_len == NULL) {
        id2_log_error("invalid input args.\n");
        return IROT_ERROR_BAD_PARAMETERS;
    }

    in_len = strlen(seed);
    if (in_len == 0) {
        id2_log_error("seed is null.\n");
        return IROT_ERROR_BAD_PARAMETERS;
    }

    if (*secret_len < ID2_DERIV_SECRET_LEN) {
        id2_log_error("short buffer, %d.\n", *secret_len);
        *secret_len = ID2_DERIV_SECRET_LEN;
        return IROT_ERROR_SHORT_BUFFER;
    }

    id2_log_info("seed: %s\n", seed);

    sign_len = sizeof(sign_buf);
    ret = _id2_sign((uint8_t*)seed, in_len, sign_buf, &sign_len);
    if (ret != IROT_SUCCESS) {
        id2_log_error("id2 sign fail, %d.\n", ret);
        return ret;
    }

    out_len = sizeof(out_buf);
    ret = _id2_hash(sign_buf, sign_len, out_buf, &out_len);
    if (ret != IROT_SUCCESS) {
        id2_log_error("id2 hash fail, %d.\n", ret);
        return ret;
    }

    id2_log_hex_dump("id2 hash output:", out_buf, out_len);

    hex_to_string(out_buf, out_len, secret, secret_len);

    id2_log_debug("secret: %s\n", secret);

    return IROT_SUCCESS;
}

#if defined(CONFIG_ID2_DEBUG)
irot_result_t id2_client_set_id2_and_key(const char* id2, int key_type, const char* key_value)
{
    int ret;
    uint32_t id2_len;
    uint32_t key_len;
    uint8_t *key_data = NULL;
    km_key_data_t key_struct;
    int km_ret;

    id2_log_debug("[id2_client_set_id2_and_key enter.]\n");

    if (id2 == NULL || key_value == NULL) {
        id2_log_error("invalid input args.\n");
        return IROT_ERROR_BAD_PARAMETERS;
    }

    id2_len = strlen(id2);
    key_len = strlen(key_value);

#if (CONFIG_ID2_KEY_TYPE == ID2_KEY_TYPE_3DES)
    if (key_type != 0x01) {
        id2_log_error("key type error, %d\n", key_type);
        return IROT_ERROR_BAD_PARAMETERS;
    }
#elif (CONFIG_ID2_KEY_TYPE == ID2_KEY_TYPE_AES)
    if (key_type != 0x02) {
        id2_log_error("key type error, %d\n", key_type);
        return IROT_ERROR_BAD_PARAMETERS;
    }
#else
    {
        id2_log_error("not support this key type, %d\n", key_type);
        return IROT_ERROR_NOT_SUPPORTED;
    }
#endif

    id2_log_info("set id2: %s\n", id2);
    id2_log_info("set key data: %s\n", key_value);
    id2_log_info("set key type: %s\n", key_type == 0x01 ? "3DES" : "AES");

    if (id2_len != ID2_ID_LEN) {
        id2_log_error("id2 length error, %d\n", id2_len);
        return IROT_ERROR_BAD_PARAMETERS;
    }

    key_data = (uint8_t *)ls_osa_malloc(key_len);
    if (key_data == NULL) {
        id2_log_error("out of mem, %d\n", key_len);
        return IROT_ERROR_OUT_OF_MEMORY;
    }

    ret = string_to_hex((const uint8_t*)key_value, key_len, key_data, &key_len);
    if (ret < 0) {
        id2_log_error("id2 key string to hex error.\n");
        ls_osa_free(key_data);
        return IROT_ERROR_BAD_PARAMETERS;
    }

    id2_log_hex_dump("id2_key", key_data, key_len);

    if (key_type == 0x01) {
        key_struct.type = KM_DES3;
    } else if (key_type == 0x02) {
        key_struct.type = KM_AES;
    }

    key_struct.sym_key.key_bit = key_len << 3;
    key_struct.sym_key.key = key_data;

    km_delete_key(ID2_KEY_NAME, ID2_KEY_NAME_LEN);

    km_ret = km_set_id2((uint8_t*)id2, ID2_ID_LEN);
    if (km_ret != KM_SUCCESS) {
        id2_log_error("km set id2 error, 0x%08x\n", km_ret);
        ls_osa_free(key_data);
        return IROT_ERROR_GENERIC;
    }

    km_ret = km_import_key(ID2_KEY_NAME, ID2_KEY_NAME_LEN,
                KM_KEY_FORMAT_RAW, &key_struct, sizeof(key_struct));
    if (km_ret != KM_SUCCESS) {
        id2_log_error("km import key error, 0x%08x\n", km_ret);
        ls_osa_free(key_data);
        return IROT_ERROR_GENERIC;
    }

    id2_log_info("set id2 and key success!\n");

    s_id2_id_len = 0;

    ls_osa_free(key_data);

    return IROT_SUCCESS;
}
#endif

