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


#ifndef _PSRAM8_CTRL_H_
#define _PSRAM8_CTRL_H_

#ifdef CT_ASM
#error "You are trying to use in an assembly code the normal H description of 'psram8_ctrl'."
#endif



// =============================================================================
//  MACROS
// =============================================================================

// =============================================================================
//  TYPES
// =============================================================================

// ============================================================================
// PSRAM8_CTRL_T
// -----------------------------------------------------------------------------
///
// =============================================================================
#define REG_PSRAM8_CTRL_BASE       0x01A04000

typedef volatile struct
{
    REG32                          ctrl_time;                    //0x00000000
    REG32                          read_ctrl;                    //0x00000004
    REG32                          cre;                          //0x00000008
    REG32                          dqs_ctrl;                     //0x0000000C
    REG32                          clk_ctrl;                     //0x00000010
    REG32                          power_up;                     //0x00000014
    REG32                          power_time;                   //0x00000018
    REG32                          reg_time;                     //0x0000001C
    REG32                          irsr;                         //0x00000020
    REG32                          imr;                          //0x00000024
    REG32                          isr;                          //0x00000028
    REG32                          icr;                          //0x0000002C
    REG32                          debug_sel;                    //0x00000030
    REG32                          timeout_val;                  //0x00000034
} HWP_PSRAM8_CTRL_T;

#define hwp_psram8Ctrl             ((HWP_PSRAM8_CTRL_T*) KSEG1(REG_PSRAM8_CTRL_BASE))


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

