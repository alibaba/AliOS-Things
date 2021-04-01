/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */
#ifndef __REG_DSPPLL_BEST2001_H__
#define __REG_DSPPLL_BEST2001_H__

#include "plat_types.h"

enum DSPPLL_REG_T {
    DSPPLL_REG_30   = 0x30,
    DSPPLL_REG_31,
    DSPPLL_REG_32,
    DSPPLL_REG_33,
    DSPPLL_REG_34,
    DSPPLL_REG_35,
    DSPPLL_REG_36,
    DSPPLL_REG_37,
    DSPPLL_REG_38,
    DSPPLL_REG_39,
    DSPPLL_REG_3A,
    DSPPLL_REG_3B,
    DSPPLL_REG_3C,
    DSPPLL_REG_3D,
    DSPPLL_REG_3E,
    DSPPLL_REG_3F,
    DSPPLL_REG_40,
    DSPPLL_REG_41,

    DSPPLL_REG_52   = 0x52,

    DSPPLL_REG_60   = 0x60,
};

// reg_30
#define REG_A7PLL_VCO_CAL_VT_SHIFT          0
#define REG_A7PLL_VCO_CAL_VT_MASK           (0x3 << REG_A7PLL_VCO_CAL_VT_SHIFT)
#define REG_A7PLL_VCO_CAL_VT(n)             BITFIELD_VAL(REG_A7PLL_VCO_CAL_VT, n)
#define REG_A7PLL_1M_PULSE_MODE             (1 << 2)

// reg_31
#define REG_A7PLL_CAL_CLK_SEL               (1 << 0)
#define REG_A7PLL_CPMODE                    (1 << 1)

// reg_32
#define REG_A7PLL_BUF_VRES_SHIFT            0
#define REG_A7PLL_BUF_VRES_MASK             (0x7 << REG_A7PLL_BUF_VRES_SHIFT)
#define REG_A7PLL_BUF_VRES(n)               BITFIELD_VAL(REG_A7PLL_BUF_VRES, n)
#define REG_A7PLL_BW_SEL_SHIFT              3
#define REG_A7PLL_BW_SEL_MASK               (0xF << REG_A7PLL_BW_SEL_SHIFT)
#define REG_A7PLL_BW_SEL(n)                 BITFIELD_VAL(REG_A7PLL_BW_SEL, n)
#define REG_A7PLL_DIV_DLYCAP_SHIFT          7
#define REG_A7PLL_DIV_DLYCAP_MASK           (0xF << REG_A7PLL_DIV_DLYCAP_SHIFT)
#define REG_A7PLL_DIV_DLYCAP(n)             BITFIELD_VAL(REG_A7PLL_DIV_DLYCAP, n)
#define REG_A7PLL_POSTDIV_SHIFT             11
#define REG_A7PLL_POSTDIV_MASK              (0x1F << REG_A7PLL_POSTDIV_SHIFT)
#define REG_A7PLL_POSTDIV(n)                BITFIELD_VAL(REG_A7PLL_POSTDIV, n)

// reg_33
#define REG_A7PLL_CP_VRES_SHIFT             0
#define REG_A7PLL_CP_VRES_MASK              (0x7 << REG_A7PLL_CP_VRES_SHIFT)
#define REG_A7PLL_CP_VRES(n)                BITFIELD_VAL(REG_A7PLL_CP_VRES, n)
#define REG_A7PLL_DIG_VRES_SHIFT            3
#define REG_A7PLL_DIG_VRES_MASK             (0x7 << REG_A7PLL_DIG_VRES_SHIFT)
#define REG_A7PLL_DIG_VRES(n)               BITFIELD_VAL(REG_A7PLL_DIG_VRES, n)
#define REG_A7PLL_DIV_FBC_SEL_SHIFT         6
#define REG_A7PLL_DIV_FBC_SEL_MASK          (0x7 << REG_A7PLL_DIV_FBC_SEL_SHIFT)
#define REG_A7PLL_DIV_FBC_SEL(n)            BITFIELD_VAL(REG_A7PLL_DIV_FBC_SEL, n)
#define REG_A7PLL_FBRES_SHIFT               9
#define REG_A7PLL_FBRES_MASK                (0x7 << REG_A7PLL_FBRES_SHIFT)
#define REG_A7PLL_FBRES(n)                  BITFIELD_VAL(REG_A7PLL_FBRES, n)
#define REG_A7PLL_ICP_SHIFT                 12
#define REG_A7PLL_ICP_MASK                  (0xF << REG_A7PLL_ICP_SHIFT)
#define REG_A7PLL_ICP(n)                    BITFIELD_VAL(REG_A7PLL_ICP, n)

