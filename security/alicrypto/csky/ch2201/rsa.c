/**
 * Copyright (C) 2017 The YunOS Project. All rights reserved.
 */

#include "mbed_crypto.h"
#include "ali_crypto.h"
#include "drv_tee.h"
#include "mbedtls/platform.h"

#define TEE_MIN_RSA_KEY_SIZE (256)
#define TEE_MAX_RSA_KEY_SIZE (2048)

typedef struct _ali_rsa_keypair_t
{
    uint32_t magic;
    size_t   n_size;
    size_t   e_size;
    size_t   d_size;
    size_t   p_size;
    size_t   q_size;
    size_t   qp_size;
    size_t   dp_size;
    size_t   dq_size;
    uint8_t  n[(TEE_MAX_RSA_KEY_SIZE >> 3)];
    uint8_t  e[(TEE_MAX_RSA_KEY_SIZE >> 3)];
    uint8_t  d[(TEE_MAX_RSA_KEY_SIZE >> 3)];
    uint8_t  p[(TEE_MAX_RSA_KEY_SIZE >> 4)];
    uint8_t  q[(TEE_MAX_RSA_KEY_SIZE >> 4)];
    uint8_t  qp[(TEE_MAX_RSA_KEY_SIZE >> 4)];
    uint8_t  dp[(TEE_MAX_RSA_KEY_SIZE >> 4)];
    uint8_t  dq[(TEE_MAX_RSA_KEY_SIZE >> 4)];
} ali_rsa_keypair_t;

typedef struct _ali_rsa_pubkey_t
{
    uint32_t magic;
    size_t   n_size;
    size_t   e_size;
    uint8_t  n[(TEE_MAX_RSA_KEY_SIZE >> 3)];
    uint8_t  e[(TEE_MAX_RSA_KEY_SIZE >> 3)];
} ali_rsa_pubkey_t;

static int myrand(void *rng_state, unsigned char *output, size_t len)
{
    int result;

    if (rng_state != NULL) {
        ali_seed(rng_state, OSA_strlen(rng_state));
    }

    result = ali_rand_gen(output, len);
    if (result != ALI_CRYPTO_SUCCESS) {
        PRINT_RET(-1, "gen rand fail(%08x)\n", result);
    }

    return 0;
}

/* mbed rsa_key -> tee rsa_key */
static ali_crypto_result _ali_rsa_key_encode(uint32_t type, void *in_key,
                                             void *out_key)
{
    mbedtls_rsa_context *key;
    int32_t              ret;

    if (in_key == NULL || out_key == NULL) {
        return ALI_CRYPTO_INVALID_ARG;
    }

    key = (mbedtls_rsa_context *)in_key;
    if (type == PK_PUBLIC) {
        /* FIXME(not use now in this branch) */
#if 0
        size_t len;
        ali_rsa_pubkey_t *rsa_key = (ali_rsa_pubkey_t *)out_key;
        OSA_memset(rsa_key, 0, sizeof(ali_rsa_pubkey_t));

        len = mbedtls_mpi_size(&key->N);
        /* TODO change the endien mode */
        ret = mbedtls_mpi_write_binary(&key->N, (unsigned char *)rsa_key->n, len);
        if (0 != ret) {
            return ALI_CRYPTO_INVALID_KEY;
        }
        rsa_key->n_size = len;

        len = mbedtls_mpi_size(&key->E);
        ret = mbedtls_mpi_write_binary(&key->E, (unsigned char *)rsa_key->e, len);
        if (0 != ret) {
            return ALI_CRYPTO_INVALID_KEY;
        }
        rsa_key->e_size = len;
#endif
    } else if (type == PK_PRIVATE) {
        size_t             len;
        ali_rsa_keypair_t *rsa_key = (ali_rsa_keypair_t *)out_key;
        OSA_memset(rsa_key, 0, sizeof(ali_rsa_keypair_t));

        len = mbedtls_mpi_size(&key->N);
        /* TODO change the endien mode */
        ret =
          mbedtls_mpi_write_binary(&key->N, (unsigned char *)rsa_key->n, len);
        if (0 != ret) {
            return ALI_CRYPTO_INVALID_KEY;
        }
        rsa_key->n_size = len;

        len = mbedtls_mpi_size(&key->E);
        ret =
          mbedtls_mpi_write_binary(&key->E, (unsigned char *)rsa_key->e, len);
        if (0 != ret) {
            return ALI_CRYPTO_INVALID_KEY;
        }
        rsa_key->e_size = len;

        len = mbedtls_mpi_size(&key->D);
        ret =
          mbedtls_mpi_write_binary(&key->D, (unsigned char *)rsa_key->d, len);
        if (0 != ret) {
            return ALI_CRYPTO_INVALID_KEY;
        }
        rsa_key->d_size = len;

        len = mbedtls_mpi_size(&key->P);
        ret =
          mbedtls_mpi_write_binary(&key->P, (unsigned char *)rsa_key->p, len);
        if (0 != ret) {
            return ALI_CRYPTO_INVALID_KEY;
        }
        rsa_key->p_size = len;

        len = mbedtls_mpi_size(&key->Q);
        ret =
          mbedtls_mpi_write_binary(&key->Q, (unsigned char *)rsa_key->q, len);
        if (0 != ret) {
            return ALI_CRYPTO_INVALID_KEY;
        }
        rsa_key->q_size = len;

        len = mbedtls_mpi_size(&key->DP);
        ret =
          mbedtls_mpi_write_binary(&key->DP, (unsigned char *)rsa_key->dp, len);
        if (0 != ret) {
            return ALI_CRYPTO_INVALID_KEY;
        }
        rsa_key->dp_size = len;

        len = mbedtls_mpi_size(&key->DQ);
        ret =
          mbedtls_mpi_write_binary(&key->DQ, (unsigned char *)rsa_key->dq, len);
        if (0 != ret) {
            return ALI_CRYPTO_INVALID_KEY;
        }
        rsa_key->dq_size = len;

        len = mbedtls_mpi_size(&key->QP);
        ret = mbedtls_mpi_write_binary(&key->QP, (unsigned char *)rsa_key->qp,
                                       (size_t)len);
        if (0 != ret) {
            return ALI_CRYPTO_INVALID_KEY;
        }
        rsa_key->qp_size = len;
    } else {
        return ALI_CRYPTO_INVALID_TYPE;
    }

    return ALI_CRYPTO_SUCCESS;
}

