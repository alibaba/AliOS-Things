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


#ifndef _SYS_IRQ_ASM_H_
#define _SYS_IRQ_ASM_H_

//THIS FILE HAS BEEN GENERATED WITH COOLWATCHER. PLEASE EDIT WITH CARE !

#ifndef CT_ASM
#error "You are trying to use in a normal C code the assembly H description of 'sys_irq'."
#endif

#include "globals_asm.h"

/// XCPU Irq Lines
#define XCPU_MAIN_IRQ_LINE                      (0)
#define XCPU_PAGE_SPY_IRQ_LINE                  (3)
#define XCPU_DEBUG_IRQ_LINE                     (4)
#define XCPU_HOST_IRQ_LINE                      (5)

//==============================================================================
// XCPU_Irq_Lines
//------------------------------------------------------------------------------
///
//==============================================================================



//==============================================================================
// sys_irq
//------------------------------------------------------------------------------
///
//==============================================================================
#define REG_SYS_IRQ_BASE           0x01A01000

#define REG_SYS_IRQ_BASE_HI        BASE_HI(REG_SYS_IRQ_BASE)
#define REG_SYS_IRQ_BASE_LO        BASE_LO(REG_SYS_IRQ_BASE)

#define REG_SYS_IRQ_CAUSE          REG_SYS_IRQ_BASE_LO + 0x00000000
#define REG_SYS_IRQ_STATUS         REG_SYS_IRQ_BASE_LO + 0x00000004
#define REG_SYS_IRQ_MASK_SET       REG_SYS_IRQ_BASE_LO + 0x00000008
#define REG_SYS_IRQ_MASK_CLEAR     REG_SYS_IRQ_BASE_LO + 0x0000000C
#define REG_SYS_IRQ_NONMASKABLE    REG_SYS_IRQ_BASE_LO + 0x00000010
#define REG_SYS_IRQ_SC             REG_SYS_IRQ_BASE_LO + 0x00000014
#define REG_SYS_IRQ_WAKEUP_MASK    REG_SYS_IRQ_BASE_LO + 0x00000018
#define REG_SYS_IRQ_CPU_SLEEP      REG_SYS_IRQ_BASE_LO + 0x0000001C
#define REG_SYS_IRQ_PULSE_MASK_SET REG_SYS_IRQ_BASE_LO + 0x00000020
#define REG_SYS_IRQ_PULSE_MASK_CLR REG_SYS_IRQ_BASE_LO + 0x00000024
#define REG_SYS_IRQ_PULSE_CLEAR    REG_SYS_IRQ_BASE_LO + 0x00000028
#define REG_SYS_IRQ_PULSE_STATUS   REG_SYS_IRQ_BASE_LO + 0x0000002C

