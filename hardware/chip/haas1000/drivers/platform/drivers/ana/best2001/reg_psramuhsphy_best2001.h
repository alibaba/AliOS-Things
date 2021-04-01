/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */
#ifndef __REG_PSRAMUHSPHY_BEST2001_H__
#define __REG_PSRAMUHSPHY_BEST2001_H__

#include "plat_types.h"

enum PSRAMPHY_REG_T {
    PSRAMPHY_REG_00   = 0x00,
    PSRAMPHY_REG_01,
    PSRAMPHY_REG_02,
    PSRAMPHY_REG_03,
    PSRAMPHY_REG_04,
    PSRAMPHY_REG_05,
    PSRAMPHY_REG_06,
    PSRAMPHY_REG_07,
    PSRAMPHY_REG_08,
    PSRAMPHY_REG_09,
    PSRAMPHY_REG_0A,
    PSRAMPHY_REG_0B,
    PSRAMPHY_REG_0C,
    PSRAMPHY_REG_0D,
    PSRAMPHY_REG_0E,
    PSRAMPHY_REG_0F,
    PSRAMPHY_REG_10,
    PSRAMPHY_REG_11,
    PSRAMPHY_REG_31 = 0x31,
    PSRAMPHY_REG_32,
};

// reg_00
#define REVID_SHIFT                         0
#define REVID_MASK                          (0xF << REVID_SHIFT)
#define REVID(n)                            BITFIELD_VAL(REVID, n)

// reg_01
#define REG_LDO_PRECHARGE                   (1 << 0)
#define REG_LDO_PU                          (1 << 1)
#define REG_PSRAM_PU                        (1 << 2)
#define REG_RESETB                          (1 << 3)
#define REG_CLK_RDY                         (1 << 4)
#define REG_DLL_PU                          (1 << 5)

// reg_02
#define REG_LDO_VTUNE_SHIFT                 0
#define REG_LDO_VTUNE_MASK                  (0x7 << REG_LDO_VTUNE_SHIFT)
#define REG_LDO_VTUNE(n)                    BITFIELD_VAL(REG_LDO_VTUNE, n)
#define REG_VREF_VTUNE_SHIFT                3
#define REG_VREF_VTUNE_MASK                 (0x7 << REG_VREF_VTUNE_SHIFT)
#define REG_VREF_VTUNE(n)                   BITFIELD_VAL(REG_VREF_VTUNE, n)
#define REG_LDO_IEN1_SHIFT                  6
#define REG_LDO_IEN1_MASK                   (0xF << REG_LDO_IEN1_SHIFT)
#define REG_LDO_IEN1(n)                     BITFIELD_VAL(REG_LDO_IEN1, n)
#define REG_LDO_IEN2_SHIFT                  10
#define REG_LDO_IEN2_MASK                   (0xF << REG_LDO_IEN2_SHIFT)
#define REG_LDO_IEN2(n)                     BITFIELD_VAL(REG_LDO_IEN2, n)
#define REG_PSRAM_SWRC_SHIFT                14
#define REG_PSRAM_SWRC_MASK                 (0x3 << REG_PSRAM_SWRC_SHIFT)
#define REG_PSRAM_SWRC(n)                   BITFIELD_VAL(REG_PSRAM_SWRC, n)

// reg_03
#define REG_PSRAM_ODT_SHIFT                 0
#define REG_PSRAM_ODT_MASK                  (0x3 << REG_PSRAM_ODT_SHIFT)
#define REG_PSRAM_ODT(n)                    BITFIELD_VAL(REG_PSRAM_ODT, n)
#define REG_DLL_SWRC_SHIFT                  2
#define REG_DLL_SWRC_MASK                   (0x3 << REG_DLL_SWRC_SHIFT)
#define REG_DLL_SWRC(n)                     BITFIELD_VAL(REG_DLL_SWRC, n)
#define REG_DLL_RANGE_SHIFT                 4
#define REG_DLL_RANGE_MASK                  (0x3 << REG_DLL_RANGE_SHIFT)
#define REG_DLL_RANGE(n)                    BITFIELD_VAL(REG_DLL_RANGE, n)
#define DLL_LOCK                            (1 << 6)
#define DLL_ALL_ONE                         (1 << 7)
#define DLL_ALL_ZERO                        (1 << 8)
#define DLL_DLY_IN_SHIFT                    9
#define DLL_DLY_IN_MASK                     (0x3F << DLL_DLY_IN_SHIFT)
#define DLL_DLY_IN(n)                       BITFIELD_VAL(DLL_DLY_IN, n)

