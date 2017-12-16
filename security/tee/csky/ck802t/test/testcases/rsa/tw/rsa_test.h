/**
 * Copyright (C) 2015 The YunOS Project. All rights reserved.
 */

#ifndef _RSA_TEST_H_
#define _RSA_TEST_H_

#include "rsa.h"
#include "tee_srv.h"
#include "tee_dbg.h"

#include "tee_tos.h"
#include "tee_crypto.h"

#define RSA_TEST_ASSERT(_x)                                     \
    do {                                                        \
        if (!(_x)) {                                            \
            tee_dbg_print(ERR, "ASSERT FAILURE:\n");            \
            tee_dbg_print(ERR, (int8_t *)"%s (%d): %s\n",       \
                    __FILE__, __LINE__, __FUNCTION__);          \
            while (1) /* loop */;                               \
        }                                                       \
    } while (0)

#define KEY_BITS            (1024)
#define KEY_BYTES           (KEY_BITS >> 3)
#define SHA1_DGST_SZ        (20)

extern uint8_t modulus[];
extern uint8_t publicExponent[];
extern uint8_t privateExponent[];
extern unsigned char Message[];
extern unsigned char Digest_Padded[];
extern unsigned char Digest_Padded_public_encrypt_no_padding_out[];
extern unsigned char Digest[];
extern unsigned char Digest_public_encrypt_pkcs1_padding_out[];
extern uint8_t Digest_signature_pkcs1_padding_out[];

#define MESSAGE_SIZE        (9)
#define DIGEST_PADDED_SIZE  (KEY_BYTES)
#define DIGEST_PADDED_PUB_ENC_NO_PADDING_OUT_SIZE   (KEY_BYTES)
#define DIGEST_SIZE  (SHA1_DGST_SZ)
#define DIGEST_PUB_ENC_PKCS1_PADDING_OUT_SIZE    (KEY_BYTES)
#define DIGEST_SIGNATURE_PKCS1_PADDING_OUT_SIZE     (KEY_BYTES)

static inline int32_t rsa_test_format_key(size_t keybits,
                        const uint8_t *n, size_t n_size,
                        const uint8_t *e, size_t e_size,
                        const uint8_t *d, size_t d_size,
                        rsa_keypair_t **keypair,
                        rsa_pubkey_t **pubkey)
{
    tee_crypto_result res;
    uint32_t keypair_size, pubkey_size;
    rsa_pubkey_t *tmp_pubkey;
    rsa_keypair_t *tmp_keypair;
    /* init pubkey if have */
    if (pubkey) {
        res = tee_rsa_get_pubkey_size(KEY_BITS, (size_t *)(&pubkey_size));
        RSA_TEST_ASSERT(TEE_CRYPTO_SUCCESS == res);

        tmp_pubkey = (rsa_pubkey_t *)malloc(pubkey_size);
        RSA_TEST_ASSERT(tmp_pubkey != NULL);

        res = tee_rsa_init_pubkey(KEY_BITS,
                          n, n_size,
                          e, e_size,
                          tmp_pubkey);
        RSA_TEST_ASSERT(TEE_CRYPTO_SUCCESS == res);
        *pubkey  = tmp_pubkey;
    }

    /* init keypair if have */
    if (keypair) {
        res = tee_rsa_get_keypair_size(KEY_BITS, (size_t *)(&keypair_size));
        RSA_TEST_ASSERT(TEE_CRYPTO_SUCCESS == res);
        tmp_keypair = (rsa_keypair_t *)malloc(keypair_size);
        RSA_TEST_ASSERT(tmp_keypair != NULL);

        res = tee_rsa_init_keypair(KEY_BITS,
                          n, n_size,
                          e, e_size,
                          d, d_size,
                          NULL, 0,
                          NULL, 0,
                          NULL, 0,
                          NULL, 0,
                          NULL, 0,
                          tmp_keypair);
        RSA_TEST_ASSERT(TEE_CRYPTO_SUCCESS == res);
        *keypair = tmp_keypair;
    }
    return 0;
}
static inline void rsa_test_clean_key(rsa_keypair_t *keypair,
                                      rsa_pubkey_t *pubkey)
{
    if (pubkey) {
        free(pubkey);
    }
    if (keypair) {
        free(keypair);
    }
}

#if DBG
extern void dump_data(char *prefix, unsigned char *data, unsigned int size);
extern void dump_code(char *name, unsigned char *data, unsigned int size);
#endif

/* API level test */
extern TEE_Result asym_cipher_enc(uint8_t *n, uint8_t *e,
                         uint32_t keybits,
                         uint8_t *in, uint32_t in_len,
                         uint8_t *out, uint32_t *p_out_len,
                         uint32_t algo);
extern TEE_Result asym_cipher_dec(uint8_t *n, uint8_t *e,
                         uint8_t *d, uint32_t keybits,
                         uint8_t *in, uint32_t in_len,
                         uint8_t *out, uint32_t *p_out_len,
                         uint32_t algo);

extern TEE_Result asym_sign(uint8_t *n, uint8_t *e,
                        uint8_t *d, uint32_t keybits,
                        uint8_t *messageData,
                        uint32_t messageLen,
                        uint8_t * signature, uint32_t * sigLen);
extern TEE_Result asym_verify(uint8_t *n, uint8_t *e,
                        uint32_t keybits,
                        uint8_t * messageData,
                        uint32_t messageLen,
                        uint8_t * signature,
                        uint32_t * sigLen,
                        bool *result);
/* Crypto level test */
extern int32_t rsa_test_v1(void);
extern int32_t rsa_test_v2(void);
extern int32_t rsa_test_v3(void);
extern int32_t rsa_test_v4(void);
extern int32_t RSA_Test_api_test(void);

#endif /* _RSA_TEST_H_ */
