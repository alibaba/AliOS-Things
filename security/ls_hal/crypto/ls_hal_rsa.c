/*
 * Copyright (C) 2016-2019 Alibaba Group Holding Limited
 */

#include "ls_hal.h"
#include "rsa.h"

int ls_hal_rsa_get_ctx_size(size_t *size)
{
    if (size == NULL) {
        PRINT_RET(HAL_CRYPT_ERROR_BAD_PARAMETERS, "ls_hal_rsa_get_ctx_size: bad input!\n");
    }
    *size = sizeof(hal_rsa_context);
    return HAL_CRYPT_SUCCESS;
}

int ls_hal_rsa_init(rsa_padding_t padding, void *context)
{
    int result = 0;
    hash_type_t type;
    hal_rsa_context *ctx;
    if(context == NULL){
        PRINT_RET(HAL_CRYPT_ERROR_BAD_PARAMETERS, "ls_hal_rsa_init failed\n");
    }
    ctx = (hal_rsa_context *) context;
    switch (padding.type) {
        case RSAES_PKCS1_V1_5:
            hal_rsa_init(ctx, HAL_RSA_PKCS_V15, HASH_NONE);
            break;
        case RSAES_PKCS1_OAEP_MGF1:
            type = padding.pad.rsaes_oaep.type;
            switch (type) {
                case SHA1:
                    type = HAL_MD_SHA1;
                    break;
                case SHA224:
                    type = HAL_MD_SHA224;
                    break;
                case SHA256:
                    type = HAL_MD_SHA256;
                    break;
                case SHA384:
                    type = HAL_MD_SHA384;
                    break;
                case SHA512:
                    type = HAL_MD_SHA512;
                    break;
                case MD5:
                    type = HAL_MD_MD5;
                    break;
                default:
                    GO_RET(HAL_CRYPT_ERROR_NOT_SUPPORTED, "ls_hal_rsa_init: invalid hash type(%d)\n", type);
            }
            hal_rsa_init(ctx, HAL_RSA_PKCS_V21, type);
            break;
        default:
            GO_RET(HAL_CRYPT_ERROR_NOT_SUPPORTED, "ls_hal_rsa_init: invalid padding type(%d)\n", padding.type);
    }
_OUT:
    return result;
}

int ls_hal_rsa_public(const rsa_pubkey_t *pubkey,
           const uint8_t *src, uint8_t *dst, size_t size, void *context)
{
#ifdef CONFIG_HAL_DEMO
    int ret;
    hal_rsa_context *ctx;
    hal_rsa_pubkey_t *pub_key = (hal_rsa_pubkey_t *) pubkey;
      
    if(context == NULL){
        PRINT_RET(HAL_CRYPT_ERROR_BAD_PARAMETERS, "ls_hal_rsa_public failed\n");
    }
    ctx = (hal_rsa_context *) context;
    ctx->len = pub_key->key_bytes;
    HAL_MPI_CHK(hal_mpi_read_binary(&(ctx->N), pub_key->n, size));
    HAL_MPI_CHK(hal_mpi_read_binary(&(ctx->E), pub_key->e, size));
    ret = hal_rsa_public(ctx, src, dst);
cleanup:
    if (ret != 0) return (HAL_CRYPT_ERROR);
    return (HAL_CRYPT_SUCCESS);
#else
    (void)pub_key;
    (void)src;
    (void)dst;
    (void)size;

    HAL_DBG_LOG("not supported!\n");
    return HAL_CRYPT_ERROR_NOT_SUPPORTED;
#endif
}

int ls_hal_rsa_private(const rsa_keypair_t *privkey,
           int (*f_rng)(void *, uint8_t *, size_t),
           const uint8_t *src, uint8_t *dst, size_t size, void *context)
{
#ifdef CONFIG_HAL_DEMO
    int ret;
    hal_rsa_context *ctx;
    hal_rsa_keypair_t *priv_key = (hal_rsa_keypair_t *) privkey;

    if(context == NULL){
        PRINT_RET(HAL_CRYPT_ERROR_BAD_PARAMETERS, "ls_hal_rsa_private failed\n");
    }
    ctx = (hal_rsa_context *) context;

    ctx->len = priv_key->key_bytes;
    HAL_MPI_CHK(hal_mpi_read_binary(&(ctx->N), priv_key->n, size));
    HAL_MPI_CHK(hal_mpi_read_binary(&(ctx->D), priv_key->d, size));
    HAL_MPI_CHK(hal_mpi_read_binary(&(ctx->P), priv_key->p, size));
    HAL_MPI_CHK(hal_mpi_read_binary(&(ctx->Q), priv_key->q, size));
    HAL_MPI_CHK(hal_mpi_read_binary(&(ctx->DP), priv_key->d, size));
    HAL_MPI_CHK(hal_mpi_read_binary(&(ctx->DQ), priv_key->p, size));
    HAL_MPI_CHK(hal_mpi_read_binary(&(ctx->QP), priv_key->q, size));
    ret = hal_rsa_private(ctx, f_rng, NULL, src, dst); 
cleanup:
    if (ret != 0)
        return (HAL_CRYPT_ERROR);

    return (HAL_CRYPT_SUCCESS);
#else
    (void)priv_key;
    (void)f_rng;
    (void)src;
    (void)dst;
    (void)size;

    HAL_DBG_LOG("not supported!\n");
    return HAL_CRYPT_ERROR_NOT_SUPPORTED;
#endif
}
