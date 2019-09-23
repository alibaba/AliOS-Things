#ifndef _ALI_CRYPTO_H_
#define _ALI_CRYPTO_H_

#include "ali_crypto_types.h"

typedef enum _ali_crypto_result
{
    ALI_CRYPTO_ERROR = (int)0xffff0000, /* Generic Error */
    ALI_CRYPTO_NOSUPPORT,               /* Scheme not support */
    ALI_CRYPTO_INVALID_KEY, /* Invalid Key in asymmetric scheme: RSA/DSA/ECCP/DH
                               etc */
    ALI_CRYPTO_INVALID_TYPE,           /* Invalid
                                          aes_type/des_type/authenc_type/hash_type/cbcmac_type/cmac_type
                                        */
    ALI_CRYPTO_INVALID_CONTEXT,        /* Invalid context in multi-thread
                                          cipher/authenc/mac/hash etc */
    ALI_CRYPTO_INVALID_PADDING,        /* Invalid
                                          sym_padding/rsassa_padding/rsaes_padding */
    ALI_CRYPTO_INVALID_AUTHENTICATION, /* Invalid authentication in
                                          AuthEnc(AES-CCM/AES-GCM)/asymmetric
                                          verify(RSA/DSA/ECCP DSA) */
    ALI_CRYPTO_INVALID_ARG,            /* Invalid arguments */
    ALI_CRYPTO_INVALID_PACKET, /* Invalid packet in asymmetric enc/dec(RSA) */
    ALI_CRYPTO_LENGTH_ERR,     /* Invalid Length in arguments */
    ALI_CRYPTO_OUTOFMEM,       /* Memory alloc NULL */
    ALI_CRYPTO_SHORT_BUFFER,   /* Output buffer is too short to store result */
    ALI_CRYPTO_NULL,           /* NULL pointer in arguments */
    ALI_CRYPTO_ERR_STATE,   /* Bad state in mulit-thread cipher/authenc/mac/hash
                               etc */
    ALI_CRYPTO_SUCCESS = 0, /* Success */
} ali_crypto_result;

#define AES_BLOCK_SIZE \
    16 /* don't change this value,since AES only support 16 byte block size */
#define AES_IV_SIZE    16
#define DES_BLOCK_SIZE 8
#define DES_IV_SIZE    8

typedef enum _sym_padding_t
{
    SYM_NOPAD     = 0,
    SYM_PKCS5_PAD = 1,
    SYM_ZERO_PAD  = 2,
} sym_padding_t;

typedef enum _aes_type_t
{
    AES_ECB    = 0,
    AES_CBC    = 1,
    AES_CTR    = 2,
    AES_CTS    = 3,
    AES_XTS    = 4,
    AES_CFB8   = 6,
    AES_CFB128 = 7,
} aes_type_t;

typedef enum _des_type_t
{
    DES_ECB  = 0,
    DES_CBC  = 1,
    DES3_ECB = 2,
    DES3_CBC = 3,
} des_type_t;

typedef enum _authenc_type_t
{
    AES_CCM = 0,
    AES_GCM = 1,
} authenc_type_t;

typedef enum _hash_type_t
{
    HASH_NONE = 0,
    SHA1      = 1,
    SHA224    = 2,
    SHA256    = 3,
    SHA384    = 4,
    SHA512    = 5,
    MD5       = 6,
} hash_type_t;

enum
{
    MD5_HASH_SIZE    = 16,
    SHA1_HASH_SIZE   = 20,
    SHA224_HASH_SIZE = 28,
    SHA256_HASH_SIZE = 32,
    SHA384_HASH_SIZE = 48,
    SHA512_HASH_SIZE = 64,
    MAX_HASH_SIZE    = 64,
};

