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
#include "km.h"
#include "keyname.h"


#if (ID2_OTP_SUPPORTED)

extern int is_id2_client_inited(void);
extern void id2_reset_cache(void);

enum
{
    KEY_TYPE_AES_128    =       1,
    KEY_TYPE_AES_192    =       2,
    KEY_TYPE_AES_256    =       3,
    KEY_TYPE_3DES_112   =       4,
    KEY_TYPE_3DES_168   =       5,
};

#define ID2_OTP_SS_KEY_LEN      0x10
#define ID2_OTP_AUTH_CODE_VER   0x00010000

#define ID2_SS_KEY_LEN          16

#define ID2_ROUNDUP(a, b)       (((a) + ((b) - 1)) & ~((b) - 1))

enum
{
    SHA256_HASH_SIZE    =       32,
};

static uint8_t g_ss_key[ID2_SS_KEY_LEN] = {0};

////////////////////////////////////////////////////////////////////////////////

static uint8_t* get_otp_prov_id(uint8_t* prov_key, uint32_t key_size)
{
    irot_result_t ret;
    uint8_t* prov_id = NULL;
    uint32_t prov_id_len;

    if (prov_key == NULL || key_size == 0)
    {
        id2_log_error("%s: invalid input args\n", __FUNC_NAME__);
        return NULL;
    }

    prov_id_len = 32;
    prov_id = irot_pal_memory_malloc(prov_id_len);
    if (prov_id == NULL)
    {
        id2_log_error("%s: malloc(%d) fail\n", __FUNC_NAME__, prov_id_len);
        return NULL;
    }

    ret = hash_sum(prov_key, key_size,
                            prov_id, &prov_id_len, DIGEST_TYPE_SHA256);
    if (ret != IROT_SUCCESS)
    {
        id2_log_error("%s: hal hash sum fail\n", __FUNC_NAME__, ret);
        irot_pal_memory_free(prov_id);
        return NULL;
    }

    return prov_id;;
}

static irot_result_t otp_get_rept_data(uint8_t key_type,
                                       uint8_t* key_data, uint32_t key_len,
                                       uint8_t* dev_fp, uint32_t dev_fp_len,
                                       uint8_t* rept_data, uint32_t* rept_len)
{
    irot_result_t ret;
    uint32_t offset;
    uint32_t padding;
    uint32_t hash_len;
    uint32_t data_len;
    uint32_t total_len;
    uint32_t block_size;
    uint8_t* prov_key = NULL;

    if (key_data == NULL || key_len == 0 ||
            dev_fp == NULL || dev_fp_len == 0 ||
            rept_data == NULL || rept_len == NULL)
    {
        id2_log_error("%s: invalid input args\n", __FUNC_NAME__);
        return IROT_ERROR_BAD_PARAMETERS;
    }

#if (ID2_CRYPTO_TYPE_CONFIG == ID2_CRYPTO_TYPE_AES)
    if (key_type == KEY_TYPE_AES_128 ||
            key_type == KEY_TYPE_AES_192 || key_type == KEY_TYPE_AES_256)
    {
        block_size = AES_BLOCK_SIZE;
    }
#elif (ID2_CRYPTO_TYPE_CONFIG == ID2_CRYPTO_TYPE_3DES)
    if (key_type == KEY_TYPE_3DES_112 || key_type == KEY_TYPE_3DES_168)
    {
        block_size = DES_BLOCK_SIZE;
    }
#endif
    else
    {
        id2_log_error("%s: not support this id2 key type: %d\n", __FUNC_NAME__, key_type);
        return IROT_ERROR_NOT_SUPPORTED;
    }

    prov_key = key_data;
    id2_log_hex_data("prov_key", prov_key, key_len);

    /* generate session key */
#if (ID2_OTP_LOCAL_TEST)
    memset(g_ss_key, '0', ID2_SS_KEY_LEN);
#else
    irot_pal_get_random(g_ss_key, ID2_SS_KEY_LEN);
#endif

    /* key_type + ss_key + dev_fp */
    data_len = 1 + ID2_SS_KEY_LEN + dev_fp_len;
    padding = block_size - data_len % block_size;

    total_len = SHA256_HASH_SIZE + data_len + padding;
    if (*rept_len < total_len)
    {
        id2_log_error("%s: rept data short buffer, %d\n", __FUNC_NAME__, *rept_len);
        *rept_len = total_len;
        ret = IROT_ERROR_SHORT_BUFFER;
        goto _out;
    }
    else
    {
        *rept_len = total_len;
    }

    offset = SHA256_HASH_SIZE;
    memcpy(rept_data + offset, &key_type, 1);
    offset += 1;
    memcpy(rept_data + offset, g_ss_key, ID2_SS_KEY_LEN);
    offset += ID2_SS_KEY_LEN;
    memcpy(rept_data + offset, dev_fp, dev_fp_len);
    offset += dev_fp_len;
    memset(rept_data + offset, padding, padding);

    id2_log_hex_data("rept_data", rept_data + SHA256_HASH_SIZE, data_len);

    hash_len = SHA256_HASH_SIZE;
    ret = hash_sum(
              rept_data + SHA256_HASH_SIZE, data_len,
              rept_data, &hash_len, DIGEST_TYPE_SHA256);
    if (ret != IROT_SUCCESS)
    {
        id2_log_error("%s: hal hash sum fail, %d\n", __FUNC_NAME__, ret);
        goto _out;
    }

#if (ID2_CRYPTO_TYPE_CONFIG == ID2_CRYPTO_TYPE_AES)
    data_len += padding;
    ret = aes_ecb(prov_key, key_len,
                  rept_data + SHA256_HASH_SIZE, data_len,
                  rept_data + SHA256_HASH_SIZE, &data_len, 1);
    if (ret != IROT_SUCCESS)
    {
        id2_log_error("%s: rept data encrypt fail, %d\n", __FUNC_NAME__, ret);
        goto _out;
    }
#elif (ID2_CRYPTO_TYPE_CONFIG == ID2_CRYPTO_TYPE_3DES)
    data_len += padding;
    ret = des3_ecb(prov_key, key_len,
                   rept_data + SHA256_HASH_SIZE, data_len,
                   rept_data + SHA256_HASH_SIZE, &data_len, 1);
    if (ret != IROT_SUCCESS)
    {
        id2_log_error("%s: rept data encrypt fail, %d\n", __FUNC_NAME__, ret);
        goto _out;
    }
#endif

_out:
    return ret;
}

