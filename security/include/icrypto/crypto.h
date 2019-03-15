#ifndef _CRYPTO_H_
#define _CRYPTO_H_
#include "ali_crypto.h"

/* aes */
#define  crypto_aes_get_ctx_size(type, size)                \
            ali_aes_get_ctx_size((type), (size))

#define  crypto_aes_init(type, is_enc, key1, key2, keybytes, iv, contex)     \
            ali_aes_init(type, is_enc, key1, key2, keybytes, iv, contex)

#define  crypto_aes_process(src, dst, size, context)    \
            ali_aes_process(src, dst, size, context)

#define crypto_aes_finish(src, src_size, dst, dst_size, padding, context)    \
           ali_aes_finish(src, src_size, dst, dst_size, padding, context)

#define crypto_aes_reset(context)       \
           ali_aes_reset(context)

/* des */
#define crypto_des_get_ctx_size(type, size)        \
           ali_des_get_ctx_size(type, size)

#define crypto_des_init(type, is_enc, key, keybytes, iv, context)               \
           ali_des_init(type, is_enc, key, keybytes, iv, context)

#define crypto_des_process(src, dst,size, context)      \
           ali_des_process(src, dst,size, context)

#define crypto_des_finish(src, src_size, dst, dst_size, padding, context)     \
           ali_des_finish(src, src_size, dst, dst_size, padding, context)

#define crypto_des_reset(context)               \
           ali_des_reset(context)

/* hash */
#define crypto_hash_get_ctx_size(type, size)             \
           ali_hash_get_ctx_size(type, size)

#define crypto_hash_init(type, context)                  \
           ali_hash_init(type, context)

#define crypto_hash_update(src, size, context)           \
           ali_hash_update(src, size, context)

#define crypto_hash_final(dgst, context)  \
           ali_hash_final(dgst, context)

#define crypto_hash_reset(context)         \
           ali_hash_reset(context)

#define crypto_hash_digest(type, src, size, dgst)     \
           ali_hash_digest(type, src, size, dgst)

/* hmac */
#define crypto_hmac_get_ctx_size(type, size)           \
           ali_hmac_get_ctx_size(type, size)

#define crypto_hmac_init(type, key, keybytes, context)       \
           ali_hmac_init(type, key, keybytes, context)

#define crypto_hmac_update(src, size, context)            \
           ali_hmac_update(src, size, context)

#define crypto_hmac_final(dgst, context)                 \
           ali_hmac_final(dgst, context)

#define crypto_hmac_reset(context)                       \
           ali_hmac_reset(context)

#define crypto_hmac_digest(type, key, keybytes, src, size, dgst)     \
           ali_hmac_digest(type, key, keybytes, src, size, dgst)

/* rsa */
#define crypto_rsa_get_keypair_size(keybits, size)        \
           ali_rsa_get_keypair_size(keybits, size)

#define crypto_rsa_get_pubkey_size(keybits, size)        \
           ali_rsa_get_pubkey_size(keybits, size)

#define crypto_rsa_init_keypair(keybits, n, n_size, e, e_size, d, d_size,    \
                      p, p_size, q, q_size, dp, dp_size, dq,                \
                      dq_size, qp, qp_size, keypair)                        \
           ali_rsa_init_keypair(keybits, n, n_size, e, e_size, d, d_size,    \
                      p, p_size, q, q_size, dp, dp_size, dq,                \
                      dq_size, qp, qp_size, keypair)

#define crypto_rsa_init_pubkey(keybits, n, n_size, e, e_size, pubkey)      \
           ali_rsa_init_pubkey(keybits, n, n_size, e, e_size, pubkey)

#define crypto_rsa_gen_keypair(keybits, e, e_size, keypair)         \
           ali_rsa_gen_keypair(keybits, e, e_size, keypair)

#define crypto_rsa_get_key_attr(attr, keypair, buffer, size)         \
           ali_rsa_get_key_attr(attr, keypair, buffer, size)

#define crypto_rsa_public_encrypt(pub_key, src, src_size,             \
                    dst, dst_size, padding)       \
           ali_rsa_public_encrypt(pub_key, src, src_size,             \
                                        dst, dst_size, padding)

#define crypto_rsa_private_decrypt(priv_key, src, src_size,           \
                                       dst, dst_size, padding)        \
           ali_rsa_private_decrypt(priv_key, src, src_size,           \
                                       dst, dst_size, padding)

#define crypto_rsa_sign(priv_key, dig, dig_size, sig, sig_size, padding)       \
           ali_rsa_sign(priv_key, dig, dig_size, sig, sig_size, padding)

#define crypto_rsa_verify(pub_key, dig, dig_size, sig,            \
                 sig_size, padding, result)         \
           ali_rsa_verify(pub_key, dig, dig_size, sig,            \
                                 sig_size, padding, result)

/* random number */
#define crypto_seed(seed, seed_len)         \
           ali_seed(seed, seed_len)

#define crypto_rand_gen(buf, len)           \
           ali_rand_gen(buf, len)

#define crypto_init()      \
          ali_crypto_init()

#define crypto_cleanup()       \
           ali_crypto_cleanup()

#endif /* _CRYPTO_H_ */
