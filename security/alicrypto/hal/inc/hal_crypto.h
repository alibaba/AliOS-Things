#ifndef __HAL_CRYPTO_H__
#define __HAL_CRYPTO_H__

#include "config.h"
#include "osa.h"

ali_crypto_result hal_crypto_init();

void hal_crypto_cleanup();

//aes
ali_crypto_result hal_aes_get_ctx_size(aes_type_t type, size_t *size);

ali_crypto_result hal_aes_init(aes_type_t type,         bool is_enc,
                                        const uint8_t *key1,    const uint8_t *key2,
                                        size_t keybytes,        const uint8_t *iv,      
                                        void *context);


ali_crypto_result hal_aes_process(const uint8_t    *src, 
                                    uint8_t         *dst, 
                                    size_t          size, 
                                    void            *context);

ali_crypto_result hal_aes_finish(const uint8_t   *src, 
                                    size_t          src_size, 
                                    uint8_t         *dst, 
                                    size_t          *dst_size, 
                                    sym_padding_t   padding,
                                    void            *context);

ali_crypto_result hal_aes_reset(void *context);

//sm4

ali_crypto_result hal_sm4_get_ctx_size(sm4_type_t type, size_t *size);

ali_crypto_result hal_sm4_init(sm4_type_t type, bool is_enc,
                      const uint8_t *key1, const uint8_t *key2,
                      size_t keybytes, const uint8_t *iv, void *context);

ali_crypto_result hal_sm4_process(const uint8_t *src, uint8_t *dst,
                                  size_t size, void *context);

ali_crypto_result hal_sm4_finish(const uint8_t *src, size_t src_size,
                                 uint8_t *dst, size_t *dst_size,
                                 sym_padding_t padding, void *context);

ali_crypto_result hal_sm4_reset(void *context);

// hash
ali_crypto_result hal_hash_get_ctx_size(hash_type_t type, size_t *size);

ali_crypto_result hal_hash_init(hash_type_t type, void *context);

ali_crypto_result hal_hash_update(const uint8_t *src, size_t size, void *context);

ali_crypto_result hal_hash_final(uint8_t *dgst, void *context);

ali_crypto_result hal_hash_reset(void *context);


//hmac
ali_crypto_result hal_hmac_get_ctx_size(hash_type_t type, size_t *size);

ali_crypto_result hal_hmac_init(hash_type_t type, const uint8_t *key,
                                        size_t keybytes, void *context);

ali_crypto_result hal_hmac_update(const uint8_t *src, size_t size, void *context);

ali_crypto_result hal_hmac_final(uint8_t *dgst, void *context);

ali_crypto_result hal_hmac_reset(void *context);

//RSA
ali_crypto_result hal_rsa_get_keypair_size(size_t keybits, size_t *size);

ali_crypto_result hal_rsa_get_pubkey_size(size_t keybits, size_t *size);

ali_crypto_result hal_rsa_init_keypair(
      size_t keybits, const uint8_t *n, size_t n_size, const uint8_t *e,
      size_t e_size, const uint8_t *d, size_t d_size, const uint8_t *p,
      size_t p_size, const uint8_t *q, size_t q_size, const uint8_t *dp,
      size_t dp_size, const uint8_t *dq, size_t dq_size, const uint8_t *qp,
      size_t qp_size, rsa_keypair_t *keypair);

ali_crypto_result hal_rsa_init_pubkey(size_t keybits, const uint8_t *n,
                                    size_t n_size, const uint8_t *e,
                                    size_t e_size, rsa_pubkey_t *pubkey);

ali_crypto_result hal_rsa_get_key_attr(rsa_key_attr_t attr,
                                       rsa_keypair_t *keypair, void *buffer,
                                       size_t *size);

ali_crypto_result hal_rsa_gen_keypair(size_t keybits, const uint8_t *e,
                                           size_t e_size, rsa_keypair_t *keypair);

ali_crypto_result hal_rsa_public_encrypt(const rsa_pubkey_t *pub_key,
                                      const uint8_t *src, size_t src_size,
                                      uint8_t *dst, size_t *dst_size,
                                      rsa_padding_t padding);

ali_crypto_result hal_rsa_private_decrypt(const rsa_keypair_t *priv_key,
                                        const uint8_t *src, size_t src_size,
                                        uint8_t *dst, size_t *dst_size,
                                        rsa_padding_t padding);

ali_crypto_result hal_rsa_sign(const rsa_keypair_t *priv_key,
                               const uint8_t *dig, size_t dig_size,
                               uint8_t *sig, size_t *sig_size,
                               rsa_padding_t padding);


ali_crypto_result hal_rsa_verify(const rsa_pubkey_t *pub_key,
                                const uint8_t *dig, size_t dig_size,
                                const uint8_t *sig, size_t sig_size,
                                rsa_padding_t padding, bool *p_result);
//sm2
ali_crypto_result hal_sm2_get_keypair_size(size_t keybits, size_t *size);

ali_crypto_result hal_sm2_get_pubkey_size(size_t keybits, size_t *size);

ali_crypto_result hal_sm2_init_keypair(
                      const uint8_t *x, size_t x_size,
                      const uint8_t *y, size_t y_size,
                      const uint8_t *d, size_t d_size,
                      size_t curve, ecc_keypair_t *keypair);

ali_crypto_result hal_sm2_init_pubkey(
                      const uint8_t *x, size_t x_size,
                      const uint8_t *y, size_t y_size,
                      size_t curve, ecc_pubkey_t *pubkey);

ali_crypto_result hal_sm2_gen_keypair(
                      size_t curve, ecc_keypair_t *keypair);

ali_crypto_result hal_sm2_sign(const ecc_keypair_t *priv_key,
                      const uint8_t *src, size_t src_size,
                      uint8_t *signature, size_t *sig_size);

ali_crypto_result hal_sm2_verify(const ecc_pubkey_t *pub_key,
                      const uint8_t *src, size_t src_size,
                      const uint8_t *signature, size_t sig_size,
                      bool *p_result);

ali_crypto_result hal_sm2_encrypt(const ecc_pubkey_t *pub_key,
                      const uint8_t *plaintext, size_t p_size,
                      uint8_t *ciphertext, size_t *c_size);

ali_crypto_result hal_sm2_decrypt(const ecc_keypair_t *priv_key,
                      const uint8_t *ciphertext, size_t *c_size,
                      uint8_t *plaintext, size_t *p_size);

ali_crypto_result hal_sm2dh_derive_secret(const uint8_t flag_server,
                      const uint8_t *ID, size_t ID_size,
                      const uint8_t *peer_ID, size_t peer_ID_size,
                      const ecc_keypair_t *priv_key,
                      const ecc_keypair_t *tmp_priv_key,
                      const ecc_pubkey_t *peer_pubkey,
                      const ecc_pubkey_t *tmp_peer_pubkey,
                      uint8_t *shared_secret, const size_t secret_size);
//rand
ali_crypto_result sal_rand_gen(uint8_t *buf, size_t len);


//Rand
ali_crypto_result hal_rand_gen(uint8_t *buf, size_t len);

ali_crypto_result hal_seed(uint8_t *seed, size_t seed_len);


#endif //__HAL_CRYPTO_H__


