/**
* Copyright (C) 2017  Alibaba Group Holding Limited.
**/
#include "config.h"
#include "osa.h"

#if defined(ALI_ALGO_RSA_C)
#include "rsa.h"
#include "oid.h"

#if defined(ALI_ALGO_PKCS1_V21)
#include "hash.h"
#include "md.h"
#endif

/*
 * Initialize an RSA context
 */
void ali_algo_rsa_init(ali_algo_rsa_context *ctx, int padding, int hash_id)
{
    memset(ctx, 0, sizeof(ali_algo_rsa_context));

    ali_algo_rsa_set_padding(ctx, padding, hash_id);

#if CONFIG_MULTH_SUPPORT
    ls_osa_mutex_create(&ctx->mutex);
#endif
}

/*
 * Set padding for an existing RSA context
 */
void ali_algo_rsa_set_padding(ali_algo_rsa_context *ctx, int padding, int hash_id)
{
    ctx->padding = padding;
    ctx->hash_id = hash_id;
}

#if defined(ALI_ALGO_GENPRIME)

/*
 * Generate an RSA keypair
 */
int ali_algo_rsa_gen_key(ali_algo_rsa_context *ctx,
                        int (*f_rng)(void *, unsigned char *, size_t),
                        void *p_rng, unsigned int nbits, int exponent)
{
    int         ret;
    ali_algo_mpi P1, Q1, H, G;

    if (f_rng == NULL || nbits < 128 || exponent < 3)
        return (ALI_ALGO_ERR_RSA_BAD_INPUT_DATA);

    if (nbits % 2)
        return (ALI_ALGO_ERR_RSA_BAD_INPUT_DATA);

    ali_algo_mpi_init(&P1);
    ali_algo_mpi_init(&Q1);
    ali_algo_mpi_init(&H);
    ali_algo_mpi_init(&G);

    /*
     * find primes P and Q with Q < P so that:
     * GCD( E, (P-1)*(Q-1) ) == 1
     */
    ALI_ALGO_MPI_CHK(ali_algo_mpi_lset(&ctx->E, exponent));

    do {
        ALI_ALGO_MPI_CHK(
          ali_algo_mpi_gen_prime(&ctx->P, nbits >> 1, 0, f_rng, p_rng));

        ALI_ALGO_MPI_CHK(
          ali_algo_mpi_gen_prime(&ctx->Q, nbits >> 1, 0, f_rng, p_rng));

        if (ali_algo_mpi_cmp_mpi(&ctx->P, &ctx->Q) == 0)
            continue;

        ALI_ALGO_MPI_CHK(ali_algo_mpi_mul_mpi(&ctx->N, &ctx->P, &ctx->Q));
        if (ali_algo_mpi_bitlen(&ctx->N) != nbits)
            continue;

        if (ali_algo_mpi_cmp_mpi(&ctx->P, &ctx->Q) < 0)
            ali_algo_mpi_swap(&ctx->P, &ctx->Q);

        ALI_ALGO_MPI_CHK(ali_algo_mpi_sub_int(&P1, &ctx->P, 1));
        ALI_ALGO_MPI_CHK(ali_algo_mpi_sub_int(&Q1, &ctx->Q, 1));
        ALI_ALGO_MPI_CHK(ali_algo_mpi_mul_mpi(&H, &P1, &Q1));
        ALI_ALGO_MPI_CHK(ali_algo_mpi_gcd(&G, &ctx->E, &H));
    } while (ali_algo_mpi_cmp_int(&G, 1) != 0);

    /*
     * D  = E^-1 mod ((P-1)*(Q-1))
     * DP = D mod (P - 1)
     * DQ = D mod (Q - 1)
     * QP = Q^-1 mod P
     */
    ALI_ALGO_MPI_CHK(ali_algo_mpi_inv_mod(&ctx->D, &ctx->E, &H));
    ALI_ALGO_MPI_CHK(ali_algo_mpi_mod_mpi(&ctx->DP, &ctx->D, &P1));
    ALI_ALGO_MPI_CHK(ali_algo_mpi_mod_mpi(&ctx->DQ, &ctx->D, &Q1));
    ALI_ALGO_MPI_CHK(ali_algo_mpi_inv_mod(&ctx->QP, &ctx->Q, &ctx->P));

    ctx->len = (ali_algo_mpi_bitlen(&ctx->N) + 7) >> 3;

cleanup:

    ali_algo_mpi_free(&P1);
    ali_algo_mpi_free(&Q1);
    ali_algo_mpi_free(&H);
    ali_algo_mpi_free(&G);

    if (ret != 0) {
        ali_algo_rsa_free(ctx);
        return (ALI_ALGO_ERR_RSA_KEY_GEN_FAILED + ret);
    }

    return (0);
}

#endif /* ALI_ALGO_GENPRIME */

/*
 * Check a public RSA key
 */
int ali_algo_rsa_check_pubkey(const ali_algo_rsa_context *ctx)
{
    if (!ctx->N.p || !ctx->E.p)
        return (ALI_ALGO_ERR_RSA_KEY_CHECK_FAILED);

    if ((ctx->N.p[0] & 1) == 0 || (ctx->E.p[0] & 1) == 0)
        return (ALI_ALGO_ERR_RSA_KEY_CHECK_FAILED);

    if (ali_algo_mpi_bitlen(&ctx->N) < 128 ||
        ali_algo_mpi_bitlen(&ctx->N) > ALI_ALGO_MPI_MAX_BITS)
        return (ALI_ALGO_ERR_RSA_KEY_CHECK_FAILED);

    if (ali_algo_mpi_bitlen(&ctx->E) < 2 ||
        ali_algo_mpi_cmp_mpi(&ctx->E, &ctx->N) >= 0)
        return (ALI_ALGO_ERR_RSA_KEY_CHECK_FAILED);

    return (0);
}

/*
 * Check a private RSA key
 */
int ali_algo_rsa_check_privkey(const ali_algo_rsa_context *ctx)
{
    int         ret;
    ali_algo_mpi PQ, DE, P1, Q1, H, I, G, G2, L1, L2, DP, DQ, QP;

    if ((ret = ali_algo_rsa_check_pubkey(ctx)) != 0)
        return (ret);

    if (!ctx->P.p || !ctx->Q.p || !ctx->D.p)
        return (ALI_ALGO_ERR_RSA_KEY_CHECK_FAILED);

    ali_algo_mpi_init(&PQ);
    ali_algo_mpi_init(&DE);
    ali_algo_mpi_init(&P1);
    ali_algo_mpi_init(&Q1);
    ali_algo_mpi_init(&H);
    ali_algo_mpi_init(&I);
    ali_algo_mpi_init(&G);
    ali_algo_mpi_init(&G2);
    ali_algo_mpi_init(&L1);
    ali_algo_mpi_init(&L2);
    ali_algo_mpi_init(&DP);
    ali_algo_mpi_init(&DQ);
    ali_algo_mpi_init(&QP);

    ALI_ALGO_MPI_CHK(ali_algo_mpi_mul_mpi(&PQ, &ctx->P, &ctx->Q));
    ALI_ALGO_MPI_CHK(ali_algo_mpi_mul_mpi(&DE, &ctx->D, &ctx->E));
    ALI_ALGO_MPI_CHK(ali_algo_mpi_sub_int(&P1, &ctx->P, 1));
    ALI_ALGO_MPI_CHK(ali_algo_mpi_sub_int(&Q1, &ctx->Q, 1));
    ALI_ALGO_MPI_CHK(ali_algo_mpi_mul_mpi(&H, &P1, &Q1));
    ALI_ALGO_MPI_CHK(ali_algo_mpi_gcd(&G, &ctx->E, &H));

    ALI_ALGO_MPI_CHK(ali_algo_mpi_gcd(&G2, &P1, &Q1));
    ALI_ALGO_MPI_CHK(ali_algo_mpi_div_mpi(&L1, &L2, &H, &G2));
    ALI_ALGO_MPI_CHK(ali_algo_mpi_mod_mpi(&I, &DE, &L1));

    ALI_ALGO_MPI_CHK(ali_algo_mpi_mod_mpi(&DP, &ctx->D, &P1));
    ALI_ALGO_MPI_CHK(ali_algo_mpi_mod_mpi(&DQ, &ctx->D, &Q1));
    ALI_ALGO_MPI_CHK(ali_algo_mpi_inv_mod(&QP, &ctx->Q, &ctx->P));
    /*
     * Check for a valid PKCS1v2 private key
     */
    if (ali_algo_mpi_cmp_mpi(&PQ, &ctx->N) != 0 ||
        ali_algo_mpi_cmp_mpi(&DP, &ctx->DP) != 0 ||
        ali_algo_mpi_cmp_mpi(&DQ, &ctx->DQ) != 0 ||
        ali_algo_mpi_cmp_mpi(&QP, &ctx->QP) != 0 ||
        ali_algo_mpi_cmp_int(&L2, 0) != 0 || ali_algo_mpi_cmp_int(&I, 1) != 0 ||
        ali_algo_mpi_cmp_int(&G, 1) != 0) {
        ret = ALI_ALGO_ERR_RSA_KEY_CHECK_FAILED;
    }

cleanup:
    ali_algo_mpi_free(&PQ);
    ali_algo_mpi_free(&DE);
    ali_algo_mpi_free(&P1);
    ali_algo_mpi_free(&Q1);
    ali_algo_mpi_free(&H);
    ali_algo_mpi_free(&I);
    ali_algo_mpi_free(&G);
    ali_algo_mpi_free(&G2);
    ali_algo_mpi_free(&L1);
    ali_algo_mpi_free(&L2);
    ali_algo_mpi_free(&DP);
    ali_algo_mpi_free(&DQ);
    ali_algo_mpi_free(&QP);

    if (ret == ALI_ALGO_ERR_RSA_KEY_CHECK_FAILED)
        return (ret);

    if (ret != 0)
        return (ALI_ALGO_ERR_RSA_KEY_CHECK_FAILED + ret);

    return (0);
}

