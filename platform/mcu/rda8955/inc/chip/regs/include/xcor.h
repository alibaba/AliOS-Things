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


#ifndef _XCOR_H_
#define _XCOR_H_

#ifdef CT_ASM
#error "You are trying to use in an assembly code the normal H description of 'xcor'."
#endif


#include "globals.h"

// =============================================================================
//  MACROS
// =============================================================================
#define XCOR_SYMBOL_SIZE                        (VITAC_MULT_SIZE)
#define XCOR_SADDER_SIZE                        (XCOR_SYMBOL_SIZE+1 +6)

// =============================================================================
//  TYPES
// =============================================================================

// ============================================================================
// XCOR_T
// -----------------------------------------------------------------------------
///
// =============================================================================
#define REG_XCOR_BASE              0x01900000

typedef volatile struct
{
    REG32                          command;                      //0x00000000
    REG32                          status;                       //0x00000004
    /// Multipurpose Data Register.
    /// Store Training Sequence in TSXC mode.
    /// Store SUM in DCOC 3rd pass mode.
    /// Store bit sequence in SREC mode.
    /// Store SUM in SPROC mode.
    /// Store I SUM in CHEST mode.
    /// Store R(k-1) in FCHXC mode.
    REG32                          hv0;                          //0x00000008
    /// Multipurpose Data Register.
    /// Store Training Sequence in TSXC mode.
    /// Store bit sequence in SREC mode.
    /// Store Q SUM in CHEST mode.
    REG32                          hv1;                          //0x0000000C
    /// Multipurpose Data Registers.
    /// D0 stores symbols/softvalues/channel taps depending on mode. Not
    /// readable.
    /// D1 stores decoded bits/IQ threshols/IQ Offsets/A terms depending on
    /// mode. Not readable.
    /// D2 (aka A1) serves as Rd address (decoded bits, A or B terms) / Wr
    /// address register (I or packed IQ results, Symbols) / event counter
    /// depending on mode.
    /// D3 (aka A2) serves as Wr address (Q results) / event counter depending
    /// on mode.
    /// D4 stores results from VITAC / extracted HardValues depending on mode.
    /// Not readable.
    /// D5 (aka A3) serves as Wr address (CQ results) Not readable.
    REG32                          data[6];                      //0x00000010
    REG32                          accuI;                        //0x00000028
    REG32                          accuQ;                        //0x0000002C
    /// Address 0 Register.
    /// Stores Rd address for symbols / SoftValues / A terms depending on mode.
    /// Auto increment/decrement/reset.
    REG32                          addr0;                        //0x00000030
    /// Multipurpose Data Edge Registers.
    REG32                          data_e[6];                    //0x00000034
} HWP_XCOR_T;

#define hwp_xcor                   ((HWP_XCOR_T*) KSEG1(REG_XCOR_BASE))


//command
#define XCOR_BITERR_EN             (1<<0)
#define XCOR_DCO1P_EN              (1<<1)
#define XCOR_DCO2P_EN              (1<<2)
#define XCOR_DCO3P_EN              (1<<3)
#define XCOR_TSCXC_EN              (1<<4)
#define XCOR_SREC_EN               (1<<5)
#define XCOR_BEXT_EN               (1<<6)
#define XCOR_SPROC_EN              (1<<7)
#define XCOR_CHEST_EN              (1<<8)
#define XCOR_FCHXC_EN              (1<<9)
#define XCOR_SLDWIN_EN             (1<<10)
#define XCOR_IT_MASK               (1<<11)
#define XCOR_PACK_IQ               (1<<12)
#define XCOR_DEROTATION_EN         (1<<13)
#define XCOR_NB_ILOOP(n)           (((n)&0xFF)<<16)
#define XCOR_NB_SYMB(n)            (((n)&0xFF)<<24)

//status
#define XCOR_OP_PENDING            (1<<0)
#define XCOR_IT_CAUSE              (1<<16)
#define XCOR_IT_STATUS             (1<<31)

//hv0
#define XCOR_HV0(n)                (((n)&0xFFFFFFFF)<<0)

//hv1
#define XCOR_HV1(n)                (((n)&0xFFFFFFFF)<<0)

//data
#define XCOR_DATA(n)               (((n)&0xFFFFFFFF)<<0)

//accuI
#define XCOR_ACCUI(n)              (((n)&0x1FFFFF)<<0)

//accuQ
#define XCOR_ACCUQ(n)              (((n)&0x1FFFFF)<<0)

//addr0
#define XCOR_ADDR0(n)              (((n)&0x1FFF)<<2)

//data_e
//#define XCOR_DATA(n)             (((n)&0xFFFFFFFF)<<0)





#endif

