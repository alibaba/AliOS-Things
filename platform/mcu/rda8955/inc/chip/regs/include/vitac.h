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


#ifndef _VITAC_H_
#define _VITAC_H_

#ifdef CT_ASM
#error "You are trying to use in an assembly code the normal H description of 'vitac'."
#endif


#include "globals.h"

// =============================================================================
//  MACROS
// =============================================================================

// =============================================================================
//  TYPES
// =============================================================================

// ============================================================================
// VITAC_T
// -----------------------------------------------------------------------------
///
// =============================================================================
#define REG_VITAC_BASE             0x01904000

typedef volatile struct
{
    REG32                          command;                      //0x00000000
    REG32                          status;                       //0x00000004
    REG32                          ch_symb_addr;                 //0x00000008
    REG32                          exp_symb_addr;                //0x0000000C
    REG32                          pm_base_addr;                 //0x00000010
    REG32                          out_base_addr;                //0x00000014
    REG32                          H0_param;                     //0x00000018
    REG32                          HL_param;                     //0x0000001C
    REG32                          rescale;                      //0x00000020
    /// This register bank stores the less significant bit of the output from
    /// the coder for a particular code (see 1.2.2.1). The kth butterfly uses
    /// the bit k of this register.
    REG32                          res_poly[3];                  //0x00000024
    REG32                          int_status;                   //0x00000030
    REG32                          int_clear;                    //0x00000034
} HWP_VITAC_T;

#define hwp_vitac                  ((HWP_VITAC_T*) KSEG1(REG_VITAC_BASE))


//command
#define VITAC_START_EQU            (1<<0)
#define VITAC_START_DEC            (1<<1)
#define VITAC_START_TB             (1<<2)
#define VITAC_INT_MASK             (1<<3)
#define VITAC_PUNCTURED            (1<<4)
#define VITAC_NB_STATES(n)         (((n)&3)<<5)
#define VITAC_BKWD_TRELLIS         (1<<7)
#define VITAC_CODE_RATE(n)         (((n)&7)<<8)
#define VITAC_BM_SHIFT_LEV(n)      (((n)&15)<<11)
#define VITAC_SV_SHIFT_LEV(n)      (((n)&15)<<15)
#define VITAC_NB_SYMBOLS(n)        (((n)&0x1FF)<<20)

//status
#define VITAC_EQU_PENDING          (1<<0)
#define VITAC_DEC_PENDING          (1<<1)
#define VITAC_TB_PENDING           (1<<2)
#define VITAC_NB_RESCALES(n)       (((n)&15)<<4)

//ch_symb_addr
#define VITAC_ADDR(n)              (((n)&0x1FFF)<<2)

//exp_symb_addr
//#define VITAC_ADDR(n)            (((n)&0x1FFF)<<2)

//pm_base_addr
//#define VITAC_ADDR(n)            (((n)&0x1FFF)<<2)

//out_base_addr
//#define VITAC_ADDR(n)            (((n)&0x1FFF)<<2)

//H0_param
#define VITAC_H0_I_PART(n)         (((n)&0x1FFF)<<0)
#define VITAC_H0_Q_PART(n)         (((n)&0x1FFF)<<16)

//HL_param
#define VITAC_HL_I_PART(n)         (((n)&0x1FFF)<<0)
#define VITAC_HL_Q_PART(n)         (((n)&0x1FFF)<<16)

//rescale
#define VITAC_THRESHOLD(n)         (((n)&0xFFFF)<<0)

//res_poly
#define VITAC_RES_POLY(n)          (((n)&0xFFFFFFFF)<<0)

//int_status
#define VITAC_IT_CAUSE             (1<<0)
#define VITAC_IT_STATUS            (1<<16)

//int_clear
#define VITAC_IT_CLEAR             (1<<0)





#endif

