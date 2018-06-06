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


#ifndef _CHOLK_ASM_H_
#define _CHOLK_ASM_H_

//THIS FILE HAS BEEN GENERATED WITH COOLWATCHER. PLEASE EDIT WITH CARE !

#ifndef CT_ASM
#error "You are trying to use in a normal C code the assembly H description of 'cholk'."
#endif



//==============================================================================
// cholk
//------------------------------------------------------------------------------
///
//==============================================================================
#define REG_CHOLK_BASE             0x01910000

#define REG_CHOLK_BASE_HI          BASE_HI(REG_CHOLK_BASE)
#define REG_CHOLK_BASE_LO          BASE_LO(REG_CHOLK_BASE)

#define REG_CHOLK_CHOLK_CTRL       REG_CHOLK_BASE_LO + 0x00000000
#define REG_CHOLK_GAIN             REG_CHOLK_BASE_LO + 0x00000004
#define REG_CHOLK_ITER_THRE        REG_CHOLK_BASE_LO + 0x00000008
#define REG_CHOLK_MCOVA_BASE       REG_CHOLK_BASE_LO + 0x0000000C
#define REG_CHOLK_MCE_BASE         REG_CHOLK_BASE_LO + 0x00000010
#define REG_CHOLK_MCOEF_BASE       REG_CHOLK_BASE_LO + 0x00000014
#define REG_CHOLK_NUMBER           REG_CHOLK_BASE_LO + 0x00000018
#define REG_CHOLK_CHOLK_STATUS     REG_CHOLK_BASE_LO + 0x0000001C
#define REG_CHOLK_CHOLK_INT        REG_CHOLK_BASE_LO + 0x00000020

//CHOLK_CTRL
#define CHOLK_CHOLK_ON             (1<<0)
#define CHOLK_CHOLK_INT_MASK       (1<<1)
#define CHOLK_CHOLK_MODE           (1<<2)

//GAIN
#define CHOLK_RESI_GAIN(n)         (((n)&3)<<0)
#define CHOLK_RESI2_GAIN(n)        (((n)&3)<<2)
#define CHOLK_OGRS_GAIN(n)         (((n)&3)<<4)
#define CHOLK_OLES1_GAIN(n)        (((n)&3)<<6)
#define CHOLK_OLES2_GAIN(n)        (((n)&3)<<8)
#define CHOLK_COEF_GAIN(n)         (((n)&3)<<10)
#define CHOLK_GRAD_GAIN(n)         (((n)&3)<<12)
#define CHOLK_GOPS_GAIN(n)         (((n)&3)<<14)
#define CHOLK_OLES3_GAIN(n)        (((n)&3)<<16)

//ITER_THRE
#define CHOLK_ITER_THRE1(n)        (((n)&0xFFFF)<<0)

//MCOVA_BASE
#define CHOLK_MCOVA_BASE(n)        (((n)&0x1FFF)<<0)

//MCE_BASE
#define CHOLK_MCE_BASE(n)          (((n)&0x1FFF)<<0)

//MCOEF_BASE
#define CHOLK_MCOEF_BASE(n)        (((n)&0x1FFF)<<0)

//NUMBER
#define CHOLK_ROW_NUMBER(n)        (((n)&31)<<0)
#define CHOLK_MCOVA_NUMBER(n)      (((n)&0x1FF)<<5)
#define CHOLK_ITER_NUMBER(n)       (((n)&31)<<14)

//CHOLK_STATUS
#define CHOLK_CHOLK_STATUS         (1<<0)

//CHOLK_INT
#define CHOLK_CHOLK_INT_RAW        (1<<0)
#define CHOLK_CHOLK_INT_OUT        (1<<1)




#endif