// reg_34
#define REG_A7PLL_IOFST_SHIFT               0
#define REG_A7PLL_IOFST_MASK                (0xF << REG_A7PLL_IOFST_SHIFT)
#define REG_A7PLL_IOFST(n)                  BITFIELD_VAL(REG_A7PLL_IOFST, n)
#define REG_A7PLL_LOCKDET_BYPASS            (1 << 4)
#define REG_A7PLL_LOCKDET_WIN_SHIFT         5
#define REG_A7PLL_LOCKDET_WIN_MASK          (0x7 << REG_A7PLL_LOCKDET_WIN_SHIFT)
#define REG_A7PLL_LOCKDET_WIN(n)            BITFIELD_VAL(REG_A7PLL_LOCKDET_WIN, n)
#define REG_A7PLL_VAR_BIAS_SHIFT            8
#define REG_A7PLL_VAR_BIAS_MASK             (0x7 << REG_A7PLL_VAR_BIAS_SHIFT)
#define REG_A7PLL_VAR_BIAS(n)               BITFIELD_VAL(REG_A7PLL_VAR_BIAS, n)
#define REG_A7PLL_TST_VCTL_SHIFT            11
#define REG_A7PLL_TST_VCTL_MASK             (0xF << REG_A7PLL_TST_VCTL_SHIFT)
#define REG_A7PLL_TST_VCTL(n)               BITFIELD_VAL(REG_A7PLL_TST_VCTL, n)

// reg_35
#define REG_A7PLL_VCO_ICTRL_SHIFT           0
#define REG_A7PLL_VCO_ICTRL_MASK            (0xF << REG_A7PLL_VCO_ICTRL_SHIFT)
#define REG_A7PLL_VCO_ICTRL(n)              BITFIELD_VAL(REG_A7PLL_VCO_ICTRL, n)
#define REG_A7PLL_VCO_VRES_SHIFT            4
#define REG_A7PLL_VCO_VRES_MASK             (0x7 << REG_A7PLL_VCO_VRES_SHIFT)
#define REG_A7PLL_VCO_VRES(n)               BITFIELD_VAL(REG_A7PLL_VCO_VRES, n)
#define REG_A7PLL_REFCLK_SEL_SHIFT          7
#define REG_A7PLL_REFCLK_SEL_MASK           (0x3 << REG_A7PLL_REFCLK_SEL_SHIFT)
#define REG_A7PLL_REFCLK_SEL(n)             BITFIELD_VAL(REG_A7PLL_REFCLK_SEL, n)

// reg_36
#define RESERVED_2_SHIFT                    0
#define RESERVED_2_MASK                     (0xFFFF << RESERVED_2_SHIFT)
#define RESERVED_2(n)                       BITFIELD_VAL(RESERVED_2, n)

// reg_37
#define REG_A7PLL_CAL_OPT                   (1 << 0)
#define REG_A7PLL_INIT_DELAY_SHIFT          1
#define REG_A7PLL_INIT_DELAY_MASK           (0x7 << REG_A7PLL_INIT_DELAY_SHIFT)
#define REG_A7PLL_INIT_DELAY(n)             BITFIELD_VAL(REG_A7PLL_INIT_DELAY, n)
#define REG_A7PLL_CNT_TIME_SHIFT            4
#define REG_A7PLL_CNT_TIME_MASK             (0x7 << REG_A7PLL_CNT_TIME_SHIFT)
#define REG_A7PLL_CNT_TIME(n)               BITFIELD_VAL(REG_A7PLL_CNT_TIME, n)
#define REG_A7PLL_WAIT_TIME_SHIFT           7
#define REG_A7PLL_WAIT_TIME_MASK            (0x7 << REG_A7PLL_WAIT_TIME_SHIFT)
#define REG_A7PLL_WAIT_TIME(n)              BITFIELD_VAL(REG_A7PLL_WAIT_TIME, n)
#define REG_A7PLL_CAL_MANUAL                (1 << 10)
#define REG_A7PLL_CAL_CNT_EN_DR             (1 << 11)
#define REG_A7PLL_CAL_CNT_EN                (1 << 12)
#define REG_A7PLL_CAL_EN_DR                 (1 << 13)
#define REG_A7PLL_CAL_EN                    (1 << 14)

