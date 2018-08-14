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


#ifndef _PSRAM8_CTRL_ASM_H_
#define _PSRAM8_CTRL_ASM_H_

//THIS FILE HAS BEEN GENERATED WITH COOLWATCHER. PLEASE EDIT WITH CARE !

#ifndef CT_ASM
#error "You are trying to use in a normal C code the assembly H description of 'psram8_ctrl'."
#endif



//==============================================================================
// psram8_ctrl
//------------------------------------------------------------------------------
///
//==============================================================================
#define REG_PSRAM8_CTRL_BASE       0x01A04000

#define REG_PSRAM8_CTRL_BASE_HI    BASE_HI(REG_PSRAM8_CTRL_BASE)
#define REG_PSRAM8_CTRL_BASE_LO    BASE_LO(REG_PSRAM8_CTRL_BASE)

#define REG_PSRAM8_CTRL_CTRL_TIME  REG_PSRAM8_CTRL_BASE_LO + 0x00000000
#define REG_PSRAM8_CTRL_READ_CTRL  REG_PSRAM8_CTRL_BASE_LO + 0x00000004
#define REG_PSRAM8_CTRL_CRE        REG_PSRAM8_CTRL_BASE_LO + 0x00000008
#define REG_PSRAM8_CTRL_DQS_CTRL   REG_PSRAM8_CTRL_BASE_LO + 0x0000000C
#define REG_PSRAM8_CTRL_CLK_CTRL   REG_PSRAM8_CTRL_BASE_LO + 0x00000010
#define REG_PSRAM8_CTRL_POWER_UP   REG_PSRAM8_CTRL_BASE_LO + 0x00000014
#define REG_PSRAM8_CTRL_POWER_TIME REG_PSRAM8_CTRL_BASE_LO + 0x00000018
#define REG_PSRAM8_CTRL_REG_TIME   REG_PSRAM8_CTRL_BASE_LO + 0x0000001C
#define REG_PSRAM8_CTRL_IRSR       REG_PSRAM8_CTRL_BASE_LO + 0x00000020
#define REG_PSRAM8_CTRL_IMR        REG_PSRAM8_CTRL_BASE_LO + 0x00000024
#define REG_PSRAM8_CTRL_ISR        REG_PSRAM8_CTRL_BASE_LO + 0x00000028
#define REG_PSRAM8_CTRL_ICR        REG_PSRAM8_CTRL_BASE_LO + 0x0000002C
#define REG_PSRAM8_CTRL_DEBUG_SEL  REG_PSRAM8_CTRL_BASE_LO + 0x00000030
#define REG_PSRAM8_CTRL_TIMEOUT_VAL REG_PSRAM8_CTRL_BASE_LO + 0x00000034

//ctrl_time
#define PSRAM8_CTRL_RL_TYPE        (1<<31)
#define PSRAM8_CTRL_RL(n)          (((n)&0x3F)<<24)
#define PSRAM8_CTRL_WL(n)          (((n)&0x3F)<<16)
#define PSRAM8_CTRL_W_TCPH(n)      (((n)&0x3F)<<8)
#define PSRAM8_CTRL_R_TCPH(n)      (((n)&0x3F)<<0)

//read_ctrl
#define PSRAM8_CTRL_FIFO_RST_TIME(n) (((n)&0x3F)<<12)
#define PSRAM8_CTRL_RD_START_MODE  (1<<8)
#define PSRAM8_CTRL_OPT_LENGTH(n)  (((n)&15)<<4)
#define PSRAM8_CTRL_RD_START_NUM(n) (((n)&15)<<0)

//cre
#define PSRAM8_CTRL_CRE            (1<<0)

//dqs_ctrl
#define PSRAM8_CTRL_O_DQS_U_DELAY(n) (((n)&0x7F)<<24)
#define PSRAM8_CTRL_O_DQS_L_DELAY(n) (((n)&0x7F)<<16)
#define PSRAM8_CTRL_I_DQS_U_DELAY(n) (((n)&0x7F)<<8)
#define PSRAM8_CTRL_I_DQS_L_DELAY(n) (((n)&0x7F)<<0)

//clk_ctrl
#define PSRAM8_CTRL_O_CLK_DELAY(n) (((n)&0x7F)<<0)

//power_up
#define PSRAM8_CTRL_INIT_DONE      (1<<8)
#define PSRAM8_CTRL_HW_POWER_PULSE (1<<4)
#define PSRAM8_CTRL_SW_INIT_DONE   (1<<1)
#define PSRAM8_CTRL_SW_POWER_LEVEL (1<<0)

//power_time
#define PSRAM8_CTRL_RST_WAIT_TIME(n) (((n)&0x3FF)<<16)
#define PSRAM8_CTRL_RST_TCPH_TIME(n) (((n)&0x3F)<<8)
#define PSRAM8_CTRL_RST_ACC_TIME(n) (((n)&0x3F)<<0)

//reg_time
#define PSRAM8_CTRL_NOP_TIME(n)    (((n)&0x3F)<<16)
#define PSRAM8_CTRL_SEND_REG_TIME(n) (((n)&0x3F)<<8)
#define PSRAM8_CTRL_READ_REG_TIME(n) (((n)&0x3F)<<0)

//irsr
#define PSRAM8_CTRL_IRSR_CROSS_1K  (1<<2)
#define PSRAM8_CTRL_IRSR_RD_TIMEOUT (1<<1)
#define PSRAM8_CTRL_IRSR_INIT_DONE (1<<0)

//imr
#define PSRAM8_CTRL_IMR_CROSS_1K   (1<<2)
#define PSRAM8_CTRL_IMR_RD_TIMEOUT (1<<1)
#define PSRAM8_CTRL_IMR_INIT_DONE  (1<<0)

//isr
#define PSRAM8_CTRL_ISR_CROSS_1K   (1<<2)
#define PSRAM8_CTRL_ISR_RD_TIMEOUT (1<<1)
#define PSRAM8_CTRL_ISR_INIT_DONE  (1<<0)

//icr
#define PSRAM8_CTRL_ICR_CROSS_1K   (1<<2)
#define PSRAM8_CTRL_ICR_RD_TIMEOUT (1<<1)
#define PSRAM8_CTRL_ICR_INIT_DONE  (1<<0)

//debug_sel
#define PSRAM8_CTRL_DEBUG_SEL(n)   (((n)&0xFF)<<0)

//timeout_val
#define PSRAM8_CTRL_TIMEOUT_VALUE(n) (((n)&0xFFFFF)<<0)




#endif
