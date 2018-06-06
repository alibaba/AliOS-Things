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


#ifndef _PAGE_SPY_H_
#define _PAGE_SPY_H_

#ifdef CT_ASM
#error "You are trying to use in an assembly code the normal H description of 'page_spy'."
#endif


#include "globals.h"

// =============================================================================
//  MACROS
// =============================================================================
#define PAGE_SPY_NB_PAGE                        (16)

// =============================================================================
//  TYPES
// =============================================================================

// ============================================================================
// PAGE_SPY_T
// -----------------------------------------------------------------------------
///
// =============================================================================
#define REG_PAGE_SPY_BASE          0x01A0C000

typedef volatile struct
{
    REG32                          enable;                       //0x00000000
    REG32                          status;                       //0x00000004
    REG32                          disable;                      //0x00000008
    REG32 Reserved_0000000C;                    //0x0000000C
    struct
    {
        REG32                      start;                        //0x00000010
        REG32                      end;                          //0x00000014
        REG32                      master;                       //0x00000018
        REG32                      addr;                         //0x0000001C
    } page[PAGE_SPY_NB_PAGE];
} HWP_PAGE_SPY_T;

#define hwp_pageSpy                ((HWP_PAGE_SPY_T*) KSEG1(REG_PAGE_SPY_BASE))


//enable
#define PAGE_SPY_ENABLE_PAGE_0     (1<<0)
#define PAGE_SPY_ENABLE_PAGE_1     (1<<1)
#define PAGE_SPY_ENABLE_PAGE_2     (1<<2)
#define PAGE_SPY_ENABLE_PAGE_3     (1<<3)
#define PAGE_SPY_ENABLE_PAGE_4     (1<<4)
#define PAGE_SPY_ENABLE_PAGE_5     (1<<5)
#define PAGE_SPY_ENABLE_PAGE_6     (1<<6)
#define PAGE_SPY_ENABLE_PAGE_7     (1<<7)
#define PAGE_SPY_ENABLE_PAGE_8     (1<<8)
#define PAGE_SPY_ENABLE_PAGE_9     (1<<9)
#define PAGE_SPY_ENABLE_PAGE_10    (1<<10)
#define PAGE_SPY_ENABLE_PAGE_11    (1<<11)
#define PAGE_SPY_ENABLE_PAGE_12    (1<<12)
#define PAGE_SPY_ENABLE_PAGE_13    (1<<13)
#define PAGE_SPY_ENABLE_PAGE_14    (1<<14)
#define PAGE_SPY_ENABLE_PAGE_15    (1<<15)
#define PAGE_SPY_ENABLE_PAGE(n)    (((n)&0xFFFF)<<0)
#define PAGE_SPY_ENABLE_PAGE_MASK  (0xFFFF<<0)
#define PAGE_SPY_ENABLE_PAGE_SHIFT (0)

//status
#define PAGE_SPY_STATUS_PAGE_0     (1<<0)
#define PAGE_SPY_STATUS_PAGE_0_NOT_HIT (0<<0)
#define PAGE_SPY_STATUS_PAGE_0_HIT (1<<0)
#define PAGE_SPY_STATUS_PAGE_1     (1<<1)
#define PAGE_SPY_STATUS_PAGE_1_NOT_HIT (0<<1)
#define PAGE_SPY_STATUS_PAGE_1_HIT (1<<1)
#define PAGE_SPY_STATUS_PAGE_2     (1<<2)
#define PAGE_SPY_STATUS_PAGE_2_NOT_HIT (0<<2)
#define PAGE_SPY_STATUS_PAGE_2_HIT (1<<2)
#define PAGE_SPY_STATUS_PAGE_3     (1<<3)
#define PAGE_SPY_STATUS_PAGE_3_NOT_HIT (0<<3)
#define PAGE_SPY_STATUS_PAGE_3_HIT (1<<3)
#define PAGE_SPY_STATUS_PAGE_4     (1<<4)
#define PAGE_SPY_STATUS_PAGE_4_NOT_HIT (0<<4)
#define PAGE_SPY_STATUS_PAGE_4_HIT (1<<4)
#define PAGE_SPY_STATUS_PAGE_5     (1<<5)
#define PAGE_SPY_STATUS_PAGE_5_NOT_HIT (0<<5)
#define PAGE_SPY_STATUS_PAGE_5_HIT (1<<5)
#define PAGE_SPY_STATUS_PAGE_6     (1<<6)
#define PAGE_SPY_STATUS_PAGE_6_NOT_HIT (0<<6)
#define PAGE_SPY_STATUS_PAGE_6_HIT (1<<6)
#define PAGE_SPY_STATUS_PAGE_7     (1<<7)
#define PAGE_SPY_STATUS_PAGE_7_NOT_HIT (0<<7)
#define PAGE_SPY_STATUS_PAGE_7_HIT (1<<7)
#define PAGE_SPY_STATUS_PAGE_8     (1<<8)
#define PAGE_SPY_STATUS_PAGE_8_NOT_HIT (0<<8)
#define PAGE_SPY_STATUS_PAGE_8_HIT (1<<8)
#define PAGE_SPY_STATUS_PAGE_9     (1<<9)
#define PAGE_SPY_STATUS_PAGE_9_NOT_HIT (0<<9)
#define PAGE_SPY_STATUS_PAGE_9_HIT (1<<9)
#define PAGE_SPY_STATUS_PAGE_10    (1<<10)
#define PAGE_SPY_STATUS_PAGE_10_NOT_HIT (0<<10)
#define PAGE_SPY_STATUS_PAGE_10_HIT (1<<10)
#define PAGE_SPY_STATUS_PAGE_11    (1<<11)
#define PAGE_SPY_STATUS_PAGE_11_NOT_HIT (0<<11)
#define PAGE_SPY_STATUS_PAGE_11_HIT (1<<11)
#define PAGE_SPY_STATUS_PAGE_12    (1<<12)
#define PAGE_SPY_STATUS_PAGE_12_NOT_HIT (0<<12)
#define PAGE_SPY_STATUS_PAGE_12_HIT (1<<12)
#define PAGE_SPY_STATUS_PAGE_13    (1<<13)
#define PAGE_SPY_STATUS_PAGE_13_NOT_HIT (0<<13)
#define PAGE_SPY_STATUS_PAGE_13_HIT (1<<13)
#define PAGE_SPY_STATUS_PAGE_14    (1<<14)
#define PAGE_SPY_STATUS_PAGE_14_NOT_HIT (0<<14)
#define PAGE_SPY_STATUS_PAGE_14_HIT (1<<14)
#define PAGE_SPY_STATUS_PAGE_15    (1<<15)
#define PAGE_SPY_STATUS_PAGE_15_NOT_HIT (0<<15)
#define PAGE_SPY_STATUS_PAGE_15_HIT (1<<15)
#define PAGE_SPY_STATUS_PAGE(n)    (((n)&0xFFFF)<<0)
#define PAGE_SPY_STATUS_PAGE_MASK  (0xFFFF<<0)
#define PAGE_SPY_STATUS_PAGE_SHIFT (0)

