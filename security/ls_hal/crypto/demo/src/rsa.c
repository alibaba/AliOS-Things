/**
* Copyright (C) 2017  Alibaba Group Holding Limited.
**/

#include "rsa.h"
#include "config.h"

/*
 * Initialize an RSA context
 */
void impl_rsa_init(impl_rsa_context *ctx, int padding, int hash_id)
{
    memset(ctx, 0, sizeof(impl_rsa_context));

    impl_rsa_set_padding(ctx, padding, hash_id);

#if CONFIG_MULTH_SUPPORT
    ls_osa_mutex_create(&ctx->mutex);
#endif
}

/*
 * Set padding for an existing RSA context
 */
void impl_rsa_set_padding(impl_rsa_context *ctx, int padding, int hash_id)
{
    ctx->padding = padding;
    ctx->hash_id = hash_id;
}

/*
 * Do an RSA public key operation
 */
int impl_rsa_public(impl_rsa_context *ctx, const unsigned char *input,
                       unsigned char *output)
{
    int         ret;
    size_t      olen;
    impl_mpi     T, N, E, RN;

    impl_mpi_init(&T);
    impl_mpi_init(&N);
    impl_mpi_init(&E);
    impl_mpi_init(&RN);

    HAL_MPI_CHK(impl_mpi_read_binary(&N, ctx->n, ctx->n_size));
    HAL_MPI_CHK(impl_mpi_read_binary(&E, ctx->e, ctx->e_size));

#if CONFIG_MULTH_SUPPORT
    ls_osa_mutex_lock(&ctx->mutex);
#endif
    HAL_MPI_CHK(impl_mpi_read_binary(&T, input, ctx->len));

    if (impl_mpi_cmp_mpi(&T, &N) >= 0) {
        ret = HAL_ERR_MPI_BAD_INPUT_DATA;
        goto cleanup;
    }

    olen = ctx->len;
    //HAL_MPI_CHK(impl_mpi_exp_mod(&T, &T, &E, &N, &ctx->RN));
    HAL_MPI_CHK(impl_mpi_exp_mod(&T, &T, &E, &N, &RN));
    HAL_MPI_CHK(impl_mpi_write_binary(&T, output, olen));

cleanup:
#if CONFIG_MULTH_SUPPORT
    ls_osa_mutex_unlock(&ctx->mutex);
#endif
    impl_mpi_free(&T);
    impl_mpi_free(&N);
    impl_mpi_free(&E);
    impl_mpi_free(&RN);

    if (ret != 0)
        return (HAL_ERR_RSA_PUBLIC_FAILED + ret);
    
    return (0);
}

#ifdef CONFIG_ENABLE_BLINDING
// FIXME: enable this cause RSA test fail
/*
 * Generate or update blinding values, see section 10 of:
 *  KOCHER, Paul C. Timing attacks on implementations of Diffie-Hellman, RSA,
 *  DSS, and other systems. In : Advances in Cryptology-CRYPTO'96. Springer
 *  Berlin Heidelberg, 1996. p. 104-113.
 */
static int rsa_prepare_blinding(impl_rsa_context *ctx,
                                int (*f_rng)(void *, unsigned char *, size_t),
                                void *p_rng)
{
    int ret, count = 0;

    if (ctx->Vf.p != NULL) {
        /* We already have blinding values, just update them by squaring */
        HAL_MPI_CHK(impl_mpi_mul_mpi(&ctx->Vi, &ctx->Vi, &ctx->Vi));
        HAL_MPI_CHK(impl_mpi_mod_mpi(&ctx->Vi, &ctx->Vi, &ctx->N));
        HAL_MPI_CHK(impl_mpi_mul_mpi(&ctx->Vf, &ctx->Vf, &ctx->Vf));
        HAL_MPI_CHK(impl_mpi_mod_mpi(&ctx->Vf, &ctx->Vf, &ctx->N));

        goto cleanup;
    }

    /* Unblinding value: Vf = random number, invertible mod N */
    do {
        if (count++ > 10)
            return (HAL_ERR_RSA_RNG_FAILED);

        HAL_MPI_CHK(
          impl_mpi_fill_random(&ctx->Vf, ctx->len - 1, f_rng, p_rng));
        HAL_MPI_CHK(impl_mpi_gcd(&ctx->Vi, &ctx->Vf, &ctx->N));
    } while (impl_mpi_cmp_int(&ctx->Vi, 1) != 0);

    /* Blinding value: Vi =  Vf^(-e) mod N */
    HAL_MPI_CHK(impl_mpi_inv_mod(&ctx->Vi, &ctx->Vf, &ctx->N));
    HAL_MPI_CHK(
      impl_mpi_exp_mod(&ctx->Vi, &ctx->Vi, &ctx->E, &ctx->N, &ctx->RN));


cleanup:
    return (ret);
}
#endif

