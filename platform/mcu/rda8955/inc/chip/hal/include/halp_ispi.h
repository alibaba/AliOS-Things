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



#ifndef _HALP_ISPI_H_
#define _HALP_ISPI_H_

#include "cs_types.h"

#include "chip_id.h"

#if (CHIP_ISPI_CS_QTY != 0)
#include "hal_ispi.h"
#include "hal_sys.h"

// =============================================================================
// hal_IspiUpdateDivider
// -----------------------------------------------------------------------------
/// Update the ISPI divider so that the maximum frequency defined is not
/// overpassed.
///
/// @param sysFreq Newly applied system frequency
// =============================================================================
PROTECTED UINT32 hal_IspiUpdateDivider(HAL_SYS_FREQ_T sysFreq);


#endif // CHIP_ISPI_CS_QTY

#endif // _HALP_ISPI_H_


