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


#ifndef _HAL_BOOT_SECTOR_RELOAD_H_
#define _HAL_BOOT_SECTOR_RELOAD_H_

#include "cs_types.h"

#include "chip_id.h"

#include "boot_sector.h"


// =============================================================================
// FUNCTIONS
// =============================================================================


// =============================================================================
// hal_BootSectorFillReloadCtx
// -----------------------------------------------------------------------------
/// Fill the boot sector reload context.
/// ctx: pointer to the boot sector reload context.
/// id: boot sector reload id
// =============================================================================
PUBLIC BOOL hal_BootSectorFillReloadCtx(BOOT_SECTOR_RELOAD_CTX_T *ctx, BOOT_SECTOR_RELOAD_ID_T id);


// =============================================================================
// hal_BootSectorReloadEnable
// -----------------------------------------------------------------------------
/// Validate the Boot Sector reload structure. This is a way to tell the Boot Sector
/// that the structure contains correct ram image and that it must be loaded
/// and executed when restart.
///
/// This is done by putting the address of the Boot Sector reload structure in
/// the Boot Sector reload structure pointer and enabling a flag in it.
// =============================================================================
PUBLIC VOID hal_BootSectorReloadEnable(VOID);


#endif // _HAL_BOOT_SECTOR_RELOAD_H_
