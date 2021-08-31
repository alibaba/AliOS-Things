/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __REG_HS_PSRAM_MC_V2_H__
#define __REG_HS_PSRAM_MC_V2_H__

#include "plat_types.h"

struct PSRAMUHS_MC_T {
    __IO uint32_t REG_000;
    __IO uint32_t REG_004;
    __IO uint32_t REG_008;
    __IO uint32_t REG_00C;
    __IO uint32_t REG_010;
    __IO uint32_t REG_014;
    __IO uint32_t REG_018;
    __IO uint32_t REG_01C;
    __IO uint32_t REG_020;
    __IO uint32_t REG_024;
    __IO uint32_t REG_028;
    __IO uint32_t REG_02C;
    __IO uint32_t REG_030;
    __IO uint32_t REG_034;
    __IO uint32_t REG_038;
    __IO uint32_t REG_03C;
    __IO uint32_t REG_040;
    __IO uint32_t REG_044;
    __IO uint32_t REG_048;
    __IO uint32_t REG_04C;
    __IO uint32_t REG_050;
    __IO uint32_t REG_054;
    __IO uint32_t REG_058;
    __IO uint32_t REG_05C;
    __IO uint32_t REG_060;
    __IO uint32_t REG_064;
    __IO uint32_t REG_068;
    __IO uint32_t REG_06C;
    __IO uint32_t REG_070;
    __IO uint32_t REG_074;
    __IO uint32_t REG_078;
    __IO uint32_t REG_07C;
    __IO uint32_t REG_080;
    __IO uint32_t REG_084;
    __IO uint32_t REG_088;
    __IO uint32_t REG_08C;
    __IO uint32_t REG_090;
    __IO uint32_t REG_094;
    __IO uint32_t REG_098;
    __IO uint32_t REG_09C;
    __IO uint32_t REG_0A0;
    __IO uint32_t REG_0A4;
    __IO uint32_t REG_0A8;
    __IO uint32_t REG_0AC;
    __IO uint32_t REG_0B0;
    __IO uint32_t REG_0B4;
    __IO uint32_t REG_0B8;
    __IO uint32_t REG_0BC;
    __IO uint32_t REG_RESERVED_0C0[0x20];
    __IO uint32_t REG_140;
    __IO uint32_t REG_144;
    __IO uint32_t REG_148;
    __IO uint32_t REG_14C;
    __IO uint32_t REG_150;
    __IO uint32_t REG_154;
    __IO uint32_t REG_158;
    __IO uint32_t REG_15C;
    __IO uint32_t REG_160;
    __IO uint32_t REG_RESERVED_164[7];
    __IO uint32_t REG_180;
    __IO uint32_t REG_184;
    __IO uint32_t REG_188;
    __IO uint32_t REG_18C;
    __IO uint32_t REG_190;
    __IO uint32_t REG_194;
    __IO uint32_t REG_RESERVED_198[0x1A];
    __IO uint32_t REG_200;
    __IO uint32_t REG_RESERVED_204[0x7F];
    __IO uint32_t REG_400;
    __IO uint32_t REG_404;
    __IO uint32_t REG_RESERVED_408[0xE];
    __IO uint32_t REG_440;
    __IO uint32_t REG_444;
    __IO uint32_t REG_448;
    __IO uint32_t REG_44C;
    __IO uint32_t REG_450;
    __IO uint32_t REG_454;
    __IO uint32_t REG_458;
    __IO uint32_t REG_45C;
    __IO uint32_t REG_460;
    __IO uint32_t REG_464;
    __IO uint32_t REG_468;
    __IO uint32_t REG_46C;
    __IO uint32_t REG_RESERVED_470[0xF4];
    __IO uint32_t REG_840;
    __IO uint32_t REG_844;
    __IO uint32_t REG_848;
    __IO uint32_t REG_84C;
};

// reg_00
#define PSRAM_UHS_MC_CHIP_BIT                    (1 << 0)
#define PSRAM_UHS_MC_CHIP_TYPE                   (1 << 1)
#define PSRAM_UHS_MC_CHIP_SWITCH                 (1 << 2)
#define PSRAM_UHS_MC_CHIP_IO_X16                 (1 << 3)
#define PSRAM_UHS_MC_CHIP_CA_PATTERN(n)          (((n) & 0x7) << 4)
#define PSRAM_UHS_MC_CHIP_CA_PATTERN_MASK        (0x7 << 4)
#define PSRAM_UHS_MC_CHIP_CA_PATTERN_SHIFT       (4)
#define PSRAM_UHS_MC_CHIP_MEM_SIZE(n)            (((n) & 0x3) << 7)
#define PSRAM_UHS_MC_CHIP_MEM_SIZE_MASK          (0x3 << 7)
#define PSRAM_UHS_MC_CHIP_MEM_SIZE_SHIFT         (7)

// reg_04
#define PSRAM_UHS_MC_MGR_CMD(n)                  (((n) & 0xFF) << 0)
#define PSRAM_UHS_MC_MGR_CMD_MASK                (0xFF << 0)
#define PSRAM_UHS_MC_MGR_CMD_SHIFT               (0)

// reg_08
#define PSRAM_UHS_MC_MGR_ADDR(n)                 (((n) & 0xFFFFFFFF) << 0)
#define PSRAM_UHS_MC_MGR_ADDR_MASK               (0xFFFFFFFF << 0)
#define PSRAM_UHS_MC_MGR_ADDR_SHIFT              (0)

