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


#ifndef _CHOLK_H_
#define _CHOLK_H_

#ifdef CT_ASM
#error "You are trying to use in an assembly code the normal H description of 'cholk'."
#endif



// =============================================================================
//  MACROS
// =============================================================================

// =============================================================================
//  TYPES
// =============================================================================

// ============================================================================
// CHOLK_T
// -----------------------------------------------------------------------------
///
// =============================================================================
#define REG_CHOLK_BASE             0x01910000

typedef volatile struct
{
    REG32                          CHOLK_CTRL;                   //0x00000000
    REG32                          GAIN;                         //0x00000004
    REG32                          ITER_THRE;                    //0x00000008
    REG32                          MCOVA_BASE;                   //0x0000000C
    REG32                          MCE_BASE;                     //0x00000010
    REG32                          MCOEF_BASE;                   //0x00000014
    REG32                          NUMBER;                       //0x00000018
    REG32                          CHOLK_STATUS;                 //0x0000001C
    REG32                          CHOLK_INT;                    //0x00000020
} HWP_CHOLK_T;

#define hwp_cholk                  ((HWP_CHOLK_T*) KSEG1(REG_CHOLK_BASE))


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