/*
 * Check if contexts holding a public and private key match
 */
int ali_algo_rsa_check_pub_priv(const ali_algo_rsa_context *pub,
                               const ali_algo_rsa_context *prv)
{
    if (ali_algo_rsa_check_pubkey(pub) != 0 ||
        ali_algo_rsa_check_privkey(prv) != 0) {
        return (ALI_ALGO_ERR_RSA_KEY_CHECK_FAILED);
    }

    if (ali_algo_mpi_cmp_mpi(&pub->N, &prv->N) != 0 ||
        ali_algo_mpi_cmp_mpi(&pub->E, &prv->E) != 0) {
        return (ALI_ALGO_ERR_RSA_KEY_CHECK_FAILED);
    }

    return (0);
}

/*
 * Do an RSA public key operation
 */
int ali_algo_rsa_public(ali_algo_rsa_context *ctx, const unsigned char *input,
                       unsigned char *output)
{
    int         ret;
    size_t      olen;
    ali_algo_mpi T;

    ali_algo_mpi_init(&T);

#if CONFIG_MULTH_SUPPORT
    ls_osa_mutex_lock(&ctx->mutex);
#endif
    ALI_ALGO_MPI_CHK(ali_algo_mpi_read_binary(&T, input, ctx->len));

    if (ali_algo_mpi_cmp_mpi(&T, &ctx->N) >= 0) {
        ret = ALI_ALGO_ERR_MPI_BAD_INPUT_DATA;
        goto cleanup;
    }

    olen = ctx->len;
    ALI_ALGO_MPI_CHK(ali_algo_mpi_exp_mod(&T, &T, &ctx->E, &ctx->N, &ctx->RN));
    ALI_ALGO_MPI_CHK(ali_algo_mpi_write_binary(&T, output, olen));

cleanup:
#if CONFIG_MULTH_SUPPORT
    ls_osa_mutex_unlock(&ctx->mutex);
#endif
    ali_algo_mpi_free(&T);

    if (ret != 0)
        return (ALI_ALGO_ERR_RSA_PUBLIC_FAILED + ret);

    return (0);
}

/*
 * Generate or update blinding values, see section 10 of:
 *  KOCHER, Paul C. Timing attacks on implementations of Diffie-Hellman, RSA,
 *  DSS, and other systems. In : Advances in Cryptology-CRYPTO'96. Springer
 *  Berlin Heidelberg, 1996. p. 104-113.
 */
static int rsa_prepare_blinding(ali_algo_rsa_context *ctx,
                                int (*f_rng)(void *, unsigned char *, size_t),
                                void *p_rng)
{
    int ret, count = 0;

    if (ctx->Vf.p != NULL) {
        /* We already have blinding values, just update them by squaring */
        ALI_ALGO_MPI_CHK(ali_algo_mpi_mul_mpi(&ctx->Vi, &ctx->Vi, &ctx->Vi));
        ALI_ALGO_MPI_CHK(ali_algo_mpi_mod_mpi(&ctx->Vi, &ctx->Vi, &ctx->N));
        ALI_ALGO_MPI_CHK(ali_algo_mpi_mul_mpi(&ctx->Vf, &ctx->Vf, &ctx->Vf));
        ALI_ALGO_MPI_CHK(ali_algo_mpi_mod_mpi(&ctx->Vf, &ctx->Vf, &ctx->N));

        goto cleanup;
    }

    /* Unblinding value: Vf = random number, invertible mod N */
    do {
        if (count++ > 10)
            return (ALI_ALGO_ERR_RSA_RNG_FAILED);

        ALI_ALGO_MPI_CHK(
          ali_algo_mpi_fill_random(&ctx->Vf, ctx->len - 1, f_rng, p_rng));
        ALI_ALGO_MPI_CHK(ali_algo_mpi_gcd(&ctx->Vi, &ctx->Vf, &ctx->N));
    } while (ali_algo_mpi_cmp_int(&ctx->Vi, 1) != 0);

    /* Blinding value: Vi =  Vf^(-e) mod N */
    ALI_ALGO_MPI_CHK(ali_algo_mpi_inv_mod(&ctx->Vi, &ctx->Vf, &ctx->N));
    ALI_ALGO_MPI_CHK(
      ali_algo_mpi_exp_mod(&ctx->Vi, &ctx->Vi, &ctx->E, &ctx->N, &ctx->RN));


cleanup:
    return (ret);
}

/*
 * Do an RSA private key operation
 */
