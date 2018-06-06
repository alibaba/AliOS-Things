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


#ifndef _SEG_SCAN_H_
#define _SEG_SCAN_H_

#ifdef CT_ASM
#error "You are trying to use in an assembly code the normal H description of 'seg_scan'."
#endif



// =============================================================================
//  MACROS
// =============================================================================

// =============================================================================
//  TYPES
// =============================================================================

// ============================================================================
// SEG_SCAN_T
// -----------------------------------------------------------------------------
///
// =============================================================================
#define REG_SEG_SCAN_BASE          0x01A0D000

typedef volatile struct
{
    REG32                          code_num_reg;                 //0x00000000
    REG32                          refresh_cycle_reg;            //0x00000004
    REG32                          ctrl;                         //0x00000008
    REG32                          code0_reg;                    //0x0000000C
    REG32                          code1_reg;                    //0x00000010
    REG32                          code2_reg;                    //0x00000014
    REG32                          code3_reg;                    //0x00000018
    REG32                          code4_reg;                    //0x0000001C
    REG32                          code5_reg;                    //0x00000020
    REG32                          code6_reg;                    //0x00000024
    REG32                          code7_reg;                    //0x00000028
    REG32                          code8_reg;                    //0x0000002C
    REG32                          code9_reg;                    //0x00000030
    REG32                          code10_reg;                   //0x00000034
    REG32                          code11_reg;                   //0x00000038
    REG32                          code12_reg;                   //0x0000003C
    REG32                          code13_reg;                   //0x00000040
    REG32                          code14_reg;                   //0x00000044
    REG32                          code15_reg;                   //0x00000048
    REG32                          code16_reg;                   //0x0000004C
    REG32                          code17_reg;                   //0x00000050
    REG32                          pin_oen0_reg;                 //0x00000054
    REG32                          pin_oen1_reg;                 //0x00000058
    REG32                          pin_oen2_reg;                 //0x0000005C
    REG32                          pin_oen3_reg;                 //0x00000060
    REG32                          pin_oen4_reg;                 //0x00000064
    REG32                          pin_oen5_reg;                 //0x00000068
    REG32                          pin_oen6_reg;                 //0x0000006C
    REG32                          pin_oen7_reg;                 //0x00000070
    REG32                          pin_oen8_reg;                 //0x00000074
    REG32                          pin_oen9_reg;                 //0x00000078
    REG32                          pin_oen10_reg;                //0x0000007C
    REG32                          pin_oen11_reg;                //0x00000080
    REG32                          pin_oen12_reg;                //0x00000084
    REG32                          pin_oen13_reg;                //0x00000088
    REG32                          pin_oen14_reg;                //0x0000008C
    REG32                          pin_oen15_reg;                //0x00000090
    REG32                          pin_oen16_reg;                //0x00000094
    REG32                          pin_oen17_reg;                //0x00000098
} HWP_SEG_SCAN_T;

#define hwp_segScan                ((HWP_SEG_SCAN_T*) KSEG1(REG_SEG_SCAN_BASE))


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

