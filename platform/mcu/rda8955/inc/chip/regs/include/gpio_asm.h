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


#ifndef _GPIO_ASM_H_
#define _GPIO_ASM_H_

//THIS FILE HAS BEEN GENERATED WITH COOLWATCHER. PLEASE EDIT WITH CARE !

#ifndef CT_ASM
#error "You are trying to use in a normal C code the assembly H description of 'gpio'."
#endif

#include "gallite_generic_config_asm.h"

#define IDX_GPIO_DCON                           (0)
#define IDX_GPO_CHG                             (0)

//==============================================================================
// gpio
//------------------------------------------------------------------------------
///
//==============================================================================
#define REG_GPIO_BASE              0x01A03000

#define REG_GPIO_BASE_HI           BASE_HI(REG_GPIO_BASE)
#define REG_GPIO_BASE_LO           BASE_LO(REG_GPIO_BASE)

#define REG_GPIO_GPIO_OEN_VAL_L    REG_GPIO_BASE_LO + 0x00000000
#define REG_GPIO_GPIO_OEN_SET_OUT_L REG_GPIO_BASE_LO + 0x00000004
#define REG_GPIO_GPIO_OEN_SET_IN_L REG_GPIO_BASE_LO + 0x00000008
#define REG_GPIO_GPIO_VAL_L        REG_GPIO_BASE_LO + 0x0000000C
#define REG_GPIO_GPIO_SET_L        REG_GPIO_BASE_LO + 0x00000010
#define REG_GPIO_GPIO_CLR_L        REG_GPIO_BASE_LO + 0x00000014
#define REG_GPIO_GPINT_R_SET_L     REG_GPIO_BASE_LO + 0x00000018
#define REG_GPIO_GPINT_R_SET_H     REG_GPIO_BASE_LO + 0x0000001C
#define REG_GPIO_INT_CLR_L         REG_GPIO_BASE_LO + 0x00000020
#define REG_GPIO_INT_STATUS_L      REG_GPIO_BASE_LO + 0x00000024
#define REG_GPIO_CHG_CTRL          REG_GPIO_BASE_LO + 0x00000028
#define REG_GPIO_CHG_CMD           REG_GPIO_BASE_LO + 0x0000002C
#define REG_GPIO_GPO_SET           REG_GPIO_BASE_LO + 0x00000030
#define REG_GPIO_GPO_CLR           REG_GPIO_BASE_LO + 0x00000034
#define REG_GPIO_GPIO_OEN_VAL_H    REG_GPIO_BASE_LO + 0x00000038
#define REG_GPIO_GPIO_OEN_SET_OUT_H REG_GPIO_BASE_LO + 0x0000003C
#define REG_GPIO_GPIO_OEN_SET_IN_H REG_GPIO_BASE_LO + 0x00000040
#define REG_GPIO_GPIO_VAL_H        REG_GPIO_BASE_LO + 0x00000044
#define REG_GPIO_GPIO_SET_H        REG_GPIO_BASE_LO + 0x00000048
#define REG_GPIO_GPIO_CLR_H        REG_GPIO_BASE_LO + 0x0000004C
#define REG_GPIO_GPINT_R_CLR_L     REG_GPIO_BASE_LO + 0x00000050
#define REG_GPIO_GPINT_R_CLR_H     REG_GPIO_BASE_LO + 0x00000054
#define REG_GPIO_GPINT_F_SET_L     REG_GPIO_BASE_LO + 0x00000058
#define REG_GPIO_GPINT_F_SET_H     REG_GPIO_BASE_LO + 0x0000005C
#define REG_GPIO_GPINT_F_CLR_L     REG_GPIO_BASE_LO + 0x00000060
#define REG_GPIO_GPINT_F_CLR_H     REG_GPIO_BASE_LO + 0x00000064
#define REG_GPIO_GPINT_DBN_EN_SET_L REG_GPIO_BASE_LO + 0x00000068
#define REG_GPIO_GPINT_DBN_EN_SET_H REG_GPIO_BASE_LO + 0x0000006C
#define REG_GPIO_GPINT_DBN_EN_CLR_L REG_GPIO_BASE_LO + 0x00000070
#define REG_GPIO_GPINT_DBN_EN_CLR_H REG_GPIO_BASE_LO + 0x00000074
#define REG_GPIO_GPINT_MODE_SET_L  REG_GPIO_BASE_LO + 0x00000078
#define REG_GPIO_GPINT_MODE_SET_H  REG_GPIO_BASE_LO + 0x0000007C
#define REG_GPIO_GPINT_MODE_CLR_L  REG_GPIO_BASE_LO + 0x00000080
#define REG_GPIO_GPINT_MODE_CLR_H  REG_GPIO_BASE_LO + 0x00000084
#define REG_GPIO_INT_STATUS_H      REG_GPIO_BASE_LO + 0x00000088
#define REG_GPIO_INT_CLR_H         REG_GPIO_BASE_LO + 0x0000008C

//gpio_oen_val_l
#define GPIO_OEN_VAL_L(n)          (((n)&0xFFFFFFFF)<<0)
#define GPIO_OEN_VAL_L_INPUT       (0x1<<0)
#define GPIO_OEN_VAL_L_OUTPUT      (0x0<<0)