int ali_algo_rsa_private(ali_algo_rsa_context *ctx,
                        int (*f_rng)(void *, unsigned char *, size_t),
                        void *p_rng, const unsigned char *input,
                        unsigned char *output)
{
    int         ret;
    size_t      olen;
    ali_algo_mpi T, T1, T2;

    /* Make sure we have private key info, prevent possible misuse */
#if defined(ALI_ALGO_RSA_NO_CRT)
    if (ctx->N.p == NULL || ctx->D.p == NULL)
        return (ALI_ALGO_ERR_RSA_BAD_INPUT_DATA);
#else
    if (ctx->P.p == NULL || ctx->Q.p == NULL || ctx->D.p == NULL)
        return (ALI_ALGO_ERR_RSA_BAD_INPUT_DATA);
#endif

    ali_algo_mpi_init(&T);
    ali_algo_mpi_init(&T1);
    ali_algo_mpi_init(&T2);

#if CONFIG_MULTH_SUPPORT
    ls_osa_mutex_lock(&ctx->mutex);
#endif
    ALI_ALGO_MPI_CHK(ali_algo_mpi_read_binary(&T, input, ctx->len));
    if (ali_algo_mpi_cmp_mpi(&T, &ctx->N) >= 0) {
        ret = ALI_ALGO_ERR_MPI_BAD_INPUT_DATA;
        goto cleanup;
    }

    if (f_rng != NULL) {
        /*
         * Blinding
         * T = T * Vi mod N
         */
        ALI_ALGO_MPI_CHK(rsa_prepare_blinding(ctx, f_rng, p_rng));
        ALI_ALGO_MPI_CHK(ali_algo_mpi_mul_mpi(&T, &T, &ctx->Vi));
        ALI_ALGO_MPI_CHK(ali_algo_mpi_mod_mpi(&T, &T, &ctx->N));
    }

#if defined(ALI_ALGO_RSA_NO_CRT)
    ALI_ALGO_MPI_CHK(ali_algo_mpi_exp_mod(&T, &T, &ctx->D, &ctx->N, &ctx->RN));
#else
    /*
     * faster decryption using the CRT
     *
     * T1 = input ^ dP mod P
     * T2 = input ^ dQ mod Q
     */
    ALI_ALGO_MPI_CHK(ali_algo_mpi_exp_mod(&T1, &T, &ctx->DP, &ctx->P, &ctx->RP));
    ALI_ALGO_MPI_CHK(ali_algo_mpi_exp_mod(&T2, &T, &ctx->DQ, &ctx->Q, &ctx->RQ));

    /*
     * T = (T1 - T2) * (Q^-1 mod P) mod P
     */
    ALI_ALGO_MPI_CHK(ali_algo_mpi_sub_mpi(&T, &T1, &T2));
    ALI_ALGO_MPI_CHK(ali_algo_mpi_mul_mpi(&T1, &T, &ctx->QP));
    ALI_ALGO_MPI_CHK(ali_algo_mpi_mod_mpi(&T, &T1, &ctx->P));

    /*
     * T = T2 + T * Q
     */
    ALI_ALGO_MPI_CHK(ali_algo_mpi_mul_mpi(&T1, &T, &ctx->Q));
    ALI_ALGO_MPI_CHK(ali_algo_mpi_add_mpi(&T, &T2, &T1));
#endif /* ALI_ALGO_RSA_NO_CRT */

    if (f_rng != NULL) {
        /*
         * Unblind
         * T = T * Vf mod N
         */
        ALI_ALGO_MPI_CHK(ali_algo_mpi_mul_mpi(&T, &T, &ctx->Vf));
        ALI_ALGO_MPI_CHK(ali_algo_mpi_mod_mpi(&T, &T, &ctx->N));
    }

    olen = ctx->len;
    ALI_ALGO_MPI_CHK(ali_algo_mpi_write_binary(&T, output, olen));

cleanup:
#if CONFIG_MULTH_SUPPORT
    ls_osa_mutex_unlock(&ctx->mutex);
#endif
    ali_algo_mpi_free(&T);
    ali_algo_mpi_free(&T1);
    ali_algo_mpi_free(&T2);

    if (ret != 0)
        return (ALI_ALGO_ERR_RSA_PRIVATE_FAILED + ret);

    return (0);
}

#if defined(ALI_ALGO_PKCS1_V21)
/**
 * Generate and apply the MGF1 operation (from PKCS#1 v2.1) to a buffer.
 *
 * \param dst       buffer to mask
 * \param dlen      length of destination buffer
 * \param src       source of the mask generation
 * \param slen      length of the source buffer
 * \param md_ctx    message digest context to use
 */
static void mgf_mask(unsigned char *dst, size_t dlen, unsigned char *src,
                     size_t slen, ali_algo_hash_context_t *md_ctx)
{
    unsigned char  mask[ALI_ALGO_MD_MAX_SIZE];
    unsigned char  counter[4];
    unsigned char *p;
    unsigned int   hlen;
    size_t         i, use_len;

    memset(mask, 0, ALI_ALGO_MD_MAX_SIZE);
    memset(counter, 0, 4);

    hlen = ali_algo_hash_get_size(md_ctx->md_info);

    /* Generate and apply dbMask */
    p = dst;

    while (dlen > 0) {
        use_len = hlen;
        if (dlen < hlen)
            use_len = dlen;

        ali_algo_hash_starts(md_ctx);
        ali_algo_hash_update(md_ctx, src, slen);
        ali_algo_hash_update(md_ctx, counter, 4);
        ali_algo_hash_finish(md_ctx, mask);

        for (i = 0; i < use_len; ++i)
            *p++ ^= mask[i];

        counter[3]++;

        dlen -= use_len;
    }
}
#endif /* ALI_ALGO_PKCS1_V21 */

#if defined(ALI_ALGO_PKCS1_V21)
/*
 * Implementation of the PKCS#1 v2.1 RSAES-OAEP-ENCRYPT function
 */
int ali_algo_rsa_rsaes_oaep_encrypt(
  ali_algo_rsa_context *ctx, int (*f_rng)(void *, unsigned char *, size_t),
  void *p_rng, int mode, const unsigned char *label, size_t label_len,
  size_t ilen, const unsigned char *input, unsigned char *output)
{
    size_t                   olen;
    int                      ret;
    unsigned char *          p = output;
    unsigned int             hlen;
    const ali_algo_md_info_t *md_info;
    ali_algo_hash_context_t   md_ctx;

    if (mode == ALI_ALGO_RSA_PRIVATE && ctx->padding != ALI_ALGO_RSA_PKCS_V21)
        return (ALI_ALGO_ERR_RSA_BAD_INPUT_DATA);

    if (f_rng == NULL)
        return (ALI_ALGO_ERR_RSA_BAD_INPUT_DATA);

    md_info = ali_algo_hash_info_from_type((ali_algo_md_type_t)ctx->hash_id);
    if (md_info == NULL)
        return (ALI_ALGO_ERR_RSA_BAD_INPUT_DATA);

    olen = ctx->len;
    hlen = ali_algo_hash_get_size(md_info);

    /* first comparison checks for overflow */
    if (ilen + 2 * hlen + 2 < ilen || olen < ilen + 2 * hlen + 2)
        return (ALI_ALGO_ERR_RSA_BAD_INPUT_DATA);

    memset(output, 0, olen);

    *p++ = 0;

    /* Generate a random octet string seed */
    if ((ret = f_rng(p_rng, p, hlen)) != 0)
        return (ALI_ALGO_ERR_RSA_RNG_FAILED + ret);

    p += hlen;

    /* Construct DB */
    ali_algo_hash(md_info, label, label_len, p);
    p += hlen;
    p += olen - 2 * hlen - 2 - ilen;
    *p++ = 1;
    memcpy(p, input, ilen);

    ali_algo_hash_init(&md_ctx);
    if ((ret = ali_algo_hash_setup(&md_ctx, md_info, 0)) != 0) {
        ali_algo_hash_free(&md_ctx);
        return (ret);
    }

    /* maskedDB: Apply dbMask to DB */
    mgf_mask(output + hlen + 1, olen - hlen - 1, output + 1, hlen, &md_ctx);

    /* maskedSeed: Apply seedMask to seed */
    mgf_mask(output + 1, hlen, output + hlen + 1, olen - hlen - 1, &md_ctx);

    ali_algo_hash_free(&md_ctx);

    return ((mode == ALI_ALGO_RSA_PUBLIC)
              ? ali_algo_rsa_public(ctx, output, output)
              : ali_algo_rsa_private(ctx, f_rng, p_rng, output, output));
}
#endif /* ALI_ALGO_PKCS1_V21 */

#if defined(ALI_ALGO_PKCS1_V15)
/*
 * Implementation of the PKCS#1 v2.1 RSAES-PKCS1-V1_5-ENCRYPT function
 */
int ali_algo_rsa_rsaes_pkcs1_v15_encrypt(
  ali_algo_rsa_context *ctx, int (*f_rng)(void *, unsigned char *, size_t),
  void *p_rng, int mode, size_t ilen, const unsigned char *input,
  unsigned char *output)
{
    size_t         nb_pad, olen;
    int            ret;
    unsigned char *p = output;

    if (mode == ALI_ALGO_RSA_PRIVATE && ctx->padding != ALI_ALGO_RSA_PKCS_V15)
        return (ALI_ALGO_ERR_RSA_BAD_INPUT_DATA);

    // We don't check p_rng because it won't be dereferenced here
    if (f_rng == NULL || input == NULL || output == NULL)
        return (ALI_ALGO_ERR_RSA_BAD_INPUT_DATA);

    olen = ctx->len;

    /* first comparison checks for overflow */
    if (ilen + 11 < ilen || olen < ilen + 11)
        return (ALI_ALGO_ERR_RSA_BAD_INPUT_DATA);

    nb_pad = olen - 3 - ilen;

    *p++ = 0;
    if (mode == ALI_ALGO_RSA_PUBLIC) {
        *p++ = ALI_ALGO_RSA_CRYPT;

        while (nb_pad-- > 0) {
            int rng_dl = 100;

            do {
                ret = f_rng(p_rng, p, 1);
            } while (*p == 0 && --rng_dl && ret == 0);

            /* Check if RNG failed to generate data */
            if (rng_dl == 0 || ret != 0)
                return (ALI_ALGO_ERR_RSA_RNG_FAILED + ret);

            p++;
        }
    } else {
        *p++ = ALI_ALGO_RSA_SIGN;

        while (nb_pad-- > 0)
            *p++ = 0xFF;
    }

    *p++ = 0;
    memcpy(p, input, ilen);

    return ((mode == ALI_ALGO_RSA_PUBLIC)
              ? ali_algo_rsa_public(ctx, output, output)
              : ali_algo_rsa_private(ctx, f_rng, p_rng, output, output));
}
#endif /* ALI_ALGO_PKCS1_V15 */

