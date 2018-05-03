#ifndef __AES_H
#define __AES_H

#if defined(__OV_788__)
#else
#include <string.h>
#endif
#include <stdint.h>

#ifdef __LINUX_UB2__
#include <unistd.h>
#endif

#define LITTLE_ENDIAN_ORDER

enum {
    JLAES_ENC_TYPE   = 1,   /* cipher unique type */
    JLAES_ENCRYPTION = 0,
    JLAES_DECRYPTION = 1,
    JLAES_BLOCK_SIZE = 16
};

enum{
	JLCYASSL_WORD_SIZE  = sizeof(uint32_t)
};

#define ALIGN16

#ifndef NULL
#define NULL 0
#endif

#define JOYLINK_ENC2_ENCRYPT     1
#define JOYLINK_ENC2_DECRYPT     0

enum
{
    JOYLINK_SUCCESS                 = 0,
    JOYLINK_BUFFER_SPACE_ERR        = -100,             /*buffer????*/
    JOYLINK_RECV_LEN_ERR            = -101,             /*???????ݳ???????*/
    JOYLINK_CHECKSUM_ERR            = -102,             /*????У??ʧ??*/
    JOYLINK_GET_CMD_ERR             = -103,             /*????????????????*/
    JOYLINK_GET_DEVID_ERR           = -104,             /*?豸ID??һ??*/
    JOYLINK_DEVID_ERR               = -105,             /*?豸ID????*/
    JOYLINK_TOKEN_ERR               = -106,             /*?豸TOKEN????*/
    JOYLINK_ENCTYPE_ERR             = -107,             /*??֧?ֵļ??ܲ???*/
    JOYLINK_MAGIC_ERR               = -108,             /*??Ч???ݰ?,magic????*/
    JOYLINK_ENCINFO_ERR             = -109,             /*??????Ϣ????*/
    
    
    JOYLINK_PARAM_INVALID           = -1000,            /*????????????*/
    JOYLINK_SYSTEM_ERR              = -1001,            /*ϵͳ???ô???,???紴??socketʧ??*/
    JOYLINK_NETWORK_TIMEOUT         = -1002,            /*???糬ʱ*/
    JOYLINK_RECV_DATA_ERR           = -1003,             /*???յ???????????*/
    JOYLINK_CANCEL_ERR              = -1004,            /*?û?ȡ??????*/
};

typedef struct JLAes {
    /* AESNI needs key first, rounds 2nd, not sure why yet */
    ALIGN16 uint32_t key[60];
    uint32_t  rounds;

    ALIGN16 uint32_t reg[JLAES_BLOCK_SIZE / sizeof(uint32_t)];      /* for CBC mode */
    ALIGN16 uint32_t tmp[JLAES_BLOCK_SIZE / sizeof(uint32_t)];      /* same         */
} JLAes;

enum {
    JLMAX_ERROR_SZ       =  80,   /* max size of error string */
    JLMAX_CODE_E         = -100,  /* errors -101 - -199 */
    JLOPEN_RAN_E         = -101,  /* opening random device error */
    JLREAD_RAN_E         = -102,  /* reading random device error */
    JLWINCRYPT_E         = -103,  /* windows crypt init error */
    JLCRYPTGEN_E         = -104,  /* windows crypt generation error */
    JLRAN_BLOCK_E        = -105,  /* reading random device would block */

    JLMP_INIT_E          = -110,  /* mp_init error state */
    JLMP_READ_E          = -111,  /* mp_read error state */
    JLMP_EXPTMOD_E       = -112,  /* mp_exptmod error state */
    JLMP_TO_E            = -113,  /* mp_to_xxx error state, can't convert */
    JLMP_SUB_E           = -114,  /* mp_sub error state, can't subtract */
    JLMP_ADD_E           = -115,  /* mp_add error state, can't add */
    JLMP_MUL_E           = -116,  /* mp_mul error state, can't multiply */
    JLMP_MULMOD_E        = -117,  /* mp_mulmod error state, can't multiply mod */
    JLMP_MOD_E           = -118,  /* mp_mod error state, can't mod */
    JLMP_INVMOD_E        = -119,  /* mp_invmod error state, can't inv mod */
    JLMP_CMP_E           = -120,  /* mp_cmp error state */
    JLMP_ZERO_E          = -121,  /* got a mp zero result, not expected */

    JLMEMORY_E           = -125,  /* out of memory error */

