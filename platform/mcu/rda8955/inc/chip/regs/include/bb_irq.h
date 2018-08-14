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


#ifndef _BB_IRQ_H_
#define _BB_IRQ_H_

#ifdef CT_ASM
#error "You are trying to use in an assembly code the normal H description of 'bb_irq'."
#endif


#include "globals.h"

// =============================================================================
//  MACROS
// =============================================================================

// ============================================================================
// BCPU_IRQ_LINES_T
// -----------------------------------------------------------------------------
///
// =============================================================================
typedef enum
{
/// BCPU Irq Lines
    BCPU_MAIN_IRQ_LINE                          = 0x00000000,
    BCPU_DEBUG_IRQ_LINE                         = 0x00000004,
    BCPU_HOST_IRQ_LINE                          = 0x00000005
} BCPU_IRQ_LINES_T;


// =============================================================================
//  TYPES
// =============================================================================

// ============================================================================
// BB_IRQ_T
// -----------------------------------------------------------------------------
///
// =============================================================================
#define REG_BB_IRQ_BASE            0x01902000

typedef volatile struct
{
    /// If cause is not null and interrupt are enabled then the interrupt line 0
    /// is driven on the system CPU.
    /// The cause for the Irq sources, one bit for each module's irq source.
    /// The cause is the actual Irq source masked by the mask register.
    REG32                          Cause;                        //0x00000000
    /// The status for the level Irq sources, one bit for each module's irq
    /// source.
    /// The status reflect the actual Irq source.
    REG32                          Status;                       //0x00000004
    /// Writing '1' sets the corresponding bit in the mask register to '1'.
    /// Reading gives the value of the mask register.
    REG32                          Mask_Set;                     //0x00000008
    /// Writing '1' clears the corresponding bit in the mask register to '0'.
    /// Reading gives the value of the mask register.
    REG32                          Mask_Clear;                   //0x0000000C
    REG32                          NonMaskable;                  //0x00000010
    REG32                          SC;                           //0x00000014
    /// Each bit to '1' in that registers allows the correcponding interrupt to
    /// wake up the System CPU (i.e.: Reenable it's clock, see CLOCK_BB_ENABLE
    /// and CLOCK_BB_DISABLE registers in general registers section)
    REG32                          WakeUp_Mask;                  //0x00000018
    REG32                          Cpu_Sleep;                    //0x0000001C
    /// Writing '1' sets the corresponding bit in the mask register to '1'.
    /// Reading gives the value of the mask register.
    REG32                          Pulse_Mask_Set;               //0x00000020
    /// Writing '1' clears the corresponding bit in the mask register to '0'.
    /// Reading gives the value of the mask register.
    REG32                          Pulse_Mask_Clr;               //0x00000024
    /// Writing '1' clears the corresponding Pulse IRQ.
    /// Pulse IRQ are set by the modules and cleared here.
    REG32                          Pulse_Clear;                  //0x00000028
    /// The status for the Pulse Irq sources, one bit for each module's irq
    /// source.
    /// The status reflect the actual Irq source.
    REG32                          Pulse_Status;                 //0x0000002C
} HWP_BB_IRQ_T;

#define hwp_bbIrq                  ((HWP_BB_IRQ_T*) KSEG1(REG_BB_IRQ_BASE))


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

