/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include "config.h"
#include "version.h"
#include "irot_pal.h"
#include "id2_client.h"
#include "log/log.h"
#include "util/util.h"
#include "alg/alg.h"
#include "alg/hashsum.h"
#include "alg/basex64.h"
#include "km.h"
#include "keyname.h"

////////////////////////////////////////////////////////////////////////////////

#define TYPE_MIN_VALUE                      0
#define TYPE_CHALLENGE_WITHOUT_EXTRA        0
#define TYPE_CHALLENGE_WITH_EXTRA           2
#define TYPE_TIMESTAMP_WITHOUT_EXTRA        1
#define TYPE_TIMESTAMP_WITH_EXTRA           3
#define TYPE_MAX_VALUE                      3

////////////////////////////////////////////////////////////////////////////////

#define TYPE_CHALLENGE_WITHOUT_EXTRA_ITLS   10
#define TYPE_CHALLENGE_WITH_EXTRA_ITLS      11

////////////////////////////////////////////////////////////////////////////////

#define MAX_SIGN_LEN                        128
#define MAX_HASH_LENGTH                     64
#define MAX_SYMM_BLOCK_LENGTH               16

////////////////////////////////////////////////////////////////////////////////

enum
{
    ID2_CACHE_NO    =   0,
    ID2_CACHE_YES   =   1,
};

enum
{
    ID2_CLIENT_INITED_NO    =   0,
    ID2_CLIENT_INITED_YES   =   1,
};

uint8_t s_id2_cache_flag = ID2_CACHE_NO;
static uint8_t s_id2_cache_buf[ID2_ID_LEN + 1] = {0};
uint8_t s_id2_client_inited_flag = ID2_CLIENT_INITED_NO;

#if (ID2_ITLS_SUPPORTED)
static uint8_t s_device_random[ID2_MAX_DEVICE_RANDOM_LEN];
static uint32_t s_device_random_len = 0;
#endif

////////////////////////////////////////////////////////////////////////////////

void dump_id2_conf_info(void)
{
    irot_pal_log("ID2 Client Version: 0x%08X\n", ID2_CLIENT_VERSION_NUMBER);
    irot_pal_log("ID2 Client Build Time: %s %s\n", __DATE__, __TIME__);
    irot_pal_log("========================================\n");
    irot_pal_log("ID2_DEBUG                  : %d\n", ID2_DEBUG);
    irot_pal_log("ID2_OTP_LOCAL_TEST         : %d\n", ID2_OTP_LOCAL_TEST);
    irot_pal_log("ID2_LOCAL_TEST             : %d\n", ID2_LOCAL_TEST);
    irot_pal_log("ID2_SET_ID_KEY_SUPPORTED   : %d\n", ID2_SET_ID_KEY_SUPPORTED);
    irot_pal_log("ID2_KM_API_EMU             : %d\n", ID2_KM_API_EMU);
    irot_pal_log("ID2_ITLS_SUPPORTED         : %d\n", ID2_ITLS_SUPPORTED);
    irot_pal_log("ID2_OTP_SUPPORTED          : %d\n", ID2_OTP_SUPPORTED);
    irot_pal_log("ID2_USE_ALI_CRYPTO         : %d\n", ID2_USE_ALI_CRYPTO);
    switch (ID2_CRYPTO_TYPE_CONFIG)
    {
    case ID2_CRYPTO_TYPE_3DES:
		{
			irot_pal_log("ID2_CRYPTO_TYPE_CONFIG     : %s\n", "ID2_CRYPTO_TYPE_3DES");
		}break;
	case ID2_CRYPTO_TYPE_AES:
		{
			irot_pal_log("ID2_CRYPTO_TYPE_CONFIG     : %s\n", "ID2_CRYPTO_TYPE_AES");
		}break;
	case ID2_CRYPTO_TYPE_RSA:
		{
			irot_pal_log("ID2_CRYPTO_TYPE_CONFIG     : %s\n", "ID2_CRYPTO_TYPE_RSA");
		}break;
	default:
		{
		}break;
	}
    irot_pal_log("========================================\n");
}

void id2_reset_cache(void)
{
    s_id2_cache_flag = ID2_CACHE_NO;
}

int is_id2_client_inited(void)
{
    return (s_id2_client_inited_flag == ID2_CLIENT_INITED_YES);
}

irot_result_t id2_client_init(void)
{
    uint32_t kmret;
    id2_log_debug("[%s] enter.\n", __FUNC_NAME__);
    dump_id2_conf_info();
    s_id2_client_inited_flag = ID2_CLIENT_INITED_NO;

    s_id2_cache_flag = ID2_CACHE_NO;
    memset(s_id2_cache_buf, 0x00, ID2_ID_LEN);

#if (ID2_ITLS_SUPPORTED)
    s_device_random_len = 0;
    memset(s_device_random, 0x00, ID2_MAX_DEVICE_RANDOM_LEN);
#endif

    kmret = km_init();
    if (kmret != KM_SUCCESS)
    {
        id2_log_error("ERROR: [%s] KM init error, (ret = 0x%08X).\n", __FUNC_NAME__, kmret);
        return IROT_ERROR_GENERIC;
    }

    s_id2_client_inited_flag = ID2_CLIENT_INITED_YES;

    id2_log_debug("[%s] exit.\n", __FUNC_NAME__);
    return IROT_SUCCESS;
}


irot_result_t id2_client_cleanup(void)
{
    id2_log_debug("[%s] enter.\n", __FUNC_NAME__);
    km_cleanup();
    s_id2_client_inited_flag = ID2_CLIENT_INITED_NO;
    id2_log_debug("[%s] exit.\n", __FUNC_NAME__);
    return IROT_SUCCESS;
}