#define HASH_SIZE(type)                         \
    (((type) == SHA1)                           \
       ? (SHA1_HASH_SIZE)                       \
       : (((type) == SHA224)                    \
            ? (SHA224_HASH_SIZE)                \
            : (((type) == SHA256)               \
                 ? (SHA256_HASH_SIZE)           \
                 : (((type) == SHA384)          \
                      ? (SHA384_HASH_SIZE)      \
                      : (((type) == SHA512)     \
                           ? (SHA512_HASH_SIZE) \
                           : (((type) == MD5) ? (MD5_HASH_SIZE) : (0)))))))

typedef enum _cbcmac_type_t
{
    AESCBCMAC  = 0,
    DESCBCMAC  = 1,
    DES3CBCMAC = 2,
} cbcmac_type_t;

typedef enum _cmac_type_t
{
    AESCMAC = 0,
} cmac_type_t;

typedef enum _rsa_key_attr_t
{
    RSA_MODULUS          = 0x130,
    RSA_PUBLIC_EXPONENT  = 0x230,
    RSA_PRIVATE_EXPONENT = 0x330,
    RSA_PRIME1           = 0x430,
    RSA_PRIME2           = 0x530,
    RSA_EXPONENT1        = 0x630,
    RSA_EXPONENT2        = 0x730,
    RSA_COEFFICIENT      = 0x830,
} rsa_key_attr_t;

typedef enum _dh_key_attr_t
{
    DH_PRIME    = 0x140,
    DH_BASE     = 0x240,
    DH_PRIVATE  = 0x340,
    DH_PUBLIC   = 0x440,
    DH_SUBPRIME = 0x540,
    DH_X_BITS   = 0x640,
} dh_key_attr_t;

typedef enum _dsa_key_attr_t
{
    DSA_PRIME    = 0x150,
    DSA_SUBPRIME = 0x250,
    DSA_BASE     = 0x350,
    DSA_PRIVATE  = 0x450,
    DSA_PUBLIC   = 0x550,
} dsa_key_attr_t;

typedef enum _rsa_pad_type_t
{
    RSA_NOPAD = 0,

    /* encrypt */
    RSAES_PKCS1_V1_5      = 10,
    RSAES_PKCS1_OAEP_MGF1 = 11,

    /* sign */
    RSASSA_PKCS1_V1_5     = 20,
    RSASSA_PKCS1_PSS_MGF1 = 21,
} rsa_pad_type_t;

typedef struct _rsa_padding_t
{
    rsa_pad_type_t type;
    union
    {
        struct
        {
            hash_type_t type;
        } rsaes_oaep;
        struct
        {
            hash_type_t type; /* md5/sha1/sha224/sha256/sha384/sha512 */
        } rsassa_v1_5;
        struct
        {
            hash_type_t type; /* sha1/sha224/sha256/sha384/sha512 */
            size_t      salt_len;
        } rsassa_pss;
    } pad;
} rsa_padding_t;

typedef enum _dsa_padding_t
{
    DSA_SHA1   = 0,
    DSA_SHA224 = 1,
    DSA_SHA256 = 2,
} dsa_padding_t;

enum
{
    CRYPTO_STATUS_CLEAN       = 0,
    CRYPTO_STATUS_INITIALIZED = 1,
    CRYPTO_STATUS_PROCESSING  = 2,
    CRYPTO_STATUS_FINISHED    = 3,
};

/* internal data types */
typedef struct __rsa_keypair rsa_keypair_t;
typedef struct __rsa_pubkey  rsa_pubkey_t;

typedef struct __dsa_keypair dsa_keypair_t;
typedef struct __dsa_pubkey  dsa_pubkey_t;

typedef struct __dh_keypair dh_keypair_t;
typedef struct __dh_pubkey  dh_pubkey_t;

typedef struct __ecc_keypair ecc_keypair_t;
typedef struct __ecc_pubkey  ecc_pubkey_t;

/********************************************************************/
/*                             SYM                                  */
/********************************************************************/

/*
 * type[in]:       must be AES_ECB/AES_CBC/AES_CTR/AES_CTS/AES_XTS
 * size[out]:      check size != NULL
 *                   -- caller will alloc "size" memory as context buffer later
 */