/* tee rsa_key -> mbed rsa_key */
static ali_crypto_result _ali_rsa_key_decode(uint32_t type, void *in_key,
                                             void *out_key)
{
    int                  ret;
    mbedtls_rsa_context *key;

    if (in_key == NULL || out_key == NULL) {
        return ALI_CRYPTO_INVALID_ARG;
    }

    key = (mbedtls_rsa_context *)out_key;
    if (type == PK_PUBLIC) {
        ali_rsa_pubkey_t *rsa_key = (ali_rsa_pubkey_t *)in_key;

        ret = mbedtls_mpi_read_binary(&key->N, rsa_key->n, rsa_key->n_size);
        if (0 != ret) {
            return ALI_CRYPTO_INVALID_KEY;
        }
        ret = mbedtls_mpi_read_binary(&key->E, rsa_key->e, rsa_key->e_size);
        if (0 != ret) {
            return ALI_CRYPTO_INVALID_KEY;
        }
    } else if (type == PK_PRIVATE) {
        ali_rsa_keypair_t *rsa_key = (ali_rsa_keypair_t *)in_key;

        ret = mbedtls_mpi_read_binary(&key->N, rsa_key->n, rsa_key->n_size);
        if (0 != ret) {
            return ALI_CRYPTO_INVALID_KEY;
        }

        ret = mbedtls_mpi_read_binary(&key->E, rsa_key->e, rsa_key->e_size);
        if (0 != ret) {
            return ALI_CRYPTO_INVALID_KEY;
        }

        ret = mbedtls_mpi_read_binary(&key->D, rsa_key->d, rsa_key->d_size);
        if (0 != ret) {
            return ALI_CRYPTO_INVALID_KEY;
        }

        if ((rsa_key->p_size) && (rsa_key->q_size) && (rsa_key->qp_size) &&
            (rsa_key->dp_size) && (rsa_key->dq_size)) {
            ret = mbedtls_mpi_read_binary(&key->P, rsa_key->p, rsa_key->p_size);
            if (0 != ret) {
                return ALI_CRYPTO_INVALID_KEY;
            }
            ret = mbedtls_mpi_read_binary(&key->Q, rsa_key->q, rsa_key->q_size);
            if (0 != ret) {
                return ALI_CRYPTO_INVALID_KEY;
            }
            ret =
              mbedtls_mpi_read_binary(&key->DP, rsa_key->dp, rsa_key->dp_size);
            if (0 != ret) {
                return ALI_CRYPTO_INVALID_KEY;
            }
            ret =
              mbedtls_mpi_read_binary(&key->DQ, rsa_key->dq, rsa_key->dq_size);
            if (0 != ret) {
                return ALI_CRYPTO_INVALID_KEY;
            }
            ret =
              mbedtls_mpi_read_binary(&key->QP, rsa_key->qp, rsa_key->qp_size);
            if (0 != ret) {
                return ALI_CRYPTO_INVALID_KEY;
            }
        }
    } else {
        return ALI_CRYPTO_INVALID_TYPE;
    }

    return ALI_CRYPTO_SUCCESS;
}

static uint32_t _ali_get_hash_size(hash_type_t type)
{
    uint32_t hash_size;

    switch (type) {
        case SHA1:
            hash_size = SHA1_HASH_SIZE;
            break;
        case SHA224:
            hash_size = SHA224_HASH_SIZE;
            break;
        case SHA256:
            hash_size = SHA256_HASH_SIZE;
            break;
        case SHA384:
            hash_size = SHA384_HASH_SIZE;
            break;
        case SHA512:
            hash_size = SHA512_HASH_SIZE;
            break;
        case MD5:
            hash_size = MD5_HASH_SIZE;
            break;
        default:
            PRINT_RET(0, "get_hash_size: invalid type(%d)\n", type);
    }

    return hash_size;
}

static int32_t _ali_get_hash_type(hash_type_t type)
{
    mbedtls_md_type_t hash_type;

    switch (type) {
        case HASH_NONE:
            hash_type = MBEDTLS_MD_NONE;
            break;
        case SHA1:
            hash_type = MBEDTLS_MD_SHA1;
            break;
        case SHA224:
            hash_type = MBEDTLS_MD_SHA224;
            break;
        case SHA256:
            hash_type = MBEDTLS_MD_SHA256;
            break;
        case SHA384:
            hash_type = MBEDTLS_MD_SHA384;
            break;
        case SHA512:
            hash_type = MBEDTLS_MD_SHA512;
            break;
        case MD5:
            hash_type = MBEDTLS_MD_MD5;
            break;
        default:
            PRINT_RET(-1, "get_hash_type: invalid type(%d)\n", type);
    }

    return (int32_t)hash_type;
}

