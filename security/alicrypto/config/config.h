/**
* Copyright (C) 2017  Alibaba Group Holding Limited.
**/


#ifndef ALI_ALGO_CONFIG_H
#define ALI_ALGO_CONFIG_H

/* ali crypto define*/
/* AES */
#define ALI_CRYPTO_AES
#define ALI_CRYPTO_AES_ECB
#define ALI_CRYPTO_AES_CBC
#define ALI_CRYPTO_AES_CTR
#define ALI_CRYPTO_AES_CFB
/* HASH */
#define ALI_CRYPTO_HASH
#define ALI_CRYPTO_HASH_MD5
#define ALI_CRYPTO_HASH_SHA1
#define ALI_CRYPTO_HASH_SHA256
//#define ALI_CRYPTO_HASH_SM3
/**/
#define ALI_CRYPTO_HMAC

/* RSA */
#define ALI_CRYPTO_RSA

/* SM2 */
//#define ALI_CRYPTO_SM2

/* SM4 */
//#define ALI_CRYPTO_SM4
//#define ALI_CRYPTO_SM4_ECB
//#define ALI_CRYPTO_SM4_CBC

/* random number */
// #define ALI_CRYPTO_RAND

/* OEM configure */
//#define ALI_CRYPTO_PLAT_AOS
//#define ALI_CRYPTO_PLATFORM_C

/********************************************************************
* define for alicrypto_algo
********************************************************************/

/* RSA */
#ifdef ALI_CRYPTO_RSA
#define ALI_ALGO_RSA_C
#define ALI_ALGO_OID_C
#define ALI_ALGO_PKCS1_V21
#define ALI_ALGO_PKCS1_V15

#define ALI_ALGO_BIGNUM_C
#define ALI_ALGO_GENPRIME
#define ALI_ALGO_RSA_NO_CRT
#define ALI_ALGO_PK_PARSE_C

#define ALI_ALGO_ASN1_PARSE_C
#define ALI_ALGO_CIPHER_C

#ifndef PLATFORM_ANDROID
#define ALI_ALGO_HAVE_ASM
#endif

#endif

/* HASH */
#ifdef ALI_CRYPTO_HASH
#ifdef ALI_CRYPTO_HASH_MD5
#define ALI_ALGO_MD_C
#define ALI_ALGO_MD5_C
#endif
#ifdef ALI_CRYPTO_HASH_SHA1
#define ALI_ALGO_SHA1_C
#endif
#ifdef ALI_CRYPTO_HASH_SHA256
#define ALI_ALGO_SHA256_C
#endif
#ifdef ALI_CRYPTO_HASH_SM3
#define ALI_ALGO_SM3_C
#endif
#endif
/* AES */
#ifdef ALI_CRYPTO_AES
#define ALI_ALGO_AES_C
#ifdef ALI_CRYPTO_AES_CTR
#define ALI_ALGO_CIPHER_MODE_CTR
#endif
#ifdef ALI_CRYPTO_AES_CBC
#define ALI_ALGO_CIPHER_MODE_CBC
#endif
#ifdef ALI_CRYPTO_AES_CFB
#define ALI_ALGO_CIPHER_MODE_CFB
#endif
#endif

/*SM2*/
#ifdef ALI_CRYPTO_SM2
#define ALI_ALGO_ECP_C
#define ECP_SHORTWEIERSTRASS
//#define ALI_ALGO_ECP_DP_SMP256R1_ENABLED
#define ALI_ALGO_ECP_DP_SMP256R2_ENABLED
#define ALI_ALGO_SM2_C
#define ALI_ALGO_SM2_SIGN_C
#define ALI_ALGO_SM2_ENCRYPT_C
#define ALI_ALGO_SM2_DH_C
#endif

/* SM4 */
#ifdef ALI_CRYPTO_SM4
#define ALI_ALGO_SM4_C
#ifdef ALI_CRYPTO_SM4_CBC
#define ALI_ALGO_CIPHER_MODE_CBC
#endif
#endif

#endif /* ALI_ALGO_CONFIG_H */
