/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "id2_config.h"
#include "id2_plat.h"
#include "id2_priv.h"
#include "id2_client.h"
#include "ali_crypto.h"
#include "km.h"

extern int is_id2_client_inited(void);

#if defined(CONFIG_ID2_OTP)

enum
{
    KEY_TYPE_AES_128    = 1,
    KEY_TYPE_AES_192    = 2,
    KEY_TYPE_AES_256    = 3,
    KEY_TYPE_3DES_112   = 4,
    KEY_TYPE_3DES_168   = 5,
};

#define ID2_OTP_AUTH_CODE_VER       0x00010000

#define ID2_OTP_SS_KEY_LEN           16
#define ID2_OTP_DEV_FP_MAX_LEN       128
#define ID2_OTP_AUTH_CODE_MAX_LEN    256

#define ID2_ROUNDUP(a, b)       (((a) + ((b) - 1)) & ~((b) - 1))

static uint8_t g_ss_key[ID2_OTP_SS_KEY_LEN] = {0};

#if (CONFIG_ID2_KEY_TYPE == ID2_KEY_TYPE_AES)
static int _id2_aes_ecb(const uint8_t* key, uint32_t key_len,
                const uint8_t* in, uint32_t in_len,
                uint8_t* out, uint32_t* out_len, bool is_enc)
{
    int ret = 0;
    size_t ctx_size;
    void* aes_ctx = NULL;
    ali_crypto_result result;

    result = ali_aes_get_ctx_size(AES_ECB, &ctx_size);
    if (result != ALI_CRYPTO_SUCCESS) {
        return -1;
    }

    aes_ctx = ls_osa_malloc(ctx_size);
    if (aes_ctx == NULL) {
        id2_log_error("out of mem, %d\n", (int)ctx_size);
        ret = -1;
        goto _out;
    }

    result = ali_aes_init(AES_ECB, is_enc, key, NULL, key_len, NULL, aes_ctx);
    if (result != ALI_CRYPTO_SUCCESS) {
        id2_log_error("aes init fail, 0x%x\n", result);
        ret = -1;
        goto _out;
    }

    result = ali_aes_process(in, out, in_len, aes_ctx);
    if (result != ALI_CRYPTO_SUCCESS) {
        id2_log_error("aes process fail, 0x%x\n", result);
        ret = -1;
        goto _out;
    }

    result = ali_aes_finish(NULL, 0, NULL, NULL, SYM_NOPAD, aes_ctx);
    if (result != ALI_CRYPTO_SUCCESS) {
        id2_log_error("aes finish fail, 0x%x\n", result);
        ret = -1;
        goto _out;
    }

    *out_len = in_len;

_out:
    ls_osa_free(aes_ctx);

    return ret;
}

#else

static int _id2_des3_ecb(const uint8_t* key, uint32_t key_len,
                const uint8_t* in, uint32_t in_len,
                uint8_t* out, uint32_t* out_len, bool is_enc)
{
    id2_log_debug("not support id2 3des key type\n");

    return -1;
}
#endif

static uint8_t* _get_otp_prov_id(uint8_t* prov_key, uint32_t key_size)
{
    irot_result_t ret;
    uint8_t* prov_id = NULL;
    uint32_t prov_id_len;

    if (prov_key == NULL || key_size == 0) {
        id2_log_error("invalid input args\n");
        return NULL;
    }

    prov_id_len = 32;
    prov_id = ls_osa_malloc(prov_id_len);
    if (prov_id == NULL) {
        id2_log_error("malloc(%d) fail\n", prov_id_len);
        return NULL;
    }

    ret = id2_plat_hash_sum(prov_key, key_size,
                   prov_id, &prov_id_len, DIGEST_TYPE_SHA256);
    if (ret != IROT_SUCCESS) {
        id2_log_error("hal hash sum fail\n", ret);
        ls_osa_free(prov_id);
        return NULL;
    }

    return prov_id;
}

