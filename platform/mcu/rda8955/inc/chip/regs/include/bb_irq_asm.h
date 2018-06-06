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


#ifndef _BB_IRQ_ASM_H_
#define _BB_IRQ_ASM_H_

//THIS FILE HAS BEEN GENERATED WITH COOLWATCHER. PLEASE EDIT WITH CARE !

#ifndef CT_ASM
#error "You are trying to use in a normal C code the assembly H description of 'bb_irq'."
#endif

#include "globals_asm.h"

/// BCPU Irq Lines
#define BCPU_MAIN_IRQ_LINE                      (0)
#define BCPU_DEBUG_IRQ_LINE                     (4)
#define BCPU_HOST_IRQ_LINE                      (5)

//==============================================================================
// BCPU_Irq_Lines
//------------------------------------------------------------------------------
///
//==============================================================================



//==============================================================================
// bb_irq
//------------------------------------------------------------------------------
///
//==============================================================================
#define REG_BB_IRQ_BASE            0x01902000

#define REG_BB_IRQ_BASE_HI         BASE_HI(REG_BB_IRQ_BASE)
#define REG_BB_IRQ_BASE_LO         BASE_LO(REG_BB_IRQ_BASE)

#define REG_BB_IRQ_CAUSE           REG_BB_IRQ_BASE_LO + 0x00000000
#define REG_BB_IRQ_STATUS          REG_BB_IRQ_BASE_LO + 0x00000004
#define REG_BB_IRQ_MASK_SET        REG_BB_IRQ_BASE_LO + 0x00000008
#define REG_BB_IRQ_MASK_CLEAR      REG_BB_IRQ_BASE_LO + 0x0000000C
#define REG_BB_IRQ_NONMASKABLE     REG_BB_IRQ_BASE_LO + 0x00000010
#define REG_BB_IRQ_SC              REG_BB_IRQ_BASE_LO + 0x00000014
#define REG_BB_IRQ_WAKEUP_MASK     REG_BB_IRQ_BASE_LO + 0x00000018
#define REG_BB_IRQ_CPU_SLEEP       REG_BB_IRQ_BASE_LO + 0x0000001C
#define REG_BB_IRQ_PULSE_MASK_SET  REG_BB_IRQ_BASE_LO + 0x00000020
#define REG_BB_IRQ_PULSE_MASK_CLR  REG_BB_IRQ_BASE_LO + 0x00000024
#define REG_BB_IRQ_PULSE_CLEAR     REG_BB_IRQ_BASE_LO + 0x00000028
#define REG_BB_IRQ_PULSE_STATUS    REG_BB_IRQ_BASE_LO + 0x0000002C

//Cause
#define BB_IRQ_BB_IRQ_TCU0         (1<<0)
#define BB_IRQ_BB_IRQ_TCU1         (1<<1)
#define BB_IRQ_BB_IRQ_FRAME        (1<<2)
#define BB_IRQ_BB_IRQ_COM0         (1<<3)
#define BB_IRQ_BB_IRQ_COM1         (1<<4)
#define BB_IRQ_BB_IRQ_VOC          (1<<5)
#define BB_IRQ_BB_IRQ_IFC0         (1<<6)
#define BB_IRQ_BB_IRQ_IFC1         (1<<7)
#define BB_IRQ_BB_IRQ_IFC2         (1<<8)
#define BB_IRQ_BB_IRQ_IFC3         (1<<9)
#define BB_IRQ_BB_IRQ_RF_IF        (1<<10)
#define BB_IRQ_BB_IRQ_ITLV         (1<<11)
#define BB_IRQ_BB_IRQ_VITAC        (1<<12)
#define BB_IRQ_BB_IRQ_XCOR         (1<<13)
#define BB_IRQ_CAUSE(n)            (((n)&0x3FFF)<<0)
#define BB_IRQ_CAUSE_MASK          (0x3FFF<<0)
#define BB_IRQ_CAUSE_SHIFT         (0)