ali_crypto_result ali_rsa_get_keypair_size(size_t keybits, size_t *size)
{
    if (size == NULL) {
        return ALI_CRYPTO_INVALID_ARG;
    }

    if (keybits < TEE_MIN_RSA_KEY_SIZE || keybits > TEE_MAX_RSA_KEY_SIZE) {
        PRINT_RET(ALI_CRYPTO_INVALID_ARG,
                  "get_keypair_size: invalid keybits(%d)\n", (int)keybits);
    }

    *size = sizeof(ali_rsa_keypair_t);

    return ALI_CRYPTO_SUCCESS;
}

ali_crypto_result ali_rsa_get_pubkey_size(size_t keybits, size_t *size)
{
    if (size == NULL) {
        return ALI_CRYPTO_INVALID_ARG;
    }

    if (keybits < TEE_MIN_RSA_KEY_SIZE || keybits > TEE_MAX_RSA_KEY_SIZE) {
        PRINT_RET(ALI_CRYPTO_INVALID_ARG,
                  "get_pubkey_size: invalid keybits(%d)\n", (int)keybits);
    }

    *size = sizeof(ali_rsa_pubkey_t);

    return ALI_CRYPTO_SUCCESS;
}

ali_crypto_result ali_rsa_init_keypair(
  size_t keybits, const uint8_t *n, size_t n_size, const uint8_t *e,
  size_t e_size, const uint8_t *d, size_t d_size, const uint8_t *p,
  size_t p_size, const uint8_t *q, size_t q_size, const uint8_t *dp,
  size_t dp_size, const uint8_t *dq, size_t dq_size, const uint8_t *qp,
  size_t qp_size, rsa_keypair_t *keypair)
{
    ali_rsa_keypair_t *rsa_key;

    if (keypair == NULL || n == NULL || n_size == 0 || e == NULL ||
        e_size == 0 || d == NULL || d_size == 0) {
        PRINT_RET(ALI_CRYPTO_INVALID_ARG, "Init_keypair: invalid args!\n");
    }

    if ((n_size << 3) > keybits || (e_size << 3) > keybits ||
        (d_size << 3) > keybits || (p_size << 3) > (keybits >> 1) ||
        (q_size << 3) > (keybits >> 1) || (dp_size << 3) > (keybits >> 1) ||
        (dq_size << 3) > (keybits >> 1) || (qp_size << 3) > (keybits >> 1)) {
        PRINT_RET(ALI_CRYPTO_INVALID_ARG,
                  "Init_keypair: key parms size not match with key size\n");
    }

    rsa_key = (ali_rsa_keypair_t *)keypair;
    OSA_memset(rsa_key, 0, sizeof(ali_rsa_keypair_t));
    rsa_key->n_size = n_size;
    rsa_key->e_size = e_size;
    rsa_key->d_size = d_size;
    OSA_memcpy(rsa_key->n, n, n_size);
    OSA_memcpy(rsa_key->e, e, e_size);
    OSA_memcpy(rsa_key->d, d, d_size);

    if (p && p_size) {
        rsa_key->p_size = p_size;
        OSA_memcpy(rsa_key->p, p, p_size);
    }
    if (q && q_size) {
        rsa_key->q_size = q_size;
        OSA_memcpy(rsa_key->q, q, q_size);
    }
    if (dp && dp_size) {
        rsa_key->dp_size = dp_size;
        OSA_memcpy(rsa_key->dp, dp, dp_size);
    }
    if (dq && dq_size) {
        rsa_key->dq_size = dq_size;
        OSA_memcpy(rsa_key->dq, dq, dq_size);
    }
    if (qp && qp_size) {
        rsa_key->qp_size = qp_size;
        OSA_memcpy(rsa_key->qp, qp, qp_size);
    }

    INIT_CTX_MAGIC(rsa_key->magic);
    return ALI_CRYPTO_SUCCESS;
}

ali_crypto_result ali_rsa_init_pubkey(size_t keybits, const uint8_t *n,
                                      size_t n_size, const uint8_t *e,
                                      size_t e_size, rsa_pubkey_t *pubkey)
{
    ali_rsa_pubkey_t *pub_key;

    if (pubkey == NULL || n == NULL || n_size == 0 || e == NULL ||
        e_size == 0) {
        PRINT_RET(ALI_CRYPTO_INVALID_ARG, "Init_pubkey: invalid args!\n");
    }

    if ((n_size << 3) > keybits || (e_size << 3) > keybits) {
        PRINT_RET(ALI_CRYPTO_INVALID_ARG,
                  "Init_pubkey: key param size not match with key size\n");
    }

    pub_key         = (ali_rsa_pubkey_t *)pubkey;
    pub_key->n_size = n_size;
    pub_key->e_size = e_size;
    OSA_memcpy(pub_key->n, n, n_size);
    OSA_memcpy(pub_key->e, e, e_size);

    INIT_CTX_MAGIC(pub_key->magic);
    return ALI_CRYPTO_SUCCESS;
}

