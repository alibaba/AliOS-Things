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


#ifndef _XCOR_ASM_H_
#define _XCOR_ASM_H_

//THIS FILE HAS BEEN GENERATED WITH COOLWATCHER. PLEASE EDIT WITH CARE !

#ifndef CT_ASM
#error "You are trying to use in a normal C code the assembly H description of 'xcor'."
#endif

#include "globals_asm.h"

#define XCOR_SYMBOL_SIZE                        (VITAC_MULT_SIZE)
#define XCOR_SADDER_SIZE                        (XCOR_SYMBOL_SIZE+1 +6)

//==============================================================================
// xcor
//------------------------------------------------------------------------------
///
//==============================================================================
#define REG_XCOR_BASE              0x01900000

#define REG_XCOR_BASE_HI           BASE_HI(REG_XCOR_BASE)
#define REG_XCOR_BASE_LO           BASE_LO(REG_XCOR_BASE)

#define REG_XCOR_COMMAND           REG_XCOR_BASE_LO + 0x00000000
#define REG_XCOR_STATUS            REG_XCOR_BASE_LO + 0x00000004
#define REG_XCOR_HV0               REG_XCOR_BASE_LO + 0x00000008
#define REG_XCOR_HV1               REG_XCOR_BASE_LO + 0x0000000C
#define REG_XCOR_DATA_0            REG_XCOR_BASE_LO + 0x00000010
#define REG_XCOR_DATA_1            REG_XCOR_BASE_LO + 0x00000014
#define REG_XCOR_DATA_2            REG_XCOR_BASE_LO + 0x00000018
#define REG_XCOR_DATA_3            REG_XCOR_BASE_LO + 0x0000001C
#define REG_XCOR_DATA_4            REG_XCOR_BASE_LO + 0x00000020
#define REG_XCOR_DATA_5            REG_XCOR_BASE_LO + 0x00000024
#define REG_XCOR_ACCUI             REG_XCOR_BASE_LO + 0x00000028
#define REG_XCOR_ACCUQ             REG_XCOR_BASE_LO + 0x0000002C
#define REG_XCOR_ADDR0             REG_XCOR_BASE_LO + 0x00000030
#define REG_XCOR_DATA_E_0          REG_XCOR_BASE_LO + 0x00000034
#define REG_XCOR_DATA_E_1          REG_XCOR_BASE_LO + 0x00000038
#define REG_XCOR_DATA_E_2          REG_XCOR_BASE_LO + 0x0000003C
#define REG_XCOR_DATA_E_3          REG_XCOR_BASE_LO + 0x00000040
#define REG_XCOR_DATA_E_4          REG_XCOR_BASE_LO + 0x00000044
#define REG_XCOR_DATA_E_5          REG_XCOR_BASE_LO + 0x00000048

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