ali_crypto_result breeze_aes_get_ctx_size(aes_type_t type, size_t *size);

/*
 * type[in]:        must be AES_ECB/AES_CBC/AES_CTR/AES_CTS/AES_XTS
 * is_enc[in]:      [true] for encrypt, [false] for decrypt
 * key1[in]:        the encrypt key
 * key2[in]:        the tweak encrypt key for XTS mode
 * keybytes[in]:    the key length of the keys(each) in bytes, should be
 * 16/24/32 bytes iv[in]:          only valid for
 * AES_CBC/AES_CTR/AES_CTS/AES_XTS
 *                    -- function can read 16 bytes from this address as the
 * internal iv context[in/out]: caller allocated memory used as internal
 * context, which size is got through breeze_aes_get_ctx_size
 *                    -- [in]:  status of context should be CLEAN or FINISHED
 *                    -- [out]: status of context is changed to INITIALIZED
 */
ali_crypto_result ali_aes_init(aes_type_t type, bool is_enc,
                               const uint8_t *key1, const uint8_t *key2,
                               size_t keybytes, const uint8_t *iv,
                               void *context);

/*
 * src[in]:         plaintext for encrypt, ciphertext for decrypt
 * dst[out]:        ciphertext for encrypt, plaintext for decrypt
 * size[in]:        the number of bytes to process
 *                    -- ECB/CBC/CTS/XTS, must be multiple of the cipher block
 * size
 *                    -- CTR, any positive integer
 * context[in/out]: internal context
 *                    -- [in]:  status of context should be INITED or PROCESSING
 *                    -- [out]: status of context is changed to PROCESSING
 */
ali_crypto_result ali_aes_process(const uint8_t *src, uint8_t *dst, size_t size,
                                  void *context);

/*
 * src[in]:          source data, plaintext for encrypt/ciphertext for decrypt
 *                     -- may be NULL, which identify that no input data, only
 * terminate crypto src_size[in]:     the number of bytes to process, src_size
 * == 0 if src == NULL
 *                     -- encrypt: SYM_NOPAD - must be multiple of the cipher
 * block size
 *                     -- decrypt: ECB/CBC - must be multiple of the cipher
 * block size dst[out]:         destination data, which is used to save
 * processed data
 *                     -- may be NULL if no input src data(src == NULL &&
 * src_size == 0)
 *                     -- ciphertext for encrypt, plaintext for decrypt
 *                     -- if no SYM_NOPAD, should remove padding data
 * accordingly dst_size[in/out]: the length of processed data, may be NULL if
 * dst == NULL
 *                     -- [in]:  buffer size
 *                     -- [out]: the actual encrypted/decrypted data size
 * padding[in]:      padding type for aes mode
 *                     -- ECB/CBC: only support SYM_NOPAD
 *                     -- CTR/CTS/XTS: padding is ignored
 * context[in/out]:  internal context
 *                     -- [in]:  status of context should be INITED or
 * PROCESSING
 *                     -- [out]: status of context is changed to FINISHED
 */
ali_crypto_result ali_aes_finish(const uint8_t *src, size_t src_size,
                                 uint8_t *dst, size_t *dst_size,
                                 sym_padding_t padding, void *context);

ali_crypto_result ali_aes_reset(void *context);
ali_crypto_result ali_aes_copy_context(void *dst_ctx, void *src_ctx);
/* des include des3 */
/*
type:       must be DES_ECB/DES_CBC/DES3_ECB/DES3_CBC
size:       check size != NULL
*/
ali_crypto_result ali_des_get_ctx_size(des_type_t type, size_t *size);