ali_crypto_result ali_rsa_gen_keypair(size_t keybits, const uint8_t *e,
                                      size_t e_size, rsa_keypair_t *keypair)
{
    int                 ret;
    int                 exponent;
    mbedtls_rsa_context ctx;
    ali_crypto_result   result;

    if (keypair == NULL) {
        return ALI_CRYPTO_INVALID_ARG;
    }

    if (e && e_size < 3) {
        uint32_t i;

        exponent = 0;
        for (i = 0; i < e_size; i++) {
            exponent = exponent * 256 + e[i];
        }
    } else {
        exponent = 65537;
    }

    mbedtls_rsa_init(&ctx, MBEDTLS_RSA_PKCS_V15, 0);
    ret = mbedtls_rsa_gen_key(&ctx, myrand, NULL, keybits, exponent);
    if (0 != ret) {
        mbedtls_rsa_free(&ctx);
        PRINT_RET(ALI_CRYPTO_ERROR, "mbedtls_rsa_gen_key failed %d\n", ret);
    }

    result = _ali_rsa_key_encode(PK_PRIVATE, (void *)&ctx, (void *)keypair);
    if (result != ALI_CRYPTO_SUCCESS) {
        mbedtls_rsa_free(&ctx);
        PRINT_RET(ALI_CRYPTO_ERROR, "Gen_keypair: rsa key encode fail(%08x)\n",
                  result);
    }

    INIT_CTX_MAGIC(((ali_rsa_keypair_t *)keypair)->magic);
    mbedtls_rsa_free(&ctx);

    return ALI_CRYPTO_SUCCESS;
}

ali_crypto_result ali_rsa_get_key_attr(rsa_key_attr_t attr,
                                       rsa_keypair_t *keypair, void *buffer,
                                       size_t *size)
{
    ali_rsa_keypair_t *rsa_key;

    if (keypair == NULL || buffer == NULL || size == NULL) {
        PRINT_RET(ALI_CRYPTO_INVALID_ARG, "Key_attr: invalid input args!\n");
    }

    if (!IS_VALID_CTX_MAGIC(((ali_rsa_keypair_t *)keypair)->magic)) {
        PRINT_RET(ALI_CRYPTO_INVALID_KEY, "Key_attr: invalid keypair!\n");
    }

    rsa_key = (ali_rsa_keypair_t *)keypair;
    switch (attr) {
        case RSA_MODULUS: {
            if (*size < rsa_key->n_size) {
                return ALI_CRYPTO_OUTOFMEM;
            }

            *size = rsa_key->n_size;
            OSA_memcpy(buffer, rsa_key->n, *size);

            break;
        }
        case RSA_PUBLIC_EXPONENT: {
            if (*size < rsa_key->e_size) {
                return ALI_CRYPTO_OUTOFMEM;
            }

            *size = rsa_key->e_size;
            OSA_memcpy(buffer, rsa_key->e, *size);

            break;
        }
        case RSA_PRIVATE_EXPONENT: {
            if (*size < rsa_key->d_size) {
                return ALI_CRYPTO_OUTOFMEM;
            }

            *size = rsa_key->d_size;
            OSA_memcpy(buffer, rsa_key->d, *size);

            break;
        }
        case RSA_PRIME1: {
            if (*size < rsa_key->p_size) {
                return ALI_CRYPTO_OUTOFMEM;
            }

            *size = rsa_key->p_size;
            OSA_memcpy(buffer, rsa_key->p, *size);

            break;
        }
        case RSA_PRIME2: {
            if (*size < rsa_key->q_size) {
                return ALI_CRYPTO_OUTOFMEM;
            }

            *size = rsa_key->q_size;
            OSA_memcpy(buffer, rsa_key->q, *size);

            break;
        }
        case RSA_EXPONENT1: {
            if (*size < rsa_key->dp_size) {
                return ALI_CRYPTO_OUTOFMEM;
            }

            *size = rsa_key->dp_size;
            OSA_memcpy(buffer, rsa_key->dp, *size);

            break;
        }
        case RSA_EXPONENT2: {
            if (*size < rsa_key->dq_size) {
                return ALI_CRYPTO_OUTOFMEM;
            }

            *size = rsa_key->dq_size;
            OSA_memcpy(buffer, rsa_key->dq, *size);

            break;
        }
        case RSA_COEFFICIENT: {
            if (*size < rsa_key->qp_size) {
                return ALI_CRYPTO_OUTOFMEM;
            }

            *size = rsa_key->qp_size;
            OSA_memcpy(buffer, rsa_key->qp, *size);

            break;
        }
        default:
            return ALI_CRYPTO_NOSUPPORT;
    }

    return ALI_CRYPTO_SUCCESS;
}

static ali_crypto_result _ali_rsa_public_encrypt_pre(
  mbedtls_rsa_context *ctx, const rsa_pubkey_t *pub_key)
{
    ali_crypto_result result = ALI_CRYPTO_SUCCESS;
    int32_t           ret;

    result = _ali_rsa_key_decode(PK_PUBLIC, (void *)pub_key, ctx);
    if (result != ALI_CRYPTO_SUCCESS) {
        GO_RET(ALI_CRYPTO_INVALID_KEY,
               "Pub_encrypt: rsa key decode fail(%08x)\n", result);
    }

    ret = mbedtls_rsa_check_pubkey(ctx);
    if (0 != ret) {
        GO_RET(ALI_CRYPTO_INVALID_KEY, "Pub_encrypt: rsa key invalid(%d)\n",
               (int)ret);
    }

_OUT:

    return result;
}

