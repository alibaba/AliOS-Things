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


#ifndef _COMREGS_H_
#define _COMREGS_H_

#ifdef CT_ASM
#error "You are trying to use in an assembly code the normal H description of 'comregs'."
#endif



// =============================================================================
//  MACROS
// =============================================================================

// =============================================================================
//  TYPES
// =============================================================================

// ============================================================================
// COMREGS_T
// -----------------------------------------------------------------------------
///
// =============================================================================
#define REG_SYS_COMREGS_BASE       0x01A0B000
#define REG_BB_COMREGS_BASE        0x0190E000

typedef volatile struct
{
    REG32                          Snapshot;                     //0x00000000
    REG32                          Snapshot_Cfg;                 //0x00000004
    REG32                          Cause;                        //0x00000008
    REG32                          Mask_Set;                     //0x0000000C
    REG32                          Mask_Clr;                     //0x00000010
    /// If accesses to ItReg_Set and ItReg_Clr registers are done simultaneously
    /// from both CPUs and affecting the same bits, the priority is given to set
    /// a bit.
    REG32                          ItReg_Set;                    //0x00000014
    /// If accesses to ItReg_Set and ItReg_Clr registers are done simultaneously
    /// from both CPUs and affecting the same bits, the priority is given to set
    /// a bit.
    REG32                          ItReg_Clr;                    //0x00000018
} HWP_COMREGS_T;

#define hwp_sysComregs             ((HWP_COMREGS_T*) KSEG1(REG_SYS_COMREGS_BASE))
#define hwp_bbComregs              ((HWP_COMREGS_T*) KSEG1(REG_BB_COMREGS_BASE))


//Snapshot
#define COMREGS_SNAPSHOT(n)        (((n)&3)<<0)

//Snapshot_Cfg
#define COMREGS_SNAPSHOT_CFG       (1<<0)
#define COMREGS_SNAPSHOT_CFG_RD    (1<<1)

//Cause
#define COMREGS_IRQ0_CAUSE(n)      (((n)&0xFF)<<0)
#define COMREGS_IRQ0_CAUSE_MASK    (0xFF<<0)
#define COMREGS_IRQ0_CAUSE_SHIFT   (0)
#define COMREGS_IRQ1_CAUSE(n)      (((n)&0xFF)<<8)
#define COMREGS_IRQ1_CAUSE_MASK    (0xFF<<8)
#define COMREGS_IRQ1_CAUSE_SHIFT   (8)

//Mask_Set
#define COMREGS_IRQ0_MASK_SET(n)   (((n)&0xFF)<<0)
#define COMREGS_IRQ0_MASK_SET_MASK (0xFF<<0)
#define COMREGS_IRQ0_MASK_SET_SHIFT (0)
#define COMREGS_IRQ1_MASK_SET(n)   (((n)&0xFF)<<8)
#define COMREGS_IRQ1_MASK_SET_MASK (0xFF<<8)
#define COMREGS_IRQ1_MASK_SET_SHIFT (8)

//Mask_Clr
#define COMREGS_IRQ0_MASK_CLR(n)   (((n)&0xFF)<<0)
#define COMREGS_IRQ0_MASK_CLR_MASK (0xFF<<0)
#define COMREGS_IRQ0_MASK_CLR_SHIFT (0)
#define COMREGS_IRQ1_MASK_CLR(n)   (((n)&0xFF)<<8)
#define COMREGS_IRQ1_MASK_CLR_MASK (0xFF<<8)
#define COMREGS_IRQ1_MASK_CLR_SHIFT (8)

//ItReg_Set
#define COMREGS_IRQ0_SET(n)        (((n)&0xFF)<<0)
#define COMREGS_IRQ0_SET_MASK      (0xFF<<0)
#define COMREGS_IRQ0_SET_SHIFT     (0)
#define COMREGS_IRQ1_SET(n)        (((n)&0xFF)<<8)
#define COMREGS_IRQ1_SET_MASK      (0xFF<<8)
#define COMREGS_IRQ1_SET_SHIFT     (8)
#define COMREGS_IRQ(n)             (((n)&0xFFFF)<<0)
#define COMREGS_IRQ_MASK           (0xFFFF<<0)
#define COMREGS_IRQ_SHIFT          (0)

//ItReg_Clr
#define COMREGS_IRQ0_CLR(n)        (((n)&0xFF)<<0)
#define COMREGS_IRQ1_CLR(n)        (((n)&0xFF)<<8)





#endif