/*
type:       must be DES_ECB/DES_CBC/DES3_ECB/DES3_CBC
is_enc:     [true] for encrypt, [false] for decrypt.
key:        function will read 'keybytes' of data as key.
keybytes:   for DES_ECB/DES_CBC, must be 64.
            for DES3_ECB/DES3_CBC, must be 128 or 192.
iv:         for DES_ECB/DES3_ECB: must be NULL.
            for DES_CBC/DES3_CBC: function will read 8 bytes as algo iv.
context:    function will use size which return from function
'ali_des_get_ctx_size' as internal context. function will check the [status[ of
'context', must be CLEAN or FINISH. function will initialize the [status] of
'context' to INIT. function will save the 'type', 'is_enc', or maybe 'iv',
'key', 'keybytes' in 'context'. function will initialize the 'context' to a
valid context.
*/
ali_crypto_result ali_des_init(des_type_t type, bool is_enc, const uint8_t *key,
                               size_t keybytes, const uint8_t *iv,
                               void *context);

/*
src:        function will read 'size' of data from this area as source data.
            MUST be NULL if 'size' is 0
dst:        function will write 'size' of data to this area as destination data.
            MUST be NULL if 'size' is 0
size:       the length of source data.
            must be multiple of 8 bytes. or 0.
            if size == 0, src MUST be NULL, dst MUST be NULL, return
TEE_SUCCESS. context:    function will use size which return from function
'ali_des_get_ctx_size' as internal context. function will check it is a valid
context. function will check the [status] of 'context', must be INIT or PROCESS.
            function will change the [status] of 'context' to PROCESS.
            function will do encrypt or decrypt indicated by the content in
'context'.
*/
ali_crypto_result ali_des_process(const uint8_t *src, uint8_t *dst, size_t size,
                                  void *context);
/*
src:        function will read 'src_size' of data from this area as source data.
            MUST be NULL if 'src_size' is 0.
src_size:   the length of source data. this have different rules for differnt
'type' and 'padding'. a. for 'padding' is SYM_NOPAD: a.1 MUST be multiple of 16
bytes. or 0. b. for other 'padding': b.1 can be any integer or 0. if 'src_size'
== 0, 'src' MUST be NULL, 'dst' MUST be NULL, and this function will reaturn
SUCCESS. dst:        function will write certain length which is retuned by
'dst_size' of data to this area as destination data. MUST be NULL if 'size' is 0
dst_size:   function will wirte some integer to this area to indicate the length
of destination data. the return value depends on 'src_size' and 'padding' a.1
for 'padding' is SYM_NOPAD, dst_size is equal to src_size. a.2 for other
'padding', 'dst_size' is 16 bytes align up of 'src_size'. padding:    the
padding type of finish. can be anyone of SYM_NOPAD/SYM_PKCS5_PAD/SYM_ZERO_PAD.
context:    function will use size which return from function
'ali_des_get_ctx_size' as internal context. function will check it is a valid
context. function will check the [status] of 'context', must be INIT or PROCESS.
            function will change the [status] of 'context' to FINISH.
            function will do encrypt or decrypt indicated by the content in
'context'. function MUST clean the content of context before this fucntion
return.
*/
ali_crypto_result ali_des_finish(const uint8_t *src, size_t src_size,
                                 uint8_t *dst, size_t *dst_size,
                                 sym_padding_t padding, void *context);

ali_crypto_result ali_des_reset(void *context);
ali_crypto_result ali_des_copy_context(void *dst_ctx, void *src_ctx);

