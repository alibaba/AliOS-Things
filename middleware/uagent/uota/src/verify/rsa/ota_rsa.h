#ifndef _OTA_RSA_H_
#define _OTA_RSA_H_
#include "ota_bignum.h"
#include "ota_log.h"

typedef struct
{
    int ver;                    /*!<  always 0          */
    size_t len;                 /*!<  size(N) in chars  */

    ota_mpi N;                      /*!<  public modulus    */
    ota_mpi E;                      /*!<  public exponent   */

    ota_mpi D;                      /*!<  private exponent  */
    ota_mpi P;                      /*!<  1st prime factor  */
    ota_mpi Q;                      /*!<  2nd prime factor  */
    ota_mpi DP;                     /*!<  D % (P - 1)       */
    ota_mpi DQ;                     /*!<  D % (Q - 1)       */
    ota_mpi QP;                     /*!<  1 / (Q % P)       */

    ota_mpi RN;                     /*!<  cached R^2 mod N  */
    ota_mpi RP;                     /*!<  cached R^2 mod P  */
    ota_mpi RQ;                     /*!<  cached R^2 mod Q  */

    ota_mpi Vi;                     /*!<  cached blinding value     */
    ota_mpi Vf;                     /*!<  cached un-blinding value  */

    int padding;                /*!<  MBEDTLS_RSA_PKCS_V15 for 1.5 padding and
                                      MBEDTLS_RSA_PKCS_v21 for OAEP/PSS         */
    int hash_id;                /*!<  Hash identifier of mbedtls_md_type_t as
                                      specified in the mbedtls_md.h header file
                                      for the EME-OAEP and EMSA-PSS
                                      encoding                          */
#if defined(MBEDTLS_THREADING_C)
    //mbedtls_threading_mutex_t mutex;    /*!<  Thread-safety mutex       */
#endif
}
ota_rsa_context;


#define OTA_GO_RET(_ret, _f, _a ...) do {               \
    OTA_LOG_E(_f, ##_a);                           \
    result =(ota_crypto_result)_ret;                \
    goto _OUT;                                      \
} while (0);

#define INIT_CTX_MAGIC(m)         (m = 0x12345678)
#define IS_VALID_CTX_MAGIC(m)     (0x12345678 == m)
#define CLEAN_CTX_MAGIC(m)         (m = 0x0)


#define OTA_ERR_RSA_BAD_INPUT_DATA                    -0x4080  
#define OTA_ERR_RSA_INVALID_PADDING                   -0x4100  
#define OTA_ERR_RSA_KEY_GEN_FAILED                    -0x4180 
#define OTA_ERR_RSA_KEY_CHECK_FAILED                  -0x4200 
#define OTA_ERR_RSA_PUBLIC_FAILED                     -0x4280  
#define OTA_ERR_RSA_PRIVATE_FAILED                    -0x4300 
#define OTA_ERR_RSA_VERIFY_FAILED                     -0x4380 
#define OTA_ERR_RSA_OUTPUT_TOO_LARGE                  -0x4400  
#define OTA_ERR_RSA_RNG_FAILED                        -0x4480  

#define OTA_RSA_PUBLIC      0
#define OTA_RSA_PRIVATE     1

#define OTA_RSA_PKCS_V15    0
#define OTA_RSA_PKCS_V21    1

#define OTA_RSA_SIGN        1
#define OTA_RSA_CRYPT       2

enum {
    PK_PUBLIC=0,
    PK_PRIVATE=1
};

typedef enum {
    OTA_MD_NONE=0,
    OTA_MD_MD2,
    OTA_MD_MD4,
    OTA_MD_MD5,
    OTA_MD_SHA1,
    OTA_MD_SHA224,
    OTA_MD_SHA256,
    OTA_MD_SHA384,
    OTA_MD_SHA512,
    OTA_MD_RIPEMD160,
} ota_md_type_t;


struct ota_md_info_t
{
    /** Digest identifier */
    ota_md_type_t type;

    /** Name of the message digest */
    const char * name;

    /** Output length of the digest function in bytes */
    int size;

    /** Block length of the digest function in bytes */
    int block_size;

    /** Digest initialisation function */
    void (*starts_func)( void *ctx );

    /** Digest update function */
    void (*update_func)( void *ctx, const unsigned char *input, size_t ilen );

    /** Digest finalisation function */
    void (*finish_func)( void *ctx, unsigned char *output );

    /** Generic digest function */
    void (*digest_func)( const unsigned char *input, size_t ilen,
                         unsigned char *output );

    /** Allocate a new context */
    void * (*ctx_alloc_func)( void );

    /** Free the given context */
    void (*ctx_free_func)( void *ctx );

    /** Clone state from a context */
    void (*clone_func)( void *dst, const void *src );

    /** Internal use only */
    void (*process_func)( void *ctx, const unsigned char *input );
};

void ota_rsa_set_padding( ota_rsa_context *ctx, int padding, int hash_id );
int ota_rsa_rsassa_pkcs1_v15_verify( ota_rsa_context *ctx,
                                 int (*f_rng)(void *, unsigned char *, unsigned int),
                                 void *p_rng,
                                 int mode,
                                 ota_md_type_t md_alg,
                                 unsigned int hashlen,
                                 const unsigned char *hash,
                                 const unsigned char *sig );

#endif