// reg_0c
#define PSRAM_UHS_MC_MGR_LEN(n)                  (((n) & 0xFF) << 0)
#define PSRAM_UHS_MC_MGR_LEN_MASK                (0xFF << 0)
#define PSRAM_UHS_MC_MGR_LEN_SHIFT               (0)

// reg_10
#define PSRAM_UHS_MC_MGR_WSTRB(n)                (((n) & 0xFFFF) << 0)
#define PSRAM_UHS_MC_MGR_WSTRB_MASK              (0xFFFF << 0)
#define PSRAM_UHS_MC_MGR_WSTRB_SHIFT             (0)

// reg_14
#define PSRAM_UHS_MC_MGR_TX_FIFO(n)              (((n) & 0xFFFFFFFF) << 0)
#define PSRAM_UHS_MC_MGR_TX_FIFO_MASK            (0xFFFFFFFF << 0)
#define PSRAM_UHS_MC_MGR_TX_FIFO_SHIFT           (0)

// reg_18
#define PSRAM_UHS_MC_MGR_RX_FIFO(n)              (((n) & 0xFFFFFFFF) << 0)
#define PSRAM_UHS_MC_MGR_RX_FIFO_MASK            (0xFFFFFFFF << 0)
#define PSRAM_UHS_MC_MGR_RX_FIFO_SHIFT           (0)

// reg_1c
#define PSRAM_UHS_MC_MGR_TX_FIFO_CLR             (1 << 0)
#define PSRAM_UHS_MC_MGR_RX_FIFO_CLR             (1 << 1)

// reg_20
#define PSRAM_UHS_MC_REFRESH_MODE                (1 << 0)
#define PSRAM_UHS_MC_BURST_REFRESH_EN            (1 << 1)

// reg_24
#define PSRAM_UHS_MC_ENTRY_SLEEP_IDLE            (1 << 0)
#define PSRAM_UHS_MC_ENTRY_SELF_REFRESH_IDLE     (1 << 1)
#define PSRAM_UHS_MC_STOP_CLK_IDLE               (1 << 2)
#define PSRAM_UHS_MC_AUTOWAKEUP_EN               (1 << 3)
#define PSRAM_UHS_MC_RES_7_4_REG24(n)            (((n) & 0xF) << 4)
#define PSRAM_UHS_MC_RES_7_4_REG24_MASK          (0xF << 4)
#define PSRAM_UHS_MC_RES_7_4_REG24_SHIFT         (4)
#define PSRAM_UHS_MC_PD_MR(n)                    (((n) & 0xFF) << 8)
#define PSRAM_UHS_MC_PD_MR_MASK                  (0xFF << 8)
#define PSRAM_UHS_MC_PD_MR_SHIFT                 (8)

// reg_28
#define PSRAM_UHS_MC_WRITE_LATENCY(n)            (((n) & 0xFF) << 0)
#define PSRAM_UHS_MC_WRITE_LATENCY_MASK          (0xFF << 0)
#define PSRAM_UHS_MC_WRITE_LATENCY_SHIFT         (0)

// reg_2c
#define PSRAM_UHS_MC_READ_LATENCY(n)             (((n) & 0xFF) << 0)
#define PSRAM_UHS_MC_READ_LATENCY_MASK           (0xFF << 0)
#define PSRAM_UHS_MC_READ_LATENCY_SHIFT          (0)

// reg_30
#define PSRAM_UHS_MC_MEMORY_WIDTH(n)             (((n) & 0x3) << 0)
#define PSRAM_UHS_MC_MEMORY_WIDTH_MASK           (0x3 << 0)
#define PSRAM_UHS_MC_MEMORY_WIDTH_SHIFT          (0)

// reg_34
#define PSRAM_UHS_MC_BURST_LENGTH(n)             (((n) & 0x7) << 0)
#define PSRAM_UHS_MC_BURST_LENGTH_MASK           (0x7 << 0)
#define PSRAM_UHS_MC_BURST_LENGTH_SHIFT          (0)
#define PSRAM_UHS_MC_RES_3_3_REG34               (1 << 3)
#define PSRAM_UHS_MC_PAGE_BOUNDARY(n)            (((n) & 0x3) << 4)
#define PSRAM_UHS_MC_PAGE_BOUNDARY_MASK          (0x3 << 4)
#define PSRAM_UHS_MC_PAGE_BOUNDARY_SHIFT         (4)

// reg_38
#define PSRAM_UHS_MC_BUS_WIDTH                   (1 << 0)

// reg_3c
#define PSRAM_UHS_MC_HIGH_PRI_LEVEL(n)           (((n) & 0x1F) << 0)
#define PSRAM_UHS_MC_HIGH_PRI_LEVEL_MASK         (0x1F << 0)
#define PSRAM_UHS_MC_HIGH_PRI_LEVEL_SHIFT        (0)

// reg_40
#define PSRAM_UHS_MC_CP_WRAP_EN                  (1 << 0)
#define PSRAM_UHS_MC_AUTO_PRECHARGE              (1 << 1)
#define PSRAM_UHS_MC_PRA_ENABLE                  (1 << 2)
#define PSRAM_UHS_MC_PRA_MAX_CNT(n)              (((n) & 0xFFFF) << 3)
#define PSRAM_UHS_MC_PRA_MAX_CNT_MASK            (0xFFFF << 3)
#define PSRAM_UHS_MC_PRA_MAX_CNT_SHIFT           (3)

