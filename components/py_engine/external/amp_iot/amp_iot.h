#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/rsa.h"
#include "mbedtls/platform.h"
#include "mbedtls/pk.h"
#include "mbedtls/bignum.h"
#include "mbedtls/base64.h"
#include "mbedtls/aes.h"


// define encrypt error code
// typedef enum {
//         AMP_ENCRYPT_SUCCESS               = 0,
//         AMP_ENCRYPT_INIT_FAIL             = -1,  /*AMP_ENCRYPT init failed.*/
//         AMP_ENCRYPT_VERIFY_MD5_FAIL       = -2, /*AMP_ENCRYPT verfiy MD5 failed.*/
//         AMP_ENCRYPT_VERIFY_SHA2_FAIL      = -3, /*AMP_ENCRYPT verfiy SH256 failed.*/
//         AMP_ENCRYPT_VERIFY_RSA_FAIL       = -4, /*AMP_ENCRYPT verfiy RSA failed.*/
//         AMP_ENCRYPT_INVALID_PARAMETER     = -20, /*AMP_ENCRYPT INVALID PARAMETER.*/
// } AMP_ENCRYPT_ERRNO_E;

// // hash cal core
// int amp_hash_init(amp_hash_ctx_t *ctx, unsigned char type);
// int amp_hash_update(amp_hash_ctx_t *ctx, const unsigned char *buf, unsigned int len);
// int amp_hash_final(amp_hash_ctx_t *ctx, unsigned char *dgst);


int rsa_encrypt_test(void);

int haas_sig_verify(void);
int haas_sig_encrypt(void);
int aes_crypt_test(void);