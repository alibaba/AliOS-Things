/**
 * Copyright (C) 2017  Alibaba Group Holding Limited.
 **/
#ifndef __IMPL_RSA_H__
#define __IMPL_RSA_H__
#include "ali_crypto_types.h"

/* rsa */
typedef struct _rsa_ops_t {
    ali_crypto_result (*rsa_get_keypair_size)(size_t keybits, size_t *size);
    
    ali_crypto_result (*rsa_get_pubkey_size)(size_t keybits, size_t *size);

    
    ali_crypto_result  (*rsa_init_keypair)(
                          size_t keybits, const uint8_t *n, size_t n_size, const uint8_t *e,
                          size_t e_size, const uint8_t *d, size_t d_size, const uint8_t *p,
                          size_t p_size, const uint8_t *q, size_t q_size, const uint8_t *dp,
                          size_t dp_size, const uint8_t *dq, size_t dq_size, const uint8_t *qp,
                          size_t qp_size, rsa_keypair_t *keypair);
    
    ali_crypto_result (*rsa_init_pubkey)(size_t keybits, const uint8_t *n,
                                        size_t n_size, const uint8_t *e,
                                        size_t e_size, rsa_pubkey_t *pubkey);
    
    ali_crypto_result (*rsa_get_key_attr)(rsa_key_attr_t attr,
                                           rsa_keypair_t *keypair, void *buffer,
                                           size_t *size);

    
    ali_crypto_result (*rsa_gen_keypair)(size_t keybits, const uint8_t *e,
                                                size_t e_size, rsa_keypair_t *keypair);

    ali_crypto_result (*rsa_public_encrypt)(const rsa_pubkey_t *pub_key,
                                       const uint8_t *src, size_t src_size,
                                       uint8_t *dst, size_t *dst_size,
                                       rsa_padding_t padding);

    ali_crypto_result (*rsa_private_decrypt)(const rsa_keypair_t *priv_key,
                                             const uint8_t *src, size_t src_size,
                                             uint8_t *dst, size_t *dst_size,
                                             rsa_padding_t padding);

    ali_crypto_result (*rsa_sign)(const rsa_keypair_t *priv_key,
                                const uint8_t *dig, size_t dig_size,
                                uint8_t *sig, size_t *sig_size,
                                rsa_padding_t padding);

    ali_crypto_result (*rsa_verify)(const rsa_pubkey_t *pub_key,
                                     const uint8_t *dig, size_t dig_size,
                                     const uint8_t *sig, size_t sig_size,
                                     rsa_padding_t padding, bool *p_result);
} rsa_ops_t;

typedef struct _rsa_impl_t {
	rsa_ops_t        ops;
} rsa_impl_t;

#define ALI_CRYPTO_RSA_IMPL_START(name)    static rsa_impl_t rsa_impl_vect_##name = {

#define ALI_CRYPTO_RSA_IMPL(                        \
            _$$rsa_get_keypair_size,                \
            _$$rsa_get_pubkey_size,                 \
            _$$rsa_init_keypair,                    \
            _$$rsa_init_pubkey,                     \
            _$$rsa_get_key_attr,                    \
            _$$rsa_gen_keypair,                     \
            _$$rsa_public_encrypt,                  \
            _$$rsa_private_decrypt,                 \
            _$$rsa_sign,                            \
            _$$rsa_verify)                          \
       {                                            \
            _$$rsa_get_keypair_size,                \
            _$$rsa_get_pubkey_size,                 \
            _$$rsa_init_keypair,                    \
            _$$rsa_init_pubkey,                     \
            _$$rsa_get_key_attr,                    \
            _$$rsa_gen_keypair,                     \
            _$$rsa_public_encrypt,                  \
            _$$rsa_private_decrypt,                 \
            _$$rsa_sign,                            \
            _$$rsa_verify                           \
        }

#define ALI_CRYPTO_RSA_IMPL_END()     };


//start
#define ALI_CRYPTO_RSA_GET_IMPL(name)                   \
rsa_impl_t *ali_crypto_rsa_get_impl()                   \
{                                                       \
    return &rsa_impl_vect_##name;                        \
}                                                       \
//end



#endif //__IMPL_RSA_H__


