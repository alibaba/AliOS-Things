/*
 * Copyright (C) 2016-2019 Alibaba Group Holding Limited
 */

#include "ls_hal.h"
#include "rsa.h"

int ls_hal_rsa_get_ctx_size(void)
{
    return sizeof(impl_rsa_context);
}

int ls_hal_rsa_init(const void *context)
{
    int result = 0;
    impl_rsa_context *ctx;
    if (context == NULL) {
        LS_HAL_LOG("ls_hal_rsa_init failed\n");
        return HAL_CRYPT_BAD_PARAMETERS;
    }
    ctx = (impl_rsa_context *) context;
    impl_rsa_init(ctx, 0, 0);
    return result;
}

int ls_hal_rsa_init_pubkey(void *context, size_t keybits,
                           const uint8_t *n, size_t n_size,
                           const uint8_t *e, size_t e_size)
{
    impl_rsa_context *ctx;
    int ret = 0;

    if (context == NULL) {
        LS_HAL_LOG("invalid ctx\n");
        return HAL_CRYPT_INVALID_CONTEXT;
    }

    ctx = (impl_rsa_context *)context;
    ctx->len = keybits >> 3;
    // init n/e
    ctx->n = n;
    ctx->n_size = n_size;
    ctx->e = e;
    ctx->e_size = e_size;

    if (ret) {
        LS_HAL_LOG("failed(0x%08x)\n", ret);
        return HAL_CRYPT_ERROR;
    }
    return ret;
}

int ls_hal_rsa_init_keypair(void *context, size_t keybits,
                            const uint8_t *n, size_t n_size,
                            const uint8_t *e, size_t e_size,
                            const uint8_t *d, size_t d_size,
                            const uint8_t *p, size_t p_size,
                            const uint8_t *q, size_t q_size,
                            const uint8_t *dp, size_t dp_size,
                            const uint8_t *dq, size_t dq_size,
                            const uint8_t *qp, size_t qp_size)
{
    impl_rsa_context *ctx;
    int ret = 0;

    if (context == NULL) {
        LS_HAL_LOG("invalid ctx\n");
        return HAL_CRYPT_INVALID_CONTEXT;
    }

    ctx = (impl_rsa_context *)context;
    ctx->len = keybits >> 3;

    ctx->n = n;
    ctx->n_size = n_size;
    ctx->d = d;
    ctx->d_size = d_size;
    ctx->p = p;
    ctx->p_size = p_size;
    ctx->q = q;
    ctx->q_size = q_size;
    ctx->dp = d;
    ctx->dp_size = d_size;
    ctx->dq = p;
    ctx->dq_size = p_size;
    ctx->qp = q;
    ctx->qp_size = q_size;

#if 0
    HAL_MPI_CHK(impl_mpi_read_binary(&(ctx->N), n, n_size));
    HAL_MPI_CHK(impl_mpi_read_binary(&(ctx->D), d, d_size));
    HAL_MPI_CHK(impl_mpi_read_binary(&(ctx->P), p, p_size));
    HAL_MPI_CHK(impl_mpi_read_binary(&(ctx->Q), q, q_size));
    HAL_MPI_CHK(impl_mpi_read_binary(&(ctx->DP), d, d_size));
    HAL_MPI_CHK(impl_mpi_read_binary(&(ctx->DQ), p, p_size));
    HAL_MPI_CHK(impl_mpi_read_binary(&(ctx->QP), q, q_size));
#endif

    if (ret) {
        LS_HAL_LOG("failed(0x%08x)\n", ret);
        return HAL_CRYPT_ERROR;
    }
    return ret;
}

int ls_hal_rsa_public(const void *context,
                      const uint8_t *src, uint8_t *dst,
                      size_t size)
{
    int ret = 0;
    impl_rsa_context *ctx;
      
    if (context == NULL) {
        LS_HAL_LOG("invalid ctx\n");
        return HAL_CRYPT_INVALID_CONTEXT;
    }

    ctx = (impl_rsa_context *)context;

    ret = impl_rsa_public(ctx, src, dst);
    if (ret) {
        LS_HAL_LOG("failed(0x%08x)\n", ret);
        return HAL_CRYPT_ERROR;
    }

    return HAL_CRYPT_SUCCESS;
}

int ls_hal_rsa_private(const void *context,
           int (*f_rng)(void *, uint8_t *, size_t),
           const uint8_t *src, uint8_t *dst, size_t size)
{
    int ret = 0;
    impl_rsa_context *ctx;

    if (context == NULL) {
        LS_HAL_LOG("invalid context\n");
        return HAL_CRYPT_INVALID_CONTEXT;
    }

    ctx = (impl_rsa_context *) context;

    ret = impl_rsa_private(ctx, f_rng, NULL, src, dst);
    if (ret) {
        LS_HAL_LOG("failed(0x%08x)\n", ret);
        return HAL_CRYPT_ERROR;
    }

    return HAL_CRYPT_SUCCESS;
}
