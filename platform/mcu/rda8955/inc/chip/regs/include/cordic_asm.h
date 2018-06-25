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


#ifndef _CORDIC_ASM_H_
#define _CORDIC_ASM_H_

//THIS FILE HAS BEEN GENERATED WITH COOLWATCHER. PLEASE EDIT WITH CARE !

#ifndef CT_ASM
#error "You are trying to use in a normal C code the assembly H description of 'cordic'."
#endif



//==============================================================================
// cordic
//------------------------------------------------------------------------------
///
//==============================================================================
#define REG_CORDIC_BASE            0x01911000

#define REG_CORDIC_BASE_HI         BASE_HI(REG_CORDIC_BASE)
#define REG_CORDIC_BASE_LO         BASE_LO(REG_CORDIC_BASE)

#define REG_CORDIC_YIN             REG_CORDIC_BASE_LO + 0x00000000
#define REG_CORDIC_XIN             REG_CORDIC_BASE_LO + 0x00000004
#define REG_CORDIC_CMD             REG_CORDIC_BASE_LO + 0x00000008
#define REG_CORDIC_STATUS          REG_CORDIC_BASE_LO + 0x0000000C
#define REG_CORDIC_DOUT            REG_CORDIC_BASE_LO + 0x00000010
#define REG_CORDIC_AMP             REG_CORDIC_BASE_LO + 0x00000014

//yin
#define CORDIC_Y_ADDR(n)           (((n)&0xFFFFFFFF)<<0)

//xin
#define CORDIC_X_ADDR(n)           (((n)&0xFFFFFFFF)<<0)

//cmd
#define CORDIC_CMD_RST             (0<<0)
#define CORDIC_CMD_START           (1<<0)

//status
#define CORDIC_OP_BUSY             (0x0<<0)
#define CORDIC_OP_FREE             (0xFFFFFFFF<<0)

//dout
#define CORDIC_RESULT_ANGLE(n)     (((n)&0xFFFF)<<0)
#define CORDIC_RESULT_AMPLITUDE(n) (((n)&0xFFFF)<<16)

//amp
#define CORDIC_RESULT_AMP(n)       (((n)&0x1FFFF)<<0)
#define CORDIC_RESULT_AMP_MASK     (0x1FFFF<<0)




#endif