static irot_result_t otp_set_otp_data(uint8_t* otp_data, uint32_t len)
{
    irot_result_t ret;
    uint32_t key_type;
    uint32_t key_size = 0;
    uint32_t key_data_len = 0;
    uint32_t kcv_data_len = 0;
    uint32_t otp_data_len = 0;
    uint32_t key_data_off = 0;
    uint32_t kcv_data_off = 0;
    uint8_t* id2_id = NULL;
    uint8_t* id2_key = NULL;
    uint32_t kmret;

    if (otp_data == NULL || len == 0)
    {
        id2_log_error("%s: invalid input args\n", __FUNC_NAME__);
        return IROT_ERROR_BAD_PARAMETERS;
    }

    id2_log_hex_data("otp_data", (uint8_t*)otp_data, len);

    key_type = otp_data[0];
#if (ID2_CRYPTO_TYPE_CONFIG == ID2_CRYPTO_TYPE_AES)
    if (key_type == KEY_TYPE_AES_128)
    {
        key_size = 16;
        key_data_len = ID2_ROUNDUP(key_size + 1, AES_BLOCK_SIZE);
        kcv_data_len = ID2_ROUNDUP(ID2_ID_LEN + 1, AES_BLOCK_SIZE);
    }
    else if (key_type == KEY_TYPE_AES_192)
    {
        key_size = 24;
        key_data_len = ID2_ROUNDUP(key_size + 1, AES_BLOCK_SIZE);
        kcv_data_len = ID2_ROUNDUP(ID2_ID_LEN + 1, AES_BLOCK_SIZE);
    }
    else if (key_type == KEY_TYPE_AES_256)
    {
        key_size = 32;
        key_data_len = ID2_ROUNDUP(key_size + 1, AES_BLOCK_SIZE);
        kcv_data_len = ID2_ROUNDUP(ID2_ID_LEN + 1, AES_BLOCK_SIZE);
    }
#elif (ID2_CRYPTO_TYPE_CONFIG == ID2_CRYPTO_TYPE_3DES)
    if (key_type == KEY_TYPE_3DES_112)
    {
        key_size = 16;
        key_data_len = ID2_ROUNDUP(key_size + 1, DES_BLOCK_SIZE);
        kcv_data_len = ID2_ROUNDUP(ID2_ID_LEN + 1, DES_BLOCK_SIZE);
    }
    else if (key_type == KEY_TYPE_3DES_168)
    {
        key_size = 24;
        key_data_len = ID2_ROUNDUP(key_size + 1, DES_BLOCK_SIZE);
        kcv_data_len = ID2_ROUNDUP(ID2_ID_LEN + 1, DES_BLOCK_SIZE);
    }
#else
#error("OTP not supported for RSA");
#endif

    otp_data_len = 1 + ID2_ID_LEN + key_data_len + kcv_data_len;
    if (len != otp_data_len)
    {
        id2_log_error("%s: otp len is not match: %d %d\n", __FUNC_NAME__, len, otp_data_len);
        return IROT_ERROR_GENERIC;
    }

    key_data_off = 1 + ID2_ID_LEN;
    kcv_data_off = 1 + ID2_ID_LEN + key_data_len;

    id2_key = irot_pal_memory_malloc(key_data_len);
    if (id2_key == NULL)
    {
        id2_log_error("%s: malloc %d fail\n", __FUNC_NAME__, key_data_len);
        return IROT_ERROR_OUT_OF_MEMORY;
    }

#if (ID2_CRYPTO_TYPE_CONFIG == ID2_CRYPTO_TYPE_AES)
    ret = aes_ecb(g_ss_key, ID2_SS_KEY_LEN,
                  otp_data + key_data_off, key_data_len,
                  id2_key, &key_data_len, 0);
    if (ret != IROT_SUCCESS)
    {
        id2_log_error("%s: id2 key decrypt fail, %d\n", __FUNC_NAME__, ret);
        goto _out;
    }
#elif (ID2_CRYPTO_TYPE_CONFIG == ID2_CRYPTO_TYPE_3DES)
    ret = des3_ecb(g_ss_key, ID2_SS_KEY_LEN,
                   otp_data + key_data_off, key_data_len,
                   id2_key, &key_data_len, 0);
    if (ret != IROT_SUCCESS)
    {
        id2_log_error("%s: id2 key decrypt fail, %d\n", __FUNC_NAME__, ret);
        goto _out;
    }
#endif

    //id2_log_hex_data("id2_key", id2_key, key_size);

    id2_id = irot_pal_memory_malloc(kcv_data_len);
    if (id2_id == NULL)
    {
        id2_log_error("%s: malloc %d fail\n", __FUNC_NAME__, kcv_data_len);
        ret = IROT_ERROR_OUT_OF_MEMORY;
        goto _out;
    }

#if (ID2_CRYPTO_TYPE_CONFIG == ID2_CRYPTO_TYPE_AES)
    ret = aes_ecb(id2_key, key_size,
                  otp_data + kcv_data_off, kcv_data_len,
                  id2_id, &kcv_data_len, 0);
    if (ret != IROT_SUCCESS)
    {
        id2_log_error("%s: id2 id decrypt fail, %d\n", __FUNC_NAME__, ret);
        goto _out;
    }
#elif (ID2_CRYPTO_TYPE_CONFIG == ID2_CRYPTO_TYPE_3DES)
    ret = des3_ecb(id2_key, key_size,
                   otp_data + kcv_data_off, kcv_data_len,
                   id2_id, &kcv_data_len, 0);
    if (ret != IROT_SUCCESS)
    {
        id2_log_error("%s: id2 id decrypt fail, %d\n", __FUNC_NAME__, ret);
        goto _out;
    }
#endif

    id2_log_hex_data("id2_id", id2_id, ID2_ID_LEN);

    if (!memcmp(id2_id, (uint8_t*)otp_data + 1, ID2_ID_LEN))
    {
        km_key_data_t key_struct;
        if ((key_type == KEY_TYPE_AES_128) || (key_type == KEY_TYPE_AES_192) || (key_type == KEY_TYPE_AES_256))
        {
            key_struct.type = KM_AES;
        }
        else if ((key_type == KEY_TYPE_3DES_112) || (key_type == KEY_TYPE_3DES_168))
        {
            key_struct.type = KM_DES3;
        }
        else
        {
            ret = IROT_ERROR_NOT_SUPPORTED;
            goto _out;
        }
        key_struct.sym_key.key_bit = (key_size << 3);
        key_struct.sym_key.key = id2_key;

        kmret = km_set_id2(id2_id, ID2_ID_LEN);
        if (kmret != KM_SUCCESS)
        {
            id2_log_error("%s: KM set id2 error, 0x%08X\n", __FUNC_NAME__, kmret);
            ret = IROT_ERROR_GENERIC;
            goto _out;
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
            goto _out;
        }
    }
    else
    {
        ret = IROT_ERROR_GENERIC;
    }

_out:
    if (id2_id)
    {
        irot_pal_memory_free(id2_id);
    }
    if (id2_key)
    {
        irot_pal_memory_free(id2_key);
    }

    return ret;
}