    JLRSA_WRONG_TYPE_E   = -130,  /* RSA wrong block type for RSA function */
    JLRSA_BUFFER_E       = -131,  /* RSA buffer error, output too small or 
                                   input too large */
    JLBUFFER_E           = -132,  /* output buffer too small or input too large */
    JLALGO_ID_E          = -133,  /* setting algo id error */
    JLPUBLIC_KEY_E       = -134,  /* setting public key error */
    JLDATE_E             = -135,  /* setting date validity error */
    JLSUBJECT_E          = -136,  /* setting subject name error */
    JLISSUER_E           = -137,  /* setting issuer  name error */
    JLCA_TRUE_E          = -138,  /* setting CA basic constraint true error */
    JLEXTENSIONS_E       = -139,  /* setting extensions error */

    JLASN_PARSE_E        = -140,  /* ASN parsing error, invalid input */
    JLASN_VERSION_E      = -141,  /* ASN version error, invalid number */
    JLASN_GETINT_E       = -142,  /* ASN get big int error, invalid data */
    JLASN_RSA_KEY_E      = -143,  /* ASN key init error, invalid input */
    JLASN_OBJECT_ID_E    = -144,  /* ASN object id error, invalid id */
    JLASN_TAG_NULL_E     = -145,  /* ASN tag error, not null */
    JLASN_EXPECT_0_E     = -146,  /* ASN expect error, not zero */
    JLASN_BITSTR_E       = -147,  /* ASN bit string error, wrong id */
    JLASN_UNKNOWN_OID_E  = -148,  /* ASN oid error, unknown sum id */
    JLASN_DATE_SZ_E      = -149,  /* ASN date error, bad size */
    JLASN_BEFORE_DATE_E  = -150,  /* ASN date error, current date before */
    JLASN_AFTER_DATE_E   = -151,  /* ASN date error, current date after */
    JLASN_SIG_OID_E      = -152,  /* ASN signature error, mismatched oid */
    JLASN_TIME_E         = -153,  /* ASN time error, unknown time type */
    JLASN_INPUT_E        = -154,  /* ASN input error, not enough data */
    JLASN_SIG_CONFIRM_E  = -155,  /* ASN sig error, confirm failure */
    JLASN_SIG_HASH_E     = -156,  /* ASN sig error, unsupported hash type */
    JLASN_SIG_KEY_E      = -157,  /* ASN sig error, unsupported key type */
    JLASN_DH_KEY_E       = -158,  /* ASN key init error, invalid input */
    JLASN_NTRU_KEY_E     = -159,  /* ASN ntru key decode error, invalid input */

    JLECC_BAD_ARG_E      = -170,  /* ECC input argument of wrong type */
    JLASN_ECC_KEY_E      = -171,  /* ASN ECC bad input */
    JLECC_CURVE_OID_E    = -172,  /* Unsupported ECC OID curve type */
    JLBAD_FUNC_ARG       = -173,  /* Bad function argument provided */
    JLNOT_COMPILED_IN    = -174,  /* Feature not compiled in */
    JLUNICODE_SIZE_E     = -175,  /* Unicode password too big */
    JLNO_PASSWORD        = -176,  /* no password provided by user */
    JLALT_NAME_E         = -177,  /* alt name size problem, too big */

    JLAES_GCM_AUTH_E     = -180,  /* AES-GCM Authentication check failure */
    JLAES_CCM_AUTH_E     = -181,  /* AES-CCM Authentication check failure */

    JLCAVIUM_INIT_E      = -182,  /* Cavium Init type error */

    JLCOMPRESS_INIT_E    = -183,  /* Compress init error */
    JLCOMPRESS_E         = -184,  /* Compress error */
    JLDECOMPRESS_INIT_E  = -185,  /* DeCompress init error */
    JLDECOMPRESS_E       = -186,  /* DeCompress error */

    JLBAD_ALIGN_E        = -187,  /* Bad alignment for operation, no alloc */

    JLMIN_CODE_E         = -200   /* errors -101 - -199 */
};


int JLdevice_aes_encrypt(const uint8_t * key, int keyLength, 
        const uint8_t * iv, const uint8_t *pPlainIn, int plainLength, 
        uint8_t *pEncOut, int maxOutLen);

int JLdevice_aes_decrypt(const uint8_t * key, int keyLength, const uint8_t * iv, const uint8_t *pEncIn, int encLength, uint8_t *pPlainOut, int maxOutLen);

#endif