// reg_44
#define PSRAM_UHS_MC_WB_DRAIN                    (1 << 0)
#define PSRAM_UHS_MC_WB_INVALID                  (1 << 1)
#define PSRAM_UHS_MC_RB_INVALID                  (1 << 2)
#define PSRAM_UHS_MC_SNP_DISABLE                 (1 << 3)
#define PSRAM_UHS_MC_BUFFERABLE_WB_EN            (1 << 4)

// reg_48
#define PSRAM_UHS_MC_FRE_RATIO(n)                (((n) & 0x3) << 0)
#define PSRAM_UHS_MC_FRE_RATIO_MASK              (0x3 << 0)
#define PSRAM_UHS_MC_FRE_RATIO_SHIFT             (0)

// reg_4c
#define PSRAM_UHS_MC_T_REFI(n)                   (((n) & 0xFFFF) << 0)
#define PSRAM_UHS_MC_T_REFI_MASK                 (0xFFFF << 0)
#define PSRAM_UHS_MC_T_REFI_SHIFT                (0)
#define PSRAM_UHS_MC_NUM_OF_BURST_RFS(n)         (((n) & 0xFFFF) << 16)
#define PSRAM_UHS_MC_NUM_OF_BURST_RFS_MASK       (0xFFFF << 16)
#define PSRAM_UHS_MC_NUM_OF_BURST_RFS_SHIFT      (16)

// reg_50
#define PSRAM_UHS_MC_T_RC(n)                     (((n) & 0xFF) << 0)
#define PSRAM_UHS_MC_T_RC_MASK                   (0xFF << 0)
#define PSRAM_UHS_MC_T_RC_SHIFT                  (0)

// reg_54
#define PSRAM_UHS_MC_T_RFC(n)                    (((n) & 0xFF) << 0)
#define PSRAM_UHS_MC_T_RFC_MASK                  (0xFF << 0)
#define PSRAM_UHS_MC_T_RFC_SHIFT                 (0)

// reg_58
#define PSRAM_UHS_MC_T_CPHR(n)                   (((n) & 0x3F) << 0)
#define PSRAM_UHS_MC_T_CPHR_MASK                 (0x3F << 0)
#define PSRAM_UHS_MC_T_CPHR_SHIFT                (0)

// reg_5c
#define PSRAM_UHS_MC_T_CPHR_AP(n)                (((n) & 0x3F) << 0)
#define PSRAM_UHS_MC_T_CPHR_AP_MASK              (0x3F << 0)
#define PSRAM_UHS_MC_T_CPHR_AP_SHIFT             (0)

// reg_60
#define PSRAM_UHS_MC_T_CPHW(n)                   (((n) & 0x3F) << 0)
#define PSRAM_UHS_MC_T_CPHW_MASK                 (0x3F << 0)
#define PSRAM_UHS_MC_T_CPHW_SHIFT                (0)

// reg_64
#define PSRAM_UHS_MC_T_CPHW_AP(n)                (((n) & 0x3F) << 0)
#define PSRAM_UHS_MC_T_CPHW_AP_MASK              (0x3F << 0)
#define PSRAM_UHS_MC_T_CPHW_AP_SHIFT             (0)

// reg_68
#define PSRAM_UHS_MC_T_MRR(n)                    (((n) & 0x3F) << 0)
#define PSRAM_UHS_MC_T_MRR_MASK                  (0x3F << 0)
#define PSRAM_UHS_MC_T_MRR_SHIFT                 (0)

// reg_6c
#define PSRAM_UHS_MC_T_MRS(n)                    (((n) & 0x3F) << 0)
#define PSRAM_UHS_MC_T_MRS_MASK                  (0x3F << 0)
#define PSRAM_UHS_MC_T_MRS_SHIFT                 (0)

// reg_70
#define PSRAM_UHS_MC_T_CEM(n)                    (((n) & 0xFFFF) << 0)
#define PSRAM_UHS_MC_T_CEM_MASK                  (0xFFFF << 0)
#define PSRAM_UHS_MC_T_CEM_SHIFT                 (0)

// reg_74
#define PSRAM_UHS_MC_T_RST(n)                    (((n) & 0xFFFF) << 0)
#define PSRAM_UHS_MC_T_RST_MASK                  (0xFFFF << 0)
#define PSRAM_UHS_MC_T_RST_SHIFT                 (0)

// reg_78
#define PSRAM_UHS_MC_T_SRF(n)                    (((n) & 0xFF) << 0)
#define PSRAM_UHS_MC_T_SRF_MASK                  (0xFF << 0)
#define PSRAM_UHS_MC_T_SRF_SHIFT                 (0)

// reg_7c
#define PSRAM_UHS_MC_T_XSR(n)                    (((n) & 0xFF) << 0)
#define PSRAM_UHS_MC_T_XSR_MASK                  (0xFF << 0)
#define PSRAM_UHS_MC_T_XSR_SHIFT                 (0)

// reg_80
#define PSRAM_UHS_MC_T_HS(n)                     (((n) & 0xFFFF) << 0)
#define PSRAM_UHS_MC_T_HS_MASK                   (0xFFFF << 0)
#define PSRAM_UHS_MC_T_HS_SHIFT                  (0)

// reg_84
#define PSRAM_UHS_MC_T_XPHS(n)                   (((n) & 0xFF) << 0)
#define PSRAM_UHS_MC_T_XPHS_MASK                 (0xFF << 0)
#define PSRAM_UHS_MC_T_XPHS_SHIFT                (0)

