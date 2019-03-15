#ifndef MBEDTLS_RSA_ALT_H
#define MBEDTLS_RSA_ALT_H

#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MBEDTLS_RSA_ALT_PKCS_V15    0
#define MBEDTLS_RSA_ALT_PKCS_V21    1

/**
 * \brief          RSA context structure
 */
typedef struct
{
    size_t n_len;
    size_t e_len;

    unsigned char *rsa_n;
    unsigned char *rsa_e;

    int padding;
    int hash_id;
} mbedtls_rsa_context;

int rsa_verify_alt(void *ctx, size_t hash_id,
                   const unsigned char *hash, size_t hash_len,
                   const unsigned char *sig, size_t sig_len);
int rsa_sign_alt(void *ctx, size_t hash_id,
                 const unsigned char *hash, size_t hash_len,
                 unsigned char *sig, size_t *sig_len);

int rsa_decrypt_alt(void *ctx, const unsigned char *input, size_t ilen,
                    unsigned char *output, size_t *olen);
int rsa_encrypt_alt(void *ctx, const unsigned char *input, size_t ilen,
                    unsigned char *output, size_t *olen);

void mbedtls_rsa_init_alt(mbedtls_rsa_context *ctx,
                          int padding, int hash_id);
void mbedtls_rsa_free_alt(mbedtls_rsa_context *ctx);

#ifdef __cplusplus
}
#endif

#endif /* MBEDTLS_RSA_ALT_H */
