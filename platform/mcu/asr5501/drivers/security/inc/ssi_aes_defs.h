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
@brief This file contains definitions that are used for the ARM CryptoCell 3xx version of the CryptoCell AES APIs.
*/

#ifndef SSI_AES_DEFS_H
#define SSI_AES_DEFS_H

#include "ssi_pal_types.h"


#ifdef __cplusplus
extern "C"
{
#endif

/************************ Defines ******************************/

/*! The size of the user's context prototype (see SaSiAesUserContext_t) in words. */
#define SASI_AES_USER_CTX_SIZE_IN_WORDS (4+4+7+4)

/*! The AES block size in words. */
#define SASI_AES_BLOCK_SIZE_IN_WORDS 4
/*! The AES block size in bytes. */
#define SASI_AES_BLOCK_SIZE_IN_BYTES  (SASI_AES_BLOCK_SIZE_IN_WORDS * sizeof(uint32_t))

/*! The size of the IV buffer in words. */
#define SASI_AES_IV_SIZE_IN_WORDS   SASI_AES_BLOCK_SIZE_IN_WORDS
/*! The size of the IV buffer in bytes. */
#define SASI_AES_IV_SIZE_IN_BYTES  (SASI_AES_IV_SIZE_IN_WORDS * sizeof(uint32_t))

/*! The maximum size of the AES KEY in words. */
#define SASI_AES_KEY_MAX_SIZE_IN_WORDS 4
/*! The maximum size of the AES KEY in bytes. */
#define SASI_AES_KEY_MAX_SIZE_IN_BYTES (SASI_AES_KEY_MAX_SIZE_IN_WORDS * sizeof(uint32_t))


#ifdef __cplusplus
}
#endif

#endif /* #ifndef SSI_AES_DEFS_H */