////////////////////////////////////////////////////////////////////////////////

irot_result_t id2_client_get_prov_stat(bool* is_prov)
{
    irot_result_t ret = IROT_SUCCESS;
    uint8_t id2[ID2_ID_LEN];
    uint32_t len = ID2_ID_LEN;
    uint32_t kmret;

    id2_log_debug("[%s] enter.\n", __FUNC_NAME__);

    if (!is_id2_client_inited())
    {
        id2_log_error("ERROR: [%s] id2 client not inited.\n", __FUNC_NAME__);
        ret = IROT_ERROR_GENERIC;
        goto EXIT;
    }

    if (is_prov == NULL)
    {
        id2_log_error("%s: invalid input arg\n", __FUNC_NAME__);
        ret = IROT_ERROR_BAD_PARAMETERS;
        goto EXIT;
    }

    kmret = km_get_id2(id2, &len);
    if (kmret == KM_ERR_ITEM_NOT_FOUND)
    {
        *is_prov = false;
        ret = IROT_SUCCESS;
        id2_log_debug("OTP prov state: False\n");
        goto EXIT;
    }
    else if (kmret != KM_SUCCESS)
    {
        id2_log_error("%s: KM get id2 fail, 0x%08X\n", __FUNC_NAME__, kmret);
        ret = IROT_ERROR_GENERIC;
        goto EXIT;
    }

    id2_log_debug("OTP prov state: True\n");
    *is_prov = true;

EXIT:
    id2_log_debug("[%s] exit.\n", __FUNC_NAME__);
    return ret;
}

