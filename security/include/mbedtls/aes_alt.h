#ifndef MBEDTLS_AES_ALT_H
#define MBEDTLS_AES_ALT_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define AES_STATUS_NONE     0
#define AES_STATUS_INIT     1
#define AES_STATUS_PROCESS  2

typedef struct
{
    size_t reset;
    size_t status;
    uint8_t key[32];
    size_t key_len;
    void *ali_ctx;
} mbedtls_aes_context;

void mbedtls_aes_init_alt( mbedtls_aes_context *ctx );
void mbedtls_aes_free_alt( mbedtls_aes_context *ctx );

int mbedtls_aes_setkey_enc_alt( mbedtls_aes_context *ctx, const unsigned char *key,
                    unsigned int keybits );
int mbedtls_aes_setkey_dec_alt( mbedtls_aes_context *ctx, const unsigned char *key,
                    unsigned int keybits );

int mbedtls_aes_crypt_ecb_alt( mbedtls_aes_context *ctx,
                    int mode,
                    const unsigned char input[16],
                    unsigned char output[16] );
int mbedtls_aes_crypt_cbc_alt( mbedtls_aes_context *ctx,
                    int mode,
                    size_t length,
                    unsigned char iv[16],
                    const unsigned char *input,
                    unsigned char *output );
#ifdef __cplusplus
}
#endif

#endif /* MBEDTLS_AES_ALT_H */