// reg_38
#define REG_A7PLL_VCO_CAPBANK_DR            (1 << 0)
#define REG_A7PLL_VCO_CAPBANK_SHIFT         1
#define REG_A7PLL_VCO_CAPBANK_MASK          (0x3FF << REG_A7PLL_VCO_CAPBANK_SHIFT)
#define REG_A7PLL_VCO_CAPBANK(n)            BITFIELD_VAL(REG_A7PLL_VCO_CAPBANK, n)
#define REG_A7PLL_MULT_SHIFT                11
#define REG_A7PLL_MULT_MASK                 (0x7 << REG_A7PLL_MULT_SHIFT)
#define REG_A7PLL_MULT(n)                   BITFIELD_VAL(REG_A7PLL_MULT, n)
#define REG_A7PLL_CAL_CLK_FREQ_SHIFT        14
#define REG_A7PLL_CAL_CLK_FREQ_MASK         (0x3 << REG_A7PLL_CAL_CLK_FREQ_SHIFT)
#define REG_A7PLL_CAL_CLK_FREQ(n)           BITFIELD_VAL(REG_A7PLL_CAL_CLK_FREQ, n)

// reg_39
#define REG_A7PLL_SDM_FREQWORD_15_0_SHIFT   0
#define REG_A7PLL_SDM_FREQWORD_15_0_MASK    (0xFFFF << REG_A7PLL_SDM_FREQWORD_15_0_SHIFT)
#define REG_A7PLL_SDM_FREQWORD_15_0(n)      BITFIELD_VAL(REG_A7PLL_SDM_FREQWORD_15_0, n)

// reg_3a
#define REG_A7PLL_SDM_FREQWORD_31_16_SHIFT  0
#define REG_A7PLL_SDM_FREQWORD_31_16_MASK   (0xFFFF << REG_A7PLL_SDM_FREQWORD_31_16_SHIFT)
#define REG_A7PLL_SDM_FREQWORD_31_16(n)     BITFIELD_VAL(REG_A7PLL_SDM_FREQWORD_31_16, n)

// reg_3b
#define REG_A7PLL_PU_DR                     (1 << 0)
#define REG_A7PLL_PU                        (1 << 1)
#define REG_A7PLL_LDO_PU_DR                 (1 << 2)
#define REG_A7PLL_LDO_PU                    (1 << 3)
#define REG_A7PLL_PRECHARGE_DR              (1 << 4)
#define REG_A7PLL_PRECHARGE                 (1 << 5)
#define REG_A7PLL_LPF_PRECHARGE_DR          (1 << 6)
#define REG_A7PLL_LPF_PRECHARGE             (1 << 7)
#define REG_A7PLL_LOOP_RESETB_DR            (1 << 8)
#define REG_A7PLL_LOOP_RESETB               (1 << 9)
#define REG_A7PLL_CAPBANK_CAL_BYPASS        (1 << 10)
#define REG_A7PLL_CAL_RESETN_DR             (1 << 11)
#define REG_A7PLL_CAL_RESETN                (1 << 12)
#define REG_A7PLL_SDM_RESETN_DR             (1 << 13)
#define REG_A7PLL_SDM_RESETN                (1 << 14)

// reg_3c
#define REG_A7PLL_CLK_FBC_EDGE              (1 << 0)
#define REG_A7PLL_INT_DEC_SEL_SHIFT         1
#define REG_A7PLL_INT_DEC_SEL_MASK          (0x7 << REG_A7PLL_INT_DEC_SEL_SHIFT)
#define REG_A7PLL_INT_DEC_SEL(n)            BITFIELD_VAL(REG_A7PLL_INT_DEC_SEL, n)
#define REG_A7PLL_PRESCALER_DEL_SHIFT       4
#define REG_A7PLL_PRESCALER_DEL_MASK        (0xF << REG_A7PLL_PRESCALER_DEL_SHIFT)
#define REG_A7PLL_PRESCALER_DEL(n)          BITFIELD_VAL(REG_A7PLL_PRESCALER_DEL, n)
#define REG_A7PLL_DITHER_BYPASS             (1 << 8)
#define REG_A7PLL_SDM_CLK_SEL_DR            (1 << 9)
#define REG_A7PLL_SDM_CLK_SEL               (1 << 10)

// reg_3d
#define REG_A7PLL_DIV_DR                    (1 << 0)
#define REG_A7PLL_DIV_INT_SHIFT             1
#define REG_A7PLL_DIV_INT_MASK              (0x7F << REG_A7PLL_DIV_INT_SHIFT)
#define REG_A7PLL_DIV_INT(n)                BITFIELD_VAL(REG_A7PLL_DIV_INT, n)
#define REG_A7PLL_DIV_FRAC_SHIFT            8
#define REG_A7PLL_DIV_FRAC_MASK             (0x3 << REG_A7PLL_DIV_FRAC_SHIFT)
#define REG_A7PLL_DIV_FRAC(n)               BITFIELD_VAL(REG_A7PLL_DIV_FRAC, n)
#define REG_A7PLL_CAL_CLK_EN_DR             (1 << 10)
#define REG_A7PLL_CAL_CLK_EN                (1 << 11)

