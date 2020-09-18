/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __REG_NORFLASHIP_V2_H__
#define __REG_NORFLASHIP_V2_H__

#include "plat_types.h"

struct NORFLASH_CTRL_T {
    __IO uint32_t REG_000;
    __IO uint32_t REG_004;
    union TXDATA_REG_T {
        __IO uint32_t TXWORD;
        __IO uint16_t TXHALFWORD;
        __IO uint8_t TXBYTE;
    } REG_008;
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
#if !defined(CHIP_BEST2300)
    __IO uint32_t REG_038;
    __IO uint32_t RESERVED_03C;
    __IO uint32_t REG_040;
    __IO uint32_t RESERVED_044[3];
    __IO uint32_t REG_050;
    __IO uint32_t REG_054;
    __IO uint32_t REG_058;
    __IO uint32_t REG_05C;
    __IO uint32_t REG_060;
    __IO uint32_t REG_064;
    __IO uint32_t REG_068;
    __IO uint32_t REG_06C;
#if !defined(CHIP_BEST1400)
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
    __IO uint32_t RESERVED_0A4[3];
    __IO uint32_t REG_0B0;
#if (CHIP_FLASH_CTRL_VER >= 3)
    __IO uint32_t RESERVED_0B4[0x13];
    __IO uint32_t REG_100;
    __IO uint32_t REG_104;
    __IO uint32_t REG_108;
    __IO uint32_t REG_10C;
    __IO uint32_t REG_110;
    __IO uint32_t REG_114;
    __IO uint32_t REG_118;
    __IO uint32_t REG_11C;
    __IO uint32_t REG_120;
    __IO uint32_t REG_124;
    __IO uint32_t REG_128;
    __IO uint32_t REG_12C;
    __IO uint32_t REG_130;
    __IO uint32_t REG_134;
    __IO uint32_t REG_138;
    __IO uint32_t REG_13C;
    __IO uint32_t REG_140;
    __IO uint32_t REG_144;
    __IO uint32_t REG_148;
    __IO uint32_t REG_14C;
    __IO uint32_t REG_150;
#endif
#endif
#endif
};

// REG_000
#define REG_000_ADDR_SHIFT                  8
#define REG_000_ADDR_MASK                   (0xFFFFFF << REG_000_ADDR_SHIFT)
#define REG_000_ADDR(n)                     BITFIELD_VAL(REG_000_ADDR, n)
#define REG_000_CMD_SHIFT                   0
#define REG_000_CMD_MASK                    (0xFF << REG_000_CMD_SHIFT)
#define REG_000_CMD(n)                      BITFIELD_VAL(REG_000_CMD, n)

#define REG_000_NEW_CMD_RX_LEN_SHIFT        13
#define REG_000_NEW_CMD_RX_LEN_MASK         (0x3FFF << REG_000_NEW_CMD_RX_LEN_SHIFT)
#define REG_000_NEW_CMD_RX_LEN(n)           BITFIELD_VAL(REG_000_NEW_CMD_RX_LEN, n)
#define REG_000_NEW_CMD_RX_LINE_SHIFT       11
#define REG_000_NEW_CMD_RX_LINE_MASK        (0x3 << REG_000_NEW_CMD_RX_LINE_SHIFT)
#define REG_000_NEW_CMD_RX_LINE(n)          BITFIELD_VAL(REG_000_NEW_CMD_RX_LINE, n)
#define REG_000_NEW_CMD_TX_LINE_SHIFT       9
#define REG_000_NEW_CMD_TX_LINE_MASK        (0x3 << REG_000_NEW_CMD_TX_LINE_SHIFT)
#define REG_000_NEW_CMD_TX_LINE(n)          BITFIELD_VAL(REG_000_NEW_CMD_TX_LINE, n)
#define REG_000_NEW_CMD_RX_EN               (1 << 8)

#define NEW_CMD_LINE_4X                     2
#define NEW_CMD_LINE_2X                     1
#define NEW_CMD_LINE_1X                     0

