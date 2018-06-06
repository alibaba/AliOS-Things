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


#ifndef _CIPHER_H_
#define _CIPHER_H_

#ifdef CT_ASM
#error "You are trying to use in an assembly code the normal H description of 'cipher'."
#endif



// =============================================================================
//  MACROS
// =============================================================================

// =============================================================================
//  TYPES
// =============================================================================

// ============================================================================
// CIPHER_T
// -----------------------------------------------------------------------------
///
// =============================================================================
#define REG_CIPHER_BASE            0x01905000

typedef volatile struct
{
    REG32                          ctrl;                         //0x00000000
    REG32                          status;                       //0x00000004
    REG32                          Kc_low;                       //0x00000008
    REG32                          Kc_high;                      //0x0000000C
    REG32                          Count;                        //0x00000010
    REG32                          Data0_block2;                 //0x00000014
    REG32                          Data1_block2;                 //0x00000018
    REG32                          Data2_block2;                 //0x0000001C
    REG32                          Data3_block2;                 //0x00000020
    struct
    {
        REG32                      Data_block1;                  //0x00000024
    } Data_block1[29];
} HWP_CIPHER_T;

#define hwp_cipher                 ((HWP_CIPHER_T*) KSEG1(REG_CIPHER_BASE))


//ctrl
#define CIPHER_ENABLE              (1<<0)
#define CIPHER_ALGORITHM_A5_1      (0<<4)
#define CIPHER_ALGORITHM_A5_2      (1<<4)

//status
#define CIPHER_RUNNING             (1<<0)
#define CIPHER_DATA_BLK_RDY        (1<<4)

//Kc_low
#define CIPHER_KC_LSB(n)           (((n)&0xFFFFFFFF)<<0)

//Kc_high
#define CIPHER_KC_MSB(n)           (((n)&0xFFFFFFFF)<<0)

//Count
#define CIPHER_COUNT(n)            (((n)&0x3FFFFF)<<0)

//Data0_block2
#define CIPHER_DATA_BLK2(n)        (((n)&0xFFFFFFFF)<<0)

//Data1_block2
//#define CIPHER_DATA_BLK2(n)      (((n)&0xFFFFFFFF)<<0)

//Data2_block2
//#define CIPHER_DATA_BLK2(n)      (((n)&0xFFFFFFFF)<<0)

//Data3_block2
#define CIPHER_MSB_DATA_BLK2(n)    (((n)&0x3FFFF)<<0)

//Data_block1
#define CIPHER_BIT0                (1<<7)
#define CIPHER_BIT1                (1<<15)
#define CIPHER_BIT2                (1<<23)
#define CIPHER_BIT3                (1<<31)





#endif

