/**
 * Copyright (C) 2017  Alibaba Group Holding Limited.
 **/

#include "ali_crypto.h"
#include "impl_crypto.h"
#include "hal_crypto.h"
#include "sal_crypto.h"


ali_crypto_result hal_rsa_get_keypair_size(size_t keybits, size_t *size)
{
    UNUSED(keybits);
    UNUSED(size);
    
    return ALI_CRYPTO_SUCCESS;
}

ali_crypto_result hal_rsa_get_pubkey_size(size_t keybits, size_t *size)
{

    return ALI_CRYPTO_SUCCESS;
}

ali_crypto_result hal_rsa_init_keypair(
      size_t keybits, const uint8_t *n, size_t n_size, const uint8_t *e,
      size_t e_size, const uint8_t *d, size_t d_size, const uint8_t *p,
      size_t p_size, const uint8_t *q, size_t q_size, const uint8_t *dp,
      size_t dp_size, const uint8_t *dq, size_t dq_size, const uint8_t *qp,
      size_t qp_size, rsa_keypair_t *keypair)
{
    UNUSED(keybits);
    UNUSED(n);
    UNUSED(n_size);
    UNUSED(e);
    UNUSED(e_size);
    UNUSED(d);
    UNUSED(d_size);
    UNUSED(p);
    UNUSED(p_size);
    UNUSED(q);
    UNUSED(q_size);
    UNUSED(dp);
    UNUSED(dp_size);
    UNUSED(dq);
    UNUSED(dq_size);
    UNUSED(qp);
    UNUSED(qp_size);
    UNUSED(keypair);

    return ALI_CRYPTO_SUCCESS;
}

ali_crypto_result hal_rsa_init_pubkey(size_t keybits, const uint8_t *n,
                                      size_t n_size, const uint8_t *e,
                                      size_t e_size, rsa_pubkey_t *pubkey)
{
    UNUSED(keybits);
    UNUSED(n);
    UNUSED(n_size);
    UNUSED(e);
    UNUSED(e_size);
    UNUSED(pubkey);


    return ALI_CRYPTO_SUCCESS;
}


ali_crypto_result hal_rsa_get_key_attr(rsa_key_attr_t attr,
                                       rsa_keypair_t *keypair, void *buffer,
                                       size_t *size)
{
    UNUSED(attr);
    UNUSED(keypair);
    UNUSED(buffer);
    UNUSED(size);


    return ALI_CRYPTO_SUCCESS;
}



ali_crypto_result hal_rsa_gen_keypair(size_t keybits, const uint8_t *e,
                                            size_t e_size, rsa_keypair_t *keypair)
{
    UNUSED(keybits);
    UNUSED(e);
    UNUSED(e_size);
    UNUSED(keypair);

    return ALI_CRYPTO_SUCCESS;
}


ali_crypto_result hal_rsa_public_encrypt(const rsa_pubkey_t *pub_key,
                                       const uint8_t *src, size_t src_size,
                                       uint8_t *dst, size_t *dst_size,
                                       rsa_padding_t padding)
{
    UNUSED(pub_key);
    UNUSED(src);
    UNUSED(src_size);
    UNUSED(dst);
    UNUSED(dst_size);
    UNUSED(padding);

    return ALI_CRYPTO_SUCCESS;
}


ali_crypto_result hal_rsa_private_decrypt(const rsa_keypair_t *priv_key,
                                          const uint8_t *src, size_t src_size,
                                          uint8_t *dst, size_t *dst_size,
                                          rsa_padding_t padding)
{
    UNUSED(priv_key);
    UNUSED(src);
    UNUSED(src_size);
    UNUSED(dst);
    UNUSED(dst_size);
    UNUSED(padding);


    return ALI_CRYPTO_SUCCESS;
}

ali_crypto_result hal_rsa_sign(const rsa_keypair_t *priv_key,
                               const uint8_t *dig, size_t dig_size,
                               uint8_t *sig, size_t *sig_size,
                               rsa_padding_t padding)
{
    UNUSED(priv_key);
    UNUSED(dig);
    UNUSED(dig_size);
    UNUSED(sig);
    UNUSED(sig_size);
    UNUSED(padding);

    return ALI_CRYPTO_SUCCESS;
}

ali_crypto_result hal_rsa_verify(const rsa_pubkey_t *pub_key,
                                 const uint8_t *dig, size_t dig_size,
                                 const uint8_t *sig, size_t sig_size,
                                 rsa_padding_t padding, bool *p_result)
{
    UNUSED(pub_key);
    UNUSED(dig);
    UNUSED(dig_size);
    UNUSED(sig);
    UNUSED(sig_size);
    UNUSED(padding);
    UNUSED(p_result);

    return ALI_CRYPTO_SUCCESS;
}



