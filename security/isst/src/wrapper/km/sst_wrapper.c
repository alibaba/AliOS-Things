/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "ls_osa.h"
#include "sst.h"
#include "sst_dbg.h"
#include "sst_osa.h"
#include "km.h"
#include "ali_crypto.h"

#define INIT_MAGIC(a) \
        do { \
            a[0] = 'a'; \
            a[1] = 'S'; \
            a[2] = 's'; \
            a[3] = 'T'; \
        } while(0);

#define CLEAN_MAGIC(a) \
        do { \
            a[0] = '0'; \
            a[1] = '0'; \
            a[2] = '0'; \
            a[3] = '0'; \
        } while(0);

#define IS_SST_MAGIC_VALID(_m)  (('a' == _m[0]) &&  \
                                ('S' == _m[1]) &&   \
                                ('s' == _m[2]) &&   \
                                ('T' == _m[3]))

#if CONFIG_SST_USE_ID2
#define SST_KEY_NAME        "id2_key"
#define SST_KEY_NAME_LEN    7
#else
#define SST_KEY_NAME        "ahBk"
#define SST_KEY_NAME_LEN    4
#endif

/* the head size=120 Bytes,include 16 Bytes obj_id in front of the data*/
#define SST_HEAD_REV    3
typedef struct _sst_head {
    uint8_t     magic[4];
    uint32_t    version;
    uint32_t    reserved[SST_HEAD_REV];
    uint32_t    type;
    uint32_t    key_size;
    uint8_t     sub_key[24];
    uint8_t     iv[16];         /*iv*/
    uint8_t     pt_hash[32];    /*plaintext data hash:sha256*/
    uint32_t    data_size;      /*data size*/
} sst_head;

//create and init sst head
static void *_sst_create(uint32_t in_size, uint32_t type)
{
    sst_head *p_sst_obj = NULL;
    uint8_t seed[8] = {0};
    uint32_t seed_len = 8;
    uint8_t rand_buf[16] = {0};
    uint32_t rand_len = 16;
    uint64_t time = 0;
    ali_crypto_result result = 0;

    p_sst_obj = ls_osa_malloc(sizeof(sst_head) + in_size);
    if (!p_sst_obj) {
        return NULL;
    }
    INIT_MAGIC(p_sst_obj->magic);
    p_sst_obj->version = SST_VERSION;
    p_sst_obj->type = type;
#if CONFIG_SST_USE_ID2
    sst_memset(p_sst_obj->reserved, 1, SST_HEAD_REV * sizeof(uint32_t));
#else
    sst_memset(p_sst_obj->reserved, 0, SST_HEAD_REV * sizeof(uint32_t));
#endif /* CONFIG_SST_USE_ID2 */

    time = sst_current_raw_time();
    sst_memcpy(seed, &time, sizeof(uint64_t));
    result = ali_seed(seed, seed_len);
    if (result != ALI_CRYPTO_SUCCESS) {
        ls_osa_free(p_sst_obj);
        return NULL;
    }
    result = ali_rand_gen(rand_buf, rand_len);
    if (result != ALI_CRYPTO_SUCCESS) {
        ls_osa_free(p_sst_obj);
        return NULL;
    }
    sst_memcpy(p_sst_obj->iv, rand_buf, sizeof(p_sst_obj->iv));
    sst_memset(p_sst_obj->pt_hash, 0, sizeof(p_sst_obj->pt_hash));
    sst_memset(p_sst_obj->sub_key, 0, sizeof(p_sst_obj->sub_key));

    p_sst_obj->data_size = in_size;
    p_sst_obj->key_size = 16;

    return (void *)p_sst_obj;
}