//gpio_oen_set_out_l
#define GPIO_OEN_SET_OUT_L(n)      (((n)&0xFFFFFFFF)<<0)

//gpio_oen_set_in_l
#define GPIO_OEN_SET_IN_L(n)       (((n)&0xFFFFFFFF)<<0)

//gpio_val_l
#define GPIO_GPIO_VAL_L(n)         (((n)&0xFFFFFFFF)<<0)

//gpio_set_l
#define GPIO_GPIO_SET_L(n)         (((n)&0xFFFFFFFF)<<0)

//gpio_clr_l
#define GPIO_GPIO_CLR_L(n)         (((n)&0xFFFFFFFF)<<0)

//gpint_r_set_l
#define GPIO_GPINT_R_SET_L(n)      (((n)&0xFFFFFFFF)<<0)

//gpint_r_set_h
#define GPIO_GPINT_R_SET_H(n)      (((n)&31)<<0)

//int_clr_l
#define GPIO_GPINT_CLR_L(n)        (((n)&0xFFFFFFFF)<<0)

//int_status_l
#define GPIO_GPINT_STATUS_L(n)     (((n)&0xFFFFFFFF)<<0)
#define GPIO_GPINT_STATUS_L_MASK   (0xFFFFFFFF<<0)
#define GPIO_GPINT_STATUS_L_SHIFT  (0)

//chg_ctrl
#define GPIO_OUT_TIME(n)           (((n)&15)<<0)
#define GPIO_WAIT_TIME(n)          (((n)&0x3F)<<4)
#define GPIO_INT_MODE_L2H          (0<<16)
#define GPIO_INT_MODE_H2L          (1<<16)
#define GPIO_INT_MODE_RR           (3<<16)

//chg_cmd
#define GPIO_DCON_MODE_SET         (1<<0)
#define GPIO_CHG_MODE_SET          (1<<4)
#define GPIO_DCON_MODE_CLR         (1<<8)
#define GPIO_CHG_MODE_CLR          (1<<12)
#define GPIO_CHG_DOWN              (1<<24)

//gpo_set
#define GPIO_GPO_SET(n)            (((n)&0x3FF)<<0)

//gpo_clr
#define GPIO_GPO_CLR(n)            (((n)&0x3FF)<<0)

//gpio_oen_val_h
#define GPIO_OEN_VAL_H(n)          (((n)&31)<<0)
#define GPIO_OEN_VAL_H_INPUT       (1<<0)
#define GPIO_OEN_VAL_H_OUTPUT      (0<<0)

//gpio_oen_set_out_h
#define GPIO_OEN_SET_OUT_H(n)      (((n)&31)<<0)

//gpio_oen_set_in_h
#define GPIO_OEN_SET_IN_H(n)       (((n)&31)<<0)

//gpio_val_h
#define GPIO_GPIO_VAL_H(n)         (((n)&31)<<0)

//gpio_set_h
#define GPIO_GPIO_SET_H(n)         (((n)&31)<<0)

//gpio_clr_h
#define GPIO_GPIO_CLR_H(n)         (((n)&31)<<0)

//gpint_r_clr_l
#define GPIO_GPINT_R_CLR_L(n)      (((n)&0xFFFFFFFF)<<0)

//gpint_r_clr_h
#define GPIO_GPINT_R_CLR_H(n)      (((n)&31)<<0)

//gpint_f_set_l
#define GPIO_GPINT_F_SET_L(n)      (((n)&0xFFFFFFFF)<<0)

//gpint_f_set_h
#define GPIO_GPINT_F_SET_H(n)      (((n)&31)<<0)

//gpint_f_clr_l
#define GPIO_GPINT_F_CLR_L(n)      (((n)&0xFFFFFFFF)<<0)

//gpint_f_clr_h
#define GPIO_GPINT_F_CLR_H(n)      (((n)&31)<<0)

//gpint_dbn_en_set_l
#define GPIO_DBN_EN_SET_L(n)       (((n)&0xFFFFFFFF)<<0)

//gpint_dbn_en_set_h
#define GPIO_DBN_EN_SET_H(n)       (((n)&31)<<0)

//gpint_dbn_en_clr_l
#define GPIO_DBN_EN_CLR_L(n)       (((n)&0xFFFFFFFF)<<0)

//gpint_dbn_en_clr_h
#define GPIO_DBN_EN_CLR_H(n)       (((n)&31)<<0)

//gpint_mode_set_l
#define GPIO_GPINT_MODE_SET_L(n)   (((n)&0xFFFFFFFF)<<0)

//gpint_mode_set_h
#define GPIO_GPINT_MODE_SET_H(n)   (((n)&31)<<0)

//gpint_mode_clr_l
#define GPIO_GPINT_MODE_CLR_L(n)   (((n)&0xFFFFFFFF)<<0)

//gpint_mode_clr_h
#define GPIO_GPINT_MODE_CLR_H(n)   (((n)&31)<<0)

//int_status_h
#define GPIO_GPINT_STATUS_H(n)     (((n)&31)<<0)
#define GPIO_GPINT_STATUS_H_MASK   (31<<0)
#define GPIO_GPINT_STATUS_H_SHIFT  (0)

//int_clr_h
#define GPIO_GPINT_CLR_H(n)        (((n)&31)<<0)




#endif
