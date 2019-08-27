/****************************************************************************
* This confidential and proprietary software may be used only as authorized *
* by a licensing agreement from ARM Israel.                                 *
* Copyright (C) 2015 ARM Limited or its affiliates. All rights reserved.    *
* The entire notice above must be reproduced on all authorized copies and   *
* copies may only be made to the extent permitted by a licensing agreement  *
* from ARM Israel.                                                          *
*****************************************************************************/



#ifndef CRYS_RND_H
#define CRYS_RND_H

#include "crys_error.h"
#include "ssi_aes.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*!
@file
@brief This file contains the CRYS APIs used for random number generation.
The random-number generation module implements referenced standard [SP800-90].
*/

/************************ Defines ******************************/

/*!  Maximal reseed counter - indicates maximal number of
requests allowed between reseeds; according to NIST 800-90
it is (2^48 - 1), our restriction is :  (0xFFFFFFFF - 0xF).*/
#define CRYS_RND_MAX_RESEED_COUNTER     (0xFFFFFFFF - 0xF)

/* maximal requested size counter (12 bits active) - maximal count
of generated random 128 bit blocks allowed per one request of
Generate function according NIST 800-90 it is (2^12 - 1) = 0x3FFFF */
#define CRYS_RND_REQUESTED_SIZE_COUNTER  0x3FFFF

/*! AES output block size in words. */
#define CRYS_RND_AES_BLOCK_SIZE_IN_WORDS  SASI_AES_BLOCK_SIZE_IN_WORDS


/* RND seed and additional input sizes */
#define CRYS_RND_SEED_MAX_SIZE_WORDS                  12
#ifndef CRYS_RND_ADDITINAL_INPUT_MAX_SIZE_WORDS
#define CRYS_RND_ADDITINAL_INPUT_MAX_SIZE_WORDS    CRYS_RND_SEED_MAX_SIZE_WORDS
#endif

/* Max size of generated random vector in bits according uint16_t type of   *
*  input size parameter in CRYS_RND_Generate function                 */
#define CRYS_RND_MAX_GEN_VECTOR_SIZE_BYTES    0xFFFF

/* allowed sizes of AES Key, in words */
#define CRYS_RND_AES_KEY_128_SIZE_WORDS  4
#define CRYS_RND_AES_KEY_192_SIZE_WORDS  6
#define CRYS_RND_AES_KEY_256_SIZE_WORDS  8

/*   Definitions of temp buffer for RND_DMA version of CRYS_RND   */
/*******************************************************************/
/*   Definitions of temp buffer for DMA version of CRYS_RND   */

#define CRYS_RND_WORK_BUFFER_SIZE_WORDS 1528  //25

/*! A definition for RAM buffer to be internally used in instantiation (or reseeding) operation. */
typedef struct
{
    /* include the specific fields that are used by the low level */
    uint32_t crysRndWorkBuff[CRYS_RND_WORK_BUFFER_SIZE_WORDS];
}CRYS_RND_WorkBuff_t;

#define CRYS_RND_EntropyEstimatData_t  CRYS_RND_WorkBuff_t
#define crysRndEntrIntBuff  crysRndWorkBuff


/* RND source buffer inner (entrpopy) offset       */
#define CRYS_RND_TRNG_SRC_INNER_OFFSET_WORDS    2


/* Max size for one RNG operation */
#define CRYS_RND_MAX_SIZE_OF_OUTPUT_BYTES    3*1024

/* Size of the expected output buffer used by FIPS KAT */
#define CRYS_PRNG_FIPS_KAT_OUT_DATA_SIZE      64

/************************ Enumerators  ****************************/

/* Definition of Fast or Slow mode of random generator (TRNG)*/
typedef  enum
{
    CRYS_RND_Fast  = 0,
    CRYS_RND_Slow  = 1,
    CRYS_RND_ModeLast = 0x7FFFFFFF,
} CRYS_RND_mode_t;



/************************ Structs  *****************************/


/* The internal state of DRBG mechanism based on AES CTR and CBC-MAC
   algorithms. It is set as global data defined by the following
   structure  */