/********************************************************************/
/*                    Authenticated Encryption                      */
/********************************************************************/
/*
type:       MUST be AES_CCM/AES_GCM
size:       check size != NULL
*/
ali_crypto_result ali_authenc_get_ctx_size(authenc_type_t type, size_t *size);
/*
type:       MUST be AES_CCM/AES_GCM
is_enc:     [true] for encrypt, [false] for decrypt.
key:        function will read 'keybytes' of data as key.
keybytes:   MUST be 16(128 bits)/24(256 bits)/32(512 bits).
nonce:      the operation 'nonce' for AES_CCM, the IV of AES_GCM.
            function will read 'nonce_len' of data as nonce or IV.
nonce_len:  the nonce length for AES_CCM, the IV length for AES_GCM.
tag_len:    the tag byte length.
payload_len: only valid for AES_CCM, the payload length. Ignore for AES_GCM.
aad_len:    only valid for AES_CCM, the aad length. Ignore for AES_GCM.
context:    function will use size which return from function
'ali_authenc_get_ctx_size' as internal context. function will check the [status]
of 'context', must be CLEAN or FINISH. function will initialize the [status] of
'context' to INIT. function will save the 'type', 'is_enc', or maybe 'nonce',
'nonce_len', 'tag_len', 'payload_len', 'aad_len' in 'context'. function will
initialize the 'context' to a valid context.
*/
ali_crypto_result ali_authenc_init(authenc_type_t type, bool is_enc,
                                   const uint8_t *key, size_t keybytes,
                                   const uint8_t *nonce, size_t nonce_len,
                                   size_t tag_len,
                                   size_t payload_len, /* valid only in CCM */
                                   size_t aad_len,     /* valid only in CCM */
                                   void  *context);
/*
aad:        the address of aad.
            function will read 'aad_size' of data from this address as aad.
aad_size:   the length in bytes of aad.
            for AES_CCM:
                the total summary of 'aad_size' of multiple calling this
function MUST equal to the 'aad_len' parameter in ali_authenc_init. context:
function will use size which return from function 'ali_authenc_get_ctx_size' as
internal context. function will check it is a valid context. function will check
the [status] of 'context', must be INIT or UPDATE_AAD. function will change the
[status] of 'context' to UPDATE_AAD.
*/
ali_crypto_result ali_authenc_update_aad(const uint8_t *aad, size_t aad_size,
                                         void *context);
/*

src:        function will read 'size' of data from this area as source data.
            MUST be NULL if 'size' is 0
dst:        function will write 'size' of data to this area as destination data.
            MUST be NULL if 'size' is 0
size:       the length of source data, can be any integer or 0.
            for AES_CCM.
                the total summary of 'size' of multiple calling this function
MUST equal to the 'payload_len' parameter in ali_authenc_init. context: function
will use size which return from function 'ali_authenc_get_ctx_size' as internal
context. function will check it is a valid context. function will check the
[status] of 'context', must be UPDATE_AAD or PROCESS. function will change the
[status] of 'context' to PROCESS. function will do encrypt or decrypt indicated
by the content in 'context'.
*/
ali_crypto_result ali_authenc_process(const uint8_t *src, uint8_t *dst,
                                      size_t size, void *context);
/*
src:        function will read 'size' of data from this area as source data.
            MUST be NULL if 'src_size' is 0.
src_size:   the length of source data.
            if 'src_size' == 0, 'src' MUST be NULL, 'dst' MUST be NULL, and this
function will reaturn SUCCESS. dst:        function will write certain length
which is retuned by 'dst_size' of data to this area as destination data. MUST be
NULL if 'size' is 0 dst_size:   function will wirte some integer to this area to
indicate the length of destination data. tag:        the tag returned by ae
encrypt. tag_len:    the tag length. context:    function will use size which
return from function 'ali_authenc_get_ctx_size' as internal context. function
will check it is a valid context. function will check the [status] of 'context',
must be UPDATE_AAD or PROCESS. function will change the [status] of 'context' to
FINISH. the 'is_enc' indicated by the content in 'context' MUST be ture.
            function will do encrypt or decrypt indicated by the content in
'context'. function MUST clean the content of context before this fucntion
return.
*/
ali_crypto_result ali_authenc_enc_finish(const uint8_t *src, size_t src_size,
                                         uint8_t *dst, size_t *dst_size,
                                         uint8_t *tag, size_t *tag_len,
                                         void *context);