ali_crypto_result ali_rsa_public_encrypt(const rsa_pubkey_t *pub_key,
                                         const uint8_t *src, size_t src_size,
                                         uint8_t *dst, size_t *dst_size,
                                         rsa_padding_t padding)
{
    int                 ret;
    size_t              key_size;
    mbedtls_rsa_context ctx;
    ali_crypto_result   result = ALI_CRYPTO_SUCCESS;
    unsigned char *     key;

    if (pub_key == NULL || src == NULL || src_size == 0 || dst_size == NULL ||
        ((dst == NULL) && (*dst_size != 0))) {
        PRINT_RET(ALI_CRYPTO_INVALID_ARG, "Pub_encrypt: invalid input args!\n");
    }

    if (!IS_VALID_CTX_MAGIC(((ali_rsa_pubkey_t *)pub_key)->magic)) {
        PRINT_RET(ALI_CRYPTO_INVALID_KEY, "Pub_encrypt: invalid pubkey!\n");
    }

    switch (padding.type) {
        case RSA_NOPAD: {
#if 0
            if (src_size != key_size) {
                MBED_DBG_E("Pub_encrypt: invalid src_size(%d)\n", src_size);
                result = ALI_CRYPTO_LENGTH_ERR;
                goto _out;
            }

            ret = rsa_exptmod(src, (ulong_t)src_size,
                              dst, (ulong_t *)dst_size, key.type, &key);
            if (ret != CRYPT_OK) {
                MBED_DBG_E("Pub_encrypt: v1_5 encrypt fail(%d)\n", ret);
                result = ALI_CRYPTO_ERROR;
                goto _out;
            }
#endif
            PRINT_RET(ALI_CRYPTO_NOSUPPORT, "Pub_encrypt: NOPAD not support\n");
        }

        case RSAES_PKCS1_V1_5: {

            mbedtls_rsa_init(&ctx, MBEDTLS_RSA_PKCS_V15, HASH_NONE);

            result = _ali_rsa_public_encrypt_pre(&ctx, pub_key);
            if (result != ALI_CRYPTO_SUCCESS) {
                mbedtls_rsa_free(&ctx);
                PRINT_RET(result,
                          "Pub_encrypt: rsa_public_encrypt_pre fail(%08x)\n",
                          result);
            }

            key_size = mbedtls_mpi_size(&ctx.N);
            key      = (unsigned char *)mbedtls_malloc(key_size * 2);

            if (!key) {
                return MBEDTLS_ERR_RSA_PUBLIC_FAILED;
            }
            OSA_memset(key, 0, key_size * 2);

            mbedtls_mpi_write_binary(&ctx.N, key, key_size);
            mbedtls_mpi_write_binary(&ctx.E, key + key_size, key_size);
            ret = csi_tee_rsa_encrypt(src, src_size, key, key_size * 2, dst,
                                      dst_size, TEE_RSA_PKCS1_PADDING);
            if (ret != 0) {
                printf("RSA ENGINE fail \n");
            }

            mbedtls_free(key);

            if (ret) {
                return MBEDTLS_ERR_RSA_PUBLIC_FAILED;
            }

            break;
        }

        case RSAES_PKCS1_OAEP_MGF1: {
#if 0
            hash_type_t ali_hash_type;

            hash_type = _ali_get_hash_type(padding.pad.rsaes_oaep.type);
            if (-1 == hash_type) {
                PRINT_RET(ALI_CRYPTO_INVALID_ARG, "Pub_encrypt: invalid hash type!\n");
            }
            mbedtls_rsa_init(&ctx, MBEDTLS_RSA_PKCS_V21, hash_type);

            result = _ali_rsa_public_encrypt_pre(&ctx, pub_key);
            if (result != ALI_CRYPTO_SUCCESS) {
                mbedtls_rsa_free(&ctx);
                PRINT_RET(result,
                          "Pub_encrypt: rsa_public_encrypt_pre fail(%08x)\n",
                          result);
            }

            key_size = mbedtls_mpi_size(&ctx.N);
            if (*dst_size < key_size) {
                GO_RET(ALI_CRYPTO_SHORT_BUFFER,
                       "Pub_encrypt: short buffer (%d)\n", (int)*dst_size);
            }
            ctx.len = key_size;

            ali_hash_type = padding.pad.rsaes_oaep.type;
            if (2 * HASH_SIZE(ali_hash_type) >= key_size - 2 ||
                src_size > key_size - 2 * HASH_SIZE(ali_hash_type) - 2) {
                GO_RET(ALI_CRYPTO_LENGTH_ERR,
                       "Pub_encrypt: invalid src size(%d %d) for rsa_%d\n",
                       (int)src_size, HASH_SIZE(ali_hash_type), (int)key_size << 3);
            }

            ret = mbedtls_rsa_pkcs1_encrypt(&ctx, myrand,
                                            NULL, MBEDTLS_RSA_PUBLIC, src_size,
                                            (const unsigned char *)src, (unsigned char *)dst);
            if (0 != ret) {
                GO_RET(ALI_CRYPTO_ERROR,
                       "Pub_encrypt: v1_5 encrypt fail(%d)\n", ret);
            }
#endif
            break;
        }

        default:
            GO_RET(ALI_CRYPTO_NOSUPPORT,
                   "Pub_encrypt: invalid padding type(%d)\n", padding.type);
    }

_OUT:
    mbedtls_rsa_free(&ctx);

    return result;
}

static ali_crypto_result _ali_rsa_priv_decrypt_pre(
  mbedtls_rsa_context *ctx, const rsa_keypair_t *priv_key)
{
    ali_crypto_result result = ALI_CRYPTO_SUCCESS;

    result = _ali_rsa_key_decode(PK_PRIVATE, (void *)priv_key, ctx);
    if (result != ALI_CRYPTO_SUCCESS) {
        GO_RET(ALI_CRYPTO_INVALID_KEY,
               "Priv_decrypt: rsa key decode fail(%08x)\n", result);
    }

    /* mbedtsl will check p q */
#if 0
    ret = mbedtls_rsa_check_privkey(ctx);
    if (0 != ret) {
        MBED_DBG_E("Priv_decrypt: rsa key invalid(%d)\n", ret);
        result = ALI_CRYPTO_INVALID_KEY;
        goto _OUT;
    }
#endif

_OUT:

    return result;
}

