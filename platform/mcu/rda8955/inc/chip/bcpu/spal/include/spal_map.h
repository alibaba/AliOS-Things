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


#ifndef _SPAL_MAP_H_
#define _SPAL_MAP_H_


#include "cs_types.h"

// =============================================================================
//  MACROS
// =============================================================================

// =============================================================================
//  TYPES
// =============================================================================

// ============================================================================
// SPAL_PROFILE_CONTROL_T
// -----------------------------------------------------------------------------
/// Profile control for SPAL, SPC, and SPP.
// =============================================================================
typedef struct
{
    UINT32                         config;                       //0x00000000
    /// The start address of the buffer to save the profile data.
    UINT32*                        buffer_start;                 //0x00000004
    /// Next available word index in the buffer to save the profile data. One word
    /// is 4 bytes.
    UINT16                         buffer_pos;                   //0x00000008
    /// The buffer size in number of words. One word is 4 bytes.
    UINT16                         buffer_size;                  //0x0000000A
} SPAL_PROFILE_CONTROL_T; //Size : 0xC

//config
#define SPAL_PROFILE_CONTROL_SPAL   (1<<0)
#define SPAL_PROFILE_CONTROL_SPC    (1<<1)
#define SPAL_PROFILE_CONTROL_SPP    (1<<2)
#define SPAL_PROFILE_CONTROL_BB_IRQ (1<<3)
#define SPAL_PROFILE_CONTROL_BB_RX_WIN (1<<4)
#define SPAL_PROFILE_CONTROL_BB_IFC2_SYM_CNT (1<<5)



// ============================================================================
// SPAL_MAP_ACCESS_T
// -----------------------------------------------------------------------------
/// Type used to define the accessible structures of SPAL.
// =============================================================================
typedef struct
{
    SPAL_PROFILE_CONTROL_T*        profileControlPtr;            //0x00000000
} SPAL_MAP_ACCESS_T; //Size : 0x4





#endif

