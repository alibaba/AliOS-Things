/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#ifndef _KM_H_
#define _KM_H_

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

//the max key name len that KM can support
#define MAX_NAME_LEN 16

//km error code
#define KM_SUCCESS               0x00000000
#define KM_ERR_GENERIC           0xffff0000
#define KM_ERR_AUTHENTICATION    0xffff0001
#define KM_ERR_BAD_PARAMS        0xffff0002
#define KM_ERR_NOT_SUPPORTED     0xffff0003
#define KM_ERR_BAD_FORMAT        0xffff0004
#define KM_ERR_SHORT_BUFFER      0xffff0005
#define KM_ERR_OUT_OF_MEMORY     0xffff0006
#define KM_ERR_ACCESS_CONFLICT   0xffff0007
#define KM_ERR_ITEM_NOT_FOUND    0xffff0008
#define KM_ERR_CORRUPT_KEY       0xffff0009
#define KM_ERR_OVERFLOW          0xffff000A

typedef void * km_op_handle_t;

typedef enum {
    KM_KEY_FORMAT_X509 = 0, /* for public key export*/
    KM_KEY_FORMAT_PKCS8 = 1, /* for asymmetric key pair import*/
    KM_KEY_FORMAT_RAW = 2, /* for symmetric key pair import */
} km_format_t;

/* only support rsa now */
typedef enum {
    KM_RSA,
    KM_ECC,
    KM_AES,
    KM_DES,
    KM_DES3,
    KM_HMAC,
    KM_SM4,
} km_key_type;

typedef enum {
    KM_ECB,
    KM_CBC,
    KM_CTR,
    KM_XTS, //not support yet
    KM_GCM,
} km_block_mode_type;

typedef enum {
    KM_NO_PADDING = 0,
    KM_PKCS1,
    KM_PKCS5,
    KM_PKCS7,
} km_padding_type;

typedef enum {
    KM_DIGEST_NONE = 0,
    KM_MD5,
    KM_SHA1,
    KM_SHA256,
    KM_SHA384,
    KM_SHA512,
    KM_SM3,
} km_digest_type;

typedef enum {
    KM_PURPOSE_ENCRYPT,
    KM_PURPOSE_DECRYPT,
    KM_PURPOSE_SIGN,
    KM_PURPOSE_VERIFY,
} km_purpose_type;

typedef struct _km_rsa_gen_param {
    uint32_t key_size;
    uint64_t exponent;
} km_rsa_gen_param;

typedef struct _km_sym_gen_param {
    uint32_t key_size;
} km_sym_gen_param;

typedef struct _km_sign_param {
    km_padding_type padding_type;
    km_digest_type digest_type;
} km_sign_param;

typedef struct _km_enc_param {
    km_padding_type padding_type;
} km_enc_param;

typedef struct _km_cipher_param {
    km_purpose_type purpose_type;
    km_block_mode_type block_mode;
    km_padding_type padding_type;
} km_cipher_param;

typedef struct _km_hmac_param {
    km_digest_type hash_type;
} km_hmac_param;

typedef struct _km_sym_param {
    km_key_type key_type;
    union {
        km_cipher_param cipher_param;
        km_hmac_param hmac_param;
    };
} km_sym_param;

//for import raw format
typedef struct _km_rsa_key_t {
    uint32_t n_len;
    uint32_t e_len;
    uint32_t d_len;
    uint32_t p_len;
    uint32_t q_len;
    uint32_t dp_len;
    uint32_t dq_len;
    uint32_t iq_len;
    uint8_t *n;
    uint8_t *e;
    uint8_t *d;
    uint8_t *p;
    uint8_t *q;
    uint8_t *dp;
    uint8_t *dq;
    uint8_t *iq;
} km_rsa_key_t;

typedef struct _km_sym_key_t {
    uint32_t key_bit;
    uint8_t *key;
} km_sym_key_t;

typedef struct _km_key_data_t {
    km_key_type type;
    union {
        km_rsa_key_t rsa_key;
        km_sym_key_t sym_key;
    };
} km_key_data_t;

#ifndef KM_FEATURES_BASIC
/*
 * km generate key
 * param: in: name: key name
 * param: in: mame_len: key name len
 * param: in: key_type: generated key type, see km_key_type
 * param: in: arg: the parameters for generate key
 *                 km_rsa_gen_param type for rsa key
 *                 km_sym_gen_param type for sym key
 * return: see km error code
 * */
