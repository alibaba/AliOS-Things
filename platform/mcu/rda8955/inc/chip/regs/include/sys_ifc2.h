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


#ifndef _SYS_IFC2_H_
#define _SYS_IFC2_H_

#ifdef CT_ASM
#error "You are trying to use in an assembly code the normal H description of 'sys_ifc2'."
#endif


#include "globals.h"

// =============================================================================
//  MACROS
// =============================================================================
#define SYS_IFC2_ADDR_LEN                       (15)
#define SYS_IFC2_ADDR_ALIGN                     (2)
#define SYS_IFC2_TC_LEN                         (8)

// =============================================================================
//  TYPES
// =============================================================================

// ============================================================================
// SYS_IFC2_T
// -----------------------------------------------------------------------------
///
// =============================================================================
#define REG_SYS_IFC2_BASE          0x01921000

typedef volatile struct
{
    /// The Channel 0 conveys data from the AIF to the memory.
    /// The Channel 1 conveys data from the memory to the AIF.
    /// These Channels only exist with Voice Option.
    struct
    {
        REG32                      control;                      //0x00000000
        REG32                      status;                       //0x00000004
        REG32                      start_addr;                   //0x00000008
        REG32                      Fifo_Size;                    //0x0000000C
        REG32 Reserved_00000010;                //0x00000010
        REG32                      int_mask;                     //0x00000014
        REG32                      int_clear;                    //0x00000018
        REG32                      cur_ahb_addr;                 //0x0000001C
    } ch[2];
} HWP_SYS_IFC2_T;

#define hwp_sysIfc2                ((HWP_SYS_IFC2_T*) KSEG1(REG_SYS_IFC2_BASE))


//control
#define SYS_IFC2_ENABLE            (1<<0)
#define SYS_IFC2_DISABLE           (1<<1)
#define SYS_IFC2_AUTO_DISABLE      (1<<4)

//status
//#define SYS_IFC2_ENABLE          (1<<0)
#define SYS_IFC2_FIFO_EMPTY        (1<<4)
#define SYS_IFC2_CAUSE_IEF         (1<<8)
#define SYS_IFC2_CAUSE_IHF         (1<<9)
#define SYS_IFC2_CAUSE_I4F         (1<<10)
#define SYS_IFC2_CAUSE_I3_4F       (1<<11)
#define SYS_IFC2_IEF               (1<<16)
#define SYS_IFC2_IHF               (1<<17)
#define SYS_IFC2_I4F               (1<<18)
#define SYS_IFC2_I3_4F             (1<<19)

//start_addr
#define SYS_IFC2_START_ADDR(n)     (((n)&0xFFFFFF)<<2)

//Fifo_Size
#define SYS_IFC2_FIFO_SIZE(n)      (((n)&0x7FF)<<4)

//int_mask
#define SYS_IFC2_END_FIFO          (1<<8)
#define SYS_IFC2_HALF_FIFO         (1<<9)
#define SYS_IFC2_QUARTER_FIFO      (1<<10)
#define SYS_IFC2_THREE_QUARTER_FIFO (1<<11)

//int_clear
//#define SYS_IFC2_END_FIFO        (1<<8)
//#define SYS_IFC2_HALF_FIFO       (1<<9)
//#define SYS_IFC2_QUARTER_FIFO    (1<<10)
//#define SYS_IFC2_THREE_QUARTER_FIFO (1<<11)

//cur_ahb_addr
#define SYS_IFC2_CUR_AHB_ADDR(n)   (((n)&0x3FFFFFF)<<0)





#endif

