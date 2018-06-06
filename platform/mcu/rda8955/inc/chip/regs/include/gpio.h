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


#ifndef _GPIO_H_
#define _GPIO_H_

#ifdef CT_ASM
#error "You are trying to use in an assembly code the normal H description of 'gpio'."
#endif


#include "gallite_generic_config.h"

// =============================================================================
//  MACROS
// =============================================================================
#define IDX_GPIO_DCON                           (0)
#define IDX_GPO_CHG                             (0)

// =============================================================================
//  TYPES
// =============================================================================

// ============================================================================
// GPIO_T
// -----------------------------------------------------------------------------
///
// =============================================================================
#define REG_GPIO_BASE              0x01A03000

typedef volatile struct
{
    REG32                          gpio_oen_val_l;               //0x00000000
    REG32                          gpio_oen_set_out_l;           //0x00000004
    REG32                          gpio_oen_set_in_l;            //0x00000008
    REG32                          gpio_val_l;                   //0x0000000C
    REG32                          gpio_set_l;                   //0x00000010
    REG32                          gpio_clr_l;                   //0x00000014
    REG32                          gpint_r_set_l;                //0x00000018
    REG32                          gpint_r_set_h;                //0x0000001C
    REG32                          int_clr_l;                    //0x00000020
    REG32                          int_status_l;                 //0x00000024
    REG32                          chg_ctrl;                     //0x00000028
    REG32                          chg_cmd;                      //0x0000002C
    REG32                          gpo_set;                      //0x00000030
    REG32                          gpo_clr;                      //0x00000034
    REG32                          gpio_oen_val_h;               //0x00000038
    REG32                          gpio_oen_set_out_h;           //0x0000003C
    REG32                          gpio_oen_set_in_h;            //0x00000040
    REG32                          gpio_val_h;                   //0x00000044
    REG32                          gpio_set_h;                   //0x00000048
    REG32                          gpio_clr_h;                   //0x0000004C
    REG32                          gpint_r_clr_l;                //0x00000050
    REG32                          gpint_r_clr_h;                //0x00000054
    REG32                          gpint_f_set_l;                //0x00000058
    REG32                          gpint_f_set_h;                //0x0000005C
    REG32                          gpint_f_clr_l;                //0x00000060
    REG32                          gpint_f_clr_h;                //0x00000064
    REG32                          gpint_dbn_en_set_l;           //0x00000068
    REG32                          gpint_dbn_en_set_h;           //0x0000006C
    REG32                          gpint_dbn_en_clr_l;           //0x00000070
    REG32                          gpint_dbn_en_clr_h;           //0x00000074
    REG32                          gpint_mode_set_l;             //0x00000078
    REG32                          gpint_mode_set_h;             //0x0000007C
    REG32                          gpint_mode_clr_l;             //0x00000080
    REG32                          gpint_mode_clr_h;             //0x00000084
    REG32                          int_status_h;                 //0x00000088
    REG32                          int_clr_h;                    //0x0000008C
} HWP_GPIO_T;

#define hwp_gpio                   ((HWP_GPIO_T*) KSEG1(REG_GPIO_BASE))


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

