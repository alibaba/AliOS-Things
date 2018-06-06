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

#ifndef _TGT_APP_CFG_H_
#define _TGT_APP_CFG_H_

// =============================================================================
// TGT_DSM_PART_CONFIG
// =============================================================================

#define TGT_DSM_VDS0_SECCNT (3)
#define TGT_DSM_VDS1_SECCNT (0)
#define TGT_DSM_WAPMMS_SECCNT (0)
#define TGT_DSM_NVRAM_SECCNT (0)
#define TGT_DSM_LTENV_SECCNT (0)
#define TGT_DSM_NBPSM_SECCNT (0)
#define TGT_DSM_MMC0_ENABLED (1)

// =============================================================================
// TGT_CSW_MEM_CONFIG
// -----------------------------------------------------------------------------
/// This structure describes the user heap size
/// cswHeapSize: Size of the heap available for csw
/// cosHeapSize: Size of the heap available for mmi
// =============================================================================
#define TGT_CSW_CONFIG             \
    {                              \
        .cswHeapSize = 350 * 1024, \
        .cosHeapSize = 1200 * 1024 \
    }

#endif //_TGT_APP_CFG_H_
