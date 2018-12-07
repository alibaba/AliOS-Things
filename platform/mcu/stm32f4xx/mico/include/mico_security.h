/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#pragma once

#include "common.h"
#include "Curve25519/curve25519-donna.h"
#include "SHAUtils/sha.h"
#include "stdint.h"
#include "CheckSumUtils.h"

/** \defgroup MICO_Algorithm_APIs MICO Algorithm APIs
  * @brief Provide commonly used Algorithm APIs
  * @{
  */

/** @addtogroup MICO_Algorithm_APIs
  * @{
  */

/** @defgroup MICO_Security_MD5 MICO MD5 Encryption
  * @brief    MiCO MD5 Encryption Function
  * @{
  */

typedef unsigned char         byte;
typedef unsigned short      word16;
typedef unsigned int        word32;
typedef unsigned long long  word64;

/** 
 *  @brief  Constant data during MD5 Encryption
 */
enum {
  MD5             =  0,      /** hash type unique */
  MD5_BLOCK_SIZE  = 64,
  MD5_DIGEST_SIZE = 16,
  MD5_PAD_SIZE    = 56
};

/** 
 *  @brief  MD5 Context definition
 */
typedef struct {
  uint32_t state[4];        /** state (ABCD) */
  uint32_t count[2];        /** number of bits, modulo 2^64 (lsb first) */
  uint32_t buffer[64];      /** input buffer */
} md5_context;


/**
 * @brief          MD5 context setup
 *
 * @param ctx      context to be initialized
 */
void InitMd5(md5_context *ctx);


/**
 * @brief          MD5 process buffer
 *
 * @param ctx      MD5 context
 * @param input    buffer holding the  data
 * @param ilen     length of the input data
 */
void Md5Update(md5_context *ctx, unsigned char *input, int ilen);


/**
 * @brief          MD5 final digest
 *
 * @param ctx      MD5 context
 * @param output   MD5 checksum result
 */
void Md5Final(md5_context *ctx, unsigned char output[16]);

/**
  * @}
  */


/** @defgroup MICO_Security_HAMC_MD5 MICO HMAC_MD5 Encryption 
  * @brief    MiCO HMAC_MD5 Encryption Function
  * @{
  */

/** 
 *  @brief  Constant data during HAMC_MD5 Encryption
 */
enum{
  HMAC_BLOCK_SIZE = 64,
  INNER_HASH_SIZE = 128, 
};

/** 
 *  @brief  Hash union during HMAC_MD5 Encryption
 */
typedef union {
  md5_context md5;
} Hash;

/** 
 *  @brief  Hmac digest during HMAC_MD5 Encription 
 */
typedef struct Hmac {
  Hash    hash;
  word32  ipad[HMAC_BLOCK_SIZE  / sizeof(word32)];  /* same block size all*/
  word32  opad[HMAC_BLOCK_SIZE  / sizeof(word32)];
  word32  innerHash[INNER_HASH_SIZE / sizeof(word32)]; /* max size */
  byte    macType;                                     /* md5  */
  byte    innerHashKeyed;                              /* keyed flag */
} Hmac;

/**
 * @brief          HMAC_MD5 extend external key into a longer bit string
 *
 * @param hmac     Hmac context
 * @param type     hash type(only include MD5)
 * @param key      key
 * @param keysz    length of key
 * @retval         None
 */
void HmacSetKey(Hmac* hmac, int type, const byte* key, word32 keySz);
  

  /**
 * @brief          HMAC_MD5 process buffer
 *
 * @param Hmac     hmac context
 * @param byte*    buffer holding the input data
 * @param inLen    length of the input data.   
 *                 Note: The length is arbitrary.
 * @retval         None
 */
void HmacUpdate(Hmac* hmac, const byte* input, word32 inLen);
  

/**
 * @brief          HMAC Final Digest
 *
 * @param hmac     Hmac context
 * @param output    buffer holding the output data
 * @retval         None
 */   
void HmacFinal(Hmac* hmac, byte* output);

/**
  * @}
  */


/** @defgroup MICO_Security_DES MICO DES Encryption Algorithm
  * @brief  MiCO DES Encryption and Decryption Function
  * @{
  */


/** 
 *  @brief  Constant data during DES Encryption
 */
