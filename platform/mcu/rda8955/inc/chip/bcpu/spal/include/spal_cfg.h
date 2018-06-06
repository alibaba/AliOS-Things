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


#ifndef __SPAL_CFG_H__
#define __SPAL_CFG_H__

#include "cs_types.h"
#include "spal_irq.h"

typedef struct
{
    BOOL irqInRom;                 /// Use irq contained on the ROM vs. RAM
    SPAL_IRQ_HANDLER_T irqHandler; /// Set VoC user defined interrupt handler function
    BOOL simuUL;                   /// test an uplink conding scheme, for model simulation only

} SPAL_GLOBAL_PARAMS_T;


// =============================================================================
// spal_Open
// -----------------------------------------------------------------------------
/// This function MUST be in RAM and not in ROM !
/// This function initializes the global parameter structure used by SPAL.
/// @param pCfg SPAL_GLOBAL_PARAMS_T. Pointer to the configuration structure.
// =============================================================================
PUBLIC VOID spal_Open (SPAL_GLOBAL_PARAMS_T* pCfg);




#endif