//disable
#define PAGE_SPY_DISABLE_PAGE_0    (1<<0)
#define PAGE_SPY_DISABLE_PAGE_1    (1<<1)
#define PAGE_SPY_DISABLE_PAGE_2    (1<<2)
#define PAGE_SPY_DISABLE_PAGE_3    (1<<3)
#define PAGE_SPY_DISABLE_PAGE_4    (1<<4)
#define PAGE_SPY_DISABLE_PAGE_5    (1<<5)
#define PAGE_SPY_DISABLE_PAGE_6    (1<<6)
#define PAGE_SPY_DISABLE_PAGE_7    (1<<7)
#define PAGE_SPY_DISABLE_PAGE_8    (1<<8)
#define PAGE_SPY_DISABLE_PAGE_9    (1<<9)
#define PAGE_SPY_DISABLE_PAGE_10   (1<<10)
#define PAGE_SPY_DISABLE_PAGE_11   (1<<11)
#define PAGE_SPY_DISABLE_PAGE_12   (1<<12)
#define PAGE_SPY_DISABLE_PAGE_13   (1<<13)
#define PAGE_SPY_DISABLE_PAGE_14   (1<<14)
#define PAGE_SPY_DISABLE_PAGE_15   (1<<15)
#define PAGE_SPY_DISABLE_PAGE(n)   (((n)&0xFFFF)<<0)
#define PAGE_SPY_DISABLE_PAGE_MASK (0xFFFF<<0)
#define PAGE_SPY_DISABLE_PAGE_SHIFT (0)

//start
#define PAGE_SPY_START_ADDRESS(n)  (((n)&0x3FFFFFF)<<0)
#define PAGE_SPY_DETECT_READ       (1<<30)
#define PAGE_SPY_DETECT_WRITE      (1<<31)
#define PAGE_SPY_MODE(n)           (((n)&3)<<30)
#define PAGE_SPY_MODE_MASK         (3<<30)
#define PAGE_SPY_MODE_SHIFT        (30)

//end
#define PAGE_SPY_END_ADDRESS(n)    (((n)&0x3FFFFFF)<<0)

//master
#define PAGE_SPY_MASTER(n)         (((n)&7)<<0)
#define PAGE_SPY_MASTER_NO_ACCESS  (0<<0)
#define PAGE_SPY_MASTER_BIST       (1<<0)
#define PAGE_SPY_MASTER_DMA        (2<<0)
#define PAGE_SPY_MASTER_SYS_XCPU   (3<<0)
#define PAGE_SPY_MASTER_AHB2AHB    (4<<0)
#define PAGE_SPY_MASTER_IFC        (5<<0)
#define PAGE_SPY_MASTER_USB        (6<<0)

//addr
#define PAGE_SPY_HIT_ADDR(n)       (((n)&0x1FFFFFF)<<0)
#define PAGE_SPY_HIT_READ          (1<<30)
#define PAGE_SPY_HIT_WRITE         (1<<31)





#endif