typedef  struct
{
    /* Seed buffer, consists from concatenated Key||V: max size 12 words */
    uint32_t  Seed[CRYS_RND_SEED_MAX_SIZE_WORDS];
    /* Previous value for continuous test */
    uint32_t  PreviousRandValue[SASI_AES_BLOCK_SIZE_IN_WORDS];

    /* AdditionalInput buffer max size = seed max size words + 4w for padding*/
    uint32_t  PreviousAdditionalInput[CRYS_RND_ADDITINAL_INPUT_MAX_SIZE_WORDS+3];
    uint32_t  AdditionalInput[CRYS_RND_ADDITINAL_INPUT_MAX_SIZE_WORDS+4];
    uint32_t  AddInputSizeWords; /* size of additional data set by user, words   */

    /* entropy source size in words */
    uint32_t  EntropySourceSizeWords;

    /* reseed counter (32 bits active) - indicates number of requests for entropy
    since instantiation or reseeding */
    uint32_t  ReseedCounter;

    /* key size: 4 or 8 words according to security strength 128 bits or 256 bits*/
    uint32_t KeySizeWords;

    /* State flag (see definition of StateFlag above), containing bit-fields, defining:
    - b'0: instantiation steps:   0 - not done, 1 - done;
    - 2b'9,8: working or testing mode: 0 - working, 1 - KAT DRBG test, 2 -
      KAT TRNG test;
    b'16: flag defining is Previous random valid or not:
            0 - not valid, 1 - valid */
    uint32_t StateFlag;

    /* Trng processing flag - indicates which ROSC lengths are:
    -  allowed (bits 0-3);
    -  total started (bits 8-11);
    -  processed (bits 16-19);
    -  started, but not processed (bits24-27)              */
    uint32_t TrngProcesState;

    /* validation tag */
    uint32_t ValidTag;

    /* Rnd source entropy size in bits */
    uint32_t  EntropySizeBits;

} CRYS_RND_State_t;


/*! The RND Generate vector function pointer type definition.
   The prototype intendent for External and CRYS internal RND functions
   pointers definitions.
   Full description can be found in ::CRYS_RND_GenerateVector function API. */
typedef uint32_t (*SaSiRndGenerateVectWorkFunc_t)(        \
                CRYS_RND_State_t  *rndState_ptr, /*context*/   \
                uint16_t          outSizeBytes,   /*in*/      \
                uint8_t          *out_ptr         /*out*/);


/*! definition of RND context that includes CRYS RND state structure and a function pointer for rnd generate function */
typedef  struct
{
        /* The pointer to internal state of RND */
       CRYS_RND_State_t   rndState;

       /* The pointer to user given function for generation random vector */
       SaSiRndGenerateVectWorkFunc_t rndGenerateVectFunc;
} CRYS_RND_Context_t;


/*! Required for internal FIPS verification for PRNG KAT. */
typedef  struct
{
       CRYS_RND_WorkBuff_t      rndWorkBuff;
       uint8_t                  rndOutputBuff[CRYS_PRNG_FIPS_KAT_OUT_DATA_SIZE];
} CRYS_PrngFipsKatCtx_t;


/*****************************************************************************/
/**********************        Public Functions      *************************/
/*****************************************************************************/

/*!
@brief This function initializes the RND context.
It must be called at least once prior to using this context with any API that requires it as a parameter (e.g., other RND APIs, asymmetric
cryptography key generation and signatures).
It is called as part of ARM TrustZone CryptoCell library initialization, which initializes and returns the primary RND context.
This primary context can be used as a single global context for all RND needs.
Alternatively, other contexts may be initialized and used with a more limited scope (for specific applications or specific threads).
The call to this function must be followed by a call to ::CRYS_RND_SetGenerateVectorFunc API to set the generate vector function.
It implements referenced standard [SP800-90] - 10.2.1.3.2 - CTR-DRBG Instantiate algorithm using AES (FIPS-PUB 197) and Derivation Function (DF).
\note Additional data can be mixed with the random seed (personalization data or nonce). If required, this data should be provided by calling ::CRYS_RND_AddAdditionalInput prior to using this API.

@return CRYS_OK on success.
@return A non-zero value from crys_rnd_error.h on failure.
*/
CIMPORT_C CRYSError_t CRYS_RND_Instantiation(
                        CRYS_RND_Context_t   *rndContext_ptr,       /*!< [in/out]  Pointer to the RND context buffer allocated by the user, which is used to
                                           maintain the RND state, as well as pointers to the functions used for
                                           random vector generation. This context must be saved and provided as a
                                           parameter to any API that uses the RND module.
                                           \note the context must be cleared before sent to the function. */
                        CRYS_RND_WorkBuff_t  *rndWorkBuff_ptr       /*!< [in/out] Scratchpad for the RND module's work. */
);