//Cause
#define SYS_IRQ_SYS_IRQ_TCU0       (1<<0)
#define SYS_IRQ_SYS_IRQ_TCU1       (1<<1)
#define SYS_IRQ_SYS_IRQ_FRAME      (1<<2)
#define SYS_IRQ_SYS_IRQ_COM0       (1<<3)
#define SYS_IRQ_SYS_IRQ_COM1       (1<<4)
#define SYS_IRQ_SYS_IRQ_VOC        (1<<5)
#define SYS_IRQ_SYS_IRQ_DMA        (1<<6)
#define SYS_IRQ_SYS_IRQ_GPIO       (1<<7)
#define SYS_IRQ_SYS_IRQ_KEYPAD     (1<<8)
#define SYS_IRQ_SYS_IRQ_TIMERS     (1<<9)
#define SYS_IRQ_SYS_IRQ_OS_TIMER   (1<<10)
#define SYS_IRQ_SYS_IRQ_CALENDAR   (1<<11)
#define SYS_IRQ_SYS_IRQ_SPI1       (1<<12)
#define SYS_IRQ_SYS_IRQ_SPI2       (1<<13)
#define SYS_IRQ_SYS_IRQ_SPI3       (1<<14)
#define SYS_IRQ_SYS_IRQ_DEBUG_UART (1<<15)
#define SYS_IRQ_SYS_IRQ_UART       (1<<16)
#define SYS_IRQ_SYS_IRQ_UART2      (1<<17)
#define SYS_IRQ_SYS_IRQ_I2C        (1<<18)
#define SYS_IRQ_SYS_IRQ_I2C2       (1<<19)
#define SYS_IRQ_SYS_IRQ_I2C3       (1<<20)
#define SYS_IRQ_SYS_IRQ_SCI        (1<<21)
#define SYS_IRQ_SYS_IRQ_RF_SPI     (1<<22)
#define SYS_IRQ_SYS_IRQ_LPS        (1<<23)
#define SYS_IRQ_SYS_IRQ_BBIFC0     (1<<24)
#define SYS_IRQ_SYS_IRQ_BBIFC1     (1<<25)
#define SYS_IRQ_SYS_IRQ_USBC       (1<<26)
#define SYS_IRQ_SYS_IRQ_GOUDA      (1<<27)
#define SYS_IRQ_SYS_IRQ_SDMMC      (1<<28)
#define SYS_IRQ_SYS_IRQ_CAMERA     (1<<29)
#define SYS_IRQ_SYS_IRQ_PMU        (1<<30)
#define SYS_IRQ_SYS_IRQ_SDMMC2     (1<<31)
#define SYS_IRQ_CAUSE(n)           (((n)&0xFFFFFFFF)<<0)
#define SYS_IRQ_CAUSE_MASK         (0xFFFFFFFF<<0)
#define SYS_IRQ_CAUSE_SHIFT        (0)

//Status
//#define SYS_IRQ_SYS_IRQ_TCU0     (1<<0)
//#define SYS_IRQ_SYS_IRQ_TCU1     (1<<1)
//#define SYS_IRQ_SYS_IRQ_FRAME    (1<<2)
//#define SYS_IRQ_SYS_IRQ_COM0     (1<<3)
//#define SYS_IRQ_SYS_IRQ_COM1     (1<<4)
//#define SYS_IRQ_SYS_IRQ_VOC      (1<<5)
//#define SYS_IRQ_SYS_IRQ_DMA      (1<<6)
//#define SYS_IRQ_SYS_IRQ_GPIO     (1<<7)
//#define SYS_IRQ_SYS_IRQ_KEYPAD   (1<<8)
//#define SYS_IRQ_SYS_IRQ_TIMERS   (1<<9)
//#define SYS_IRQ_SYS_IRQ_OS_TIMER (1<<10)
//#define SYS_IRQ_SYS_IRQ_CALENDAR (1<<11)
//#define SYS_IRQ_SYS_IRQ_SPI1     (1<<12)
//#define SYS_IRQ_SYS_IRQ_SPI2     (1<<13)
//#define SYS_IRQ_SYS_IRQ_SPI3     (1<<14)
//#define SYS_IRQ_SYS_IRQ_DEBUG_UART (1<<15)
//#define SYS_IRQ_SYS_IRQ_UART     (1<<16)
//#define SYS_IRQ_SYS_IRQ_UART2    (1<<17)
//#define SYS_IRQ_SYS_IRQ_I2C      (1<<18)
//#define SYS_IRQ_SYS_IRQ_I2C2     (1<<19)
//#define SYS_IRQ_SYS_IRQ_I2C3     (1<<20)
//#define SYS_IRQ_SYS_IRQ_SCI      (1<<21)
//#define SYS_IRQ_SYS_IRQ_RF_SPI   (1<<22)
//#define SYS_IRQ_SYS_IRQ_LPS      (1<<23)
//#define SYS_IRQ_SYS_IRQ_BBIFC0   (1<<24)
//#define SYS_IRQ_SYS_IRQ_BBIFC1   (1<<25)
//#define SYS_IRQ_SYS_IRQ_USBC     (1<<26)
//#define SYS_IRQ_SYS_IRQ_GOUDA    (1<<27)
//#define SYS_IRQ_SYS_IRQ_SDMMC    (1<<28)
//#define SYS_IRQ_SYS_IRQ_CAMERA   (1<<29)
//#define SYS_IRQ_SYS_IRQ_PMU      (1<<30)
//#define SYS_IRQ_SYS_IRQ_SDMMC2   (1<<31)
#define SYS_IRQ_STATUS(n)          (((n)&0xFFFFFFFF)<<0)
#define SYS_IRQ_STATUS_MASK        (0xFFFFFFFF<<0)
#define SYS_IRQ_STATUS_SHIFT       (0)