/*
 * Add the message padding, then do an RSA operation
 */
int ali_algo_rsa_pkcs1_encrypt(ali_algo_rsa_context *ctx,
                              int (*f_rng)(void *, unsigned char *, size_t),
                              void *p_rng, int mode, size_t ilen,
                              const unsigned char *input, unsigned char *output)
{
    switch (ctx->padding) {
#if defined(ALI_ALGO_PKCS1_V15)
        case ALI_ALGO_RSA_PKCS_V15:
            return ali_algo_rsa_rsaes_pkcs1_v15_encrypt(ctx, f_rng, p_rng, mode,
                                                       ilen, input, output);
#endif

#if defined(ALI_ALGO_PKCS1_V21)
        case ALI_ALGO_RSA_PKCS_V21:
            return ali_algo_rsa_rsaes_oaep_encrypt(ctx, f_rng, p_rng, mode, NULL,
                                                  0, ilen, input, output);
#endif

        default:
            return (ALI_ALGO_ERR_RSA_INVALID_PADDING);
    }
}

#if defined(ALI_ALGO_PKCS1_V21)
/*
 * Implementation of the PKCS#1 v2.1 RSAES-OAEP-DECRYPT function
 */
int ali_algo_rsa_rsaes_oaep_decrypt(
  ali_algo_rsa_context *ctx, int (*f_rng)(void *, unsigned char *, size_t),
  void *p_rng, int mode, const unsigned char *label, size_t label_len,
  size_t *olen, const unsigned char *input, unsigned char *output,
  size_t output_max_len)
{
    int                      ret;
    size_t                   ilen, i, pad_len;
    unsigned char *          p, bad, pad_done;
    unsigned char            buf[ALI_ALGO_MPI_MAX_SIZE];
    unsigned char            lhash[ALI_ALGO_MD_MAX_SIZE];
    unsigned int             hlen;
    const ali_algo_md_info_t *md_info;
    ali_algo_hash_context_t   md_ctx;

    /*
     * Parameters sanity checks
     */
    if (mode == ALI_ALGO_RSA_PRIVATE && ctx->padding != ALI_ALGO_RSA_PKCS_V21)
        return (ALI_ALGO_ERR_RSA_BAD_INPUT_DATA);

    ilen = ctx->len;

    if (ilen < 16 || ilen > sizeof(buf))
        return (ALI_ALGO_ERR_RSA_BAD_INPUT_DATA);

    md_info = ali_algo_hash_info_from_type((ali_algo_md_type_t)ctx->hash_id);
    if (md_info == NULL)
        return (ALI_ALGO_ERR_RSA_BAD_INPUT_DATA);

    hlen = ali_algo_hash_get_size(md_info);

    // checking for integer underflow
    if (2 * hlen + 2 > ilen)
        return (ALI_ALGO_ERR_RSA_BAD_INPUT_DATA);

    /*
     * RSA operation
     */
    ret = (mode == ALI_ALGO_RSA_PUBLIC)
            ? ali_algo_rsa_public(ctx, input, buf)
            : ali_algo_rsa_private(ctx, f_rng, p_rng, input, buf);

    if (ret != 0)
        return (ret);

    /*
     * Unmask data and generate lHash
     */
    ali_algo_hash_init(&md_ctx);
    if ((ret = ali_algo_hash_setup(&md_ctx, md_info, 0)) != 0) {
        ali_algo_hash_free(&md_ctx);
        return (ret);
    }


    /* Generate lHash */
    ali_algo_hash(md_info, label, label_len, lhash);

    /* seed: Apply seedMask to maskedSeed */
    mgf_mask(buf + 1, hlen, buf + hlen + 1, ilen - hlen - 1, &md_ctx);

    /* DB: Apply dbMask to maskedDB */
    mgf_mask(buf + hlen + 1, ilen - hlen - 1, buf + 1, hlen, &md_ctx);

    ali_algo_hash_free(&md_ctx);

    /*
     * Check contents, in "constant-time"
     */
    p   = buf;
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
    if (bad != 0)
        return (ALI_ALGO_ERR_RSA_INVALID_PADDING);

    if (ilen - (p - buf) > output_max_len)
        return (ALI_ALGO_ERR_RSA_OUTPUT_TOO_LARGE);

    *olen = ilen - (p - buf);
    memcpy(output, p, *olen);

    return (0);
}
#endif /* ALI_ALGO_PKCS1_V21 */

#if defined(ALI_ALGO_PKCS1_V15)
/*
 * Implementation of the PKCS#1 v2.1 RSAES-PKCS1-V1_5-DECRYPT function
 */
int ali_algo_rsa_rsaes_pkcs1_v15_decrypt(
  ali_algo_rsa_context *ctx, int (*f_rng)(void *, unsigned char *, size_t),
  void *p_rng, int mode, size_t *olen, const unsigned char *input,
  unsigned char *output, size_t output_max_len)
{
    int            ret;
    size_t         ilen, pad_count = 0, i;
    unsigned char *p, bad, pad_done = 0;
    unsigned char  buf[ALI_ALGO_MPI_MAX_SIZE];

    if (mode == ALI_ALGO_RSA_PRIVATE && ctx->padding != ALI_ALGO_RSA_PKCS_V15)
        return (ALI_ALGO_ERR_RSA_BAD_INPUT_DATA);

    ilen = ctx->len;

    if (ilen < 16 || ilen > sizeof(buf))
        return (ALI_ALGO_ERR_RSA_BAD_INPUT_DATA);

    ret = (mode == ALI_ALGO_RSA_PUBLIC)
            ? ali_algo_rsa_public(ctx, input, buf)
            : ali_algo_rsa_private(ctx, f_rng, p_rng, input, buf);

    if (ret != 0)
        return (ret);

    p   = buf;
    bad = 0;

    /*
     * Check and get padding len in "constant-time"
     */
    bad |= *p++; /* First byte must be 0 */

    /* This test does not depend on secret data */
    if (mode == ALI_ALGO_RSA_PRIVATE) {
        bad |= *p++ ^ ALI_ALGO_RSA_CRYPT;

        /* Get padding len, but always read till end of buffer
         * (minus one, for the 00 byte) */
        for (i = 0; i < ilen - 3; i++) {
            pad_done |= ((p[i] | (unsigned char)-p[i]) >> 7) ^ 1;
            pad_count += ((pad_done | (unsigned char)-pad_done) >> 7) ^ 1;
        }

        p += pad_count;
        bad |= *p++; /* Must be zero */
    } else {
        bad |= *p++ ^ ALI_ALGO_RSA_SIGN;

        /* Get padding len, but always read till end of buffer
         * (minus one, for the 00 byte) */
        for (i = 0; i < ilen - 3; i++) {
            pad_done |= (p[i] != 0xFF);
            pad_count += (pad_done == 0);
        }

        p += pad_count;
        bad |= *p++; /* Must be zero */
    }

    bad |= (pad_count < 8);

    if (bad)
        return (ALI_ALGO_ERR_RSA_INVALID_PADDING);

    if (ilen - (p - buf) > output_max_len)
        return (ALI_ALGO_ERR_RSA_OUTPUT_TOO_LARGE);

    *olen = ilen - (p - buf);
    memcpy(output, p, *olen);

    return (0);
}
#endif /* ALI_ALGO_PKCS1_V15 */

/*
 * Do an RSA operation, then remove the message padding
 */
