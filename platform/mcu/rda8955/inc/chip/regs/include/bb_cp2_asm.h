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


#ifndef _BB_CP2_ASM_H_
#define _BB_CP2_ASM_H_

//THIS FILE HAS BEEN GENERATED WITH COOLWATCHER. PLEASE EDIT WITH CARE !

#ifndef CT_ASM
#error "You are trying to use in a normal C code the assembly H description of 'bb_cp2'."
#endif



//==============================================================================
// bb_cp2
//------------------------------------------------------------------------------
///
//==============================================================================
#define REG_BB_CP2_BASE            0x01908000

#define REG_BB_CP2_BASE_HI         BASE_HI(REG_BB_CP2_BASE)
#define REG_BB_CP2_BASE_LO         BASE_LO(REG_BB_CP2_BASE)

#define REG_BB_CP2_CTRL            REG_BB_CP2_BASE_LO + 0x00000000
#define REG_BB_CP2_BIT_NUMBER      REG_BB_CP2_BASE_LO + 0x00000004
#define REG_BB_CP2_STATUS          REG_BB_CP2_BASE_LO + 0x00000008
#define REG_BB_CP2_LRAM_ADDR       REG_BB_CP2_BASE_LO + 0x0000000C
#define REG_BB_CP2_CRC_CODE_LSB    REG_BB_CP2_BASE_LO + 0x00000010
#define REG_BB_CP2_CRC_CODE_MSB    REG_BB_CP2_BASE_LO + 0x00000014
#define REG_BB_CP2_CP2_SELECT      REG_BB_CP2_BASE_LO + 0x0000007C
#define REG_BB_CP2_LRAM_DATA       REG_BB_CP2_BASE_LO + 0x00000080

//ctrl
#define BB_CP2_FIRST_POLY(n)       (((n)&7)<<0)
#define BB_CP2_SECOND_POLY(n)      (((n)&7)<<3)
#define BB_CP2_THIRD_POLY(n)       (((n)&7)<<6)
#define BB_CP2_FOURTH_POLY(n)      (((n)&7)<<9)
#define BB_CP2_FITH_POLY(n)        (((n)&7)<<12)
#define BB_CP2_SIXTH_POLY(n)       (((n)&7)<<15)
#define BB_CP2_RSC_POLY(n)         (((n)&7)<<18)
#define BB_CP2_NB_POLY(n)          (((n)&7)<<21)
#define BB_CP2_ENABLE_PUNCTURING   (1<<24)

//bit_number
#define BB_CP2_BIT_NUMBER(n)       (((n)&0x1FF)<<0)

//Status
#define BB_CP2_ENABLE              (1<<0)

//lram_addr
#define BB_CP2_LRAM_ADDRESS(n)     (((n)&31)<<0)
#define BB_CP2_LRAM_SELECT         (1<<5)

//CRC_code_LSB
#define BB_CP2_CRC_LSB_CODE(n)     (((n)&0xFFFFFFFF)<<0)

//CRC_code_MSB
#define BB_CP2_CRC_MSB_CODE(n)     (((n)&0xFF)<<0)

//CP2_Select
#define BB_CP2_CP2_SELECT          (1<<0)

//LRAM_Data
#define BB_CP2_LRAM_DATA(n)        (((n)&0xFFFFFFFF)<<0)




#endif