static uint32_t _sst_enc_data(void *p_sst, const uint8_t *data_in)
{
    sst_head *p_sst_obj = (sst_head *)p_sst;
    void *p_data = NULL;
    void *envelope_ctx = NULL;
    uint32_t protected_key_len = 16;
    ali_crypto_result result = 0;
    uint32_t data_len = 0;
    uint32_t ret  = SST_SUCCESS;

    if (!p_sst_obj || !data_in) {
        SST_ERR("the input param is error data_in[%p] !!\n", data_in);
        return SST_ERROR_BAD_PARAMETERS;
    }

    if (!IS_SST_MAGIC_VALID(p_sst_obj->magic)) {
        SST_ERR("the sst magic is error!\n");
        return SST_ERROR_BAD_PARAMETERS;
    }

    /* HASH Plaintext */
    data_len = p_sst_obj->data_size;
    result = ali_sha256_digest(data_in, data_len, p_sst_obj->pt_hash);
    if (result) {
        SST_ERR("hash plain text failed 0x%x\n", result);
        return SST_ERROR_GENERIC;
    }

    p_data = (uint8_t *)p_sst + sizeof(sst_head);
    /* generate subkey*/
    ret = km_envelope_begin(&envelope_ctx, SST_KEY_NAME, SST_KEY_NAME_LEN,
                            p_sst_obj->iv, sizeof(p_sst_obj->iv),
                            p_sst_obj->sub_key, &protected_key_len, KM_PURPOSE_ENCRYPT);
    if (ret) {
        ret = SST_ERROR_GENERIC;
        return ret;
    }

    /* enc data */
    ret = km_envelope_finish(envelope_ctx, (uint8_t *)data_in, data_len, p_data, &data_len);
    if (ret || data_len != p_sst_obj->data_size) {
        SST_ERR("envelope finish failed 0x%x\n", (unsigned int)ret);
        return SST_ERROR_GENERIC;
    }

    return ret;
}

static uint32_t _sst_dec_data(void *p_sst, uint8_t *data_out)
{
    sst_head *p_sst_obj = (sst_head *)p_sst;
    ali_crypto_result result = 0;
    void *envelope_ctx = NULL;
    uint32_t protected_key_len = 16;
    uint32_t ret = SST_SUCCESS;
    uint8_t hash[32];
    uint32_t data_len = 0;

    if (!p_sst_obj || !data_out) {
        SST_ERR("the input param is error data_in[%p]!!\n", data_out);
        return SST_ERROR_BAD_PARAMETERS;
    }

    if (!IS_SST_MAGIC_VALID(p_sst_obj->magic)) {
        SST_ERR("the sst magic is error!\n");
        return SST_ERROR_BAD_PARAMETERS;
    }

    /* decrypt get subkey*/
    if (p_sst_obj->key_size != 16) {
        SST_ERR("key size error key_size = %d\n", (unsigned int)p_sst_obj->key_size);
        return SST_ERROR_GENERIC;
    }

    ret = km_envelope_begin(&envelope_ctx, SST_KEY_NAME, SST_KEY_NAME_LEN,
                            p_sst_obj->iv, 16, p_sst_obj->sub_key,
                            &protected_key_len, KM_PURPOSE_DECRYPT);
    if (ret) {
        ret = SST_ERROR_GENERIC;
        return ret;
    }

    data_len = p_sst_obj->data_size;
    sst_memset(data_out, 0, data_len);
    /* dec data */
    ret = km_envelope_finish(envelope_ctx, (uint8_t *)p_sst + sizeof(sst_head), data_len, data_out, &data_len);
    if (data_len != p_sst_obj->data_size) {
        SST_ERR("the pt data len != ct data len\n");
        return SST_ERROR_GENERIC;
    }

    /*check hash*/
    result = ali_sha256_digest(data_out, (size_t)data_len, hash);
    if (result != ALI_CRYPTO_SUCCESS) {
        SST_ERR("hash digest failed(%08x)\n", result);
        return SST_ERROR_GENERIC;
    }

    if (sst_memcmp(p_sst_obj->pt_hash, hash, sizeof(p_sst_obj->pt_hash))) {
        SST_ERR("the data is corrupt!!\n");
        return SST_ERROR_GENERIC;
    }

    return ret;
}

