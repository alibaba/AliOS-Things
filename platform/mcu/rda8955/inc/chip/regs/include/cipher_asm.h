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


#ifndef _CIPHER_ASM_H_
#define _CIPHER_ASM_H_

//THIS FILE HAS BEEN GENERATED WITH COOLWATCHER. PLEASE EDIT WITH CARE !

#ifndef CT_ASM
#error "You are trying to use in a normal C code the assembly H description of 'cipher'."
#endif



//==============================================================================
// cipher
//------------------------------------------------------------------------------
///
//==============================================================================
#define REG_CIPHER_BASE            0x01905000

#define REG_CIPHER_BASE_HI         BASE_HI(REG_CIPHER_BASE)
#define REG_CIPHER_BASE_LO         BASE_LO(REG_CIPHER_BASE)

#define REG_CIPHER_CTRL            REG_CIPHER_BASE_LO + 0x00000000
#define REG_CIPHER_STATUS          REG_CIPHER_BASE_LO + 0x00000004
#define REG_CIPHER_KC_LOW          REG_CIPHER_BASE_LO + 0x00000008
#define REG_CIPHER_KC_HIGH         REG_CIPHER_BASE_LO + 0x0000000C
#define REG_CIPHER_COUNT           REG_CIPHER_BASE_LO + 0x00000010
#define REG_CIPHER_DATA0_BLOCK2    REG_CIPHER_BASE_LO + 0x00000014
#define REG_CIPHER_DATA1_BLOCK2    REG_CIPHER_BASE_LO + 0x00000018
#define REG_CIPHER_DATA2_BLOCK2    REG_CIPHER_BASE_LO + 0x0000001C
#define REG_CIPHER_DATA3_BLOCK2    REG_CIPHER_BASE_LO + 0x00000020
#define REG_CIPHER_DATA_BLOCK1_0_DATA_BLOCK1 REG_CIPHER_BASE_LO + 0x00000024
#define REG_CIPHER_DATA_BLOCK1_1_DATA_BLOCK1 REG_CIPHER_BASE_LO + 0x00000028
#define REG_CIPHER_DATA_BLOCK1_2_DATA_BLOCK1 REG_CIPHER_BASE_LO + 0x0000002C
#define REG_CIPHER_DATA_BLOCK1_3_DATA_BLOCK1 REG_CIPHER_BASE_LO + 0x00000030
#define REG_CIPHER_DATA_BLOCK1_4_DATA_BLOCK1 REG_CIPHER_BASE_LO + 0x00000034
#define REG_CIPHER_DATA_BLOCK1_5_DATA_BLOCK1 REG_CIPHER_BASE_LO + 0x00000038
#define REG_CIPHER_DATA_BLOCK1_6_DATA_BLOCK1 REG_CIPHER_BASE_LO + 0x0000003C
#define REG_CIPHER_DATA_BLOCK1_7_DATA_BLOCK1 REG_CIPHER_BASE_LO + 0x00000040
#define REG_CIPHER_DATA_BLOCK1_8_DATA_BLOCK1 REG_CIPHER_BASE_LO + 0x00000044
#define REG_CIPHER_DATA_BLOCK1_9_DATA_BLOCK1 REG_CIPHER_BASE_LO + 0x00000048
#define REG_CIPHER_DATA_BLOCK1_10_DATA_BLOCK1 REG_CIPHER_BASE_LO + 0x0000004C
#define REG_CIPHER_DATA_BLOCK1_11_DATA_BLOCK1 REG_CIPHER_BASE_LO + 0x00000050
#define REG_CIPHER_DATA_BLOCK1_12_DATA_BLOCK1 REG_CIPHER_BASE_LO + 0x00000054
#define REG_CIPHER_DATA_BLOCK1_13_DATA_BLOCK1 REG_CIPHER_BASE_LO + 0x00000058
#define REG_CIPHER_DATA_BLOCK1_14_DATA_BLOCK1 REG_CIPHER_BASE_LO + 0x0000005C
#define REG_CIPHER_DATA_BLOCK1_15_DATA_BLOCK1 REG_CIPHER_BASE_LO + 0x00000060
#define REG_CIPHER_DATA_BLOCK1_16_DATA_BLOCK1 REG_CIPHER_BASE_LO + 0x00000064
#define REG_CIPHER_DATA_BLOCK1_17_DATA_BLOCK1 REG_CIPHER_BASE_LO + 0x00000068
#define REG_CIPHER_DATA_BLOCK1_18_DATA_BLOCK1 REG_CIPHER_BASE_LO + 0x0000006C
#define REG_CIPHER_DATA_BLOCK1_19_DATA_BLOCK1 REG_CIPHER_BASE_LO + 0x00000070
#define REG_CIPHER_DATA_BLOCK1_20_DATA_BLOCK1 REG_CIPHER_BASE_LO + 0x00000074
#define REG_CIPHER_DATA_BLOCK1_21_DATA_BLOCK1 REG_CIPHER_BASE_LO + 0x00000078
#define REG_CIPHER_DATA_BLOCK1_22_DATA_BLOCK1 REG_CIPHER_BASE_LO + 0x0000007C
#define REG_CIPHER_DATA_BLOCK1_23_DATA_BLOCK1 REG_CIPHER_BASE_LO + 0x00000080
#define REG_CIPHER_DATA_BLOCK1_24_DATA_BLOCK1 REG_CIPHER_BASE_LO + 0x00000084
#define REG_CIPHER_DATA_BLOCK1_25_DATA_BLOCK1 REG_CIPHER_BASE_LO + 0x00000088
#define REG_CIPHER_DATA_BLOCK1_26_DATA_BLOCK1 REG_CIPHER_BASE_LO + 0x0000008C
#define REG_CIPHER_DATA_BLOCK1_27_DATA_BLOCK1 REG_CIPHER_BASE_LO + 0x00000090
#define REG_CIPHER_DATA_BLOCK1_28_DATA_BLOCK1 REG_CIPHER_BASE_LO + 0x00000094

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
