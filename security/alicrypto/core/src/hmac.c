/**
* Copyright (C) 2017  Alibaba Group Holding Limited.
**/
#include "config.h"
#include "osa.h"


#if defined(ALI_CRYPTO_HMAC)

#include "md.h"
#include "hash.h"
#include "md_internal.h"
#include "hmac.h"



/* Implementation that should never be optimized out by the compiler */
static void ali_algo_zeroize(void *v, size_t n)
{
    volatile unsigned char *p = v;
    while (n--)
        *p++ = 0;
}

int ali_algo_hmac_starts(ali_algo_hash_context_t *ctx, const unsigned char *key,
                        size_t keylen)
{
    unsigned char  sum[ALI_ALGO_MD_MAX_SIZE];
    unsigned char *ipad, *opad;
    size_t         i;

    if (ctx == NULL || ctx->md_info == NULL || ctx->hmac_ctx == NULL)
        return (ALI_ALGO_ERR_MD_BAD_INPUT_DATA);

    if (keylen > (size_t)ctx->md_info->block_size) {
        ctx->md_info->starts_func(ctx->md_ctx);
        ctx->md_info->update_func(ctx->md_ctx, key, keylen);
        ctx->md_info->finish_func(ctx->md_ctx, sum);

        keylen = ctx->md_info->size;
        key    = sum;
    }

    ipad = (unsigned char *)ctx->hmac_ctx;
    opad = (unsigned char *)ctx->hmac_ctx + ctx->md_info->block_size;

    osa_memset(ipad, 0x36, ctx->md_info->block_size);
    osa_memset(opad, 0x5C, ctx->md_info->block_size);

    for (i = 0; i < keylen; i++) {
        ipad[i] = (unsigned char)(ipad[i] ^ key[i]);
        opad[i] = (unsigned char)(opad[i] ^ key[i]);
    }

    ali_algo_zeroize(sum, sizeof(sum));

    ctx->md_info->starts_func(ctx->md_ctx);
    ctx->md_info->update_func(ctx->md_ctx, ipad, ctx->md_info->block_size);

    return (0);
}

int ali_algo_hmac_update(ali_algo_hash_context_t *ctx, const unsigned char *input,
                        size_t ilen)
{
    if (ctx == NULL || ctx->md_info == NULL || ctx->hmac_ctx == NULL)
        return (ALI_ALGO_ERR_MD_BAD_INPUT_DATA);

    ctx->md_info->update_func(ctx->md_ctx, input, ilen);

    return (0);
}

int ali_algo_hmac_finish(ali_algo_hash_context_t *ctx, unsigned char *output)
{
    unsigned char  tmp[ALI_ALGO_MD_MAX_SIZE];
    unsigned char *opad;

    if (ctx == NULL || ctx->md_info == NULL || ctx->hmac_ctx == NULL)
        return (ALI_ALGO_ERR_MD_BAD_INPUT_DATA);

    opad = (unsigned char *)ctx->hmac_ctx + ctx->md_info->block_size;

    ctx->md_info->finish_func(ctx->md_ctx, tmp);
    ctx->md_info->starts_func(ctx->md_ctx);
    ctx->md_info->update_func(ctx->md_ctx, opad, ctx->md_info->block_size);
    ctx->md_info->update_func(ctx->md_ctx, tmp, ctx->md_info->size);
    ctx->md_info->finish_func(ctx->md_ctx, output);

    return (0);
}

int ali_algo_hash_hmac(const ali_algo_hash_info_t *md_info,
                      const unsigned char *key, size_t keylen,
                      const unsigned char *input, size_t ilen,
                      unsigned char *output)
{
    ali_algo_hash_context_t ctx;
    int                    ret;

    if (md_info == NULL)
        return (ALI_ALGO_ERR_MD_BAD_INPUT_DATA);

    ali_algo_hash_init(&ctx);

    if ((ret = ali_algo_hash_setup(&ctx, md_info, 1)) != 0)
        return (ret);

    ali_algo_hmac_starts(&ctx, key, keylen);
    ali_algo_hmac_update(&ctx, input, ilen);
    ali_algo_hmac_finish(&ctx, output);

    ali_algo_hash_free(&ctx);

    return (0);
}

int ali_algo_hmac_reset(ali_algo_hash_context_t *ctx)
{
    unsigned char *ipad;

    if (ctx == NULL || ctx->md_info == NULL || ctx->hmac_ctx == NULL)
        return (ALI_ALGO_ERR_MD_BAD_INPUT_DATA);

    ipad = (unsigned char *)ctx->hmac_ctx;

    ctx->md_info->starts_func(ctx->md_ctx);
    ctx->md_info->update_func(ctx->md_ctx, ipad, ctx->md_info->block_size);

    return (0);
}

#endif /* ALI_CRYPTO_HMAC */
