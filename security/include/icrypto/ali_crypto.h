/**
 * Copyright (C) 2017  Alibaba Group Holding Limited.
 **/

/* Alibaba TEE Crypto API: version 1.1 */

#ifndef _ALI_CRYPTO_H_
#define _ALI_CRYPTO_H_

#include "ali_crypto_types.h"
#include "ali_crypto_debug.h"


#define HASH_SIZE(type) (((type) == SHA1) ? (SHA1_HASH_SIZE) : (     \
                             ((type) == SHA224) ? (SHA224_HASH_SIZE) : ( \
                             ((type) == SHA256) ? (SHA256_HASH_SIZE) : ( \
                             ((type) == SM3   ) ? (SM3_HASH_SIZE   ) : ( \
                             ((type) == SHA384) ? (SHA384_HASH_SIZE) : ( \
                             ((type) == SHA512) ? (SHA512_HASH_SIZE) : ( \
                             ((type) == MD5) ? (MD5_HASH_SIZE) : (0))))))))

typedef struct _rsa_padding_t {
    rsa_pad_type_t type;
    union {
        struct {
            hash_type_t type;
        } rsaes_oaep;
        struct {
            hash_type_t type;   /* md5/sha1/sha224/sha256/sha384/sha512 */
        } rsassa_v1_5;
        struct {
            hash_type_t type;   /* sha1/sha224/sha256/sha384/sha512 */
            size_t salt_len;
        } rsassa_pss;
    } pad;
} rsa_padding_t;

/* internal data types */
typedef struct __rsa_keypair  rsa_keypair_t;
typedef struct __rsa_pubkey   rsa_pubkey_t;

typedef struct __ecc_keypair  ecc_keypair_t;
typedef struct __ecc_pubkey   ecc_pubkey_t;

/********************************************************************/
/*                             SYM                                  */
/********************************************************************/

/*
 * type[in]:       must be AES_ECB/AES_CBC/AES_CTR/AES_CFB
 * size[out]:      check size != NULL
 *                   -- caller will alloc "size" memory as context buffer later
 */
ali_crypto_result ali_aes_get_ctx_size(aes_type_t type, size_t *size);

/*
 * type[in]:        must be AES_ECB/AES_CBC/AES_CTR/AES_CFB
 * is_enc[in]:      [true] for encrypt, [false] for decrypt
 * key1[in]:        the encrypt key
 * key2[in]:        the tweak encrypt key for XTS mode
 * keybytes[in]:    the key length of the keys(each) in bytes, should be 16/24/32 bytes
 * iv[in]:          only valid for AES_CBC/AES_CTR
 *                    -- function can read 16 bytes from this address as the internal iv
 * context[in/out]: caller allocated memory used as internal context, which size is got through ali_aes_get_ctx_size
 *                    -- [in]:  status of context should be CLEAN or FINISHED
 *                    -- [out]: status of context is changed to INITIALIZED
 */
ali_crypto_result ali_aes_init(aes_type_t type, bool is_enc,
                               const uint8_t *key1, const uint8_t *key2,
                               size_t keybytes, const uint8_t *iv, void *context);

/*
 * src[in]:         plaintext for encrypt, ciphertext for decrypt
 * dst[out]:        ciphertext for encrypt, plaintext for decrypt
 * size[in]:        the number of bytes to process
 *                    -- ECB/CBC, must be multiple of the cipher block size
 *                    -- CTR/CFB, any positive integer
 * context[in/out]: internal context
 *                    -- [in]:  status of context should be INITED or PROCESSING
 *                    -- [out]: status of context is changed to PROCESSING
 */
ali_crypto_result ali_aes_process(const uint8_t *src, uint8_t *dst,
                                  size_t size, void *context);

/*
 * src[in]:          source data, plaintext for encrypt/ciphertext for decrypt
 *                     -- may be NULL, which identify that no input data, only terminate crypto
 * src_size[in]:     the number of bytes to process, src_size == 0 if src == NULL
 *                     -- encrypt: SYM_NOPAD - must be multiple of the cipher block size
 *                     -- decrypt: ECB/CBC - must be multiple of the cipher block size
 * dst[out]:         destination data, which is used to save processed data
 *                     -- may be NULL if no input src data(src == NULL && src_size == 0)
 *                     -- ciphertext for encrypt, plaintext for decrypt
 *                     -- if no SYM_NOPAD, should remove padding data accordingly
 * dst_size[in/out]: the length of processed data, may be NULL if dst == NULL
 *                     -- [in]:  buffer size
 *                     -- [out]: the actual encrypted/decrypted data size
 * padding[in]:      padding type for aes mode
 *                     -- ECB/CBC: only support SYM_NOPAD
 *                     -- CTR/CFB: padding is ignored
 * context[in/out]:  internal context
 *                     -- [in]:  status of context should be INITED or PROCESSING
 *                     -- [out]: status of context is changed to FINISHED
 */
ali_crypto_result ali_aes_finish(const uint8_t *src, size_t src_size,
                                 uint8_t *dst, size_t *dst_size,
                                 sym_padding_t padding, void *context);