irot_result_t id2_client_get_version(uint32_t* pversion)
{
    irot_result_t ret = IROT_SUCCESS;
    if (!is_id2_client_inited())
    {
        id2_log_error("ERROR: [%s] id2 client not inited.\n", __FUNC_NAME__);
        ret = IROT_ERROR_GENERIC;
    }
    else
    {
        *pversion = ID2_CLIENT_VERSION_NUMBER;
    }
    return ret;
}

irot_result_t id2_client_get_id(uint8_t* id, uint32_t* len)
{
    irot_result_t ret = IROT_SUCCESS;
    uint32_t kmret;
    id2_log_debug("[%s] enter.\n", __FUNC_NAME__);

    if (!is_id2_client_inited())
    {
        id2_log_error("ERROR: [%s] id2 client not inited.\n", __FUNC_NAME__);
        ret = IROT_ERROR_GENERIC;
        goto EXIT;
    }

    // check parameter
    if ((id == NULL) || (len == NULL))
    {
        id2_log_error("ERROR: [%s] id or length is NULL.\n", __FUNC_NAME__);
        ret = IROT_ERROR_BAD_PARAMETERS;
        goto EXIT;
    }

    //check length
    if (*len < ID2_ID_LEN)
    {
        id2_log_error("ERROR: [%s] id buffer must >= %d.\n", __FUNC_NAME__, ID2_ID_LEN);
        ret = IROT_ERROR_SHORT_BUFFER;
        goto EXIT;
    }

    //get ID from HAL
    if (s_id2_cache_flag != ID2_CACHE_YES)
    {
        kmret = km_get_id2(id, len);
        if (kmret == KM_ERR_ITEM_NOT_FOUND)
        {
            id2_log_debug("km_get_id2: KM_ERR_ITEM_NOT_FOUND\n");
            ret = IROT_ERROR_ITEM_NOT_FOUND;
            goto EXIT;
        }
        if (kmret != KM_SUCCESS)
        {
            id2_log_error("ERROR: [%s] KM get id error, (ret = 0x%08X).\n", __FUNC_NAME__, kmret);
            ret = IROT_ERROR_GENERIC;
            goto EXIT;
        }

        //from hex to ASCII
        if (*len == (ID2_ID_LEN >> 1))
        {
            hex_to_ascii(id, *len, id, len);
            *len = ID2_ID_LEN;
        }
        if (*len != ID2_ID_LEN)
        {
            id2_log_error("ERROR: [%s] HAL get id length error, (length = %d).\n", __FUNC_NAME__, *len);
            ret = IROT_ERROR_GENERIC;
            goto EXIT;
        }
        //cache the ID2
        memcpy(s_id2_cache_buf, id, ID2_ID_LEN);
        s_id2_cache_flag = ID2_CACHE_YES;
    }
    //get ID from cache
    else
    {
        memcpy(id, s_id2_cache_buf, ID2_ID_LEN);
        *len = ID2_ID_LEN;
    }
    id2_log_debug("Get ID2: %s\n", s_id2_cache_buf);
EXIT:
    id2_log_debug("[%s] exit.\n", __FUNC_NAME__);
    return ret;
}

static irot_result_t id2_hash(uint8_t* sign_in, uint32_t sign_in_len, uint8_t* hash_buf, uint32_t* hash_len)
{
    irot_result_t ret;
    // hash in PAL
    ret = hash_sum(sign_in, sign_in_len, hash_buf, hash_len, DIGEST_TYPE_SHA256);
    id2_log_debug("[%s] exit.\n", __FUNC_NAME__);
    return ret;
}

#if ((ID2_CRYPTO_TYPE_CONFIG == ID2_CRYPTO_TYPE_3DES) || (ID2_CRYPTO_TYPE_CONFIG == ID2_CRYPTO_TYPE_AES))
static irot_result_t sym_cipher(const uint8_t* in, uint32_t in_len, uint8_t* out, uint32_t* out_len, int is_enc)
{
    irot_result_t ret = IROT_SUCCESS;
    uint32_t kmret;
    uint32_t block_len;
    int result;
    size_t output_len;
    km_sym_param kmsym_param;

    id2_log_debug("[%s] enter.\n", __FUNC_NAME__);
    // sym algorithm type
#if (ID2_CRYPTO_TYPE_CONFIG == ID2_CRYPTO_TYPE_3DES)
    block_len = DES_BLOCK_SIZE;
    kmsym_param.key_type = KM_DES3;
#elif (ID2_CRYPTO_TYPE_CONFIG == ID2_CRYPTO_TYPE_AES)
    block_len = AES_BLOCK_SIZE;
    kmsym_param.key_type = KM_AES;
#else
#error("id2 crypto type configuration error.")
#endif

    id2_log_hex_data("sym cipher input data:", in, in_len);
    if ((in_len % block_len) != 0)
    {
        id2_log_error("ERROR: [%s] input data length must be a multiple of %d.\n", __FUNC_NAME__, block_len);
        ret = IROT_ERROR_BAD_PARAMETERS;
        goto EXIT;
    }

    if (*out_len < in_len)
    {
        id2_log_error("ERROR: [%s] output buffer must >= input length(%d)", __FUNC_NAME__, in_len);
        ret = IROT_ERROR_SHORT_BUFFER;
        goto EXIT;
    }

    kmsym_param.cipher_param.block_mode = KM_ECB;
    kmsym_param.cipher_param.padding_type = KM_NO_PADDING;
    kmsym_param.cipher_param.purpose_type = is_enc ? KM_PURPOSE_ENCRYPT : KM_PURPOSE_DECRYPT;
    output_len = *out_len;

    kmret = km_cipher(ID2_KEY_NAME, ID2_KEY_NAME_LEN, &kmsym_param, NULL, 0, (uint8_t*)in, in_len, out, &output_len);
    if (kmret != KM_SUCCESS)
    {
        id2_log_error("ERROR: [%s] KM cipher error, (ret = 0x%8X).\n", __FUNC_NAME__, kmret);
        ret = IROT_ERROR_GENERIC;
        goto EXIT;
    }
    *out_len = output_len;
    id2_log_hex_data("sym cipher output data:", out, *out_len);

    //check output length
    if ((*out_len < block_len) || (*out_len % block_len != 0))
    {
        id2_log_error("ERROR: [%s] output length error.\n", __FUNC_NAME__);
        ret = IROT_ERROR_GENERIC;
        goto EXIT;
    }
    if (!is_enc)
    {
        id2_log_debug("[%s] sym cipher pkcs5 unpading data.\n", __FUNC_NAME__);
        result = pkcs5_unpading(out, *out_len, out_len, block_len);
        if (result != 0)
        {
            id2_log_debug("ERROR: [%s] pkcs5 unpading data error.\n", __FUNC_NAME__);
            ret = IROT_ERROR_GENERIC;
            goto EXIT;
        }
    }
EXIT:
    id2_log_debug("[%s] exit.\n", __FUNC_NAME__);
    return ret;
}