/*!
@brief Clears existing RNG instantiation state.

@return CRYS_OK on success.
@return A non-zero value from crys_rnd_error.h on failure.
*/
CIMPORT_C CRYSError_t CRYS_RND_UnInstantiation(
                        CRYS_RND_Context_t *rndContext_ptr       /*!< [in/out] Pointer to the RND context buffer. */
);


/*!
@brief This function is used for reseeding the RNG with additional entropy and additional user-provided input.
(additional data should be provided by calling ::CRYS_RND_AddAdditionalInput prior to using this API).
It implements referenced standard [SP800-90] - 10.2.1.4.2 - CTR-DRBG Reseeding algorithm, using AES (FIPS-PUB 197) and Derivation Function (DF).

@return CRYS_OK on success.
@return A non-zero value from crys_rnd_error.h on failure.
*/
CIMPORT_C CRYSError_t CRYS_RND_Reseeding(
                        CRYS_RND_Context_t   *rndContext_ptr,      /*!< [in/out] Pointer to the RND context buffer. */
                        CRYS_RND_WorkBuff_t  *rndWorkBuff_ptr      /*!< [in/out] Scratchpad for the RND module's work. */
);


/****************************************************************************************/
/*!
@brief Generates a random vector according to the algorithm defined in referenced standard [SP800-90] - 10.2.1.5.2 - CTR-DRBG.
The generation algorithm uses AES (FIPS-PUB 197) and Derivation Function (DF).

\note
<ul id="noteb"><li> The RND module must be instantiated prior to invocation of this API.</li>
<li> In the following cases, Reseeding operation must be performed prior to vector generation:</li>
    <ul><li> Prediction resistance is required.</li>
    <li> The function returns CRYS_RND_RESEED_COUNTER_OVERFLOW_ERROR, stating that the Reseed Counter has passed its upper-limit (2^32-2).</li></ul></ul>

@return CRYS_OK on success.
@return A non-zero value from crys_rnd_error.h on failure.
*/
CIMPORT_C CRYSError_t CRYS_RND_GenerateVector(
                            CRYS_RND_State_t *rndState_ptr,     /*!< [in/out] Pointer to the RND state structure, which is part of the RND context structure.
                                     Use rndContext->rndState field of the context for this parameter. */
                            uint16_t   outSizeBytes,            /*!< [in]  The size in bytes of the random vector required. The maximal size is 2^16 -1 bytes. */
                            uint8_t   *out_ptr                  /*!< [out] The pointer to output buffer. */
);


/****************************************************************************************/
/*!

@brief This function sets the RND vector generation function into the RND context.

It must be called after ::CRYS_RND_Instantiation, and prior to any other API that requires the RND context as parameter.
It is called as part of ARM TrustZone CryptoCell library initialization, to set the RND vector generation function into the primary RND context,
after ::CRYS_RND_Instantiation is called.

@return CRYS_OK on success.
@return A non-zero value from crys_rnd_error.h on failure.
*/
CRYSError_t CRYS_RND_SetGenerateVectorFunc(
                    CRYS_RND_Context_t *rndContext_ptr,                     /*!< [in/out] Pointer to the RND context buffer allocated by the user,
                                              which is used to maintain the RND state as well as pointers
                                              to the functions used for random vector generation. */
                    SaSiRndGenerateVectWorkFunc_t rndGenerateVectFunc       /*!< [in] Pointer to the random vector generation function.
                                              The pointer should point to the ::CRYS_RND_GenerateVector function. */
);