/*
src:        function will read 'size' of data from this area as source data.
            MUST be NULL if 'src_size' is 0.
src_size:   the length of source data.
            if 'src_size' == 0, 'src' MUST be NULL, 'dst' MUST be NULL, and this
function will reaturn SUCCESS. dst:        function will write certain length
which is retuned by 'dst_size' of data to this area as destination data. MUST be
NULL if 'size' is 0 dst_size:   function will wirte some integer to this area to
indicate the length of destination data. tag:        the tag parameter. function
will read 'tag_len' of data from this address as the decrypt tag. tag_len: the
tag length. context:    function will use size which return from function
'ali_authenc_get_ctx_size' as internal context. function will check it is a
valid context. function will check the [status] of 'context', must be UPDATE_AAD
or PROCESS. function will change the [status] of 'context' to FINISH. the
'is_enc' indicated by the content in 'context' MUST be false. function will do
encrypt or decrypt indicated by the content in 'context'. function MUST clean
the content of context before this fucntion return.
*/
ali_crypto_result ali_authenc_dec_finish(const uint8_t *src, size_t src_size,
                                         uint8_t *dst, size_t *dst_size,
                                         const uint8_t *tag, size_t tag_len,
                                         void *context);
ali_crypto_result ali_authenc_reset(void *context);
ali_crypto_result ali_authenc_copy_context(void *dst_ctx, void *src_ctx);
/********************************************************************/
/*                             HASH                                 */
/********************************************************************/
ali_crypto_result ali_hash_get_ctx_size(hash_type_t type, size_t *size);
ali_crypto_result ali_hash_init(hash_type_t type, void *context);
ali_crypto_result ali_hash_update(const uint8_t *src, size_t size,
                                  void *context);
ali_crypto_result ali_hash_final(uint8_t *dgst, void *context);
ali_crypto_result ali_hash_reset(void *context);
ali_crypto_result ali_hash_copy_context(void *dst_ctx, void *src_ctx);

ali_crypto_result ali_hash_digest(hash_type_t type, const uint8_t *src,
                                  size_t size, uint8_t *dgst);

/********************************************************************/
/*                             MAC                                  */
/********************************************************************/
/* hmac */
ali_crypto_result ali_hmac_get_ctx_size(hash_type_t type, size_t *size);
ali_crypto_result ali_hmac_init(hash_type_t type, const uint8_t *key,
                                size_t keybytes, void *context);
ali_crypto_result ali_hmac_update(const uint8_t *src, size_t size,
                                  void *context);
ali_crypto_result ali_hmac_final(uint8_t *dgst, void *context);
ali_crypto_result ali_hmac_reset(void *context);
ali_crypto_result ali_hmac_copy_context(void *dst_ctx, void *src_ctx);
ali_crypto_result ali_hmac_digest(hash_type_t type, const uint8_t *key,
                                  size_t keybytes, const uint8_t *src,
                                  size_t size, uint8_t *dgst);

/* cbcmac */
ali_crypto_result ali_cbcmac_get_ctx_size(cbcmac_type_t type, size_t *size);
ali_crypto_result ali_cbcmac_init(cbcmac_type_t type, const uint8_t *key,
                                  size_t keybytes, void *context);
ali_crypto_result ali_cbcmac_update(const uint8_t *src, size_t size,
                                    void *context);
ali_crypto_result ali_cbcmac_final(sym_padding_t padding, uint8_t *dgst,
                                   void *context);
ali_crypto_result ali_cbcmac_reset(void *context);
ali_crypto_result ali_cbcmac_copy_context(void *dst_ctx, void *src_ctx);
ali_crypto_result ali_cbcmac_digest(cbcmac_type_t type, const uint8_t *key,
                                    size_t keybytes, const uint8_t *src,
                                    size_t size, sym_padding_t padding,
                                    uint8_t *dgst);

/* cmac */
ali_crypto_result ali_cmac_get_ctx_size(cmac_type_t type, size_t *size);
ali_crypto_result ali_cmac_init(cmac_type_t type, const uint8_t *key,
                                size_t keybytes, void *context);
ali_crypto_result ali_cmac_update(const uint8_t *src, size_t size,
                                  void *context);
ali_crypto_result ali_cmac_final(sym_padding_t padding, uint8_t *dgst,
                                 void *context);