//Mask_Set
//#define SYS_IRQ_SYS_IRQ_TCU0     (1<<0)
//#define SYS_IRQ_SYS_IRQ_TCU1     (1<<1)
//#define SYS_IRQ_SYS_IRQ_FRAME    (1<<2)
//#define SYS_IRQ_SYS_IRQ_COM0     (1<<3)
//#define SYS_IRQ_SYS_IRQ_COM1     (1<<4)
//#define SYS_IRQ_SYS_IRQ_VOC      (1<<5)
//#define SYS_IRQ_SYS_IRQ_DMA      (1<<6)
//#define SYS_IRQ_SYS_IRQ_GPIO     (1<<7)
//#define SYS_IRQ_SYS_IRQ_KEYPAD   (1<<8)
//#define SYS_IRQ_SYS_IRQ_TIMERS   (1<<9)
//#define SYS_IRQ_SYS_IRQ_OS_TIMER (1<<10)
//#define SYS_IRQ_SYS_IRQ_CALENDAR (1<<11)
//#define SYS_IRQ_SYS_IRQ_SPI1     (1<<12)
//#define SYS_IRQ_SYS_IRQ_SPI2     (1<<13)
//#define SYS_IRQ_SYS_IRQ_SPI3     (1<<14)
//#define SYS_IRQ_SYS_IRQ_DEBUG_UART (1<<15)
//#define SYS_IRQ_SYS_IRQ_UART     (1<<16)
//#define SYS_IRQ_SYS_IRQ_UART2    (1<<17)
//#define SYS_IRQ_SYS_IRQ_I2C      (1<<18)
//#define SYS_IRQ_SYS_IRQ_I2C2     (1<<19)
//#define SYS_IRQ_SYS_IRQ_I2C3     (1<<20)
//#define SYS_IRQ_SYS_IRQ_SCI      (1<<21)
//#define SYS_IRQ_SYS_IRQ_RF_SPI   (1<<22)
//#define SYS_IRQ_SYS_IRQ_LPS      (1<<23)
//#define SYS_IRQ_SYS_IRQ_BBIFC0   (1<<24)
//#define SYS_IRQ_SYS_IRQ_BBIFC1   (1<<25)
//#define SYS_IRQ_SYS_IRQ_USBC     (1<<26)
//#define SYS_IRQ_SYS_IRQ_GOUDA    (1<<27)
//#define SYS_IRQ_SYS_IRQ_SDMMC    (1<<28)
//#define SYS_IRQ_SYS_IRQ_CAMERA   (1<<29)
//#define SYS_IRQ_SYS_IRQ_PMU      (1<<30)
//#define SYS_IRQ_SYS_IRQ_SDMMC2   (1<<31)
#define SYS_IRQ_MASK_SET(n)        (((n)&0xFFFFFFFF)<<0)
#define SYS_IRQ_MASK_SET_MASK      (0xFFFFFFFF<<0)
#define SYS_IRQ_MASK_SET_SHIFT     (0)

