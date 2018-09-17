#ifndef OTA_RSA_PARAM_H_
#define OTA_RSA_PARAM_H_
#include "stdio.h"
#include "stdint.h"

#ifndef false
#define false 0
#endif

#ifndef true
#define true 1
#endif

#ifndef bool
#define bool char
#endif


typedef enum __hash_type_t {
    HASH_NONE   = 0,
    SHA1        = 1,
    SHA224      = 2,
    SHA256      = 3,
    SHA384      = 4,
    SHA512      = 5,
    MD5         = 6,
} ota_hash_type_t;

/*
typedef struct
{
    ota_hash_type_t hash_method;
    int         ctx_size;
    void *      ctx_hash;
} ota_hash_ctx_params;
*/

typedef enum _ota_crypto_result {
    OTA_CRYPTO_ERROR = (int)0xffff0000, 
    OTA_CRYPTO_NOSUPPORT,               
    OTA_CRYPTO_INVALID_KEY,             
    OTA_CRYPTO_INVALID_TYPE,            
    OTA_CRYPTO_INVALID_CONTEXT,         
    OTA_CRYPTO_INVALID_PADDING,         
    OTA_CRYPTO_INVALID_AUTHENTICATION,  
    OTA_CRYPTO_INVALID_ARG,             
    OTA_CRYPTO_INVALID_PACKET,          
    OTA_CRYPTO_LENGTH_ERR,              
    OTA_CRYPTO_OUTOFMEM,                
    OTA_CRYPTO_SHORT_BUFFER,            
    OTA_CRYPTO_NULL,                    
    OTA_CRYPTO_ERR_STATE,               
    OTA_CRYPTO_SUCCESS = 0,            
} ota_crypto_result;


typedef enum _rsa_pad_type_t {
    RSA_NOPAD               = 0,

    /* encrypt */
    RSAES_PKCS1_V1_5        = 10,
    RSAES_PKCS1_OAEP_MGF1   = 11,

    /* sign */
    RSASSA_PKCS1_V1_5       = 20,
    RSASSA_PKCS1_PSS_MGF1   = 21,
} ota_rsa_pad_type_t;

typedef struct _rsa_padding_t {
    ota_rsa_pad_type_t type;
    union {
        struct {
            ota_hash_type_t type;
        } rsaes_oaep;
        struct {
            ota_hash_type_t type;   
        } rsassa_v1_5;
        struct {
            ota_hash_type_t type;   
            unsigned int salt_len;
        } rsassa_pss;
    } pad;
} ota_rsa_padding_t;

#define TEE_MIN_RSA_KEY_SIZE      (256)
#define TEE_MAX_RSA_KEY_SIZE      (2048)

typedef struct _ota_rsa_pubkey_t {
    uint32_t magic;
    size_t n_size;
    size_t e_size;
    uint8_t n[(TEE_MAX_RSA_KEY_SIZE >> 3)];
    uint8_t e[(TEE_MAX_RSA_KEY_SIZE >> 3)];
} ota_rsa_pubkey_t;


ota_crypto_result ota_rsa_get_pubkey_size(unsigned int keybits, unsigned int *size);
ota_crypto_result ota_rsa_init_pubkey(unsigned int keybits,
                      const unsigned char *n, unsigned int n_size,
                      const unsigned char *e, unsigned int e_size,
                      ota_rsa_pubkey_t *pubkey);
ota_crypto_result ota_rsa_verify(const ota_rsa_pubkey_t *pub_key,
                      const uint8_t *dig, size_t dig_size,
                      const uint8_t *sig, size_t sig_size,
                      ota_rsa_padding_t padding, bool *p_result);

#endif