/*
 * Do an RSA private key operation
 */
int impl_rsa_private(impl_rsa_context *ctx,
                        int (*f_rng)(void *, unsigned char *, size_t),
                        void *p_rng, const unsigned char *input,
                        unsigned char *output)
{
    int         ret;
    size_t      olen;
    impl_mpi T, T1, T2;
    impl_mpi N, D, P, Q, DP, DQ, QP, VI, VF, RN, RP, RQ;

    impl_mpi_init(&T);
    impl_mpi_init(&T1);
    impl_mpi_init(&T2);
    impl_mpi_init(&N);
    impl_mpi_init(&D);
    impl_mpi_init(&P);
    impl_mpi_init(&Q);
    impl_mpi_init(&DP);
    impl_mpi_init(&DQ);
    impl_mpi_init(&QP);
    impl_mpi_init(&VI);
    impl_mpi_init(&VF);
    impl_mpi_init(&RN);
    impl_mpi_init(&RP);
    impl_mpi_init(&RQ);

    HAL_MPI_CHK(impl_mpi_read_binary(&N, ctx->n, ctx->n_size));
    HAL_MPI_CHK(impl_mpi_read_binary(&D, ctx->d, ctx->d_size));
    HAL_MPI_CHK(impl_mpi_read_binary(&P, ctx->p, ctx->p_size));
    HAL_MPI_CHK(impl_mpi_read_binary(&Q, ctx->q, ctx->q_size));
    HAL_MPI_CHK(impl_mpi_read_binary(&DP, ctx->d, ctx->d_size));
    HAL_MPI_CHK(impl_mpi_read_binary(&DQ, ctx->p, ctx->p_size));
    HAL_MPI_CHK(impl_mpi_read_binary(&Q, ctx->q, ctx->q_size));

    /* Make sure we have private key info, prevent possible misuse */
#if defined(ALI_ALGO_RSA_NO_CRT)
    //if (ctx->N.p == NULL || ctx->D.p == NULL)
    if (N.p == NULL || D.p == NULL)
        return (HAL_ERR_RSA_BAD_INPUT_DATA);
#else
    //if (ctx->P.p == NULL || ctx->Q.p == NULL || ctx->D.p == NULL)
    if (P.p == NULL || Q.p == NULL || D.p == NULL)
        return (HAL_ERR_RSA_BAD_INPUT_DATA);
#endif

#if CONFIG_MULTH_SUPPORT
    ls_osa_mutex_lock(&ctx->mutex);
#endif
    HAL_MPI_CHK(impl_mpi_read_binary(&T, input, ctx->len));
    //if (impl_mpi_cmp_mpi(&T, &ctx->N) >= 0) {
    if (impl_mpi_cmp_mpi(&T, &N) >= 0) {
        ret = HAL_ERR_MPI_BAD_INPUT_DATA;
        goto cleanup;
    }

#ifdef CONFIG_ENABLE_BLINDING
    // FIXME: enable blind/unblind cause RSA failed
    if (f_rng != NULL) {
        /*
         * Blinding
         * T = T * Vi mod N
         */
        HAL_MPI_CHK(rsa_prepare_blinding(ctx, f_rng, p_rng));
        //HAL_MPI_CHK(impl_mpi_mul_mpi(&T, &T, &ctx->Vi));
        HAL_MPI_CHK(impl_mpi_mul_mpi(&T, &T, &VI));
        //HAL_MPI_CHK(impl_mpi_mod_mpi(&T, &T, &ctx->N));
        HAL_MPI_CHK(impl_mpi_mod_mpi(&T, &T, &N));
    }
#endif

#if defined(ALI_ALGO_RSA_NO_CRT)
    //HAL_MPI_CHK(impl_mpi_exp_mod(&T, &T, &ctx->D, &ctx->N, &ctx->RN));
    HAL_MPI_CHK(impl_mpi_exp_mod(&T, &T, &D, &N, &RN));
#else
    /*
     * faster decryption using the CRT
     *
     * T1 = input ^ dP mod P
     * T2 = input ^ dQ mod Q
     */
    //HAL_MPI_CHK(impl_mpi_exp_mod(&T1, &T, &ctx->DP, &ctx->P, &ctx->RP));
    HAL_MPI_CHK(impl_mpi_exp_mod(&T1, &T, &DP, &P, &RP));
    //HAL_MPI_CHK(impl_mpi_exp_mod(&T2, &T, &ctx->DQ, &ctx->Q, &ctx->RQ));
    HAL_MPI_CHK(impl_mpi_exp_mod(&T2, &T, &DQ, &Q, &RQ));

    /*
     * T = (T1 - T2) * (Q^-1 mod P) mod P
     */
    HAL_MPI_CHK(impl_mpi_sub_mpi(&T, &T1, &T2));
    //HAL_MPI_CHK(impl_mpi_mul_mpi(&T1, &T, &ctx->QP));
    HAL_MPI_CHK(impl_mpi_mul_mpi(&T1, &T, &QP));
    //HAL_MPI_CHK(impl_mpi_mod_mpi(&T, &T1, &ctx->P));
    HAL_MPI_CHK(impl_mpi_mod_mpi(&T, &T1, &P));

    /*
     * T = T2 + T * Q
     */
    //HAL_MPI_CHK(impl_mpi_mul_mpi(&T1, &T, &ctx->Q));
    HAL_MPI_CHK(impl_mpi_mul_mpi(&T1, &T, &Q));
    HAL_MPI_CHK(impl_mpi_add_mpi(&T, &T2, &T1));
#endif /* ALI_ALGO_RSA_NO_CRT */

#ifdef CONFIG_ENABLE_BLINDING
    // FIXME: enable blind/unblind cause RSA failed
    if (f_rng != NULL) {
        /*
         * Unblind
         * T = T * Vf mod N
         */
        //HAL_MPI_CHK(impl_mpi_mul_mpi(&T, &T, &ctx->Vf));
        HAL_MPI_CHK(impl_mpi_mul_mpi(&T, &T, &VF));
        //HAL_MPI_CHK(impl_mpi_mod_mpi(&T, &T, &ctx->N));
        HAL_MPI_CHK(impl_mpi_mod_mpi(&T, &T, &N));
    }
#endif
    olen = ctx->len;
    HAL_MPI_CHK(impl_mpi_write_binary(&T, output, olen));

cleanup:
#if CONFIG_MULTH_SUPPORT
    ls_osa_mutex_unlock(&ctx->mutex);
#endif
    impl_mpi_free(&T);
    impl_mpi_free(&T1);
    impl_mpi_free(&T2);
    impl_mpi_free(&T2);

    impl_mpi_free(&N);
    impl_mpi_free(&D);
    impl_mpi_free(&P);
    impl_mpi_free(&Q);
    impl_mpi_free(&DP);
    impl_mpi_free(&DQ);
    impl_mpi_free(&QP);
    impl_mpi_free(&VI);
    impl_mpi_free(&VF);
    impl_mpi_free(&RN);
    impl_mpi_free(&RP);
    impl_mpi_free(&RQ);

    if (ret != 0)
        return (HAL_ERR_RSA_PRIVATE_FAILED + ret);

    return (0);
}
