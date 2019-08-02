/**
 * Copyright (C) 2017  Alibaba Group Holding Limited.
 */

#include "ali_crypto.h"
#include "ls_hal_crypt.h"

ali_crypto_result ali_rsa_get_keypair_size(size_t keybits, size_t *size)
{
    if (size == NULL) {
        return ALI_CRYPTO_INVALID_ARG;
    }

    if (keybits < MIN_RSA_KEY_SIZE || keybits > MAX_RSA_KEY_SIZE) {
        CRYPTO_DBG_LOG("get_keypair_size: invalid keybits(%d)\n", (int)keybits);
        return ALI_CRYPTO_INVALID_ARG;
    }

    *size = sizeof(api_rsa_ctx_t) + ls_hal_rsa_get_ctx_size();
    return ALI_CRYPTO_SUCCESS;
}

ali_crypto_result ali_rsa_get_pubkey_size(size_t keybits, size_t *size)
{
    if (size == NULL) {
        return ALI_CRYPTO_INVALID_ARG;
    }

    if (keybits < MIN_RSA_KEY_SIZE || keybits > MAX_RSA_KEY_SIZE) {
        CRYPTO_DBG_LOG("get_pubkey_size: invalid keybits(%d)\n", (int)keybits);
        return ALI_CRYPTO_INVALID_ARG;
    }

    *size = sizeof(api_rsa_ctx_t) + ls_hal_rsa_get_ctx_size();
    return ALI_CRYPTO_SUCCESS;
}

ali_crypto_result ali_rsa_init_keypair(size_t keybits,
                    const uint8_t *n, size_t n_size,
                    const uint8_t *e, size_t e_size,
                    const uint8_t *d, size_t d_size,
                    const uint8_t *p, size_t p_size,
                    const uint8_t *q, size_t q_size,
                    const uint8_t *dp, size_t dp_size,
                    const uint8_t *dq, size_t dq_size,
                    const uint8_t *qp, size_t qp_size,
                    void *keypair_ctx)
{
    ls_hal_crypt_result ret = 0;
    api_rsa_ctx_t       *ctx;

    if ((n_size << 3) > keybits || (e_size << 3) > keybits ||
        (d_size << 3) > keybits || (p_size << 3) > (keybits >> 1) ||
        (q_size << 3) > (keybits >> 1) || (dp_size << 3) > (keybits >> 1) ||
        (dq_size << 3) > (keybits >> 1) || (qp_size << 3) > (keybits >> 1)) {
        CRYPTO_DBG_LOG("invalid args\n");
        return ALI_CRYPTO_INVALID_ARG;
    }
 
    if (keypair_ctx == NULL) {
        CRYPTO_DBG_LOG("invalid ctx\n");
        return ALI_CRYPTO_INVALID_CONTEXT;
    }

    ctx = (api_rsa_ctx_t *)keypair_ctx;
    ctx->hal_ctx = (char *)&(ctx->hal_ctx) + sizeof(ctx->hal_ctx);

    // init hal_ctx
    ret = ls_hal_rsa_init(ctx->hal_ctx);
    if (ret) {
        CRYPTO_DBG_LOG("hal_ctx init failed(0x%08x)\n", ret);
        return ALI_CRYPTO_ERROR;
    }

    ctx->keybytes = keybits >> 3;

    ret = ls_hal_rsa_init_keypair(ctx->hal_ctx, keybits,
                                  n, n_size,
                                  e, e_size,
                                  d, d_size,
                                  p, p_size,
                                  q, q_size,
                                  dp, dp_size,
                                  dq, dq_size,
                                  qp, qp_size);
    if (ret) {
        CRYPTO_DBG_LOG("init keypair failed(0x%08x)\n", ret);
        return ALI_CRYPTO_ERROR;
    }

    return ALI_CRYPTO_SUCCESS;
}

ali_crypto_result ali_rsa_init_pubkey(size_t keybits, const uint8_t *n,
                                      size_t n_size, const uint8_t *e,
                                      size_t e_size, void *pubkey)
{
    ls_hal_crypt_result ret = 0;
    api_rsa_ctx_t       *ctx;

    if (n == NULL || n_size == 0 || e == NULL || e_size == 0) {
        CRYPTO_DBG_LOG("invalid args!\n");
        return ALI_CRYPTO_INVALID_ARG;
    }

    if ((n_size << 3) > keybits || (e_size << 3) > keybits) {
        CRYPTO_DBG_LOG("key param size not match with key size\n");
        return ALI_CRYPTO_INVALID_ARG;
    }

    if (pubkey == NULL) {
        CRYPTO_DBG_LOG("invalid ctx\n");
        return ALI_CRYPTO_INVALID_CONTEXT;
    }

    ctx = (api_rsa_ctx_t *)pubkey;
    ctx->hal_ctx = (char *)&(ctx->hal_ctx) + sizeof(ctx->hal_ctx);

    // init hal_ctx
    ret = ls_hal_rsa_init(ctx->hal_ctx);
    if (ret) {
        CRYPTO_DBG_LOG("hal_ctx init failed(0x%08x)\n", ret);
        return ALI_CRYPTO_ERROR;
    }

    ctx->keybytes = keybits >> 3;
    ret = ls_hal_rsa_init_pubkey(ctx->hal_ctx, keybits,
                                 n, n_size,
                                 e, e_size);
    if (ret) {
        CRYPTO_DBG_LOG("init pubkey failed(0x%08x)\n", ret);
        return ALI_CRYPTO_ERROR;
    }

    return ALI_CRYPTO_SUCCESS;
}

ali_crypto_result ali_rsa_get_key_attr(rsa_key_attr_t attr,
                                       rsa_keypair_t *keypair, void *buffer,
                                       size_t *size)
{
    CRYPTO_DBG_LOG("not supported\n");
    return ALI_CRYPTO_NOSUPPORT;
}

ali_crypto_result ali_rsa_gen_keypair(size_t keybits, const uint8_t *e,
                                            size_t e_size, rsa_keypair_t *keypair)
{
    CRYPTO_DBG_LOG("not supported\n");
    return ALI_CRYPTO_NOSUPPORT;
}

#if defined(CONFIG_RSA_DECRYPT) || defined(RSA_PADDING_PKCS1_V15) || defined(RSA_PADDING_PKCS1_V21)
static int myrand(void *rng_state, unsigned char *output, size_t len)
{
    int ret;

    ret = ali_rand_gen(output, len);
    if (ret != ALI_CRYPTO_SUCCESS) {
        CRYPTO_DBG_LOG("gen rand failed(%08x)\n", ret);
        return ret;
    }

    return 0;
}
#endif

#ifdef CONFIG_RSA_ENCRYPT
#ifdef RSA_PADDING_PKCS1_V21
/**
 * Generate and apply the MGF1 operation (from PKCS#1 v2.1) to a buffer.
 *
 * \param dst       buffer to mask
 * \param dlen      length of destination buffer
 * \param src       source of the mask generation
 * \param slen      length of the source buffer
 * \param md_ctx    message digest context to use
 */
