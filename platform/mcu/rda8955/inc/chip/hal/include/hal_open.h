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


#ifndef __HAL_OPEN_H__
#define __HAL_OPEN_H__

#include "cs_types.h"
#include "hal_config.h"

// =============================================================================
// hal_Open
// -----------------------------------------------------------------------------
/// Initializes the HAL layer.
/// This function also do the open of the PMD driver and set the idle-mode.
// =============================================================================
PUBLIC VOID hal_Open(CONST HAL_CFG_CONFIG_T*);

#endif // __HAL_OPEN_H__


