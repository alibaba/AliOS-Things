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


#ifndef _CIPHER_A53_H_
#define _CIPHER_A53_H_

#ifdef CT_ASM
#error "You are trying to use in an assembly code the normal H description of 'cipher_a53'."
#endif



// =============================================================================
//  MACROS
// =============================================================================

// =============================================================================
//  TYPES
// =============================================================================

// ============================================================================
// CIPHER_A53_T
// -----------------------------------------------------------------------------
///
// =============================================================================
#define REG_CIPHER_A53_BASE        0x0191F000

typedef volatile struct
{
    REG16                          CIPH_STAT;                    //0x00000000
    REG16                          Key_Register0;                //0x00000002
    REG16                          Key_Register1;                //0x00000004
    REG16                          Key_Register2;                //0x00000006
    REG16                          Key_Register3;                //0x00000008
    REG16                          TMOD26;                       //0x0000000A
    REG16                          TMOD51;                       //0x0000000C
    REG16                          SFNUM;                        //0x0000000E
    REG16                          Key_Register4;                //0x00000010
    REG16                          Key_Register5;                //0x00000012
    REG16                          Key_Register6;                //0x00000014
    REG16                          Key_Register7;                //0x00000016
    REG16                          Key_Data1;                    //0x00000018
    REG16                          Key_Data2;                    //0x0000001A
    REG16                          Key_Data3;                    //0x0000001C
    REG16                          Key_Data4;                    //0x0000001E
    REG16                          unused1[112];                 //0x00000020
    REG16                          block1[8];                    //0x00000100
    REG16                          unused2[24];                  //0x00000110
    REG16                          block2[8];                    //0x00000140
} HWP_CIPHER_A53_T;

#define hwp_cipherA53              ((HWP_CIPHER_A53_T*) KSEG1(REG_CIPHER_A53_BASE))


//CIPH_STAT
#define CIPHER_A53_RES(n)          (((n)&0x7FF)<<5)
#define CIPHER_A53_A53             (1<<4)
#define CIPHER_A53_INIT            (1<<3)
#define CIPHER_A53_EDGE            (1<<2)
#define CIPHER_A53_A52             (1<<1)
#define CIPHER_A53_CACT            (1<<0)

//Key_Register0
#define CIPHER_A53_KEY0(n)         (((n)&0xFFFF)<<0)

//Key_Register1
#define CIPHER_A53_KEY1(n)         (((n)&0xFFFF)<<0)

//Key_Register2
#define CIPHER_A53_KEY2(n)         (((n)&0xFFFF)<<0)

//Key_Register3
#define CIPHER_A53_KEY3(n)         (((n)&0xFFFF)<<0)

//TMOD26
#define CIPHER_A53_A52_RES_TMOD26(n) (((n)&0x7FF)<<5)
#define CIPHER_A53_A52_T26N(n)     (((n)&31)<<0)

//TMOD51
#define CIPHER_A53_A52_RES_TMOD51(n) (((n)&0x3FF)<<6)
#define CIPHER_A53_A52_T51N(n)     (((n)&0x3F)<<0)

//SFNUM
#define CIPHER_A53_A52_RES_SFNUM(n) (((n)&31)<<11)
#define CIPHER_A53_A52_SFN(n)      (((n)&0x7FF)<<0)

//Key_Register4
#define CIPHER_A53_KEY4(n)         (((n)&0xFFFF)<<0)

//Key_Register5
#define CIPHER_A53_KEY5(n)         (((n)&0xFFFF)<<0)

//Key_Register6
#define CIPHER_A53_KEY6(n)         (((n)&0xFFFF)<<0)

//Key_Register7
#define CIPHER_A53_KEY7(n)         (((n)&0xFFFF)<<0)

//Key_Data1
#define CIPHER_A53_CA(n)           (((n)&0xFF)<<8)
#define CIPHER_A53_CB(n)           (((n)&31)<<3)
#define CIPHER_A53_CD              (1<<2)
#define CIPHER_A53_RES_KEY_DAT1(n) (((n)&3)<<0)

//Key_Data2
#define CIPHER_A53_CE(n)           (((n)&0xFFFF)<<0)

//Key_Data3
#define CIPHER_A53_RES_15_14(n)    (((n)&3)<<14)
#define CIPHER_A53_SFN_10_5(n)     (((n)&0x3F)<<8)
#define CIPHER_A53_RES_7_0(n)      (((n)&0xFF)<<0)

//Key_Data4
#define CIPHER_A53_SFN_4_0(n)      (((n)&31)<<11)
#define CIPHER_A53_T51N(n)         (((n)&0x3F)<<5)
#define CIPHER_A53_T26N(n)         (((n)&31)<<0)



// ============================================================================
// CIPHER_A53_SPRAM_T
// -----------------------------------------------------------------------------
///
// =============================================================================
#define REG_CIPHER_A53_SPRAM_BASE  0x0191F100

typedef volatile struct
{
    UINT8 spram_array[128];                     //0x00000000
} HWP_CIPHER_A53_SPRAM_T;

#define hwp_cipherA53Spram         ((HWP_CIPHER_A53_SPRAM_T*) KSEG1(REG_CIPHER_A53_SPRAM_BASE))






#endif