static ali_crypto_result mgf_mask(unsigned char *dst, size_t dlen,
                                  unsigned char *src, size_t slen,
                                  void *hash_ctx)
{
    ali_crypto_result ret = ALI_CRYPTO_SUCCESS;
    unsigned char  mask[MAX_HASH_SIZE];
    unsigned char  counter[4];
    unsigned char  *p;
    unsigned int   hlen;
    size_t         i, use_len;
    api_hash_ctx_t *ctx;
	hash_type_t    type;
 
    memset(mask, 0, MAX_HASH_SIZE);
    memset(counter, 0, 4);

    ctx = (api_hash_ctx_t *) hash_ctx;
    hlen = HASH_SIZE(ctx->type);
    type = ctx->type;
    /* Generate and apply dbMask */
    p = dst;

    while (dlen > 0) {
        use_len = hlen;
        if (dlen < hlen)
            use_len = dlen;

        ret = ali_hash_init(type, ctx);
        if (ret != ALI_CRYPTO_SUCCESS) {
            CRYPTO_DBG_LOG("hash init failed(0x%08x)\n", ret);
            return ret;
        }

        ret = ali_hash_update(src, slen, ctx);
        if (ret != ALI_CRYPTO_SUCCESS) {
            CRYPTO_DBG_LOG("hash update failed(0x%08x)\n", ret);
            return ret;
        }

        ret = ali_hash_update(counter, 4, ctx);
        if (ret != ALI_CRYPTO_SUCCESS) {
            CRYPTO_DBG_LOG("hash update failed(0x%08x)\n", ret);
            return ret;
        }

        ret = ali_hash_final(mask, ctx);
        if (ret != ALI_CRYPTO_SUCCESS) {
            CRYPTO_DBG_LOG("hash final failed(0x%08x)\n", ret);
            return ret;
        }

        for (i = 0; i < use_len; ++i)
            *p++ ^= mask[i];

        counter[3]++;

        dlen -= use_len;
    }

    return ret;
}

static int _pkcs_1_v2_1_encode(const rsa_pubkey_t *pubkey,
                               int (*f_rng)(void *, unsigned char *, size_t),
                               void *p_rng, 
                               const uint8_t *label, size_t label_len, 
                               size_t ilen, const uint8_t *input, 
                               size_t *olen, uint8_t *output,
                               hash_type_t type)
{
    ali_crypto_result        ret = ALI_CRYPTO_SUCCESS;
    uint8_t *                p = output;
    unsigned int             hlen;
    void                     *ctx;
    size_t                   ctx_size;
    api_rsa_ctx_t            *rsa_ctx;

    rsa_ctx = (api_rsa_ctx_t *)pubkey;

    if (f_rng == NULL || input == NULL || output == NULL) {
        CRYPTO_DBG_LOG("invalid arg\n");
        return ALI_CRYPTO_INVALID_ARG;
    }

    *olen = rsa_ctx->keybytes;
    hlen = HASH_SIZE(type);

    /* first comparison checks for overflow */
    if (ilen + 2 * hlen + 2 < ilen || *olen < ilen + 2 * hlen + 2) {
        CRYPTO_DBG_LOG("invalid ilen(%d)/hlen(%d)/olen(%d)\n",
            ilen, hlen, *olen);
        return ALI_CRYPTO_INVALID_ARG;
    }

    memset(output, 0, *olen);

    *p++ = 0;

    /* Generate a random octet string seed */
    if ((ret = f_rng(p_rng, p, hlen)) != 0)
        return ret;

    p += hlen;

    /* Construct DB */
    ret = ali_hash_digest(type, label, label_len, p);
    if (ret != ALI_CRYPTO_SUCCESS) {
        CRYPTO_DBG_LOG("digest failed(0x%08x)\n", ret);
        return ret;
    }

    p += hlen;
    p += *olen - 2 * hlen - 2 - ilen;
    *p++ = 1;
    memcpy(p, input, ilen);
   
    ret = ali_hash_get_ctx_size(type, &ctx_size);
    if (ret != ALI_CRYPTO_SUCCESS) {
        CRYPTO_DBG_LOG("get ctx size failed(%08x)\n", ret);
        return ret;
    }

    ctx = ls_osa_malloc(ctx_size);
    if (ctx == NULL) {
        CRYPTO_DBG_LOG("malloc(%d) failed\n", (int)ctx_size);
        return ALI_CRYPTO_ERROR;
    }
    memset(ctx, 0, ctx_size);

    ret = ali_hash_init(type, ctx);
    if (ALI_CRYPTO_SUCCESS != ret) {
       CRYPTO_DBG_LOG("ali_hash_init failed(0x%08x)\n", ret);
       goto _cleanup;
    }

    /* maskedDB: Apply dbMask to DB */
    ret = mgf_mask(output + hlen + 1, *olen - hlen - 1, output + 1, hlen, ctx);
    if (ALI_CRYPTO_SUCCESS != ret) {
       CRYPTO_DBG_LOG("mgf_mask failed(0x%08x)\n", ret);
       goto _cleanup;
    }

    /* maskedSeed: Apply seedMask to seed */
    ret = mgf_mask(output + 1, hlen, output + hlen + 1, *olen - hlen - 1, ctx);
    if (ALI_CRYPTO_SUCCESS != ret) {
       CRYPTO_DBG_LOG("mgf_mask failed(0x%08x)\n", ret);
       goto _cleanup;
    }

_cleanup:
	if (ctx != NULL)
        ls_osa_free(ctx);
    return ret; 
}
#endif /* RSA_PADDING_PKCS1_V21 */

#ifdef RSA_PADDING_PKCS1_V15
/*
 * Implementation of the PKCS#1 v2.1 RSAES-PKCS1-V1_5-ENCRYPT function
 */
static int _pkcs_1_v1_5_encode(const rsa_pubkey_t *pubkey,
                     int (*f_rng)(void *, unsigned char *, size_t),
                     void *p_rng,
                     size_t ilen, const uint8_t *input,
                     size_t *olen, uint8_t *output, 
                     size_t keybytes)
{
    size_t             nb_pad;
    ali_crypto_result  ret = ALI_CRYPTO_SUCCESS;
    unsigned char      *p = output;
    int                rng_dl;

    // We don't check p_rng because it won't be dereferenced here
    if (f_rng == NULL || input == NULL || output == NULL) {
        CRYPTO_DBG_LOG("invalid arg\n");
        return ALI_CRYPTO_INVALID_ARG;
    }
    
    *olen = keybytes;

    /* first comparison checks for overflow */
    if (*olen < ilen + 11) {
        CRYPTO_DBG_LOG("invalid olen(%d)/ilen(%d)\n", *olen, ilen);
        return ALI_CRYPTO_INVALID_ARG;
    }

    nb_pad = *olen - 3 - ilen;

    *p++ = 0;
    *p++ = ALI_CRYPTO_RSA_CRYPT;

    while (nb_pad-- > 0) {
        rng_dl = 100;
        /* transform zero bytes(if any) to non-zero random bytes */
        do {
            ret = f_rng(p_rng, p, 1);
        } while (*p == 0 && --rng_dl && ret == 0);

        /* Check if RNG failed to generate data */
        if (rng_dl == 0 || ret != 0) {
            CRYPTO_DBG_LOG("failed ret(0x%08x) rng_dl(%d)\n", ret, rng_dl);
            return ALI_CRYPTO_ERROR;
        }
        p++;
    }

    *p++ = 0;
    memcpy(p, input, ilen);

    return ret; 
}
#endif

