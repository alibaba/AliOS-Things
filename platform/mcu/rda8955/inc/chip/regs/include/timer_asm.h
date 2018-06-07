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


#ifndef _TIMER_ASM_H_
#define _TIMER_ASM_H_

//THIS FILE HAS BEEN GENERATED WITH COOLWATCHER. PLEASE EDIT WITH CARE !

#ifndef CT_ASM
#error "You are trying to use in a normal C code the assembly H description of 'timer'."
#endif


#define NB_INTERVAL                             (1)
#define INT_TIMER_NB_BITS                       (24)
#define WD_TIMER_NB_BITS                        (24)
#define HW_TIMER_NB_BITS                        (32)
#define TIM_MAXVAL                              (0XFFFFFF)

//==============================================================================
// timer
//------------------------------------------------------------------------------
///
//==============================================================================
#define REG_TIMER_BASE             0x01A02000

#define REG_TIMER_BASE_HI          BASE_HI(REG_TIMER_BASE)
#define REG_TIMER_BASE_LO          BASE_LO(REG_TIMER_BASE)

#define REG_TIMER_OSTIMER_CTRL     REG_TIMER_BASE_LO + 0x00000000
#define REG_TIMER_OSTIMER_CURVAL   REG_TIMER_BASE_LO + 0x00000004
#define REG_TIMER_WDTIMER_CTRL     REG_TIMER_BASE_LO + 0x00000008
#define REG_TIMER_WDTIMER_LOADVAL  REG_TIMER_BASE_LO + 0x0000000C
#define REG_TIMER_HWTIMER_CTRL     REG_TIMER_BASE_LO + 0x00000010
#define REG_TIMER_HWTIMER_CURVAL   REG_TIMER_BASE_LO + 0x00000014
#define REG_TIMER_TIMER_IRQ_MASK_SET REG_TIMER_BASE_LO + 0x00000018
#define REG_TIMER_TIMER_IRQ_MASK_CLR REG_TIMER_BASE_LO + 0x0000001C
#define REG_TIMER_TIMER_IRQ_CLR    REG_TIMER_BASE_LO + 0x00000020
#define REG_TIMER_TIMER_IRQ_CAUSE  REG_TIMER_BASE_LO + 0x00000024

//OSTimer_Ctrl
#define TIMER_LOADVAL(n)           (((n)&0xFFFFFF)<<0)
#define TIMER_LOADVAL_MASK         (0xFFFFFF<<0)
#define TIMER_LOADVAL_SHIFT        (0)
#define TIMER_ENABLE               (1<<24)
#define TIMER_ENABLED              (1<<25)
#define TIMER_CLEARED              (1<<26)
#define TIMER_REPEAT               (1<<28)
#define TIMER_WRAP                 (1<<29)
#define TIMER_LOAD                 (1<<30)

//OSTimer_CurVal
#define TIMER_CURVAL(n)            (((n)&0xFFFFFFFF)<<0)

//WDTimer_Ctrl
#define TIMER_START                (1<<0)
#define TIMER_STOP                 (1<<4)
#define TIMER_RELOAD               (1<<16)
#define TIMER_WDENABLED            (1<<3)

//WDTimer_LoadVal
//#define TIMER_LOADVAL(n)         (((n)&0xFFFFFF)<<0)

//HWTimer_Ctrl
#define TIMER_INTERVAL_EN          (1<<8)
#define TIMER_INTERVAL(n)          (((n)&3)<<0)

//HWTimer_CurVal
//#define TIMER_CURVAL(n)          (((n)&0xFFFFFFFF)<<0)

//Timer_Irq_Mask_Set
#define TIMER_OSTIMER_MASK         (1<<0)
#define TIMER_HWTIMER_WRAP_MASK    (1<<1)
#define TIMER_HWTIMER_ITV_MASK     (1<<2)

//Timer_Irq_Mask_Clr
//#define TIMER_OSTIMER_MASK       (1<<0)
//#define TIMER_HWTIMER_WRAP_MASK  (1<<1)
//#define TIMER_HWTIMER_ITV_MASK   (1<<2)

//Timer_Irq_Clr
#define TIMER_OSTIMER_CLR          (1<<0)
#define TIMER_HWTIMER_WRAP_CLR     (1<<1)
#define TIMER_HWTIMER_ITV_CLR      (1<<2)

//Timer_Irq_Cause
#define TIMER_OSTIMER_CAUSE        (1<<0)
#define TIMER_HWTIMER_WRAP_CAUSE   (1<<1)
#define TIMER_HWTIMER_ITV_CAUSE    (1<<2)
#define TIMER_OSTIMER_STATUS       (1<<16)
#define TIMER_HWTIMER_WRAP_STATUS  (1<<17)
#define TIMER_HWTIMER_ITV_STATUS   (1<<18)
#define TIMER_OTHER_TIMS_IRQ(n)    (((n)&3)<<1)
#define TIMER_OTHER_TIMS_IRQ_MASK  (3<<1)
#define TIMER_OTHER_TIMS_IRQ_SHIFT (1)




#endif
