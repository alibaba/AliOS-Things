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



#ifndef _HAL_NAND_H_
#define _HAL_NAND_H_

/// @defgroup cipher HAL NAND Driver
///
/// @{
///

#include "cs_types.h"

// =============================================================================
//  MACROS
// =============================================================================

// =============================================================================
//  TYPES
// =============================================================================

// =============================================================================
//  FUNCTIONS
// =============================================================================

// =============================================================================
// hal_NandOpen
// -----------------------------------------------------------------------------
/// Initialize NAND flash controller
/// @param None
/// @return None
// =============================================================================
PUBLIC VOID hal_NandOpen(VOID);


// =============================================================================
// hal_NandReadData
// -----------------------------------------------------------------------------
/// Read data from NAND flash
/// @param addr    The start address in NAND flash
/// @param pData   Pointing to a buffer to hold the data
/// @param len     The number of bytes to be read
/// @return None
// =============================================================================
PUBLIC VOID hal_NandReadData(UINT32 addr, VOID *pData, UINT32 len);



/// @} <-- End of the nand group

#endif // _HAL_NAND_H_