int ali_algo_rsa_pkcs1_decrypt(ali_algo_rsa_context *ctx,
                              int (*f_rng)(void *, unsigned char *, size_t),
                              void *p_rng, int mode, size_t *olen,
                              const unsigned char *input, unsigned char *output,
                              size_t output_max_len)
{
    switch (ctx->padding) {
#if defined(ALI_ALGO_PKCS1_V15)
        case ALI_ALGO_RSA_PKCS_V15:
            return ali_algo_rsa_rsaes_pkcs1_v15_decrypt(
              ctx, f_rng, p_rng, mode, olen, input, output, output_max_len);
#endif

#if defined(ALI_ALGO_PKCS1_V21)
        case ALI_ALGO_RSA_PKCS_V21:
            return ali_algo_rsa_rsaes_oaep_decrypt(ctx, f_rng, p_rng, mode, NULL,
                                                  0, olen, input, output,
                                                  output_max_len);
#endif

        default:
            return (ALI_ALGO_ERR_RSA_INVALID_PADDING);
    }
}

#if defined(ALI_ALGO_PKCS1_V21)
/*
 * Implementation of the PKCS#1 v2.1 RSASSA-PSS-SIGN function
 */
int ali_algo_rsa_rsassa_pss_sign(ali_algo_rsa_context *ctx,
                                int (*f_rng)(void *, unsigned char *, size_t),
                                void *p_rng, int mode, ali_algo_md_type_t md_alg,
                                unsigned int hashlen, const unsigned char *hash,
                                unsigned char *sig)
{
    size_t                   olen;
    unsigned char *          p = sig;
    unsigned char            salt[ALI_ALGO_MD_MAX_SIZE];
    unsigned int             slen, hlen, offset = 0;
    int                      ret;
    size_t                   msb;
    const ali_algo_md_info_t *md_info;
    ali_algo_hash_context_t   md_ctx;

    if (mode == ALI_ALGO_RSA_PRIVATE && ctx->padding != ALI_ALGO_RSA_PKCS_V21)
        return (ALI_ALGO_ERR_RSA_BAD_INPUT_DATA);

    if (f_rng == NULL)
        return (ALI_ALGO_ERR_RSA_BAD_INPUT_DATA);

    olen = ctx->len;

    if (md_alg != ALI_ALGO_MD_NONE) {
        /* Gather length of hash to sign */
        md_info = ali_algo_hash_info_from_type(md_alg);
        if (md_info == NULL)
            return (ALI_ALGO_ERR_RSA_BAD_INPUT_DATA);

        hashlen = ali_algo_hash_get_size(md_info);
    }

    md_info = ali_algo_hash_info_from_type((ali_algo_md_type_t)ctx->hash_id);
    if (md_info == NULL)
        return (ALI_ALGO_ERR_RSA_BAD_INPUT_DATA);

    hlen = ali_algo_hash_get_size(md_info);
    slen = hlen;

    if (olen < hlen + slen + 2)
        return (ALI_ALGO_ERR_RSA_BAD_INPUT_DATA);

    memset(sig, 0, olen);

    /* Generate salt of length slen */
    if ((ret = f_rng(p_rng, salt, slen)) != 0)
        return (ALI_ALGO_ERR_RSA_RNG_FAILED + ret);

    /* Note: EMSA-PSS encoding is over the length of N - 1 bits */
    msb = ali_algo_mpi_bitlen(&ctx->N) - 1;
    p += olen - hlen * 2 - 2;
    *p++ = 0x01;
    memcpy(p, salt, slen);
    p += slen;

    ali_algo_hash_init(&md_ctx);
    if ((ret = ali_algo_hash_setup(&md_ctx, md_info, 0)) != 0) {
        ali_algo_hash_free(&md_ctx);
        return (ret);
    }

    /* Generate H = Hash( M' ) */
    ali_algo_hash_starts(&md_ctx);
    ali_algo_hash_update(&md_ctx, p, 8);
    ali_algo_hash_update(&md_ctx, hash, hashlen);
    ali_algo_hash_update(&md_ctx, salt, slen);
    ali_algo_hash_finish(&md_ctx, p);

    /* Compensate for boundary condition when applying mask */
    if (msb % 8 == 0)
        offset = 1;

    /* maskedDB: Apply dbMask to DB */
    mgf_mask(sig + offset, olen - hlen - 1 - offset, p, hlen, &md_ctx);

    ali_algo_hash_free(&md_ctx);

    msb = ali_algo_mpi_bitlen(&ctx->N) - 1;
    sig[0] &= 0xFF >> (olen * 8 - msb);

    p += hlen;
    *p++ = 0xBC;

    return ((mode == ALI_ALGO_RSA_PUBLIC)
              ? ali_algo_rsa_public(ctx, sig, sig)
              : ali_algo_rsa_private(ctx, f_rng, p_rng, sig, sig));
}
#endif /* ALI_ALGO_PKCS1_V21 */

#if defined(ALI_ALGO_PKCS1_V15)
/*
 * Implementation of the PKCS#1 v2.1 RSASSA-PKCS1-V1_5-SIGN function
 */
/*
 * Do an RSA operation to sign the message digest
 */
int ali_algo_rsa_rsassa_pkcs1_v15_sign(
  ali_algo_rsa_context *ctx, int (*f_rng)(void *, unsigned char *, size_t),
  void *p_rng, int mode, ali_algo_md_type_t md_alg, unsigned int hashlen,
  const unsigned char *hash, unsigned char *sig)
{
    size_t                 nb_pad, olen, oid_size = 0;
    unsigned char *        p       = sig;
    const char *           oid     = NULL;
    unsigned char *        sig_try = NULL, *verif = NULL;
    size_t                 i;
    unsigned char          diff;
    volatile unsigned char diff_no_optimize;
    int                    ret;

    if (mode == ALI_ALGO_RSA_PRIVATE && ctx->padding != ALI_ALGO_RSA_PKCS_V15)
        return (ALI_ALGO_ERR_RSA_BAD_INPUT_DATA);

    olen   = ctx->len;
    nb_pad = olen - 3;

    if (md_alg != ALI_ALGO_MD_NONE) {
        const ali_algo_md_info_t *md_info = ali_algo_hash_info_from_type(md_alg);
        if (md_info == NULL)
            return (ALI_ALGO_ERR_RSA_BAD_INPUT_DATA);

        if (ali_algo_oid_get_oid_by_md(md_alg, &oid, &oid_size) != 0)
            return (ALI_ALGO_ERR_RSA_BAD_INPUT_DATA);

        nb_pad -= 10 + oid_size;

        hashlen = ali_algo_hash_get_size(md_info);
    }

    nb_pad -= hashlen;

    if ((nb_pad < 8) || (nb_pad > olen))
        return (ALI_ALGO_ERR_RSA_BAD_INPUT_DATA);

    *p++ = 0;
    *p++ = ALI_ALGO_RSA_SIGN;
    memset(p, 0xFF, nb_pad);
    p += nb_pad;
    *p++ = 0;

    if (md_alg == ALI_ALGO_MD_NONE) {
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
        *p++ = ALI_ALGO_ASN1_SEQUENCE | ALI_ALGO_ASN1_CONSTRUCTED;
        *p++ = (unsigned char)(0x08 + oid_size + hashlen);
        *p++ = ALI_ALGO_ASN1_SEQUENCE | ALI_ALGO_ASN1_CONSTRUCTED;
        *p++ = (unsigned char)(0x04 + oid_size);
        *p++ = ALI_ALGO_ASN1_OID;
        *p++ = oid_size & 0xFF;
        memcpy(p, oid, oid_size);
        p += oid_size;
        *p++ = ALI_ALGO_ASN1_NULL;
        *p++ = 0x00;
        *p++ = ALI_ALGO_ASN1_OCTET_STRING;
        *p++ = hashlen;
        memcpy(p, hash, hashlen);
    }

    if (mode == ALI_ALGO_RSA_PUBLIC)
        return (ali_algo_rsa_public(ctx, sig, sig));

    /*
     * In order to prevent Lenstra's attack, make the signature in a
     * temporary buffer and check it before returning it.
     */
    sig_try = osa_calloc(1, ctx->len);
    if (sig_try == NULL)
        return (ALI_ALGO_ERR_MPI_ALLOC_FAILED);

    verif = osa_calloc(1, ctx->len);
    if (verif == NULL) {
        osa_free(sig_try);
        return (ALI_ALGO_ERR_MPI_ALLOC_FAILED);
    }

    ALI_ALGO_MPI_CHK(ali_algo_rsa_private(ctx, f_rng, p_rng, sig, sig_try));
    ALI_ALGO_MPI_CHK(ali_algo_rsa_public(ctx, sig_try, verif));

    /* Compare in constant time just in case */
    for (diff = 0, i = 0; i < ctx->len; i++)
        diff |= verif[i] ^ sig[i];
    diff_no_optimize = diff;

    if (diff_no_optimize != 0) {
        ret = ALI_ALGO_ERR_RSA_PRIVATE_FAILED;
        goto cleanup;
    }

    osa_memcpy(sig, sig_try, ctx->len);

cleanup:
    osa_free(sig_try);
    osa_free(verif);

    return (ret);
}
#endif /* ALI_ALGO_PKCS1_V15 */

