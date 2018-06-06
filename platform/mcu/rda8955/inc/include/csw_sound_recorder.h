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


#ifndef _CSW_SOUND_RECORDER_H_
#define _CSW_SOUND_RECORDER_H_

#if (CSW_EXTENDED_API == 1)

#include "cs_types.h"
#include "mci_sound_recorder.h"

// ============================================================================
// Defines
// ============================================================================

// ============================================================================
// INT32 Mmc_sndRecResume(VOID)
// ============================================================================
#define    Mmc_sndRecResume mci_SndRecResume

// ============================================================================
// INT32 Mmc_sndRecPause(VOID)
// ============================================================================
#define    Mmc_sndRecPause mci_SndRecPause

// ============================================================================
// INT32 Mmc_sndRecStop(VOID)
// ============================================================================
#define    Mmc_sndRecStop mci_SndRecStop


#endif // CSW_EXTENDED_API

/// @}

#endif //_CSW_SOUND_RECORDER_H_