static irot_result_t sym_sign(uint8_t* sign_in, uint32_t sign_in_len, uint8_t* sign_out, uint32_t* sign_out_len)
{
    irot_result_t ret = IROT_SUCCESS;
    uint8_t hash_buf[MAX_HASH_LENGTH + MAX_SYMM_BLOCK_LENGTH];
    uint32_t hash_len = sizeof(hash_buf);
    uint32_t input_len;
    uint8_t block_len;
    int result;

    id2_log_debug("[%s] enter.\n", __FUNC_NAME__);
    id2_log_hex_data("symmetric algorithm signature input data:", sign_in, sign_in_len);
    // compute the hash
    ret = id2_hash(sign_in, sign_in_len, hash_buf, &hash_len);
    if (ret != IROT_SUCCESS)
    {
        id2_log_error("ERROR: [%s] hash compute error.\n", __FUNC_NAME__);
        goto EXIT;
    }
    id2_log_hex_data("hash:", hash_buf, hash_len);

    // padding the hash with PKCS5
#if (ID2_CRYPTO_TYPE_CONFIG == ID2_CRYPTO_TYPE_3DES)
    block_len = DES_BLOCK_SIZE;
#else
    block_len = AES_BLOCK_SIZE;
#endif
    result = pkcs5_pading(hash_buf, hash_len, &input_len, block_len);
    if (result != 0)
    {
        id2_log_debug("ERROR: pkcs5 padding data error\n");
        goto EXIT;
    }

    id2_log_hex_data("after padding hash:", hash_buf, input_len);
    ret = sym_cipher(hash_buf, input_len, sign_out, sign_out_len, 1);
EXIT:
    id2_log_debug("[%s] exit.\n", __FUNC_NAME__);
    return ret;
}
#endif

#if (ID2_CRYPTO_TYPE_CONFIG == ID2_CRYPTO_TYPE_RSA)
static irot_result_t asym_sign(uint8_t* sign_in, uint32_t sign_in_len, uint8_t* sign_out, uint32_t* sign_out_len)
{
    km_sign_param sign_params;
    uint32_t kmret;

    id2_log_debug("[%s] enter.\n", __FUNC_NAME__);
    // hash and signature type
#if (ID2_HASH_TYPE_CONFIG == ID2_HASH_TYPE_SHA1)
    sign_params.digest_type = KM_SHA1;
#elif (ID2_HASH_TYPE_CONFIG == ID2_HASH_TYPE_SHA256)
    sign_params.digest_type = KM_SHA256;
#else
#error("ID2 hash type configuration error.");
#endif
    sign_params.padding_type = KM_PKCS1;
    id2_log_hex_data("asymmetric algorithm signature input data:", sign_in, sign_in_len);

    kmret = km_sign(ID2_KEY_NAME, ID2_KEY_NAME_LEN, &sign_params, sign_in, sign_in_len, sign_out, sign_out_len);
    if (kmret != KM_SUCCESS)
    {
        id2_log_error("ERROR: [%s] KM sign error, (ret = 0x%08X)\n", __FUNC_NAME__, kmret);
        return IROT_ERROR_GENERIC;
    }
    id2_log_hex_data("signature output:", sign_out, *sign_out_len);
    id2_log_debug("[%s] exit.\n", __FUNC_NAME__);
    return IROT_SUCCESS;
}
#endif

static irot_result_t id2_sign(uint8_t* sign_in, uint32_t sign_in_len, uint8_t* sign_out, uint32_t* sign_out_len)
{
    irot_result_t ret;
#if ((ID2_CRYPTO_TYPE_CONFIG == ID2_CRYPTO_TYPE_3DES) || (ID2_CRYPTO_TYPE_CONFIG == ID2_CRYPTO_TYPE_AES))
    ret = sym_sign(sign_in, sign_in_len, sign_out, sign_out_len);
#else
    ret = asym_sign(sign_in, sign_in_len, sign_out, sign_out_len);
#endif
    id2_log_debug("[%s] exit.\n", __FUNC_NAME__);
    return ret;
}

