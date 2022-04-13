#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if !defined(MBEDTLS_CTR_DRBG_C)

#include "mbedtls/ctr_drbg.h"

#include <string.h>
#if defined(CONFIG_TEE_CA)
#include <drv/tee.h>
#endif
static void _tls_random(unsigned char *output, size_t output_len)
{
#if defined(CONFIG_TEE_CA)
    csi_tee_rand_generate(output, output_len);
#else
    int i;
    uint32_t random;
    int mod = output_len % 4;
    int count = 0;
    static uint32_t rnd = 0x12345;
    for (i = 0; i < output_len / 4; i++) {
        random = rnd * 0xFFFF777;
        rnd = random;
        output[count++] = (random >> 24) & 0xFF;
        output[count++] = (random >> 16) & 0xFF;
        output[count++] = (random >> 8) & 0xFF;
        output[count++] = (random) & 0xFF;
    }
    random = rnd * 0xFFFF777;
    rnd = random;
    for (i = 0; i < mod; i++) {
        output[i + count] = (random >> 8 * i) & 0xFF;
    }
#endif
}

void mbedtls_ctr_drbg_init( mbedtls_ctr_drbg_context *ctx )
{
    (void)ctx;
}

int mbedtls_ctr_drbg_random(void *p_rng, unsigned char *output, size_t output_len)
{
    (void)p_rng;
    _tls_random(output, output_len);
    return 0;
}

void mbedtls_ctr_drbg_free( mbedtls_ctr_drbg_context *ctx )
{
    (void)ctx;
}

#endif /* MBEDTLS_CTR_DRBG_C */