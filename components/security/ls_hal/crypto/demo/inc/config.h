/**
* Copyright (C) 2017  Alibaba Group Holding Limited.
**/

#ifndef ALI_ALGO_CONFIG_H
#define ALI_ALGO_CONFIG_H

/* AES */
#define ALI_ALGO_AES_C

/* RSA */
#define ALI_ALGO_RSA_C
#define ALI_ALGO_GENPRIME // use prime table
#define ALI_ALGO_RSA_NO_CRT
#define ALI_ALGO_PK_PARSE_C
#define ALI_ALGO_CIPHER_C

#ifndef PLATFORM_ANDROID
#define ALI_ALGO_HAVE_ASM
#endif // PLATFORM_ANDROID

/* HASH */
#define ALI_ALGO_MD_C

/* SM3 */
#ifdef ALI_CRYPTO_HASH_SM3
#define ALI_ALGO_SM3_C
#endif // ALI_CRYPTO_HASH_SM3

/* SM2 */
#ifdef ALI_CRYPTO_SM2
#define ALI_ALGO_ECP_C
#define ECP_SHORTWEIERSTRASS
//#define ALI_ALGO_ECP_DP_SMP256R1_ENABLED
#define ALI_ALGO_ECP_DP_SMP256R2_ENABLED
#define ALI_ALGO_SM2_C
#define ALI_ALGO_SM2_SIGN_C
#define ALI_ALGO_SM2_ENCRYPT_C
#define ALI_ALGO_SM2_DH_C
#endif // ALI_CRYPTO_SM2

/* SM4 */
#ifdef ALI_CRYPTO_SM4
#define ALI_ALGO_SM4_C
#ifdef ALI_CRYPTO_SM4_CBC
#endif // ALI_CRYPTO_SM4_CBC
#endif // ALI_CRYPTO_SM4

#endif /* ALI_ALGO_CONFIG_H */