ali_crypto_result ali_rsa_public_encrypt(const rsa_pubkey_t *pubkey,
                                         const uint8_t *src, size_t src_size,
                                         uint8_t *dst, size_t *dst_size,
                                         rsa_padding_t padding)
{
    int                ret = 0;
    size_t             keybytes;
    uint32_t           seed;
    api_rsa_ctx_t      *ctx;

    if (pubkey == NULL) {
        CRYPTO_DBG_LOG("invalid ctx\n");
        return ALI_CRYPTO_INVALID_CONTEXT;
    }
    
    if (src == NULL || src_size == 0 || dst_size == NULL ||
            ((dst == NULL) && (*dst_size != 0))) {
        CRYPTO_DBG_LOG("invalid input args!\n");
        return ALI_CRYPTO_INVALID_ARG;
    }

    ctx = (api_rsa_ctx_t *)pubkey;
    ctx->hal_ctx = (char *)&(ctx->hal_ctx) + sizeof(ctx->hal_ctx);
    keybytes = ctx->keybytes;

    if (*dst_size < keybytes) {
        CRYPTO_DBG_LOG("short buffer(%d) < %d\n", (int)*dst_size, keybytes);
        *dst_size = keybytes;
        return ALI_CRYPTO_SHORT_BUFFER;
    }

    switch (padding.type) {
#ifdef RSA_PADDING_PKCS1_V15
        case RSAES_PKCS1_V1_5: {
            // pkcs1_v1_5 padding encode
            seed = (uint32_t)ls_osa_get_time_ms();
            ret = _pkcs_1_v1_5_encode(pubkey, myrand, (uint32_t *)&seed, 
                                      src_size, src, 
                                      dst_size, dst, 
                                      keybytes);
            if (ret) {
                CRYPTO_DBG_LOG("pkcs encode failed(0x%08x)\n", ret);
                return ALI_CRYPTO_ERROR;
            }

            ret = ls_hal_rsa_public(ctx->hal_ctx, dst, dst, *dst_size);
            if (ret) {
                CRYPTO_DBG_LOG("hal_rsa_public failed(0x%08x)\n", ret);
                return ALI_CRYPTO_ERROR;
            }
            break;
        }
#endif
#ifdef RSA_PADDING_PKCS1_V21
        case RSAES_PKCS1_OAEP_MGF1: {
            hash_type_t ali_hash_type;
            ali_hash_type = padding.pad.rsaes_oaep.type;
            if (2 * HASH_SIZE(ali_hash_type) >= keybytes - 2 ||
                src_size > keybytes - 2 * HASH_SIZE(ali_hash_type) - 2) {
                CRYPTO_DBG_LOG("invalid src size(%d %d) for rsa_%d\n",
                       (int)src_size, HASH_SIZE(ali_hash_type),
                       (int)keybytes << 3);
                return ALI_CRYPTO_LENGTH_ERR;
            }

            seed = (uint32_t)ls_osa_get_time_ms();
            ret = _pkcs_1_v2_1_encode(pubkey, 
                                      myrand, (uint32_t *)&seed, 
                                      NULL, 0, 
                                      src_size, src, 
                                      dst_size, dst, 
                                      ali_hash_type);
            if (ret) {
                CRYPTO_DBG_LOG("encrypt failed(0x%08x)\n", ret);
                return ALI_CRYPTO_ERROR;
            }

            ret = ls_hal_rsa_public(ctx->hal_ctx, dst, dst, *dst_size);
            if (ret) {
                CRYPTO_DBG_LOG("_rsa_public failed(0x%08x)\n", ret);
                ret = ALI_CRYPTO_ERROR;
            }
            break;
        }
#endif
        default:
            CRYPTO_DBG_LOG("invalid padding type(%d)\n", padding.type);
            ret = ALI_CRYPTO_NOSUPPORT;
            break;
    }
    return ret;
}
#endif /* CONFIG_RSA_ENCRYPT */

#if defined(RSA_PADDING_PKCS1_V15) || defined(RSA_PADDING_PKCS1_V21)
static int _rsa_private(const rsa_keypair_t *privkey,
                        int (*f_rng)(void *, uint8_t *, size_t),
                        const uint8_t *input,
                        uint8_t *output, size_t *olen,
                        rsa_padding_t padding)
{
    api_rsa_ctx_t       *ctx;
    ali_crypto_result   ret = 0;

    if (privkey == NULL) {
        CRYPTO_DBG_LOG("invalid ctx\n");
        return ALI_CRYPTO_INVALID_CONTEXT;
    }

    ctx = (api_rsa_ctx_t *)privkey;
    ctx->hal_ctx = (char *)&(ctx->hal_ctx) + sizeof(ctx->hal_ctx);
    *olen = ctx->keybytes;

    ret = ls_hal_rsa_private(ctx->hal_ctx, f_rng, input, output, *olen);
    if (ret) {
        CRYPTO_DBG_LOG("failed(0x%08x)\n", ret);
        ret = ALI_CRYPTO_ERROR;
    }

	return ret;
}
#endif

#ifdef CONFIG_RSA_DECRYPT
#ifdef RSA_PADDING_PKCS1_V15
static int _pkcs_1_v1_5_decode(const rsa_keypair_t *privkey, 
                               int (*f_rng)(void *, unsigned char *, size_t),
                               void *p_rng, int mode, 
                               size_t ilen,  uint8_t *input,
                               size_t *olen, uint8_t *output, 
                               rsa_padding_t padding, size_t keybytes)
{
    size_t             pad_count = 0, i;
    ali_crypto_result  ret = ALI_CRYPTO_SUCCESS;
    unsigned char      bad, pad_done = 0, *p;

    p   = input;
    bad = 0;
    /*
     * Check and get padding len in "constant-time"
     */
    bad |= *p++; /* First byte must be 0 */
    /* This test does not depend on secret data */
    bad |= *p++ ^ ALI_CRYPTO_RSA_CRYPT;

    /* Get padding len, but always read till end of buffer
     * (minus one, for the 00 byte) */
    for (i = 0; i < ilen - 3; i++) {
        pad_done |= ((p[i] | (unsigned char)-p[i]) >> 7) ^ 1;
        pad_count += ((pad_done | (unsigned char)-pad_done) >> 7) ^ 1;
    }

    p += pad_count;
    bad |= *p++; /* Must be zero */

    bad |= (pad_count < 8);

    if (bad) {
        CRYPTO_DBG_LOG("non-zero bad(0x%08x)!\n");
        return ALI_CRYPTO_INVALID_PADDING;
    }

    if (ilen - (p - input) > keybytes) {
        CRYPTO_DBG_LOG("length err ilen(%d) keybytes(%d) p-input(%d)\n",
            ilen, keybytes, (p-input));
        return ALI_CRYPTO_LENGTH_ERR;
    }

    *olen = ilen - (p - input);
    memcpy(output, p, *olen);

    return ret;
}
#endif /* RSA_PADDING_PKCS1_V15 */