// reg_04
#define REG_DLL_DLY_INI_SHIFT               0
#define REG_DLL_DLY_INI_MASK                (0xFF << REG_DLL_DLY_INI_SHIFT)
#define REG_DLL_DLY_INI(n)                  BITFIELD_VAL(REG_DLL_DLY_INI, n)
#define REG_DLL_SHIFT                       8
#define REG_DLL_MASK                        (0xFF << REG_DLL_SHIFT)
#define REG_DLL(n)                          BITFIELD_VAL(REG_DLL, n)

// reg_05
#define REG_PSRAM_TXDRV_SHIFT               0
#define REG_PSRAM_TXDRV_MASK                (0x7 << REG_PSRAM_TXDRV_SHIFT)
#define REG_PSRAM_TXDRV(n)                  BITFIELD_VAL(REG_PSRAM_TXDRV, n)
#define REG_PSRAM_RX_BIAS_SHIFT             3
#define REG_PSRAM_RX_BIAS_MASK              (0x3 << REG_PSRAM_RX_BIAS_SHIFT)
#define REG_PSRAM_RX_BIAS(n)                BITFIELD_VAL(REG_PSRAM_RX_BIAS, n)
#define REG_PSRAM_PULLUP                    (1 << 5)
#define REG_PSRAM_PULLDN                    (1 << 6)
#define REG_PSRAM_PULLUP_DQS_SHIFT          7
#define REG_PSRAM_PULLUP_DQS_MASK           (0x3 << REG_PSRAM_PULLUP_DQS_SHIFT)
#define REG_PSRAM_PULLUP_DQS(n)             BITFIELD_VAL(REG_PSRAM_PULLUP_DQS, n)
#define REG_PSRAM_PULLDN_DQS_SHIFT          9
#define REG_PSRAM_PULLDN_DQS_MASK           (0x3 << REG_PSRAM_PULLDN_DQS_SHIFT)
#define REG_PSRAM_PULLDN_DQS(n)             BITFIELD_VAL(REG_PSRAM_PULLDN_DQS, n)
#define REG_PSRAM_TX_WR_CK_SEL              (1 << 11)

// reg_06
#define REG_PSRAM_TX_DM_DLY_SHIFT           0
#define REG_PSRAM_TX_DM_DLY_MASK            (0x1F << REG_PSRAM_TX_DM_DLY_SHIFT)
#define REG_PSRAM_TX_DM_DLY(n)              BITFIELD_VAL(REG_PSRAM_TX_DM_DLY, n)
#define REG_PSRAM_TX_DQ_DLY_4_0_SHIFT       5
#define REG_PSRAM_TX_DQ_DLY_4_0_MASK        (0x1F << REG_PSRAM_TX_DQ_DLY_4_0_SHIFT)
#define REG_PSRAM_TX_DQ_DLY_4_0(n)          BITFIELD_VAL(REG_PSRAM_TX_DQ_DLY_4_0, n)
#define REG_PSRAM_TX_DQ_DLY_9_5_SHIFT       10
#define REG_PSRAM_TX_DQ_DLY_9_5_MASK        (0x1F << REG_PSRAM_TX_DQ_DLY_9_5_SHIFT)
#define REG_PSRAM_TX_DQ_DLY_9_5(n)          BITFIELD_VAL(REG_PSRAM_TX_DQ_DLY_9_5, n)

// reg_07
#define REG_PSRAM_TX_DQ_DLY_14_10_SHIFT     0
#define REG_PSRAM_TX_DQ_DLY_14_10_MASK      (0x1F << REG_PSRAM_TX_DQ_DLY_14_10_SHIFT)
#define REG_PSRAM_TX_DQ_DLY_14_10(n)        BITFIELD_VAL(REG_PSRAM_TX_DQ_DLY_14_10, n)
#define REG_PSRAM_TX_DQ_DLY_19_15_SHIFT     5
#define REG_PSRAM_TX_DQ_DLY_19_15_MASK      (0x1F << REG_PSRAM_TX_DQ_DLY_19_15_SHIFT)
#define REG_PSRAM_TX_DQ_DLY_19_15(n)        BITFIELD_VAL(REG_PSRAM_TX_DQ_DLY_19_15, n)
#define REG_PSRAM_TX_DQ_DLY_24_20_SHIFT     10
#define REG_PSRAM_TX_DQ_DLY_24_20_MASK      (0x1F << REG_PSRAM_TX_DQ_DLY_24_20_SHIFT)
#define REG_PSRAM_TX_DQ_DLY_24_20(n)        BITFIELD_VAL(REG_PSRAM_TX_DQ_DLY_24_20, n)