// reg_88
#define PSRAM_UHS_MC_T_XHS(n)                    (((n) & 0xFFFFF) << 0)
#define PSRAM_UHS_MC_T_XHS_MASK                  (0xFFFFF << 0)
#define PSRAM_UHS_MC_T_XHS_SHIFT                 (0)

// reg_8c
#define PSRAM_UHS_MC_T_ZQCAL(n)                  (((n) & 0xFFFFF) << 0)
#define PSRAM_UHS_MC_T_ZQCAL_MASK                (0xFFFFF << 0)
#define PSRAM_UHS_MC_T_ZQCAL_SHIFT               (0)

// reg_90
#define PSRAM_UHS_MC_T_ZQCRST(n)                 (((n) & 0xFFFFF) << 0)
#define PSRAM_UHS_MC_T_ZQCRST_MASK               (0xFFFFF << 0)
#define PSRAM_UHS_MC_T_ZQCRST_SHIFT              (0)

// reg_94
#define PSRAM_UHS_MC_T_XCKD(n)                   (((n) & 0x3F) << 0)
#define PSRAM_UHS_MC_T_XCKD_MASK                 (0x3F << 0)
#define PSRAM_UHS_MC_T_XCKD_SHIFT                (0)

// reg_98
#define PSRAM_UHS_MC_T_ECKD(n)                   (((n) & 0x3F) << 0)
#define PSRAM_UHS_MC_T_ECKD_MASK                 (0x3F << 0)
#define PSRAM_UHS_MC_T_ECKD_SHIFT                (0)

// reg_9c
#define PSRAM_UHS_MC_WR_DMY_CYC(n)               (((n) & 0xFF) << 0)
#define PSRAM_UHS_MC_WR_DMY_CYC_MASK             (0xFF << 0)
#define PSRAM_UHS_MC_WR_DMY_CYC_SHIFT            (0)

// reg_a0
#define PSRAM_UHS_MC_STOP_CLK_IN_NOP             (1 << 0)
#define PSRAM_UHS_MC_NOP_DMY_CYC(n)              (((n) & 0xFF) << 1)
#define PSRAM_UHS_MC_NOP_DMY_CYC_MASK            (0xFF << 1)
#define PSRAM_UHS_MC_NOP_DMY_CYC_SHIFT           (1)

// reg_a4
#define PSRAM_UHS_MC_QUEUE_IDLE_CYCLE(n)         (((n) & 0xFFFFFFFF) << 0)
#define PSRAM_UHS_MC_QUEUE_IDLE_CYCLE_MASK       (0xFFFFFFFF << 0)
#define PSRAM_UHS_MC_QUEUE_IDLE_CYCLE_SHIFT      (0)

// reg_a8
#define PSRAM_UHS_MC_T_EXPANDRD(n)               (((n) & 0x3F) << 0)
#define PSRAM_UHS_MC_T_EXPANDRD_MASK             (0x3F << 0)
#define PSRAM_UHS_MC_T_EXPANDRD_SHIFT            (0)

// reg_ac
#define PSRAM_UHS_MC_RX_SYNC_BYPASS              (1 << 0)

// reg_b0
#define PSRAM_UHS_MC_MGR_FIFO_TEST_EN            (1 << 0)

// reg_b4
#define PSRAM_UHS_MC_T_ZQCAS(n)                  (((n) & 0xFFFFF) << 0)
#define PSRAM_UHS_MC_T_ZQCAS_MASK                (0xFFFFF << 0)
#define PSRAM_UHS_MC_T_ZQCAS_SHIFT               (0)

// reg_b8
#define PSRAM_UHS_MC_T_NEW_HOLD(n)               (((n) & 0xFFFFFFFF) << 0)
#define PSRAM_UHS_MC_T_NEW_HOLD_MASK             (0xFFFFFFFF << 0)
#define PSRAM_UHS_MC_T_NEW_HOLD_SHIFT            (0)

// reg_bc
#define PSRAM_UHS_MC_NEW_CMD_OP(n)               (((n) & 0x7) << 0)
#define PSRAM_UHS_MC_NEW_CMD_OP_MASK             (0x7 << 0)
#define PSRAM_UHS_MC_NEW_CMD_OP_SHIFT            (0)

// reg_140
#define PSRAM_UHS_MC_CMD_TABLE_ARRAY_RD(n)       (((n) & 0xFF) << 0)
#define PSRAM_UHS_MC_CMD_TABLE_ARRAY_RD_MASK     (0xFF << 0)
#define PSRAM_UHS_MC_CMD_TABLE_ARRAY_RD_SHIFT    (0)

// reg_144
#define PSRAM_UHS_MC_CMD_TABLE_ARRAY_WR(n)       (((n) & 0xFF) << 0)
#define PSRAM_UHS_MC_CMD_TABLE_ARRAY_WR_MASK     (0xFF << 0)
#define PSRAM_UHS_MC_CMD_TABLE_ARRAY_WR_SHIFT    (0)

// reg_148
#define PSRAM_UHS_MC_CMD_TABLE_REG_RD(n)         (((n) & 0xFF) << 0)
#define PSRAM_UHS_MC_CMD_TABLE_REG_RD_MASK       (0xFF << 0)
#define PSRAM_UHS_MC_CMD_TABLE_REG_RD_SHIFT      (0)

// reg_14c
#define PSRAM_UHS_MC_CMD_TABLE_REG_WR(n)         (((n) & 0xFF) << 0)
#define PSRAM_UHS_MC_CMD_TABLE_REG_WR_MASK       (0xFF << 0)
#define PSRAM_UHS_MC_CMD_TABLE_REG_WR_SHIFT      (0)