static int _otp_get_rept_data(uint8_t key_type,
                              uint8_t* key_data, uint32_t key_len,
                              uint8_t* dev_fp, uint32_t dev_fp_len,
                              uint8_t* rept_data, uint32_t* rept_len)
{
    int ret = 0;
    uint32_t offset;
    uint32_t padding;
    uint32_t hash_len;
    uint32_t data_len;
    uint32_t total_len;
    uint32_t block_size = 0;
    uint8_t* prov_key = NULL;

    if (key_data == NULL || key_len == 0 ||
        dev_fp == NULL || dev_fp_len == 0 ||
        rept_data == NULL || rept_len == NULL) {
        id2_log_error("invalid input args\n");
        return -1;
    }

#if (CONFIG_ID2_KEY_TYPE == ID2_KEY_TYPE_AES)
    if (key_type == KEY_TYPE_AES_128 ||
        key_type == KEY_TYPE_AES_192 || key_type == KEY_TYPE_AES_256) {
        block_size = AES_BLOCK_SIZE;
    }
#elif (CONFIG_ID2_KEY_TYPE == ID2_KEY_TYPE_3DES)
    if (key_type == KEY_TYPE_3DES_112 || key_type == KEY_TYPE_3DES_168) {
        block_size = DES_BLOCK_SIZE;
    }
#endif

    if (block_size == 0) {
        id2_log_error("not support this key type: %d\n", key_type);
        return -1;
    }

    prov_key = key_data;
    id2_log_hex_dump("prov_key", prov_key, key_len);

    /* generate session key */
    id2_plat_get_random(g_ss_key, ID2_OTP_SS_KEY_LEN);

    /* key_type + ss_key + dev_fp */
    data_len = 1 + ID2_OTP_SS_KEY_LEN + dev_fp_len;
    padding = block_size - data_len % block_size;

    total_len = SHA256_HASH_SIZE + data_len + padding;
    if (*rept_len < total_len) {
        id2_log_error("rept data short buffer, %d\n", *rept_len);
        *rept_len = total_len;
        return -1;
    } else {
        *rept_len = total_len;
    }

    offset = SHA256_HASH_SIZE;
    memcpy(rept_data + offset, &key_type, 1);
    offset += 1;
    memcpy(rept_data + offset, g_ss_key, ID2_OTP_SS_KEY_LEN);
    offset += ID2_OTP_SS_KEY_LEN;
    memcpy(rept_data + offset, dev_fp, dev_fp_len);
    offset += dev_fp_len;
    memset(rept_data + offset, padding, padding);

    id2_log_hex_dump("rept_data", rept_data + SHA256_HASH_SIZE, data_len);

    hash_len = SHA256_HASH_SIZE;
    ret = id2_plat_hash_sum(
              rept_data + SHA256_HASH_SIZE, data_len,
              rept_data, &hash_len, DIGEST_TYPE_SHA256);
    if (ret < 0) {
        id2_log_error("hash sum fail\n");
        return -1;
    }

#if (CONFIG_ID2_KEY_TYPE == ID2_KEY_TYPE_AES)
    data_len += padding;
    ret = _id2_aes_ecb(prov_key, key_len,
               rept_data + SHA256_HASH_SIZE, data_len,
               rept_data + SHA256_HASH_SIZE, &data_len, true);
    if (ret < 0) {
        id2_log_error("rept data encrypt fail\n");
        return -1;
    }
#elif (CONFIG_ID2_KEY_TYPE == ID2_KEY_TYPE_3DES)
    data_len += padding;
    ret = _id2_des3_ecb(prov_key, key_len,
               rept_data + SHA256_HASH_SIZE, data_len,
               rept_data + SHA256_HASH_SIZE, &data_len, true);
    if (ret < 0) {
        id2_log_error("rept data encrypt fail\n");
        return -1;
    }
#endif

    return 0;
}