enum {
  DES_ENC_TYPE    = 2,     /* cipher unique type */
  DES3_ENC_TYPE   = 3,     /* cipher unique type */
  DES_BLOCK_SIZE  = 8,
  DES_KS_SIZE     = 32,
  DES_ENCRYPTION  = 0,
  DES_DECRYPTION  = 1
};
/** 
 *  @brief  Des context 
 */
typedef struct Des {
  word32 key[DES_KS_SIZE];
  word32 reg[DES_BLOCK_SIZE / sizeof(word32)];      /* for CBC mode */
  word32 tmp[DES_BLOCK_SIZE / sizeof(word32)];      /* same         */
} Des;

/** 
 *  @brief  Des3 context 
 */
typedef struct Des3 {
  word32 key[3][DES_KS_SIZE];
  word32 reg[DES_BLOCK_SIZE / sizeof(word32)];      /* for CBC mode */
  word32 tmp[DES_BLOCK_SIZE / sizeof(word32)];      /* same         */
} Des3;

/**
 * @brief          DES extend external key into a longer bit string
 *
 * @param des      Des context
 * @param key      external key defined by user,the length must be 64 bits
 * @param iv       extension key defined by user,must be the same size with key
 * @param dir      Specify the process of encryption or decryption
 * @retval         None
 */
void Des_SetKey(Des* des, const byte* key, const byte* iv, int dir);


/**
 * @brief          DES encryption process in Cbc mode
 *
 * @param des      Des context
 * @param output   buffer to hold output - Cipher Text. It has the same size with input
 * @param input    buffer to hold input  - Plain  Text. 
 *                 Note: The size of input must be the multiple of DES_BLOCK_SIZE 64 bits
 * @param sz       size of input
 *                 Note: input and output have the same size.
 * @retval         None
 */



/**
 * @brief          DES decryption process in Cbc mode
 *
 * @param des      Des context
 * @param output   buffer to hold output - Plain  Text.
 * @param input    buffer to hold input  - Cipher Text. 
 *                 Note: The size of input - Cipher Text must be with 2^64 bits
 *                        And it must be the multiple of DES_BLOCK_SIZE 8 bytes
 * @param sz       size of input
 *                 Note: input and output have the same size.
 * @retval         None
 */
void Des_CbcDecrypt(Des* des, byte* output, const byte* input, word32 sz);


/**
 * @brief          DES3 extend external key into a longer bit string
 *
 * @param des      Des3 context
 * @param key      external key defined by user,the length must be 192 bits��24 bytes��
 * @param iv       extension key defined by user,must be the same size with key
 * @param dir      Specify the process of encryption or decryption
 * @retval         None 
 */
void Des3_SetKey(Des3* des, const byte* key, const byte* iv,int dir);


/**
 * @brief          DES3 encryption process in Cbc mode
 *
 * @param des      Des3 context
 * @param output   buffer to hold output - Cipher Text. It has the same size with input
 * @param input    buffer to hold input  - Plain  Text. 
 *                 Note: The size of input must be the multiple of DES_BLOCK_SIZE 64 bits
 * @param sz       size of input
 *                 Note: input and output have the same size.
 * @retval         None
 */
void Des3_CbcEncrypt(Des3* des, byte* output, const byte* input,word32 sz);


/**
 * @brief          DES3 decryption process in Cbc mode
 *
 * @param des      Des3 context
 * @param output   buffer to hold output - Plain  Text.
 * @param input    buffer to hold input  - Cipher Text. 
 *                 Note: The size of input - Cipher Text must be with 2^64 bits
 *                        And it must be the multiple of DES_BLOCK_SIZE 8 bytes
 * @param sz       size of input
 *                 Note: input and output have the same size.
 * @retval         None
 */
void Des3_CbcDecrypt(Des3* des, byte* output, const byte* input,word32 sz);
/**
  * @}
  */


/** @defgroup MICO_Security_AES MICO AES Encryption Algorytm
  * @brief    MiCO AES Encryption and Decryption Function

  * @{
  */
/** 
 *  @brief    Constant data during AES Encription 
 */
enum {
  AES_ENC_TYPE   = 1,   /* cipher unique type */
  AES_ENCRYPTION = 0,
  AES_DECRYPTION = 1,
  AES_BLOCK_SIZE = 16
};

