/****************************************************************************
* This confidential and proprietary software may be used only as authorized *
* by a licensing agreement from ARM Israel.                                 *
* Copyright (C) 2015 ARM Limited or its affiliates. All rights reserved.    *
* The entire notice above must be reproduced on all authorized copies and   *
* copies may only be made to the extent permitted by a licensing agreement  *
* from ARM Israel.                                                          *
*****************************************************************************/

/*!
@file
@brief This file contains the platform dependent definitions and types.
*/

#ifndef SSI_PAL_TYPES_H
#define SSI_PAL_TYPES_H

#include "ssi_pal_types_plat.h"

typedef enum {
    SASI_FALSE = 0,
    SASI_TRUE = 1
} SaSiBool;

#define SASI_SUCCESS              0UL
#define SASI_FAIL          1UL

#define SASI_1K_SIZE_IN_BYTES    1024
#define SASI_BITS_IN_BYTE    8
#define SASI_BITS_IN_32BIT_WORD    32
#define SASI_32BIT_WORD_SIZE    (sizeof(uint32_t))

#define SASI_OK   SASI_SUCCESS

#define SASI_UNUSED_PARAM(prm)  ((void)prm)

#define SASI_MAX_UINT32_VAL     (0xFFFFFFFF)


/* Minimum and Maximum macros */
#ifdef  min
#define CRYS_MIN(a,b) min( a , b )
#else
#define CRYS_MIN( a , b ) ( ( (a) < (b) ) ? (a) : (b) )
#endif

#ifdef max
#define CRYS_MAX(a,b) max( a , b )
#else
#define CRYS_MAX( a , b ) ( ( (a) > (b) ) ? (a) : (b) )
#endif


#define CALC_FULL_BYTES(numBits)         (((numBits) + (SASI_BITS_IN_BYTE -1))/SASI_BITS_IN_BYTE)
#define CALC_FULL_32BIT_WORDS(numBits)         (((numBits) + (SASI_BITS_IN_32BIT_WORD -1))/SASI_BITS_IN_32BIT_WORD)
#define CALC_32BIT_WORDS_FROM_BYTES(sizeBytes)  (((sizeBytes) + SASI_32BIT_WORD_SIZE - 1) / SASI_32BIT_WORD_SIZE)
#define ROUNDUP_BITS_TO_32BIT_WORD(numBits)     (CALC_FULL_32BIT_WORDS(numBits)*SASI_BITS_IN_32BIT_WORD)
#define ROUNDUP_BITS_TO_BYTES(numBits)         (CALC_FULL_BYTES(numBits)*SASI_BITS_IN_BYTE)
#define ROUNDUP_BYTES_TO_32BIT_WORD(numBytes)     (SASI_32BIT_WORD_SIZE*(((numBytes)+SASI_32BIT_WORD_SIZE-1)/SASI_32BIT_WORD_SIZE))



#endif
