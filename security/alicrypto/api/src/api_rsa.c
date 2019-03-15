/**
 * Copyright (C) 2017  Alibaba Group Holding Limited.
 */
#include "ali_crypto.h"
#include "sal_crypto.h"
#include "config.h"

#ifdef ALI_CRYPTO_RSA

ali_crypto_result ali_rsa_get_keypair_size(size_t keybits, size_t *size)
{
    ali_crypto_result result = 0;

    if (size == NULL) {
        return ALI_CRYPTO_INVALID_ARG;
    }
    
    result = sal_rsa_get_keypair_size(keybits, size);
    return result;
}

ali_crypto_result ali_rsa_get_pubkey_size(size_t keybits, size_t *size)
{
    ali_crypto_result result = 0;

    if (size == NULL) {
        return ALI_CRYPTO_INVALID_ARG;
    }
    
    result = sal_rsa_get_pubkey_size(keybits, size);
    return result;

}

ali_crypto_result ali_rsa_init_keypair(
      size_t keybits, const uint8_t *n, size_t n_size, const uint8_t *e,
      size_t e_size, const uint8_t *d, size_t d_size, const uint8_t *p,
      size_t p_size, const uint8_t *q, size_t q_size, const uint8_t *dp,
      size_t dp_size, const uint8_t *dq, size_t dq_size, const uint8_t *qp,
      size_t qp_size, rsa_keypair_t *keypair)
{
    ali_crypto_result result = 0;
    
    if (keypair == NULL || n == NULL || n_size == 0 || e == NULL ||
        e_size == 0 || d == NULL || d_size == 0) {
        PRINT_RET(ALI_CRYPTO_INVALID_ARG, "invalid args!\n");
    }

    if ((n_size << 3) > keybits || (e_size << 3) > keybits ||
        (d_size << 3) > keybits || (p_size << 3) > (keybits >> 1) ||
        (q_size << 3) > (keybits >> 1) || (dp_size << 3) > (keybits >> 1) ||
        (dq_size << 3) > (keybits >> 1) || (qp_size << 3) > (keybits >> 1)) {
        PRINT_RET(ALI_CRYPTO_INVALID_ARG, "key parms size not match with key size\n");
    }
        
    result = sal_rsa_init_keypair(
                                      keybits,  n, n_size, e,
                                      e_size, d, d_size, p,
                                      p_size, q, q_size, dp,
                                      dp_size, dq, dq_size, qp,
                                      qp_size, keypair);
    return result;
}

ali_crypto_result ali_rsa_init_pubkey(size_t keybits, const uint8_t *n,
                                      size_t n_size, const uint8_t *e,
                                      size_t e_size, rsa_pubkey_t *pubkey)
{
    ali_crypto_result result = 0;

    if (pubkey == NULL || n == NULL || n_size == 0 || e == NULL ||
        e_size == 0) {
        PRINT_RET(ALI_CRYPTO_INVALID_ARG, "invalid args!\n");
    }

    if ((n_size << 3) > keybits || (e_size << 3) > keybits) {
        PRINT_RET(ALI_CRYPTO_INVALID_ARG, "key param size not match with key size\n");
    }

    result = sal_rsa_init_pubkey(keybits, n, n_size, e, e_size, pubkey);
    return result;
}


ali_crypto_result ali_rsa_get_key_attr(rsa_key_attr_t attr,
                                       rsa_keypair_t *keypair, void *buffer,
                                       size_t *size)
{
    ali_crypto_result result = 0;

    if (keypair == NULL || buffer == NULL || size == NULL) {
        PRINT_RET(ALI_CRYPTO_INVALID_ARG, "invalid input args!\n");
    }

    result = sal_rsa_get_key_attr(attr, keypair, buffer, size);
    return result;
}



ali_crypto_result ali_rsa_gen_keypair(size_t keybits, const uint8_t *e,
                                            size_t e_size, rsa_keypair_t *keypair)
{
    ali_crypto_result result = 0;

    if (keypair == NULL) {
        return ALI_CRYPTO_INVALID_ARG;
    }
    
    result = sal_rsa_gen_keypair(keybits, e, e_size, keypair);
    return result;
}


ali_crypto_result ali_rsa_public_encrypt(const rsa_pubkey_t *pub_key,
                                       const uint8_t *src, size_t src_size,
                                       uint8_t *dst, size_t *dst_size,
                                       rsa_padding_t padding)
{
    ali_crypto_result result = 0;

    if (pub_key == NULL || src == NULL || src_size == 0 || dst_size == NULL ||
                        ((dst == NULL) && (*dst_size != 0))) {
        PRINT_RET(ALI_CRYPTO_INVALID_ARG, "invalid input args!\n");
    }

    result = sal_rsa_public_encrypt(pub_key, src, src_size,
                                                    dst, dst_size, padding);
    return result;
}


ali_crypto_result ali_rsa_private_decrypt(const rsa_keypair_t *priv_key,
                                          const uint8_t *src, size_t src_size,
                                          uint8_t *dst, size_t *dst_size,
                                          rsa_padding_t padding)
{
    ali_crypto_result result = 0;

    if (priv_key == NULL || src == NULL || src_size == 0 || dst_size == NULL ||
        ((dst == NULL) && (*dst_size != 0))) {
        PRINT_RET(ALI_CRYPTO_INVALID_ARG, "invalid input args!\n");
    }
        
    result = sal_rsa_private_decrypt(priv_key, src, src_size, dst, dst_size, padding);
    return result;
}

ali_crypto_result ali_rsa_sign(const rsa_keypair_t *priv_key,
                               const uint8_t *dig, size_t dig_size,
                               uint8_t *sig, size_t *sig_size,
                               rsa_padding_t padding)
{
    ali_crypto_result result = 0;

    if (priv_key == NULL || dig == NULL || 
            dig_size == 0 || sig == NULL || sig_size == NULL) {
        PRINT_RET(ALI_CRYPTO_INVALID_ARG, "invalid input args!\n");
    }

    result = sal_rsa_sign(priv_key, dig, dig_size, 
                                        sig, sig_size, padding);
    return result;
}

ali_crypto_result ali_rsa_verify(const rsa_pubkey_t *pub_key,
                                 const uint8_t *dig, size_t dig_size,
                                 const uint8_t *sig, size_t sig_size,
                                 rsa_padding_t padding, bool *p_result)
{
    ali_crypto_result result = 0;

    if (pub_key == NULL || NULL == p_result || 
            dig == NULL || dig_size == 0 ||
            sig == NULL || sig_size == 0) {
        PRINT_RET(ALI_CRYPTO_INVALID_ARG, "invalid input args!\n");
    }
    
    result = sal_rsa_verify(pub_key, dig, dig_size, sig, 
                                        sig_size, padding, p_result);
    return result;
}
#endif /* ALI_CRYPTO_RSA */