ali_crypto_result ali_aes_reset(void *context);


/* des */
ali_crypto_result ali_des_get_ctx_size(des_type_t type, size_t *size);

/*
type:       must be DES_ECB/DES_CBC/DES3_ECB/DES3_CBC
is_enc:     [true] for encrypt, [false] for decrypt.
key:        function will read 'keybytes' of data as key.
keybytes:   for DES_ECB/DES_CBC, must be 64.
            for DES3_ECB/DES3_CBC, must be 128 or 192.
iv:         for DES_ECB/DES3_ECB: must be NULL.
            for DES_CBC/DES3_CBC: function will read 8 bytes as algo iv.
context:    function will use size which return from function 'ali_des_get_ctx_size'
            as internal context.
            function will check the [status[ of 'context', must be CLEAN or FINISH.
            function will initialize the [status] of 'context' to INIT.
            function will save the 'type', 'is_enc', or maybe 'iv', 'key', 'keybytes' in 'context'.
            function will initialize the 'context' to a valid context.
*/
ali_crypto_result ali_des_init(des_type_t type, bool is_enc,
                               const uint8_t *key, size_t keybytes,
                               const uint8_t *iv, void *context);

/*
src:        function will read 'size' of data from this area as source data.
            MUST be NULL if 'size' is 0
dst:        function will write 'size' of data to this area as destination data.
            MUST be NULL if 'size' is 0
size:       the length of source data.
            must be multiple of 8 bytes. or 0.
            if size == 0, src MUST be NULL, dst MUST be NULL, return TEE_SUCCESS.
context:    function will use size which return from function 'ali_des_get_ctx_size'
            as internal context.
            function will check it is a valid context.
            function will check the [status] of 'context', must be INIT or PROCESS.
            function will change the [status] of 'context' to PROCESS.
            function will do encrypt or decrypt indicated by the content in 'context'.
*/
ali_crypto_result ali_des_process(const uint8_t *src, uint8_t *dst,
                                  size_t size, void *context);
/*
src:        function will read 'src_size' of data from this area as source data.
            MUST be NULL if 'src_size' is 0.
src_size:   the length of source data. this have different rules for differnt 'type' and 'padding'.
            a. for 'padding' is SYM_NOPAD:
                a.1 MUST be multiple of 16 bytes. or 0.
            b. for other 'padding':
                b.1 can be any integer or 0.
            if 'src_size' == 0, 'src' MUST be NULL, 'dst' MUST be NULL, and this function will reaturn SUCCESS.
dst:        function will write certain length which is retuned by 'dst_size'
            of data to this area as destination data.
            MUST be NULL if 'size' is 0
dst_size:   function will wirte some integer to this area to indicate the length of destination data.
            the return value depends on 'src_size' and 'padding'
            a.1 for 'padding' is SYM_NOPAD, dst_size is equal to src_size.
            a.2 for other 'padding', 'dst_size' is 16 bytes align up of 'src_size'.
padding:    the padding type of finish.
            can be anyone of SYM_NOPAD/SYM_PKCS5_PAD/SYM_ZERO_PAD.
context:    function will use size which return from function 'ali_des_get_ctx_size'
            as internal context.
            function will check it is a valid context.
            function will check the [status] of 'context', must be INIT or PROCESS.
            function will change the [status] of 'context' to FINISH.
            function will do encrypt or decrypt indicated by the content in 'context'.
            function MUST clean the content of context before this fucntion return.
*/
ali_crypto_result ali_des_finish(const uint8_t *src, size_t src_size,
                                 uint8_t *dst, size_t *dst_size,
                                 sym_padding_t padding, void *context);

ali_crypto_result ali_des_reset(void *context);

/********************************************************************/
/*                             HASH                                 */
/********************************************************************/
ali_crypto_result ali_hash_get_ctx_size(hash_type_t type, size_t *size);
ali_crypto_result ali_hash_init(hash_type_t type, void *context);
ali_crypto_result ali_hash_update(const uint8_t *src, size_t size, void *context);
ali_crypto_result ali_hash_final(uint8_t *dgst, void *context);
ali_crypto_result ali_hash_digest(hash_type_t type,
                                  const uint8_t *src, size_t size, uint8_t *dgst);
ali_crypto_result ali_hash_reset(void *context);

/********************************************************************/
/*                             MAC                                  */
/********************************************************************/
/* hmac */
ali_crypto_result ali_hmac_get_ctx_size(hash_type_t type, size_t *size);
ali_crypto_result ali_hmac_init(hash_type_t type,
                                const uint8_t *key, size_t keybytes, void *context);
ali_crypto_result ali_hmac_update(const uint8_t *src, size_t size, void *context);
ali_crypto_result ali_hmac_final(uint8_t *dgst, void *context);
ali_crypto_result ali_hmac_digest(hash_type_t type,
                                  const uint8_t *key, size_t keybytes,
                                  const uint8_t *src, size_t size, uint8_t *dgst);
ali_crypto_result ali_hmac_reset(void *context);