static irot_result_t id2_gen_auth_code(uint8_t auth_code_type, const char* server_random,
                                       const uint8_t* extra, uint32_t extra_len,
                                       uint8_t* auth_code, uint32_t* auth_code_len)
{
    irot_result_t ret = IROT_SUCCESS;

    uint8_t id2_sbuf[ID2_ID_LEN + 1] = {0};
    uint32_t id2_len = ID2_ID_LEN;

    uint32_t drandom_array[2];
    char drandom_sbuf[ID2_MAX_DEVICE_RANDOM_LEN + 1] = {0};

    uint8_t* sign_in = NULL;
    uint32_t sign_in_len = 0;

    uint8_t* sign_out = NULL;
    uint32_t sign_out_len = 0;

    uint8_t* sign_base64 = NULL;
    uint32_t sign_base64_len = 0;

    uint32_t auth_len = 0;
    const uint8_t* extra_p = (const uint8_t*)"";

    id2_log_debug("[%s] enter.\n", __FUNC_NAME__);

    //check parameters
    if ((auth_code == NULL) || (auth_code_len == NULL) || (server_random == NULL) || (auth_code_type > TYPE_MAX_VALUE))
    {
        id2_log_error("ERROR: [%s] null pointer or auth type error.\n", __FUNC_NAME__);
        ret = IROT_ERROR_BAD_PARAMETERS;
        goto EXIT;
    }

    //check server random
    if (strlen(server_random) > ID2_MAX_SERVER_RANDOM_LEN)
    {
        id2_log_error("ERROR: [%s] challenge or timestamp length must <= %d.\n", __FUNC_NAME__, ID2_MAX_SERVER_RANDOM_LEN);
        ret = IROT_ERROR_EXCESS_DATA;
        goto EXIT;
    }
    if (strlen(server_random) == 0)
    {
        id2_log_error("ERROR: [%s] challenge or timestamp length must != 0.\n", __FUNC_NAME__);
        ret = IROT_ERROR_BAD_PARAMETERS;
        goto EXIT;
    }

    //check extra
    if (extra_len > ID2_MAX_EXTRA_LEN)
    {
        id2_log_error("ERROR: [%s] extra data length must <= %d.\n", __FUNC_NAME__, ID2_MAX_EXTRA_LEN);
        ret = IROT_ERROR_EXCESS_DATA;
        goto EXIT;
    }

    //check authcode
    if (*auth_code_len < AUTH_CODE_BUF_LEN)
    {
        id2_log_error("ERROR: [%s] auth code buffer length must >= %d.\n", __FUNC_NAME__, AUTH_CODE_BUF_LEN);
        ret = IROT_ERROR_SHORT_BUFFER;
        goto EXIT;
    }

    //get ID
    ret = id2_client_get_id(id2_sbuf, &id2_len);
    if (ret != IROT_SUCCESS)
    {
        id2_log_error("ERROR: [%s] get ID error.\n", __FUNC_NAME__);
        goto EXIT;
    }

    //get device random
    ret = irot_pal_get_random((uint8_t*)&drandom_array, sizeof(drandom_array));
    if (ret != IROT_SUCCESS)
    {
        id2_log_error("ERROR: [%s] get random error.\n", __FUNC_NAME__);
        goto EXIT;
    }
#if (ID2_LOCAL_TEST)
    memset((uint8_t*)&drandom_array, 0xAB, sizeof(drandom_array));
#endif
    if (snprintf(drandom_sbuf, sizeof(drandom_sbuf), "%08X%08X", (unsigned int)drandom_array[0], (unsigned int)drandom_array[1]) < 0)
    {
        id2_log_error("ERROR: [%s] snprintf error for random buffer.\n", __FUNC_NAME__);
        ret = IROT_ERROR_GENERIC;
        goto EXIT;
    }

    //signature input_data = | ID2 | device random | server challenge | extra data |
    sign_in_len = ID2_ID_LEN + strlen(drandom_sbuf) + strlen(server_random) + extra_len;
    sign_in = (uint8_t*)irot_pal_memory_malloc(sign_in_len + 1);
    if (sign_in == NULL)
    {
        id2_log_error("ERROR: [%s] malloc memory for signature input data error.\n", __FUNC_NAME__);
        ret = IROT_ERROR_OUT_OF_MEMORY;
        goto EXIT;
    }
    memset(sign_in, 0x00, sign_in_len + 1);

    if (extra)
    {
        extra_p = extra;
    }
    if (snprintf((char*)sign_in, sign_in_len + 1, "%s%s%s%s", id2_sbuf, drandom_sbuf, server_random, (const char*)extra_p) < 0)
    {
        id2_log_error("ERROR: [%s] snprintf error for signature input data.\n", __FUNC_NAME__);
        ret = IROT_ERROR_GENERIC;
        goto EXIT;
    }

    //signature output buffer
    sign_out = (uint8_t*)irot_pal_memory_malloc(MAX_SIGN_LEN);
    if (sign_out == NULL)
    {
        id2_log_error("ERROR: [%s] malloc memory for signature output buffer error.\n", __FUNC_NAME__);
        ret = IROT_ERROR_OUT_OF_MEMORY;
        goto EXIT;
    }
    memset(sign_out, 0x00, MAX_SIGN_LEN);

    //ID2 signature
    sign_out_len = MAX_SIGN_LEN;
    id2_log_debug("signature input data: %s\n", sign_in);
    ret = id2_sign(sign_in, sign_in_len, sign_out, &sign_out_len);
    if (ret != IROT_SUCCESS)
    {
        id2_log_error("ERROR: [%s] signature error.\n", __FUNC_NAME__);
        goto EXIT;
    }

    //base64
    sign_base64_len = sign_out_len * 2;
    sign_base64 = (uint8_t*)irot_pal_memory_malloc(sign_base64_len);
    if (sign_base64 == NULL)
    {
        id2_log_error("ERROR: [%s] malloc memory for base64 output buffer error.\n", __FUNC_NAME__);
        ret = IROT_ERROR_OUT_OF_MEMORY;
        goto EXIT;
    }
    memset(sign_base64, 0x00, sign_base64_len);
    ret = basex64_encode(sign_out, sign_out_len, sign_base64, &sign_base64_len);
    if (ret != IROT_SUCCESS)
    {
        id2_log_error("ERROR: [%s] base64 encode error.\n", __FUNC_NAME__);
        goto EXIT;
    }
    id2_log_debug("base64 output: %s\n", sign_base64);

    //                  | ID2 | device random | server challenge | extra data |
    //| auth type | hash type | device random | server_challenge | base64     |
    auth_len = (uint32_t)(1 + 1 + strlen(drandom_sbuf) + strlen(server_random) + sign_base64_len + 4);
    if (*auth_code_len <= auth_len)
    {
        id2_log_error("ERROR: [%s] auth code buffer too short.\n", __FUNC_NAME__);
        ret = IROT_ERROR_SHORT_BUFFER;
        goto EXIT;
    }
    if (snprintf((char*)auth_code, auth_len + 1, "%d~%d~%s~%s~%s", auth_code_type, ID2_HASH_TYPE_CONFIG, drandom_sbuf, server_random, sign_base64) < 0)
    {
        id2_log_error("ERROR: [%s] snprintf auth code error.\n", __FUNC_NAME__);
        ret = IROT_ERROR_GENERIC;
        goto EXIT;
    }
    *auth_code_len = auth_len;
    id2_log_debug("auth_code: [%d] %s\n", strlen((char*)auth_code), auth_code);
EXIT:
    if (sign_in)
    {
        irot_pal_memory_free(sign_in);
    }
    if (sign_out)
    {
        irot_pal_memory_free(sign_out);
    }
    if (sign_base64)
    {
        irot_pal_memory_free(sign_base64);
    }
    id2_log_debug("[%s] exit.\n", __FUNC_NAME__);
    return ret;
}

