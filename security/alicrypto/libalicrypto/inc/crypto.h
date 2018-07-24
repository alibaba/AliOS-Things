#ifndef _CRYPTO_H_
#define _CRYPTO_H_
#include "ali_crypto.h"


#define crypto_aes_get_ctx_size(type, size) ali_aes_get_ctx_size((type), (size))

#define crypto_aes_init(type, is_enc, key1, key2, keybytes, iv, contex) \
    ali_aes_init(type, is_enc, key1, key2, keybytes, iv, contex)

#define crypto_aes_process(src, dst, size, context) \
    ali_aes_process(src, dst, size, context)

#define crypto_aes_finish(src, src_size, dst, dst_size, padding, context) \
    ali_aes_finish(src, src_size, dst, dst_size, padding, context)

#define crypto_aes_reset(context) ali_aes_reset(context)

#define crypto_aes_copy_context(dst_ctx, src_ctx) \
    ali_aes_copy_context(dst_ctx, src_ctx)

#define crypto_des_get_ctx_size(type, size) ali_des_get_ctx_size(type, size)

#define crypto_des_init(type, is_enc, key, keybytes, iv, context) \
    ali_des_init(type, is_enc, key, keybytes, iv, context)

#define crypto_des_process(src, dst, size, context) \
    ali_des_process(src, dst, size, context)

#define crypto_des_finish(src, src_size, dst, dst_size, padding, context) \
    ali_des_finish(src, src_size, dst, dst_size, padding, context)

#define crypto_des_reset(context) ali_des_reset(context)

#define crypto_des_copy_context(dst_ctx, src_ctx) \
    ali_des_copy_context(dst_ctx, src_ctx)

#define crypto_authenc_get_ctx_size(type, size) \
    ali_authenc_get_ctx_size(type, size)

#define crypto_authenc_init(type, is_enc, key, keybytes, nonce, nonce_len,   \
                            tag_len, payload_len, aad_len, context)          \
    ali_authenc_init(type, is_enc, key, keybytes, nonce, nonce_len, tag_len, \
                     payload_len, aad_len, context)

#define crypto_authenc_update_aad(aad, aad_size, context) \
    ali_authenc_update_aad(aad, aad_size, context)

#define crypto_authenc_process(src, dst, size, context) \
    ali_authenc_process(src, dst, size, context)
#define crypto_authenc_enc_finish(src, src_size, dst, dst_size, tag, tag_len, \
                                  context)                                    \
    ali_authenc_enc_finish(src, src_size, dst, dst_size, tag, tag_len, context)
#define crypto_authenc_dec_finish(src, src_size, dst, dst_size, tag, tag_len, \
                                  context)                                    \
    ali_authenc_dec_finish(src, src_size, dst, dst_size, tag, tag_len, context)

#define crypto_authenc_reset(context) ali_authenc_reset(context)

#define crypto_authenc_copy_context(dst_ctx, src_ctx) \
    ali_authenc_copy_context(dst_ctx, src_ctx)
#define crypto_hash_get_ctx_size(type, size) ali_hash_get_ctx_size(type, size)

#define crypto_hash_init(type, context) ali_hash_init(type, context)

#define crypto_hash_update(src, size, context) \
    ali_hash_update(src, size, context)

#define crypto_hash_final(dgst, context) ali_hash_final(dgst, context)

#define crypto_hash_reset(context) ali_hash_reset(context)

#define crypto_hash_copy_context(dst_ctx, src_ctx) \
    ali_hash_copy_context(dst_ctx, src_ctx)

#define crypto_hash_digest(type, src, size, dgst) \
    ali_hash_digest(type, src, size, dgst)

#define crypto_hmac_get_ctx_size(type, size) ali_hmac_get_ctx_size(type, size)

#define crypto_hmac_init(type, key, keybytes, context) \
    ali_hmac_init(type, key, keybytes, context)

#define crypto_hmac_update(src, size, context) \
    ali_hmac_update(src, size, context)

#define crypto_hmac_final(dgst, context) ali_hmac_final(dgst, context)

#define crypto_hmac_reset(context) ali_hmac_reset(context)

#define crypto_hmac_copy_context(dst_ctx, src_ctx) \
    ali_hmac_copy_context(dst_ctx, src_ctx)