/*
 * Do an RSA operation to sign the message digest
 */
int ali_algo_rsa_pkcs1_sign(ali_algo_rsa_context *ctx,
                           int (*f_rng)(void *, unsigned char *, size_t),
                           void *p_rng, int mode, ali_algo_md_type_t md_alg,
                           unsigned int hashlen, const unsigned char *hash,
                           unsigned char *sig)
{
    switch (ctx->padding) {
#if defined(ALI_ALGO_PKCS1_V15)
        case ALI_ALGO_RSA_PKCS_V15:
            return ali_algo_rsa_rsassa_pkcs1_v15_sign(
              ctx, f_rng, p_rng, mode, md_alg, hashlen, hash, sig);
#endif

#if defined(ALI_ALGO_PKCS1_V21)
        case ALI_ALGO_RSA_PKCS_V21:
            return ali_algo_rsa_rsassa_pss_sign(ctx, f_rng, p_rng, mode, md_alg,
                                               hashlen, hash, sig);
#endif

        default:
            return (ALI_ALGO_ERR_RSA_INVALID_PADDING);
    }
}

#if defined(ALI_ALGO_PKCS1_V21)
/*
 * Implementation of the PKCS#1 v2.1 RSASSA-PSS-VERIFY function
 */
int ali_algo_rsa_rsassa_pss_verify_ext(
  ali_algo_rsa_context *ctx, int (*f_rng)(void *, unsigned char *, size_t),
  void *p_rng, int mode, ali_algo_md_type_t md_alg, unsigned int hashlen,
  const unsigned char *hash, ali_algo_md_type_t mgf1_hash_id,
  int expected_salt_len, const unsigned char *sig)
{
    int                      ret;
    size_t                   siglen;
    unsigned char *          p;
    unsigned char            result[ALI_ALGO_MD_MAX_SIZE];
    unsigned char            zeros[8];
    unsigned int             hlen;
    size_t                   slen, msb;
    const ali_algo_md_info_t *md_info;
    ali_algo_hash_context_t   md_ctx;
    unsigned char            buf[ALI_ALGO_MPI_MAX_SIZE];

    if (mode == ALI_ALGO_RSA_PRIVATE && ctx->padding != ALI_ALGO_RSA_PKCS_V21)
        return (ALI_ALGO_ERR_RSA_BAD_INPUT_DATA);

    siglen = ctx->len;

    if (siglen < 16 || siglen > sizeof(buf))
        return (ALI_ALGO_ERR_RSA_BAD_INPUT_DATA);

    ret = (mode == ALI_ALGO_RSA_PUBLIC)
            ? ali_algo_rsa_public(ctx, sig, buf)
            : ali_algo_rsa_private(ctx, f_rng, p_rng, sig, buf);

    if (ret != 0)
        return (ret);

    p = buf;

    if (buf[siglen - 1] != 0xBC)
        return (ALI_ALGO_ERR_RSA_INVALID_PADDING);

    if (md_alg != ALI_ALGO_MD_NONE) {
        /* Gather length of hash to sign */
        md_info = ali_algo_hash_info_from_type(md_alg);
        if (md_info == NULL)
            return (ALI_ALGO_ERR_RSA_BAD_INPUT_DATA);

        hashlen = ali_algo_hash_get_size(md_info);
    }

    md_info = ali_algo_hash_info_from_type(mgf1_hash_id);
    if (md_info == NULL)
        return (ALI_ALGO_ERR_RSA_BAD_INPUT_DATA);

    hlen = ali_algo_hash_get_size(md_info);
    slen = siglen - hlen - 1; /* Currently length of salt + padding */

    memset(zeros, 0, 8);

    /*
     * Note: EMSA-PSS verification is over the length of N - 1 bits
     */
    msb = ali_algo_mpi_bitlen(&ctx->N) - 1;

    /* Compensate for boundary condition when applying mask */
    if (msb % 8 == 0) {
        p++;
        siglen -= 1;
    }
    if (buf[0] >> (8 - siglen * 8 + msb))
        return (ALI_ALGO_ERR_RSA_BAD_INPUT_DATA);

    ali_algo_hash_init(&md_ctx);
    if ((ret = ali_algo_hash_setup(&md_ctx, md_info, 0)) != 0) {
        ali_algo_hash_free(&md_ctx);
        return (ret);
    }

    mgf_mask(p, siglen - hlen - 1, p + siglen - hlen - 1, hlen, &md_ctx);

    buf[0] &= 0xFF >> (siglen * 8 - msb);

    while (p < buf + siglen && *p == 0)
        p++;

    if (p == buf + siglen || *p++ != 0x01) {
        ali_algo_hash_free(&md_ctx);
        return (ALI_ALGO_ERR_RSA_INVALID_PADDING);
    }

    /* Actual salt len */
    slen -= p - buf;

    if (expected_salt_len != ALI_ALGO_RSA_SALT_LEN_ANY &&
        slen != (size_t)expected_salt_len) {
        ali_algo_hash_free(&md_ctx);
        return (ALI_ALGO_ERR_RSA_INVALID_PADDING);
    }

    /*
     * Generate H = Hash( M' )
     */
    ali_algo_hash_starts(&md_ctx);
    ali_algo_hash_update(&md_ctx, zeros, 8);
    ali_algo_hash_update(&md_ctx, hash, hashlen);
    ali_algo_hash_update(&md_ctx, p, slen);
    ali_algo_hash_finish(&md_ctx, result);

    ali_algo_hash_free(&md_ctx);

    if (memcmp(p + slen, result, hlen) == 0)
        return (0);
    else
        return (ALI_ALGO_ERR_RSA_VERIFY_FAILED);
}

/*
 * Simplified PKCS#1 v2.1 RSASSA-PSS-VERIFY function
 */
int ali_algo_rsa_rsassa_pss_verify(
  ali_algo_rsa_context *ctx, int (*f_rng)(void *, unsigned char *, size_t),
  void *p_rng, int mode, ali_algo_md_type_t md_alg, unsigned int hashlen,
  const unsigned char *hash, const unsigned char *sig)
{
    ali_algo_md_type_t mgf1_hash_id = (ctx->hash_id != ALI_ALGO_MD_NONE)
                                       ? (ali_algo_md_type_t)ctx->hash_id
                                       : md_alg;

    return (ali_algo_rsa_rsassa_pss_verify_ext(ctx, f_rng, p_rng, mode, md_alg,
                                              hashlen, hash, mgf1_hash_id,
                                              ALI_ALGO_RSA_SALT_LEN_ANY, sig));
}
#endif /* ALI_ALGO_PKCS1_V21 */

#if defined(ALI_ALGO_PKCS1_V15)
/*
 * Implementation of the PKCS#1 v2.1 RSASSA-PKCS1-v1_5-VERIFY function
 */