// REG_004
#define REG_004_RES26_SHIFT                 26
#define REG_004_RES26_MASK                  (0x3F << REG_004_RES26_SHIFT)
#define REG_004_RES26(n)                    BITFIELD_VAL(REG_004_RES26, n)
#define REG_004_CONTINUOUS_MODE             (1 << 25)
#define REG_004_BLOCK_SIZE_SHIFT            12
#define REG_004_BLOCK_SIZE_MASK             (0x1FFF << REG_004_BLOCK_SIZE_SHIFT)
#define REG_004_BLOCK_SIZE(n)               BITFIELD_VAL(REG_004_BLOCK_SIZE, n)
#define REG_004_MODEBIT_SHIFT               4
#define REG_004_MODEBIT_MASK                (0xFF << REG_004_MODEBIT_SHIFT)
#define REG_004_MODEBIT(n)                  BITFIELD_VAL(REG_004_MODEBIT, n)
#define REG_004_RES1_SHIFT                  1
#define REG_004_RES1_MASK                   (0x7 << REG_004_RES1_SHIFT)
#define REG_004_RES1(n)                     BITFIELD_VAL(REG_004_RES1, n)
#define REG_004_NEW_CMD_EN                  (1 << 0)

// REG_008
#define REG_008_TXDATA_SHIFT                0
#define REG_008_TXDATA_MASK                 (0xFFFFFFFF << REG_008_TXDATA_SHIFT)
#define REG_008_TXDATA(n)                   BITFIELD_VAL(REG_008_TXDATA, n)

// REG_00C
#define REG_00C_RES_SHIFT                   13
#define REG_00C_RES_MASK                    (0x7FFFF << REG_00C_RES_SHIFT)
#define REG_00C_RES(n)                      BITFIELD_VAL(REG_00C_RES, n)
#define REG_00C_TXFIFO_EMPCNT_SHIFT         8
#define REG_00C_TXFIFO_EMPCNT_MASK          (0x1F << REG_00C_TXFIFO_EMPCNT_SHIFT)
#define REG_00C_TXFIFO_EMPCNT(n)            BITFIELD_VAL(REG_00C_TXFIFO_EMPCNT, n)
#define REG_00C_RXFIFO_COUNT_SHIFT          4
#define REG_00C_RXFIFO_COUNT_MASK           (0xF << REG_00C_RXFIFO_COUNT_SHIFT)
#define REG_00C_RXFIFO_COUNT(n)             BITFIELD_VAL(REG_00C_RXFIFO_COUNT, n)
#define REG_00C_RXFIFO_EMPTY                (1 << 3)
#define REG_00C_TXFIFO_FULL                 (1 << 2)
#define REG_00C_TXFIFO_EMPTY                (1 << 1)
#define REG_00C_BUSY                        (1 << 0)

// REG_010
#define REG_010_RXDATA_SHIFT                0
#define REG_010_RXDATA_MASK                 (0xFFFFFFFF << REG_010_RXDATA_SHIFT)
#define REG_010_RXDATA(n)                   BITFIELD_VAL(REG_010_RXDATA, n)

// REG_014
#define REG_014_RES29_SHIFT                 29
#define REG_014_RES29_MASK                  (0x7 << REG_014_RES29_SHIFT)
#define REG_014_RES29(n)                    BITFIELD_VAL(REG_014_RES29, n)
#define REG_014_EXTRA_SHSL_SHIFT            25
#define REG_014_EXTRA_SHSL_MASK             (0xF << REG_014_EXTRA_SHSL_SHIFT)
#define REG_014_EXTRA_SHSL(n)               BITFIELD_VAL(REG_014_EXTRA_SHSL, n)
#define REG_014_EXTRA_SHSL_EN               (1 << 24)
#define REG_014_CLKDIV_SHIFT                16
#define REG_014_CLKDIV_MASK                 (0xFF << REG_014_CLKDIV_SHIFT)
#define REG_014_CLKDIV(n)                   BITFIELD_VAL(REG_014_CLKDIV, n)
#define REG_014_SAMPLESEL_SHIFT             12
#define REG_014_SAMPLESEL_MASK              (0xF << REG_014_SAMPLESEL_SHIFT)
#define REG_014_SAMPLESEL(n)                BITFIELD_VAL(REG_014_SAMPLESEL, n)
// Since 2300p
#define EXTRA_TCHSH_O_SHIFT                 8
#define EXTRA_TCHSH_O_MASK                  (0xF << EXTRA_TCHSH_O_SHIFT)
#define EXTRA_TCHSH_O(n)                    BITFIELD_VAL(EXTRA_TCHSH_O, n)
#define EXTRA_TCHSH_EN_O                    (1 << 7)
// End of since 2300p
#define REG_014_CMDQUAD                     (1 << 6)
#define REG_014_RAM_DUALMODE                (1 << 5)
#define REG_014_RAM_QUADMODE                (1 << 4)
#define REG_014_FOUR_BYTE_ADDR_EN           (1 << 3)
#define REG_014_RES2                        (1 << 2)
#define REG_014_HOLDPIN                     (1 << 1)
#define REG_014_WPROPIN                     (1 << 0)