#ifdef RSA_PADDING_PKCS1_V21
static int _pkcs_1_v2_1_decode(const  rsa_keypair_t *privkey,
                               int (*f_rng)(void *, unsigned char *, size_t),
                               void *p_rng, int mode,
                               const unsigned char *label, size_t label_len,
                               size_t ilen, uint8_t *input,
                               size_t *olen, unsigned char *output,
                               rsa_padding_t padding, size_t keybytes,
                               hash_type_t hash_type)
{
    ali_crypto_result        ret;
    size_t                   i, pad_len, hlen;
    unsigned char *          p, bad, pad_done;
    unsigned char            lhash[MAX_HASH_SIZE];
    void                     *ctx;
    size_t                   ctx_size;

    hlen = HASH_SIZE(hash_type);
    // checking for integer underflow
    if (2 * hlen + 2 > ilen) {
        CRYPTO_DBG_LOG("invalid hlen(%d)/ilen(%d)\n", hlen, ilen);
        return ALI_CRYPTO_INVALID_ARG;
    }
    /*
     * Unmask data and generate lHash
     */
    ret = ali_hash_get_ctx_size(hash_type, &ctx_size);
    if (ret != ALI_CRYPTO_SUCCESS) {
        CRYPTO_DBG_LOG("get ctx size fail(%08x)\n", ret);
    }

    ctx = ls_osa_malloc(ctx_size);
    if (ctx == NULL) {
        CRYPTO_DBG_LOG("malloc(%d) fail\n", ctx_size);
        return ALI_CRYPTO_OUTOFMEM;
    }
    memset(ctx, 0, ctx_size);

    ret = ali_hash_init(hash_type, ctx);
    if ( ALI_CRYPTO_SUCCESS != ret) {
       CRYPTO_DBG_LOG("failed(0x%08x)\n", ret);
       goto _cleanup;
    }

    /* Generate lHash */
    ret = ali_hash_digest(hash_type, label, label_len, lhash);
    if( ALI_CRYPTO_SUCCESS != ret) {
       CRYPTO_DBG_LOG("failed(0x%08x)\n", ret);
       goto _cleanup;
    }

    /* seed: Apply seedMask to maskedSeed */
    ret = mgf_mask(input + 1, hlen, input + hlen + 1, ilen - hlen - 1, ctx);
    if (ALI_CRYPTO_SUCCESS != ret) {
       CRYPTO_DBG_LOG("mgf_mask failed(0x%08x)\n", ret);
       goto _cleanup;
    }

    /* DB: Apply dbMask to maskedDB */
    ret = mgf_mask(input + hlen + 1, ilen - hlen - 1, input + 1, hlen, ctx);
    if (ALI_CRYPTO_SUCCESS != ret) {
       CRYPTO_DBG_LOG("mgf_mask failed(0x%08x)\n", ret);
       goto _cleanup;
    }

    ali_hash_reset(ctx);

    /*
     * Check contents, in "constant-time"
     */
    p   = input;
    bad = 0;

    bad |= *p++; /* First byte must be 0 */

    p += hlen; /* Skip seed */

    /* Check lHash */
    for (i = 0; i < hlen; i++)
        bad |= lhash[i] ^ *p++;

    /* Get zero-padding len, but always read till end of buffer
     * (minus one, for the 01 byte) */
    pad_len  = 0;
    pad_done = 0;
    for (i = 0; i < ilen - 2 * hlen - 2; i++) {
        pad_done |= p[i];
        pad_len += ((pad_done | (unsigned char)-pad_done) >> 7) ^ 1;
    }

    p += pad_len;
    bad |= *p++ ^ 0x01;

    /*
     * The only information "leaked" is whether the padding was correct or not
     * (eg, no data is copied if it was not correct). This meets the
     * recommendations in PKCS#1 v2.2: an opponent cannot distinguish between
     * the different error conditions.
     */
    if (bad != 0) {
        CRYPTO_DBG_LOG("wrong padding\n");
        ret = ALI_CRYPTO_INVALID_PADDING;
        goto _cleanup;
    }

    if (ilen - (p - input) > keybytes) {
        CRYPTO_DBG_LOG("wrong length\n");
        ret = ALI_CRYPTO_LENGTH_ERR;
        goto _cleanup;
    }

    *olen = ilen - (p - input);
    memcpy(output, p, *olen);
_cleanup:
	if (ctx != NULL)
        ls_osa_free(ctx);

    return ret; 

}
#endif /* RSA_PADDING_PKCS1_V21 */

static int _rsa_pkcs1_decrypt(const rsa_keypair_t *privkey, 
                int(*f_rng)(void *, unsigned char *, size_t),
                void *p_rng, int mode, size_t *olen, 
                const unsigned char *input, unsigned char *output, 
                rsa_padding_t padding, size_t keybytes)
{
    hash_type_t hash_type;
    ali_crypto_result ret = ALI_CRYPTO_SUCCESS;
    uint8_t *tmp_buf = NULL;
    uint32_t tmp_len = (MAX_RSA_KEY_SIZE >> 3);
    size_t ilen;

    switch (padding.type) {
#ifdef RSA_PADDING_PKCS1_V15
        case RSAES_PKCS1_V1_5:
            if (f_rng == NULL || p_rng == NULL 
                || input == NULL || output == NULL) {
                CRYPTO_DBG_LOG("invalid args\n");
                return ALI_CRYPTO_INVALID_ARG;
            }

            tmp_buf = ls_osa_malloc(tmp_len);
            if (tmp_buf == NULL) {
                CRYPTO_DBG_LOG("malloc(%d) failed\n", keybytes);
                return ALI_CRYPTO_OUTOFMEM;
            }

            ilen = keybytes;
            if (ilen < 16 || ilen > tmp_len) {
                CRYPTO_DBG_LOG("invalid ilen(%d)\n", ilen);
                ret = ALI_CRYPTO_INVALID_ARG;
                break;
            }

            ret = _rsa_private(privkey, f_rng, input, tmp_buf, &ilen, padding);
            if (ret != 0) {
                CRYPTO_DBG_LOG("_rsa_private failed(0x%08x)\n", ret);
                ret = ALI_CRYPTO_ERROR;
                break;
            }

            // pkcs v1_5 decode
            ret = _pkcs_1_v1_5_decode(privkey,
                                      f_rng, p_rng, mode,
                                      ilen, tmp_buf,
                                      olen, output,
                                      padding, keybytes);
            if (ret) {
                CRYPTO_DBG_LOG("rsa pkcs1 dec failed(0x%08x)\n", ret);
                ret = ALI_CRYPTO_ERROR;
            }
            break;
#endif
#ifdef RSA_PADDING_PKCS1_V21
        case RSAES_PKCS1_OAEP_MGF1:
            hash_type = padding.pad.rsaes_oaep.type;
            if (hash_type == SHA512 && keybytes == 128) {
                CRYPTO_DBG_LOG("rsa_%d not support hash(%d)\n",
                       (int)keybytes << 3, hash_type);
                return ALI_CRYPTO_NOSUPPORT;
            }

            if (f_rng == NULL || p_rng == NULL ||
                input == NULL || output == NULL) {
                CRYPTO_DBG_LOG("invalid args\n");
                return ALI_CRYPTO_INVALID_ARG;
            }

            tmp_buf = ls_osa_malloc(tmp_len);
            if (tmp_buf == NULL) {
                CRYPTO_DBG_LOG("malloc(%d) failed\n", keybytes);
                return ALI_CRYPTO_OUTOFMEM;
            }

            ilen = keybytes;
            if (ilen < 16 || ilen > tmp_len) {
                CRYPTO_DBG_LOG("invalid ilen(%d)\n", ilen);
                ret = ALI_CRYPTO_INVALID_ARG;
                break;
            }

            ret = _rsa_private(privkey, f_rng, input, tmp_buf, &ilen, padding);
            if (ret != ALI_CRYPTO_SUCCESS) {
                CRYPTO_DBG_LOG("fail(%08x)\n", ret);
                break;
            }

            /* *dst_size max is key_size - 2*HASH_SIZE(hash_type) - 2 */
            ret =  _pkcs_1_v2_1_decode(privkey,
                                       f_rng, p_rng, mode,
                                       NULL, 0,
                                       ilen, tmp_buf,
                                       olen, output,
                                       padding, keybytes,
                                       hash_type);
            if (ret) {
                CRYPTO_DBG_LOG("rsa oaep dec failed(0x%08x)\n", ret);
                ret = ALI_CRYPTO_ERROR;
            }
            break;
#endif
        default:
            CRYPTO_DBG_LOG("rsa padding type(0x%d) not supported\n", ret);
            ret = ALI_CRYPTO_NOSUPPORT;
            break;
    }

    if (tmp_buf) {
        ls_osa_free(tmp_buf);
        tmp_buf = NULL;
    }
    return ret;
}