// reg_150
#define PSRAM_UHS_MC_CMD_TABLE_AUTO_REFR(n)      (((n) & 0xFF) << 0)
#define PSRAM_UHS_MC_CMD_TABLE_AUTO_REFR_MASK    (0xFF << 0)
#define PSRAM_UHS_MC_CMD_TABLE_AUTO_REFR_SHIFT   (0)

// reg_154
#define PSRAM_UHS_MC_CMD_TABLE_SELF_REFR(n)      (((n) & 0xFF) << 0)
#define PSRAM_UHS_MC_CMD_TABLE_SELF_REFR_MASK    (0xFF << 0)
#define PSRAM_UHS_MC_CMD_TABLE_SELF_REFR_SHIFT   (0)

// reg_158
#define PSRAM_UHS_MC_CMD_TABLE_HSLP_ENTRY(n)     (((n) & 0xFF) << 0)
#define PSRAM_UHS_MC_CMD_TABLE_HSLP_ENTRY_MASK   (0xFF << 0)
#define PSRAM_UHS_MC_CMD_TABLE_HSLP_ENTRY_SHIFT  (0)

// reg_15c
#define PSRAM_UHS_MC_CMD_TABLE_GLBRST(n)         (((n) & 0xFF) << 0)
#define PSRAM_UHS_MC_CMD_TABLE_GLBRST_MASK       (0xFF << 0)
#define PSRAM_UHS_MC_CMD_TABLE_GLBRST_SHIFT      (0)

// reg_160
#define PSRAM_UHS_MC_CMD_TABLE_NOP(n)            (((n) & 0xFF) << 0)
#define PSRAM_UHS_MC_CMD_TABLE_NOP_MASK          (0xFF << 0)
#define PSRAM_UHS_MC_CMD_TABLE_NOP_SHIFT         (0)

// reg_180
#define PSRAM_UHS_MC_CA_MAP_BIT0(n)              (((n) & 0x1F) << 0)
#define PSRAM_UHS_MC_CA_MAP_BIT0_MASK            (0x1F << 0)
#define PSRAM_UHS_MC_CA_MAP_BIT0_SHIFT           (0)
#define PSRAM_UHS_MC_CA_MAP_BIT1(n)              (((n) & 0x1F) << 5)
#define PSRAM_UHS_MC_CA_MAP_BIT1_MASK            (0x1F << 5)
#define PSRAM_UHS_MC_CA_MAP_BIT1_SHIFT           (5)
#define PSRAM_UHS_MC_CA_MAP_BIT2(n)              (((n) & 0x1F) << 10)
#define PSRAM_UHS_MC_CA_MAP_BIT2_MASK            (0x1F << 10)
#define PSRAM_UHS_MC_CA_MAP_BIT2_SHIFT           (10)
#define PSRAM_UHS_MC_CA_MAP_BIT3(n)              (((n) & 0x1F) << 15)
#define PSRAM_UHS_MC_CA_MAP_BIT3_MASK            (0x1F << 15)
#define PSRAM_UHS_MC_CA_MAP_BIT3_SHIFT           (15)
#define PSRAM_UHS_MC_CA_MAP_BIT4(n)              (((n) & 0x1F) << 20)
#define PSRAM_UHS_MC_CA_MAP_BIT4_MASK            (0x1F << 20)
#define PSRAM_UHS_MC_CA_MAP_BIT4_SHIFT           (20)
#define PSRAM_UHS_MC_CA_MAP_BIT5(n)              (((n) & 0x1F) << 25)
#define PSRAM_UHS_MC_CA_MAP_BIT5_MASK            (0x1F << 25)
#define PSRAM_UHS_MC_CA_MAP_BIT5_SHIFT           (25)

// reg_184
#define PSRAM_UHS_MC_CA_MAP_BIT6(n)              (((n) & 0x1F) << 0)
#define PSRAM_UHS_MC_CA_MAP_BIT6_MASK            (0x1F << 0)
#define PSRAM_UHS_MC_CA_MAP_BIT6_SHIFT           (0)
#define PSRAM_UHS_MC_CA_MAP_BIT7(n)              (((n) & 0x1F) << 5)
#define PSRAM_UHS_MC_CA_MAP_BIT7_MASK            (0x1F << 5)
#define PSRAM_UHS_MC_CA_MAP_BIT7_SHIFT           (5)
#define PSRAM_UHS_MC_CA_MAP_BIT8(n)              (((n) & 0x1F) << 10)
#define PSRAM_UHS_MC_CA_MAP_BIT8_MASK            (0x1F << 10)
#define PSRAM_UHS_MC_CA_MAP_BIT8_SHIFT           (10)
#define PSRAM_UHS_MC_CA_MAP_BIT9(n)              (((n) & 0x1F) << 15)
#define PSRAM_UHS_MC_CA_MAP_BIT9_MASK            (0x1F << 15)
#define PSRAM_UHS_MC_CA_MAP_BIT9_SHIFT           (15)
#define PSRAM_UHS_MC_CA_MAP_BIT10(n)             (((n) & 0x1F) << 20)
#define PSRAM_UHS_MC_CA_MAP_BIT10_MASK           (0x1F << 20)
#define PSRAM_UHS_MC_CA_MAP_BIT10_SHIFT          (20)
#define PSRAM_UHS_MC_CA_MAP_BIT11(n)             (((n) & 0x1F) << 25)
#define PSRAM_UHS_MC_CA_MAP_BIT11_MASK           (0x1F << 25)
#define PSRAM_UHS_MC_CA_MAP_BIT11_SHIFT          (25)