// reg_08
#define REG_PSRAM_TX_DQ_DLY_29_25_SHIFT     0
#define REG_PSRAM_TX_DQ_DLY_29_25_MASK      (0x1F << REG_PSRAM_TX_DQ_DLY_29_25_SHIFT)
#define REG_PSRAM_TX_DQ_DLY_29_25(n)        BITFIELD_VAL(REG_PSRAM_TX_DQ_DLY_29_25, n)
#define REG_PSRAM_TX_DQ_DLY_34_30_SHIFT     5
#define REG_PSRAM_TX_DQ_DLY_34_30_MASK      (0x1F << REG_PSRAM_TX_DQ_DLY_34_30_SHIFT)
#define REG_PSRAM_TX_DQ_DLY_34_30(n)        BITFIELD_VAL(REG_PSRAM_TX_DQ_DLY_34_30, n)
#define REG_PSRAM_TX_DQ_DLY_39_35_SHIFT     10
#define REG_PSRAM_TX_DQ_DLY_39_35_MASK      (0x1F << REG_PSRAM_TX_DQ_DLY_39_35_SHIFT)
#define REG_PSRAM_TX_DQ_DLY_39_35(n)        BITFIELD_VAL(REG_PSRAM_TX_DQ_DLY_39_35, n)

// reg_09
#define REG_PSRAM_TX_CMD_DLY_4_0_SHIFT      0
#define REG_PSRAM_TX_CMD_DLY_4_0_MASK       (0x1F << REG_PSRAM_TX_CMD_DLY_4_0_SHIFT)
#define REG_PSRAM_TX_CMD_DLY_4_0(n)         BITFIELD_VAL(REG_PSRAM_TX_CMD_DLY_4_0, n)
#define REG_PSRAM_TX_CMD_DLY_9_5_SHIFT      5
#define REG_PSRAM_TX_CMD_DLY_9_5_MASK       (0x1F << REG_PSRAM_TX_CMD_DLY_9_5_SHIFT)
#define REG_PSRAM_TX_CMD_DLY_9_5(n)         BITFIELD_VAL(REG_PSRAM_TX_CMD_DLY_9_5, n)
#define REG_PSRAM_TX_CMD_DLY_14_10_SHIFT    10
#define REG_PSRAM_TX_CMD_DLY_14_10_MASK     (0x1F << REG_PSRAM_TX_CMD_DLY_14_10_SHIFT)
#define REG_PSRAM_TX_CMD_DLY_14_10(n)       BITFIELD_VAL(REG_PSRAM_TX_CMD_DLY_14_10, n)

// reg_0a
#define REG_PSRAM_TX_CMD_DLY_19_15_SHIFT    0
#define REG_PSRAM_TX_CMD_DLY_19_15_MASK     (0x1F << REG_PSRAM_TX_CMD_DLY_19_15_SHIFT)
#define REG_PSRAM_TX_CMD_DLY_19_15(n)       BITFIELD_VAL(REG_PSRAM_TX_CMD_DLY_19_15, n)
#define REG_PSRAM_TX_CMD_DLY_24_20_SHIFT    5
#define REG_PSRAM_TX_CMD_DLY_24_20_MASK     (0x1F << REG_PSRAM_TX_CMD_DLY_24_20_SHIFT)
#define REG_PSRAM_TX_CMD_DLY_24_20(n)       BITFIELD_VAL(REG_PSRAM_TX_CMD_DLY_24_20, n)
#define REG_PSRAM_TX_CMD_DLY_29_25_SHIFT    10
#define REG_PSRAM_TX_CMD_DLY_29_25_MASK     (0x1F << REG_PSRAM_TX_CMD_DLY_29_25_SHIFT)
#define REG_PSRAM_TX_CMD_DLY_29_25(n)       BITFIELD_VAL(REG_PSRAM_TX_CMD_DLY_29_25, n)