static int _otp_set_otp_data(uint8_t* otp_data, uint32_t len)
{
    int ret = 0;
    uint32_t key_type;
    uint32_t key_size = 0;
    uint32_t key_data_len = 0;
    uint32_t kcv_data_len = 0;
    uint32_t otp_data_len = 0;
    uint32_t key_data_off = 0;
    uint32_t kcv_data_off = 0;
    uint8_t* id2_id = NULL;
    uint8_t* id2_key = NULL;
    uint32_t km_ret;

    if (otp_data == NULL || len == 0) {
        id2_log_error("invalid input args\n");
        return -1;
    }

    id2_log_hex_dump("otp_data", (uint8_t*)otp_data, len);

    key_type = otp_data[0];

#if (CONFIG_ID2_KEY_TYPE == ID2_KEY_TYPE_AES)
    if (key_type == KEY_TYPE_AES_128) {
        key_size = 16;
    } else if (key_type == KEY_TYPE_AES_192) {
        key_size = 24;
    } else if (key_type == KEY_TYPE_AES_256) {
        key_size = 32;
    }

    key_data_len = ID2_ROUNDUP(key_size + 1, AES_BLOCK_SIZE);
    kcv_data_len = ID2_ROUNDUP(ID2_ID_LEN + 1, AES_BLOCK_SIZE);
#elif (CONFIG_ID2_KEY_TYPE == ID2_KEY_TYPE_3DES)
    if (key_type == KEY_TYPE_3DES_112) {
        key_size = 16;
    } else if (key_type == KEY_TYPE_3DES_168) {
        key_size = 24;
    }

    key_data_len = ID2_ROUNDUP(key_size + 1, DES_BLOCK_SIZE);
    kcv_data_len = ID2_ROUNDUP(ID2_ID_LEN + 1, DES_BLOCK_SIZE);
#else
#error("OTP not supported for RSA");
#endif

    otp_data_len = 1 + ID2_ID_LEN + key_data_len + kcv_data_len;
    if (len != otp_data_len) {
        id2_log_error("otp len is not match: %d %d\n", len, otp_data_len);
        return IROT_ERROR_GENERIC;
    }

    key_data_off = 1 + ID2_ID_LEN;
    kcv_data_off = 1 + ID2_ID_LEN + key_data_len;

    id2_key = ls_osa_malloc(key_data_len);
    if (id2_key == NULL) {
        id2_log_error("malloc %d fail\n", key_data_len);
        return -1;
    }

#if (CONFIG_ID2_KEY_TYPE == ID2_KEY_TYPE_AES)
    ret = _id2_aes_ecb(g_ss_key, ID2_OTP_SS_KEY_LEN,
               otp_data + key_data_off, key_data_len,
               id2_key, &key_data_len, false);
    if (ret < 0) {
        id2_log_error("id2 key decrypt fail\n");
        goto _out;
    }
#elif (CONFIG_ID2_KEY_TYPE == ID2_KEY_TYPE_3DES)
    ret = _id2_des3_ecb(g_ss_key, ID2_OTP_SS_KEY_LEN,
                   otp_data + key_data_off, key_data_len,
                   id2_key, &key_data_len, 0);
    if (ret < 0) {
        id2_log_error("id2 key decrypt fail\n");
        goto _out;
    }
#endif

    id2_id = ls_osa_malloc(kcv_data_len);
    if (id2_id == NULL) {
        id2_log_error("out of mem, %d\n", kcv_data_len);
        ret = -1;
        goto _out;
    }
    memset(id2_id, 0, kcv_data_len);

#if (CONFIG_ID2_KEY_TYPE == ID2_KEY_TYPE_AES)
    ret = _id2_aes_ecb(id2_key, key_size,
                  otp_data + kcv_data_off, kcv_data_len,
                  id2_id, &kcv_data_len, false);
    if (ret < 0) {
        id2_log_error("id2 id decrypt fail\n");
        goto _out;
    }
#elif (CONFIG_ID2_KEY_TYPE == ID2_KEY_TYPE_3DES)
    ret = _id2_des3_ecb(id2_key, key_size,
                   otp_data + kcv_data_off, kcv_data_len,
                   id2_id, &kcv_data_len, true);
    if (ret < 0) {
        id2_log_error("id2 id decrypt fail\n");
        goto _out;
    }
#endif

    id2_log_hex_dump("id2_id", id2_id, ID2_ID_LEN);

    if (!memcmp(id2_id, (uint8_t*)otp_data + 1, ID2_ID_LEN)) {
        km_key_data_t key_struct;
        if (key_type == KEY_TYPE_AES_128 ||
            key_type == KEY_TYPE_AES_192 || key_type == KEY_TYPE_AES_256) {
            key_struct.type = KM_AES;
        } else if (key_type == KEY_TYPE_3DES_112 || key_type == KEY_TYPE_3DES_168) {
            key_struct.type = KM_DES3;
        } else {
            id2_log_error("not support this key type, %d\n", key_type);
            ret = -1;
            goto _out;
        }

        key_struct.sym_key.key_bit = (key_size << 3);
        key_struct.sym_key.key = id2_key;

        km_ret = km_set_id2(id2_id, ID2_ID_LEN);
        if (km_ret != KM_SUCCESS) {
            id2_log_error("km set id2 error, 0x%08x\n", km_ret);
            ret = -1;
            goto _out;
        }

        km_delete_key(ID2_KEY_NAME, ID2_KEY_NAME_LEN);

        km_ret = km_import_key(ID2_KEY_NAME, ID2_KEY_NAME_LEN,
                               KM_KEY_FORMAT_RAW, &key_struct, sizeof(key_struct));
        if (km_ret != KM_SUCCESS) {
            id2_log_error("km import key error, 0x%08x\n", km_ret);
            ret = -1;
            goto _out;
        }
    } else {
        id2_log_error("id2 id is not correct\n");
        ret = -1;
    }

    ret = 0;

_out:
    if (id2_id) {
        ls_osa_free(id2_id);
    }
    if (id2_key) {
        ls_osa_free(id2_key);
    }

    return ret;
}