ali_crypto_result ali_rsa_private_decrypt(const rsa_keypair_t *priv_key,
                                          const uint8_t *src, size_t src_size,
                                          uint8_t *dst, size_t *dst_size,
                                          rsa_padding_t padding)
{
    int                 ret;
    size_t              key_size;
    uint8_t *           tmp_dst = NULL;
    size_t              tmp_dst_size;
    mbedtls_rsa_context ctx;
    int32_t             hash_type;
    ali_crypto_result   result = ALI_CRYPTO_SUCCESS;
    unsigned char *     key;

    if (priv_key == NULL || src == NULL || src_size == 0 || dst_size == NULL ||
        ((dst == NULL) && (*dst_size != 0))) {
        PRINT_RET(ALI_CRYPTO_INVALID_ARG,
                  "Priv_decrypt: invalid input args!\n");
    }

    if (!IS_VALID_CTX_MAGIC(((ali_rsa_keypair_t *)priv_key)->magic)) {
        PRINT_RET(ALI_CRYPTO_INVALID_KEY, "Priv_decrypt: invalid priv_key!\n");
    }

    switch (padding.type) {
        case RSA_NOPAD: {
#if 0
            if (*dst_size < key_size) {
                MBED_DBG_E("Priv_decrypt: short buffer(%d)\n", *dst_size);
                result = ALI_CRYPTO_SHORT_BUFFER;
                goto _out;
            }

            ret = rsa_exptmod(src, (ulong_t)src_size,
                              dst, (ulong_t *)dst_size, key.type, &key);
            if (ret != CRYPT_OK) {
                MBED_DBG_E("Priv_decrypt: v1_5 decrypt fail(%d)\n", ret);
                result = ALI_CRYPTO_ERROR;
                goto _out;
            }
#endif
            PRINT_RET(ALI_CRYPTO_NOSUPPORT, "Pub_encrypt: NOPAD not support\n");
        }

        case RSAES_PKCS1_V1_5: {

            mbedtls_rsa_init(&ctx, MBEDTLS_RSA_PKCS_V15, HASH_NONE);

            result = _ali_rsa_priv_decrypt_pre(&ctx, priv_key);
            if (result != ALI_CRYPTO_SUCCESS) {
                mbedtls_rsa_free(&ctx);
                PRINT_RET(result,
                          "Priv_decrypt: rsa_public_encrypt_pre fail(%08x)\n",
                          result);
            }

            key_size = mbedtls_mpi_size(&ctx.N);
            key      = (unsigned char *)mbedtls_malloc(key_size * 3);

            if (!key) {
                return MBEDTLS_ERR_RSA_PUBLIC_FAILED;
            }
			OSA_memset(key, 0, key_size * 3);

            mbedtls_mpi_write_binary(&ctx.N, key, key_size);
            mbedtls_mpi_write_binary(&ctx.E, key + key_size, key_size);
            mbedtls_mpi_write_binary(&ctx.D, key + key_size + key_size,
                                     key_size);
            ret = csi_tee_rsa_decrypt(src, src_size, key, key_size * 3, dst,
                                      dst_size, TEE_RSA_PKCS1_PADDING);
            mbedtls_free(key);

            if (ret) {
                return MBEDTLS_ERR_RSA_KEY_GEN_FAILED;
            }

            break;
        }

        case RSAES_PKCS1_OAEP_MGF1: {
            hash_type_t ali_hash_type;

            hash_type = _ali_get_hash_type(padding.pad.rsaes_oaep.type);
            if (-1 == hash_type) {
                PRINT_RET(ALI_CRYPTO_INVALID_ARG,
                          "Pub_encrypt: invalid hash type!\n");
            }
            mbedtls_rsa_init(&ctx, MBEDTLS_RSA_PKCS_V21, hash_type);

            result = _ali_rsa_priv_decrypt_pre(&ctx, priv_key);
            if (result != ALI_CRYPTO_SUCCESS) {
                mbedtls_rsa_free(&ctx);
                PRINT_RET(result,
                          "Pub_decrypt: rsa_public_encrypt_pre fail(%08x)\n",
                          result);
            }

            key_size = mbedtls_mpi_size(&ctx.N);
            if (src_size != key_size) {
                GO_RET(ALI_CRYPTO_LENGTH_ERR,
                       "Priv_decrypt: invalid src size(%d)\n", (int)src_size);
            }
            ctx.len = key_size;

            ali_hash_type = padding.pad.rsaes_oaep.type;
            if (ali_hash_type == SHA512 && key_size == 128) {
                GO_RET(ALI_CRYPTO_NOSUPPORT,
                       "Priv_decrypt: rsa_%d not support hash(%d)\n",
                       (int)key_size << 3, ali_hash_type);
            }
            /* *dst_size max is key_size - 2*HASH_SIZE(hash_type) - 2 */

            tmp_dst_size = key_size;
            tmp_dst      = (uint8_t *)OSA_malloc(tmp_dst_size);
            if (!tmp_dst) {
                GO_RET(ALI_CRYPTO_OUTOFMEM, "Priv_decrypt: malloc(0x%x) fail\n",
                       (int)tmp_dst_size);
            }

            ret = mbedtls_rsa_pkcs1_decrypt(
              &ctx, myrand, NULL, MBEDTLS_RSA_PRIVATE, &tmp_dst_size,
              (const unsigned char *)src, (unsigned char *)tmp_dst,
              (size_t)key_size);
            if (0 != ret) {
                GO_RET(ALI_CRYPTO_ERROR,
                       "Priv_decrypt: v2_1 decrypt fail(%d)\n", ret);
            }

            if (*dst_size < tmp_dst_size) {
                GO_RET(ALI_CRYPTO_ERROR, "Priv_decrypt: short buffer(%d)\n",
                       (int)*dst_size);
            }

            OSA_memcpy(dst, tmp_dst, tmp_dst_size);
            *dst_size = tmp_dst_size;
            break;
        }

        default:
            MBED_DBG_E("Priv_decrypt: no support type(%d)\n", padding.type);
            result = ALI_CRYPTO_NOSUPPORT;
    }

_OUT:
    if (NULL != tmp_dst) {
        OSA_free(tmp_dst);
    }
    mbedtls_rsa_free(&ctx);

    return result;
}

