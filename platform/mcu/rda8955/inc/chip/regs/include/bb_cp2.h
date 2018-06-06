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


#ifndef _BB_CP2_H_
#define _BB_CP2_H_

#ifdef CT_ASM
#error "You are trying to use in an assembly code the normal H description of 'bb_cp2'."
#endif



// =============================================================================
//  MACROS
// =============================================================================

// =============================================================================
//  TYPES
// =============================================================================

// ============================================================================
// BB_CP2_T
// -----------------------------------------------------------------------------
///
// =============================================================================
#define REG_BB_CP2_BASE            0x01908000

typedef volatile struct
{
    REG32                          ctrl;                         //0x00000000
    REG32                          bit_number;                   //0x00000004
    REG32                          Status;                       //0x00000008
    REG32                          lram_addr;                    //0x0000000C
    REG32                          CRC_code_LSB;                 //0x00000010
    REG32                          CRC_code_MSB;                 //0x00000014
    REG32 Reserved_00000018[25];                //0x00000018
    REG32                          CP2_Select;                   //0x0000007C
    REG32                          LRAM_Data;                    //0x00000080
} HWP_BB_CP2_T;

#define hwp_bbCp2                  ((HWP_BB_CP2_T*) KSEG1(REG_BB_CP2_BASE))


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





// changing xml generated defines
#undef BB_CP2_ENABLE_PUNCTURING
#undef BB_CP2_LRAM_DATA
#undef BB_CP2_BIT_NUMBER

#define BB_CP2_ENABLE_PUNCTURING(n) (((n)&1)<<24)

/// BB_CP2 address mapping
#define BB_CP2_CTRL                              0
#define BB_CP2_BIT_NUMBER                        1
#define BB_CP2_STATUS                            2
#define BB_CP2_LRAM_ADDR                         3
#define BB_CP2_CRC_CODE_LSB                      4
#define BB_CP2_CRC_CODE_MSB                      5
#define BB_CP2_LRAM_DATA                         0
#define BB_CP2_LRAM_PUNC                         (0<<5)
#define BB_CP2_DATA_LRAM                         (1<<5)

/* BB_CP2 ACCESSES */
// macro for converting a constant to a string
#define CT_CONVERT_TO_STRING(x) #x
// control register -> GPR
#define CT_BB_CP2_RD_CTRL_REG(regaddr, n)       asm volatile("cfc2 %0, $" CT_CONVERT_TO_STRING(regaddr) :"=r"((n)))
//              GPR -> control register
#define CT_BB_CP2_WR_CTRL_REG(regaddr, n)       asm volatile("ctc2 %0, $" CT_CONVERT_TO_STRING(regaddr) ::"r"((n)))
// general register -> GPR
#define CT_BB_CP2_RD_GNRL_REG_GPR(regaddr, n)   asm volatile("mfc2 %0, $" CT_CONVERT_TO_STRING(regaddr) :"=r"((n)))
//              GPR -> general register
#define CT_BB_CP2_WR_GNRL_REG_GPR(regaddr, n)   asm volatile("mtc2 %0, $" CT_CONVERT_TO_STRING(regaddr) ::"r"((n)))
// general register -> memory
#define CT_BB_CP2_RD_GNRL_REG_MEM(regaddr, out)	asm volatile("swc2 $" CT_CONVERT_TO_STRING(regaddr) ", 0(%0)"::"r"((out)))
//           memory -> general register
#define CT_BB_CP2_WR_GNRL_REG_MEM(regaddr, in) 	asm volatile("lwc2 $" CT_CONVERT_TO_STRING(regaddr) ", 0(%0)"::"r"((in)))




#endif