// REG_018
#define REG_018_RES_SHIFT                   2
#define REG_018_RES_MASK                    (0x3FFFFFFF << REG_018_RES_SHIFT)
#define REG_018_RES(n)                      BITFIELD_VAL(REG_018_RES, n)
#define REG_018_TXFIFOCLR                   (1 << 1)
#define REG_018_RXFIFOCLR                   (1 << 0)

// REG_01C
#define REG_01C_RES18_SHIFT                 18
#define REG_01C_RES18_MASK                  (0x3FFF << REG_01C_RES18_SHIFT)
#define REG_01C_RES18(n)                    BITFIELD_VAL(REG_01C_RES18, n)
#define REG_01C_DMA_RX_SIZE_SHIFT           16
#define REG_01C_DMA_RX_SIZE_MASK            (0x3 << REG_01C_DMA_RX_SIZE_SHIFT)
#define REG_01C_DMA_RX_SIZE(n)              BITFIELD_VAL(REG_01C_DMA_RX_SIZE, n)
#define REG_01C_TX_THRESHOLD_SHIFT          8
#define REG_01C_TX_THRESHOLD_MASK           (0x1F << REG_01C_TX_THRESHOLD_SHIFT)
#define REG_01C_TX_THRESHOLD(n)             BITFIELD_VAL(REG_01C_TX_THRESHOLD, n)
#define REG_01C_RX_THRESHOLD_SHIFT          4
#define REG_01C_RX_THRESHOLD_MASK           (0xF << REG_01C_RX_THRESHOLD_SHIFT)
#define REG_01C_RX_THRESHOLD(n)             BITFIELD_VAL(REG_01C_RX_THRESHOLD, n)
#define REG_01C_RES3                        (1 << 3)
#define REG_01C_DMACTRL_RX_EN               (1 << 2)
#define REG_01C_DMACTRL_TX_EN               (1 << 1)
#define REG_01C_NAND_SEL                    (1 << 0)

// REG_020
#define REG_020_DUALCMD_SHIFT               24
#define REG_020_DUALCMD_MASK                (0xFF << REG_020_DUALCMD_SHIFT)
#define REG_020_DUALCMD(n)                  BITFIELD_VAL(REG_020_DUALCMD, n)
#define REG_020_READCMD_SHIFT               16
#define REG_020_READCMD_MASK                (0xFF << REG_020_READCMD_SHIFT)
#define REG_020_READCMD(n)                  BITFIELD_VAL(REG_020_READCMD, n)
#define REG_020_FREADCMD_SHIFT              8
#define REG_020_FREADCMD_MASK               (0xFF << REG_020_FREADCMD_SHIFT)
#define REG_020_FREADCMD(n)                 BITFIELD_VAL(REG_020_FREADCMD, n)
#define REG_020_QUADCMD_SHIFT               0
#define REG_020_QUADCMD_MASK                (0xFF << REG_020_QUADCMD_SHIFT)
#define REG_020_QUADCMD(n)                  BITFIELD_VAL(REG_020_QUADCMD, n)

// REG_024
#define REG_024_CMD_SEQ1_SHIFT              0
#define REG_024_CMD_SEQ1_MASK               (0xFFFFFFFF << REG_024_CMD_SEQ1_SHIFT)
#define REG_024_CMD_SEQ1(n)                 BITFIELD_VAL(REG_024_CMD_SEQ1, n)

// REG_028
#define REG_028_CMD_SEQ_EN                  (1 << 4)
#define REG_028_CMD_SEQ2_SHIFT              0
#define REG_028_CMD_SEQ2_MASK               (0xF << REG_028_CMD_SEQ2_SHIFT)
#define REG_028_CMD_SEQ2(n)                 BITFIELD_VAL(REG_028_CMD_SEQ2, n)

// REG_02C
#define REG_02C_RES_SHIFT                   1
#define REG_02C_RES_MASK                    (0x7FFFFFFF << REG_02C_RES_SHIFT)
#define REG_02C_RES(n)                      BITFIELD_VAL(REG_02C_RES, n)
#define REG_02C_FETCH_EN                    (1 << 0)