irot_result_t id2_client_get_challenge_auth_code(const char* server_random,
        const uint8_t* extra, uint32_t extra_len,
        uint8_t* auth_code, uint32_t* auth_code_len)
{
    irot_result_t ret;
    uint8_t auth_code_type;

    if (!is_id2_client_inited())
    {
        id2_log_error("ERROR: [%s] id2 client not inited.\n", __FUNC_NAME__);
        return IROT_ERROR_GENERIC;
    }
    auth_code_type = (extra == NULL) ? TYPE_CHALLENGE_WITHOUT_EXTRA : TYPE_CHALLENGE_WITH_EXTRA;
    ret = id2_gen_auth_code(auth_code_type, server_random, extra, extra_len, auth_code, auth_code_len);
    return ret;
}

irot_result_t id2_client_get_timestamp_auth_code(const char* timestamp,
        const uint8_t* extra, uint32_t extra_len,
        uint8_t* auth_code, uint32_t* auth_code_len)
{
    irot_result_t ret;
    uint8_t auth_code_type;

    if (!is_id2_client_inited())
    {
        id2_log_error("ERROR: [%s] id2 client not inited.\n", __FUNC_NAME__);
        return IROT_ERROR_GENERIC;
    }
    auth_code_type = (extra == NULL) ? TYPE_TIMESTAMP_WITHOUT_EXTRA : TYPE_TIMESTAMP_WITH_EXTRA;
    ret = id2_gen_auth_code(auth_code_type, timestamp, extra, extra_len, auth_code, auth_code_len);
    return ret;
}