irot_result_t id2_client_get_prov_stat(bool* is_prov)
{
    uint8_t id2[ID2_ID_LEN];
    uint32_t len = ID2_ID_LEN;
    uint32_t km_ret;

    id2_log_debug("[id2_client_get_prov_stat enter.]\n");

    if (!is_id2_client_inited()) {
        id2_log_error("id2 not inited.\n");
        return IROT_ERROR_GENERIC;
    }

    if (is_prov == NULL) {
        id2_log_error("invalid input arg\n");
        return IROT_ERROR_BAD_PARAMETERS;
    }

    km_ret = km_get_id2(id2, &len);
    if (km_ret == KM_ERR_ITEM_NOT_FOUND) {
        *is_prov = false;
        goto _out;
    } else if (km_ret != KM_SUCCESS) {
        id2_log_error("km get id2 fail, 0x%08x\n", km_ret);
        return IROT_ERROR_GENERIC;
    }

    *is_prov = true;

_out:
    id2_log_debug("id2 prov state: %s\n", *is_prov == true ? "true" : "false");

    return IROT_SUCCESS;
}

irot_result_t id2_client_get_otp_auth_code(const uint8_t* token, uint32_t token_len,
        uint8_t* auth_code, uint32_t* len)
{
    irot_result_t ret = IROT_SUCCESS;
    uint32_t km_ret;
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

    id2_log_debug("[id2_client_get_otp_auth_code enter.]\n");

    if (!is_id2_client_inited()) {
        id2_log_error("id2 not inited.\n");
        ret = IROT_ERROR_GENERIC;
        goto _out;
    }

    if (token == NULL || token_len < 32 ||
        auth_code == NULL || len == NULL) {
        id2_log_error("invalid input args(token_len: %d)\n", token_len);
        ret = IROT_ERROR_BAD_PARAMETERS;
        goto _out;
    }

    /* magic */
    if (token[0] != 0x69) {
        id2_log_error("invalid token magic: 0x%x\n", token[0]);
        ret = IROT_ERROR_GENERIC;
        goto _out;
    }

    use_type = token[1] - '0';
    if (use_type != 0x01) {
        id2_log_error("not support this use type: 0x%x\n", use_type);
        ret = IROT_ERROR_NOT_SUPPORTED;
        goto _out;
    }

    key_type = token[2] - '0';
#if (CONFIG_ID2_KEY_TYPE == ID2_KEY_TYPE_AES)
    if (key_type == KEY_TYPE_AES_128 ||
        key_type == KEY_TYPE_AES_192 || key_type == KEY_TYPE_AES_256) {
        key_size = 16;
        block_size = AES_BLOCK_SIZE;
    }
#elif (CONFIG_ID2_KEY_TYPE == ID2_KEY_TYPE_3DES)
    if (key_type == KEY_TYPE_3DES_112 || key_type == KEY_TYPE_3DES_168) {
        key_size = 16;
        block_size = DES_BLOCK_SIZE;
    }
#endif
    else {
        id2_log_error("not support this key type: 0x%x\n", key_type);
        ret = IROT_ERROR_NOT_SUPPORTED;
        goto _out;
    }

    prov_id = _get_otp_prov_id((uint8_t*)token + 3, key_size);
    if (prov_id == NULL) {
        id2_log_error("get prov id fail\n");
        ret = IROT_ERROR_GENERIC;
        goto _out;
    }

    id2_log_hex_dump("prov_id", prov_id, 32);

    dev_fp_len = ID2_OTP_DEV_FP_MAX_LEN;
    dev_fp = ls_osa_malloc(dev_fp_len);
    if (dev_fp == NULL) {
        id2_log_error("malloc %d fail\n", dev_fp_len);
        ret = IROT_ERROR_OUT_OF_MEMORY;
        goto _out;
    }

    km_ret = km_get_attestation(dev_fp, &dev_fp_len);
    if (km_ret != KM_SUCCESS) {
        id2_log_error("km_get_attestation error, 0x%08x\n", km_ret);
        ret = IROT_ERROR_GENERIC;
        goto _out;
    }

    id2_log_hex_dump("attestation", dev_fp, dev_fp_len);

    /* block size + sha256 hash size */
    rept_len = block_size + 32;
    rept_len += 1 + ID2_OTP_SS_KEY_LEN + dev_fp_len + block_size;
    rept_data = ls_osa_malloc(rept_len);
    if (rept_data == NULL) {
        id2_log_error("malloc %d fail\n", rept_len);
        ret = IROT_ERROR_OUT_OF_MEMORY;
        goto _out;
    }

    ret = _otp_get_rept_data(key_type,
                   (uint8_t*)token + 3, key_size,
                   dev_fp, dev_fp_len, rept_data, &rept_len);
    if (ret != IROT_SUCCESS) {
        id2_log_error("get rept data fail, %d\n", ret);
        goto _out;
    }

    if (*len < 4 + 32 + rept_len) {
        id2_log_error("auth code short buffer: %d\n", *len);
        *len = 4 + 32 + rept_len;
        ret = IROT_ERROR_SHORT_BUFFER;
        goto _out;
    } else {
        *len = 4 + 32 + rept_len;
    }

    if (*len > ID2_OTP_AUTH_CODE_MAX_LEN) {
        id2_log_error("invalid id2 otp length, %d\n", *len);
        ret = IROT_ERROR_EXCESS_DATA;
        goto _out;
    }

    memcpy(auth_code, &auth_ver, 4);
    memcpy(auth_code + 4, prov_id, 32);
    memcpy(auth_code + 4 + 32 , rept_data, rept_len);

    id2_log_hex_dump("otp_auth_code", auth_code, *len);

_out:
    if (prov_id) {
        ls_osa_free(prov_id);
    }
    if (dev_fp) {
        ls_osa_free(dev_fp);
    }
    if (rept_data) {
        ls_osa_free(rept_data);
    }

    return ret;
}