// REG_030
#define REG_030_RES_SHIFT                   2
#define REG_030_RES_MASK                    (0x3FFFFFFF << REG_030_RES_SHIFT)
#define REG_030_RES(n)                      BITFIELD_VAL(REG_030_RES, n)
#define REG_030_ADDR_25_24_SHIFT            0
#define REG_030_ADDR_25_24_MASK             (0x3 << REG_030_ADDR_25_24_SHIFT)
#define REG_030_ADDR_25_24(n)               BITFIELD_VAL(REG_030_ADDR_25_24, n)

// REG_034
#define REG_034_RES_SHIFT                   22
#define REG_034_RES_MASK                    (0x3FF << REG_034_RES_SHIFT)
#define REG_034_RES(n)                      BITFIELD_VAL(REG_034_RES, n)
#define REG_034_SPI_IOEN_SHIFT              18
#define REG_034_SPI_IOEN_MASK               (0xF << REG_034_SPI_IOEN_SHIFT)
#define REG_034_SPI_IOEN(n)                 BITFIELD_VAL(REG_034_SPI_IOEN, n)
#define REG_034_SPI_IODRV_SHIFT             16
#define REG_034_SPI_IODRV_MASK              (0x3 << REG_034_SPI_IODRV_SHIFT)
#define REG_034_SPI_IODRV(n)                BITFIELD_VAL(REG_034_SPI_IODRV, n)
#define REG_034_SPI_IORES_SHIFT             8
#define REG_034_SPI_IORES_MASK              (0xFF << REG_034_SPI_IORES_SHIFT)
#define REG_034_SPI_IORES(n)                BITFIELD_VAL(REG_034_SPI_IORES, n)
#define REG_034_SPI_RDEN_SHIFT              4
#define REG_034_SPI_RDEN_MASK               (0xF << REG_034_SPI_RDEN_SHIFT)
#define REG_034_SPI_RDEN(n)                 BITFIELD_VAL(REG_034_SPI_RDEN, n)
#define REG_034_SPI_RUEN_SHIFT              0
#define REG_034_SPI_RUEN_MASK               (0xF << REG_034_SPI_RUEN_SHIFT)
#define REG_034_SPI_RUEN(n)                 BITFIELD_VAL(REG_034_SPI_RUEN, n)

// REG_038
#define REG_038_MAN_WRAP_BITS_SHIFT         13
#define REG_038_MAN_WRAP_BITS_MASK          (0x3 << REG_038_MAN_WRAP_BITS_SHIFT)
#define REG_038_MAN_WRAP_BITS(n)            BITFIELD_VAL(REG_038_MAN_WRAP_BITS, n)
#define REG_038_MAN_WRAP_ENABLE_SHIFT       12
#define REG_038_MAN_WRAP_ENABLE_MASK        (0x1 << REG_038_MAN_WRAP_ENABLE_SHIFT)
#define REG_038_MAN_WRAP_ENABLE             (REG_038_MAN_WRAP_ENABLE_MASK)
#define REG_038_AUTO_WRAP_CMD_SHIFT         4
#define REG_038_AUTO_WRAP_CMD_MASK          (0xFF << REG_038_AUTO_WRAP_CMD_SHIFT)
#define REG_038_AUTO_WRAP_CMD(n)            BITFIELD_VAL(REG_038_AUTO_WRAP_CMD, n)
#define REG_038_WRAP_MODE_SEL_SHIFT         0
#define REG_038_WRAP_MODE_SEL_MASK          (0x1 << REG_038_WRAP_MODE_SEL_SHIFT)
#define REG_038_WRAP_MODE_SEL               (REG_038_WRAP_MODE_SEL_MASK)

#if (CHIP_FLASH_CTRL_VER >= 3)
// REG_40
#define REG_40_RES_31_6_SHIFT               6
#define REG_40_RES_31_6_MASK                (0x3FFFFFF << REG_40_RES_31_6_SHIFT)
#define REG_40_RES_31_6(n)                  BITFIELD_VAL(REG_40_RES_31_6, n)
#define REG_40_SPH                          (1 << 5)
#define REG_40_RES_4                        (1 << 4)
#define REG_40_DQS_MODE                     (1 << 3)
#define REG_40_DTR_MODE                     (1 << 2)
#define REG_40_OPI_MODE                     (1 << 1)
#define REG_40_QPI_MODE                     (1 << 0)
#endif

// REG_058
#define REG_058_IDX_SHIFT                   0
#define REG_058_IDX_MASK                    (0x7 << REG_058_IDX_SHIFT)
#define REG_058_IDX(n)                      BITFIELD_VAL(REG_058_IDX, n)