int ali_algo_rsa_rsassa_pkcs1_v15_verify(
  ali_algo_rsa_context *ctx, int (*f_rng)(void *, unsigned char *, size_t),
  void *p_rng, int mode, ali_algo_md_type_t md_alg, unsigned int hashlen,
  const unsigned char *hash, const unsigned char *sig)
{
    int                      ret;
    size_t                   len, siglen, asn1_len;
    unsigned char *          p, *end;
    ali_algo_md_type_t        msg_md_alg;
    const ali_algo_md_info_t *md_info;
    ali_algo_asn1_buf         oid;
    unsigned char            buf[ALI_ALGO_MPI_MAX_SIZE];

    if (mode == ALI_ALGO_RSA_PRIVATE && ctx->padding != ALI_ALGO_RSA_PKCS_V15)
        return (ALI_ALGO_ERR_RSA_BAD_INPUT_DATA);

    siglen = ctx->len;

    if (siglen < 16 || siglen > sizeof(buf))
        return (ALI_ALGO_ERR_RSA_BAD_INPUT_DATA);

    ret = (mode == ALI_ALGO_RSA_PUBLIC)
            ? ali_algo_rsa_public(ctx, sig, buf)
            : ali_algo_rsa_private(ctx, f_rng, p_rng, sig, buf);

    if (ret != 0)
        return (ret);

    p = buf;

    if (*p++ != 0 || *p++ != ALI_ALGO_RSA_SIGN)
        return (ALI_ALGO_ERR_RSA_INVALID_PADDING);

    while (*p != 0) {
        if (p >= buf + siglen - 1 || *p != 0xFF)
            return (ALI_ALGO_ERR_RSA_INVALID_PADDING);
        p++;
    }
    p++;

    len = siglen - (p - buf);

    if (len == hashlen && md_alg == ALI_ALGO_MD_NONE) {
        if (memcmp(p, hash, hashlen) == 0)
            return (0);
        else
            return (ALI_ALGO_ERR_RSA_VERIFY_FAILED);
    }

    md_info = ali_algo_hash_info_from_type(md_alg);
    if (md_info == NULL)
        return (ALI_ALGO_ERR_RSA_BAD_INPUT_DATA);
    hashlen = ali_algo_hash_get_size(md_info);

    end = p + len;

    /*
     * Parse the ASN.1 structure inside the PKCS#1 v1.5 structure
     */
    if ((ret = ali_algo_asn1_get_tag(&p, end, &asn1_len,
                                    ALI_ALGO_ASN1_CONSTRUCTED |
                                      ALI_ALGO_ASN1_SEQUENCE)) != 0)
        return (ALI_ALGO_ERR_RSA_VERIFY_FAILED);

    if (asn1_len + 2 != len)
        return (ALI_ALGO_ERR_RSA_VERIFY_FAILED);

    if ((ret = ali_algo_asn1_get_tag(&p, end, &asn1_len,
                                    ALI_ALGO_ASN1_CONSTRUCTED |
                                      ALI_ALGO_ASN1_SEQUENCE)) != 0)
        return (ALI_ALGO_ERR_RSA_VERIFY_FAILED);

    if (asn1_len + 6 + hashlen != len)
        return (ALI_ALGO_ERR_RSA_VERIFY_FAILED);

    if ((ret = ali_algo_asn1_get_tag(&p, end, &oid.len, ALI_ALGO_ASN1_OID)) != 0)
        return (ALI_ALGO_ERR_RSA_VERIFY_FAILED);

    oid.p = p;
    p += oid.len;

    if (ali_algo_oid_get_md_alg(&oid, &msg_md_alg) != 0)
        return (ALI_ALGO_ERR_RSA_VERIFY_FAILED);

    if (md_alg != msg_md_alg)
        return (ALI_ALGO_ERR_RSA_VERIFY_FAILED);

    /*
     * assume the algorithm parameters must be NULL
     */
    if ((ret = ali_algo_asn1_get_tag(&p, end, &asn1_len, ALI_ALGO_ASN1_NULL)) !=
        0)
        return (ALI_ALGO_ERR_RSA_VERIFY_FAILED);

    if ((ret = ali_algo_asn1_get_tag(&p, end, &asn1_len,
                                    ALI_ALGO_ASN1_OCTET_STRING)) != 0)
        return (ALI_ALGO_ERR_RSA_VERIFY_FAILED);

    if (asn1_len != hashlen)
        return (ALI_ALGO_ERR_RSA_VERIFY_FAILED);

    if (memcmp(p, hash, hashlen) != 0)
        return (ALI_ALGO_ERR_RSA_VERIFY_FAILED);

    p += hashlen;

    if (p != end)
        return (ALI_ALGO_ERR_RSA_VERIFY_FAILED);

    return (0);
}
#endif /* ALI_ALGO_PKCS1_V15 */

/*
 * Do an RSA operation and check the message digest
 */
int ali_algo_rsa_pkcs1_verify(ali_algo_rsa_context *ctx,
                             int (*f_rng)(void *, unsigned char *, size_t),
                             void *p_rng, int mode, ali_algo_md_type_t md_alg,
                             unsigned int hashlen, const unsigned char *hash,
                             const unsigned char *sig)
{
    switch (ctx->padding) {
#if defined(ALI_ALGO_PKCS1_V15)
        case ALI_ALGO_RSA_PKCS_V15:
            return ali_algo_rsa_rsassa_pkcs1_v15_verify(
              ctx, f_rng, p_rng, mode, md_alg, hashlen, hash, sig);
#endif

#if defined(ALI_ALGO_PKCS1_V21)
        case ALI_ALGO_RSA_PKCS_V21:
            return ali_algo_rsa_rsassa_pss_verify(ctx, f_rng, p_rng, mode,
                                                 md_alg, hashlen, hash, sig);
#endif

        default:
            return (ALI_ALGO_ERR_RSA_INVALID_PADDING);
    }
}

/*
 * Copy the components of an RSA key
 */
int ali_algo_rsa_copy(ali_algo_rsa_context *dst, const ali_algo_rsa_context *src)
{
    int ret;

    dst->ver = src->ver;
    dst->len = src->len;

    ALI_ALGO_MPI_CHK(ali_algo_mpi_copy(&dst->N, &src->N));
    ALI_ALGO_MPI_CHK(ali_algo_mpi_copy(&dst->E, &src->E));

    ALI_ALGO_MPI_CHK(ali_algo_mpi_copy(&dst->D, &src->D));
    ALI_ALGO_MPI_CHK(ali_algo_mpi_copy(&dst->P, &src->P));
    ALI_ALGO_MPI_CHK(ali_algo_mpi_copy(&dst->Q, &src->Q));
    ALI_ALGO_MPI_CHK(ali_algo_mpi_copy(&dst->DP, &src->DP));
    ALI_ALGO_MPI_CHK(ali_algo_mpi_copy(&dst->DQ, &src->DQ));
    ALI_ALGO_MPI_CHK(ali_algo_mpi_copy(&dst->QP, &src->QP));

    ALI_ALGO_MPI_CHK(ali_algo_mpi_copy(&dst->RN, &src->RN));
    ALI_ALGO_MPI_CHK(ali_algo_mpi_copy(&dst->RP, &src->RP));
    ALI_ALGO_MPI_CHK(ali_algo_mpi_copy(&dst->RQ, &src->RQ));

    ALI_ALGO_MPI_CHK(ali_algo_mpi_copy(&dst->Vi, &src->Vi));
    ALI_ALGO_MPI_CHK(ali_algo_mpi_copy(&dst->Vf, &src->Vf));

    dst->padding = src->padding;
    dst->hash_id = src->hash_id;

cleanup:
    if (ret != 0)
        ali_algo_rsa_free(dst);

    return (ret);
}

/*
 * Free the components of an RSA key
 */
void ali_algo_rsa_free(ali_algo_rsa_context *ctx)
{
    ali_algo_mpi_free(&ctx->Vi);
    ali_algo_mpi_free(&ctx->Vf);
    ali_algo_mpi_free(&ctx->RQ);
    ali_algo_mpi_free(&ctx->RP);
    ali_algo_mpi_free(&ctx->RN);
    ali_algo_mpi_free(&ctx->QP);
    ali_algo_mpi_free(&ctx->DQ);
    ali_algo_mpi_free(&ctx->DP);
    ali_algo_mpi_free(&ctx->Q);
    ali_algo_mpi_free(&ctx->P);
    ali_algo_mpi_free(&ctx->D);
    ali_algo_mpi_free(&ctx->E);
    ali_algo_mpi_free(&ctx->N);
#if CONFIG_MULTH_SUPPORT
    ls_osa_mutex_destroy(&ctx->mutex);
#endif
}

