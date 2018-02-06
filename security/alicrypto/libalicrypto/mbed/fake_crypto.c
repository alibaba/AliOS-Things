/**
 * Copyright (C) 2016 The YunOS Project. All rights reserved.
 */

#include "ali_crypto.h"

ali_crypto_result ali_ecc_get_keypair_size(size_t curve, size_t *size)
{
    return ALI_CRYPTO_NOSUPPORT;
}
ali_crypto_result ali_ecc_get_pubkey_size(size_t curve, size_t *size)
{
    return ALI_CRYPTO_NOSUPPORT;
}
ali_crypto_result ali_ecc_init_keypair(
                      const uint8_t *x, size_t x_size,
                      const uint8_t *y, size_t y_size,
                      const uint8_t *d, size_t d_size,
                      size_t curve, ecc_keypair_t *keypair)
{
    return ALI_CRYPTO_NOSUPPORT;
}
ali_crypto_result ali_ecc_init_pubkey(
                      const uint8_t *x, size_t x_size,
                      const uint8_t *y, size_t y_size,
                      size_t curve, ecc_pubkey_t *pubkey)
{
    return ALI_CRYPTO_NOSUPPORT;
}
ali_crypto_result ali_ecc_gen_keypair(
                      size_t curve, ecc_keypair_t *keypair)
{
    return ALI_CRYPTO_NOSUPPORT;
}

ali_crypto_result ali_ecdsa_sign(const ecc_keypair_t *priv_key,
                      const uint8_t *src, size_t src_size,
                      uint8_t *signature, size_t *sig_size)
{
    return ALI_CRYPTO_NOSUPPORT;
}
ali_crypto_result ali_ecdsa_verify(const ecc_pubkey_t *pub_key,
                      const uint8_t *src, size_t src_size,
                      const uint8_t *signature, size_t sig_size,
                      bool *result)
{
    return ALI_CRYPTO_NOSUPPORT;
}

ali_crypto_result ali_ecdh_derive_secret(
                      const ecc_keypair_t *priv_key,
                      const ecc_pubkey_t *peer_pubkey_key,
                      uint8_t *shared_secret, size_t *secret_size)
{
    return ALI_CRYPTO_NOSUPPORT;
}
