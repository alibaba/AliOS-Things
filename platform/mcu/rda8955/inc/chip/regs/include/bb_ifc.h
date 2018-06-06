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


#ifndef _BB_IFC_H_
#define _BB_IFC_H_

#ifdef CT_ASM
#error "You are trying to use in an assembly code the normal H description of 'bb_ifc'."
#endif


#include "globals.h"

// =============================================================================
//  MACROS
// =============================================================================
#define BB_IFC_ADDR_LEN                         (15)
#define BB_IFC_ADDR_ALIGN                       (2)
#define BB_IFC_TC_LEN_CH2                       (9)
#define BB_IFC_TC_LEN                           (8)

// =============================================================================
//  TYPES
// =============================================================================

// ============================================================================
// BB_IFC_T
// -----------------------------------------------------------------------------
///
// =============================================================================
#define REG_BB_IFC_BASE            0x01901000

typedef volatile struct
{
    REG32 Reserved_00000000[16];                //0x00000000
    REG32                          ch2_control;                  //0x00000040
    REG32                          ch2_status;                   //0x00000044
    REG32                          ch2_start_addr;               //0x00000048
    REG32                          ch2_end_addr;                 //0x0000004C
    REG32                          ch2_tc;                       //0x00000050
    REG32                          ch2_int_mask;                 //0x00000054
    REG32                          ch2_int_clear;                //0x00000058
    REG32                          ch2_cur_ahb_addr;             //0x0000005C
    REG32                          ch3_control;                  //0x00000060
    REG32                          ch3_status;                   //0x00000064
    REG32                          ch3_start_addr;               //0x00000068
    REG32 Reserved_0000006C;                    //0x0000006C
    REG32                          ch3_tc;                       //0x00000070
    REG32                          ch3_int_mask;                 //0x00000074
    REG32                          ch3_int_clear;                //0x00000078
    REG32                          ch3_cur_ahb_addr;             //0x0000007C
} HWP_BB_IFC_T;

#define hwp_bbIfc                  ((HWP_BB_IFC_T*) KSEG1(REG_BB_IFC_BASE))


//ch2_control
#define BB_IFC_ENABLE              (1<<0)
#define BB_IFC_DISABLE             (1<<1)
#define BB_IFC_BURST_SIZE          (1<<16)
#define BB_IFC_FIFO_MODE           (1<<17)

//ch2_status
//#define BB_IFC_ENABLE            (1<<0)
#define BB_IFC_ENABLE_DISABLE      (0<<0)
#define BB_IFC_ENABLE_ENABLE       (1<<0)
#define BB_IFC_FIFO_EMPTY          (1<<4)
#define BB_IFC_CAUSE_ITC           (1<<8)
#define BB_IFC_CAUSE_IEF           (1<<9)
#define BB_IFC_CAUSE_IHTC          (1<<10)
#define BB_IFC_ITC                 (1<<12)
#define BB_IFC_IEF                 (1<<13)
#define BB_IFC_IHTC                (1<<14)
#define BB_IFC_CH2_CUR_TC(n)       (((n)&0x1FF)<<16)

//ch2_start_addr
#define BB_IFC_CH2_START_ADDR(n)   (((n)&0x1FFF)<<2)
#define BB_IFC_PAGE_ADDR(n)        (((n)&0x7FF)<<15)

//ch2_end_addr
#define BB_IFC_END_ADDR(n)         (((n)&0x1FFF)<<2)
//#define BB_IFC_PAGE_ADDR(n)      (((n)&0x7FF)<<15)

//ch2_tc
#define BB_IFC_CH2_TC(n)           (((n)&0x1FF)<<0)

//ch2_int_mask
#define BB_IFC_END_TC              (1<<0)
#define BB_IFC_END_FIFO            (1<<1)
#define BB_IFC_HALF_TC             (1<<2)

//ch2_int_clear
//#define BB_IFC_END_TC            (1<<0)
//#define BB_IFC_END_FIFO          (1<<1)
#define BB_IFC_HALF_FIFO           (1<<2)

//ch2_cur_ahb_addr
#define BB_IFC_CUR_AHB_ADDR(n)     (((n)&0x3FFFFFF)<<0)

//ch3_control
//#define BB_IFC_ENABLE            (1<<0)
//#define BB_IFC_DISABLE           (1<<1)

//ch3_status
//#define BB_IFC_ENABLE            (1<<0)
//#define BB_IFC_ENABLE_DISABLE    (0<<0)
//#define BB_IFC_ENABLE_ENABLE     (1<<0)
//#define BB_IFC_FIFO_EMPTY        (1<<4)
//#define BB_IFC_CAUSE_ITC         (1<<8)
//#define BB_IFC_ITC               (1<<12)
#define BB_IFC_CH3_CUR_TC(n)       (((n)&0xFF)<<16)

//ch3_start_addr
#define BB_IFC_CH3_START_ADDR(n)   (((n)&0x1FFF)<<2)

//ch3_tc
#define BB_IFC_CH3_TC(n)           (((n)&0xFF)<<0)

//ch3_int_mask
//#define BB_IFC_END_TC            (1<<0)

//ch3_int_clear
//#define BB_IFC_END_TC            (1<<0)

//ch3_cur_ahb_addr
//#define BB_IFC_CUR_AHB_ADDR(n)   (((n)&0x3FFFFFF)<<0)





#endif