//Mask_Clear
//#define SYS_IRQ_SYS_IRQ_TCU0     (1<<0)
//#define SYS_IRQ_SYS_IRQ_TCU1     (1<<1)
//#define SYS_IRQ_SYS_IRQ_FRAME    (1<<2)
//#define SYS_IRQ_SYS_IRQ_COM0     (1<<3)
//#define SYS_IRQ_SYS_IRQ_COM1     (1<<4)
//#define SYS_IRQ_SYS_IRQ_VOC      (1<<5)
//#define SYS_IRQ_SYS_IRQ_DMA      (1<<6)
//#define SYS_IRQ_SYS_IRQ_GPIO     (1<<7)
//#define SYS_IRQ_SYS_IRQ_KEYPAD   (1<<8)
//#define SYS_IRQ_SYS_IRQ_TIMERS   (1<<9)
//#define SYS_IRQ_SYS_IRQ_OS_TIMER (1<<10)
//#define SYS_IRQ_SYS_IRQ_CALENDAR (1<<11)
//#define SYS_IRQ_SYS_IRQ_SPI1     (1<<12)
//#define SYS_IRQ_SYS_IRQ_SPI2     (1<<13)
//#define SYS_IRQ_SYS_IRQ_SPI3     (1<<14)
//#define SYS_IRQ_SYS_IRQ_DEBUG_UART (1<<15)
//#define SYS_IRQ_SYS_IRQ_UART     (1<<16)
//#define SYS_IRQ_SYS_IRQ_UART2    (1<<17)
//#define SYS_IRQ_SYS_IRQ_I2C      (1<<18)
//#define SYS_IRQ_SYS_IRQ_I2C2     (1<<19)
//#define SYS_IRQ_SYS_IRQ_I2C3     (1<<20)
//#define SYS_IRQ_SYS_IRQ_SCI      (1<<21)
//#define SYS_IRQ_SYS_IRQ_RF_SPI   (1<<22)
//#define SYS_IRQ_SYS_IRQ_LPS      (1<<23)
//#define SYS_IRQ_SYS_IRQ_BBIFC0   (1<<24)
//#define SYS_IRQ_SYS_IRQ_BBIFC1   (1<<25)
//#define SYS_IRQ_SYS_IRQ_USBC     (1<<26)
//#define SYS_IRQ_SYS_IRQ_GOUDA    (1<<27)
//#define SYS_IRQ_SYS_IRQ_SDMMC    (1<<28)
//#define SYS_IRQ_SYS_IRQ_CAMERA   (1<<29)
//#define SYS_IRQ_SYS_IRQ_PMU      (1<<30)
//#define SYS_IRQ_SYS_IRQ_SDMMC2   (1<<31)
#define SYS_IRQ_MASK_CLR(n)        (((n)&0xFFFFFFFF)<<0)
#define SYS_IRQ_MASK_CLR_MASK      (0xFFFFFFFF<<0)
#define SYS_IRQ_MASK_CLR_SHIFT     (0)

//NonMaskable
#define SYS_IRQ_MAIN_IRQ           (1<<10)
#define SYS_IRQ_PAGE_SPY_IRQ       (1<<13)
#define SYS_IRQ_DEBUG_IRQ          (1<<14)
#define SYS_IRQ_HOST_IRQ           (1<<15)
#define SYS_IRQ_INTENABLE_STATUS   (1<<31)

//SC
#define SYS_IRQ_INTENABLE          (1<<0)