// REG_060
#define REG_060_ADDR_BGN_SHIFT              0
#define REG_060_ADDR_BGN_MASK               (0xFFFFFFFF << REG_060_ADDR_BGN_SHIFT)
#define REG_060_ADDR_BGN(n)                 BITFIELD_VAL(REG_060_ADDR_BGN, n)

// REG_064
#define REG_064_ADDR_END_SHIFT              0
#define REG_064_ADDR_END_MASK               (0xFFFFFFFF << REG_064_ADDR_END_SHIFT)
#define REG_064_ADDR_END(n)                 BITFIELD_VAL(REG_064_ADDR_END, n)

// REG_06C
#define REG_06C_DEC_ENABLE_SHIFT            0
#define REG_06C_DEC_ENABLE_MASK             (0x1 << REG_06C_DEC_ENABLE_SHIFT)
#define REG_06C_DEC_ENABLE                  (REG_06C_DEC_ENABLE_MASK)

// REG_0A0
#ifdef CHIP_BEST2300P
#define REG_0A0_LEN_WIDTH3_SHIFT            12
#define REG_0A0_LEN_WIDTH3_MASK             (0xF << REG_0A0_LEN_WIDTH3_SHIFT)
#define REG_0A0_LEN_WIDTH3(n)               BITFIELD_VAL(REG_0A0_LEN_WIDTH3, n)
#define REG_0A0_LEN_WIDTH2_SHIFT            8
#define REG_0A0_LEN_WIDTH2_MASK             (0xF << REG_0A0_LEN_WIDTH2_SHIFT)
#define REG_0A0_LEN_WIDTH2(n)               BITFIELD_VAL(REG_0A0_LEN_WIDTH2, n)
#define REG_0A0_LEN_WIDTH1_SHIFT            4
#define REG_0A0_LEN_WIDTH1_MASK             (0xF << REG_0A0_LEN_WIDTH1_SHIFT)
#define REG_0A0_LEN_WIDTH1(n)               BITFIELD_VAL(REG_0A0_LEN_WIDTH1, n)
#define REG_0A0_LEN_WIDTH0_SHIFT            0
#define REG_0A0_LEN_WIDTH0_MASK             (0xF << REG_0A0_LEN_WIDTH0_SHIFT)
#define REG_0A0_LEN_WIDTH0(n)               BITFIELD_VAL(REG_0A0_LEN_WIDTH0, n)
#endif

// REG_0B0
#define REG_0B0_ADDR3_REMAP_EN              (1 << 4)
#define REG_0B0_ADDR2_REMAP_EN              (1 << 3)
#define REG_0B0_ADDR1_REMAP_EN              (1 << 2)
#define REG_0B0_ADDR0_REMAP_EN              (1 << 1)
#define REG_0B0_GLB_REMAP_EN                (1 << 0)
// End of since 2300p

#if (CHIP_FLASH_CTRL_VER >= 3)
// REG_100
#define REG_100_DUMMY_READ_SHIFT            0
#define REG_100_DUMMY_READ_MASK             (0x1F << REG_100_DUMMY_READ_SHIFT)
#define REG_100_DUMMY_READ(n)               BITFIELD_VAL(REG_100_DUMMY_READ, n)

// REG_104
#define REG_104_DUMMY_DIOR_SHIFT            0
#define REG_104_DUMMY_DIOR_MASK             (0x1F << REG_104_DUMMY_DIOR_SHIFT)
#define REG_104_DUMMY_DIOR(n)               BITFIELD_VAL(REG_104_DUMMY_DIOR, n)

// REG_108
#define REG_108_DUMMY_QIOR_SHIFT            0
#define REG_108_DUMMY_QIOR_MASK             (0x1F << REG_108_DUMMY_QIOR_SHIFT)
#define REG_108_DUMMY_QIOR(n)               BITFIELD_VAL(REG_108_DUMMY_QIOR, n)

// REG_10C
#define REG_10C_DUMMY_FREAD_SHIFT           0
#define REG_10C_DUMMY_FREAD_MASK            (0x1F << REG_10C_DUMMY_FREAD_SHIFT)
#define REG_10C_DUMMY_FREAD(n)              BITFIELD_VAL(REG_10C_DUMMY_FREAD, n)

// REG_110
#define REG_110_DUMMY_DOR_SHIFT             0
#define REG_110_DUMMY_DOR_MASK              (0x1F << REG_110_DUMMY_DOR_SHIFT)
#define REG_110_DUMMY_DOR(n)                BITFIELD_VAL(REG_110_DUMMY_DOR, n)