ali_crypto_result ali_cmac_reset(void *context);
ali_crypto_result ali_cmac_copy_context(void *dst_ctx, void *src_ctx);
ali_crypto_result ali_cmac_digest(cmac_type_t type, const uint8_t *key,
                                  size_t keybytes, const uint8_t *src,
                                  size_t size, sym_padding_t padding,
                                  uint8_t *dgst);

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
ali_crypto_result ali_rsa_init_keypair(
  size_t keybits, const uint8_t *n, size_t n_size, const uint8_t *e,
  size_t e_size, const uint8_t *d, size_t d_size, const uint8_t *p,
  size_t p_size, const uint8_t *q, size_t q_size, const uint8_t *dp,
  size_t dp_size, const uint8_t *dq, size_t dq_size, const uint8_t *qp,
  size_t qp_size, rsa_keypair_t *keypair);

/*
 * Initialize RSA public key
 *
 * keybits[in]:    rsa key length in bits
 * n/n_size[in]:   rsa modulus data and size in bytes
 * e/e_size[in]:   rsa public exponent data and size in bytes
 * pubkey[out]:    output buffer, which is used to save initialized rsa public
 * key
 */
ali_crypto_result ali_rsa_init_pubkey(size_t keybits, const uint8_t *n,
                                      size_t n_size, const uint8_t *e,
                                      size_t e_size, rsa_pubkey_t *pubkey);

/*
 * Generate RSA keypair
 *
 * keybits[in]:   rsa key length in bits
 * e[in]:         optional, public exponent
 * e_size[in]:    optional, public exponent size in bytes
 * keypair[out]:  output buffer, which is used to save generated rsa key pair
 */
ali_crypto_result ali_rsa_gen_keypair(size_t keybits, const uint8_t *e,
                                      size_t e_size, rsa_keypair_t *keypair);

/*
 * Get key attribute
 *
 * attr[in]:      rsa key attribute ID
 * keypair[in]:   rsa keypair buffer
 * buffer[out]:   buffer, which is used to save required attribute
 * size[in/out]:  buffer max size and key attribute actual size in bytes
 */
ali_crypto_result ali_rsa_get_key_attr(rsa_key_attr_t attr,
                                       rsa_keypair_t *keypair, void *buffer,
                                       size_t *size);

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
                               uint8_t *sig, size_t *sig_size,
                               rsa_padding_t padding);

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

/* DSA sign/verify */
/*
    g:  Generator of subgroup (public)
    p:  Prime number (public)
    q:  Order of subgroup (public)
    y:  Public key
    x:  Private key
*/
ali_crypto_result ali_dsa_get_keypair_size(size_t keybits, size_t *size);
ali_crypto_result ali_dsa_get_pubkey_size(size_t keybits, size_t *size);
ali_crypto_result ali_dsa_init_keypair(size_t keybits, const uint8_t *g,
                                       size_t g_size, const uint8_t *p,
                                       size_t p_size, const uint8_t *q,
                                       size_t q_size, const uint8_t *y,
                                       size_t y_size, const uint8_t *x,
                                       size_t x_size, dsa_keypair_t *keypair);
ali_crypto_result ali_dsa_init_pubkey(size_t keybits, const uint8_t *g,
                                      size_t g_size, const uint8_t *p,
                                      size_t p_size, const uint8_t *q,
                                      size_t q_size, const uint8_t *y,
                                      size_t y_size, dsa_pubkey_t *pubkey);
ali_crypto_result ali_dsa_gen_keypair(size_t keybit, const uint8_t *g,
                                      size_t g_size, const uint8_t *p,
                                      size_t p_size, const uint8_t *q,
                                      size_t q_size, dsa_keypair_t *keypair);
ali_crypto_result ali_dsa_sign(const dsa_keypair_t *priv_key,
                               const uint8_t *src, size_t src_size,
                               uint8_t *signature, size_t *sig_size,
                               dsa_padding_t padding);
