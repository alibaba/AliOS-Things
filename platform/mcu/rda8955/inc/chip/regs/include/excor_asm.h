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


#ifndef _EXCOR_ASM_H_
#define _EXCOR_ASM_H_

//THIS FILE HAS BEEN GENERATED WITH COOLWATCHER. PLEASE EDIT WITH CARE !

#ifndef CT_ASM
#error "You are trying to use in a normal C code the assembly H description of 'excor'."
#endif

#include "globals_asm.h"

#define EXCOR_IDLE                              (0)
#define EXCOR_BMMLZF                            (1)
#define EXCOR_COMPMATRIMUL                      (2)
#define EXCOR_COMPCONV                          (3)
#define EXCOR_COMPPOW                           (4)
#define EXCOR_DEROTATE                          (5)
#define EXCOR_DCCOMP                            (6)
#define EXCOR_SRECPSK8                          (7)
#define EXCOR_FCCH                              (8)
#define EXCOR_IR_COMB                           (9)
#define EXCOR_IQ_SHIFT                          (10)
#define EXCOR_STATUS_MASK                       (1)
#define EXCOR_FASTMATRIMUL                      (11)
#define EXCOR_FASTCONV                          (12)

//==============================================================================
// excor
//------------------------------------------------------------------------------
///
//==============================================================================
#define REG_EXCOR_BASE             0x0190F000

#define REG_EXCOR_BASE_HI          BASE_HI(REG_EXCOR_BASE)
#define REG_EXCOR_BASE_LO          BASE_LO(REG_EXCOR_BASE)

#define REG_EXCOR_CTRL             REG_EXCOR_BASE_LO + 0x00000000
#define REG_EXCOR_ADDR0            REG_EXCOR_BASE_LO + 0x00000004
#define REG_EXCOR_ADDR1            REG_EXCOR_BASE_LO + 0x00000008
#define REG_EXCOR_ADDR2            REG_EXCOR_BASE_LO + 0x0000000C
#define REG_EXCOR_ADDR3            REG_EXCOR_BASE_LO + 0x00000010
#define REG_EXCOR_ADDR4            REG_EXCOR_BASE_LO + 0x00000014
#define REG_EXCOR_ADDR5            REG_EXCOR_BASE_LO + 0x00000018
#define REG_EXCOR_DATA0            REG_EXCOR_BASE_LO + 0x0000001C
#define REG_EXCOR_DATA1            REG_EXCOR_BASE_LO + 0x00000020
#define REG_EXCOR_DATA2            REG_EXCOR_BASE_LO + 0x00000024
#define REG_EXCOR_DATA3            REG_EXCOR_BASE_LO + 0x00000028
#define REG_EXCOR_DATA4            REG_EXCOR_BASE_LO + 0x0000002C
#define REG_EXCOR_DATA5            REG_EXCOR_BASE_LO + 0x00000030
#define REG_EXCOR_STATUS           REG_EXCOR_BASE_LO + 0x00000034
#define REG_EXCOR_CTRL_FAST        REG_EXCOR_BASE_LO + 0x00000038

//ctrl
#define EXCOR_CMD(n)               (((n)&0xFF)<<0)
#define EXCOR_NB_ILOOP(n)          (((n)&0xFF)<<8)
#define EXCOR_NB_OLOOP(n)          (((n)&0x3FF)<<16)
#define EXCOR_SHIFT_BIT(n)         (((n)&31)<<26)

//addr0
#define EXCOR_ADDR0(n)             (((n)&0x7FFF)<<0)

//addr1
#define EXCOR_ADDR1(n)             (((n)&0x7FFF)<<0)

//addr2
#define EXCOR_ADDR2(n)             (((n)&0x7FFF)<<0)

//addr3
#define EXCOR_ADDR3(n)             (((n)&0xFFFFFFFF)<<0)

//addr4
#define EXCOR_ADDR4(n)             (((n)&0xFFFFFFFF)<<0)

//addr5
#define EXCOR_ADDR5(n)             (((n)&0xFFFFFFFF)<<0)

//data0
#define EXCOR_DATA0(n)             (((n)&0xFFFFFFFF)<<0)

//data1
#define EXCOR_DATA1(n)             (((n)&0xFFFFFFFF)<<0)

//data2
#define EXCOR_DATA2(n)             (((n)&0xFFFFFFFF)<<0)

//data3
#define EXCOR_DATA3(n)             (((n)&0xFFFFFFFF)<<0)

//data4
#define EXCOR_IRCOM_PSIDX0(n)      (((n)&3)<<0)
#define EXCOR_IRCOM_PSIDX2(n)      (((n)&3)<<4)
#define EXCOR_IRCOM_PSIDX1(n)      (((n)&0x7FF)<<8)

//data5
#define EXCOR_DATA5(n)             (((n)&0xFFFFFFFF)<<0)

//status
#define EXCOR_STATUS(n)            (((n)&0xFF)<<0)

//ctrl_fast
#define EXCOR_LOOP_NUM_A(n)        (((n)&7)<<0)
#define EXCOR_LOOP_NUM_B(n)        (((n)&7)<<4)
#define EXCOR_LOOP_NUM_AB(n)       (((n)&0xFF)<<8)
#define EXCOR_SHIFT_BIT_REG1(n)    (((n)&31)<<16)




#endif
