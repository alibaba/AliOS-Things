/**
 * Copyright (C) 2017  Alibaba Group Holding Limited.
 **/

#include "ali_crypto.h"
#include "impl_crypto.h"
#include "hal_crypto.h"
#include "sal_crypto.h"

#ifdef ALI_CRYPTO_SM2
ali_crypto_result hal_sm2_get_keypair_size(size_t keybits, size_t *size)
{

    UNUSED(keybits);
    UNUSED(size);
    ALI_CRYPTO_DBG_I("hal_sm2_get_keypair_size\n");
    return ALI_CRYPTO_SUCCESS;        
} 

ali_crypto_result hal_sm2_get_pubkey_size(size_t keybits, size_t *size)
{
    UNUSED(keybits);
    UNUSED(size);
    ALI_CRYPTO_DBG_I("hal_sm2_get_pubkey_size\n");
    return ALI_CRYPTO_SUCCESS;       
}

ali_crypto_result hal_sm2_init_keypair(
                      const uint8_t *x, size_t x_size,
                      const uint8_t *y, size_t y_size,
                      const uint8_t *d, size_t d_size,
                      size_t curve, ecc_keypair_t *keypair)
{
    UNUSED(x);
    UNUSED(x_size);
    UNUSED(y);
    UNUSED(y_size);
    UNUSED(d);
    UNUSED(d_size);
    UNUSED(curve);
    UNUSED(keypair);
    ALI_CRYPTO_DBG_I("hal_sm2_init_keypair\n");
    return ALI_CRYPTO_SUCCESS;       
}

ali_crypto_result hal_sm2_init_pubkey(
                      const uint8_t *x, size_t x_size,
                      const uint8_t *y, size_t y_size,
                      size_t curve, ecc_pubkey_t *pubkey)
{
    UNUSED(x);
    UNUSED(x_size);
    UNUSED(y);
    UNUSED(y_size);
    UNUSED(curve);
    UNUSED(pubkey);
    ALI_CRYPTO_DBG_I("hal_sm2_init_keypair\n");
    return ALI_CRYPTO_SUCCESS;       
}

ali_crypto_result hal_sm2_gen_keypair(
                      size_t curve, ecc_keypair_t *keypair)
{
    UNUSED(curve);
    UNUSED(keypair);
    ALI_CRYPTO_DBG_I("hal_sm2_gen_keypair\n");
    return ALI_CRYPTO_SUCCESS;       
}

ali_crypto_result hal_sm2_sign(const ecc_keypair_t *priv_key,
                      const uint8_t *src, size_t src_size,
                      uint8_t *signature, size_t *sig_size)
{
    UNUSED(priv_key);
    UNUSED(src);
    UNUSED(src_size);
    UNUSED(signature);
    UNUSED(sig_size);
    ALI_CRYPTO_DBG_I("hal_sm2_sign\n");
    return ALI_CRYPTO_SUCCESS;       
}

ali_crypto_result hal_sm2_verify(const ecc_pubkey_t *pub_key,
                      const uint8_t *src, size_t src_size,
                      const uint8_t *signature, size_t sig_size,
                      bool *p_result)
{
    UNUSED(pub_key);
    UNUSED(src);
    UNUSED(src_size);
    UNUSED(signature);
    UNUSED(sig_size);
    UNUSED(p_result);
    ALI_CRYPTO_DBG_I("hal_sm2_verify\n");
    return ALI_CRYPTO_SUCCESS;       
}
ali_crypto_result hal_sm2_encrypt(const ecc_pubkey_t *pub_key,
                      const uint8_t *plaintext, size_t p_size,
                      uint8_t *ciphertext, size_t *c_size)
{
    UNUSED(pub_key);
    UNUSED(plaintext);
    UNUSED(p_size);
    UNUSED(ciphertext);
    UNUSED(c_size);
    ALI_CRYPTO_DBG_I("hal_sm2_encrypt\n");
    return ALI_CRYPTO_SUCCESS;
};

ali_crypto_result hal_sm2_decrypt(const ecc_keypair_t *priv_key,
                      const uint8_t *ciphertext, size_t *c_size,
                      uint8_t *plaintext, size_t *p_size)
{
    UNUSED(priv_key);
    UNUSED(ciphertext);
    UNUSED(c_size);
    UNUSED(plaintext);
    UNUSED(p_size);
    ALI_CRYPTO_DBG_I("hal_sm2_decrypt\n");
    return ALI_CRYPTO_SUCCESS;
};

ali_crypto_result hal_sm2dh_derive_secret(const uint8_t flag_server,
                      const uint8_t *ID, size_t ID_size,
                      const uint8_t *peer_ID, size_t peer_ID_size,
                      const ecc_keypair_t *priv_key,
                      const ecc_keypair_t *tmp_priv_key,
                      const ecc_pubkey_t *peer_pubkey,
                      const ecc_pubkey_t *tmp_peer_pubkey,
                      uint8_t *shared_secret, const size_t secret_size){
    UNUSED(flag_server);
    UNUSED(ID);
    UNUSED(ID_size);
    UNUSED(peer_ID);
    UNUSED(peer_ID_size);
    UNUSED(priv_key);
    UNUSED(tmp_priv_key);
    UNUSED(peer_pubkey);
    UNUSED(tmp_peer_pubkey);
    UNUSED(shared_secret);
    UNUSED(secret_size);
    ALI_CRYPTO_DBG_I("hal_sm2_derive_secret\n");
    return ALI_CRYPTO_SUCCESS;
};
#endif