#if (ID2_CRYPTO_TYPE_CONFIG == ID2_CRYPTO_TYPE_RSA)
#define RSA_BLOCK_SIZE  128
irot_result_t asym_decrypt(const uint8_t* in, uint32_t in_len, uint8_t* out, uint32_t* out_len)
{
    irot_result_t ret = IROT_SUCCESS;
    km_enc_param enc_params;
    uint32_t kmret;
    uint32_t out_buf_len = *out_len;
    uint32_t out_data_len = 0;
    if (in_len % RSA_BLOCK_SIZE != 0)
    {
        return IROT_ERROR_BAD_PARAMETERS;
    }

    enc_params.padding_type = KM_PKCS1;
    while (in_len > 0)
    {
        *out_len = out_buf_len;
        //decrypt one block each time
        kmret = km_asym_decrypt(ID2_KEY_NAME, ID2_KEY_NAME_LEN, &enc_params, in, in_len, out, out_len);
        if (kmret != KM_SUCCESS)
        {
            id2_log_error("ERROR: [%s] KM asym decrypt error, (ret = 0x%8X).\n", __FUNC_NAME__, kmret);
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
#endif

irot_result_t id2_client_decrypt(const uint8_t* in, uint32_t in_len, uint8_t* out, uint32_t* out_len)
{
    irot_result_t ret;
    uint8_t* p = NULL;
    uint32_t buf_len;

    id2_log_debug("[%s] enter.\n", __FUNC_NAME__);
    if (!is_id2_client_inited())
    {
        id2_log_error("ERROR: [%s] id2 client not inited.\n", __FUNC_NAME__);
        ret = IROT_ERROR_GENERIC;
        goto EXIT;
    }
    if ((in == NULL) || (in_len == 0) || (out == NULL) || (out_len == NULL))
    {
        id2_log_error("ERROR: [%s] parameter error.\n", __FUNC_NAME__);
        ret = IROT_ERROR_BAD_PARAMETERS;
        goto EXIT;
    }
    if (in_len > ID2_MAX_CRYPTO_LEN)
    {
        id2_log_error("ERROR: [%s] input data length must <= %d.\n", __FUNC_NAME__, ID2_MAX_CRYPTO_LEN);
        ret = IROT_ERROR_EXCESS_DATA;
        goto EXIT;
    }

    p = (uint8_t*)irot_pal_memory_malloc(in_len);
    if (p == NULL)
    {
        ret = IROT_ERROR_OUT_OF_MEMORY;
        goto EXIT;
    }

    buf_len = in_len;

#if ((ID2_CRYPTO_TYPE_CONFIG == ID2_CRYPTO_TYPE_3DES) || (ID2_CRYPTO_TYPE_CONFIG == ID2_CRYPTO_TYPE_AES))
    ret = sym_cipher(in, in_len, p, &buf_len, 0);
#elif (ID2_CRYPTO_TYPE_CONFIG == ID2_CRYPTO_TYPE_RSA)
    ret = asym_decrypt(in, in_len, p, &buf_len);
#else
#error("id2 crypto type configuration error.");
#endif
    if (ret != IROT_SUCCESS)
    {
        goto EXIT;
    }
    if (buf_len > *out_len)
    {
        ret = IROT_ERROR_SHORT_BUFFER;
        goto EXIT;
    }
    memcpy(out, p, buf_len);
    *out_len = buf_len;
EXIT:
    if (p)
    {
        irot_pal_memory_free(p);
    }
    return ret;
}


#if (ID2_ITLS_SUPPORTED)
irot_result_t id2_client_get_device_challenge(uint8_t* device_random_buf, uint32_t* device_random_len)
{
    irot_result_t ret = IROT_SUCCESS;
    uint32_t random_array[2];

    id2_log_debug("[%s] enter.\n", __FUNC_NAME__);

    if (!is_id2_client_inited())
    {
        id2_log_error("ERROR: [%s] id2 client not inited.\n", __FUNC_NAME__);
        ret = IROT_ERROR_GENERIC;
        goto EXIT;
    }

    if ((device_random_buf == NULL) || (device_random_len == NULL))
    {
        id2_log_error("ERROR: [%s] param error.\n", __FUNC_NAME__);
        ret = IROT_ERROR_BAD_PARAMETERS;
        goto EXIT;
    }
    if (*device_random_len <= ID2_MAX_DEVICE_RANDOM_LEN)
    {
        id2_log_error("ERROR: [%s] device challenge buffer must > %d.\n", __FUNC_NAME__, ID2_MAX_DEVICE_RANDOM_LEN);
        ret = IROT_ERROR_SHORT_BUFFER;
        goto EXIT;
    }

    ret = irot_pal_get_random((uint8_t*)&random_array, sizeof(random_array));
    if (ret != IROT_SUCCESS)
    {
        id2_log_error("ERROR: [%s] get random error.\n", __FUNC_NAME__);
        goto EXIT;
    }
#if (ID2_LOCAL_TEST)
    memset((uint8_t*)&random_array, 0xAB, sizeof(random_array));
#endif
    if (snprintf((char*)device_random_buf, ID2_MAX_DEVICE_RANDOM_LEN + 1, "%08X%08X", (unsigned int)random_array[0], (unsigned int)random_array[1]) < 0)
    {
        id2_log_error("ERROR: [%s] snprintf device random error.\n", __FUNC_NAME__);
        ret = IROT_ERROR_GENERIC;
        goto EXIT;
    }
    *device_random_len = strlen((const char*)device_random_buf);

    memset(s_device_random, 0x00, ID2_MAX_DEVICE_RANDOM_LEN);
    memcpy(s_device_random, device_random_buf, *device_random_len);
    s_device_random_len = *device_random_len;
    id2_log_hex_data("device challenge", device_random_buf, *device_random_len);

EXIT:
    id2_log_debug("[%s] exit.\n", __FUNC_NAME__);
    return ret;
}

static void _inverse_id2(uint8_t* id2)
{
    int i = 0;
    int hex = 0;
    for (i = 0; i < ID2_ID_LEN; ++i)
    {
        hex = char_to_hex(*id2);
        hex = 0x0F - hex;
        *id2++ = hex_to_char(hex);
    }
}

irot_result_t id2_client_verify_server(const uint8_t* server_auth_code, uint32_t server_auth_code_len,
                                       const uint8_t* device_random, uint32_t device_random_len,
                                       const uint8_t* server_extra, uint32_t server_extra_len)
{
    irot_result_t ret = IROT_ERROR_GENERIC;
    uint32_t id2_len = ID2_ID_LEN;
    uint8_t auth_code_type;
    uint8_t hash_type;
    char* p = NULL;
    uint32_t len;

    const uint8_t* head = NULL;
    const uint8_t* tail = NULL;

    const uint8_t* pdrandom = NULL;
    uint32_t len1 = 0;
    uint32_t len2 = 0;

    uint8_t* sign_in = NULL;
    uint32_t sign_in_len;

    uint8_t* sign_out = NULL;
    uint32_t sign_out_len;

    uint8_t* sign_base64 = NULL;
    uint32_t sign_base64_len;

    id2_log_debug("[%s] enter.\n", __FUNC_NAME__);

    if (!is_id2_client_inited())
    {
        id2_log_error("ERROR: [%s] id2 client not inited.\n", __FUNC_NAME__);
        ret = IROT_ERROR_GENERIC;
        goto EXIT;
    }

    //check parameter
    if ((server_auth_code == NULL) || (server_auth_code_len == 0))
    {
        id2_log_error("ERROR: [%s] param error.\n", __FUNC_NAME__);
        ret = IROT_ERROR_BAD_PARAMETERS;
        goto EXIT;
    }
    if (server_extra_len > ID2_MAX_EXTRA_LEN)
    {
        id2_log_error("ERROR: [%s] extra length must <= %d.\n", __FUNC_NAME__, ID2_MAX_EXTRA_LEN);
        ret = IROT_ERROR_EXCESS_DATA;
        goto EXIT;
    }

    if (device_random == NULL)
    {
        if (device_random_len != 0)
        {
            id2_log_error("ERROR: [%s] device challenge length must = 0 when device challenge is NULL.\n", __FUNC_NAME__);
            ret = IROT_ERROR_BAD_PARAMETERS;
            goto EXIT;
        }
    }

    //get the real device random
    if (device_random == NULL)
    {
        pdrandom = s_device_random;
        len1 = s_device_random_len;
    }
    else
    {
        pdrandom = device_random;
        len1 = device_random_len;
    }

    //auth code type
    head = tail = server_auth_code;
    auth_code_type = (*head++ - '0') * 10;
    auth_code_type += (*head++ - '0');
    if ((auth_code_type != TYPE_CHALLENGE_WITHOUT_EXTRA_ITLS) && (auth_code_type != TYPE_CHALLENGE_WITH_EXTRA_ITLS))
    {
        id2_log_error("ERROR: [%s] auth code type error.\n", __FUNC_NAME__);
        ret = IROT_ERROR_GENERIC;
        goto EXIT;
    }

    //dump the input value
    id2_log_hex_data("server auth code", server_auth_code, server_auth_code_len);
    if (device_random)
    {
        id2_log_hex_data("device random", device_random, device_random_len);
    }
    id2_log_hex_data("server extra", server_extra, server_extra_len);

    //hash type
    head++;
    hash_type = *head++ - '0';
    if (hash_type != ID2_HASH_TYPE_CONFIG)
    {
        id2_log_error("ERROR: [%s] hash type error.\n", __FUNC_NAME__);
        ret = IROT_ERROR_GENERIC;
        goto EXIT;
    }

    //compare device random
    head++;
    tail = (uint8_t*)strchr((const char*)head, '~');
    if (tail == NULL)
    {
        id2_log_error("ERROR: [%s] auth code format error.\n", __FUNC_NAME__);
        ret = IROT_ERROR_GENERIC;
        goto EXIT;
    }

    len2 = tail - head;
    if (len1 != len2)
    {
        id2_log_error("ERROR: [%s] device challenge length error.\n", __FUNC_NAME__);
        ret = IROT_ERROR_GENERIC;
        goto EXIT;
    }

    if (memcmp(head, pdrandom, len1) != 0)
    {
        id2_log_error("ERROR: [%s] compare device challenge failed.\n", __FUNC_NAME__);
        ret = IROT_ERROR_GENERIC;
        goto EXIT;
    }

    //signature
    head = tail + 1;
    sign_in_len = ID2_ID_LEN + len1 + server_extra_len;
    sign_in = (uint8_t*)irot_pal_memory_malloc(sign_in_len + 1);
    if (sign_in == NULL)
    {
        id2_log_error("ERROR: [%s] malloc memory for signature input buffer error.\n", __FUNC_NAME__);
        ret = IROT_ERROR_OUT_OF_MEMORY;
        goto EXIT;
    }
    memset(sign_in, 0x00, sign_in_len + 1);
    ret = id2_client_get_id(sign_in, &id2_len);
    if (ret != IROT_SUCCESS)
    {
        id2_log_error("ERROR: [%s] get ID error.\n", __FUNC_NAME__);
        ret = IROT_ERROR_GENERIC;
        goto EXIT;
    }

    // inverse ID
    _inverse_id2(sign_in);

    memcpy(sign_in + ID2_ID_LEN, pdrandom, len1);
    if ((auth_code_type == TYPE_CHALLENGE_WITH_EXTRA_ITLS) && (server_extra != NULL))
    {
        memcpy(sign_in + ID2_ID_LEN + len1, server_extra, server_extra_len);
    }

    //signature output buffer
    sign_out = (uint8_t*)irot_pal_memory_malloc(MAX_SIGN_LEN);
    if (sign_out == NULL)
    {
        id2_log_error("ERROR: [%s] malloc memory for signature output buffer error.\n", __FUNC_NAME__);
        ret = IROT_ERROR_OUT_OF_MEMORY;
        goto EXIT;
    }
    memset(sign_out, 0x00, MAX_SIGN_LEN);

    //ID2 sign
    sign_out_len = MAX_SIGN_LEN;
    id2_log_debug("signature input data: %s\n", sign_in);
    ret = id2_sign(sign_in, sign_in_len, sign_out, &sign_out_len);
    if (ret != 0)
    {
        id2_log_error("ERROR: [%s] signature error.\n", __FUNC_NAME__);
        ret = IROT_ERROR_GENERIC;
        goto EXIT;
    }

    //base64
    sign_base64_len = sign_out_len * 2;
    sign_base64 = (uint8_t*)irot_pal_memory_malloc(sign_base64_len);
    if (sign_base64 == NULL)
    {
        id2_log_error("ERROR: [%s] malloc base64 output buffer error.\n", __FUNC_NAME__);
        ret = IROT_ERROR_OUT_OF_MEMORY;
        goto EXIT;
    }
    memset(sign_base64, 0x00, sign_base64_len);
    basex64_encode(sign_out, sign_out_len, sign_base64, &sign_base64_len);

    id2_log_debug("base64 output: %s\n", sign_base64);

    //output server authcode
    len = server_auth_code_len + 1;
    p = irot_pal_memory_malloc(len);
    if (p == NULL)
    {
        id2_log_error("ERROR: [%s] malloc temp buffer error.\n", __FUNC_NAME__);
        ret = IROT_ERROR_OUT_OF_MEMORY;
        goto EXIT;
    }
    memcpy(p, server_auth_code, server_auth_code_len);
    p[server_auth_code_len] = '\0';
    id2_log_debug("server authcode: %s\n", p);

    len2 = server_auth_code_len - (head - server_auth_code);
    if (sign_base64_len != len2)
    {
        id2_log_error("ERROR: [%s] signature length error %d != %d.\n", __FUNC_NAME__, sign_base64_len, len2);
        ret = IROT_ERROR_GENERIC;
        goto EXIT;
    }

    if (memcmp(sign_base64, head, len2) != 0)
    {
        id2_log_error("ERROR: [%s] signature compare error.\n", __FUNC_NAME__);
        ret = IROT_ERROR_GENERIC;
        goto EXIT;
    }
    else
    {
        ret = IROT_SUCCESS;
    }

    // remove the cached device challenge
    if (device_random == NULL)
    {
        memset(s_device_random, 0x00, ID2_MAX_DEVICE_RANDOM_LEN);
        s_device_random_len = 0;
    }
EXIT:
    if (p)
    {
        irot_pal_memory_free(p);
    }
    if (sign_in)
    {
        irot_pal_memory_free(sign_in);
    }
    if (sign_out)
    {
        irot_pal_memory_free(sign_out);
    }
    if (sign_base64)
    {
        irot_pal_memory_free(sign_base64);
    }
    id2_log_debug("[%s] exit.\n", __FUNC_NAME__);
    return ret;
}

#endif


irot_result_t id2_client_get_secret(const char* seed, uint8_t* secret, uint32_t* len)
{
    irot_result_t ret = IROT_SUCCESS;
    uint32_t in_len;
    uint8_t sign_buf[0x80];
    uint8_t out_buf[32];
    uint32_t sign_len;
    uint32_t out_len;

    if ((seed == NULL) || (secret == NULL) || (len == NULL))
    {
        ret = IROT_ERROR_BAD_PARAMETERS;
        goto EXIT;
    }
    in_len = strlen(seed);
    if (in_len == 0)
    {
        ret = IROT_ERROR_BAD_PARAMETERS;
        goto EXIT;
    }
    if (*len < 64)
    {
        ret = IROT_ERROR_SHORT_BUFFER;
        goto EXIT;
    }

    id2_log_debug("Seed: %s\n", seed);

    sign_len = sizeof(sign_buf);
    ret = id2_sign((uint8_t*)seed, in_len, sign_buf, &sign_len);
    if (ret != IROT_SUCCESS)
    {
        goto EXIT;
    }

    out_len = sizeof(out_buf);
    ret = hash_sum(sign_buf, sign_len, out_buf, &out_len, DIGEST_TYPE_SHA256);
    if (ret != IROT_SUCCESS)
    {
        goto EXIT;
    }

    id2_log_hex_data("output secret", out_buf, out_len);
    hex_to_ascii(out_buf, out_len, secret, len);

EXIT:
    return ret;
}


#if (ID2_SET_ID_KEY_SUPPORTED)
irot_result_t id2_client_set_id2_and_key(const char* id2, int key_type, const char* key)
{
    irot_result_t ret = IROT_SUCCESS;
    int vret;
    uint32_t out_len;
    uint8_t ID2[ID2_ID_LEN >> 1];
    uint8_t key_data[32];
    uint32_t id2_len;
    uint32_t key_len;
    km_key_data_t key_struct;
    int kmret;

    //check parameter
    if ((id2 == NULL) || (key == NULL))
    {
        ret = IROT_ERROR_BAD_PARAMETERS;
        goto EXIT;
    }

    id2_len = strlen(id2);
    key_len = strlen(key);

    //check keytype and key length
#if (ID2_CRYPTO_TYPE_CONFIG == ID2_CRYPTO_TYPE_3DES)
    if (key_type != 0x01)
    {
        id2_log_error("key type error, the configration is 3DES.\n");
        ret =  IROT_ERROR_BAD_PARAMETERS;
        goto EXIT;
    }
    if ((key_len != (16 * 2)) && (key_len != (24 * 2)))
    {
        id2_log_error("3DES key length error, key length is %d\n", key_len);
        ret = IROT_ERROR_BAD_PARAMETERS;
        goto EXIT;
    }
#elif (ID2_CRYPTO_TYPE_CONFIG == ID2_CRYPTO_TYPE_AES)
    if (key_type != 0x02)
    {
        id2_log_error("key type error, the configration is AES.\n");
        ret = IROT_ERROR_BAD_PARAMETERS;
        goto EXIT;
    }
    if ((key_len != (16 * 2)) && (key_len != (24 * 2)) && (key_len != (32 * 2)))
    {
        id2_log_error("AES key length error, key length is %d\n", key_len);
        ret = IROT_ERROR_BAD_PARAMETERS;
        goto EXIT;
    }
#else
#error("crypto type configration error");
#endif

    //check id2 length
    if (id2_len != ID2_ID_LEN)
    {
        id2_log_error("ID2 length error: %d\n", id2_len);
        ret = IROT_ERROR_BAD_PARAMETERS;
        goto EXIT;
    }

    out_len = sizeof(ID2);
    vret = ascii_to_hex((const uint8_t*)id2, id2_len, ID2, &out_len);
    if (vret != 0)
    {
        id2_log_error("ID2 value error: %s", id2);
        ret = IROT_ERROR_BAD_PARAMETERS;
        goto EXIT;
    }
    out_len = sizeof(key_data);
    vret = ascii_to_hex((const uint8_t*)key, key_len, key_data, &out_len);
    if (vret != 0)
    {
        id2_log_error("key value error: %s\n", key);
        ret = IROT_ERROR_BAD_PARAMETERS;
        goto EXIT;
    }

    if ((key_type == 0x01))
    {
        key_struct.type = KM_DES3;
    }
    else if (key_type == 0x02)
    {
        key_struct.type = KM_AES;
    }
    else
    {
        ret = IROT_ERROR_BAD_PARAMETERS;
        goto EXIT;
    }
    key_struct.sym_key.key_bit = ((key_len / 2) << 3);
    key_struct.sym_key.key = key_data;

    kmret = km_set_id2((uint8_t*)id2, ID2_ID_LEN);
    if (kmret != KM_SUCCESS)
    {
        id2_log_error("%s: KM set id2 error, 0x%08X\n", __FUNC_NAME__, kmret);
        ret = IROT_ERROR_GENERIC;
        goto EXIT;
    }
    kmret = km_delete_key(ID2_KEY_NAME, ID2_KEY_NAME_LEN);
    if (kmret != KM_SUCCESS)
    {
        //id2_log_error("%s: KM delete key, 0x%08X\n", __FUNC_NAME__, kmret);
    }
    kmret = km_import_key(ID2_KEY_NAME, ID2_KEY_NAME_LEN, KM_KEY_FORMAT_RAW, &key_struct, sizeof(key_struct));
    if (kmret != KM_SUCCESS)
    {
        id2_log_error("%s: KM import key error, 0x%08X\n", __FUNC_NAME__, kmret);
        ret = IROT_ERROR_GENERIC;
        goto EXIT;
    }
    id2_log_debug("Set ID2 Success: %s\n", id2);
    id2_log_debug("        KeyType: %d\n", key_type);
    id2_log_debug("Set Key Success: %s\n", key);
    //modify the cache flag
    s_id2_cache_flag = ID2_CACHE_NO;
EXIT:
    return ret;
}

#endif