/** 
 *  @brief    Aes context 
 */
typedef struct Aes {
    /* AESNI needs key first, rounds 2nd, not sure why yet */
  word32 key[60];
  word32  rounds;
  word32 reg[AES_BLOCK_SIZE / sizeof(word32)];      
  word32 tmp[AES_BLOCK_SIZE / sizeof(word32)];
} Aes;

/**
 * @brief          AES extend external key into a longer bit string
 *
 * @param aes      AES context
 * @param key      external key defined by user,must be 128 bits
 * @param ilen      size  of AES_BLOCK_SIZE
 * @param iv       extension key defined by user,must be the same size with key
 * @param dir      Specify the process of encryption or decryption
 * @retval         kNoErr is returned on success, otherwise, kXXXErr is returned.
 */
int  AesSetKey(Aes* aes, const byte* key, word32 ilen, const byte* iv,int dir);


/**
 * @brief          AES encryption process in Cbc mode
 *
 * @param aes      AES context
 * @param output   buffer to hold output - Cipher Text.
 * @param input    buffer to hold input  - Plain  Text. 
 *                 Note: The size of input - Plain Text must be the multiple of AES_BLOCK_SIZE 128 bits
 * @param sz       size of input
 *                 Note: input and output have the same size.
 * @retval         kNoErr is returned on success, otherwise, kXXXErr is returned.
 */
int  AesCbcEncrypt(Aes* aes, byte* output, const byte* input, word32 sz);


/**
 * @brief          AES decryption process in Cbc mode
 *
 * @param aes      AES context
 * @param output   buffer to hold output - Plain  Text. It has the same size with input
 * @param input    buffer to hold input - Cipher Text.
 *                 Note: The size of input must be the multiple of AES_BLOCK_SIZE 128 bits
 * @param sz       size of input
 *                 Note: input and output have the same size.
 * @retval         kNoErr is returned on success, otherwise, kXXXErr is returned.
 */
int  AesCbcDecrypt(Aes* aes, byte* output, const byte* input, word32 sz);


/**
* @brief          AES encryption process in Cbc mode with PKCS#5 padding
 *
 * @param aes      AES context
 * @param output   buffer to hold output - Cipher Text.
 * @param input    buffer to hold input  - Plain  Text. 
 * @param sz       size of input
 * @retval         the Cipher Text size, should be the multiple of AES_BLOCK_SIZE 128 bits 
 */
word32 AesCbcEncryptPkcs5Padding(Aes* aes, byte* output, const byte* input, word32 sz);


/**
* @brief          AES decryption process in Cbc mode with PKCS#5 padding
 *
 * @param aes      AES context
 * @param output   buffer to hold output - Plain Text.
 * @param input    buffer to hold input  - Cipher  Text. 
 * @param sz       size of input, must be the multiple of AES_BLOCK_SIZE 128 bits 
 * @retval         the Plain Text size
 */
word32 AesCbcDecryptPkcs5Padding(Aes* aes, byte* output, const byte* input, word32 sz);


/**
 * @brief          AES extend external key into a longer bit string
 *
 * @param aes      AES context
 * @param key      external key defined by user,must be 128 bits
 * @param ilen     size  of AES_BLOCK_SIZE.
 * @param out      buffer to put cipher text.
 * @param dir      Specify the process of encryption or decryption
 * @retval         kNoErr is returned on success, otherwise, kXXXErr is returned.
 */
int  AesSetKeyDirect(Aes* aes, const byte* key, word32 ilen,const byte* out, int dir);
               

/**
 * @brief          AES encryption process in ecb mode
 *
 * @param aes      AES context
 * @param output   buffer to hold output - Plain  Text. It has the same size with input
 * @param input    buffer to hold input - Cipher Text.
 *                 Note: The size of input must be the multiple of AES_BLOCK_SIZE 128 bits
 * @param sz       size of input
 *                 Note: input and output have the same size.
 * @retval         None.
 */
void AesEncryptDirect(Aes* aes, byte* out, const byte* in);


/**
 * @brief          AES decryption process in ecb mode
 *
 * @param aes      AES context
 * @param output   buffer to hold output - Plain  Text. It has the same size with input
 * @param input    buffer to hold input - Cipher Text.
 *                 Note: The size of input must be the multiple of AES_BLOCK_SIZE 128 bits
 * @param sz       size of input
 *                 Note: input and output have the same size.
 * @retval         None.
 */