enum
{
    DEVICE_TEST_FP_LEN  = 16,
};

irot_result_t id2_client_get_otp_auth_code(const uint8_t* token, uint32_t token_len,
        uint8_t* auth_code, uint32_t* len)
{
    irot_result_t ret = IROT_SUCCESS;
    uint32_t kmret;
    uint8_t use_type;
    uint8_t key_type;
    uint32_t key_size = 0;
    uint32_t block_size = 0;
    uint32_t rept_len = 0;
    uint32_t dev_fp_len = 0;
    uint8_t* dev_fp = NULL;
    uint8_t* prov_id = NULL;
    uint8_t* rept_data = NULL;
    uint32_t auth_ver = ID2_OTP_AUTH_CODE_VER;

    id2_log_debug("[%s] enter.\n", __FUNC_NAME__);

    if (!is_id2_client_inited())
    {
        id2_log_error("ERROR: [%s] id2 client not inited.\n", __FUNC_NAME__);
        ret = IROT_ERROR_GENERIC;
        goto EXIT;
    }

    if (token == NULL || token_len != 32 ||
            auth_code == NULL || len == NULL)
    {
        id2_log_error("%s: invalid input args\n", __FUNC_NAME__);
        ret = IROT_ERROR_BAD_PARAMETERS;
        goto EXIT;
    }

    /* magic */
    if (token[0] != 0x69)
    {
        id2_log_error("%s: invalid token magic: 0x%x\n", __FUNC_NAME__, token[0]);
        ret = IROT_ERROR_GENERIC;
        goto EXIT;
    }

    use_type = token[1] - '0';
    if (use_type != 0x01)
    {
        id2_log_error("%s: not support this use type: 0x%x\n", __FUNC_NAME__, use_type);
        ret = IROT_ERROR_NOT_SUPPORTED;
        goto EXIT;
    }

    key_type = token[2] - '0';
#if (ID2_CRYPTO_TYPE_CONFIG == ID2_CRYPTO_TYPE_AES)
    if (key_type == KEY_TYPE_AES_128 ||
            key_type == KEY_TYPE_AES_192 || key_type == KEY_TYPE_AES_256)
    {
        key_size = 16;
        block_size = AES_BLOCK_SIZE;
    }
#elif (ID2_CRYPTO_TYPE_CONFIG == ID2_CRYPTO_TYPE_3DES)
    if (key_type == KEY_TYPE_3DES_112 || key_type == KEY_TYPE_3DES_168)
    {
        key_size = 16;
        block_size = DES_BLOCK_SIZE;
    }
#endif
    else
    {
        id2_log_error("%s: not support this key type: 0x%x\n", __FUNC_NAME__, key_type);
        ret = IROT_ERROR_NOT_SUPPORTED;
        goto EXIT;
    }

    prov_id = get_otp_prov_id((uint8_t*)token + 3, key_size);
    if (prov_id == NULL)
    {
        id2_log_error("%s: get prov id fail\n", __FUNC_NAME__);
        ret = IROT_ERROR_GENERIC;
        goto EXIT;
    }

    id2_log_hex_data("prov_id", prov_id, 32);

#if ID2_OTP_LOCAL_TEST
    dev_fp_len = DEVICE_TEST_FP_LEN;
#else
    dev_fp_len = 0x100;
#endif
    dev_fp = irot_pal_memory_malloc(dev_fp_len);
    if (dev_fp == NULL)
    {
        id2_log_error("%s: malloc %d fail\n", __FUNC_NAME__, dev_fp_len);
        ret = IROT_ERROR_OUT_OF_MEMORY;
        goto EXIT;
    }
#if ID2_OTP_LOCAL_TEST
    kmret = KM_SUCCESS;
    memset(dev_fp, '1', DEVICE_TEST_FP_LEN);
#else
    kmret = km_get_attestation(dev_fp, &dev_fp_len);
#endif
    if (kmret != KM_SUCCESS)
    {
        id2_log_error("%s: KM km_get_attestation error, 0x%08X\n", __FUNC_NAME__, kmret);
        ret = IROT_ERROR_GENERIC;
        goto EXIT;
    }
    id2_log_hex_data("attestation", dev_fp, dev_fp_len);

    /* one cipher block size and sha256 hash size */
    rept_len = block_size + 32;
    rept_len += 1 + ID2_OTP_SS_KEY_LEN + dev_fp_len + block_size;
    rept_data = irot_pal_memory_malloc(rept_len);
    if (rept_data == NULL)
    {
        id2_log_error("%s: malloc %d fail\n", __FUNC_NAME__, rept_len);
        ret = IROT_ERROR_OUT_OF_MEMORY;
        goto EXIT;
    }

    ret = otp_get_rept_data(key_type,
                            (uint8_t*)token + 3, key_size,
                            dev_fp, dev_fp_len, rept_data, &rept_len);
    if (ret != IROT_SUCCESS)
    {
        id2_log_error("%s: get rept data fail, %d\n", __FUNC_NAME__, ret);
        goto EXIT;
    }

    if (*len < 4 + 32 + rept_len)
    {
        id2_log_error("%s, auth code short buffer: %d\n", __FUNC_NAME__, *len);
        *len = 4 + 32 + rept_len;
        ret = IROT_ERROR_SHORT_BUFFER;
        goto EXIT;
    }
    else
    {
        *len = 4 + 32 + rept_len;
    }

    memcpy(auth_code, &auth_ver, 4);
    memcpy(auth_code + 4, prov_id, 32);
    memcpy(auth_code + 4 + 32 , rept_data, rept_len);

    id2_log_hex_data("otp_auth_code", auth_code, *len);

EXIT:
    if (prov_id)
    {
        irot_pal_memory_free(prov_id);
    }
    if (dev_fp)
    {
        irot_pal_memory_free(dev_fp);
    }
    if (rept_data)
    {
        irot_pal_memory_free(rept_data);
    }

    id2_log_debug("[%s] exit.\n", __FUNC_NAME__);

    return ret;
}

irot_result_t id2_client_load_otp_data(const uint8_t* otp_data, uint32_t len)
{
    irot_result_t ret;

    id2_log_debug("[%s] enter.\n", __FUNC_NAME__);

    if (!is_id2_client_inited())
    {
        id2_log_error("ERROR: [%s] id2 client not inited.\n", __FUNC_NAME__);
        ret = IROT_ERROR_GENERIC;
        goto EXIT;
    }

    ret = otp_set_otp_data((uint8_t*)otp_data, len);
    if (ret != IROT_SUCCESS)
    {
        id2_log_error("%s: set otp data fail, %d\n", __FUNC_NAME__, ret);
        goto EXIT;
    }
    id2_reset_cache();

EXIT:
    id2_log_debug("[%s] exit.\n", __FUNC_NAME__);
    return ret;
}

#endif /* ID2_OTP_SUPPORTED */