ali_crypto_result ali_rsa_private_decrypt(const rsa_keypair_t *privkey,
                                          const uint8_t *src, size_t src_size,
                                          uint8_t *dst, size_t *dst_size,
                                          rsa_padding_t padding)
{
    ali_crypto_result   ret = 0;
    uint8_t             *tmp_dst;
	size_t              keybytes, tmp_dst_size;
	uint32_t            seed;
    api_rsa_ctx_t       *ctx;

    if (privkey == NULL) {
        CRYPTO_DBG_LOG("invalid ctx\n");
        return ALI_CRYPTO_INVALID_CONTEXT;
    }

    if (src == NULL || src_size == 0 || dst_size == NULL ||
        ((dst == NULL) && (*dst_size != 0))) {
        CRYPTO_DBG_LOG("invalid input args!\n");
        return ALI_CRYPTO_INVALID_ARG;
    }

    ctx = (api_rsa_ctx_t *)privkey;
    keybytes = ctx->keybytes;

    if (src_size != keybytes) {
        CRYPTO_DBG_LOG("invalid src_size(%d) keybytes(%d)\n",
            (int)src_size, keybytes);
        return ALI_CRYPTO_LENGTH_ERR;
    }
    tmp_dst_size = keybytes;
    tmp_dst = (uint8_t *)ls_osa_malloc(tmp_dst_size);
    if (!tmp_dst) {
        CRYPTO_DBG_LOG("malloc(0x%x) fail\n", (int)tmp_dst_size);
        return ALI_CRYPTO_OUTOFMEM;
    }

    seed = (uint32_t)ls_osa_get_time_ms();
    ret = ali_seed((uint8_t *)&seed, 4);
    if (0 != ret) {
        CRYPTO_DBG_LOG("fail(%d)\n", ret);
        goto _cleanup;
    }
    ret = _rsa_pkcs1_decrypt(ctx, myrand, (uint32_t *)&seed,
                             ALI_CRYPTO_RSA_PRIVATE, &tmp_dst_size,
                             src, tmp_dst,
                             padding, keybytes);
    if (0 != ret) {
        CRYPTO_DBG_LOG("fail(%d)\n", ret);
        goto _cleanup;
    }
    if (*dst_size < tmp_dst_size) {
        *dst_size = tmp_dst_size;
        CRYPTO_DBG_LOG("short buffer(%d)\n", (int)*dst_size);
        ret = ALI_CRYPTO_SHORT_BUFFER;
        goto _cleanup;
    }
    memcpy(dst, tmp_dst, tmp_dst_size);
    *dst_size = tmp_dst_size;

_cleanup:
    if (NULL != tmp_dst) {
        ls_osa_free(tmp_dst);
    }
    return ret;
}
#endif /* CONFIG_RSA_DECRYPT */

#ifdef CONFIG_RSA_SIGN
  
#ifdef RSA_PADDING_PKCS1_V15
/*
 * Implementation of the PKCS#1 v2.1 RSASSA-PKCS1-V1_5-SIGN function
 */

static int _pkcs1_v1_5_sign(hash_type_t   hash_type,
                            uint8_t       *sig,
                            const uint8_t *hash,
                            uint32_t      hashlen,
                            size_t        keybytes)
{
    size_t                 nb_pad, olen, oid_size = 0;
    unsigned char *        p       = sig;
    const char *           oid     = NULL;
    if (hash == NULL || sig == NULL) {
        CRYPTO_DBG_LOG("invalid args\n");
        return ALI_CRYPTO_INVALID_ARG;
    }

    olen   = keybytes;
    nb_pad = olen - 3;

    if (hash_type != HASH_NONE) {
        nb_pad -= 10 + oid_size;
        hashlen = HASH_SIZE(hash_type);
    }

    nb_pad -= hashlen;

    if ((nb_pad < 8) || (nb_pad > olen)) {
        CRYPTO_DBG_LOG("invalid keybytes(%d)\n", keybytes);
        return ALI_CRYPTO_INVALID_KEY;
    }

    *p++ = 0;
    *p++ = ALI_CRYPTO_RSA_SIGN;
    memset(p, 0xFF, nb_pad);
    p += nb_pad;
    *p++ = 0;

    if (hash_type == HASH_NONE) {
        memcpy(p, hash, hashlen);
    } else {
        /*
         * DigestInfo ::= SEQUENCE {
         *   digestAlgorithm DigestAlgorithmIdentifier,
         *   digest Digest }
         *
         * DigestAlgorithmIdentifier ::= AlgorithmIdentifier
         *
         * Digest ::= OCTET STRING
         */
        *p++ = ALI_CRYPTO_ASN1_SEQUENCE | ALI_CRYPTO_ASN1_CONSTRUCTED;
        *p++ = (unsigned char)(0x08 + oid_size + hashlen);
        *p++ = ALI_CRYPTO_ASN1_SEQUENCE | ALI_CRYPTO_ASN1_CONSTRUCTED;
        *p++ = (unsigned char)(0x04 + oid_size);
        *p++ = ALI_CRYPTO_ASN1_OID;
        *p++ = oid_size & 0xFF;
        memcpy(p, oid, oid_size);
        p += oid_size;
        *p++ = ALI_CRYPTO_ASN1_NULL;
        *p++ = 0x00;
        *p++ = ALI_CRYPTO_ASN1_OCTET_STRING;
        *p++ = hashlen;
        memcpy(p, hash, hashlen);
    }

    return ALI_CRYPTO_SUCCESS;
}
#endif /* RSA_PADDING_PKCS1_V15 */

/*
 * Do an RSA operation to sign the message digest
 */

#ifdef RSA_PADDING_PKCS1_V21

static int _ssa_pss_sign(int (*f_rng)(void *, unsigned char *, size_t),
                         void *p_rng,
                         uint8_t       *sig,
                         hash_type_t   hash_type,
                         const uint8_t *hash,
                         uint32_t      hashlen,
                         size_t        keybytes)
{
    size_t                   olen;
    unsigned char *          p = sig;
    unsigned char            salt[MAX_HASH_SIZE];
    unsigned int             slen, hlen, offset = 0;
    ali_crypto_result        ret = 0;
    size_t                   msb, ctx_size;
    void                     *ctx;

    olen = keybytes;

    if (hash_type != HASH_NONE) {
        /* Gather length of hash to sign */
        hashlen = HASH_SIZE(hash_type);
    }

    hlen = HASH_SIZE(hash_type);
    slen = hlen;

    if (olen < hlen + slen + 2) {
        CRYPTO_DBG_LOG("invalid olen(%d) hlen(%d), slen(%d)\n", 
                olen, hlen, slen);
        return ALI_CRYPTO_INVALID_ARG;
    }

    memset(sig, 0, olen);

    /* Generate salt of length slen */
    if ((ret = f_rng(p_rng, salt, slen)) != 0) {
        CRYPTO_DBG_LOG("rng failed(0x%08x)\n", ret);
        return ALI_CRYPTO_ERROR;
    }

    /* Note: EMSA-PSS encoding is over the length of N - 1 bits */
    msb = (keybytes << 3) - 1;
    p += olen - hlen * 2 - 2;
    *p++ = 0x01;
    memcpy(p, salt, slen);
    p += slen;

    ret = ali_hash_get_ctx_size(hash_type, &ctx_size);
    if (ret != ALI_CRYPTO_SUCCESS) {
        CRYPTO_DBG_LOG("get ctx size failed(%08x)\n", ret);
        return ret;
    }
    ctx = ls_osa_malloc(ctx_size);
    if (ctx == NULL) {
        CRYPTO_DBG_LOG("malloc(%d) failed\n", (int)ctx_size);
        return ALI_CRYPTO_OUTOFMEM;
    }
    memset(ctx, 0, ctx_size);

    ret = ali_hash_init(hash_type, ctx);
    if (ALI_CRYPTO_SUCCESS != ret) {
        CRYPTO_DBG_LOG("hash_init failed(0x%08x)\n", ret);
        goto _cleanup;
    }

    /* Generate H = Hash( M' ) */
    ret = ali_hash_update(p, 8, ctx);
    if (ALI_CRYPTO_SUCCESS != ret) {
        CRYPTO_DBG_LOG("hash_update failed(0x%08x)\n", ret);
        goto _cleanup;
    }

    ret = ali_hash_update(hash, hashlen, ctx);
    if (ALI_CRYPTO_SUCCESS != ret) {
        CRYPTO_DBG_LOG("hash_update failed(0x%08x)\n", ret);
        goto _cleanup;
    }

    ret = ali_hash_update(salt, slen, ctx);
    if (ALI_CRYPTO_SUCCESS != ret) {
        CRYPTO_DBG_LOG("hash_update failed(0x%08x)\n", ret);
        goto _cleanup;
    }

    ret = ali_hash_final(p, ctx);
    if (ALI_CRYPTO_SUCCESS != ret) {
        CRYPTO_DBG_LOG("hash_update failed(0x%08x)\n", ret);
        goto _cleanup;
    }

    /* Compensate for boundary condition when applying mask */
    if (msb % 8 == 0)
        offset = 1;

    /* maskedDB: Apply dbMask to DB */
    ret = mgf_mask(sig + offset, olen - hlen - 1 - offset, p, hlen, ctx);
    if (ALI_CRYPTO_SUCCESS != ret) {
       CRYPTO_DBG_LOG("mgf_mask failed(0x%08x)\n", ret);
       goto _cleanup;
    }

    ali_hash_reset(ctx);

    msb = (keybytes << 3) - 1;
    sig[0] &= 0xFF >> (olen * 8 - msb);

    p += hlen;
    *p++ = 0xBC;

_cleanup:
    if (ctx != NULL)
        ls_osa_free(ctx);

    return ret;
}