// reg_188
#define PSRAM_UHS_MC_CA_MAP_BIT12(n)             (((n) & 0x1F) << 0)
#define PSRAM_UHS_MC_CA_MAP_BIT12_MASK           (0x1F << 0)
#define PSRAM_UHS_MC_CA_MAP_BIT12_SHIFT          (0)
#define PSRAM_UHS_MC_CA_MAP_BIT13(n)             (((n) & 0x1F) << 5)
#define PSRAM_UHS_MC_CA_MAP_BIT13_MASK           (0x1F << 5)
#define PSRAM_UHS_MC_CA_MAP_BIT13_SHIFT          (5)
#define PSRAM_UHS_MC_CA_MAP_BIT14(n)             (((n) & 0x1F) << 10)
#define PSRAM_UHS_MC_CA_MAP_BIT14_MASK           (0x1F << 10)
#define PSRAM_UHS_MC_CA_MAP_BIT14_SHIFT          (10)
#define PSRAM_UHS_MC_CA_MAP_BIT15(n)             (((n) & 0x1F) << 15)
#define PSRAM_UHS_MC_CA_MAP_BIT15_MASK           (0x1F << 15)
#define PSRAM_UHS_MC_CA_MAP_BIT15_SHIFT          (15)
#define PSRAM_UHS_MC_CA_MAP_BIT16(n)             (((n) & 0x1F) << 20)
#define PSRAM_UHS_MC_CA_MAP_BIT16_MASK           (0x1F << 20)
#define PSRAM_UHS_MC_CA_MAP_BIT16_SHIFT          (20)
#define PSRAM_UHS_MC_CA_MAP_BIT17(n)             (((n) & 0x1F) << 25)
#define PSRAM_UHS_MC_CA_MAP_BIT17_MASK           (0x1F << 25)
#define PSRAM_UHS_MC_CA_MAP_BIT17_SHIFT          (25)

// reg_18c
#define PSRAM_UHS_MC_CA_MAP_BIT18(n)             (((n) & 0x1F) << 0)
#define PSRAM_UHS_MC_CA_MAP_BIT18_MASK           (0x1F << 0)
#define PSRAM_UHS_MC_CA_MAP_BIT18_SHIFT          (0)
#define PSRAM_UHS_MC_CA_MAP_BIT19(n)             (((n) & 0x1F) << 5)
#define PSRAM_UHS_MC_CA_MAP_BIT19_MASK           (0x1F << 5)
#define PSRAM_UHS_MC_CA_MAP_BIT19_SHIFT          (5)
#define PSRAM_UHS_MC_CA_MAP_BIT20(n)             (((n) & 0x1F) << 10)
#define PSRAM_UHS_MC_CA_MAP_BIT20_MASK           (0x1F << 10)
#define PSRAM_UHS_MC_CA_MAP_BIT20_SHIFT          (10)
#define PSRAM_UHS_MC_CA_MAP_BIT21(n)             (((n) & 0x1F) << 15)
#define PSRAM_UHS_MC_CA_MAP_BIT21_MASK           (0x1F << 15)
#define PSRAM_UHS_MC_CA_MAP_BIT21_SHIFT          (15)
#define PSRAM_UHS_MC_CA_MAP_BIT22(n)             (((n) & 0x1F) << 20)
#define PSRAM_UHS_MC_CA_MAP_BIT22_MASK           (0x1F << 20)
#define PSRAM_UHS_MC_CA_MAP_BIT22_SHIFT          (20)
#define PSRAM_UHS_MC_CA_MAP_BIT23(n)             (((n) & 0x1F) << 25)
#define PSRAM_UHS_MC_CA_MAP_BIT23_MASK           (0x1F << 25)
#define PSRAM_UHS_MC_CA_MAP_BIT23_SHIFT          (25)

// reg_190
#define PSRAM_UHS_MC_CA_MAP_BIT24(n)             (((n) & 0x1F) << 0)
#define PSRAM_UHS_MC_CA_MAP_BIT24_MASK           (0x1F << 0)
#define PSRAM_UHS_MC_CA_MAP_BIT24_SHIFT          (0)
#define PSRAM_UHS_MC_CA_MAP_BIT25(n)             (((n) & 0x1F) << 5)
#define PSRAM_UHS_MC_CA_MAP_BIT25_MASK           (0x1F << 5)
#define PSRAM_UHS_MC_CA_MAP_BIT25_SHIFT          (5)
#define PSRAM_UHS_MC_CA_MAP_BIT26(n)             (((n) & 0x1F) << 10)
#define PSRAM_UHS_MC_CA_MAP_BIT26_MASK           (0x1F << 10)
#define PSRAM_UHS_MC_CA_MAP_BIT26_SHIFT          (10)
#define PSRAM_UHS_MC_CA_MAP_BIT27(n)             (((n) & 0x1F) << 15)
#define PSRAM_UHS_MC_CA_MAP_BIT27_MASK           (0x1F << 15)
#define PSRAM_UHS_MC_CA_MAP_BIT27_SHIFT          (15)
#define PSRAM_UHS_MC_CA_MAP_BIT28(n)             (((n) & 0x1F) << 20)
#define PSRAM_UHS_MC_CA_MAP_BIT28_MASK           (0x1F << 20)
#define PSRAM_UHS_MC_CA_MAP_BIT28_SHIFT          (20)
#define PSRAM_UHS_MC_CA_MAP_BIT29(n)             (((n) & 0x1F) << 25)
#define PSRAM_UHS_MC_CA_MAP_BIT29_MASK           (0x1F << 25)
#define PSRAM_UHS_MC_CA_MAP_BIT29_SHIFT          (25)