void AesDecryptDirect(Aes* aes, byte* out, const byte* in);
/**
  * @}
  */


/** @defgroup MICO_Security_ARC4 MICO ARC4 Encryption Algorytm
  * @brief    MiCO ARC4 Encryption and Decryption Fucntion
  * @{
  */

/** 
 *  @brief    Constant data during Arc4 Encription 
 */
enum {
  ARC4_ENC_TYPE   = 4,    /* cipher unique type */
  ARC4_STATE_SIZE = 256
};
/** 
 *  @brief    Arc4 context
 */
typedef struct Arc4 {
  byte x;
  byte y;
  byte state[ARC4_STATE_SIZE];
} Arc4;


/**
 * @brief          ARC4 extend external key
 *
 * @param arc4     ARC4 context
 * @param key      external key defined by user
 * @param keylen   length of the key, range is 1-256 bits.
 *                 Note: 0 is illegal.
 * @retval         None.
 */
void Arc4SetKey(Arc4 *arc4, const byte *key, word32 keylen );


/**
 * @brief          ARC4 encryption or decription process
 *
 * @param arc4     ARC4 context
 * @param output   buffer to hold output - Cipher Text.It has the same size with input
 * @param input    buffer to hold input - Plain  Text. 
 *                 Note: The length must be within 1 to 20 bytes.
 * @param outlen   size of output.      
 * @retval         None.
 */
void Arc4Process(Arc4 *arc4, byte *output, const byte *input, word32 outlen);
/**
  * @}
  */


/** @defgroup MICO_Security_Rabbit MICO Rabbit Encryption Algorithm
  * @brief  MiCO Rabbit Encryption and Decryption Function
  * @{
  */

/** 
 *  @brief    Constant data during Rabbit Encryption
 */
enum {
  RABBIT_ENC_TYPE  = 5     /* cipher unique type */
};


/** 
 *  @brief    Rabbit Context
 */
typedef struct RabbitCtx {
  word32 x[8];
  word32 c[8];
  word32 carry;
} RabbitCtx;
    
/** 
 *  @brief    Rabbit stream cipher
 */
typedef struct Rabbit {
    RabbitCtx masterCtx;
    RabbitCtx workCtx;
} Rabbit;


/**
 * @brief          Rabbit extend external key into a longer bit string
 *
 * @param rabbit   Rabbit context
 * @param key      Key Seed defined by user, but length must be 128 bits
 * @param iv       initial vector - IV, but length must be 64 bits
 */
int RabbitSetKey(Rabbit* rabbit, const byte* key, const byte* iv);


/**
 * @brief          Rabbit encryption or decryption process
 *
 * @param rabbit   Rabbit context
 * @param output   buffer to hold output - Cipher Text.
 *                 Note: The size of output is the same with input.
 * @param input    buffer to hold input. 
 *                 Note: The size must be within 1 to 16bytes
 * @param sz       size of output.     
 * @retval         kNoErr is returned on success, otherwise, kXXXErr is returned.
 */
int RabbitProcess(Rabbit* rabbit, byte* output, const byte* input, word32 sz);
/**
  * @}
  */



/** @defgroup MICO_Security_RSA MICO RSA Encryption Algorithm
  * @brief    MiCO RSA Encryption and Decryption Function
  * @{
  */

/** 
 *  @brief    Constant data during RSA Encryption
 */
#define DBRG_SEED_LEN (440/8)
enum {
    RSA_PUBLIC   = 0,
    RSA_PRIVATE  = 1
};
/** 
 *  @brief    OS specific seeder 
 */
typedef struct OS_Seed {
  int fd;
} OS_Seed;


/** 
 *  @brief    the infamous mp_int structure 
 */
typedef struct  {
    int used, alloc, sign;
    unsigned short *dp;
} mp_int;

/** 
 *  @brief   rsakey define
 */
typedef struct RsaKey {
    mp_int n, e, d, p, q, dP, dQ, u;
    int   type;                               /* public or private */
    void* heap;                               /* for user memory overrides */
} RsaKey;


/** 
 *  @brief    Constant data during RSA Encryption
 */