#endif /* RSA_PADDING_PKCS1_V21 */

/*
 * Do an RSA operation to sign the message digest
 */
static int _rsa_pkcs1_sign(const rsa_keypair_t *privkey,
                           int (*f_rng)(void *, unsigned char *, size_t), void *p_rng,
                           hash_type_t hash_type, unsigned int hashlen,
                           const uint8_t *hash, uint8_t *sig,
                           rsa_padding_t padding, size_t *keybytes)
{
    ali_crypto_result ret = 0;

    if (f_rng == NULL || p_rng == NULL || hash == NULL || sig == NULL) {
        CRYPTO_DBG_LOG("invalid args\n");
        return ALI_CRYPTO_INVALID_ARG;
    }

    switch (padding.type) {
#ifdef RSA_PADDING_PKCS1_V15
        case RSASSA_PKCS1_V1_5:
            ret = _pkcs1_v1_5_sign(hash_type, sig, hash, hashlen, *keybytes);
            if (ret) {
                CRYPTO_DBG_LOG("_pkcs1_v1_5_sign failed(0x%08x)\n", ret);
                ret = ALI_CRYPTO_ERROR;
                break;
            }

            ret = _rsa_private(privkey, f_rng, sig, sig, keybytes, padding);
            if (ret) {
                CRYPTO_DBG_LOG("_rsa_private failed(0x%08x)\n", ret);
                ret = ALI_CRYPTO_ERROR;
            }
            break;
#endif
#ifdef RSA_PADDING_PKCS1_V21
        case RSASSA_PKCS1_PSS_MGF1:
            ret = _ssa_pss_sign(f_rng, p_rng, sig, hash_type, hash, hashlen, *keybytes);
            if (ret) {
                CRYPTO_DBG_LOG("_ssa_pss_sign failed(0x%08x)\n", ret);
                ret = ALI_CRYPTO_ERROR;
                break;
            }

            ret = _rsa_private(privkey, f_rng, sig, sig, keybytes, padding);
            if (ret) {
                CRYPTO_DBG_LOG("_rsa_private failed(0x%08x)\n", ret);
                ret = ALI_CRYPTO_ERROR;
            }
            break;
#endif
        default:
            return ALI_CRYPTO_INVALID_PADDING;
    }

    return ret;
}

ali_crypto_result ali_rsa_sign(const rsa_keypair_t *privkey,
                               const uint8_t *dig, size_t dig_size,
                               uint8_t *sig, size_t *sig_size,
                               rsa_padding_t padding)
{
    api_rsa_ctx_t       *ctx;
    hash_type_t         ali_hash_type;
    uint32_t            hash_size;
	size_t              keybytes;
	uint32_t            seed;
	int                 ret = 0;

    if (privkey == NULL) {
        CRYPTO_DBG_LOG("invalid ctx\n");
        return ALI_CRYPTO_INVALID_CONTEXT;
    }

    if (dig == NULL || dig_size == 0 || sig == NULL || sig_size == NULL) {
        CRYPTO_DBG_LOG("invalid input args!\n");
        return ALI_CRYPTO_INVALID_ARG;
    }

    ctx = (api_rsa_ctx_t *)privkey;
    keybytes = ctx->keybytes; 

	if (*sig_size < keybytes) {
	    *sig_size = keybytes;
	    CRYPTO_DBG_LOG("short buffer(%d)\n", (int)*sig_size);
        return ALI_CRYPTO_SHORT_BUFFER;
	}

    if (padding.type == RSASSA_PKCS1_V1_5) {
        ali_hash_type = padding.pad.rsassa_v1_5.type;
        hash_size = HASH_SIZE(ali_hash_type);
        if (dig_size + 11 > keybytes) {
            CRYPTO_DBG_LOG("invalid dig_size(%d)\n", (int)dig_size);
            return ALI_CRYPTO_LENGTH_ERR;
        }
    } else if (padding.type == RSASSA_PKCS1_PSS_MGF1) {
        ali_hash_type = padding.pad.rsassa_pss.type;
        hash_size = HASH_SIZE(ali_hash_type);
        if (keybytes < padding.pad.rsassa_pss.salt_len + hash_size + 2) {
            CRYPTO_DBG_LOG("invalid salt size(%d) for rsa_%d\n",
                   (int)padding.pad.rsassa_pss.salt_len, (int)keybytes << 3);
            return ALI_CRYPTO_LENGTH_ERR;
        }
    } else {
        CRYPTO_DBG_LOG("invalid padding type(%d)\n", padding.type);
        return ALI_CRYPTO_NOSUPPORT;
	}
    if (dig_size != hash_size) {
        CRYPTO_DBG_LOG("invalid dig_size(%d vs %d)\n", (int)dig_size, (int)hash_size);
        return ALI_CRYPTO_LENGTH_ERR;
    }

    seed = (uint32_t)ls_osa_get_time_ms();
    ret = _rsa_pkcs1_sign(privkey,
                          myrand, (uint32_t *)&seed,
                          ali_hash_type, hash_size,
                          (const unsigned char *)dig, (unsigned char *)sig,
                          padding, &keybytes);
    if (0 != ret) {
        CRYPTO_DBG_LOG("ali_algo_rsa_pkcs1_sign failed %d\n", ret);
        return ALI_CRYPTO_ERROR;
    }
    return 0;
}
#endif   /* SIGN */

#ifdef CONFIG_RSA_VERIFY
#ifdef RSA_PADDING_PKCS1_V15
/*
 * ASN.1 DER decoding routines
 */