/********************************************************************/
/*                             ASYM                                 */
/********************************************************************/
/* RSA */
/*
 * e:  Public exponent
 * d:  Private exponent
 * n:  Modulus
 *
 * Optional CRT parameters
 * p, q:  N = pq
 * qp:  1/q mod p
 * dp:  d mod (p-1)
 * dq:  d mod (q-1)
 */

/*
 * keybits[in]: key length in bits
 * size[out]:   total size in bytes of rsa keypair
 */
ali_crypto_result ali_rsa_get_keypair_size(size_t keybits, size_t *size);

/*
 * keybits[in]: key length in bits
 * size[out]:   total size in bytes of rsa public key
 */
ali_crypto_result ali_rsa_get_pubkey_size(size_t keybits, size_t *size);

/*
 * Initialize RSA keypair
 *
 * keybits[in]:    rsa keypair length in bits
 * n/n_size[in]:   rsa modulus data and size in bytes
 * e/e_size[in]:   rsa public exponent data and size in bytes
 * d/d_size[in]:   rsa private exponent data and size in bytes
 * p/p_size[in]:   rsa prime1 data and size in bits, may be NULL/0
 * q/q_size[in]:   rsa prime2 data and size in bits, may be NULL/0
 * dp/dp_size[in]: rsa exponent2 data and size in bits, may be NULL/0
 * dq/dq_size[in]: rsa exponent2 data and size in bits, may be NULL/0
 * dq/dq_size[in]: rsa coefficient data and size in bits, may be NULL/0
 * keypair[out]:   output buffer, which is used to save initialized rsa key pair
 */
ali_crypto_result ali_rsa_init_keypair(size_t keybits,
                                       const uint8_t *n, size_t n_size,
                                       const uint8_t *e, size_t e_size,
                                       const uint8_t *d, size_t d_size,
                                       const uint8_t *p, size_t p_size,
                                       const uint8_t *q, size_t q_size,
                                       const uint8_t *dp, size_t dp_size,
                                       const uint8_t *dq, size_t dq_size,
                                       const uint8_t *qp, size_t qp_size,
                                       rsa_keypair_t *keypair);

/*
 * Initialize RSA public key
 *
 * keybits[in]:    rsa key length in bits
 * n/n_size[in]:   rsa modulus data and size in bytes
 * e/e_size[in]:   rsa public exponent data and size in bytes
 * pubkey[out]:    output buffer, which is used to save initialized rsa public key
 */
ali_crypto_result ali_rsa_init_pubkey(size_t keybits,
                                      const uint8_t *n, size_t n_size,
                                      const uint8_t *e, size_t e_size,
                                      rsa_pubkey_t *pubkey);

/*
 * Generate RSA keypair
 *
 * keybits[in]:   rsa key length in bits
 * e[in]:         optional, public exponent
 * e_size[in]:    optional, public exponent size in bytes
 * keypair[out]:  output buffer, which is used to save generated rsa key pair
 */
ali_crypto_result ali_rsa_gen_keypair(size_t keybits,
                                      const uint8_t *e, size_t e_size,
                                      rsa_keypair_t *keypair);

/*
 * Get key attribute
 *
 * attr[in]:      rsa key attribute ID
 * keypair[in]:   rsa keypair buffer
 * buffer[out]:   buffer, which is used to save required attribute
 * size[in/out]:  buffer max size and key attribute actual size in bytes
 */
ali_crypto_result ali_rsa_get_key_attr(rsa_key_attr_t attr,
                                       rsa_keypair_t *keypair, void *buffer, size_t *size);

ali_crypto_result ali_rsa_public_encrypt(const rsa_pubkey_t *pub_key,
                                         const uint8_t *src, size_t src_size,
                                         uint8_t *dst, size_t *dst_size,
                                         rsa_padding_t padding);
ali_crypto_result ali_rsa_private_decrypt(const rsa_keypair_t *priv_key,
                                          const uint8_t *src, size_t src_size,
                                          uint8_t *dst, size_t *dst_size,
                                          rsa_padding_t padding);

/*
 * dig[in]:          the digest to sign
 * dig_size[in]:     the length of the digest to sign (byte)
 * sig[out]:         the signature data
 * sig_size[in/out]: the buffer size and resulting size of signature
 */
ali_crypto_result ali_rsa_sign(const rsa_keypair_t *priv_key,
                               const uint8_t *dig, size_t dig_size,
                               uint8_t *sig, size_t *sig_size, rsa_padding_t padding);

/*
 * dig[in]:      the digest of message that was signed
 * dig_size[in]: the digest size in bytes
 * sig[in]:      the signature data
 * sig_size[in]: the length of the signature data (byte)
 */
ali_crypto_result ali_rsa_verify(const rsa_pubkey_t *pub_key,
                                 const uint8_t *dig, size_t dig_size,
                                 const uint8_t *sig, size_t sig_size,
                                 rsa_padding_t padding, bool *result);

/* random generator */
ali_crypto_result ali_seed(uint8_t *seed, size_t seed_len);
ali_crypto_result ali_rand_gen(uint8_t *buf, size_t len);

ali_crypto_result ali_crypto_init(void);
void ali_crypto_cleanup(void);

#endif /* _ALI_CRYPTO_H_ */