// reg_0b
#define REG_PSRAM_TX_CMD_DLY_34_30_SHIFT    0
#define REG_PSRAM_TX_CMD_DLY_34_30_MASK     (0x1F << REG_PSRAM_TX_CMD_DLY_34_30_SHIFT)
#define REG_PSRAM_TX_CMD_DLY_34_30(n)       BITFIELD_VAL(REG_PSRAM_TX_CMD_DLY_34_30, n)
#define REG_PSRAM_TX_CMD_DLY_39_35_SHIFT    5
#define REG_PSRAM_TX_CMD_DLY_39_35_MASK     (0x1F << REG_PSRAM_TX_CMD_DLY_39_35_SHIFT)
#define REG_PSRAM_TX_CMD_DLY_39_35(n)       BITFIELD_VAL(REG_PSRAM_TX_CMD_DLY_39_35, n)

// reg_0c
#define REG_PSRAM_TX_DQS_DLY_SHIFT          0
#define REG_PSRAM_TX_DQS_DLY_MASK           (0x1F << REG_PSRAM_TX_DQS_DLY_SHIFT)
#define REG_PSRAM_TX_DQS_DLY(n)             BITFIELD_VAL(REG_PSRAM_TX_DQS_DLY, n)
#define REG_PSRAM_RX_ADQ_DLY_4_0_SHIFT      5
#define REG_PSRAM_RX_ADQ_DLY_4_0_MASK       (0x1F << REG_PSRAM_RX_ADQ_DLY_4_0_SHIFT)
#define REG_PSRAM_RX_ADQ_DLY_4_0(n)         BITFIELD_VAL(REG_PSRAM_RX_ADQ_DLY_4_0, n)
#define REG_PSRAM_RX_ADQ_DLY_9_5_SHIFT      10
#define REG_PSRAM_RX_ADQ_DLY_9_5_MASK       (0x1F << REG_PSRAM_RX_ADQ_DLY_9_5_SHIFT)
#define REG_PSRAM_RX_ADQ_DLY_9_5(n)         BITFIELD_VAL(REG_PSRAM_RX_ADQ_DLY_9_5, n)

// reg_0d
#define REG_PSRAM_RX_ADQ_DLY_14_10_SHIFT    0
#define REG_PSRAM_RX_ADQ_DLY_14_10_MASK     (0x1F << REG_PSRAM_RX_ADQ_DLY_14_10_SHIFT)
#define REG_PSRAM_RX_ADQ_DLY_14_10(n)       BITFIELD_VAL(REG_PSRAM_RX_ADQ_DLY_14_10, n)
#define REG_PSRAM_RX_ADQ_DLY_19_15_SHIFT    5
#define REG_PSRAM_RX_ADQ_DLY_19_15_MASK     (0x1F << REG_PSRAM_RX_ADQ_DLY_19_15_SHIFT)
#define REG_PSRAM_RX_ADQ_DLY_19_15(n)       BITFIELD_VAL(REG_PSRAM_RX_ADQ_DLY_19_15, n)
#define REG_PSRAM_RX_ADQ_DLY_24_20_SHIFT    10
#define REG_PSRAM_RX_ADQ_DLY_24_20_MASK     (0x1F << REG_PSRAM_RX_ADQ_DLY_24_20_SHIFT)
#define REG_PSRAM_RX_ADQ_DLY_24_20(n)       BITFIELD_VAL(REG_PSRAM_RX_ADQ_DLY_24_20, n)

// reg_0e
#define REG_PSRAM_RX_ADQ_DLY_29_25_SHIFT    0
#define REG_PSRAM_RX_ADQ_DLY_29_25_MASK     (0x1F << REG_PSRAM_RX_ADQ_DLY_29_25_SHIFT)
#define REG_PSRAM_RX_ADQ_DLY_29_25(n)       BITFIELD_VAL(REG_PSRAM_RX_ADQ_DLY_29_25, n)
#define REG_PSRAM_RX_ADQ_DLY_34_30_SHIFT    5
#define REG_PSRAM_RX_ADQ_DLY_34_30_MASK     (0x1F << REG_PSRAM_RX_ADQ_DLY_34_30_SHIFT)
#define REG_PSRAM_RX_ADQ_DLY_34_30(n)       BITFIELD_VAL(REG_PSRAM_RX_ADQ_DLY_34_30, n)
#define REG_PSRAM_RX_ADQ_DLY_39_35_SHIFT    10
#define REG_PSRAM_RX_ADQ_DLY_39_35_MASK     (0x1F << REG_PSRAM_RX_ADQ_DLY_39_35_SHIFT)
#define REG_PSRAM_RX_ADQ_DLY_39_35(n)       BITFIELD_VAL(REG_PSRAM_RX_ADQ_DLY_39_35, n)

