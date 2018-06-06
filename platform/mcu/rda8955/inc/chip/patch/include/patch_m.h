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



#ifndef _PATCH_M_H_
#define _PATCH_M_H_

#include "cs_types.h"

/// @defgroup PATCH
/// @{


// =============================================================================
//  MACROS
// =============================================================================

#define PATCH_FAILED    (-1)

// =============================================================================
//  TYPES
// =============================================================================


// =============================================================================
//  FUNCTIONS
// =============================================================================


// =============================================================================
// patch_Open
// -----------------------------------------------------------------------------
/// This function install all the hardware patch available for the current chip
// =============================================================================
PUBLIC VOID patch_Open(VOID);


// =============================================================================
// patch_ReplaceFunction
// -----------------------------------------------------------------------------
/// This function install and enable a patch to replace a function.
/// The function in ROM must be aligned on 16 bytes boundaries.
/// The two functions must have the exact same prototype !
/// The patch will actually replace \p oldFunction by a direct jump
/// to \p newFunction.
/// @param oldFunction  the address/symbol of the function in ROM to replace.
/// @param newFunction  the address/symbol of the new implementation of the
///                     function.
/// @return             the number of the allocated hardware patch
///                     or \c PATCH_FAILED
// =============================================================================
PUBLIC INT32 patch_ReplaceFunction(CONST VOID (*oldFunction)(VOID),
                                   CONST VOID (*newFunction)(VOID));


// =============================================================================
// patch_ReplaceData
// -----------------------------------------------------------------------------
/// This function install and enable a patch to replace some rom data.
/// The data in ROM must be aligned on 16 bytes boundaries.
/// @param oldDataAddr  the address of the data in ROM to replace.
/// @param newData      pointing to the new data.
/// @param len          the length of the new data in bytes and must be 16.
/// @return             the number of the allocated hardware patch
///                     or \c PATCH_FAILED
// =============================================================================
PUBLIC INT32 patch_ReplaceData(CONST VOID *oldDataAddr,
                               CONST VOID *newData,
                               UINT32 len);


// =============================================================================
// patch_GetLocation
// -----------------------------------------------------------------------------
/// This function allocate a hardware patch, it then need to be enabled with
/// #patch_Enable.
/// Use this function to get a patch location when #patch_ReplaceFunction cannot
/// be used for your patch (patch data or patch assembly code)
/// @param patchArea    pointer to recieve the address of the patch area
///                     It can be null.
/// @return             the number of the hardware patch or \c PATCH_FAILED
// =============================================================================
PUBLIC INT32 patch_GetLocation(UINT32** patchArea);


// =============================================================================
// patch_Enable
// -----------------------------------------------------------------------------
/// This function enables a hardware patch.
/// Once the replacement code is placed in internal sram, use this function to
/// enable the hardware patch.
/// @param patchNumber  the number of the patch as returned by #patch_GetLocation
/// @param patchAddress the address in the rom to patch (will be 16 bytes aligned)
// =============================================================================
PUBLIC VOID patch_Enable(UINT32 patchNumber, UINT32 patchAddress);


///  @} <- End of the PATCH group

#endif // _PATCH_M_H_

