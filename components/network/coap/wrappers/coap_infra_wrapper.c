/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */
#include <assert.h>
#include <string.h>

#include "aos/kernel.h"
#include "coap_wrapper.h"
#include "cJSON.h"
#include "mbedtls/md5.h"
#include "mbedtls/aes.h"
#include "mbedtls/sha256.h"

typedef struct {
    mbedtls_aes_context ctx;
    uint8_t iv[16];
    uint8_t key[16];
} aes_context_t;

static void * mbedtls_aes128_init(
            const unsigned char *key,
            const unsigned char *iv,
            bool  is_encrypt)
{
    int ret = 0;
    aes_context_t *p_aes128 = NULL;

    if (!key || !iv) return p_aes128;

    p_aes128 = (aes_context_t *)aos_malloc(sizeof(aes_context_t));
    if (!p_aes128) return p_aes128;

    mbedtls_aes_init(&p_aes128->ctx);
    if (is_encrypt) {
        ret = mbedtls_aes_setkey_enc(&p_aes128->ctx, key, 128);
    } else {
        ret = mbedtls_aes_setkey_dec(&p_aes128->ctx, key, 128);
    }

    if (ret == 0) {
        memcpy(p_aes128->iv, iv, 16);
        memcpy(p_aes128->key, key, 16);
    } else {
        aos_free(p_aes128);
        p_aes128 = NULL;
    }

    return p_aes128;
}

static int mbedtls_aes128_destroy(void *aes)
{
    if (!aes) return -1;

    mbedtls_aes_free(&((aes_context_t *)aes)->ctx);
    aos_free(aes);
    return 0;
}

static int mbedtls_aes128_cbc_decrypt(
            void *aes,
            const void *src,
            unsigned int blockNum,
            void *dst)
{
    int i   = 0;
    int ret = -1;
    aes_context_t *p_aes128 = (aes_context_t *)aes;

    if (!aes || !src || !dst) return ret;

    for (i = 0; i < blockNum; ++i) {

        ret = mbedtls_aes_crypt_cbc(&p_aes128->ctx, MBEDTLS_AES_DECRYPT, 16,
                                    p_aes128->iv, src, dst);
        src = (unsigned char *)src + 16;
        dst = (unsigned char *)dst + 16;
    }

    return ret;
}

static int mbedtls_aes128_cbc_encrypt(
            void *aes,
            const void *src,
            size_t blockNum,
            void *dst)
{
    int i   = 0;
    int ret = ret;
    aes_context_t *p_aes128 = (aes_context_t *)aes;

    if (!aes || !src || !dst) return -1;

    for (i = 0; i < blockNum; ++i) {

        ret = mbedtls_aes_crypt_cbc(&p_aes128->ctx, MBEDTLS_AES_ENCRYPT, 16,
                                    p_aes128->iv, src, dst);
        src = (unsigned char *)src + 16;
        dst = (unsigned char *)dst + 16;
    }

    return ret;
}

void * coap_wrapper_aes128_init(
            const unsigned char *key,
            const unsigned char *iv,
            bool  is_encrypt)
{
    return mbedtls_aes128_init(key, iv, is_encrypt);
}

int coap_wrapper_aes128_destroy(void *aes)
{
    return mbedtls_aes128_destroy(aes);
}

int coap_wrapper_aes128_cbc_decrypt(
            void *aes,
            const void *src,
            unsigned int blockNum,
            void *dst)
{
    return mbedtls_aes128_cbc_decrypt(aes, src, blockNum, dst);
}

int coap_wrapper_aes128_cbc_encrypt(
            void *aes,
            const void *src,
            unsigned int blockNum,
            void *dst)
{
    return mbedtls_aes128_cbc_encrypt(aes, src, blockNum, dst);
}

void * coap_wrapper_cjson_parse(const char *src)
{
    return cJSON_Parse(src);
}

void * coap_wrapper_cjson_object_item(void *root, const char *key)
{
    return cJSON_GetObjectItem((cJSON*)root, key);
}

int coap_wrapper_cjson_value_int(void *node)
{
    cJSON* obj = (cJSON*) node;
    assert(obj != NULL);
    return obj->valueint;
}