#define crypto_hmac_digest(type, key, keybytes, src, size, dgst) \
    ali_hmac_digest(type, key, keybytes, src, size, dgst)

#define crypto_cbcmac_get_ctx_size(type, size) \
    ali_cbcmac_get_ctx_size(type, size)

#define crypto_cbcmac_init(type, key, keybytes, context) \
    ali_cbcmac_init(type, key, keybytes, context)

#define crypto_cbcmac_update(src, size, context) \
    ali_cbcmac_update(src, size, context)

#define crypto_cbcmac_final(padding, dgst, context) \
    ali_cbcmac_final(padding, dgst, context)

#define crypto_cbcmac_reset(context) ali_cbcmac_reset(context)

#define crypto_cbcmac_copy_context(dst_ctx, src_ctx) \
    ali_cbcmac_copy_context(dst_ctx, src_ctx)

#define crypto_cbcmac_digest(type, key, keybytes, src, size, padding, dgst) \
    ali_cbcmac_digest(type, key, keybytes, src, size, padding, dgst)

#define crypto_cmac_get_ctx_size(type, size) ali_cmac_get_ctx_size(type, size)

#define crypto_cmac_init(type, key, keybytes, context) \
    ali_cmac_init(type, key, keybytes, context)

#define crypto_cmac_update(src, size, context) \
    ali_cmac_update(src, size, context)

#define crypto_cmac_final(padding, dgst, context) \
    ali_cmac_final(padding, dgst, context)

#define crypto_cmac_reset(context) ali_cmac_reset(context)

#define crypto_cmac_copy_context(dst_ctx, src_ctx) \
    ali_cmac_copy_context(dst_ctx, src_ctx)

#define crypto_cmac_digest(type, key, keybytes, src, size, padding, dgst) \
    ali_cmac_digest(type, key, keybytes, src, size, padding, dgst)

#define crypto_rsa_get_keypair_size(keybits, size) \
    ali_rsa_get_keypair_size(keybits, size)

#define crypto_rsa_get_pubkey_size(keybits, size) \
    ali_rsa_get_pubkey_size(keybits, size)

#define crypto_rsa_init_keypair(keybits, n, n_size, e, e_size, d, d_size, p,  \
                                p_size, q, q_size, dp, dp_size, dq, dq_size,  \
                                qp, qp_size, keypair)                         \
    ali_rsa_init_keypair(keybits, n, n_size, e, e_size, d, d_size, p, p_size, \
                         q, q_size, dp, dp_size, dq, dq_size, qp, qp_size,    \
                         keypair)

#define crypto_rsa_init_pubkey(keybits, n, n_size, e, e_size, pubkey) \
    ali_rsa_init_pubkey(keybits, n, n_size, e, e_size, pubkey)

#define crypto_rsa_gen_keypair(keybits, e, e_size, keypair) \
    ali_rsa_gen_keypair(keybits, e, e_size, keypair)

#define crypto_rsa_get_key_attr(attr, keypair, buffer, size) \
    ali_rsa_get_key_attr(attr, keypair, buffer, size)

#define crypto_rsa_public_encrypt(pub_key, src, src_size, dst, dst_size, \
                                  padding)                               \
    ali_rsa_public_encrypt(pub_key, src, src_size, dst, dst_size, padding)

#define crypto_rsa_private_decrypt(priv_key, src, src_size, dst, dst_size, \
                                   padding)                                \
    ali_rsa_private_decrypt(priv_key, src, src_size, dst, dst_size, padding)

#define crypto_rsa_sign(priv_key, dig, dig_size, sig, sig_size, padding) \
    ali_rsa_sign(priv_key, dig, dig_size, sig, sig_size, padding)

#define crypto_rsa_verify(pub_key, dig, dig_size, sig, sig_size, padding, \
                          result)                                         \
    ali_rsa_verify(pub_key, dig, dig_size, sig, sig_size, padding, result)

#define crypto_dsa_get_keypair_size(keybits, size) \
    ali_dsa_get_keypair_size(keybits, size)

#define crypto_dsa_get_pubkey_size(keybits, size) \
    ali_dsa_get_pubkey_size(keybits, size)