ali_crypto_result ali_rsa_sign(const rsa_keypair_t *priv_key,
                               const uint8_t *dig, size_t dig_size,
                               uint8_t *sig, size_t *sig_size,
                               rsa_padding_t padding)
{
    int                 ret;
    uint32_t            key_size;
    uint32_t            hash_size;
    hash_type_t         ali_hash_type;
    int32_t             hash_type;
    mbedtls_rsa_context ctx;
    ali_crypto_result   result = ALI_CRYPTO_SUCCESS;
    unsigned char *     key;
    unsigned char       type;

    if (priv_key == NULL || dig == NULL || dig_size == 0 || sig == NULL ||
        sig_size == NULL) {
        PRINT_RET(ALI_CRYPTO_INVALID_ARG, "Rsa_sign: invalid input args!\n");
    }

    if (!IS_VALID_CTX_MAGIC(((ali_rsa_keypair_t *)priv_key)->magic)) {
        PRINT_RET(ALI_CRYPTO_INVALID_KEY, "Rsa_sign: invalid priv_key!\n");
    }

    if (padding.type == RSASSA_PKCS1_V1_5) {
        ali_hash_type = padding.pad.rsassa_v1_5.type;
    } else if (padding.type == RSASSA_PKCS1_PSS_MGF1) {
        ali_hash_type = padding.pad.rsassa_pss.type;
    } else {
        PRINT_RET(ALI_CRYPTO_NOSUPPORT, "Rsa_sign: invalid padding type(%d)\n",
                  padding.type);
    }

    hash_size = _ali_get_hash_size(ali_hash_type);
    if (dig_size != hash_size) {
        PRINT_RET(ALI_CRYPTO_LENGTH_ERR,
                  "Rsa_sign: invalid dig size(%d vs %d)\n", (int)dig_size,
                  (int)hash_size);
    }

    hash_type = _ali_get_hash_type(ali_hash_type);
    if (-1 == hash_type) {
        PRINT_RET(ALI_CRYPTO_INVALID_ARG, "Rsa_sign: invalid hash type!\n");
    }

    if (padding.type == RSASSA_PKCS1_V1_5) {
        mbedtls_rsa_init(&ctx, MBEDTLS_RSA_PKCS_V15, hash_type);
    } else {
        mbedtls_rsa_init(&ctx, MBEDTLS_RSA_PKCS_V21, hash_type);
    }

    result = _ali_rsa_priv_decrypt_pre(&ctx, priv_key);

    if (0 != result) {
        mbedtls_rsa_free(&ctx);
        PRINT_RET(result, "Rsa_sign: mbedtls_ctr_drbg_seed fail(%08x)\n",
                  result);
    }

    key_size = mbedtls_mpi_size(&ctx.N);
    if (*sig_size < key_size) {
        GO_RET(ALI_CRYPTO_SHORT_BUFFER, "Priv_decrypt: invalid src size(%d)\n",
               (int)*sig_size);
    }
    ctx.len = key_size;

    if (padding.type == RSASSA_PKCS1_V1_5) {
        if (dig_size + 11 > key_size) {
            GO_RET(ALI_CRYPTO_LENGTH_ERR, "Rsa_sign: invalid dig_size(%d)\n",
                   (int)dig_size);
        }
    } else {
        if (key_size < padding.pad.rsassa_pss.salt_len + hash_size + 2) {
            GO_RET(ALI_CRYPTO_LENGTH_ERR,
                   "Rsa_sign: invalid salt size(%d) for rsa_%d\n",
                   (int)padding.pad.rsassa_pss.salt_len, (int)key_size << 3);
        }
    }


    if (ali_hash_type == (int)MBEDTLS_MD_MD5) {
        type = TEE_RSA_MD5;
    } else if (ali_hash_type == (int)MBEDTLS_MD_SHA1) {
        type = TEE_RSA_SHA1;
    } else if (ali_hash_type == (int)MBEDTLS_MD_SHA256) {
        type = TEE_RSA_SHA256;
    } else {
        return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
    }

    key_size = mbedtls_mpi_size(&ctx.N);
    key      = (unsigned char *)mbedtls_malloc(key_size * 3);

    if (!key) {
        return MBEDTLS_ERR_RSA_PUBLIC_FAILED;
    }
	OSA_memset(key, 0, key_size * 3);

    mbedtls_mpi_write_binary(&ctx.N, key, key_size);
    mbedtls_mpi_write_binary(&ctx.E, key + key_size, key_size);
    mbedtls_mpi_write_binary(&ctx.D, key + key_size + key_size, key_size);
    ret =
      csi_tee_rsa_sign(dig, dig_size, key, key_size * 3, sig, sig_size, type);
    mbedtls_free(key);

    if (ret) {
        return MBEDTLS_ERR_RSA_PRIVATE_FAILED;
    }

    if (0 != ret) {
        GO_RET(ALI_CRYPTO_ERROR, "Rsa_sign: mbedtls_rsa_pkcs1_sign fail %d\n",
               ret);
    }

_OUT:
    mbedtls_rsa_free(&ctx);
    return result;
}