// reg_194
#define PSRAM_UHS_MC_CA_MAP_BIT30(n)             (((n) & 0x1F) << 0)
#define PSRAM_UHS_MC_CA_MAP_BIT30_MASK           (0x1F << 0)
#define PSRAM_UHS_MC_CA_MAP_BIT30_SHIFT          (0)
#define PSRAM_UHS_MC_CA_MAP_BIT31(n)             (((n) & 0x1F) << 5)
#define PSRAM_UHS_MC_CA_MAP_BIT31_MASK           (0x1F << 5)
#define PSRAM_UHS_MC_CA_MAP_BIT31_SHIFT          (5)
#define PSRAM_UHS_MC_CA_MAP_BIT32(n)             (((n) & 0x1F) << 10)
#define PSRAM_UHS_MC_CA_MAP_BIT32_MASK           (0x1F << 10)
#define PSRAM_UHS_MC_CA_MAP_BIT32_SHIFT          (10)

// reg_190

// reg_400
#define PSRAM_UHS_MC_INIT_COMPLETE               (1 << 0)

// reg_404
#define PSRAM_UHS_MC_BUSY                        (1 << 0)
#define PSRAM_UHS_MC_MGR_RXFIFO_R_EMPTY          (1 << 1)
#define PSRAM_UHS_MC_MGR_RXFIFO_FULL_CNT(n)      (((n) & 0xF) << 2)
#define PSRAM_UHS_MC_MGR_RXFIFO_FULL_CNT_MASK    (0xF << 2)
#define PSRAM_UHS_MC_MGR_RXFIFO_FULL_CNT_SHIFT   (2)
#define PSRAM_UHS_MC_MGR_TXFIFO_W_FULL           (1 << 6)
#define PSRAM_UHS_MC_MGR_TXFIFO_EMPTY_CNT(n)     (((n) & 0xF) << 7)
#define PSRAM_UHS_MC_MGR_TXFIFO_EMPTY_CNT_MASK   (0xF << 7)
#define PSRAM_UHS_MC_MGR_TXFIFO_EMPTY_CNT_SHIFT  (7)
#define PSRAM_UHS_MC_WB_FILL_LEVEL(n)            (((n) & 0x1F) << 11)
#define PSRAM_UHS_MC_WB_FILL_LEVEL_MASK          (0x1F << 11)
#define PSRAM_UHS_MC_WB_FILL_LEVEL_SHIFT         (11)
#define PSRAM_UHS_MC_CP_FSM_STATE(n)             (((n) & 0xF) << 16)
#define PSRAM_UHS_MC_CP_FSM_STATE_MASK           (0xF << 16)
#define PSRAM_UHS_MC_CP_FSM_STATE_SHIFT          (16)
#define PSRAM_UHS_MC_RD_FSM(n)                   (((n) & 0x3) << 20)
#define PSRAM_UHS_MC_RD_FSM_MASK                 (0x3 << 20)
#define PSRAM_UHS_MC_RD_FSM_SHIFT                (20)

// reg_440
#define PSRAM_UHS_MC_PMU_MONITOR_START           (1 << 0)
#define PSRAM_UHS_MC_PMU_MONITOR_END             (1 << 1)

// reg_444
#define PSRAM_UHS_MC_PMU_TOL_MON_CLK_CYCLE0(n)   (((n) & 0xFFFFFFFF) << 0)
#define PSRAM_UHS_MC_PMU_TOL_MON_CLK_CYCLE0_MASK (0xFFFFFFFF << 0)
#define PSRAM_UHS_MC_PMU_TOL_MON_CLK_CYCLE0_SHIFT (0)

// reg_448
#define PSRAM_UHS_MC_PMU_TOL_MON_CLK_CYCLE1(n)   (((n) & 0xFFFFFFFF) << 0)
#define PSRAM_UHS_MC_PMU_TOL_MON_CLK_CYCLE1_MASK (0xFFFFFFFF << 0)
#define PSRAM_UHS_MC_PMU_TOL_MON_CLK_CYCLE1_SHIFT (0)

// reg_44c
#define PSRAM_UHS_MC_PMU_TOL_WR_DATA_BYTES0(n)   (((n) & 0xFFFFFFFF) << 0)
#define PSRAM_UHS_MC_PMU_TOL_WR_DATA_BYTES0_MASK (0xFFFFFFFF << 0)
#define PSRAM_UHS_MC_PMU_TOL_WR_DATA_BYTES0_SHIFT (0)

// reg_450
#define PSRAM_UHS_MC_PMU_TOL_WR_DATA_BYTES1(n)   (((n) & 0xFFFFFFFF) << 0)
#define PSRAM_UHS_MC_PMU_TOL_WR_DATA_BYTES1_MASK (0xFFFFFFFF << 0)
#define PSRAM_UHS_MC_PMU_TOL_WR_DATA_BYTES1_SHIFT (0)

// reg_454
#define PSRAM_UHS_MC_PMU_TOL_RD_DATA_BYTES0(n)   (((n) & 0xFFFFFFFF) << 0)
#define PSRAM_UHS_MC_PMU_TOL_RD_DATA_BYTES0_MASK (0xFFFFFFFF << 0)
#define PSRAM_UHS_MC_PMU_TOL_RD_DATA_BYTES0_SHIFT (0)