uint32_t km_generate_key(const char *name, const uint32_t name_len,
                     km_key_type key_type, void *arg);

/*
 * km import key
 * param: in: name: key name
 * param: in: nam_len: key name len
 * param: in: format: the format for key data
 * param: in: key_data: key data to import
 * param: in: key_data_len: the len of key data
 * return: see km error code
 * */
uint32_t km_import_key(const char *name, const uint32_t name_len, km_format_t format,
                   const km_key_data_t *key_data, const uint32_t key_data_len);

/*
 * km export key
 * param: in:     name: key name
 * param: in:     name_len: key name len
 * param: in:     format: the format for export data
 * param: out:    export_data: the data of exported key
 * param: in_out: export_data_size:
 *                in: the length of export_data buffer
 *                out: the real length of export_data
 * return: see km error code
 * */
uint32_t km_export_key(const char *name, const uint32_t name_len, km_format_t format,
                   uint8_t *export_data, uint32_t *export_data_size);

/*
 * km mac: computer mac
 * param: in:     name: key name
 * param: in:     name_len: key name len
 * param: in:     mac_params: the params to computer mac
 * param: in:     iv: iv for computer mac , if no need can pass NULL
 * param: in:     iv_len: the length for iv buffer
 * param: in:     src: the source for computer mac
 * param: in:     src_len: the lengthof src buffer
 * param: out:    mac: output mac
 * param: in_out: mac_len
 *                in: the length of mac buffer
 *                out: the real length of mac result
 * return: see km error code
 * */
uint32_t km_mac(const char *name, const uint32_t name_len, km_sym_param *mac_params,
        const uint8_t *iv, const uint32_t iv_len, uint8_t *src, size_t src_len,
        uint8_t *mac, uint32_t *mac_len);

/*
 * km_delete_key: delete key stored in km
 * param: in:     name: key name
 * param: in:     name_len: key name len
 * return: see km error code
 * */

uint32_t km_delete_key(const char *name, const uint32_t name_len);

/*
 * km_delete_all: delete all key stored in km
 * return: see km error code
 * */
uint32_t km_delete_all();

/*
 * km_envelope_begin: to generate a digit envelope
 * param: out:      ctx
 * param: in:       name: the root key for the envelope
 * param: in:       name_len: the length of the name
 * param: in/out:   protected_key: the encrypted sub_key
 *                  in: for parse envelope
 *                  out: for generate envelope
 * param: in_out:   protected_key_len:
 *                  in: the length of protected_key buffer
 *                  out: the real length of protected_key
 * param: is_enc:   to generate envelope or to parse envelope
 *                  KM_PURPOSE_ENCRYPT for parse envelope
 *                  KM_PURPOSE_DECRYPT for generate envelope
 * return: see km error code
 *
 * */

uint32_t km_envelope_begin(void **ctx, const char *name, const uint32_t name_len,
        uint8_t *iv, uint16_t iv_len,
        uint8_t *protected_key, uint32_t *protected_key_len, km_purpose_type is_enc);

/*
 * km_envelope_update: to generate a digit envelope
 * param: in:       ctx
 * param: in:       src: the source data to encrypt
 * param: in:       src_len: the length of the src
 * param: out:      dest: the out buffer
 * param: in_out:   dest_len:
 *                  in: the length of dest buffer
 *                  out: the real length of dest buffer
 * return: see km error code
 * */

uint32_t km_envelope_update(void *ctx, uint8_t *src, uint32_t src_len,
        uint8_t *dest, uint32_t *dest_len);

/*
 * km_envelope_update: to generate a digit
 * param: in:       ctx
 * param: in:       src: the source data to encrypt
 * param: in:       src_len: the length of the src
 * param: out:      dest: the out buffer
 * param: in_out:   dest_len:
 *                  in: the length of dest buffer
 *                  out: the real length of dest buffer
 * return: see km error code
 *
 * */
uint32_t km_envelope_finish(void *ctx, uint8_t *src, uint32_t src_len,
        uint8_t *dest, uint32_t *dest_len);