static int _asn1_get_len(unsigned char **p, const unsigned char *end,
                         size_t *len)
{
    if ((end - *p) < 1)
        return ALI_CRYPTO_INVALID_PADDING;

    if ((**p & 0x80) == 0)
        *len = *(*p)++;
    else {
        switch (**p & 0x7F) {
            case 1:
                if ((end - *p) < 2)
                    return ALI_CRYPTO_INVALID_PADDING;

                *len = (*p)[1];
                (*p) += 2;
                break;

            case 2:
                if ((end - *p) < 3)
                    return ALI_CRYPTO_INVALID_PADDING;

                *len = ((size_t)(*p)[1] << 8) | (*p)[2];
                (*p) += 3;
                break;

            case 3:
                if ((end - *p) < 4)
                    return ALI_CRYPTO_INVALID_PADDING;

                *len =
                  ((size_t)(*p)[1] << 16) | ((size_t)(*p)[2] << 8) | (*p)[3];
                (*p) += 4;
                break;

            case 4:
                if ((end - *p) < 5)
                    return ALI_CRYPTO_INVALID_PADDING;

                *len = ((size_t)(*p)[1] << 24) | ((size_t)(*p)[2] << 16) |
                       ((size_t)(*p)[3] << 8) | (*p)[4];
                (*p) += 5;
                break;

            default:
                CRYPTO_DBG_LOG("invalid padding\n");
                return ALI_CRYPTO_INVALID_PADDING;
        }
    }

    if (*len > (size_t)(end - *p))
        return ALI_CRYPTO_INVALID_PADDING;

    return 0;
}

static int _asn1_get_tag(unsigned char **p, const unsigned char *end,
                         size_t *len, int tag)
{
    if ((end - *p) < 1) {
        CRYPTO_DBG_LOG("invalid end/p\n");
        return ALI_CRYPTO_INVALID_PADDING;
    }

    if (**p != tag) {
        CRYPTO_DBG_LOG("invalid p/tag\n");
        return ALI_CRYPTO_INVALID_PADDING;
    }

    (*p)++;

    return (_asn1_get_len(p, end, len));
}

/**
 * Type-length-value structure that allows for ASN1 using DER.
 */
typedef struct asn1_buf {
    int tag;                /**< ASN1 type, e.g. ALI_ALGO_ASN1_UTF8_STRING. */
    size_t len;             /**< ASN1 length, in octets. */
    unsigned char *p;       /**< ASN1 data, e.g. in ASCII. */
} asn1_buf;

/*
 * Implementation of the PKCS#1 v2.1 RSASSA-PKCS1-v1_5-VERIFY function
 */
static int _pkcs1_v15_verify(hash_type_t hash_type,
                             size_t hashlen, const uint8_t *hash,
                             uint8_t *input, size_t input_size)
{
    int ret = 0;
    size_t  len, siglen, asn1_len;
    uint8_t *p, *end;
    asn1_buf oid;

    siglen = input_size;
    p = input;

    if (*p++ != 0 || *p++ != ALI_CRYPTO_RSA_SIGN) {
        CRYPTO_DBG_LOG("invalid padding\n");
        return ALI_CRYPTO_INVALID_PADDING;
    }

    while (*p != 0) {
        if (p >= input + siglen - 1 || *p != 0xFF) {
            CRYPTO_DBG_LOG("invalid padding\n");
            return ALI_CRYPTO_INVALID_PADDING;
        }
        p++;
    }
    p++;

    len = siglen - (p - input);

    if (len == hashlen && hash_type == HASH_NONE) {
        if (memcmp(p, hash, hashlen)) {
            CRYPTO_DBG_LOG("hash not equal\n");
            return ALI_CRYPTO_ERROR;
        }
    }

    hashlen = HASH_SIZE(hash_type);

    end = p + len;

    /*
     * Parse the ASN.1 structure inside the PKCS#1 v1.5 structure
     */
    if ((ret = _asn1_get_tag(&p, end, &asn1_len,
                             ALI_CRYPTO_ASN1_CONSTRUCTED |
                             ALI_CRYPTO_ASN1_SEQUENCE)) != 0) {
        CRYPTO_DBG_LOG("get tag failed(0x%08x)\n", ret);
        return ALI_CRYPTO_ERROR;
    }

    if (asn1_len + 2 != len) {
        CRYPTO_DBG_LOG("failed asn1_len(%d) len(%d)\n", asn1_len, len);
        return ALI_CRYPTO_ERROR;
    }

    if ((ret = _asn1_get_tag(&p, end, &asn1_len,
                             ALI_CRYPTO_ASN1_CONSTRUCTED |
                             ALI_CRYPTO_ASN1_SEQUENCE)) != 0) {
        CRYPTO_DBG_LOG("_asn1_get_tag failed(0x%08x)\n", ret);
        return ALI_CRYPTO_ERROR;
    }

    if (asn1_len + 6 + hashlen != len)
        return ALI_CRYPTO_ERROR;

    if ((ret = _asn1_get_tag(&p, end, &oid.len, ALI_CRYPTO_ASN1_OID)) != 0)
        return ALI_CRYPTO_ERROR;

    oid.p = p;
    p += oid.len;
    /*
     * assume the algorithm parameters must be NULL
     */
    if ((ret = _asn1_get_tag(&p, end, &asn1_len, ALI_CRYPTO_ASN1_NULL)) !=
        0)
        return ALI_CRYPTO_ERROR;

    if ((ret = _asn1_get_tag(&p, end, &asn1_len,
                                    ALI_CRYPTO_ASN1_OCTET_STRING)) != 0)
        return ALI_CRYPTO_ERROR;

    if (asn1_len != hashlen)
        return ALI_CRYPTO_ERROR;

    if (memcmp(p, hash, hashlen) != 0)
        return ALI_CRYPTO_ERROR;

    p += hashlen;

    if (p != end)
        return ALI_CRYPTO_ERROR;

    return ALI_CRYPTO_SUCCESS;
}

#endif /* RSA_PADDING_PKCS1_V15 */

#ifdef RSA_PADDING_PKCS1_V21
/*
 * Implementation of the PKCS#1 v2.1 RSASSA-PSS-VERIFY function
 */