enum {
    SHA256_BLOCK_SIZE   = 64,
    SHA256_DIGEST_SIZE  = 32,
    SHA256_PAD_SIZE     = 56
};

/** 
 *  @brief    ha256 digest 
 */
typedef struct Sha256 {
    word32  buffLen;   /* in bytes          */
    word32  loLen;     /* length in bytes   */
    word32  hiLen;     /* length in bytes   */
    word32  digest[SHA256_DIGEST_SIZE / sizeof(word32)];
    word32  buffer[SHA256_BLOCK_SIZE  / sizeof(word32)];
} Sha256;

/** 
 *  @brief  RNG definition used for RSA
 */
typedef struct CyaSSL_RNG {
  OS_Seed seed;
  Sha256 sha;
  byte digest[SHA256_DIGEST_SIZE];
  byte V[DBRG_SEED_LEN];
  byte C[DBRG_SEED_LEN];
  word64 reseed_ctr;
} CyaSSL_RNG;



/**
 * @brief          Initialize RsaKey
 *
 * @param key      RsaKey context
 *
 * @retval         None
 */
void InitRsaKey(RsaKey* key, void*);


/**
 * @brief          Free RsaKey
 *
 * @param key      RsaKey context
 *
 * @retval         None
 */
void FreeRsaKey(RsaKey* key);


/**
 * @brief          Initialize Rng for RSA
 *
 * @param key      CyaSSL_RNG context
 *
 * @retval         kNoErr is returned on success, otherwise, kXXXErr is returned.
 */
int  InitRng(CyaSSL_RNG *);


/**
 * @brief          Encrypt the string with the public key
 *
 * @param in       string need to be encrypted
 * @param inLen    string length
 * @param out      used to store the result of encryption 
 * @param outLen   length of out
 * @param key      RsaKey context
 * @param rng      CyaSSL_RNG context
 *
 * @retval         kNoErr is returned on success, otherwise, kXXXErr is returned.
 */
int  RsaPublicEncrypt(const byte* in, word32 inLen, byte* out,
                      word32 outLen, RsaKey* key, CyaSSL_RNG* rng);


/**
 * @brief          Decrypt the string with the private key
 *
 * @param in       string need to be decrypted
 * @param inLen    string length
 * @param out      used to store the result of decryption 
 * @param outLen   length of out
 * @param key      RsaKey context
 *
 * @retval         Return the length in bytes written to out or a negative 
                   number in case of an error.
 */
int  RsaPrivateDecrypt(const byte* in, word32 inLen, byte* out,
                       word32 outLen, RsaKey* key);


/**
 * @brief          Sign data using RSA
 * @param in       string need to be signed
 * @param inLen    string length
 * @param out      used to store the result of signing 
 * @param outLen   length of out
 * @param key      RsaKey context
 * @param rng      rng used to sign
 *
 * @retval         return the length in bytes written to out or a negative 
                   number in case of an error.
 */
int  RsaSSL_Sign(const byte* in, word32 inLen, byte* out,
                 word32 outLen, RsaKey* key, CyaSSL_RNG* rng);


/**
 * @brief          Verify data using RSA
 *
 * @param in       string need to be verify
 * @param inLen    string length
 * @param out      used to store the result of verifying
 * @param outLen   length of out
 * @param key      RsaKey context
 *
 * @retval         return the length in bytes written to out or a negative 
                   number in case of an error.
 */
int  RsaSSL_Verify(const byte* in, word32 inLen, byte* out,
                   word32 outLen, RsaKey* key);


/**
 * @brief             Decode the private key
 *
 * @param input       holds the raw data from the key
 * @param inOutIdx    start address
 * @param key         RsaKey context 
 * @param length      length to decode
 *
 * @retval            return the index or error
 */
int RsaPrivateKeyDecode(const byte* input, word32* inOutIdx, RsaKey* key,
                        word32 length);


/**
 * @brief             Decode the public key
 *
 * @param input       holds the raw data from the key
 * @param inOutIdx    start address
 * @param key         RsaKey context 
 * @param length      length to decode
 *
 * @retval            return the index or error
 */
int RsaPublicKeyDecode(const byte* input, word32* inOutIdx, RsaKey *key,word32);

/**
  * @}
  */


/**
  * @}
  */
