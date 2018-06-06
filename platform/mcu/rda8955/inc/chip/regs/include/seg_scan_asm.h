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


#ifndef _SEG_SCAN_ASM_H_
#define _SEG_SCAN_ASM_H_

//THIS FILE HAS BEEN GENERATED WITH COOLWATCHER. PLEASE EDIT WITH CARE !

#ifndef CT_ASM
#error "You are trying to use in a normal C code the assembly H description of 'seg_scan'."
#endif



//==============================================================================
// seg_scan
//------------------------------------------------------------------------------
///
//==============================================================================
#define REG_SEG_SCAN_BASE          0x01A0D000

#define REG_SEG_SCAN_BASE_HI       BASE_HI(REG_SEG_SCAN_BASE)
#define REG_SEG_SCAN_BASE_LO       BASE_LO(REG_SEG_SCAN_BASE)

#define REG_SEG_SCAN_CODE_NUM_REG  REG_SEG_SCAN_BASE_LO + 0x00000000
#define REG_SEG_SCAN_REFRESH_CYCLE_REG REG_SEG_SCAN_BASE_LO + 0x00000004
#define REG_SEG_SCAN_CTRL          REG_SEG_SCAN_BASE_LO + 0x00000008
#define REG_SEG_SCAN_CODE0_REG     REG_SEG_SCAN_BASE_LO + 0x0000000C
#define REG_SEG_SCAN_CODE1_REG     REG_SEG_SCAN_BASE_LO + 0x00000010
#define REG_SEG_SCAN_CODE2_REG     REG_SEG_SCAN_BASE_LO + 0x00000014
#define REG_SEG_SCAN_CODE3_REG     REG_SEG_SCAN_BASE_LO + 0x00000018
#define REG_SEG_SCAN_CODE4_REG     REG_SEG_SCAN_BASE_LO + 0x0000001C
#define REG_SEG_SCAN_CODE5_REG     REG_SEG_SCAN_BASE_LO + 0x00000020
#define REG_SEG_SCAN_CODE6_REG     REG_SEG_SCAN_BASE_LO + 0x00000024
#define REG_SEG_SCAN_CODE7_REG     REG_SEG_SCAN_BASE_LO + 0x00000028
#define REG_SEG_SCAN_CODE8_REG     REG_SEG_SCAN_BASE_LO + 0x0000002C
#define REG_SEG_SCAN_CODE9_REG     REG_SEG_SCAN_BASE_LO + 0x00000030
#define REG_SEG_SCAN_CODE10_REG    REG_SEG_SCAN_BASE_LO + 0x00000034
#define REG_SEG_SCAN_CODE11_REG    REG_SEG_SCAN_BASE_LO + 0x00000038
#define REG_SEG_SCAN_CODE12_REG    REG_SEG_SCAN_BASE_LO + 0x0000003C
#define REG_SEG_SCAN_CODE13_REG    REG_SEG_SCAN_BASE_LO + 0x00000040
#define REG_SEG_SCAN_CODE14_REG    REG_SEG_SCAN_BASE_LO + 0x00000044
#define REG_SEG_SCAN_CODE15_REG    REG_SEG_SCAN_BASE_LO + 0x00000048
#define REG_SEG_SCAN_CODE16_REG    REG_SEG_SCAN_BASE_LO + 0x0000004C
#define REG_SEG_SCAN_CODE17_REG    REG_SEG_SCAN_BASE_LO + 0x00000050
#define REG_SEG_SCAN_PIN_OEN0_REG  REG_SEG_SCAN_BASE_LO + 0x00000054
#define REG_SEG_SCAN_PIN_OEN1_REG  REG_SEG_SCAN_BASE_LO + 0x00000058
#define REG_SEG_SCAN_PIN_OEN2_REG  REG_SEG_SCAN_BASE_LO + 0x0000005C
#define REG_SEG_SCAN_PIN_OEN3_REG  REG_SEG_SCAN_BASE_LO + 0x00000060
#define REG_SEG_SCAN_PIN_OEN4_REG  REG_SEG_SCAN_BASE_LO + 0x00000064
#define REG_SEG_SCAN_PIN_OEN5_REG  REG_SEG_SCAN_BASE_LO + 0x00000068
#define REG_SEG_SCAN_PIN_OEN6_REG  REG_SEG_SCAN_BASE_LO + 0x0000006C
#define REG_SEG_SCAN_PIN_OEN7_REG  REG_SEG_SCAN_BASE_LO + 0x00000070
#define REG_SEG_SCAN_PIN_OEN8_REG  REG_SEG_SCAN_BASE_LO + 0x00000074
#define REG_SEG_SCAN_PIN_OEN9_REG  REG_SEG_SCAN_BASE_LO + 0x00000078
#define REG_SEG_SCAN_PIN_OEN10_REG REG_SEG_SCAN_BASE_LO + 0x0000007C
#define REG_SEG_SCAN_PIN_OEN11_REG REG_SEG_SCAN_BASE_LO + 0x00000080
#define REG_SEG_SCAN_PIN_OEN12_REG REG_SEG_SCAN_BASE_LO + 0x00000084
#define REG_SEG_SCAN_PIN_OEN13_REG REG_SEG_SCAN_BASE_LO + 0x00000088
#define REG_SEG_SCAN_PIN_OEN14_REG REG_SEG_SCAN_BASE_LO + 0x0000008C
#define REG_SEG_SCAN_PIN_OEN15_REG REG_SEG_SCAN_BASE_LO + 0x00000090
#define REG_SEG_SCAN_PIN_OEN16_REG REG_SEG_SCAN_BASE_LO + 0x00000094
#define REG_SEG_SCAN_PIN_OEN17_REG REG_SEG_SCAN_BASE_LO + 0x00000098