#define crypto_dsa_init_keypair(keybits, g, g_size, p, p_size, q, q_size, y,  \
                                y_size, x, x_size, keypair)                   \
    ali_dsa_init_keypair(keybits, g, g_size, p, p_size, q, q_size, y, y_size, \
                         x, x_size, keypair)

#define crypto_dsa_init_pubkey(keybits, g, g_size, p, p_size, q, q_size, y,  \
                               y_size, pubkey)                               \
    ali_dsa_init_pubkey(keybits, g, g_size, p, p_size, q, q_size, y, y_size, \
                        pubkey)

#define crypto_dsa_gen_keypair(keybit, g, g_size, p, p_size, q, q_size, \
                               keypair)                                 \
    ali_dsa_gen_keypair(keybit, g, g_size, p, p_size, q, q_size, keypair)

#define crypto_dsa_sign(priv_key, src, src_size, signature, sig_size, padding) \
    ali_dsa_sign(priv_key, src, src_size, signature, sig_size, padding)

#define crypto_dsa_verify(pub_key, src, src_size, signature, sig_size, \
                          padding, result)                             \
    ali_dsa_verify(pub_key, src, src_size, signature, sig_size, padding, result)

#define crypto_dsa_get_key_attr(attr, keypair, buffer, size) \
    ali_dsa_get_key_attr(attr, keypair, buffer, size)

#define crypto_dh_get_keypair_size(keybits, size) \
    ali_dh_get_keypair_size(keybits, size)

#define crypto_dh_get_pubkey_size(keybits, size) \
    ali_dh_get_pubkey_size(keybits, size)

#define crypto_dh_init_keypair(keybits, g, g_size, p, p_size, y, y_size, x,  \
                               x_size, q, q_size, xbits, keypair)            \
    ali_dh_init_keypair(keybits, g, g_size, p, p_size, y, y_size, x, x_size, \
                        q, q_size, xbits, keypair)

#define crypto_dh_init_pubkey(keybits, y, y_size, pubkey) \
    ali_dh_init_pubkey(keybits, y, y_size, pubkey)

#define crypto_dh_gen_keypair(keybit, g, g_size, p, p_size, q, q_size, xbits, \
                              keypair)                                        \
    ali_dh_gen_keypair(keybit, g, g_size, p, p_size, q, q_size, xbits, keypair)

#define crypto_dh_derive_secret(priv_key, peer_pub_key, shared_secret, \
                                secret_size)                           \
    ali_dh_derive_secret(priv_key, peer_pub_key, shared_secret, secret_size)

#define crypto_dh_get_key_attr(attr, keypair, buffer, size) \
    ali_dh_get_key_attr(attr, keypair, buffer, size)

#define crypto_ecc_get_keypair_size(curve, size) \
    ali_ecc_get_keypair_size(curve, size)

#define crypto_ecc_get_pubkey_size(curve, size) \
    ali_ecc_get_pubkey_size(curve, size)

#define crypto_ecc_init_keypair(x, x_size, y, y_size, d, d_size, curve, \
                                keypair)                                \
    ali_ecc_init_keypair(x, x_size, y, y_size, d, d_size, curve, keypair)

#define crypto_ecc_init_pubkey(x, x_size, y, y_size, curve, pubkey) \
    ali_ecc_init_pubkey(x, x_size, y, y_size, curve, pubkey)

#define crypto_ecc_gen_keypair(curve, keypair) \
    ali_ecc_gen_keypair(curve, keypair)

#define crypto_ecdsa_sign(priv_key, src, src_size, signature, sig_size) \
    ali_ecdsa_sign(priv_key, src, src_size, signature, sig_size)

#define crypto_ecdsa_verify(pub_key, src, src_size, signature, sig_size, \
                            result)                                      \
    ali_ecdsa_verify(pub_key, src, src_size, signature, sig_size, result)

#define crypto_ecdh_derive_secret(priv_key, peer_pubkey_key, shared_secret, \
                                  secret_size)                              \
    ali_ecdh_derive_secret(priv_key, peer_pubkey_key, shared_secret,        \
                           secret_size)

#define crypto_seed(seed, seed_len) ali_seed(seed, seed_len)

#define crypto_rand_gen(buf, len) ali_rand_gen(buf, len)

#define crypto_init() ali_crypto_init()

#define crypto_cleanup() ali_crypto_cleanup()

#endif /* _CRYPTO_H_ */