//Status
//#define BB_IRQ_BB_IRQ_TCU0       (1<<0)
//#define BB_IRQ_BB_IRQ_TCU1       (1<<1)
//#define BB_IRQ_BB_IRQ_FRAME      (1<<2)
//#define BB_IRQ_BB_IRQ_COM0       (1<<3)
//#define BB_IRQ_BB_IRQ_COM1       (1<<4)
//#define BB_IRQ_BB_IRQ_VOC        (1<<5)
//#define BB_IRQ_BB_IRQ_IFC0       (1<<6)
//#define BB_IRQ_BB_IRQ_IFC1       (1<<7)
//#define BB_IRQ_BB_IRQ_IFC2       (1<<8)
//#define BB_IRQ_BB_IRQ_IFC3       (1<<9)
//#define BB_IRQ_BB_IRQ_RF_IF      (1<<10)
//#define BB_IRQ_BB_IRQ_ITLV       (1<<11)
//#define BB_IRQ_BB_IRQ_VITAC      (1<<12)
//#define BB_IRQ_BB_IRQ_XCOR       (1<<13)
#define BB_IRQ_STATUS(n)           (((n)&0x3FFF)<<0)
#define BB_IRQ_STATUS_MASK         (0x3FFF<<0)
#define BB_IRQ_STATUS_SHIFT        (0)

//Mask_Set
//#define BB_IRQ_BB_IRQ_TCU0       (1<<0)
//#define BB_IRQ_BB_IRQ_TCU1       (1<<1)
//#define BB_IRQ_BB_IRQ_FRAME      (1<<2)
//#define BB_IRQ_BB_IRQ_COM0       (1<<3)
//#define BB_IRQ_BB_IRQ_COM1       (1<<4)
//#define BB_IRQ_BB_IRQ_VOC        (1<<5)
//#define BB_IRQ_BB_IRQ_IFC0       (1<<6)
//#define BB_IRQ_BB_IRQ_IFC1       (1<<7)
//#define BB_IRQ_BB_IRQ_IFC2       (1<<8)
//#define BB_IRQ_BB_IRQ_IFC3       (1<<9)
//#define BB_IRQ_BB_IRQ_RF_IF      (1<<10)
//#define BB_IRQ_BB_IRQ_ITLV       (1<<11)
//#define BB_IRQ_BB_IRQ_VITAC      (1<<12)
//#define BB_IRQ_BB_IRQ_XCOR       (1<<13)
#define BB_IRQ_MASK_SET(n)         (((n)&0x3FFF)<<0)
#define BB_IRQ_MASK_SET_MASK       (0x3FFF<<0)
#define BB_IRQ_MASK_SET_SHIFT      (0)

//Mask_Clear
//#define BB_IRQ_BB_IRQ_TCU0       (1<<0)
//#define BB_IRQ_BB_IRQ_TCU1       (1<<1)
//#define BB_IRQ_BB_IRQ_FRAME      (1<<2)
//#define BB_IRQ_BB_IRQ_COM0       (1<<3)
//#define BB_IRQ_BB_IRQ_COM1       (1<<4)
//#define BB_IRQ_BB_IRQ_VOC        (1<<5)
//#define BB_IRQ_BB_IRQ_IFC0       (1<<6)
//#define BB_IRQ_BB_IRQ_IFC1       (1<<7)
//#define BB_IRQ_BB_IRQ_IFC2       (1<<8)
//#define BB_IRQ_BB_IRQ_IFC3       (1<<9)
//#define BB_IRQ_BB_IRQ_RF_IF      (1<<10)
//#define BB_IRQ_BB_IRQ_ITLV       (1<<11)
//#define BB_IRQ_BB_IRQ_VITAC      (1<<12)
//#define BB_IRQ_BB_IRQ_XCOR       (1<<13)
#define BB_IRQ_MASK_CLR(n)         (((n)&0x3FFF)<<0)
#define BB_IRQ_MASK_CLR_MASK       (0x3FFF<<0)
#define BB_IRQ_MASK_CLR_SHIFT      (0)