//if p_sst is NULL, just to get len for rtos
uint32_t sst_imp_get_data_len(void *p_sst, uint32_t obj_len)
{
    sst_head *p_sst_obj = (sst_head *)p_sst;

    if (!p_sst_obj) {
        if (obj_len > sizeof(sst_head)) {
            return obj_len - sizeof(sst_head);
        }
        return 0;
    }
    if (!IS_SST_MAGIC_VALID(p_sst_obj->magic)) {
        SST_ERR("the sst magic is error!\n");
        return 0;
    }

    if (p_sst_obj->data_size != obj_len - sizeof(sst_head)) {
        SST_ERR("error length\n");
        return 0;
    }

    return p_sst_obj->data_size;
}

//out: obj_name
uint32_t sst_imp_set_obj_name(const char *item_name, char *obj_name)
{
    ali_crypto_result alicrypto_ret;
    uint8_t hash_dst[SHA256_HASH_SIZE] = { 0 };
    uint32_t i = 0;

    //item name : obj name
    alicrypto_ret = ali_sha256_digest((uint8_t *)item_name, strlen(item_name), hash_dst);
    //for hash(item_name)
    if (alicrypto_ret != ALI_CRYPTO_SUCCESS) {
        SST_ERR("alicrypto hash fail 0x%x.\n", alicrypto_ret);
        return SST_ERROR_GENERIC;
    } else {
        for (i = 0; i < SHA256_HASH_SIZE; i++) {
            ls_osa_snprintf(obj_name + 2 * i, SHA256_HASH_SIZE + SHA256_HASH_SIZE, "%02X", hash_dst[i]);
        }
    }

    obj_name[2 * SHA256_HASH_SIZE] = '\0';

    return SST_SUCCESS;
}

uint32_t sst_imp_init(void)
{
    uint32_t ret = SST_SUCCESS;

#if !CONFIG_SST_USE_ID2
    km_sym_gen_param param = {0};

    param.key_size = 128;
    ret = km_generate_key(SST_KEY_NAME, SST_KEY_NAME_LEN, KM_AES, &param);
    if (KM_SUCCESS != ret) {
        if (KM_ERR_ACCESS_CONFLICT == ret) {
            return SST_SUCCESS;
        }
        return SST_ERROR_GENERIC;
    }
#endif /* !CONFIG_SST_USE_ID2 */

    return ret;
}

uint32_t sst_imp_hash_data(uint8_t *data_in, uint32_t size, uint8_t *data_out)
{
    ali_crypto_result result = 0;
    uint32_t ret = SST_SUCCESS;

    if (!data_in || !data_out) {
        return SST_ERROR_BAD_PARAMETERS;
    }

    result = ali_sha256_digest(data_in, size, data_out);
    if (result != ALI_CRYPTO_SUCCESS) {
        SST_ERR("hash fail(%08x)\n", result);
        return SST_ERROR_GENERIC;
    }

    return ret;
}

uint32_t sst_imp_create_obj(const uint8_t *data, uint32_t data_len, uint32_t type,
                            void **p_sst, uint32_t *obj_len)
{
    uint32_t ret = 0;

    if (!p_sst) {
        SST_ERR("bad parameters\n");
        return SST_ERROR_BAD_PARAMETERS;
    }

    *p_sst = _sst_create(data_len, type);
    if (NULL == *p_sst) {
        SST_ERR("create sst obj error!\n");
        return SST_ERROR_GENERIC;
    }

    ret = _sst_enc_data(*p_sst, data);
    if (SST_SUCCESS != ret) {
        SST_ERR("enc sst data error!\n");
        return ret;
    }

    *obj_len = sizeof(sst_head) +
               ((sst_head *)(*p_sst))->data_size;

    return SST_SUCCESS;
}

uint32_t sst_imp_get_obj_len(uint32_t data_len)
{
    return data_len + sizeof(sst_head);
}

