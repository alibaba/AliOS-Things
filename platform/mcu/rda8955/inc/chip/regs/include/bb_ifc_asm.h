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


#ifndef _BB_IFC_ASM_H_
#define _BB_IFC_ASM_H_

//THIS FILE HAS BEEN GENERATED WITH COOLWATCHER. PLEASE EDIT WITH CARE !

#ifndef CT_ASM
#error "You are trying to use in a normal C code the assembly H description of 'bb_ifc'."
#endif

#include "globals_asm.h"

#define BB_IFC_ADDR_LEN                         (15)
#define BB_IFC_ADDR_ALIGN                       (2)
#define BB_IFC_TC_LEN_CH2                       (9)
#define BB_IFC_TC_LEN                           (8)

//==============================================================================
// bb_ifc
//------------------------------------------------------------------------------
///
//==============================================================================
#define REG_BB_IFC_BASE            0x01901000

#define REG_BB_IFC_BASE_HI         BASE_HI(REG_BB_IFC_BASE)
#define REG_BB_IFC_BASE_LO         BASE_LO(REG_BB_IFC_BASE)

#define REG_BB_IFC_CH2_CONTROL     REG_BB_IFC_BASE_LO + 0x00000040
#define REG_BB_IFC_CH2_STATUS      REG_BB_IFC_BASE_LO + 0x00000044
#define REG_BB_IFC_CH2_START_ADDR  REG_BB_IFC_BASE_LO + 0x00000048
#define REG_BB_IFC_CH2_END_ADDR    REG_BB_IFC_BASE_LO + 0x0000004C
#define REG_BB_IFC_CH2_TC          REG_BB_IFC_BASE_LO + 0x00000050
#define REG_BB_IFC_CH2_INT_MASK    REG_BB_IFC_BASE_LO + 0x00000054
#define REG_BB_IFC_CH2_INT_CLEAR   REG_BB_IFC_BASE_LO + 0x00000058
#define REG_BB_IFC_CH2_CUR_AHB_ADDR REG_BB_IFC_BASE_LO + 0x0000005C
#define REG_BB_IFC_CH3_CONTROL     REG_BB_IFC_BASE_LO + 0x00000060
#define REG_BB_IFC_CH3_STATUS      REG_BB_IFC_BASE_LO + 0x00000064
#define REG_BB_IFC_CH3_START_ADDR  REG_BB_IFC_BASE_LO + 0x00000068
#define REG_BB_IFC_CH3_TC          REG_BB_IFC_BASE_LO + 0x00000070
#define REG_BB_IFC_CH3_INT_MASK    REG_BB_IFC_BASE_LO + 0x00000074
#define REG_BB_IFC_CH3_INT_CLEAR   REG_BB_IFC_BASE_LO + 0x00000078
#define REG_BB_IFC_CH3_CUR_AHB_ADDR REG_BB_IFC_BASE_LO + 0x0000007C

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