//NonMaskable
#define BB_IRQ_MAIN_IRQ            (1<<10)
#define BB_IRQ_DEBUG_IRQ           (1<<14)
#define BB_IRQ_HOST_IRQ            (1<<15)
#define BB_IRQ_INTENABLE_STATUS    (1<<31)

//SC
#define BB_IRQ_INTENABLE           (1<<0)

//WakeUp_Mask
//#define BB_IRQ_BB_IRQ_TCU0       (1<<0)
//#define BB_IRQ_BB_IRQ_TCU1       (1<<1)
//#define BB_IRQ_BB_IRQ_FRAME      (1<<2)
//#define BB_IRQ_BB_IRQ_COM0       (1<<3)
//#define BB_IRQ_BB_IRQ_COM1       (1<<4)
//#define BB_IRQ_BB_IRQ_VOC        (1<<5)
//#define BB_IRQ_BB_IRQ_IFC0       (1<<6)
//#define BB_IRQ_BB_IRQ_IFC1       (1<<7)
//#define BB_IRQ_BB_IRQ_IFC2       (1<<8)
//#define BB_IRQ_BB_IRQ_IFC3       (1<<9)
//#define BB_IRQ_BB_IRQ_RF_IF      (1<<10)
//#define BB_IRQ_BB_IRQ_ITLV       (1<<11)
//#define BB_IRQ_BB_IRQ_VITAC      (1<<12)
//#define BB_IRQ_BB_IRQ_XCOR       (1<<13)
#define BB_IRQ_WAKEUP_MASK(n)      (((n)&0x3FFF)<<0)
#define BB_IRQ_WAKEUP_MASK_MASK    (0x3FFF<<0)
#define BB_IRQ_WAKEUP_MASK_SHIFT   (0)

//Cpu_Sleep
#define BB_IRQ_SLEEP               (1<<0)

//Pulse_Mask_Set
//#define BB_IRQ_BB_IRQ_TCU0       (1<<0)
//#define BB_IRQ_BB_IRQ_TCU1       (1<<1)
//#define BB_IRQ_BB_IRQ_FRAME      (1<<2)
#define BB_IRQ_PULSE_MASK_SET(n)   (((n)&7)<<0)
#define BB_IRQ_PULSE_MASK_SET_MASK (7<<0)
#define BB_IRQ_PULSE_MASK_SET_SHIFT (0)

//Pulse_Mask_Clr
//#define BB_IRQ_BB_IRQ_TCU0       (1<<0)
//#define BB_IRQ_BB_IRQ_TCU1       (1<<1)
//#define BB_IRQ_BB_IRQ_FRAME      (1<<2)
#define BB_IRQ_PULSE_MASK_CLR(n)   (((n)&7)<<0)
#define BB_IRQ_PULSE_MASK_CLR_MASK (7<<0)
#define BB_IRQ_PULSE_MASK_CLR_SHIFT (0)

//Pulse_Clear
//#define BB_IRQ_BB_IRQ_TCU0       (1<<0)
//#define BB_IRQ_BB_IRQ_TCU1       (1<<1)
//#define BB_IRQ_BB_IRQ_FRAME      (1<<2)
#define BB_IRQ_PULSE_CLR(n)        (((n)&7)<<0)
#define BB_IRQ_PULSE_CLR_MASK      (7<<0)
#define BB_IRQ_PULSE_CLR_SHIFT     (0)

//Pulse_Status
//#define BB_IRQ_BB_IRQ_TCU0       (1<<0)
//#define BB_IRQ_BB_IRQ_TCU1       (1<<1)
//#define BB_IRQ_BB_IRQ_FRAME      (1<<2)
#define BB_IRQ_PULSE_STATUS(n)     (((n)&7)<<0)
#define BB_IRQ_PULSE_STATUS_MASK   (7<<0)
#define BB_IRQ_PULSE_STATUS_SHIFT  (0)




#endif
