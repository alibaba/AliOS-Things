#ifndef _TEE_CLIENT_CRYPTO_
#define _TEE_CLIENT_CRYPTO_

#include "tee_client_api.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define CRYPTO_SRV_UUID    {                            \
	 0x5445455f, 0x4944, 0x325f,                        \
	{0x53, 0x52, 0x56, 0x5f, 0x55, 0x55, 0x49, 0x40 }   \
}

#define TEE_AES_ENC_ECB (0x1)
#define TEE_AES_DEC_ECB (0x2)
#define TEE_AES_ENC_CBC (0x3)
#define TEE_AES_DEC_CBC (0x4)
#define TEE_RAND_GEN    (0x8)
#define TEE_SEED        (0x9)

#define AES_ENGINE_CTX_SIZE     (96)
enum {
    AES_STATUS_NEW          = 0,
    AES_STATUS_INITIALIZED  = 1,
    AES_STATUS_PROCESSING   = 2,
    AES_STATUS_FINISHED     = 3,
};

typedef struct _tee_aes_ctx_t {
    uint32_t magic;

    uint32_t status;
    aes_type_t type;
    bool is_enc;
	uint8_t key[32];
	uint8_t keylen;
    uint8_t iv[AES_IV_SIZE];
    char stream_block[AES_BLOCK_SIZE];
    char offset;

    /* this must > aes context size defined in driver */
    uint32_t ctx[AES_ENGINE_CTX_SIZE >> 2];
} tee_aes_ctx_t;

/****** TEE AES mode *****/
typedef enum {
    TEE_AES_MODE_ECB = 0,    ///< TEE AES ECB mode
    TEE_AES_MODE_CBC = 1,    ///< TEE AES CBC mode
    TEE_AES_MODE_MAX,        ///< invaild mode
}
tee_aes_mode_e;

void tee_aes_init(uint32_t *ctx);
void tee_aes_free(uint32_t *ctx);

TEEC_Result tee_aes_encrypt_ecb(const uint8_t *in, uint32_t in_len,
                                const uint8_t *key, uint32_t key_len,
                                tee_aes_ctx_t *ctx,
                                uint8_t *out);

TEEC_Result tee_aes_decrypt_ecb(const uint8_t *in, uint32_t in_len,
                                const uint8_t *key, uint32_t key_len,
                                tee_aes_ctx_t *ctx,
                                uint8_t *out);

TEEC_Result tee_aes_encrypt_cbc(const uint8_t *in, uint32_t in_len,
                                const uint8_t *key, uint32_t key_len,
                                tee_aes_ctx_t *ctx,
                                uint8_t *out);

TEEC_Result tee_aes_decrypt_cbc(const uint8_t *in, uint32_t in_len,
                                const uint8_t *key, uint32_t key_len,
                                tee_aes_ctx_t *ctx,
                                uint8_t *out);

#endif // #ifdef _TEE_CLIENT_CRYPTO_
