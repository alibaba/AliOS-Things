/*
 * Copyright (C) 2017  Alibaba Group Holding Limited.
 */

#ifndef _ALI_CRYPTO_ALGO_H_
#define _ALI_CRYPTO_ALGO_H_

#if defined(__ARMCC_VERSION)
#pragma anon_unions
#endif

#include "ali_crypto.h"
#include "ali_crypto_debug.h"
#include "config.h"
#include "osa.h"

#include "aes.h"
#include "sm4.h"
#include "sha1.h"
#include "sha256.h"
#include "sm3.h"
#include "md.h"
#include "hash.h"
#include "md5.h"
#include "rsa.h"
#include "ecp.h"
#include "sm2.h"
#include "hmac.h"

#define INIT_CTX_MAGIC(m) (m = 0x12345678)
#define IS_VALID_CTX_MAGIC(m) (0x12345678 == m)
#define CLEAN_CTX_MAGIC(m) (m = 0x0)

enum
{
    PK_PUBLIC  = 0,
    PK_PRIVATE = 1
};

typedef struct _hash_ctx_t
{
    uint32_t    magic;
    uint32_t    status;
    hash_type_t type;

    union
    {
        uint8_t                sym_ctx[1];
        ali_algo_md5_context    md5_ctx;
        ali_algo_sha1_context   sha1_ctx;
        ali_algo_sha256_context sha256_ctx;
        ali_algo_sm3_context    sm3_ctx;
    };
} hash_ctx_t;

typedef struct _hmac_ctx_t
{
    uint32_t    magic;
    uint32_t    status;
    hash_type_t type;

    union
    {
        uint8_t                sym_ctx[1];
        ali_algo_hash_context_t ctx;
    };
} hmac_ctx_t;

typedef struct _des_ctx_t
{
    uint32_t   magic;
    uint32_t   status;
    des_type_t type;
    uint32_t   is_enc;
    union
    {
         uint8_t sym_ctx[1];
     };
} des_ctx_t;

typedef struct _aes_ctx_t
{
    uint32_t   magic;
    uint32_t   status;
    aes_type_t type;
    uint32_t   is_enc;
    uint8_t    iv[AES_IV_SIZE];
    size_t     offset;
    uint8_t    stream_block[AES_BLOCK_SIZE];
    union
    {
        uint8_t             sym_ctx[1];
        ali_algo_aes_context ctx;
        
    };
} aes_ctx_t;

typedef struct _sm4_ctx_t {
    uint32_t magic;
        uint32_t status;
        sm4_type_t type;
        uint32_t is_enc;
        uint8_t iv[SM4_IV_SIZE];
        size_t offset;
        union {
            uint8_t sym_ctx[1];
            ali_algo_sm4_context ctx;
        };
} sm4_ctx_t;


//aes
ali_crypto_result sal_aes_get_ctx_size(aes_type_t type, size_t *size);

ali_crypto_result sal_aes_init(aes_type_t type, bool is_enc,
                               const uint8_t *key1, const uint8_t *key2,
                               size_t keybytes, const uint8_t *iv,
                               void *context);

ali_crypto_result sal_aes_process(const uint8_t *src, uint8_t *dst, size_t size,
                                  void *context);

ali_crypto_result sal_aes_finish(const uint8_t *src, size_t src_size,
                               uint8_t *dst, size_t *dst_size,
                               sym_padding_t padding, void *context);

ali_crypto_result sal_aes_reset(void *context);

//sm4
ali_crypto_result sal_sm4_get_ctx_size(sm4_type_t type, size_t *size);

ali_crypto_result sal_sm4_init(sm4_type_t type, bool is_enc,
                      const uint8_t *key1, const uint8_t *key2,
                      size_t keybytes, const uint8_t *iv, void *context);

ali_crypto_result sal_sm4_process(const uint8_t *src, uint8_t *dst,
                                  size_t size, void *context);

ali_crypto_result sal_sm4_finish(const uint8_t *src, size_t src_size,
                                 uint8_t *dst, size_t *dst_size,
                                 sym_padding_t padding, void *context);

ali_crypto_result sal_sm4_reset(void *context);

//hash
ali_crypto_result sal_hash_get_ctx_size(hash_type_t type, size_t *size);

ali_crypto_result sal_hash_init(hash_type_t type, void *context);

ali_crypto_result sal_hash_update(const uint8_t *src, size_t size,
                                  void *context);

ali_crypto_result sal_hash_final(uint8_t *dgst, void *context);

ali_crypto_result sal_hash_reset(void *context);

//hamc
ali_crypto_result sal_hmac_get_ctx_size(hash_type_t type, size_t *size);

ali_crypto_result sal_hmac_init(hash_type_t type, const uint8_t *key,
                                size_t keybytes, void *context);

