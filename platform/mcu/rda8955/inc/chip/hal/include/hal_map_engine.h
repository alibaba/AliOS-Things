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


#ifndef _HAL_MAP_ENGINE_H_
#define _HAL_MAP_ENGINE_H_


#include "hal_error.h"

// =============================================================================
//  MACROS
// =============================================================================

// ============================================================================
// HAL_MAP_ID_T
// -----------------------------------------------------------------------------
/// Software modules identifiers. Caution, these values are also used in the file
/// "csw_ver.h". Must be compatible with the type MAP_TABLE_T of "chipstd_soft_root.xmd".
// =============================================================================
typedef enum
{
    HAL_MAP_ID_HAL                              = 0x00000000,
    HAL_MAP_ID_PAL                              = 0x00000001,
    HAL_MAP_ID_SPAL                             = 0x00000002,
    HAL_MAP_ID_SPP                              = 0x00000003,
    HAL_MAP_ID_SPC                              = 0x00000004,
    HAL_MAP_ID_RFD                              = 0x00000005,
    HAL_MAP_ID_FREE1                            = 0x00000006,
    HAL_MAP_ID_AT                               = 0x00000007,
    HAL_MAP_ID_MEMD                             = 0x00000008,
    HAL_MAP_ID_PMD                              = 0x00000009,
    HAL_MAP_ID_STACK                            = 0x0000000A,
    HAL_MAP_ID_VPP                              = 0x0000000B,
    HAL_MAP_ID_SX                               = 0x0000000C,
    HAL_MAP_ID_STD                              = 0x0000000D,
    HAL_MAP_ID_LCDD                             = 0x0000000E,
    HAL_MAP_ID_ARS                              = 0x0000000F,
    HAL_MAP_ID_APS                              = 0x00000010,
    HAL_MAP_ID_VOIS                             = 0x00000011,
    HAL_MAP_ID_CALIB                            = 0x00000012,
    HAL_MAP_ID_GTES                             = 0x00000013,
    HAL_MAP_ID_FLASH_PROG                       = 0x00000014,
    HAL_MAP_ID_CSW                              = 0x00000015,
    HAL_MAP_ID_MMI                              = 0x00000016,
    HAL_MAP_ID_AUD                              = 0x00000017,
    HAL_MAP_ID_MDI                              = 0x00000018,
    HAL_MAP_ID_BTD                              = 0x00000019,
    HAL_MAP_ID_COUNT                            = 0x0000001A
} HAL_MAP_ID_T;


// =============================================================================
//  TYPES
// =============================================================================

// ============================================================================
// HAL_MAP_VERSION_T
// -----------------------------------------------------------------------------
/// Type used to define the version of a software module.
// =============================================================================
typedef struct
{
    UINT32                         revision;                     //0x00000000
    UINT32                         number;                       //0x00000004
    UINT32                         date;                         //0x00000008
    INT8*                          string;                       //0x0000000C
} HAL_MAP_VERSION_T; //Size : 0x10



// ============================================================================
// HAL_MAP_MODULE_T
// -----------------------------------------------------------------------------
/// Type used to access the version and the memory map of a soft module.
// =============================================================================
typedef struct
{
    HAL_MAP_VERSION_T*             version;                      //0x00000000
    VOID*                          access;                       //0x00000004
} HAL_MAP_MODULE_T; //Size : 0x8



// ============================================================================
// HAL_MAP_TABLE_T
// -----------------------------------------------------------------------------
/// Type used to access the version and the memory map of a soft module.
// =============================================================================
typedef struct
{
    /// Type used to access the version and the memory map of a soft module.
    HAL_MAP_MODULE_T               modules[HAL_MAP_ID_COUNT];    //0x00000000
} HAL_MAP_TABLE_T; //Size : 0xD0


// Locate const access/version in rodata, and usually it will be in FLASH
#define HAL_MAP_ACCESS_CONST_LOC    __attribute__((section(".rodata")))
#define HAL_MAP_VER_CONST_LOC       __attribute__((section(".rodata")))

// Locate non-const access/version in uncached data
#define HAL_MAP_ACCESS_LOC          __attribute__((section(".ucdata")))
#define HAL_MAP_VER_LOC             __attribute__((section(".ucdata")))

// =============================================================================
//  FUNCTIONS
// =============================================================================



// =============================================================================
// hal_MapEngineOpen
// -----------------------------------------------------------------------------
/// Initialize the map engine so that the remote tools can access the map
/// structures of the software modules.
// =============================================================================
PUBLIC VOID hal_MapEngineOpen(VOID);



// =============================================================================
// hal_MapEngineRegisterModule
// -----------------------------------------------------------------------------
/// Configure the map access field for the specified module in the table
/// located at the address pointer by the fixed pointer.
/// @param id Id of the module for which the version and the access
/// pointer will be specified.
/// @param version Pointer to a structure giving the version information for
/// the module 'id'. This structure must be instantiated by the module 'id'.
/// @param access Pointer to a structure containing the informations that will
/// be accessible from the remote PC (e.g. through CoolWatcher). This strucutre
/// must be instantiated by the module 'id'.
/// @return HAL_ERR_NO if no error or HAL_ERR_BAD_PARAMETER if the module 'id'
/// cannot be found.
// =============================================================================
PUBLIC HAL_ERR_T hal_MapEngineRegisterModule(HAL_MAP_ID_T id, CONST HAL_MAP_VERSION_T *version, VOID* access);


// =============================================================================
// hal_MapEnginePrintModuleVersion
// -----------------------------------------------------------------------------
/// Print a module version's string on the Trace, if the module has been
/// registered.
/// @param id Number of the Module whose version is to be put
/// on the Trace.
// =============================================================================
PUBLIC VOID hal_MapEnginePrintModuleVersion(HAL_MAP_ID_T id);


// =============================================================================
// hal_MapEnginePrintAllModulesVersion
// -----------------------------------------------------------------------------
/// Print all module version's string on the Trace.
// =============================================================================
PUBLIC VOID hal_MapEnginePrintAllModulesVersion(VOID);


// =============================================================================
// hal_MapEngineGetVersion
// -----------------------------------------------------------------------------
/// Get the pointer to the version structure of a given module.
///
/// @param id Number of the Module whose version is to be got.
/// @return Pointer to the version structure of said module.
// =============================================================================
PUBLIC CONST HAL_MAP_VERSION_T* hal_MapEngineGetVersion(HAL_MAP_ID_T id);


/// @} // --> End of the group.


#endif