// reg_0f
#define REG_PSRAM_RX_DQS_DLY_SHIFT          0
#define REG_PSRAM_RX_DQS_DLY_MASK           (0x1F << REG_PSRAM_RX_DQS_DLY_SHIFT)
#define REG_PSRAM_RX_DQS_DLY(n)             BITFIELD_VAL(REG_PSRAM_RX_DQS_DLY, n)
#define REG_PSRAM_TX_CLK_DLY_SHIFT          5
#define REG_PSRAM_TX_CLK_DLY_MASK           (0x1F << REG_PSRAM_TX_CLK_DLY_SHIFT)
#define REG_PSRAM_TX_CLK_DLY(n)             BITFIELD_VAL(REG_PSRAM_TX_CLK_DLY, n)
#define REG_PSRAM_TX_CEB_DLY_SHIFT          10
#define REG_PSRAM_TX_CEB_DLY_MASK           (0x1F << REG_PSRAM_TX_CEB_DLY_SHIFT)
#define REG_PSRAM_TX_CEB_DLY(n)             BITFIELD_VAL(REG_PSRAM_TX_CEB_DLY, n)

// reg_10
#define REG_PSRAM_OCD_CLKP_SHIFT            0
#define REG_PSRAM_OCD_CLKP_MASK             (0x7 << REG_PSRAM_OCD_CLKP_SHIFT)
#define REG_PSRAM_OCD_CLKP(n)               BITFIELD_VAL(REG_PSRAM_OCD_CLKP, n)
#define REG_PSRAM_OCD_CLKN_SHIFT            3
#define REG_PSRAM_OCD_CLKN_MASK             (0x7 << REG_PSRAM_OCD_CLKN_SHIFT)
#define REG_PSRAM_OCD_CLKN(n)               BITFIELD_VAL(REG_PSRAM_OCD_CLKN, n)
#define REG_PSRAM_OCD_DQSP_SHIFT            6
#define REG_PSRAM_OCD_DQSP_MASK             (0x7 << REG_PSRAM_OCD_DQSP_SHIFT)
#define REG_PSRAM_OCD_DQSP(n)               BITFIELD_VAL(REG_PSRAM_OCD_DQSP, n)
#define REG_PSRAM_OCD_DQSN_SHIFT            9
#define REG_PSRAM_OCD_DQSN_MASK             (0x7 << REG_PSRAM_OCD_DQSN_SHIFT)
#define REG_PSRAM_OCD_DQSN(n)               BITFIELD_VAL(REG_PSRAM_OCD_DQSN, n)

// reg_11
#define REG_PSRAM_TX_RD_CK_SEL              (1 << 0)
#define REG_PSRAM_TX_RD_CK_DLY_SHIFT        1
#define REG_PSRAM_TX_RD_CK_DLY_MASK         (0x7 << REG_PSRAM_TX_RD_CK_DLY_SHIFT)
#define REG_PSRAM_TX_RD_CK_DLY(n)           BITFIELD_VAL(REG_PSRAM_TX_RD_CK_DLY, n)
#define REG_PSRAM_TX_RD_POINTER_DLY_SHIFT   4
#define REG_PSRAM_TX_RD_POINTER_DLY_MASK    (0x7 << REG_PSRAM_TX_RD_POINTER_DLY_SHIFT)
#define REG_PSRAM_TX_RD_POINTER_DLY(n)      BITFIELD_VAL(REG_PSRAM_TX_RD_POINTER_DLY, n)
#define REG_PSRAM_TX_FIFO_TEST_EN           (1 << 7)
#define REG_PSRAM_TX_FIFO_BYPASS            (1 << 8)
#define REG_PSRAM_RX_FIFO_BYPASS            (1 << 9)

// reg_31
#define REG_DLLPLL_PATH_EN_SHIFT            2
#define REG_DLLPLL_PATH_EN_MASK             (0x3 << REG_DLLPLL_PATH_EN_SHIFT)
#define REG_DLLPLL_PATH_EN(n)               BITFIELD_VAL(REG_DLLPLL_PATH_EN, n)

// reg_32
#define REG_DLLPLL_BUF_VRES_SHIFT           0
#define REG_DLLPLL_BUF_VRES_MASK            (0x7 << REG_DLLPLL_BUF_VRES_SHIFT)
#define REG_DLLPLL_BUF_VRES(n)              BITFIELD_VAL(REG_DLLPLL_BUF_VRES, n)

#endif