ali_crypto_result sal_hmac_update(const uint8_t *src, size_t size,
                                  void *context);

ali_crypto_result sal_hmac_final(uint8_t *dgst, void *context);

ali_crypto_result sal_hmac_reset(void *context);

//rsa
ali_crypto_result sal_rsa_get_keypair_size(size_t keybits, size_t *size);

ali_crypto_result sal_rsa_get_pubkey_size(size_t keybits, size_t *size);

ali_crypto_result sal_rsa_init_keypair(
          size_t keybits, const uint8_t *n, size_t n_size, const uint8_t *e,
          size_t e_size, const uint8_t *d, size_t d_size, const uint8_t *p,
          size_t p_size, const uint8_t *q, size_t q_size, const uint8_t *dp,
          size_t dp_size, const uint8_t *dq, size_t dq_size, const uint8_t *qp,
          size_t qp_size, rsa_keypair_t *keypair);

ali_crypto_result sal_rsa_init_pubkey(size_t keybits, const uint8_t *n,
                                      size_t n_size, const uint8_t *e,
                                      size_t e_size, rsa_pubkey_t *pubkey);

ali_crypto_result sal_rsa_gen_keypair(size_t keybits, const uint8_t *e,
                                      size_t e_size, rsa_keypair_t *keypair);
                                      
ali_crypto_result sal_rsa_get_key_attr(rsa_key_attr_t attr,
                                       rsa_keypair_t *keypair, void *buffer,
                                       size_t *size);

ali_crypto_result sal_rsa_public_encrypt(const rsa_pubkey_t *pub_key,
                                         const uint8_t *src, size_t src_size,
                                         uint8_t *dst, size_t *dst_size,
                                         rsa_padding_t padding);

ali_crypto_result sal_rsa_private_decrypt(const rsa_keypair_t *priv_key,
                                          const uint8_t *src, size_t src_size,
                                          uint8_t *dst, size_t *dst_size,
                                          rsa_padding_t padding);


ali_crypto_result sal_rsa_sign(const rsa_keypair_t *priv_key,
                                       const uint8_t *dig, size_t dig_size,
                                       uint8_t *sig, size_t *sig_size,
                                       rsa_padding_t padding);

ali_crypto_result sal_rsa_verify(const rsa_pubkey_t *pub_key,
                                 const uint8_t *dig, size_t dig_size,
                                 const uint8_t *sig, size_t sig_size,
                                 rsa_padding_t padding, bool *p_result);
//sm2
ali_crypto_result sal_sm2_get_keypair_size(size_t keybits, size_t *size);

ali_crypto_result sal_sm2_get_pubkey_size(size_t keybits, size_t *size);

ali_crypto_result sal_sm2_init_keypair(
                      const uint8_t *x, size_t x_size,
                      const uint8_t *y, size_t y_size,
                      const uint8_t *d, size_t d_size,
                      size_t curve, ecc_keypair_t *keypair);

ali_crypto_result sal_sm2_init_pubkey(
                      const uint8_t *x, size_t x_size,
                      const uint8_t *y, size_t y_size,
                      size_t curve, ecc_pubkey_t *pubkey);

ali_crypto_result sal_sm2_gen_keypair(
                      size_t curve, ecc_keypair_t *keypair);

ali_crypto_result sal_sm2_sign(const ecc_keypair_t *priv_key,
                      const uint8_t *src, size_t src_size,
                      uint8_t *signature, size_t *sig_size);

ali_crypto_result sal_sm2_verify(const ecc_pubkey_t *pub_key,
                      const uint8_t *src, size_t src_size,
                      const uint8_t *signature, size_t sig_size,
                      bool *p_result);

ali_crypto_result sal_sm2_encrypt(const ecc_pubkey_t *pub_key,
                      const uint8_t *plaintext, size_t p_size,
                      uint8_t *ciphertext, size_t *c_size);

ali_crypto_result sal_sm2_decrypt(const ecc_keypair_t *priv_key,
                      const uint8_t *ciphertext, size_t *c_size,
                      uint8_t *plaintext, size_t *p_size);

ali_crypto_result sal_sm2dh_derive_secret(const uint8_t flag_server,
                      const uint8_t *ID, size_t ID_size,
                      const uint8_t *peer_ID, size_t peer_ID_size,
                      const ecc_keypair_t *priv_key,
                      const ecc_keypair_t *tmp_priv_key,
                      const ecc_pubkey_t *peer_pubkey,
                      const ecc_pubkey_t *tmp_peer_pubkey,
                      uint8_t *shared_secret, const size_t secret_size);

//rand
ali_crypto_result sal_rand_gen(uint8_t *buf, size_t len);

ali_crypto_result sal_seed(uint8_t *seed, size_t seed_len);

#endif /* _ALI_CRYPTO_ALGO_H_ */
