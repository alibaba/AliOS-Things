/* Copyright (C) 2016 RDA Technologies Limited and/or its affiliates("RDA").
* All rights reserved.
*
* This software is supplied "AS IS" without any warranties.
* RDA assumes no responsibility or liability for the use of the software,
* conveys no license or title under any patent, copyright, or mask work
* right to the product. RDA reserves the right to make changes in the
* software without notification.  RDA also make no representation or
* warranty that such application will be suitable for the specified use
* without further testing or modification.
*/


#ifndef SPAL_CIPHER_H
#define SPAL_CIPHER_H

// SPAL CIPHER
///@defgroup spal_cipher SPAL Ciphering API
///@{



//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "cs_types.h"
#include "global_macros.h"
#include "cipher.h"

//------------------------------------------------------------------------------
// Defines
//------------------------------------------------------------------------------

//-----------------------
// Cipher parameter types
//-----------------------

// Errors returned by the cipher driver functions
typedef enum
{
    cipher_err_none = 0,
    cipher_err_len = 1,
    cipher_err_running = 0xFF
} SPAL_CIPHER_ERROR_T;

typedef enum
{
    CIPHER_A51 = 0,
    CIPHER_A52 = 1,
#if (CHIP_CIPHER_A53_SUPPORTED == 1)
    CIPHER_A53 = 2
#endif
} SPAL_CIPHER_MODE_T;


//------------------------------------------------------------------------------
// Functions
//------------------------------------------------------------------------------


//======================================================================
// Initialize the count register
//----------------------------------------------------------------------
/// This function initializes the count register.
///
/// @param count UINT32. Counter register.
//======================================================================
PUBLIC UINT8 spal_CipherCountInit (UINT32 count);

//======================================================================
// spal_CipherStart
//----------------------------------------------------------------------
/// Start the encipherment.
///
/// @param algo UINT8.
///
/// @return UINT8. Error or no error??
//======================================================================
PUBLIC UINT8 spal_CipherStart(UINT8 algo);

//======================================================================
// spal_CipherCipherNB
//----------------------------------------------------------------------
/// This function cipher 114 bits for normal burst.
///
/// @param BufferIn UINT32*. Pointer to input buffer.
/// @param BufferOut UINT32*. Pointer to output buffer.
//
//======================================================================
PUBLIC VOID spal_CipherCipherNb(UINT32 *BufferIn, UINT32 *BufferOut);

//======================================================================
// spal_CipherDecipherNB
//----------------------------------------------------------------------
/// This function deciphers 114 bits for normal burst.
///
/// @param BufferIn UINT32*. Pointer to input buffer.
/// @param BufferOut UINT32*. Pointer to output buffer.
//
//======================================================================
PUBLIC VOID spal_CipherDecipherNb(UINT32 *BufferIn, UINT32 *BufferOut);




//======================================================================
// Initialize the count register
//----------------------------------------------------------------------
/// This function initializes the count register.
///
/// @param count UINT32. Counter register.
//======================================================================
PUBLIC UINT8 spal_CipherA53CountInit (UINT32 count);

//======================================================================
// spal_CipherA53Start
//----------------------------------------------------------------------
/// Start the encipherment.
///
/// @param algo UINT8.
///
/// @return UINT8. Error or no error??
//======================================================================
PUBLIC UINT8 spal_CipherA53Start();

//======================================================================
// spal_CipherCipherA53Nb
//----------------------------------------------------------------------
/// This function cipher 114 bits for normal burst.
///
/// @param BufferIn UINT32*. Pointer to input buffer.
/// @param BufferOut UINT32*. Pointer to output buffer.
//
//======================================================================
PUBLIC VOID spal_CipherCipherA53Nb(UINT32 *BufferIn, UINT32 *BufferOut);

//======================================================================
// spal_CipherDecipherA53Nb
//----------------------------------------------------------------------
/// This function deciphers 114 bits for normal burst.
///
/// @param BufferIn UINT32*. Pointer to input buffer.
/// @param BufferOut UINT32*. Pointer to output buffer.
//
//======================================================================
PUBLIC VOID spal_CipherDecipherA53Nb(UINT32 *BufferIn, UINT32 *BufferOut);

///@}
//
#endif                                          // #ifndef SPAL_CIPHER_H