/*
 * to get device id
 * param: out: id: the out buffer
 * param: in_out: id_len
 *        in: length of id buffer
 *        out: the real length of id

 * return: see km error code
 * */
uint32_t km_get_attestation(uint8_t *id, uint32_t *id_len);
#endif /* KM_FEATURES_BASIC */

/*
 * km sign
 * param: in:      name: key name
 * param: in:      name_len: key name len
 * param: in:      sign_params: the params to sign
 *                 km_sign_param: for rsa sign
 * param: in:      data: the data to sign
 * param: in:      data_len: the length for data
 * param: out:     out: the out buf
 * param: in_out:  out_len:
 *                 in: the length of out buffer
 *                 out: the real length of out
 * return: see km error code
 *
 * */

uint32_t km_sign(const char *name, const uint32_t name_len, void *sign_params,
             const uint8_t *data, const size_t data_len,
             uint8_t *out, size_t *out_len);

/*
 * km verify
 * param: in:      name: key name
 * param: in:      name_len: key name len
 * param: in:      sign_params: the params to verify
 *                 km_sign_param: for rsa sign
 * param: in:      data: the data to verify
 * param: in:      data_len: the length for data
 * param: in:      signature: the signature buffer
 * param: in:      signature_len: the length of signature buffer
 * return: see km error code
 *
 * */
uint32_t km_verify(const char *name, const uint32_t name_len, void *sign_params,
               const uint8_t *data, const size_t data_len,
               const uint8_t *signature, const size_t signature_len);

/*
 * km asymmetric encrypt
 * param: in:      name: key name
 * param: in:      name_len: key name len
 * param: in:      enc_params: the params to encrypt
 *                 km_enc_param: for rsa encrypt
 * param: in:      src: the source data to encrypt
 * param: in:      src_len: the length for src
 * param: in:      dest: the out buffet
 * param: in_out:  dest_len:
 *                 in: the length of dest buffer
 *                 out: the real length of dest
 * return: see km error code
 * */
uint32_t km_asym_encrypt(const char *name, const uint32_t name_len, void *enc_params,
                const uint8_t *src, const size_t src_len,
             uint8_t *dest, size_t *dest_len);
/*
 * km asymmetric decrypt
 * param: in:      name: key name
 * param: in:      name_len: key name len
 * param: in:      enc_params: the params to decrypt
 *                 km_enc_param: for rsa encrypt
 * param: in:      src: the source data to encrypt
 * param: in:      src_len: the length for src
 * param: in:      dest: the out buffet
 * param: in_out:  dest_len:
 *                 in: the length of dest buffer
 *                 out: the real length of dest
 * return: see km error code
 * */
uint32_t km_asym_decrypt(const char *name, const uint32_t name_len, void *enc_params,
                const uint8_t *src, const size_t src_len,
               uint8_t *dest, size_t *dest_len);

/*
 * km symmetric cipher
 * param: in:      name: key name
 * param: in:      name_len: key name len
 * param: in:      cipher_params: the params to symmetric cipher
 * param: in:      iv: the iv for symmetric cipher
 * param: in:      iv_len: the length of input iv
 * param: in:      src: the source data to cipher
 * param: in:      src_len: the length for src
 * param: in:      dest: the out buffet
 * param: in_out:  dest_len:
 *                 in: the length of dest buffer
 *                 out: the real length of dest
 * return: see km error code
 * */

uint32_t km_cipher(const char *name, const uint32_t name_len, km_sym_param *cipher_params,
        const uint8_t *iv, const uint32_t iv_len, uint8_t *src, size_t src_len,
        uint8_t *dest, size_t *dest_len);
/*
 * to show km version and new file for no rsvd part platform
 * return: see km error code
 * */

uint32_t km_init();

/*
 * to clean resource
 * */

void km_cleanup();

/*
 * to get id2 id
 * param: out: the out buffer
 * param: in_out:
 *        in: length of id2
 *        out: the real length of id2
 * return: see km error code
 * */
uint32_t km_get_id2(uint8_t *id2, uint32_t *len);
/*
 * to set id2 id
 * param: in: the id buffer
 * param: in: the id len
 * return: see km error code
 * */
uint32_t km_set_id2(uint8_t *id2, uint32_t len);


#ifdef __cplusplus
}
#endif

#endif /* _KM_H_ */