// REG_114
#define REG_114_DUMMY_QOR_SHIFT             0
#define REG_114_DUMMY_QOR_MASK              (0x1F << REG_114_DUMMY_QOR_SHIFT)
#define REG_114_DUMMY_QOR(n)                BITFIELD_VAL(REG_114_DUMMY_QOR, n)

// REG_118
#define REG_118_DUMMY_QIOWR_SHIFT           0
#define REG_118_DUMMY_QIOWR_MASK            (0x1F << REG_118_DUMMY_QIOWR_SHIFT)
#define REG_118_DUMMY_QIOWR(n)              BITFIELD_VAL(REG_118_DUMMY_QIOWR, n)

// REG_11C
#define REG_11C_DUMMY_BRWR_SHIFT            0
#define REG_11C_DUMMY_BRWR_MASK             (0x1F << REG_11C_DUMMY_BRWR_SHIFT)
#define REG_11C_DUMMY_BRWR(n)               BITFIELD_VAL(REG_11C_DUMMY_BRWR, n)

// REG_120
#define REG_120_DUMMY_BFRD_SHIFT            0
#define REG_120_DUMMY_BFRD_MASK             (0x1F << REG_120_DUMMY_BFRD_SHIFT)
#define REG_120_DUMMY_BFRD(n)               BITFIELD_VAL(REG_120_DUMMY_BFRD, n)

// REG_124
#define REG_124_DUMMY_DTRFRD_SHIFT          0
#define REG_124_DUMMY_DTRFRD_MASK           (0x1F << REG_124_DUMMY_DTRFRD_SHIFT)
#define REG_124_DUMMY_DTRFRD(n)             BITFIELD_VAL(REG_124_DUMMY_DTRFRD, n)

// REG_128
#define REG_128_DUMMY_DTR2RD_SHIFT          0
#define REG_128_DUMMY_DTR2RD_MASK           (0x1F << REG_128_DUMMY_DTR2RD_SHIFT)
#define REG_128_DUMMY_DTR2RD(n)             BITFIELD_VAL(REG_128_DUMMY_DTR2RD, n)

// REG_12C
#define REG_12C_DUMMY_DTR4RD_SHIFT          0
#define REG_12C_DUMMY_DTR4RD_MASK           (0x1F << REG_12C_DUMMY_DTR4RD_SHIFT)
#define REG_12C_DUMMY_DTR4RD(n)             BITFIELD_VAL(REG_12C_DUMMY_DTR4RD, n)

// REG_130
#define REG_130_DUMMY_DTRRWR_SHIFT          0
#define REG_130_DUMMY_DTRRWR_MASK           (0x1F << REG_130_DUMMY_DTRRWR_SHIFT)
#define REG_130_DUMMY_DTRRWR(n)             BITFIELD_VAL(REG_130_DUMMY_DTRRWR, n)

// REG_134
#define REG_134_DUMMY_OCTFRD_SHIFT          0
#define REG_134_DUMMY_OCTFRD_MASK           (0x1F << REG_134_DUMMY_OCTFRD_SHIFT)
#define REG_134_DUMMY_OCTFRD(n)             BITFIELD_VAL(REG_134_DUMMY_OCTFRD, n)

// REG_138
#define REG_138_DUMMY_OCTIORD_SHIFT         0
#define REG_138_DUMMY_OCTIORD_MASK          (0x1F << REG_138_DUMMY_OCTIORD_SHIFT)
#define REG_138_DUMMY_OCTIORD(n)            BITFIELD_VAL(REG_138_DUMMY_OCTIORD, n)

// REG_13C
#define REG_13C_DUMMY_4BDTROCTRD_SHIFT      0
#define REG_13C_DUMMY_4BDTROCTRD_MASK       (0x1F << REG_13C_DUMMY_4BDTROCTRD_SHIFT)
#define REG_13C_DUMMY_4BDTROCTRD(n)         BITFIELD_VAL(REG_13C_DUMMY_4BDTROCTRD, n)

// REG_150
#define REG_150_DUMMY_OTHERS_SHIFT          0
#define REG_150_DUMMY_OTHERS_MASK           (0x1F << REG_150_DUMMY_OTHERS_SHIFT)
#define REG_150_DUMMY_OTHERS(n)             BITFIELD_VAL(REG_150_DUMMY_OTHERS, n)
#endif

#endif