// reg_3e
#define REG_A7PLL_WAIT_T1_VALUE_SHIFT       0
#define REG_A7PLL_WAIT_T1_VALUE_MASK        (0xFF << REG_A7PLL_WAIT_T1_VALUE_SHIFT)
#define REG_A7PLL_WAIT_T1_VALUE(n)          BITFIELD_VAL(REG_A7PLL_WAIT_T1_VALUE, n)
#define REG_A7PLL_WAIT_T2_VALUE_SHIFT       8
#define REG_A7PLL_WAIT_T2_VALUE_MASK        (0xFF << REG_A7PLL_WAIT_T2_VALUE_SHIFT)
#define REG_A7PLL_WAIT_T2_VALUE(n)          BITFIELD_VAL(REG_A7PLL_WAIT_T2_VALUE, n)

// reg_3f
#define REG_A7PLL_WAIT_T3_VALUE_SHIFT       0
#define REG_A7PLL_WAIT_T3_VALUE_MASK        (0xFF << REG_A7PLL_WAIT_T3_VALUE_SHIFT)
#define REG_A7PLL_WAIT_T3_VALUE(n)          BITFIELD_VAL(REG_A7PLL_WAIT_T3_VALUE, n)
#define REG_A7PLL_WAIT_T4_VALUE_SHIFT       8
#define REG_A7PLL_WAIT_T4_VALUE_MASK        (0xFF << REG_A7PLL_WAIT_T4_VALUE_SHIFT)
#define REG_A7PLL_WAIT_T4_VALUE(n)          BITFIELD_VAL(REG_A7PLL_WAIT_T4_VALUE, n)

// reg_40
#define REG_A7PLL_FREQ_OFFSET_SHIFT         0
#define REG_A7PLL_FREQ_OFFSET_MASK          (0xFFFF << REG_A7PLL_FREQ_OFFSET_SHIFT)
#define REG_A7PLL_FREQ_OFFSET(n)            BITFIELD_VAL(REG_A7PLL_FREQ_OFFSET, n)

// reg_41
#define REG_A7PLL_FREQ_EN                   (1 << 0)
#define REG_A7PLL_SSC_ENABLE                (1 << 1)
#define REG_A7PLL_TRI_FREQ_OFST_SEL_SHIFT   2
#define REG_A7PLL_TRI_FREQ_OFST_SEL_MASK    (0x7 << REG_A7PLL_TRI_FREQ_OFST_SEL_SHIFT)
#define REG_A7PLL_TRI_FREQ_OFST_SEL(n)      BITFIELD_VAL(REG_A7PLL_TRI_FREQ_OFST_SEL, n)
#define REG_A7PLL_TRI_FREQ_SEL_SHIFT        5
#define REG_A7PLL_TRI_FREQ_SEL_MASK         (0x3 << REG_A7PLL_TRI_FREQ_SEL_SHIFT)
#define REG_A7PLL_TRI_FREQ_SEL(n)           BITFIELD_VAL(REG_A7PLL_TRI_FREQ_SEL, n)
#define REG_A7PLL_TRI_STEP_SEL_SHIFT        7
#define REG_A7PLL_TRI_STEP_SEL_MASK         (0x3 << REG_A7PLL_TRI_STEP_SEL_SHIFT)
#define REG_A7PLL_TRI_STEP_SEL(n)           BITFIELD_VAL(REG_A7PLL_TRI_STEP_SEL, n)
#define REG_A7PLL_TRI_OUT_DIV2              (1 << 9)

// reg_52
#define RESERVED_1_SHIFT                    0
#define RESERVED_1_MASK                     (0xFFFF << RESERVED_1_SHIFT)
#define RESERVED_1(n)                       BITFIELD_VAL(RESERVED_1, n)

// reg_60
#define DSPPLL_CHIP_ADDR_I2C_SHIFT          0
#define DSPPLL_CHIP_ADDR_I2C_MASK           (0x7F << DSPPLL_CHIP_ADDR_I2C_SHIFT)
#define DSPPLL_CHIP_ADDR_I2C(n)             BITFIELD_VAL(DSPPLL_CHIP_ADDR_I2C, n)

#endif