//WakeUp_Mask
//#define SYS_IRQ_SYS_IRQ_TCU0     (1<<0)
//#define SYS_IRQ_SYS_IRQ_TCU1     (1<<1)
//#define SYS_IRQ_SYS_IRQ_FRAME    (1<<2)
//#define SYS_IRQ_SYS_IRQ_COM0     (1<<3)
//#define SYS_IRQ_SYS_IRQ_COM1     (1<<4)
//#define SYS_IRQ_SYS_IRQ_VOC      (1<<5)
//#define SYS_IRQ_SYS_IRQ_DMA      (1<<6)
//#define SYS_IRQ_SYS_IRQ_GPIO     (1<<7)
//#define SYS_IRQ_SYS_IRQ_KEYPAD   (1<<8)
//#define SYS_IRQ_SYS_IRQ_TIMERS   (1<<9)
//#define SYS_IRQ_SYS_IRQ_OS_TIMER (1<<10)
//#define SYS_IRQ_SYS_IRQ_CALENDAR (1<<11)
//#define SYS_IRQ_SYS_IRQ_SPI1     (1<<12)
//#define SYS_IRQ_SYS_IRQ_SPI2     (1<<13)
//#define SYS_IRQ_SYS_IRQ_SPI3     (1<<14)
//#define SYS_IRQ_SYS_IRQ_DEBUG_UART (1<<15)
//#define SYS_IRQ_SYS_IRQ_UART     (1<<16)
//#define SYS_IRQ_SYS_IRQ_UART2    (1<<17)
//#define SYS_IRQ_SYS_IRQ_I2C      (1<<18)
//#define SYS_IRQ_SYS_IRQ_I2C2     (1<<19)
//#define SYS_IRQ_SYS_IRQ_I2C3     (1<<20)
//#define SYS_IRQ_SYS_IRQ_SCI      (1<<21)
//#define SYS_IRQ_SYS_IRQ_RF_SPI   (1<<22)
//#define SYS_IRQ_SYS_IRQ_LPS      (1<<23)
//#define SYS_IRQ_SYS_IRQ_BBIFC0   (1<<24)
//#define SYS_IRQ_SYS_IRQ_BBIFC1   (1<<25)
//#define SYS_IRQ_SYS_IRQ_USBC     (1<<26)
//#define SYS_IRQ_SYS_IRQ_GOUDA    (1<<27)
//#define SYS_IRQ_SYS_IRQ_SDMMC    (1<<28)
//#define SYS_IRQ_SYS_IRQ_CAMERA   (1<<29)
//#define SYS_IRQ_SYS_IRQ_PMU      (1<<30)
//#define SYS_IRQ_SYS_IRQ_SDMMC2   (1<<31)
#define SYS_IRQ_WAKEUP_MASK(n)     (((n)&0xFFFFFFFF)<<0)
#define SYS_IRQ_WAKEUP_MASK_MASK   (0xFFFFFFFF<<0)
#define SYS_IRQ_WAKEUP_MASK_SHIFT  (0)

//Cpu_Sleep
#define SYS_IRQ_SLEEP              (1<<0)

//Pulse_Mask_Set
//#define SYS_IRQ_SYS_IRQ_TCU0     (1<<0)
//#define SYS_IRQ_SYS_IRQ_TCU1     (1<<1)
//#define SYS_IRQ_SYS_IRQ_FRAME    (1<<2)
#define SYS_IRQ_PULSE_MASK_SET(n)  (((n)&7)<<0)
#define SYS_IRQ_PULSE_MASK_SET_MASK (7<<0)
#define SYS_IRQ_PULSE_MASK_SET_SHIFT (0)

//Pulse_Mask_Clr
//#define SYS_IRQ_SYS_IRQ_TCU0     (1<<0)
//#define SYS_IRQ_SYS_IRQ_TCU1     (1<<1)
//#define SYS_IRQ_SYS_IRQ_FRAME    (1<<2)
#define SYS_IRQ_PULSE_MASK_CLR(n)  (((n)&7)<<0)
#define SYS_IRQ_PULSE_MASK_CLR_MASK (7<<0)
#define SYS_IRQ_PULSE_MASK_CLR_SHIFT (0)

//Pulse_Clear
//#define SYS_IRQ_SYS_IRQ_TCU0     (1<<0)
//#define SYS_IRQ_SYS_IRQ_TCU1     (1<<1)
//#define SYS_IRQ_SYS_IRQ_FRAME    (1<<2)
#define SYS_IRQ_PULSE_CLR(n)       (((n)&7)<<0)
#define SYS_IRQ_PULSE_CLR_MASK     (7<<0)
#define SYS_IRQ_PULSE_CLR_SHIFT    (0)

//Pulse_Status
//#define SYS_IRQ_SYS_IRQ_TCU0     (1<<0)
//#define SYS_IRQ_SYS_IRQ_TCU1     (1<<1)
//#define SYS_IRQ_SYS_IRQ_FRAME    (1<<2)
#define SYS_IRQ_PULSE_STATUS(n)    (((n)&7)<<0)
#define SYS_IRQ_PULSE_STATUS_MASK  (7<<0)
#define SYS_IRQ_PULSE_STATUS_SHIFT (0)




#endif