irot_result_t id2_client_load_otp_data(const uint8_t* otp_data, uint32_t len)
{
    irot_result_t ret;

    id2_log_debug("[id2_client_load_otp_data enter.]\n");

    if (!is_id2_client_inited()) {
        id2_log_error("id2 not inited.\n");
        ret = IROT_ERROR_GENERIC;
        goto _out;
    }

    ret = _otp_set_otp_data((uint8_t*)otp_data, len);
    if (ret != IROT_SUCCESS) {
        id2_log_error("set otp data fail, %d\n", ret);
        goto _out;
    }

_out:
    return ret;
}

#else  /* CONFIG_ID2_OTP */

irot_result_t id2_client_get_prov_stat(bool* is_prov)
{
    uint8_t id2[ID2_ID_LEN];
    uint32_t len = ID2_ID_LEN;
    uint32_t km_ret;

    id2_log_debug("[id2_client_get_prov_stat enter.]\n");

    if (!is_id2_client_inited()) {
        id2_log_error("id2 not inited.\n");
        return IROT_ERROR_GENERIC;
    }

    if (is_prov == NULL) {
        id2_log_error("invalid input arg\n");
        return IROT_ERROR_BAD_PARAMETERS;
    }

    km_ret = km_get_id2(id2, &len);
    if (km_ret == KM_ERR_ITEM_NOT_FOUND) {
        *is_prov = false;
        goto _out;
    } else if (km_ret != KM_SUCCESS) {
        id2_log_error("km get id2 fail, 0x%08x\n", km_ret);
        return IROT_ERROR_GENERIC;
    }

    *is_prov = true;

_out:
    id2_log_debug("id2 prov state: %s\n", *is_prov == true ? "true" : "false");

    return IROT_SUCCESS;
}

irot_result_t id2_client_get_otp_auth_code(const uint8_t* token, uint32_t token_len,
        uint8_t* auth_code, uint32_t* len)
{
    (void)token;
    (void)token_len;
    (void)auth_code;
    (void)len;

    id2_log_info("not supported!!\n");

    return IROT_ERROR_NOT_SUPPORTED;
}

irot_result_t id2_client_load_otp_data(const uint8_t* otp_data, uint32_t len)
{
    (void)otp_data;
    (void)len;

    id2_log_info("not supported!!\n");

    return IROT_ERROR_NOT_SUPPORTED;
}

#endif  /* CONFIG_ID2_OTP */
