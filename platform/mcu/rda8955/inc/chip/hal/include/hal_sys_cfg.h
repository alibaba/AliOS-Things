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



#ifndef _HAL_SYS_CFG_H_
#define _HAL_SYS_CFG_H_

#include "cs_types.h"

//RF Page4
//Register 02h
//1，将PLL的内部数字电压调到最低：pll_vreg_bit<3:0>打到全0  page 4:Register 02h. 9:6    Pll_vreg_bit<3:0>
//2，内部filter运放电流调到最低：pll_filter_ibit<2:0>打到全0, page4:Register 01h. 8:6   Pll_filter_ibit<2:0>    Default: 100
//3，内部cp电流调到最低：pll_cp_bit<2:0>和pll_cpaux_bit<2:0>打到全0 page4:Register 01h. 5:3 Pll_cp_bit<2:0> Default: 100
//4，将内部vco调到频率最低模式：pll_vco_low_test置为1  page4:Register 02h.11    Pll_vco_low_test    Default: 0
//5，关闭时钟输出：pll_clkout_en<1:0>打到全0  page4: Register 01h. 15:12     Register 01h.  Pll_clkout_en<3:0>  Default: 0100

//page 4
#define SYS_RDA6220_Reg101h              0x1

#define SYS_PLL_CP_MASK      0x7
#define SYS_PLL_CP_POS         3
#define SYS_PLL_CP_EN(val)          ((val&SYS_PLL_CP_MASK)<<SYS_PLL_CP_POS)

#define SYS_PLL_FILTER_MASK      0x7
#define SYS_PLL_FILTER_POS         6
#define SYS_PLL_FILTER_EN(val)          ((val&SYS_PLL_FILTER_MASK)<<SYS_PLL_FILTER_POS)

#define SYS_PLL_CPAUX_MASK      0x7
#define SYS_PLL_CPAUX_POS         9
#define SYS_PLL_CPAUX_EN(val)          ((val&SYS_PLL_CPAUX_MASK)<<SYS_PLL_CPAUX_POS)

#define SYS_PLL_CLKOUT_EN_MASK      0xf
#define SYS_PLL_CLKOUT_EN_POS         12
#define SYS_PLL_CLKOUT_EN(val)          ((val&SYS_PLL_CLKOUT_EN_MASK)<<SYS_PLL_CLKOUT_EN_POS)

#define SYS_RDA6220_Reg102h              0x2

#define SYS_PD_PLL_REG(val)          (val<<4)
#define SYS_PD_PLL_DR(val)          (val<<5)

#define SYS_PLL_VREG_MASK 0xf
#define SYS_PLL_VREG(val)           ( (val&SYS_PLL_VREG_MASK)<<6)
#define SYS_PLL_TEST_EN(val)              (val<<10)

//page 1
#define SYS_RDA6220_Reg07H    0x7
#define SYS_RDA6220_Reg20H    0x20

// =============================================================================
// HAL_SYS_PLL_PROFILE_T
// -----------------------------------------------------------------------------
/// Profile register selection.
/// Only flght mode and normal mode are used in the system.
// =============================================================================
typedef enum
{
    HAL_SYS_PLL_MODE_NORMAL = 0,
    HAL_SYS_PLL_MODE_FLIGHT,
    HAL_SYS_PLL_MODE_OFF,
    HAL_SYS_PLL_MODE_QTY,
} HAL_SYS_PLL_PROFILE_T;


// =============================================================================
// HAL_SYS_PLL_CONFIG_T
// -----------------------------------------------------------------------------
/// This structure describes the configuration of the pll structure
// =============================================================================
typedef union
{
    struct
    {
        UINT16 Pll_sdm_clk_sel_nor:1;
        UINT16 Pll_sdm_clk_sel_rst:1;
        UINT16 Pll_int_mode:1;
        UINT16 Pll_cp_bit:3;
        UINT16 Pll_filter_ibit:3;
        UINT16 Pll_cpaux_bit:3;
        UINT16 :2;
        UINT16 Pll_clkout_en:2;
    };
    UINT16 reg;
} HAL_SYS_PLL_CONFIG_01H_T;

typedef union
{
    struct
    {
        UINT16 :2;
        UINT16 Pll_reset_reg:1;
        UINT16 Pll_reset_dr:1;
        UINT16 Pd_pll_reg:1;
        UINT16 Pd_pll_dr:1;
        UINT16 Pll_vreg_bit:4;
        UINT16 Pll_test_en:1;
        UINT16 Pll_vco_low_test:1;
        UINT16 Pll_vco_high_test:1;
        UINT16 Pll_refmulti2:1;
        UINT16 Pll_sdm_clk_test_en:1;
        UINT16 Pll_lock:1;
    };
    UINT16 reg;
} HAL_SYS_PLL_CONFIG_02H_T;


#endif