/**********************************************************************************************************/
/*!
@brief Generates a random vector with specific limitations by testing candidates (described and used in FIPS 186-4: B.1.2, B.4.2 etc.).

This function will draw a random vector, compare it to the range limits, and if within range - return it in rndVect_ptr.
If outside the range, the function will continue retrying until a conforming vector is found, or the maximal retries limit is exceeded.
If maxVect_ptr is provided, rndSizeInBits specifies its size, and the output vector must conform to the range [1 < rndVect < maxVect_ptr].
If maxVect_ptr is NULL, rndSizeInBits specifies the exact required vector size, and the output vector must be the exact same
bit size (with its most significant bit = 1).
\note
The RND module must be instantiated prior to invocation of this API.

@return CRYS_OK on success.
@return A non-zero value from crys_rnd_error.h on failure.
*/
CIMPORT_C CRYSError_t CRYS_RND_GenerateVectorInRange(
                    CRYS_RND_Context_t *rndContext_ptr,     /*!< [in/out] Pointer to the RND context buffer. */
                    uint32_t  rndSizeInBits,                /*!< [in]  The size in bits of the random vector required. The allowed size in range  2 <= rndSizeInBits < 2^19-1, bits. */
                    uint8_t  *maxVect_ptr,                  /*!< [in]  Pointer to the vector defining the upper limit for the random vector output, Given as little-endian byte array.
                                                                       If not NULL, its actual size is treated as [(rndSizeInBits+7)/8] bytes. */
                    uint8_t  *rndVect_ptr                   /*!< [in/out] Pointer to the output buffer for the random vector. Must be at least [(rndSizeInBits+7)/8] bytes.
                                                                 Treated as little-endian byte array. */
);


/*************************************************************************************/
/*!
@brief Used for adding additional input/personalization data provided by the user,
to be later used by the ::CRYS_RND_Instantiation/::CRYS_RND_Reseeding/::CRYS_RND_GenerateVector functions.

@return CRYS_OK on success.
@return A non-zero value from crys_rnd_error.h on failure.
*/
CIMPORT_C CRYSError_t CRYS_RND_AddAdditionalInput(
                            CRYS_RND_Context_t *rndContext_ptr,     /*!< [in/out] Pointer to the RND context buffer. */
                            uint8_t *additonalInput_ptr,            /*!< [in]  The Additional Input buffer. */
                            uint16_t additonalInputSize             /*!< [in]  The size of the Additional Input buffer. Must be <= 48, and a multiple of 4. */
);

/*!
@brief The CRYS_RND_EnterKatMode function sets KAT mode bit into StateFlag of global CRYS_RND_WorkingState structure.

The user must call this function before calling functions performing KAT tests.

\note Total size of entropy and nonce must be not great than: ::CRYS_RND_MAX_KAT_ENTROPY_AND_NONCE_SIZE, defined.

@return CRYS_OK on success.
@return A non-zero value from crys_rnd_error.h on failure.
*/
CIMPORT_C CRYSError_t CRYS_RND_EnterKatMode(
            CRYS_RND_Context_t *rndContext_ptr,     /*!< [in/out] Pointer to the RND context buffer. */
            uint8_t            *entrData_ptr,       /*!< [in] Entropy data. */
            uint32_t            entrSize,           /*!< [in] Entropy size in bytes. */
            uint8_t            *nonce_ptr,          /*!< [in] Nonce. */
            uint32_t            nonceSize,          /*!< [in] Entropy size in bytes. */
            CRYS_RND_WorkBuff_t  *workBuff_ptr      /*!< [out] RND working buffer, must be the same buffer, which should be passed into
                            Instantiation/Reseeding functions. */
);

/**********************************************************************************************************/
/*!
@brief The CRYS_RND_DisableKatMode function disables KAT mode bit into StateFlag of global CRYS_RND_WorkingState structure.

The user must call this function after KAT tests before actual using RND module (Instantiation etc.).

@return CRYS_OK on success.
@return A non-zero value from crys_rnd_error.h on failure.
*/
CIMPORT_C void CRYS_RND_DisableKatMode(
                    CRYS_RND_Context_t   *rndContext_ptr     /*!< [in/out] Pointer to the RND context buffer. */
);


#ifdef __cplusplus
}
#endif

#endif /* #ifndef CRYS_RND_H */