static int _pss_verify(hash_type_t hash_type,
                       size_t hashlen, const uint8_t *hash,
                       int expected_salt_len,
                       uint8_t *input, size_t input_size)
{
    int ret = 0;
    size_t siglen;
    uint8_t *p;
    uint8_t res[MAX_HASH_SIZE];
    uint8_t zeros[8];
    uint32_t hlen;
    size_t slen, msb;
    size_t ctx_size;
    void *ctx;

    p = input;
    siglen = input_size;

    if (input[siglen - 1] != 0xBC)
        return ALI_CRYPTO_INVALID_ARG;

    if (hash_type != HASH_NONE) {
        /* Gather length of hash to sign */
        hashlen = HASH_SIZE(hash_type);
    }

    hlen = HASH_SIZE(hash_type);
    slen = siglen - hlen - 1; /* Currently length of salt + padding */

    memset(zeros, 0, 8);

    /*
     * Note: EMSA-PSS verification is over the length of N - 1 bits
     */
    msb = (input_size << 3) - 1;

    /* Compensate for boundary condition when applying mask */
    if (msb % 8 == 0) {
        p++;
        siglen -= 1;
    }
    if (input[0] >> (8 - siglen * 8 + msb))
        return ALI_CRYPTO_INVALID_ARG;

    ret = ali_hash_get_ctx_size(hash_type, &ctx_size);
    if (ret != ALI_CRYPTO_SUCCESS) {
        CRYPTO_DBG_LOG("get ctx size failed(%08x)\n", ret);
        return ret;
    }
    ctx = ls_osa_malloc(ctx_size);
    if (ctx == NULL) {
        CRYPTO_DBG_LOG("malloc(%d) failed\n", (int)ctx_size);
        return ALI_CRYPTO_OUTOFMEM;
    }
    memset(ctx, 0, ctx_size);

    ret = ali_hash_init(hash_type, ctx);
    if (ALI_CRYPTO_SUCCESS != ret) {
       CRYPTO_DBG_LOG("ali_hash_init failed(0x%08x)\n", ret);
       goto _cleanup;
    }

    ret = mgf_mask(p, siglen - hlen - 1, p + siglen - hlen - 1, hlen, ctx);
    if (ALI_CRYPTO_SUCCESS != ret) {
       CRYPTO_DBG_LOG("mgf_mask failed(0x%08x)\n", ret);
       goto _cleanup;
    }

    input[0] &= 0xFF >> (siglen * 8 - msb);

    while (p < input + siglen && *p == 0)
        p++;

    if (p == input + siglen || *p++ != 0x01) {
        ret = ALI_CRYPTO_INVALID_PADDING;
        goto _cleanup;
    }

    /* Actual salt len */
    slen -= p - input;

    if (expected_salt_len != ALI_CRYPTO_RSA_SALT_LEN_ANY &&
        slen != (size_t)expected_salt_len) {
        CRYPTO_DBG_LOG("wrong slen(%d)\n", slen);
        ret = ALI_CRYPTO_INVALID_PADDING;
        goto _cleanup;
    }

    /*
     * Generate H = Hash( M' )
     */
    ret = ali_hash_init(hash_type, ctx);
    if (ret) {
       CRYPTO_DBG_LOG("ali_hash_init failed(0x%08x)\n", ret);
       goto _cleanup;
    }
    ali_hash_update(zeros, 8, ctx);
    ali_hash_update(hash, hashlen, ctx);
    ali_hash_update(p, slen, ctx);
    ali_hash_final(res, ctx);

    if (memcmp(p + slen, res, hlen)) {
        CRYPTO_DBG_LOG("compare failed\n");
        ret = ALI_CRYPTO_ERROR;
    }

_cleanup:
    if (ctx != NULL)
        ls_osa_free(ctx);
    return ret;
}

#endif

static int _rsa_pkcs1_verify(const rsa_pubkey_t *pub_key,
                             int (*f_rng)(void *, unsigned char *, size_t),
                             void *p_rng, int mode, size_t hashlen,
                             const unsigned char *hash,
                             const unsigned char *sig, 
                             rsa_padding_t padding,
                             hash_type_t hash_type, size_t keybytes)
{
    int ret = 0;
    uint8_t buf[MAX_RSA_KEY_SIZE>>3];
    api_rsa_ctx_t  *ctx = (api_rsa_ctx_t *)pub_key;
    ctx->hal_ctx = (char *)&(ctx->hal_ctx) + sizeof(ctx->hal_ctx);

    if (keybytes < 16) {
        CRYPTO_DBG_LOG("invalid keybytes(%d)\n", keybytes);
        return ALI_CRYPTO_INVALID_ARG;
    }

    switch (padding.type) {
#ifdef RSA_PADDING_PKCS1_V15
        case RSASSA_PKCS1_V1_5:
            ret = ls_hal_rsa_public(ctx->hal_ctx, sig, buf, keybytes);
            if (ret) {
                CRYPTO_DBG_LOG("hal_rsa_public failed(0x%08x)\n", ret);
                ret = ALI_CRYPTO_ERROR;
                break;
            }

            ret = _pkcs1_v15_verify(hash_type, hashlen, hash, buf, keybytes);
            if (ret) {
                CRYPTO_DBG_LOG("pkcs1_v15 verify failed(0x%08x)\n", ret);
                ret = ALI_CRYPTO_ERROR;
            }

            break;
#endif
#ifdef RSA_PADDING_PKCS1_V21
        case RSASSA_PKCS1_PSS_MGF1:
            ret = ls_hal_rsa_public(ctx->hal_ctx, sig, buf, keybytes);
            if (ret) {
                CRYPTO_DBG_LOG("hal_rsa_public failed(0x%08x)\n", ret);
                ret = ALI_CRYPTO_ERROR;
                break;
            }

            ret = _pss_verify(hash_type, hashlen, hash, -1, buf, keybytes);
            if (ret) {
                CRYPTO_DBG_LOG("_pkcs1_v15_verify failed(0x%08x)\n", ret);
                ret = ALI_CRYPTO_ERROR;
            }

            break;
#endif
        default:
            CRYPTO_DBG_LOG("invalid type(%d)\n", padding.type);
            return ALI_CRYPTO_INVALID_PADDING;
    }

    return ret;
}

ali_crypto_result ali_rsa_verify(const rsa_pubkey_t *pubkey,
                                 const uint8_t *dig, size_t dig_size,
                                 const uint8_t *sig, size_t sig_size,
                                 rsa_padding_t padding, bool *p_result)
{
    ali_crypto_result   ret = ALI_CRYPTO_SUCCESS;
    uint32_t            hash_size;
    hash_type_t         ali_hash_type;
    api_rsa_ctx_t       *ctx;
    size_t              keybytes;

    if (p_result == NULL) {
        CRYPTO_DBG_LOG("p_result is NULL\n");
        return ALI_CRYPTO_INVALID_ARG;
    }
    *p_result = 0;

    if (pubkey == NULL) {
        CRYPTO_DBG_LOG("invalid ctx\n");
        return ALI_CRYPTO_INVALID_CONTEXT;
    }

    if (pubkey == NULL || NULL == p_result
        || dig == NULL || dig_size == 0
        || sig == NULL || sig_size == 0) {
        CRYPTO_DBG_LOG("invalid input args!\n");
	    return ALI_CRYPTO_INVALID_ARG;
    }

    ctx = (api_rsa_ctx_t *)pubkey;
    keybytes = ctx->keybytes;

    if (padding.type == RSASSA_PKCS1_V1_5) {
        ali_hash_type = padding.pad.rsassa_v1_5.type;
    } else if (padding.type == RSASSA_PKCS1_PSS_MGF1) {
        ali_hash_type = padding.pad.rsassa_pss.type;
    } else {
        CRYPTO_DBG_LOG("invalid padding type(%d)\n", padding.type);
        return ALI_CRYPTO_NOSUPPORT;
    }
    hash_size = HASH_SIZE(ali_hash_type);
    
	if (sig_size != keybytes) {
        CRYPTO_DBG_LOG("invalid src_size(%d), keybytes(%d)\n",
            sig_size, keybytes);
        return ALI_CRYPTO_LENGTH_ERR;
    }

    if (padding.type == RSASSA_PKCS1_V1_5) {
        if (dig_size + 11 > keybytes) {
            CRYPTO_DBG_LOG("invalid dig_size(%d)\n", (int)dig_size);
            return ALI_CRYPTO_LENGTH_ERR;
        }
    } else {
        if (keybytes < padding.pad.rsassa_pss.salt_len + hash_size + 2) {
            CRYPTO_DBG_LOG("invalid salt size(%d) for rsa_%d\n",
                   (int)padding.pad.rsassa_pss.salt_len, (int)keybytes << 3);
            return ALI_CRYPTO_LENGTH_ERR;
        }
    }

    ret = _rsa_pkcs1_verify(pubkey, NULL, NULL,
                            ALI_CRYPTO_RSA_PUBLIC, keybytes,
                            (const unsigned char *)dig,
                            (const unsigned char *)sig,
                            padding, ali_hash_type, keybytes);
    if (0 != ret) {
        CRYPTO_DBG_LOG("pkcs1 verify failed(0x%08x)\n", ret);
        return ALI_CRYPTO_ERROR;
    }

    if (0 == ret) {
        *p_result = 1;
    }

    return ret;
}
#endif /*VERIFY */