uint32_t sst_imp_get_obj_data(void *p_sst, uint32_t obj_len, uint8_t *data, uint32_t *data_len, uint32_t *type)
{
    sst_head *p_sst_obj = (sst_head *)p_sst;
    uint32_t ret = 0;

    if (!p_sst_obj) {
        SST_ERR("sst obj null!\n");
        return SST_ERROR_GENERIC;
    }

    if (!IS_SST_MAGIC_VALID(p_sst_obj->magic)) {
        SST_ERR("the sst magic is error!\n");
        return SST_ERROR_GENERIC;
    }

    if (*data_len < p_sst_obj->data_size) {
        if (*data_len != 0)
            SST_ERR("short data buffer\n");
        *data_len = p_sst_obj->data_size;
        return SST_ERROR_SHORT_BUFFER;
    }

    *data_len = p_sst_obj->data_size;
    if (obj_len != sizeof(sst_head) + p_sst_obj->data_size) {
        SST_ERR("the sst size error!\n");
        return SST_ERROR_GENERIC;
    }

    *type = p_sst_obj->type;
    if (*type == SST_TYPE_NONE) {
        SST_ERR("get sst obj type error!\n");
        return SST_ERROR_GENERIC;
    }

    ret = _sst_dec_data(p_sst, data);
    if (SST_SUCCESS != ret) {
        SST_ERR("dec sst file error!\n");
        return ret;
    }

    return SST_SUCCESS;
}

void sst_imp_destroy_obj(void *p_sst)
{
    if (!p_sst) {
        return;
    }

    sst_memset(p_sst, 0, sizeof(sst_head));
    ls_osa_free(p_sst);
}

#if CONFIG_SST_MIGRATION || CONFIG_DATA_MIGRATION
uint32_t sst_imp_enc_mig_data(uint32_t type,
                              uint8_t *data_in,
                              uint32_t data_size,
                              uint8_t *key,
                              uint8_t *data_out)
{
    sst_mig_head *p_sst_mig_head = NULL;
    uint8_t *p_data = NULL;
    uint8_t seed[8];
    size_t seed_len = 8;
    uint8_t iv[16] = {0};

    ali_crypto_result result = 0;
    uint8_t hash[MAX_HASH_SIZE] = {0};

    size_t ctx_size = 0;
    void *ctx = NULL;
    uint32_t tmp_len = 16;
    uint32_t ret  = SST_SUCCESS;
    uint64_t time = 0;

    if (!data_out || !data_in || !key) {
        SST_ERR("the input param is error !!\n");
        return SST_ERROR_BAD_PARAMETERS;
    }

    p_sst_mig_head = (sst_mig_head *)data_out;
    INIT_MIG_MAGIC(p_sst_mig_head->magic);
    p_sst_mig_head->version = SST_VERSION;
    p_sst_mig_head->type = type;
    p_sst_mig_head->data_size = data_size;
    p_data = data_out + sizeof(sst_mig_head);
    /* generate subkey*/
    time = sst_current_raw_time();
    sst_memcpy(seed, &time, sizeof(uint64_t));
    result = ali_seed(seed, seed_len);
    if (result != ALI_CRYPTO_SUCCESS) {
        ret = SST_ERROR_GENERIC;
        goto _err;
    }
    result = ali_rand_gen(key, SST_MIGRATION_KEY_LEN);
    if (result != ALI_CRYPTO_SUCCESS) {
        ret = SST_ERROR_GENERIC;
        goto _err;
    }

    /* HASH Plaintext and fill pt_hash */
    result = ali_sha256_digest(data_in, data_size, hash);
    if (result != ALI_CRYPTO_SUCCESS) {
        SST_ERR("hash fail(%08x)\n", result);
        return SST_ERROR_GENERIC;
    }
    sst_memcpy(p_sst_mig_head->pt_hash, hash, sizeof(p_sst_mig_head->pt_hash));

    /* enc data */
    result = ali_aes_get_ctx_size(AES_CTR, &ctx_size);
    if (result) {
        SST_ERR("get ctx fail(%08x)\n", result);
        ret = SST_ERROR_GENERIC;
        goto _err;

    }
    ctx = ls_osa_malloc(ctx_size);
    if (!ctx) {
        SST_ERR("malloc ctx fail\n");
        ret = SST_ERROR_OUT_OF_MEMORY;
        goto _err;
    }
    sst_memset(ctx, 0, ctx_size);

    result = ali_aes_init(AES_CTR, true, key, NULL,
                          SST_MIGRATION_KEY_LEN, iv, ctx);
    if (result) {
        SST_ERR("aes init fail\n");
        ret = SST_ERROR_GENERIC;
        goto _err;
    }
    tmp_len = data_size;

    result = ali_aes_finish(data_in, data_size, p_data, (size_t *)&tmp_len, SYM_NOPAD, ctx);
    if (result) {
        SST_ERR("aes finish fail\n");
        ret = SST_ERROR_GENERIC;
        goto _err;
    }

_err:
    if (ctx) {
        ls_osa_free(ctx);
    }

    return ret;
}