#if defined(ALI_ALGO_SELF_TEST)

#include "../include/alicrypto_algo/sha1.h"

/*
 * Example RSA-1024 keypair, for test purposes
 */
#define KEY_LEN 128

#define RSA_N                          \
    "9292758453063D803DD603D5E777D788" \
    "8ED1D5BF35786190FA2F23EBC0848AEA" \
    "DDA92CA6C3D80B32C4D109BE0F36D6AE" \
    "7130B9CED7ACDF54CFC7555AC14EEBAB" \
    "93A89813FBF3C4F8066D2D800F7C38A8" \
    "1AE31942917403FF4946B0A83D3D3E05" \
    "EE57C6F5F5606FB5D4BC6CD34EE0801A" \
    "5E94BB77B07507233A0BC7BAC8F90F79"

#define RSA_E "10001"

#define RSA_D                          \
    "24BF6185468786FDD303083D25E64EFC" \
    "66CA472BC44D253102F8B4A9D3BFA750" \
    "91386C0077937FE33FA3252D28855837" \
    "AE1B484A8A9A45F7EE8C0C634F99E8CD" \
    "DF79C5CE07EE72C7F123142198164234" \
    "CABB724CF78B8173B9F880FC86322407" \
    "AF1FEDFDDE2BEB674CA15F3E81A1521E" \
    "071513A1E85B5DFA031F21ECAE91A34D"

#define RSA_P                          \
    "C36D0EB7FCD285223CFB5AABA5BDA3D8" \
    "2C01CAD19EA484A87EA4377637E75500" \
    "FCB2005C5C7DD6EC4AC023CDA285D796" \
    "C3D9E75E1EFC42488BB4F1D13AC30A57"

#define RSA_Q                          \
    "C000DF51A7C77AE8D7C7370C1FF55B69" \
    "E211C2B9E5DB1ED0BF61D0D9899620F4" \
    "910E4168387E3C30AA1E00C339A79508" \
    "8452DD96A9A5EA5D9DCA68DA636032AF"

#define RSA_DP                         \
    "C1ACF567564274FB07A0BBAD5D26E298" \
    "3C94D22288ACD763FD8E5600ED4A702D" \
    "F84198A5F06C2E72236AE490C93F07F8" \
    "3CC559CD27BC2D1CA488811730BB5725"

#define RSA_DQ                         \
    "4959CBF6F8FEF750AEE6977C155579C7" \
    "D8AAEA56749EA28623272E4F7D0592AF" \
    "7C1F1313CAC9471B5C523BFE592F517B" \
    "407A1BD76C164B93DA2D32A383E58357"

#define RSA_QP                         \
    "9AE7FBC99546432DF71896FC239EADAE" \
    "F38D18D2B2F0E2DD275AA977E2BF4411" \
    "F5A3B2A5D33605AEBBCCBA7FEB9F2D2F" \
    "A74206CEC169D74BF5A8C50D6F48EA08"

#define PT_LEN 24
#define RSA_PT                                         \
    "\xAA\xBB\xCC\x03\x02\x01\x00\xFF\xFF\xFF\xFF\xFF" \
    "\x11\x22\x33\x0A\x0B\x0C\xCC\xDD\xDD\xDD\xDD\xDD"

#if defined(ALI_ALGO_PKCS1_V15)
static int myrand(void *rng_state, unsigned char *output, size_t len)
{
    size_t i;

    if (rng_state != NULL)
        rng_state = NULL;

    for (i = 0; i < len; ++i)
        output[i] = rand();
    return (0);
}
#endif /* ALI_ALGO_PKCS1_V15 */

/*
 * Checkup routine
 */
int ali_algo_rsa_self_test(int verbose)
{
    int ret = 0;
#if defined(ALI_ALGO_PKCS1_V15)
    size_t              len;
    ali_algo_rsa_context rsa;
    unsigned char       rsa_plaintext[PT_LEN];
    unsigned char       rsa_decrypted[PT_LEN];
    unsigned char       rsa_ciphertext[KEY_LEN];
#if defined(ALI_ALGO_SHA1_C)
    unsigned char sha1sum[20];
#endif

    ali_algo_rsa_init(&rsa, ALI_ALGO_RSA_PKCS_V15, 0);

    rsa.len = KEY_LEN;
    ALI_ALGO_MPI_CHK(ali_algo_mpi_read_string(&rsa.N, 16, RSA_N));
    ALI_ALGO_MPI_CHK(ali_algo_mpi_read_string(&rsa.E, 16, RSA_E));
    ALI_ALGO_MPI_CHK(ali_algo_mpi_read_string(&rsa.D, 16, RSA_D));
    ALI_ALGO_MPI_CHK(ali_algo_mpi_read_string(&rsa.P, 16, RSA_P));
    ALI_ALGO_MPI_CHK(ali_algo_mpi_read_string(&rsa.Q, 16, RSA_Q));
    ALI_ALGO_MPI_CHK(ali_algo_mpi_read_string(&rsa.DP, 16, RSA_DP));
    ALI_ALGO_MPI_CHK(ali_algo_mpi_read_string(&rsa.DQ, 16, RSA_DQ));
    ALI_ALGO_MPI_CHK(ali_algo_mpi_read_string(&rsa.QP, 16, RSA_QP));

    if (verbose != 0)
        osa_printf("  RSA key validation: ");

    if (ali_algo_rsa_check_pubkey(&rsa) != 0 ||
        ali_algo_rsa_check_privkey(&rsa) != 0) {
        if (verbose != 0)
            osa_printf("failed\n");

        return (1);
    }

    if (verbose != 0)
        osa_printf("passed\n  PKCS#1 encryption : ");

    memcpy(rsa_plaintext, RSA_PT, PT_LEN);

    if (ali_algo_rsa_pkcs1_encrypt(&rsa, myrand, NULL, ALI_ALGO_RSA_PUBLIC,
                                  PT_LEN, rsa_plaintext, rsa_ciphertext) != 0) {
        if (verbose != 0)
            osa_printf("failed\n");

        return (1);
    }

    if (verbose != 0)
        osa_printf("passed\n  PKCS#1 decryption : ");

    if (ali_algo_rsa_pkcs1_decrypt(&rsa, myrand, NULL, ALI_ALGO_RSA_PRIVATE, &len,
                                  rsa_ciphertext, rsa_decrypted,
                                  sizeof(rsa_decrypted)) != 0) {
        if (verbose != 0)
            osa_printf("failed\n");

        return (1);
    }

    if (memcmp(rsa_decrypted, rsa_plaintext, len) != 0) {
        if (verbose != 0)
            osa_printf("failed\n");

        return (1);
    }

    if (verbose != 0)
        osa_printf("passed\n");

#if defined(ALI_ALGO_SHA1_C)
    if (verbose != 0)
        osa_printf("  PKCS#1 data sign  : ");

    ali_algo_sha1(rsa_plaintext, PT_LEN, sha1sum);

    if (ali_algo_rsa_pkcs1_sign(&rsa, myrand, NULL, ALI_ALGO_RSA_PRIVATE,
                               ALI_ALGO_MD_SHA1, 0, sha1sum,
                               rsa_ciphertext) != 0) {
        if (verbose != 0)
            osa_printf("failed\n");

        return (1);
    }

    if (verbose != 0)
        osa_printf("passed\n  PKCS#1 sig. verify: ");

    if (ali_algo_rsa_pkcs1_verify(&rsa, NULL, NULL, ALI_ALGO_RSA_PUBLIC,
                                 ALI_ALGO_MD_SHA1, 0, sha1sum,
                                 rsa_ciphertext) != 0) {
        if (verbose != 0)
            osa_printf("failed\n");

        return (1);
    }

    if (verbose != 0)
        osa_printf("passed\n");
#endif /* ALI_ALGO_SHA1_C */

    if (verbose != 0)
        osa_printf("\n");

cleanup:
    ali_algo_rsa_free(&rsa);
#else  /* ALI_ALGO_PKCS1_V15 */
    ((void)verbose);
#endif /* ALI_ALGO_PKCS1_V15 */
    return (ret);
}

#endif /* ALI_ALGO_SELF_TEST */

#endif /* ALI_ALGO_RSA_C */
