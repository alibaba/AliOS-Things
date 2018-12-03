/**
 * Copyright (C) 2018  Alibaba Group Holding Limited.
 **/
#ifndef __IMPL_SM2_H__
#define __IMPL_SM2_H__
#include "ali_crypto_types.h"

/* SM2 */
typedef struct _sm2_ops_t {
    ali_crypto_result (*sm2_get_keypair_size)(size_t keybits, size_t *size);
    
    ali_crypto_result (*sm2_get_pubkey_size)(size_t keybits, size_t *size);

    ali_crypto_result (*sm2_init_keypair)(
                      const uint8_t *x, size_t x_size,
                      const uint8_t *y, size_t y_size,
                      const uint8_t *d, size_t d_size,
                      size_t curve, ecc_keypair_t *keypair);

    ali_crypto_result (*sm2_init_pubkey)(
                      const uint8_t *x, size_t x_size,
                      const uint8_t *y, size_t y_size,
                      size_t curve, ecc_pubkey_t *pubkey);

    ali_crypto_result (*sm2_gen_keypair)(
                      size_t curve, ecc_keypair_t *keypair);

    ali_crypto_result (*sm2_sign) (const ecc_keypair_t *priv_key,
                      const uint8_t *src, size_t src_size,
                      uint8_t *signature, size_t *sig_size);

    ali_crypto_result (*sm2_verify) (const ecc_pubkey_t *pub_key,
                      const uint8_t *src, size_t src_size,
                      const uint8_t *signature, size_t sig_size,
                      bool *p_result);

    ali_crypto_result (*sm2_encrypt) (const ecc_pubkey_t *pub_key,
                      const uint8_t *plaintext, size_t p_size,
                      uint8_t *ciphertext, size_t *c_size);

    ali_crypto_result (*sm2_decrypt) (const ecc_keypair_t *priv_key,
                      const uint8_t *ciphertext, size_t *c_size,
                      uint8_t *plaintext, size_t *p_size);

   ali_crypto_result (*sm2dh_derive_secret)(const uint8_t flag_server,
                const uint8_t *ID, size_t ID_size,
                const uint8_t *peer_ID, size_t peer_ID_size,
                const ecc_keypair_t *priv_key,
                const ecc_keypair_t *tmp_priv_key,
                const ecc_pubkey_t *peer_pubkey,
                const ecc_pubkey_t *tmp_peer_pubkey,
                uint8_t *shared_secret, const size_t secret_size);
} sm2_ops_t;

typedef struct _sm2_impl_t {
	sm2_ops_t        ops;
} sm2_impl_t;

#define ALI_CRYPTO_SM2_IMPL_START(name)    static sm2_impl_t sm2_impl_vect_##name = {

#define ALI_CRYPTO_SM2_IMPL(                        \
            _$$sm2_get_keypair_size,                \
            _$$sm2_get_pubkey_size,                 \
            _$$sm2_init_keypair,                    \
            _$$sm2_init_pubkey,                     \
            _$$sm2_gen_keypair,                     \
            _$$sm2_sign,                            \
            _$$sm2_verify,                          \
            _$$sm2_encrypt,                         \
            _$$sm2_decrypt,                         \
            _$$sm2dh_derive_secret)                 \
    {                                               \
            _$$sm2_get_keypair_size,                \
            _$$sm2_get_pubkey_size,                 \
            _$$sm2_init_keypair,                    \
            _$$sm2_init_pubkey,                     \
            _$$sm2_gen_keypair,                     \
            _$$sm2_sign,                            \
            _$$sm2_verify,                          \
            _$$sm2_encrypt,                         \
            _$$sm2_decrypt,                         \
            _$$sm2dh_derive_secret                  \
    }

#define ALI_CRYPTO_SM2_IMPL_END()     };

//start
#define ALI_CRYPTO_SM2_GET_IMPL(name)                   \
sm2_impl_t *ali_crypto_sm2_get_impl()                   \
{                                                       \
    return &sm2_impl_vect_##name;                       \
}                                                       \
//end


#endif //__IMPL_SM2_H__