ali_crypto_result ali_rsa_verify(const rsa_pubkey_t *pub_key,
                                 const uint8_t *dig, size_t dig_size,
                                 const uint8_t *sig, size_t sig_size,
                                 rsa_padding_t padding, bool *p_result)
{
    int                 ret;
    uint32_t            hash_size;
    uint32_t            key_size;
    hash_type_t         ali_hash_type;
    int32_t             hash_type;
    mbedtls_rsa_context ctx;
    ali_crypto_result   result = ALI_CRYPTO_SUCCESS;
    unsigned char       type;
    unsigned char *     key;

    if (pub_key == NULL || NULL == p_result || dig == NULL || dig_size == 0 ||
        sig == NULL || sig_size == 0) {
        PRINT_RET(ALI_CRYPTO_INVALID_ARG, "Rsa_verify: invalid input args!\n");
    }

    if (!IS_VALID_CTX_MAGIC(((ali_rsa_pubkey_t *)pub_key)->magic)) {
        PRINT_RET(ALI_CRYPTO_INVALID_KEY, "Rsa_verify: invalid pubkey!\n");
    }

    if (padding.type == RSASSA_PKCS1_V1_5) {
        ali_hash_type = padding.pad.rsassa_v1_5.type;
    } else if (padding.type == RSASSA_PKCS1_PSS_MGF1) {
        ali_hash_type = padding.pad.rsassa_pss.type;
    } else {
        *p_result = 0;
        PRINT_RET(ALI_CRYPTO_NOSUPPORT,
                  "Rsa_verify: invalid padding type(%d)\n", padding.type);
    }

    hash_size = _ali_get_hash_size(ali_hash_type);
    if (dig_size != hash_size) {
        *p_result = 1;
        PRINT_RET(ALI_CRYPTO_LENGTH_ERR,
                  "Rsa_verify: invalid dig size(%d vs %d)\n", (int)dig_size,
                  (int)hash_size);
    }

    hash_type = _ali_get_hash_type(ali_hash_type);
    if (-1 == hash_type) {
        *p_result = 0;
        PRINT_RET(ALI_CRYPTO_INVALID_ARG, "Rsa_verify: invalid hash type!\n");
    }

    if (padding.type == RSASSA_PKCS1_V1_5) {
        mbedtls_rsa_init(&ctx, MBEDTLS_RSA_PKCS_V15, hash_type);
    } else {
        mbedtls_rsa_init(&ctx, MBEDTLS_RSA_PKCS_V21, hash_type);
    }

    result = _ali_rsa_key_decode(PK_PUBLIC, (void *)pub_key, &ctx);
    if (ALI_CRYPTO_SUCCESS != result) {
        *p_result = 0;
        GO_RET(ALI_CRYPTO_INVALID_KEY,
               "Rsa_verify: rsa key decode fail(%08x)\n", result);
    }

    ret = mbedtls_rsa_check_pubkey(&ctx);
    if (0 != ret) {
        GO_RET(ALI_CRYPTO_INVALID_KEY, "Rsa_verify: rsa key invalid(%08x)\n",
               ret);
    }

    key_size = mbedtls_mpi_size(&ctx.N);
    if (sig_size != key_size) {
        GO_RET(ALI_CRYPTO_LENGTH_ERR, "Rsa_verify: invalid src size(%d)\n",
               (int)sig_size);
    }
    ctx.len = key_size;

    if (padding.type == RSASSA_PKCS1_V1_5) {
        if (dig_size + 11 > key_size) {
            GO_RET(ALI_CRYPTO_LENGTH_ERR, "Rsa_verify: invalid dig_size(%d)\n",
                   (int)dig_size);
        }
    } else {
        if (key_size < padding.pad.rsassa_pss.salt_len + hash_size + 2) {
            GO_RET(ALI_CRYPTO_LENGTH_ERR,
                   "Rsa_verify: invalid salt size(%d) for rsa_%d\n",
                   (int)padding.pad.rsassa_pss.salt_len, (int)key_size << 3);
        }
    }

    if (ali_hash_type == (int)MBEDTLS_MD_MD5) {
        type = TEE_RSA_MD5;
    } else if (ali_hash_type == (int)MBEDTLS_MD_SHA1) {
        type = TEE_RSA_SHA1;
    } else if (ali_hash_type == (int)MBEDTLS_MD_SHA256) {
        type = TEE_RSA_SHA256;
    } else {
        return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
    }

    key_size = mbedtls_mpi_size(&ctx.N);
    key      = (unsigned char *)mbedtls_malloc(key_size * 2);

    if (!key) {
        return MBEDTLS_ERR_RSA_PUBLIC_FAILED;
    }
	OSA_memset(key, 0, key_size * 2);

    mbedtls_mpi_write_binary(&ctx.N, key, key_size);
    mbedtls_mpi_write_binary(&ctx.E, key + key_size, key_size);
    ret = csi_tee_rsa_verify((uint8_t *)dig, dig_size, key, key_size * 2,
                             (uint8_t *)sig, ctx.len, type);
    mbedtls_free(key);

    result = ret ? 0 : 1;

_OUT:
    if (0 == result) {
        *p_result = 1;
    } else {
        *p_result = 0;
    }
    mbedtls_rsa_free(&ctx);

    return result;
}
