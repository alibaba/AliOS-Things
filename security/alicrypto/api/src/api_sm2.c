
/**
 * Copyright (C) 2018  Alibaba Group Holding Limited.
 */
#include "ali_crypto.h"

#ifdef ALI_CRYPTO_SM2
#include "sal_crypto.h"

ali_crypto_result ali_sm2_get_keypair_size(size_t keybits, size_t *size)
{
    ali_crypto_result result = 0;

    if (size == NULL) {
        return ALI_CRYPTO_INVALID_ARG;
    }

    result = sal_sm2_get_keypair_size(keybits, size);
    return result;
}


ali_crypto_result ali_sm2_get_pubkey_size(size_t keybits, size_t *size)
{
    ali_crypto_result result = 0;

    if (size == NULL) {
        return ALI_CRYPTO_INVALID_ARG;
    }

    result = sal_sm2_get_pubkey_size(keybits, size);
    return result;
}

ali_crypto_result ali_sm2_init_keypair(
                      const uint8_t *x, size_t x_size,
                      const uint8_t *y, size_t y_size,
                      const uint8_t *d, size_t d_size,
                      size_t curve, ecc_keypair_t *keypair)
{

    ali_crypto_result result = 0;

    if (keypair == NULL || x == NULL || x_size == 0 
		|| y == NULL || y_size == 0 || d == NULL || d_size == 0 
		|| curve == 0) {
        PRINT_RET(ALI_CRYPTO_INVALID_ARG, "invalid args!\n");
    }

    result = sal_sm2_init_keypair(x, x_size, y, y_size, d, d_size, curve, keypair);
    return result;
}

ali_crypto_result ali_sm2_init_pubkey(
                      const uint8_t *x, size_t x_size,
                      const uint8_t *y, size_t y_size,
                      size_t curve, ecc_pubkey_t *pubkey)
{
    ali_crypto_result result = 0;

    if (pubkey == NULL || 
        x == NULL || x_size == 0 ||
        y == NULL || y_size == 0 ||
        curve == 0) {
        PRINT_RET(ALI_CRYPTO_INVALID_ARG, "Init_keypair: invalid args!\n");
    }

    result = sal_sm2_init_pubkey(x, x_size, y, y_size, curve, pubkey);
    return result;
}

ali_crypto_result ali_sm2_gen_keypair(
                      size_t curve, ecc_keypair_t *keypair)
{
    ali_crypto_result result = 0;

    if (curve == 0 || keypair == NULL) {
        return ALI_CRYPTO_INVALID_ARG;
    }

    result = sal_sm2_gen_keypair(curve, keypair);
    return result;
}

ali_crypto_result ali_sm2_sign(const ecc_keypair_t *priv_key,
                      const uint8_t *src, size_t src_size,
                      uint8_t *signature, size_t *sig_size)
{
    ali_crypto_result result = 0;

    if (priv_key == NULL ||
        src == NULL || src_size == 0 ||
        signature == NULL || (*sig_size) == 0){
        PRINT_RET(ALI_CRYPTO_INVALID_ARG, "sm2_sign: invalid input args!\n");
    }

    result = sal_sm2_sign(priv_key, src, src_size, signature, sig_size);
    return result;
}

ali_crypto_result ali_sm2_verify(const ecc_pubkey_t *pub_key,
                      const uint8_t *src, size_t src_size,
                      const uint8_t *signature, size_t sig_size,
                      bool *p_result)
{
    ali_crypto_result result = 0;
    
    if (pub_key == NULL ||
        src == NULL || src_size == 0 ||
        signature == NULL || sig_size == 0 ||
        p_result == NULL){
        PRINT_RET(ALI_CRYPTO_INVALID_ARG, "sm2_verify: invalid input args!\n");
    }

    result = sal_sm2_verify(pub_key, src, src_size, signature, sig_size, p_result);
    return result;
}

ali_crypto_result ali_sm2_encrypt(const ecc_pubkey_t *pub_key,
                      const uint8_t *plaintext, size_t p_size,
                      uint8_t *ciphertext, size_t *c_size)
{
    ali_crypto_result result = 0;

    if (pub_key == NULL || plaintext  == NULL || p_size == 0 ||
              (ciphertext == NULL && (*c_size) == 0)){
        PRINT_RET(ALI_CRYPTO_INVALID_ARG, "ali_sm2_encrypt: invalid input args!\n");
    }
    result = sal_sm2_encrypt(pub_key, plaintext, p_size, ciphertext, c_size);
    return result;
};

ali_crypto_result ali_sm2_decrypt(const ecc_keypair_t *priv_key,
                      const uint8_t *ciphertext, size_t *c_size,
                      uint8_t *plaintext, size_t *p_size)
{
    ali_crypto_result result = 0;

    if (priv_key == NULL || ciphertext == NULL || (*c_size) == 0 ||
                (plaintext  == NULL && (*p_size) == 0)){
        PRINT_RET(ALI_CRYPTO_INVALID_ARG, "ali_sm2_decrypt: invsald input args!\n");
    }

    result = sal_sm2_decrypt(priv_key, ciphertext, c_size, plaintext, p_size);
    return result;
};


ali_crypto_result ali_sm2dh_derive_secret(const uint8_t flag_server,
                const uint8_t *ID, size_t ID_size,
                const uint8_t *peer_ID, size_t peer_ID_size,
                const ecc_keypair_t *priv_key,
                const ecc_keypair_t *tmp_priv_key,
                const ecc_pubkey_t *peer_pubkey,
                const ecc_pubkey_t *tmp_peer_pubkey,
                uint8_t *shared_secret, const size_t secret_size)
{
    ali_crypto_result result = 0;

    if ((flag_server != 1 && flag_server != 0) ||
           ID == NULL || ID_size == 0 ||
           peer_ID == NULL || peer_ID_size == 0 ||
           priv_key == NULL || tmp_priv_key == NULL ||
           peer_pubkey == NULL || tmp_peer_pubkey == NULL ||
           shared_secret == NULL || secret_size == 0){
        PRINT_RET(ALI_CRYPTO_INVALID_ARG, "ali_sm2dh_derive_secret: invalid input args!\n");
    }

    result = sal_sm2dh_derive_secret(flag_server,
                     ID, ID_size, peer_ID, peer_ID_size,
                     priv_key, tmp_priv_key,
                     peer_pubkey, tmp_peer_pubkey,
                     shared_secret, secret_size);
    return result;

}

#endif /* ALI_CRYPTO_SM2 */