uint32_t sst_imp_dec_mig_data(uint32_t *type,
                              void *p_sst_mig,
                              uint8_t *key,
                              uint8_t **p_data_out,
                              uint32_t *data_size)
{
    sst_mig_head *p_sst_mig_head = (sst_mig_head *)p_sst_mig;
    uint8_t iv[16];
    uint8_t *ct_data = NULL;
    uint8_t *pt_data = NULL;

    ali_crypto_result result = 0;
    uint8_t hash[MAX_HASH_SIZE];

    size_t ctx_size = 0;
    void *ctx = NULL;
    uint32_t tmp_len = 0;
    uint32_t ret = SST_SUCCESS;

    sst_memset(iv, 0, 16);
    sst_memset(hash, 0, MAX_HASH_SIZE);

    if (!p_sst_mig || !p_data_out) {
        SST_ERR("the input param is error data_in!!\n");
        return SST_ERROR_BAD_PARAMETERS;
    }

    if (!IS_SST_MIG_MAGIC_VALID(p_sst_mig_head->magic)) {
        SST_ERR("the sst magic is error!\n");
        return SST_ERROR_BAD_PARAMETERS;
    }

    pt_data = ls_osa_malloc(p_sst_mig_head->data_size);
    if (!pt_data) {
        return SST_ERROR_OUT_OF_MEMORY;
    }
    sst_memset(pt_data, 0, p_sst_mig_head->data_size);
    *p_data_out = pt_data;

    /* dec data */
    result = ali_aes_get_ctx_size(AES_CTR, &ctx_size);
    if (result) {
        SST_ERR("get ctx fail(%08x)\n", result);
        return SST_ERROR_GENERIC;
    }
    ctx = ls_osa_malloc(ctx_size);
    if (!ctx) {
        SST_ERR("malloc ctx fail\n");
        return SST_ERROR_OUT_OF_MEMORY;
    }
    sst_memset(ctx, 0, ctx_size);

    result = ali_aes_init(AES_CTR, false, key, NULL,
                          16, iv, ctx);
    if (result) {
        SST_ERR("aes init fail\n");
        ret = SST_ERROR_GENERIC;
        goto _err;
    }
    tmp_len = p_sst_mig_head->data_size;
    ct_data = (uint8_t *)((uint8_t *)p_sst_mig + sizeof(sst_mig_head));

    result = ali_aes_finish(ct_data, p_sst_mig_head->data_size,
                            pt_data, (size_t *)&tmp_len, SYM_NOPAD, ctx);
    if (result) {
        SST_ERR("aes finish fail\n");
        ret = SST_ERROR_GENERIC;
        goto _err;
    }

    if (tmp_len != p_sst_mig_head->data_size) {
        SST_ERR("the pt data len != ct data len\n");
        ret = SST_ERROR_GENERIC;
        goto _err;
    }
    /*check hash*/
    result = ali_sha256_digest(pt_data, tmp_len, hash);
    if (sst_memcmp(p_sst_mig_head->pt_hash, hash, sizeof(p_sst_mig_head->pt_hash))) {
        SST_ERR("the data is corrupt!!\n");
        ret = SST_ERROR_GENERIC;
        goto _err;
    }

    *data_size = p_sst_mig_head->data_size;
    *type = p_sst_mig_head->type;

_err:
    if (ctx) {
        ls_osa_free(ctx);
        ctx = NULL;
    }

    return ret;
}
#endif