//code_num_reg
#define SEG_SCAN_CODE_NUM(n)       (((n)&31)<<0)

//refresh_cycle_reg
#define SEG_SCAN_REFRESH_CYCLE(n)  (((n)&0xFFFFFFFF)<<0)

//ctrl
#define SEG_SCAN_START             (1<<0)
#define SEG_SCAN_STOP              (1<<1)
#define SEG_SCAN_LOAD_EN           (1<<2)

//code0_reg
#define SEG_SCAN_CODE0(n)          (((n)&0x3FFFF)<<0)

//code1_reg
#define SEG_SCAN_CODE1(n)          (((n)&0x3FFFF)<<0)

//code2_reg
#define SEG_SCAN_CODE2(n)          (((n)&0x3FFFF)<<0)

//code3_reg
#define SEG_SCAN_CODE3(n)          (((n)&0x3FFFF)<<0)

//code4_reg
#define SEG_SCAN_CODE4(n)          (((n)&0x3FFFF)<<0)

//code5_reg
#define SEG_SCAN_CODE5(n)          (((n)&0x3FFFF)<<0)

//code6_reg
#define SEG_SCAN_CODE6(n)          (((n)&0x3FFFF)<<0)

//code7_reg
#define SEG_SCAN_CODE7(n)          (((n)&0x3FFFF)<<0)

//code8_reg
#define SEG_SCAN_CODE8(n)          (((n)&0x3FFFF)<<0)

//code9_reg
#define SEG_SCAN_CODE9(n)          (((n)&0x3FFFF)<<0)

//code10_reg
#define SEG_SCAN_CODE10(n)         (((n)&0x3FFFF)<<0)

//code11_reg
#define SEG_SCAN_CODE11(n)         (((n)&0x3FFFF)<<0)

//code12_reg
#define SEG_SCAN_CODE12(n)         (((n)&0x3FFFF)<<0)

//code13_reg
#define SEG_SCAN_CODE13(n)         (((n)&0x3FFFF)<<0)

//code14_reg
#define SEG_SCAN_CODE14(n)         (((n)&0x3FFFF)<<0)

//code15_reg
#define SEG_SCAN_CODE15(n)         (((n)&0x3FFFF)<<0)

//code16_reg
#define SEG_SCAN_CODE16(n)         (((n)&0x3FFFF)<<0)

//code17_reg
#define SEG_SCAN_CODE17(n)         (((n)&0x3FFFF)<<0)

//pin_oen0_reg
#define SEG_SCAN_PIN_OEN0(n)       (((n)&0x3FFFF)<<0)

//pin_oen1_reg
#define SEG_SCAN_PIN_OEN1(n)       (((n)&0x3FFFF)<<0)

//pin_oen2_reg
#define SEG_SCAN_PIN_OEN2(n)       (((n)&0x3FFFF)<<0)

//pin_oen3_reg
#define SEG_SCAN_PIN_OEN3(n)       (((n)&0x3FFFF)<<0)

//pin_oen4_reg
#define SEG_SCAN_PIN_OEN4(n)       (((n)&0x3FFFF)<<0)

//pin_oen5_reg
#define SEG_SCAN_PIN_OEN5(n)       (((n)&0x3FFFF)<<0)

//pin_oen6_reg
#define SEG_SCAN_PIN_OEN6(n)       (((n)&0x3FFFF)<<0)

//pin_oen7_reg
#define SEG_SCAN_PIN_OEN7(n)       (((n)&0x3FFFF)<<0)

//pin_oen8_reg
#define SEG_SCAN_PIN_OEN8(n)       (((n)&0x3FFFF)<<0)

//pin_oen9_reg
#define SEG_SCAN_PIN_OEN9(n)       (((n)&0x3FFFF)<<0)

//pin_oen10_reg
#define SEG_SCAN_PIN_OEN10(n)      (((n)&0x3FFFF)<<0)

//pin_oen11_reg
#define SEG_SCAN_PIN_OEN11(n)      (((n)&0x3FFFF)<<0)

//pin_oen12_reg
#define SEG_SCAN_PIN_OEN12(n)      (((n)&0x3FFFF)<<0)

//pin_oen13_reg
#define SEG_SCAN_PIN_OEN13(n)      (((n)&0x3FFFF)<<0)

//pin_oen14_reg
#define SEG_SCAN_PIN_OEN14(n)      (((n)&0x3FFFF)<<0)

//pin_oen15_reg
#define SEG_SCAN_PIN_OEN15(n)      (((n)&0x3FFFF)<<0)

//pin_oen16_reg
#define SEG_SCAN_PIN_OEN16(n)      (((n)&0x3FFFF)<<0)

//pin_oen17_reg
#define SEG_SCAN_PIN_OEN17(n)      (((n)&0x3FFFF)<<0)




#endif
