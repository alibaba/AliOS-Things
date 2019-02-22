/**
 * Copyright (C) 2017  Alibaba Group Holding Limited.
 */
#include "sal_crypto.h"
#include "osa.h"
#include "ali_crypto.h"

#define TEE_MIN_RSA_KEY_SIZE (256)
#define TEE_MAX_RSA_KEY_SIZE (2048)

typedef struct _sal_rsa_keypair_t
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
} sal_rsa_keypair_t;

typedef struct _sal_rsa_pubkey_t
{
    uint32_t magic;
    size_t   n_size;
    size_t   e_size;
    uint8_t  n[(TEE_MAX_RSA_KEY_SIZE >> 3)];
    uint8_t  e[(TEE_MAX_RSA_KEY_SIZE >> 3)];
} sal_rsa_pubkey_t;

static int myrand(void *rng_state, unsigned char *output, size_t len)
{
    int result;

    result = ali_rand_gen(output, len);
    if (result != ALI_CRYPTO_SUCCESS) {
        PRINT_RET(-1, "gen rand fail(%08x)\n", result);
    }
    return 0;
}

/* sal rsa_key -> tee rsa_key */
static ali_crypto_result _sal_rsa_key_encode(uint32_t type, void *in_key,
                                             void *out_key)
{
    ali_algo_rsa_context *key;
    int32_t              ret;

    if (in_key == NULL || out_key == NULL) {
        return ALI_CRYPTO_INVALID_ARG;
    }

    key = (ali_algo_rsa_context *)in_key;
    if (type == PK_PUBLIC) {
        /* FIXME(not use now in this branch) */
#if 0
        size_t len;
        sal_rsa_pubkey_t *rsa_key = (sal_rsa_pubkey_t *)out_key;
        osa_memset(rsa_key, 0, sizeof(sal_rsa_pubkey_t));

        len = ali_algo_mpi_size(&key->N);
        /* TODO change the endien mode */
        ret = ali_algo_mpi_write_binary(&key->N, (unsigned char *)rsa_key->n, len);
        if (0 != ret) {
            return ALI_CRYPTO_INVALID_KEY;
        }
        rsa_key->n_size = len;

        len = ali_algo_mpi_size(&key->E);
        ret = ali_algo_mpi_write_binary(&key->E, (unsigned char *)rsa_key->e, len);
        if (0 != ret) {
            return ALI_CRYPTO_INVALID_KEY;
        }
        rsa_key->e_size = len;
#endif
    } else if (type == PK_PRIVATE) {
        size_t             len;
        sal_rsa_keypair_t *rsa_key = (sal_rsa_keypair_t *)out_key;
        osa_memset(rsa_key, 0, sizeof(sal_rsa_keypair_t));

        len = ali_algo_mpi_size(&key->N);
        /* TODO change the endien mode */
        ret =
          ali_algo_mpi_write_binary(&key->N, (unsigned char *)rsa_key->n, len);
        if (0 != ret) {
            return ALI_CRYPTO_INVALID_KEY;
        }
        rsa_key->n_size = len;

        len = ali_algo_mpi_size(&key->E);
        ret =
          ali_algo_mpi_write_binary(&key->E, (unsigned char *)rsa_key->e, len);
        if (0 != ret) {
            return ALI_CRYPTO_INVALID_KEY;
        }
        rsa_key->e_size = len;

        len = ali_algo_mpi_size(&key->D);
        ret =
          ali_algo_mpi_write_binary(&key->D, (unsigned char *)rsa_key->d, len);
        if (0 != ret) {
            return ALI_CRYPTO_INVALID_KEY;
        }
        rsa_key->d_size = len;

        len = ali_algo_mpi_size(&key->P);
        ret =
          ali_algo_mpi_write_binary(&key->P, (unsigned char *)rsa_key->p, len);
        if (0 != ret) {
            return ALI_CRYPTO_INVALID_KEY;
        }
        rsa_key->p_size = len;

        len = ali_algo_mpi_size(&key->Q);
        ret =
          ali_algo_mpi_write_binary(&key->Q, (unsigned char *)rsa_key->q, len);
        if (0 != ret) {
            return ALI_CRYPTO_INVALID_KEY;
        }
        rsa_key->q_size = len;

        len = ali_algo_mpi_size(&key->DP);
        ret =
          ali_algo_mpi_write_binary(&key->DP, (unsigned char *)rsa_key->dp, len);
        if (0 != ret) {
            return ALI_CRYPTO_INVALID_KEY;
        }
        rsa_key->dp_size = len;

        len = ali_algo_mpi_size(&key->DQ);
        ret =
          ali_algo_mpi_write_binary(&key->DQ, (unsigned char *)rsa_key->dq, len);
        if (0 != ret) {
            return ALI_CRYPTO_INVALID_KEY;
        }
        rsa_key->dq_size = len;

        len = ali_algo_mpi_size(&key->QP);
        ret = ali_algo_mpi_write_binary(&key->QP, (unsigned char *)rsa_key->qp,
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

/* tee rsa_key -> sal rsa_key */
static ali_crypto_result _sal_rsa_key_decode(uint32_t type, void *in_key,
                                             void *out_key)
{
    int                  ret;
    ali_algo_rsa_context *key;

    if (in_key == NULL || out_key == NULL) {
        return ALI_CRYPTO_INVALID_ARG;
    }

    key = (ali_algo_rsa_context *)out_key;
    if (type == PK_PUBLIC) {
        sal_rsa_pubkey_t *rsa_key = (sal_rsa_pubkey_t *)in_key;

        ret = ali_algo_mpi_read_binary(&key->N, rsa_key->n, rsa_key->n_size);
        if (0 != ret) {
            return ALI_CRYPTO_INVALID_KEY;
        }
        ret = ali_algo_mpi_read_binary(&key->E, rsa_key->e, rsa_key->e_size);
        if (0 != ret) {
            return ALI_CRYPTO_INVALID_KEY;
        }
    } else if (type == PK_PRIVATE) {
        sal_rsa_keypair_t *rsa_key = (sal_rsa_keypair_t *)in_key;

        ret = ali_algo_mpi_read_binary(&key->N, rsa_key->n, rsa_key->n_size);
        if (0 != ret) {
            return ALI_CRYPTO_INVALID_KEY;
        }

        ret = ali_algo_mpi_read_binary(&key->E, rsa_key->e, rsa_key->e_size);
        if (0 != ret) {
            return ALI_CRYPTO_INVALID_KEY;
        }

        ret = ali_algo_mpi_read_binary(&key->D, rsa_key->d, rsa_key->d_size);
        if (0 != ret) {
            return ALI_CRYPTO_INVALID_KEY;
        }

        if ((rsa_key->p_size) && (rsa_key->q_size) && (rsa_key->qp_size) &&
            (rsa_key->dp_size) && (rsa_key->dq_size)) {
            ret = ali_algo_mpi_read_binary(&key->P, rsa_key->p, rsa_key->p_size);
            if (0 != ret) {
                return ALI_CRYPTO_INVALID_KEY;
            }
            ret = ali_algo_mpi_read_binary(&key->Q, rsa_key->q, rsa_key->q_size);
            if (0 != ret) {
                return ALI_CRYPTO_INVALID_KEY;
            }
            ret =
              ali_algo_mpi_read_binary(&key->DP, rsa_key->dp, rsa_key->dp_size);
            if (0 != ret) {
                return ALI_CRYPTO_INVALID_KEY;
            }
            ret =
              ali_algo_mpi_read_binary(&key->DQ, rsa_key->dq, rsa_key->dq_size);
            if (0 != ret) {
                return ALI_CRYPTO_INVALID_KEY;
            }
            ret =
              ali_algo_mpi_read_binary(&key->QP, rsa_key->qp, rsa_key->qp_size);
            if (0 != ret) {
                return ALI_CRYPTO_INVALID_KEY;
            }
        }
    } else {
        return ALI_CRYPTO_INVALID_TYPE;
    }

    return ALI_CRYPTO_SUCCESS;
}

static uint32_t _sal_get_hash_size(hash_type_t type)
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

static int32_t _sal_get_hash_type(hash_type_t type)
{
    ali_algo_md_type_t hash_type;

    switch (type) {
        case HASH_NONE:
            hash_type = ALI_ALGO_MD_NONE;
            break;
        case SHA1:
            hash_type = ALI_ALGO_MD_SHA1;
            break;
        case SHA224:
            hash_type = ALI_ALGO_MD_SHA224;
            break;
        case SHA256:
            hash_type = ALI_ALGO_MD_SHA256;
            break;
        case SHA384:
            hash_type = ALI_ALGO_MD_SHA384;
            break;
        case SHA512:
            hash_type = ALI_ALGO_MD_SHA512;
            break;
        case MD5:
            hash_type = ALI_ALGO_MD_MD5;
            break;
        default:
            PRINT_RET(-1, "get_hash_type: invalid type(%d)\n", type);
    }

    return (int32_t)hash_type;
}

ali_crypto_result sal_rsa_get_keypair_size(size_t keybits, size_t *size)
{
    if (size == NULL) {
        return ALI_CRYPTO_INVALID_ARG;
    }

    if (keybits < TEE_MIN_RSA_KEY_SIZE || keybits > TEE_MAX_RSA_KEY_SIZE) {
        PRINT_RET(ALI_CRYPTO_INVALID_ARG,
                  "get_keypair_size: invalid keybits(%d)\n", (int)keybits);
    }

    *size = sizeof(sal_rsa_keypair_t);

    return ALI_CRYPTO_SUCCESS;
}

ali_crypto_result sal_rsa_get_pubkey_size(size_t keybits, size_t *size)
{
    if (size == NULL) {
        return ALI_CRYPTO_INVALID_ARG;
    }

    if (keybits < TEE_MIN_RSA_KEY_SIZE || keybits > TEE_MAX_RSA_KEY_SIZE) {
        PRINT_RET(ALI_CRYPTO_INVALID_ARG,
                  "get_pubkey_size: invalid keybits(%d)\n", (int)keybits);
    }

    *size = sizeof(sal_rsa_pubkey_t);

    return ALI_CRYPTO_SUCCESS;
}

ali_crypto_result sal_rsa_init_keypair(
  size_t keybits, const uint8_t *n, size_t n_size, const uint8_t *e,
  size_t e_size, const uint8_t *d, size_t d_size, const uint8_t *p,
  size_t p_size, const uint8_t *q, size_t q_size, const uint8_t *dp,
  size_t dp_size, const uint8_t *dq, size_t dq_size, const uint8_t *qp,
  size_t qp_size, rsa_keypair_t *keypair)
{
    sal_rsa_keypair_t *rsa_key;

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

    rsa_key = (sal_rsa_keypair_t *)keypair;
    osa_memset(rsa_key, 0, sizeof(sal_rsa_keypair_t));
    rsa_key->n_size = n_size;
    rsa_key->e_size = e_size;
    rsa_key->d_size = d_size;
    osa_memcpy(rsa_key->n, n, n_size);
    osa_memcpy(rsa_key->e, e, e_size);
    osa_memcpy(rsa_key->d, d, d_size);

    if (p && p_size) {
        rsa_key->p_size = p_size;
        osa_memcpy(rsa_key->p, p, p_size);
    }
    if (q && q_size) {
        rsa_key->q_size = q_size;
        osa_memcpy(rsa_key->q, q, q_size);
    }
    if (dp && dp_size) {
        rsa_key->dp_size = dp_size;
        osa_memcpy(rsa_key->dp, dp, dp_size);
    }
    if (dq && dq_size) {
        rsa_key->dq_size = dq_size;
        osa_memcpy(rsa_key->dq, dq, dq_size);
    }
    if (qp && qp_size) {
        rsa_key->qp_size = qp_size;
        osa_memcpy(rsa_key->qp, qp, qp_size);
    }

    INIT_CTX_MAGIC(rsa_key->magic);
    return ALI_CRYPTO_SUCCESS;
}

ali_crypto_result sal_rsa_init_pubkey(size_t keybits, const uint8_t *n,
                                      size_t n_size, const uint8_t *e,
                                      size_t e_size, rsa_pubkey_t *pubkey)
{
    sal_rsa_pubkey_t *pub_key;

    if (pubkey == NULL || n == NULL || n_size == 0 || e == NULL ||
        e_size == 0) {
        PRINT_RET(ALI_CRYPTO_INVALID_ARG, "Init_pubkey: invalid args!\n");
    }

    if ((n_size << 3) > keybits || (e_size << 3) > keybits) {
        PRINT_RET(ALI_CRYPTO_INVALID_ARG,
                  "Init_pubkey: key param size not match with key size\n");
    }

    pub_key         = (sal_rsa_pubkey_t *)pubkey;
    pub_key->n_size = n_size;
    pub_key->e_size = e_size;
    osa_memcpy(pub_key->n, n, n_size);
    osa_memcpy(pub_key->e, e, e_size);

    INIT_CTX_MAGIC(pub_key->magic);
    return ALI_CRYPTO_SUCCESS;
}

ali_crypto_result sal_rsa_gen_keypair(size_t keybits, const uint8_t *e,
                                      size_t e_size, rsa_keypair_t *keypair)
{
    int                  ret, exponent;
    uint32_t             seed;
    ali_algo_rsa_context ctx;
    ali_crypto_result    result = 0;

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

    ali_algo_rsa_init(&ctx, ALI_ALGO_RSA_PKCS_V15, 0);
    seed = (uint32_t)ls_osa_get_time_ms();
    ret = ali_seed((uint8_t *)&seed, 4);
    if (0 != ret) {
        GO_RET(ALI_CRYPTO_ERROR, "sal_rsa_gen_keypair: set seed failed %d\n", ret);
    }
    ret = ali_algo_rsa_gen_key(&ctx, myrand, &seed, keybits, exponent);
    if (0 != ret) {
        GO_RET(ALI_CRYPTO_ERROR, "ali_algo_rsa_gen_key failed %d\n", ret);
    }

    ret = _sal_rsa_key_encode(PK_PRIVATE, (void *)&ctx, (void *)keypair);
    if (ret != ALI_CRYPTO_SUCCESS) {
        GO_RET(ALI_CRYPTO_ERROR, "Gen_keypair: rsa key encode fail(%08x)\n",
                  ret);
    }

    INIT_CTX_MAGIC(((sal_rsa_keypair_t *)keypair)->magic);
_OUT:
    ali_algo_rsa_free(&ctx);
    return result;
}

ali_crypto_result sal_rsa_get_key_attr(rsa_key_attr_t attr,
                                       rsa_keypair_t *keypair, void *buffer,
                                       size_t *size)
{
    sal_rsa_keypair_t *rsa_key;

    if (keypair == NULL || buffer == NULL || size == NULL) {
        PRINT_RET(ALI_CRYPTO_INVALID_ARG, "Key_attr: invalid input args!\n");
    }

    if (!IS_VALID_CTX_MAGIC(((sal_rsa_keypair_t *)keypair)->magic)) {
        PRINT_RET(ALI_CRYPTO_INVALID_KEY, "Key_attr: invalid keypair!\n");
    }

    rsa_key = (sal_rsa_keypair_t *)keypair;
    switch (attr) {
        case RSA_MODULUS: {
            if (*size < rsa_key->n_size) {
                return ALI_CRYPTO_OUTOFMEM;
            }

            *size = rsa_key->n_size;
            osa_memcpy(buffer, rsa_key->n, *size);

            break;
        }
        case RSA_PUBLIC_EXPONENT: {
            if (*size < rsa_key->e_size) {
                return ALI_CRYPTO_OUTOFMEM;
            }

            *size = rsa_key->e_size;
            osa_memcpy(buffer, rsa_key->e, *size);

            break;
        }
        case RSA_PRIVATE_EXPONENT: {
            if (*size < rsa_key->d_size) {
                return ALI_CRYPTO_OUTOFMEM;
            }

            *size = rsa_key->d_size;
            osa_memcpy(buffer, rsa_key->d, *size);

            break;
        }
        case RSA_PRIME1: {
            if (*size < rsa_key->p_size) {
                return ALI_CRYPTO_OUTOFMEM;
            }

            *size = rsa_key->p_size;
            osa_memcpy(buffer, rsa_key->p, *size);

            break;
        }
        case RSA_PRIME2: {
            if (*size < rsa_key->q_size) {
                return ALI_CRYPTO_OUTOFMEM;
            }

            *size = rsa_key->q_size;
            osa_memcpy(buffer, rsa_key->q, *size);

            break;
        }
        case RSA_EXPONENT1: {
            if (*size < rsa_key->dp_size) {
                return ALI_CRYPTO_OUTOFMEM;
            }

            *size = rsa_key->dp_size;
            osa_memcpy(buffer, rsa_key->dp, *size);

            break;
        }
        case RSA_EXPONENT2: {
            if (*size < rsa_key->dq_size) {
                return ALI_CRYPTO_OUTOFMEM;
            }

            *size = rsa_key->dq_size;
            osa_memcpy(buffer, rsa_key->dq, *size);

            break;
        }
        case RSA_COEFFICIENT: {
            if (*size < rsa_key->qp_size) {
                return ALI_CRYPTO_OUTOFMEM;
            }

            *size = rsa_key->qp_size;
            osa_memcpy(buffer, rsa_key->qp, *size);

            break;
        }
        default:
            return ALI_CRYPTO_NOSUPPORT;
    }

    return ALI_CRYPTO_SUCCESS;
}

static ali_crypto_result _sal_rsa_public_encrypt_pre(
  ali_algo_rsa_context *ctx, const rsa_pubkey_t *pub_key)
{
    ali_crypto_result result = ALI_CRYPTO_SUCCESS;
    int32_t           ret;

    result = _sal_rsa_key_decode(PK_PUBLIC, (void *)pub_key, ctx);
    if (result != ALI_CRYPTO_SUCCESS) {
        GO_RET(ALI_CRYPTO_INVALID_KEY,
               "Pub_encrypt: rsa key decode fail(%08x)\n", result);
    }

    ret = ali_algo_rsa_check_pubkey(ctx);
    if (0 != ret) {
        GO_RET(ALI_CRYPTO_INVALID_KEY, "Pub_encrypt: rsa key invalid(%d)\n",
               (int)ret);
    }

_OUT:

    return result;
}

ali_crypto_result sal_rsa_public_encrypt(const rsa_pubkey_t *pub_key,
                                         const uint8_t *src, size_t src_size,
                                         uint8_t *dst, size_t *dst_size,
                                         rsa_padding_t padding)
{
    int                 ret;
    size_t              key_size;
    ali_algo_rsa_context ctx;
    int32_t             hash_type;
    ali_crypto_result   result = ALI_CRYPTO_SUCCESS;
    uint32_t           seed;

    if (pub_key == NULL || src == NULL || src_size == 0 || dst_size == NULL ||
        ((dst == NULL) && (*dst_size != 0))) {
        PRINT_RET(ALI_CRYPTO_INVALID_ARG, "Pub_encrypt: invalid input args!\n");
    }

    if (!IS_VALID_CTX_MAGIC(((sal_rsa_pubkey_t *)pub_key)->magic)) {
        PRINT_RET(ALI_CRYPTO_INVALID_KEY, "Pub_encrypt: invalid pubkey!\n");
    }

    switch (padding.type) {
        case RSA_NOPAD: {
#if 0
            if (src_size != key_size) {
                ALI_CRYPTO_DBG_E("Pub_encrypt: invalid src_size(%d)\n", src_size);
                result = ALI_CRYPTO_LENGTH_ERR;
                goto _out;
            }

            ret = rsa_exptmod(src, (ulong_t)src_size,
                              dst, (ulong_t *)dst_size, key.type, &key);
            if (ret != CRYPT_OK) {
                ALI_CRYPTO_DBG_E("Pub_encrypt: v1_5 encrypt fail(%d)\n", ret);
                result = ALI_CRYPTO_ERROR;
                goto _out;
            }
#endif
            PRINT_RET(ALI_CRYPTO_NOSUPPORT, "Pub_encrypt: NOPAD not support\n");
        }

        case RSAES_PKCS1_V1_5: {
            ali_algo_rsa_init(&ctx, ALI_ALGO_RSA_PKCS_V15, HASH_NONE);

            result = _sal_rsa_public_encrypt_pre(&ctx, pub_key);
            if (result != ALI_CRYPTO_SUCCESS) {
                ali_algo_rsa_free(&ctx);
                PRINT_RET(result,
                          "Pub_encrypt: rsa_public_encrypt_pre fail(%08x)\n",
                          result);
            }

            key_size = ali_algo_mpi_size(&ctx.N);
            if (*dst_size < key_size) {
                *dst_size = key_size;
                GO_RET(ALI_CRYPTO_SHORT_BUFFER,
                       "Pub_encrypt: short buffer (%d)\n", (int)*dst_size);
            }
            ctx.len = key_size;

            if (src_size + 11 > key_size) {
                GO_RET(ALI_CRYPTO_LENGTH_ERR,
                       "Pub_encrypt: invalid src_size(%d)\n", (int)src_size);
            }

            seed = (uint32_t)ls_osa_get_time_ms();
            ret = ali_seed((uint8_t *)&seed, 4);
            if (0 != ret) {
                GO_RET(ALI_CRYPTO_ERROR, "Pub_encrypt: set seed failed %d\n", ret);
            }
            ret = ali_algo_rsa_pkcs1_encrypt(
              &ctx, myrand, (uint32_t *)&seed, ALI_ALGO_RSA_PUBLIC, src_size,
              (const unsigned char *)src, (unsigned char *)dst);
            if (0 != ret) {
                GO_RET(ALI_CRYPTO_ERROR, "Pub_encrypt: v1_5 encrypt fail(%d)\n",
                       ret);
            }
            break;
        }

        case RSAES_PKCS1_OAEP_MGF1: {
            hash_type_t ali_hash_type;

            hash_type = _sal_get_hash_type(padding.pad.rsaes_oaep.type);
            if (-1 == hash_type) {
                PRINT_RET(ALI_CRYPTO_INVALID_ARG,
                          "Pub_encrypt: invalid hash type!\n");
            }
            ali_algo_rsa_init(&ctx, ALI_ALGO_RSA_PKCS_V21, hash_type);

            result = _sal_rsa_public_encrypt_pre(&ctx, pub_key);
            if (result != ALI_CRYPTO_SUCCESS) {
                ali_algo_rsa_free(&ctx);
                PRINT_RET(result,
                          "Pub_encrypt: rsa_public_encrypt_pre fail(%08x)\n",
                          result);
            }

            key_size = ali_algo_mpi_size(&ctx.N);
            if (*dst_size < key_size) {
                *dst_size = key_size;
                GO_RET(ALI_CRYPTO_SHORT_BUFFER,
                       "Pub_encrypt: short buffer (%d)\n", (int)*dst_size);
            }
            ctx.len = key_size;

            ali_hash_type = padding.pad.rsaes_oaep.type;
            if (2 * HASH_SIZE(ali_hash_type) >= key_size - 2 ||
                src_size > key_size - 2 * HASH_SIZE(ali_hash_type) - 2) {
                GO_RET(ALI_CRYPTO_LENGTH_ERR,
                       "Pub_encrypt: invalid src size(%d %d) for rsa_%d\n",
                       (int)src_size, HASH_SIZE(ali_hash_type),
                       (int)key_size << 3);
            }

            seed = (uint32_t)ls_osa_get_time_ms();
            ret = ali_seed((uint8_t *)&seed, 4);
            if (0 != ret) {
                GO_RET(ALI_CRYPTO_ERROR, "Pub_encrypt: set seed failed %d\n", ret);
            }
            ret = ali_algo_rsa_pkcs1_encrypt(
              &ctx, myrand, (uint32_t *)&seed, ALI_ALGO_RSA_PUBLIC, src_size,
              (const unsigned char *)src, (unsigned char *)dst);
            if (0 != ret) {
                GO_RET(ALI_CRYPTO_ERROR, "Pub_encrypt: v1_5 encrypt fail(%d)\n",
                       ret);
            }

            break;
        }

        default:
            GO_RET(ALI_CRYPTO_NOSUPPORT,
                   "Pub_encrypt: invalid padding type(%d)\n", padding.type);
    }

_OUT:
    ali_algo_rsa_free(&ctx);

    return result;
}

static ali_crypto_result _sal_rsa_priv_decrypt_pre(
  ali_algo_rsa_context *ctx, const rsa_keypair_t *priv_key)
{
    ali_crypto_result result = ALI_CRYPTO_SUCCESS;

    result = _sal_rsa_key_decode(PK_PRIVATE, (void *)priv_key, ctx);
    if (result != ALI_CRYPTO_SUCCESS) {
        GO_RET(ALI_CRYPTO_INVALID_KEY,
               "Priv_decrypt: rsa key decode fail(%08x)\n", result);
    }

    /* alicrypto_algo will check p q */
#if 0
    ret = ali_algo_rsa_check_privkey(ctx);
    if (0 != ret) {
        ALI_CRYPTO_DBG_E("Priv_decrypt: rsa key invalid(%d)\n", ret);
        result = ALI_CRYPTO_INVALID_KEY;
        goto _OUT;
    }
#endif

_OUT:

    return result;
}

ali_crypto_result sal_rsa_private_decrypt(const rsa_keypair_t *priv_key,
                                          const uint8_t *src, size_t src_size,
                                          uint8_t *dst, size_t *dst_size,
                                          rsa_padding_t padding)
{
    int                 ret;
    size_t              key_size;
    uint8_t *           tmp_dst = NULL;
    size_t              tmp_dst_size;
    ali_algo_rsa_context ctx;
    int32_t             hash_type;
    ali_crypto_result   result = ALI_CRYPTO_SUCCESS;
    uint32_t            seed;

    if (priv_key == NULL || src == NULL || src_size == 0 || dst_size == NULL ||
        ((dst == NULL) && (*dst_size != 0))) {
        PRINT_RET(ALI_CRYPTO_INVALID_ARG,
                  "Priv_decrypt: invalid input args!\n");
    }

    if (!IS_VALID_CTX_MAGIC(((sal_rsa_keypair_t *)priv_key)->magic)) {
        PRINT_RET(ALI_CRYPTO_INVALID_KEY, "Priv_decrypt: invalid priv_key!\n");
    }

    switch (padding.type) {
        case RSA_NOPAD: {
#if 0
            if (*dst_size < key_size) {
                *dst_size = key_size;
                ALI_CRYPTO_DBG_E("Priv_decrypt: short buffer(%d)\n", *dst_size);
                result = ALI_CRYPTO_SHORT_BUFFER;
                goto _out;
            }

            ret = rsa_exptmod(src, (ulong_t)src_size,
                              dst, (ulong_t *)dst_size, key.type, &key);
            if (ret != CRYPT_OK) {
                ALI_CRYPTO_DBG_E("Priv_decrypt: v1_5 decrypt fail(%d)\n", ret);
                result = ALI_CRYPTO_ERROR;
                goto _out;
            }
#endif
            PRINT_RET(ALI_CRYPTO_NOSUPPORT, "Pub_encrypt: NOPAD not support\n");
        }

        case RSAES_PKCS1_V1_5: {
            ali_algo_rsa_init(&ctx, ALI_ALGO_RSA_PKCS_V15, HASH_NONE);

            result = _sal_rsa_priv_decrypt_pre(&ctx, priv_key);
            if (result != ALI_CRYPTO_SUCCESS) {
                ali_algo_rsa_free(&ctx);
                PRINT_RET(result,
                          "Priv_decrypt: rsa_public_encrypt_pre fail(%08x)\n",
                          result);
            }

            key_size = ali_algo_mpi_size(&ctx.N);
            if (src_size != key_size) {
                GO_RET(ALI_CRYPTO_LENGTH_ERR,
                       "Priv_decrypt: invalid src size(%d)\n", (int)src_size);
            }
            ctx.len = key_size;

            tmp_dst_size = key_size;
            tmp_dst      = (uint8_t *)osa_malloc(tmp_dst_size);
            if (!tmp_dst) {
                GO_RET(ALI_CRYPTO_OUTOFMEM, "Priv_decrypt: malloc(0x%x) fail\n",
                       (int)tmp_dst_size);
            }

            seed = (uint32_t)ls_osa_get_time_ms();
            ret = ali_seed((uint8_t *)&seed, 4);
            if (0 != ret) {
                GO_RET(ALI_CRYPTO_ERROR, "Priv_decrypt: set seed failed %d\n", ret);
            }
            ret = ali_algo_rsa_pkcs1_decrypt(
              &ctx, myrand, (uint32_t *)&seed, ALI_ALGO_RSA_PRIVATE, &tmp_dst_size,
              (const unsigned char *)src, (unsigned char *)tmp_dst,
              (size_t)key_size);
            if (0 != ret) {
                GO_RET(ALI_CRYPTO_ERROR,
                       "Priv_decrypt: v1_5 decrypt fail(%d)\n", ret);
            }

            if (*dst_size < tmp_dst_size) {
                *dst_size = tmp_dst_size;
                GO_RET(ALI_CRYPTO_SHORT_BUFFER,
                       "Priv_decrypt: short buffer(%d)\n", (int)*dst_size);
            }

            osa_memcpy(dst, tmp_dst, tmp_dst_size);
            *dst_size = tmp_dst_size;
            break;
        }

        case RSAES_PKCS1_OAEP_MGF1: {
            hash_type_t ali_hash_type;

            hash_type = _sal_get_hash_type(padding.pad.rsaes_oaep.type);
            if (-1 == hash_type) {
                PRINT_RET(ALI_CRYPTO_INVALID_ARG,
                          "Pub_encrypt: invalid hash type!\n");
            }
            ali_algo_rsa_init(&ctx, ALI_ALGO_RSA_PKCS_V21, hash_type);

            result = _sal_rsa_priv_decrypt_pre(&ctx, priv_key);
            if (result != ALI_CRYPTO_SUCCESS) {
                ali_algo_rsa_free(&ctx);
                PRINT_RET(result,
                          "Pub_decrypt: rsa_public_encrypt_pre fail(%08x)\n",
                          result);
            }

            key_size = ali_algo_mpi_size(&ctx.N);
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
            tmp_dst      = (uint8_t *)osa_malloc(tmp_dst_size);
            if (!tmp_dst) {
                GO_RET(ALI_CRYPTO_OUTOFMEM, "Priv_decrypt: malloc(0x%x) fail\n",
                       (int)tmp_dst_size);
            }

            seed = (uint32_t)ls_osa_get_time_ms();
            ret = ali_seed((uint8_t *)&seed, 4);
            if (0 != ret) {
                GO_RET(ALI_CRYPTO_ERROR, "Priv_decrypt: set seed failed %d\n", ret);
            }
            ret = ali_algo_rsa_pkcs1_decrypt(
              &ctx, myrand,(uint32_t *)&seed, ALI_ALGO_RSA_PRIVATE, &tmp_dst_size,
              (const unsigned char *)src, (unsigned char *)tmp_dst,
              (size_t)key_size);
            if (0 != ret) {
                GO_RET(ALI_CRYPTO_ERROR,
                       "Priv_decrypt: v2_1 decrypt fail(%d)\n", ret);
            }

            if (*dst_size < tmp_dst_size) {
                *dst_size = tmp_dst_size;
                GO_RET(ALI_CRYPTO_ERROR, "Priv_decrypt: short buffer(%d)\n",
                       (int)*dst_size);
            }

            osa_memcpy(dst, tmp_dst, tmp_dst_size);
            *dst_size = tmp_dst_size;
            break;
        }

        default:
            ALI_CRYPTO_DBG_E("Priv_decrypt: no support type(%d)\n", padding.type);
            result = ALI_CRYPTO_NOSUPPORT;
    }

_OUT:
    if (NULL != tmp_dst) {
        osa_free(tmp_dst);
    }
    ali_algo_rsa_free(&ctx);

    return result;
}

ali_crypto_result sal_rsa_sign(const rsa_keypair_t *priv_key,
                               const uint8_t *dig, size_t dig_size,
                               uint8_t *sig, size_t *sig_size,
                               rsa_padding_t padding)
{
    int                 ret;
    uint32_t            key_size;
    uint32_t            hash_size;
    hash_type_t         ali_hash_type;
    int32_t             hash_type;
    ali_algo_rsa_context ctx;
    ali_crypto_result   result = ALI_CRYPTO_SUCCESS;
    uint32_t            seed;

    if (priv_key == NULL || dig == NULL || dig_size == 0 || sig == NULL ||
        sig_size == NULL) {
        PRINT_RET(ALI_CRYPTO_INVALID_ARG, "Rsa_sign: invalid input args!\n");
    }

    if (!IS_VALID_CTX_MAGIC(((sal_rsa_keypair_t *)priv_key)->magic)) {
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

    hash_size = _sal_get_hash_size(ali_hash_type);
    if (dig_size != hash_size) {
        PRINT_RET(ALI_CRYPTO_LENGTH_ERR,
                  "Rsa_sign: invalid dig size(%d vs %d)\n", (int)dig_size,
                  (int)hash_size);
    }

    hash_type = _sal_get_hash_type(ali_hash_type);
    if (-1 == hash_type) {
        PRINT_RET(ALI_CRYPTO_INVALID_ARG, "Rsa_sign: invalid hash type!\n");
    }

    if (padding.type == RSASSA_PKCS1_V1_5) {
        ali_algo_rsa_init(&ctx, ALI_ALGO_RSA_PKCS_V15, hash_type);
    } else {
        ali_algo_rsa_init(&ctx, ALI_ALGO_RSA_PKCS_V21, hash_type);
    }

    result = _sal_rsa_priv_decrypt_pre(&ctx, priv_key);

    if (0 != result) {
        ali_algo_rsa_free(&ctx);
        PRINT_RET(result, "Rsa_sign: ali_algo_ctr_drbg_seed fail(%08x)\n",
                  result);
    }

    key_size = ali_algo_mpi_size(&ctx.N);
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

    seed = (uint32_t)ls_osa_get_time_ms();
    ret = ali_seed((uint8_t *)&seed, 4);
    if (0 != ret) {
        GO_RET(ALI_CRYPTO_ERROR, "Rsa_sign: set seed failed %d\n", ret);
    }
    ret = ali_algo_rsa_pkcs1_sign(
      &ctx, myrand, (uint32_t *)&seed, ALI_ALGO_RSA_PRIVATE, (ali_algo_md_type_t)hash_type,
      (unsigned int)0, (const unsigned char *)dig, (unsigned char *)sig);
    if (0 != ret) {
        GO_RET(ALI_CRYPTO_ERROR, "Rsa_sign: ali_algo_rsa_pkcs1_sign fail %d\n",
               ret);
    }

_OUT:
    ali_algo_rsa_free(&ctx);
    return result;
}

ali_crypto_result sal_rsa_verify(const rsa_pubkey_t *pub_key,
                                 const uint8_t *dig, size_t dig_size,
                                 const uint8_t *sig, size_t sig_size,
                                 rsa_padding_t padding, bool *p_result)
{
    int                 ret;
    uint32_t            hash_size;
    uint32_t            key_size;
    hash_type_t         ali_hash_type;
    int32_t             hash_type;
    ali_algo_rsa_context ctx;
    ali_crypto_result   result = ALI_CRYPTO_SUCCESS;

    if (pub_key == NULL || NULL == p_result || dig == NULL || dig_size == 0 ||
        sig == NULL || sig_size == 0) {
        PRINT_RET(ALI_CRYPTO_INVALID_ARG, "Rsa_verify: invalid input args!\n");
    }

    if (!IS_VALID_CTX_MAGIC(((sal_rsa_pubkey_t *)pub_key)->magic)) {
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

    hash_size = _sal_get_hash_size(ali_hash_type);
    if (dig_size != hash_size) {
        *p_result = 1;
        PRINT_RET(ALI_CRYPTO_LENGTH_ERR,
                  "Rsa_verify: invalid dig size(%d vs %d)\n", (int)dig_size,
                  (int)hash_size);
    }

    hash_type = _sal_get_hash_type(ali_hash_type);
    if (-1 == hash_type) {
        *p_result = 0;
        PRINT_RET(ALI_CRYPTO_INVALID_ARG, "Rsa_verify: invalid hash type!\n");
    }

    if (padding.type == RSASSA_PKCS1_V1_5) {
        ali_algo_rsa_init(&ctx, ALI_ALGO_RSA_PKCS_V15, hash_type);
    } else {
        ali_algo_rsa_init(&ctx, ALI_ALGO_RSA_PKCS_V21, hash_type);
    }

    result = _sal_rsa_key_decode(PK_PUBLIC, (void *)pub_key, &ctx);
    if (ALI_CRYPTO_SUCCESS != result) {
        *p_result = 0;
        GO_RET(ALI_CRYPTO_INVALID_KEY,
               "Rsa_verify: rsa key decode fail(%08x)\n", result);
    }

    ret = ali_algo_rsa_check_pubkey(&ctx);
    if (0 != ret) {
        GO_RET(ALI_CRYPTO_INVALID_KEY, "Rsa_verify: rsa key invalid(%08x)\n",
               ret);
    }

    key_size = ali_algo_mpi_size(&ctx.N);
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

    ret = ali_algo_rsa_pkcs1_verify(
      &ctx, NULL, NULL, ALI_ALGO_RSA_PUBLIC, (ali_algo_md_type_t)hash_type,
      (unsigned int)0, (const unsigned char *)dig, (const unsigned char *)sig);
    if (0 != ret) {
        GO_RET(ALI_CRYPTO_ERROR,
               "Rsa_verify: ali_algo_rsa_pkcs1_verify fail %d\n", ret);
    }

_OUT:
    if (0 == result) {
        *p_result = 1;
    } else {
        *p_result = 0;
    }
    ali_algo_rsa_free(&ctx);

    return result;
}
