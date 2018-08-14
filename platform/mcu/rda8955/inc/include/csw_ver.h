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


#ifndef _CSW_VER_H_
#define _CSW_VER_H_

#include "cs_types.h"


// =============================================================================
//  MACROS
// =============================================================================


// =============================================================================
//  TYPES
// =============================================================================

/// This type must stay compatible with the HAL_MAP_MODULE_T type.
typedef enum
{
    CSW_VER_MODULE_ID_HAL                              = 0x00000000,
    CSW_VER_MODULE_ID_PAL                              = 0x00000001,
    CSW_VER_MODULE_ID_SPAL                             = 0x00000002,
    CSW_VER_MODULE_ID_SPP                              = 0x00000003,
    CSW_VER_MODULE_ID_SPC                              = 0x00000004,
    CSW_VER_MODULE_ID_RFD                              = 0x00000005,
    CSW_VER_MODULE_ID_FREE1                            = 0x00000006,
    CSW_VER_MODULE_ID_FREE2                            = 0x00000007,
    CSW_VER_MODULE_ID_MEMD                             = 0x00000008,
    CSW_VER_MODULE_ID_PMD                              = 0x00000009,
    CSW_VER_MODULE_ID_STACK                            = 0x0000000A,
    CSW_VER_MODULE_ID_VPP                              = 0x0000000B,
    CSW_VER_MODULE_ID_SX                               = 0x0000000C,
    CSW_VER_MODULE_ID_STD                              = 0x0000000D,
    CSW_VER_MODULE_ID_LCDD                             = 0x0000000E,
    CSW_VER_MODULE_ID_ARS                              = 0x0000000F,
    CSW_VER_MODULE_ID_APS                              = 0x00000010,
    CSW_VER_MODULE_ID_VOIS                             = 0x00000011,
    CSW_VER_MODULE_ID_CALIB                            = 0x00000012,
    CSW_VER_MODULE_ID_GTES                             = 0x00000013,
    CSW_VER_MODULE_ID_FLASH_PROG                       = 0x00000014,
    CSW_VER_MODULE_ID_CSW                              = 0x00000015,
    CSW_VER_MODULE_ID_MMI                              = 0x00000016,
    CSW_VER_MODULE_ID_AUD                              = 0x00000017,
    CSW_VER_MODULE_ID_COUNT                            = 0x00000018
} CSW_VER_MODULE_ID_T;




// =============================================================================
//  FUNCTIONS
// =============================================================================


// =============================================================================
//csw_VerGetRevision
// -----------------------------------------------------------------------------
/// Return the revision of a given module.
///
/// @param  moduleId Id the module.
/// @return The SVN revision of that version of the module.
// =============================================================================
PUBLIC UINT32 csw_VerGetRevision(CSW_VER_MODULE_ID_T moduleId);



// =============================================================================
//csw_VerGetNumber
// -----------------------------------------------------------------------------
/// Return the version number of a given module.
///
/// @param  moduleId Id the module.
/// @return The version number of that version of the module.
// =============================================================================
PUBLIC UINT32 csw_VerGetNumber(CSW_VER_MODULE_ID_T moduleId);



// =============================================================================
//csw_VerGetDate
// -----------------------------------------------------------------------------
/// Return the date of build of a given module.
/// The format is 0xYYYYMMDD.
///
/// @param  moduleId Id the module.
/// @return The build date of that version of the module.
// =============================================================================
PUBLIC UINT32 csw_VerGetDate(CSW_VER_MODULE_ID_T moduleId);



// =============================================================================
//csw_VerGetString
// -----------------------------------------------------------------------------
/// Return the version string of a given module.
///
/// @param  moduleId Id the module.
/// @return The version string of that version of the module.
// =============================================================================
PUBLIC CONST CHAR* csw_VerGetString(CSW_VER_MODULE_ID_T moduleId);



// =============================================================================
//csw_VerSetMmiVersion
// -----------------------------------------------------------------------------
/// Register the version of the MMI in the CSW version mechanism.
///
/// @param  revision Number of the current revision (the SCM revision).
/// @param  string Pointer to the string version.
// =============================================================================
PUBLIC VOID csw_VerSetMmiVersion(UINT32 revision, CONST CHAR* string);



#endif // _CSW_VER_H_
