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


#ifndef _COMREGS_ASM_H_
#define _COMREGS_ASM_H_

//THIS FILE HAS BEEN GENERATED WITH COOLWATCHER. PLEASE EDIT WITH CARE !

#ifndef CT_ASM
#error "You are trying to use in a normal C code the assembly H description of 'comregs'."
#endif



//==============================================================================
// comregs
//------------------------------------------------------------------------------
///
//==============================================================================
#define REG_SYS_COMREGS_BASE       0x01A0B000
#define REG_BB_COMREGS_BASE        0x0190E000

#define REG_SYS_COMREGS_BASE_HI    BASE_HI(REG_SYS_COMREGS_BASE)
#define REG_SYS_COMREGS_BASE_LO    BASE_LO(REG_SYS_COMREGS_BASE)

#define REG_SYS_COMREGS_SNAPSHOT   REG_SYS_COMREGS_BASE_LO + 0x00000000
#define REG_SYS_COMREGS_SNAPSHOT_CFG REG_SYS_COMREGS_BASE_LO + 0x00000004
#define REG_SYS_COMREGS_CAUSE      REG_SYS_COMREGS_BASE_LO + 0x00000008
#define REG_SYS_COMREGS_MASK_SET   REG_SYS_COMREGS_BASE_LO + 0x0000000C
#define REG_SYS_COMREGS_MASK_CLR   REG_SYS_COMREGS_BASE_LO + 0x00000010
#define REG_SYS_COMREGS_ITREG_SET  REG_SYS_COMREGS_BASE_LO + 0x00000014
#define REG_SYS_COMREGS_ITREG_CLR  REG_SYS_COMREGS_BASE_LO + 0x00000018

#define REG_BB_COMREGS_BASE_HI     BASE_HI(REG_BB_COMREGS_BASE)
#define REG_BB_COMREGS_BASE_LO     BASE_LO(REG_BB_COMREGS_BASE)

#define REG_BB_COMREGS_SNAPSHOT    REG_BB_COMREGS_BASE_LO + 0x00000000
#define REG_BB_COMREGS_SNAPSHOT_CFG REG_BB_COMREGS_BASE_LO + 0x00000004
#define REG_BB_COMREGS_CAUSE       REG_BB_COMREGS_BASE_LO + 0x00000008
#define REG_BB_COMREGS_MASK_SET    REG_BB_COMREGS_BASE_LO + 0x0000000C
#define REG_BB_COMREGS_MASK_CLR    REG_BB_COMREGS_BASE_LO + 0x00000010
#define REG_BB_COMREGS_ITREG_SET   REG_BB_COMREGS_BASE_LO + 0x00000014
#define REG_BB_COMREGS_ITREG_CLR   REG_BB_COMREGS_BASE_LO + 0x00000018

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