ali_crypto_result ali_dsa_verify(const dsa_pubkey_t *pub_key,
                                 const uint8_t *src, size_t src_size,
                                 const uint8_t *signature, size_t sig_size,
                                 dsa_padding_t padding, bool *result);

ali_crypto_result ali_dsa_get_key_attr(dsa_key_attr_t attr,
                                       dsa_keypair_t *keypair, void *buffer,
                                       uint32_t *size);

/* DH derive shared secret */
/*
    g:  Generator of Z_p
    p:  Prime modulus
    y:  Public key
    x:  Private key
    q:  Optional
    xbits:  Optional
*/
ali_crypto_result ali_dh_get_keypair_size(size_t keybits, size_t *size);
ali_crypto_result ali_dh_get_pubkey_size(size_t keybits, size_t *size);
ali_crypto_result ali_dh_init_keypair(size_t keybits, const uint8_t *g,
                                      size_t g_size, const uint8_t *p,
                                      size_t p_size, const uint8_t *y,
                                      size_t y_size, const uint8_t *x,
                                      size_t x_size, const uint8_t *q,
                                      size_t        q_size, /* optional */
                                      size_t        xbits,  /* optional */
                                      dh_keypair_t *keypair);
ali_crypto_result ali_dh_init_pubkey(size_t keybits, const uint8_t *y,
                                     size_t y_size, dh_pubkey_t *pubkey);
ali_crypto_result ali_dh_gen_keypair(size_t keybit, const uint8_t *g,
                                     size_t g_size, const uint8_t *p,
                                     size_t p_size, const uint8_t *q,
                                     size_t q_size, size_t xbits,
                                     dh_keypair_t *keypair);
ali_crypto_result ali_dh_derive_secret(const dh_keypair_t *priv_key,
                                       const dh_pubkey_t  *peer_pub_key,
                                       uint8_t            *shared_secret,
                                       size_t             *secret_size);
ali_crypto_result ali_dh_get_key_attr(dh_key_attr_t attr, dh_keypair_t *keypair,
                                      void *buffer, uint32_t *size);

/*
    d:  Private value
    x:  Public value x
    y:  Public value y
    curve:  Curve type
*/
ali_crypto_result ali_ecc_get_keypair_size(size_t curve, size_t *size);
ali_crypto_result ali_ecc_get_pubkey_size(size_t curve, size_t *size);
ali_crypto_result ali_ecc_init_keypair(const uint8_t *x, size_t x_size,
                                       const uint8_t *y, size_t y_size,
                                       const uint8_t *d, size_t d_size,
                                       size_t curve, ecc_keypair_t *keypair);
ali_crypto_result ali_ecc_init_pubkey(const uint8_t *x, size_t x_size,
                                      const uint8_t *y, size_t y_size,
                                      size_t curve, ecc_pubkey_t *pubkey);
ali_crypto_result ali_ecc_gen_keypair(size_t curve, ecc_keypair_t *keypair);

/* ECDSA sign/verify */
ali_crypto_result ali_ecdsa_sign(const ecc_keypair_t *priv_key,
                                 const uint8_t *src, size_t src_size,
                                 uint8_t *signature, size_t *sig_size);
ali_crypto_result ali_ecdsa_verify(const ecc_pubkey_t *pub_key,
                                   const uint8_t *src, size_t src_size,
                                   const uint8_t *signature, size_t sig_size,
                                   bool *result);

/* ECDH derive shared secret */
ali_crypto_result ali_ecdh_derive_secret(const ecc_keypair_t *priv_key,
                                         const ecc_pubkey_t  *peer_pubkey_key,
                                         uint8_t             *shared_secret,
                                         size_t              *secret_size);

/* random generator */
ali_crypto_result ali_seed(uint8_t *seed, size_t seed_len);
ali_crypto_result ali_rand_gen(uint8_t *buf, size_t len);

ali_crypto_result ali_crypto_init(void);
void              ali_crypto_cleanup(void);

#endif /* _ALI_CRYPTO_H_ */