char * coap_wrapper_cjson_value_string(void *node)
{
    cJSON* obj = (cJSON*) node;
    assert(obj != NULL);
    return obj->valuestring;
}

void coap_wrapper_cjson_release(void *root)
{
    cJSON_Delete(root);
}

static char utils_hb2hex(unsigned char hb)
{
    hb = hb & 0xF;
    return (char)(hb < 10 ? '0' + hb : hb - 10 + 'a');
}

#define MD5_KEY_IOPAD_SIZE  (64)
#define MD5_DIGEST_SIZE     (16)
static void mbedtls_hmac_md5(const char *msg, int msg_len, char *digest, const char *key, int key_len)
{
    mbedtls_md5_context context;
    unsigned char k_ipad[MD5_KEY_IOPAD_SIZE];    /* inner padding - key XORd with ipad  */
    unsigned char k_opad[MD5_KEY_IOPAD_SIZE];    /* outer padding - key XORd with opad */
    unsigned char out[MD5_DIGEST_SIZE];
    int i;

    if ((NULL == msg) || (NULL == digest) || (NULL == key)) {
        return;
    }

    if (key_len > MD5_KEY_IOPAD_SIZE) {
        return;
    }

    /* start out by storing key in pads */
    memset(k_ipad, 0, sizeof(k_ipad));
    memset(k_opad, 0, sizeof(k_opad));
    memcpy(k_ipad, key, key_len);
    memcpy(k_opad, key, key_len);

    /* XOR key with ipad and opad values */
    for (i = 0; i < MD5_KEY_IOPAD_SIZE; i++) {
        k_ipad[i] ^= 0x36;
        k_opad[i] ^= 0x5c;
    }

    /* perform inner MD5 */
    mbedtls_md5_init(&context);                                      /* init context for 1st pass */
    mbedtls_md5_starts(&context);                                    /* setup context for 1st pass */
    mbedtls_md5_update(&context, k_ipad, MD5_KEY_IOPAD_SIZE);            /* start with inner pad */
    mbedtls_md5_update(&context, (unsigned char *) msg, msg_len);    /* then text of datagram */
    mbedtls_md5_finish(&context, out);                               /* finish up 1st pass */

    /* perform outer MD5 */
    mbedtls_md5_init(&context);                              /* init context for 2nd pass */
    mbedtls_md5_starts(&context);                            /* setup context for 2nd pass */
    mbedtls_md5_update(&context, k_opad, MD5_KEY_IOPAD_SIZE);    /* start with outer pad */
    mbedtls_md5_update(&context, out, MD5_DIGEST_SIZE);      /* then results of 1st hash */
    mbedtls_md5_finish(&context, out);                       /* finish up 2nd pass */

    for (i = 0; i < MD5_DIGEST_SIZE; ++i) {
        digest[i * 2] = utils_hb2hex(out[i] >> 4);
        digest[i * 2 + 1] = utils_hb2hex(out[i]);
    }
}

void coap_wrapper_hmac_md5(const char *msg, int msg_len, char *digest, const char *key, int key_len)
{
    return mbedtls_hmac_md5(msg, msg_len, digest, key, key_len);
}

static void util_sha256(const unsigned char *input, unsigned int ilen, unsigned char output[32])
{
    mbedtls_sha256_context ctx;
    mbedtls_sha256_init(&ctx);
    mbedtls_sha256_starts(&ctx, 0);
    mbedtls_sha256_update(&ctx, input, ilen);
    mbedtls_sha256_finish(&ctx, output);
    mbedtls_sha256_free(&ctx);
}

void coap_wrapper_sha256(const unsigned char *input, unsigned int ilen, unsigned char output[32])
{
    return util_sha256(input, ilen, output);
}

#ifdef INFRA_REPORT
#include "linkkit/infra/infra_report.h"
void coap_wrapper_set_report_func(info_report_func_pt func)
{
    iotx_set_report_func(func);
}

int coap_wrapper_report_devinfo(void *pclient)
{
    return iotx_report_devinfo(pclient);
}

/* report Firmware version */
int coap_wrapper_report_firmware_version(void *pclient)
{
    return iotx_report_firmware_version(pclient);
}

/* report ModuleID */
int coap_wrapper_report_mid(void *pclient)
{
    return iotx_report_mid(pclient);
}
#endif /* INFRA_REPORT */