// reg_458
#define PSRAM_UHS_MC_PMU_TOL_RD_DATA_BYTES1(n)   (((n) & 0xFFFFFFFF) << 0)
#define PSRAM_UHS_MC_PMU_TOL_RD_DATA_BYTES1_MASK (0xFFFFFFFF << 0)
#define PSRAM_UHS_MC_PMU_TOL_RD_DATA_BYTES1_SHIFT (0)

// reg_45c
#define PSRAM_UHS_MC_PMU_TOL_RD_ACC_LATENCY0(n)  (((n) & 0xFFFFFFFF) << 0)
#define PSRAM_UHS_MC_PMU_TOL_RD_ACC_LATENCY0_MASK (0xFFFFFFFF << 0)
#define PSRAM_UHS_MC_PMU_TOL_RD_ACC_LATENCY0_SHIFT (0)

// reg_460
#define PSRAM_UHS_MC_PMU_TOL_RD_ACC_LATENCY1(n)  (((n) & 0xFFFFFFFF) << 0)
#define PSRAM_UHS_MC_PMU_TOL_RD_ACC_LATENCY1_MASK (0xFFFFFFFF << 0)
#define PSRAM_UHS_MC_PMU_TOL_RD_ACC_LATENCY1_SHIFT (0)

// reg_464
#define PSRAM_UHS_MC_PMU_TOL_RD_ACC_NUM0(n)      (((n) & 0xFFFFFFFF) << 0)
#define PSRAM_UHS_MC_PMU_TOL_RD_ACC_NUM0_MASK    (0xFFFFFFFF << 0)
#define PSRAM_UHS_MC_PMU_TOL_RD_ACC_NUM0_SHIFT   (0)

// reg_468
#define PSRAM_UHS_MC_PMU_TOL_RD_ACC_NUM1(n)      (((n) & 0xFFFFFFFF) << 0)
#define PSRAM_UHS_MC_PMU_TOL_RD_ACC_NUM1_MASK    (0xFFFFFFFF << 0)
#define PSRAM_UHS_MC_PMU_TOL_RD_ACC_NUM1_SHIFT   (0)

// reg_46c
#define PSRAM_UHS_MC_PMU_MAX_RD_ACC_LATENCY(n)   (((n) & 0xFFFF) << 0)
#define PSRAM_UHS_MC_PMU_MAX_RD_ACC_LATENCY_MASK (0xFFFF << 0)
#define PSRAM_UHS_MC_PMU_MAX_RD_ACC_LATENCY_SHIFT (0)

// reg_840
#define PSRAM_UHS_MC_PHY_CTRL_DELAY(n)           (((n) & 0x3) << 0)
#define PSRAM_UHS_MC_PHY_CTRL_DELAY_MASK         (0x3 << 0)
#define PSRAM_UHS_MC_PHY_CTRL_DELAY_SHIFT        (0)
#define PSRAM_UHS_MC_RESERVED_2_REG840           (1 << 2)
#define PSRAM_UHS_MC_PHY_RX_DLY_EN               (1 << 3)
#define PSRAM_UHS_MC_ANA_LOOPBACK_EN             (1 << 4)
#define PSRAM_UHS_MC_ANA_TEST_TXFIFO             (1 << 5)
#define PSRAM_UHS_MC_PHY_LOOPBACK_EN             (1 << 6)
#define PSRAM_UHS_MC_PHY_ALIGN_BYPASS            (1 << 7)
#define PSRAM_UHS_MC_PHY0_CONFLICT_CLR           (1 << 8)
#define PSRAM_UHS_MC_PHY1_CONFLICT_CLR           (1 << 9)
#define PSRAM_UHS_MC_PHY_IDLE_PAD_EN             (1 << 10)
#define PSRAM_UHS_MC_PHY_DUMMY_CYC_EN            (1 << 11)

// reg_844
#define PSRAM_UHS_MC_T_WPST(n)                   (((n) & 0x7) << 0)
#define PSRAM_UHS_MC_T_WPST_MASK                 (0x7 << 0)
#define PSRAM_UHS_MC_T_WPST_SHIFT                (0)

// reg_848
#define PSRAM_UHS_MC_T_LAT_SWITCH_CYC(n)         (((n) & 0x7) << 0)
#define PSRAM_UHS_MC_T_LAT_SWITCH_CYC_MASK       (0x7 << 0)
#define PSRAM_UHS_MC_T_LAT_SWITCH_CYC_SHIFT      (0)

// reg_84c
#define PSRAM_UHS_MC_PHY0_CONFLICT_STS           (1 << 0)
#define PSRAM_UHS_MC_PHY1_CONFLICT_STS           (1 << 1)
#define PSRAM_UHS_MC_PHY0_FSM_STATE(n)           (((n) & 0xF) << 2)
#define PSRAM_UHS_MC_PHY0_FSM_STATE_MASK         (0xF << 2)
#define PSRAM_UHS_MC_PHY0_FSM_STATE_SHIFT        (2)
#define PSRAM_UHS_MC_PHY1_FSM_STATE(n)           (((n) & 0xF) << 6)
#define PSRAM_UHS_MC_PHY1_FSM_STATE_MASK         (0xF << 6)
#define PSRAM_UHS_MC_PHY1_FSM_STATE_SHIFT        (6)

#endif
