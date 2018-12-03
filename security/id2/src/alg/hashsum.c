#include <stdint.h>
#include "error_code.h"
#include "irot_pal.h"
#include "hashsum.h"
#include "config.h"

#if (ID2_USE_ALI_CRYPTO)
#include "ali_crypto.h"
#include "crypto.h"

irot_result_t hash_sum(const uint8_t* in, uint32_t in_len, uint8_t* out, uint32_t* out_len, digest_t type)
{
    irot_result_t ret = IROT_SUCCESS;
    size_t ctx_size;
    ali_crypto_result result;
    void* ctx = NULL;

    if (type == DIGEST_TYPE_SHA256)
    {
        if (*out_len < 32)
        {
            ret = IROT_ERROR_SHORT_BUFFER;
        }
        else
        {
            result = crypto_hash_get_ctx_size(SHA256, &ctx_size);
            if (result != ALI_CRYPTO_SUCCESS)
            {
                ret = IROT_ERROR_GENERIC;
                goto EXIT;
            }
            ctx = irot_pal_memory_malloc(ctx_size);
            if (ctx == NULL)
            {
                ret = IROT_ERROR_GENERIC;
                goto EXIT;
            }
            result = crypto_hash_init(SHA256, ctx);
            if (result != ALI_CRYPTO_SUCCESS)
            {
                ret = IROT_ERROR_GENERIC;
                goto EXIT;
            }
            result = crypto_hash_update(in, in_len, ctx);
            if (result != ALI_CRYPTO_SUCCESS)
            {
                ret = IROT_ERROR_GENERIC;
                goto EXIT;
            }
            result = crypto_hash_final(out, ctx);
            if (result != ALI_CRYPTO_SUCCESS)
            {
                ret = IROT_ERROR_GENERIC;
                goto EXIT;
            }
            *out_len = 32;
        }
    }
    else
    {
        ret = IROT_ERROR_NOT_SUPPORTED;
    }
EXIT:
    irot_pal_memory_free(ctx);
    return ret;
}

#else

#include "sha2.c"

static int digest_sha256(const void *data, uint32_t length, unsigned char *digest)
{
    SHA256_CTX *ctx = (SHA256_CTX *) irot_pal_memory_malloc(sizeof(SHA256_CTX));
    if (NULL == ctx) {
        return -1;
    }

    memset(ctx, 0, sizeof(SHA256_CTX));

    SHA256_Init(ctx);
    SHA256_Update(ctx, data, length);
    SHA256_Final(digest, ctx);
    irot_pal_memory_free(ctx);

    return 0;
}

irot_result_t hash_sum(const uint8_t* in, uint32_t in_len, uint8_t* out, uint32_t* out_len, digest_t type)
{
    irot_result_t ret = IROT_SUCCESS;
    int ret;

    if (type == DIGEST_TYPE_SHA256)
    {
        if (*out_len < 32)
        {
            ret = IROT_ERROR_SHORT_BUFFER;
        }
        else
        {
            ret = digest_sha256(in, in_len, out);
            if(ret != 0)
            {
                ret = IROT_ERROR_GENERIC;
                goto EXIT;
            }
            *out_len = 32;
        }
    }
    else
    {
        ret = IROT_ERROR_NOT_SUPPORTED;
    }
EXIT:
    return ret;
}

#endif

