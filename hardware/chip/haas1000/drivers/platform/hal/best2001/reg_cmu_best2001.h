/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */
#ifndef __REG_CMU_BEST2001_H__
#define __REG_CMU_BEST2001_H__

#include "plat_types.h"

struct CMU_T {
    __IO uint32_t HCLK_ENABLE;      // 0x00
    __IO uint32_t HCLK_DISABLE;     // 0x04
    __IO uint32_t PCLK_ENABLE;      // 0x08
    __IO uint32_t PCLK_DISABLE;     // 0x0C
    __IO uint32_t OCLK_ENABLE;      // 0x10
    __IO uint32_t OCLK_DISABLE;     // 0x14
    __IO uint32_t HCLK_MODE;        // 0x18
    __IO uint32_t PCLK_MODE;        // 0x1C
    __IO uint32_t OCLK_MODE;        // 0x20
    __IO uint32_t RESERVED_024;     // 0x24
    __IO uint32_t HRESET_PULSE;     // 0x28
    __IO uint32_t PRESET_PULSE;     // 0x2C
    __IO uint32_t ORESET_PULSE;     // 0x30
    __IO uint32_t HRESET_SET;       // 0x34
    __IO uint32_t HRESET_CLR;       // 0x38
    __IO uint32_t PRESET_SET;       // 0x3C
    __IO uint32_t PRESET_CLR;       // 0x40
    __IO uint32_t ORESET_SET;       // 0x44
    __IO uint32_t ORESET_CLR;       // 0x48
    __IO uint32_t TIMER0_CLK;       // 0x4C
    __IO uint32_t BOOTMODE;         // 0x50
    __IO uint32_t MCU_TIMER;        // 0x54
    __IO uint32_t SLEEP;            // 0x58
    __IO uint32_t PERIPH_CLK;       // 0x5C
    __IO uint32_t SYS_CLK_ENABLE;   // 0x60
    __IO uint32_t SYS_CLK_DISABLE;  // 0x64
    __IO uint32_t ADMA_CH15_REQ;    // 0x68
    __IO uint32_t BOOT_DVS;         // 0x6C
    __IO uint32_t UART_CLK;         // 0x70
    __IO uint32_t I2C_CLK;          // 0x74
    __IO uint32_t RAM_CFG0;         // 0x78
    __IO uint32_t RAM_CFG1;         // 0x7C
    __IO uint32_t WRITE_UNLOCK;     // 0x80
    __IO uint32_t WAKEUP_MASK0;     // 0x84
    __IO uint32_t WAKEUP_MASK1;     // 0x88
    __IO uint32_t WAKEUP_CLK_CFG;   // 0x8C
    __IO uint32_t TIMER1_CLK;       // 0x90
    __IO uint32_t TIMER2_CLK;       // 0x94
    __IO uint32_t CP2MCU_IRQ_SET;   // 0x98
    __IO uint32_t CP2MCU_IRQ_CLR;   // 0x9C
    __IO uint32_t ISIRQ_SET;        // 0xA0
    __IO uint32_t ISIRQ_CLR;        // 0xA4
    __IO uint32_t SYS_DIV;          // 0xA8
    __IO uint32_t RESERVED_0AC;     // 0xAC
    __IO uint32_t MCU2BT_INTMASK0;  // 0xB0
    __IO uint32_t MCU2BT_INTMASK1;  // 0xB4
    __IO uint32_t MCU2CP_IRQ_SET;   // 0xB8
    __IO uint32_t MCU2CP_IRQ_CLR;   // 0xBC
    __IO uint32_t MEMSC[4];         // 0xC0
    __I  uint32_t MEMSC_STATUS;     // 0xD0
    __IO uint32_t ADMA_CH0_4_REQ;   // 0xD4
    __IO uint32_t ADMA_CH5_9_REQ;   // 0xD8
    __IO uint32_t ADMA_CH10_14_REQ; // 0xDC
    __IO uint32_t GDMA_CH0_4_REQ;   // 0xE0
    __IO uint32_t GDMA_CH5_9_REQ;   // 0xE4
    __IO uint32_t GDMA_CH10_14_REQ; // 0xE8
    __IO uint32_t GDMA_CH15_REQ;    // 0xEC
    __IO uint32_t MISC;             // 0xF0
    __IO uint32_t SIMU_RES;         // 0xF4
    __IO uint32_t SEC_ROM_CFG;      // 0xF8
    __IO uint32_t ACC_CTRL;         // 0xFC
    __IO uint32_t CP_VTOR;          // 0x100
    __IO uint32_t XCLK_ENABLE;      // 0x104
    __IO uint32_t XCLK_DISABLE;     // 0x108
    __IO uint32_t XCLK_MODE;        // 0x10C
    __IO uint32_t XRESET_PULSE;     // 0x110
    __IO uint32_t XRESET_SET;       // 0x114
    __IO uint32_t XRESET_CLR;       // 0x118
    __IO uint32_t DSP_DIV;          // 0x11C
    __IO uint32_t WAKEUP_MASK2;     // 0x120
    __IO uint32_t MCU2BT_INTMASK2;  // 0x124
    __IO uint32_t MCU2WF_INTMASK0;  // 0x128
    __IO uint32_t MCU2WF_INTMASK1;  // 0x12C
    __IO uint32_t MCU2WF_INTMASK2;  // 0x130
    __IO uint32_t MCU2DSP_INTMASK0; // 0x134
    __IO uint32_t MCU2DSP_INTMASK1; // 0x138
    __IO uint32_t MCU2DSP_INTMASK2; // 0x13C
    __IO uint32_t DSP_CFG0;         // 0x140
    __IO uint32_t DSP_CFG1;         // 0x144
    __IO uint32_t DSP_CFG2;         // 0x148
    __IO uint32_t DSP_CFG3;         // 0x14C
    __IO uint32_t APCLK_ENABLE;     // 0x150
    __IO uint32_t APCLK_DISABLE;    // 0x154
    __IO uint32_t APCLK_MODE;       // 0x158
    __IO uint32_t APRESET_PULSE;    // 0x15C
    __IO uint32_t APRESET_SET;      // 0x160
    __IO uint32_t APRESET_CLR;      // 0x164
    __IO uint32_t AP_TIMER1_CLK;    // 0x168
    __IO uint32_t AP_TIMER2_CLK;    // 0x16C
    __IO uint32_t RESERVED_170[4];  // 0x170
    __IO uint32_t MEMSC_AUX[0x1C];  // 0x180
};

enum CMU_DMA_REQ_T {
    CODEC_DMA_RX_REQ            = 0,
    CODEC_DMA_TX_REQ            = 1,
    DSD_DMA_RX_REQ              = 2,
    DSD_DMA_TX_REQ              = 3,
    CODEC_DMA_TX2_REQ           = 4,
    NULL_DMA_REQ                = 5,
    FLS_DMA_RX_REQ              = 10,
    FLS_DMA_TX_REQ              = 11,
    BTDUMP_DMA_REQ              = 12,
    SDEMMC_DMA_REQ              = 13,
    I2C0_DMA_RX_REQ             = 14,
    I2C0_DMA_TX_REQ             = 15,
    I2C1_DMA_RX_REQ             = 16,
    I2C1_DMA_TX_REQ             = 17,
    SPILCD0_DMA_RX_REQ          = 18,
    SPILCD0_DMA_TX_REQ          = 19,
    SPILCD1_DMA_RX_REQ          = 20,
    SPILCD1_DMA_TX_REQ          = 21,
    SPI_ITN_DMA_RX_REQ          = 22,
    SPI_ITN_DMA_TX_REQ          = 23,
    UART0_DMA_RX_REQ            = 24,
    UART0_DMA_TX_REQ            = 25,
    UART1_DMA_RX_REQ            = 26,
    UART1_DMA_TX_REQ            = 27,
    UART2_DMA_RX_REQ            = 28,
    UART2_DMA_TX_REQ            = 29,
    PCM_DMA_RX_REQ              = 30,
    PCM_DMA_TX_REQ              = 31,
    I2S0_DMA_RX_REQ             = 32,
    I2S0_DMA_TX_REQ             = 33,
    SPDIF0_DMA_RX_REQ           = 34,
    SPDIF0_DMA_TX_REQ           = 35,
};
// reg_000
#define CMU_MANUAL_HCLK_ENABLE(n)               (((n) & 0xFFFFFFFF) << 0)
#define CMU_MANUAL_HCLK_ENABLE_MASK             (0xFFFFFFFF << 0)
#define CMU_MANUAL_HCLK_ENABLE_SHIFT            (0)

// reg_004
#define CMU_MANUAL_HCLK_DISABLE(n)              (((n) & 0xFFFFFFFF) << 0)
#define CMU_MANUAL_HCLK_DISABLE_MASK            (0xFFFFFFFF << 0)
#define CMU_MANUAL_HCLK_DISABLE_SHIFT           (0)

// reg_008
#define CMU_MANUAL_PCLK_ENABLE(n)               (((n) & 0xFFFFFFFF) << 0)
#define CMU_MANUAL_PCLK_ENABLE_MASK             (0xFFFFFFFF << 0)
#define CMU_MANUAL_PCLK_ENABLE_SHIFT            (0)

// reg_00c
#define CMU_MANUAL_PCLK_DISABLE(n)              (((n) & 0xFFFFFFFF) << 0)
#define CMU_MANUAL_PCLK_DISABLE_MASK            (0xFFFFFFFF << 0)
#define CMU_MANUAL_PCLK_DISABLE_SHIFT           (0)

// reg_010
#define CMU_MANUAL_OCLK_ENABLE(n)               (((n) & 0xFFFFFFFF) << 0)
#define CMU_MANUAL_OCLK_ENABLE_MASK             (0xFFFFFFFF << 0)
#define CMU_MANUAL_OCLK_ENABLE_SHIFT            (0)

// reg_014
#define CMU_MANUAL_OCLK_DISABLE(n)              (((n) & 0xFFFFFFFF) << 0)
#define CMU_MANUAL_OCLK_DISABLE_MASK            (0xFFFFFFFF << 0)
#define CMU_MANUAL_OCLK_DISABLE_SHIFT           (0)

// reg_018
#define CMU_MODE_HCLK(n)                        (((n) & 0xFFFFFFFF) << 0)
#define CMU_MODE_HCLK_MASK                      (0xFFFFFFFF << 0)
#define CMU_MODE_HCLK_SHIFT                     (0)

// reg_01c
#define CMU_MODE_PCLK(n)                        (((n) & 0xFFFFFFFF) << 0)
#define CMU_MODE_PCLK_MASK                      (0xFFFFFFFF << 0)
#define CMU_MODE_PCLK_SHIFT                     (0)

// reg_020
#define CMU_MODE_OCLK(n)                        (((n) & 0xFFFFFFFF) << 0)
#define CMU_MODE_OCLK_MASK                      (0xFFFFFFFF << 0)
#define CMU_MODE_OCLK_SHIFT                     (0)

// reg_028
#define CMU_HRESETN_PULSE(n)                    (((n) & 0xFFFFFFFF) << 0)
#define CMU_HRESETN_PULSE_MASK                  (0xFFFFFFFF << 0)
#define CMU_HRESETN_PULSE_SHIFT                 (0)

#define SYS_PRST_NUM                            22

// reg_2c
#define CMU_PRESETN_PULSE(n)                    (((n) & 0xFFFFFFFF) << 0)
#define CMU_PRESETN_PULSE_MASK                  (0xFFFFFFFF << 0)
#define CMU_PRESETN_PULSE_SHIFT                 (0)
#define CMU_GLOBAL_RESETN_PULSE                 (1 << (SYS_PRST_NUM+1-1))

// reg_030
#define CMU_ORESETN_PULSE(n)                    (((n) & 0xFFFFFFFF) << 0)
#define CMU_ORESETN_PULSE_MASK                  (0xFFFFFFFF << 0)
#define CMU_ORESETN_PULSE_SHIFT                 (0)

// reg_034
#define CMU_HRESETN_SET(n)                      (((n) & 0xFFFFFFFF) << 0)
#define CMU_HRESETN_SET_MASK                    (0xFFFFFFFF << 0)
#define CMU_HRESETN_SET_SHIFT                   (0)

// reg_038
#define CMU_HRESETN_CLR(n)                      (((n) & 0xFFFFFFFF) << 0)
#define CMU_HRESETN_CLR_MASK                    (0xFFFFFFFF << 0)
#define CMU_HRESETN_CLR_SHIFT                   (0)

// reg_03c
#define CMU_PRESETN_SET(n)                      (((n) & 0xFFFFFFFF) << 0)
#define CMU_PRESETN_SET_MASK                    (0xFFFFFFFF << 0)
#define CMU_PRESETN_SET_SHIFT                   (0)
#define CMU_GLOBAL_RESETN_SET                   (1 << (SYS_PRST_NUM+1-1))

// reg_040
#define CMU_PRESETN_CLR(n)                      (((n) & 0xFFFFFFFF) << 0)
#define CMU_PRESETN_CLR_MASK                    (0xFFFFFFFF << 0)
#define CMU_PRESETN_CLR_SHIFT                   (0)
#define CMU_GLOBAL_RESETN_CLR                   (1 << (SYS_PRST_NUM+1-1))

// reg_044
#define CMU_ORESETN_SET(n)                      (((n) & 0xFFFFFFFF) << 0)
#define CMU_ORESETN_SET_MASK                    (0xFFFFFFFF << 0)
#define CMU_ORESETN_SET_SHIFT                   (0)

// reg_048
#define CMU_ORESETN_CLR(n)                      (((n) & 0xFFFFFFFF) << 0)
#define CMU_ORESETN_CLR_MASK                    (0xFFFFFFFF << 0)
#define CMU_ORESETN_CLR_SHIFT                   (0)

// reg_04c
#define CMU_CFG_DIV_TIMER00(n)                  (((n) & 0xFFFF) << 0)
#define CMU_CFG_DIV_TIMER00_MASK                (0xFFFF << 0)
#define CMU_CFG_DIV_TIMER00_SHIFT               (0)
#define CMU_CFG_DIV_TIMER01(n)                  (((n) & 0xFFFF) << 16)
#define CMU_CFG_DIV_TIMER01_MASK                (0xFFFF << 16)
#define CMU_CFG_DIV_TIMER01_SHIFT               (16)

// reg_050
#define CMU_WATCHDOG_RESET                      (1 << 0)
#define CMU_SOFT_GLOBLE_RESET                   (1 << 1)
#define CMU_RTC_INTR_H                          (1 << 2)
#define CMU_CHG_INTR_H                          (1 << 3)
#define CMU_SOFT_BOOT_MODE(n)                   (((n) & 0xFFFFFFF) << 4)
#define CMU_SOFT_BOOT_MODE_MASK                 (0xFFFFFFF << 4)
#define CMU_SOFT_BOOT_MODE_SHIFT                (4)

// reg_054
#define CMU_CFG_HCLK_MCU_OFF_TIMER(n)           (((n) & 0xFF) << 0)
#define CMU_CFG_HCLK_MCU_OFF_TIMER_MASK         (0xFF << 0)
#define CMU_CFG_HCLK_MCU_OFF_TIMER_SHIFT        (0)
#define CMU_HCLK_MCU_ENABLE                     (1 << 8)
#define CMU_SECURE_BOOT_JTAG                    (1 << 9)
#define CMU_RAM_RETN_UP_EARLY                   (1 << 10)
#define CMU_FLS_SEC_MSK_EN                      (1 << 11)
#define CMU_SECURE_BOOT_I2C                     (1 << 12)
#define CMU_DEBUG_REG_SEL(n)                    (((n) & 0x7) << 13)
#define CMU_DEBUG_REG_SEL_MASK                  (0x7 << 13)
#define CMU_DEBUG_REG_SEL_SHIFT                 (13)

// reg_058
#define CMU_SLEEP_TIMER(n)                      (((n) & 0xFFFFFF) << 0)
#define CMU_SLEEP_TIMER_MASK                    (0xFFFFFF << 0)
#define CMU_SLEEP_TIMER_SHIFT                   (0)
#define CMU_SLEEP_TIMER_EN                      (1 << 24)
#define CMU_DEEPSLEEP_EN                        (1 << 25)
#define CMU_DEEPSLEEP_ROMRAM_EN                 (1 << 26)
#define CMU_MANUAL_RAM_RETN                     (1 << 27)
#define CMU_DEEPSLEEP_START                     (1 << 28)
#define CMU_DEEPSLEEP_MODE                      (1 << 29)
#define CMU_PU_OSC                              (1 << 30)
#define CMU_WAKEDOWN_DEEPSLEEP_L                (1 << 31)

// reg_05c
#define CMU_CFG_DIV_SDMMC(n)                    (((n) & 0xF) << 0)
#define CMU_CFG_DIV_SDMMC_MASK                  (0xF << 0)
#define CMU_CFG_DIV_SDMMC_SHIFT                 (0)
#define CMU_SEL_OSCX2_SDMMC                     (1 << 4)
#define CMU_SEL_PLL_SDMMC                       (1 << 5)
#define CMU_EN_PLL_SDMMC                        (1 << 6)
#define CMU_SEL_32K_TIMER(n)                    (((n) & 0x7) << 7)
#define CMU_SEL_32K_TIMER_MASK                  (0x7 << 7)
#define CMU_SEL_32K_TIMER_SHIFT                 (7)
#define CMU_SEL_32K_WDT                         (1 << 10)
#define CMU_SEL_TIMER_FAST(n)                   (((n) & 0x7) << 11)
#define CMU_SEL_TIMER_FAST_MASK                 (0x7 << 11)
#define CMU_SEL_TIMER_FAST_SHIFT                (11)
#define CMU_CFG_CLK_OUT(n)                      (((n) & 0xF) << 14)
#define CMU_CFG_CLK_OUT_MASK                    (0xF << 14)
#define CMU_CFG_CLK_OUT_SHIFT                   (14)
#define CMU_SPI_I2C_DMAREQ_SEL                  (1 << 18)
#define CMU_MASK_OBS(n)                         (((n) & 0x3F) << 19)
#define CMU_MASK_OBS_MASK                       (0x3F << 19)
#define CMU_MASK_OBS_SHIFT                      (19)
#define CMU_JTAG_SEL_CP                         (1 << 25)
#define CMU_JTAG_SEL_A7                         (1 << 26)
#define CMU_BT_ALLIRQ_MASK                      (1 << 27)
#define CMU_WF_ALLIRQ_MASK                      (1 << 28)
#define CMU_A7_ALLIRQ_MASK                      (1 << 29)

// reg_060
#define CMU_RSTN_DIV_MCU_ENABLE                 (1 << 0)
#define CMU_BYPASS_DIV_MCU_ENABLE               (1 << 1)
#define CMU_SEL_MCU_OSC_4_ENABLE                (1 << 2)
#define CMU_SEL_MCU_OSC_2_ENABLE                (1 << 3)
#define CMU_SEL_MCU_OSCX4_ENABLE                (1 << 4)
#define CMU_SEL_MCU_SLOW_ENABLE                 (1 << 5)
#define CMU_SEL_MCU_FAST_ENABLE                 (1 << 6)
#define CMU_SEL_MCU_PLL_ENABLE                  (1 << 7)
#define CMU_RSTN_DIV_A7_ENABLE                  (1 << 8)
#define CMU_BYPASS_DIV_A7_ENABLE                (1 << 9)
#define CMU_SEL_A7_OSC_4_ENABLE                 (1 << 10)
#define CMU_SEL_A7_OSC_2_ENABLE                 (1 << 11)
#define CMU_SEL_A7_OSCX4_ENABLE                 (1 << 12)
#define CMU_SEL_A7_SLOW_ENABLE                  (1 << 13)
#define CMU_SEL_A7_FAST_ENABLE                  (1 << 14)
#define CMU_SEL_A7_PLL_ENABLE                   (1 << 15)
#define CMU_EN_PLLMCU_ENABLE                    (1 << 16)
#define CMU_PU_PLLMCU_ENABLE                    (1 << 17)
#define CMU_EN_PLLA7_ENABLE                     (1 << 18)
#define CMU_PU_PLLA7_ENABLE                     (1 << 19)

// reg_064
#define CMU_RSTN_DIV_MCU_DISABLE                (1 << 0)
#define CMU_BYPASS_DIV_MCU_DISABLE              (1 << 1)
#define CMU_SEL_MCU_OSC_4_DISABLE               (1 << 2)
#define CMU_SEL_MCU_OSC_2_DISABLE               (1 << 3)
#define CMU_SEL_MCU_OSCX4_DISABLE               (1 << 4)
#define CMU_SEL_MCU_SLOW_DISABLE                (1 << 5)
#define CMU_SEL_MCU_FAST_DISABLE                (1 << 6)
#define CMU_SEL_MCU_PLL_DISABLE                 (1 << 7)
#define CMU_RSTN_DIV_A7_DISABLE                 (1 << 8)
#define CMU_BYPASS_DIV_A7_DISABLE               (1 << 9)
#define CMU_SEL_A7_OSC_4_DISABLE                (1 << 10)
#define CMU_SEL_A7_OSC_2_DISABLE                (1 << 11)
#define CMU_SEL_A7_OSCX4_DISABLE                (1 << 12)
#define CMU_SEL_A7_SLOW_DISABLE                 (1 << 13)
#define CMU_SEL_A7_FAST_DISABLE                 (1 << 14)
#define CMU_SEL_A7_PLL_DISABLE                  (1 << 15)
#define CMU_EN_PLLMCU_DISABLE                   (1 << 16)
#define CMU_PU_PLLMCU_DISABLE                   (1 << 17)
#define CMU_EN_PLLA7_DISABLE                    (1 << 18)
#define CMU_PU_PLLA7_DISABLE                    (1 << 19)

// reg_068
#define CMU_ADMA_CH15_REQ_IDX(n)                (((n) & 0x3F) << 0)
#define CMU_ADMA_CH15_REQ_IDX_MASK              (0x3F << 0)
#define CMU_ADMA_CH15_REQ_IDX_SHIFT             (0)

// reg_06c
#define CMU_ROM_EMA(n)                          (((n) & 0x7) << 0)
#define CMU_ROM_EMA_MASK                        (0x7 << 0)
#define CMU_ROM_EMA_SHIFT                       (0)
#define CMU_ROM_KEN                             (1 << 3)
#define CMU_ROM_PGEN(n)                         (((n) & 0x3) << 4)
#define CMU_ROM_PGEN_MASK                       (0x3 << 4)
#define CMU_ROM_PGEN_SHIFT                      (4)
#define CMU_RAM_EMA(n)                          (((n) & 0x7) << 6)
#define CMU_RAM_EMA_MASK                        (0x7 << 6)
#define CMU_RAM_EMA_SHIFT                       (6)
#define CMU_RAM_EMAW(n)                         (((n) & 0x3) << 9)
#define CMU_RAM_EMAW_MASK                       (0x3 << 9)
#define CMU_RAM_EMAW_SHIFT                      (9)
#define CMU_RF_EMA(n)                           (((n) & 0x7) << 11)
#define CMU_RF_EMA_MASK                         (0x7 << 11)
#define CMU_RF_EMA_SHIFT                        (11)
#define CMU_RF_EMAW(n)                          (((n) & 0x3) << 14)
#define CMU_RF_EMAW_MASK                        (0x3 << 14)
#define CMU_RF_EMAW_SHIFT                       (14)
#define CMU_RF2_EMAA(n)                         (((n) & 0x7) << 16)
#define CMU_RF2_EMAA_MASK                       (0x7 << 16)
#define CMU_RF2_EMAA_SHIFT                      (16)
#define CMU_RF2_EMAWA(n)                        (((n) & 0x3) << 19)
#define CMU_RF2_EMAWA_MASK                      (0x3 << 19)
#define CMU_RF2_EMAWA_SHIFT                     (19)
#define CMU_RF2_EMAB(n)                         (((n) & 0x7) << 21)
#define CMU_RF2_EMAB_MASK                       (0x7 << 21)
#define CMU_RF2_EMAB_SHIFT                      (21)
#define CMU_RF2_EMAWB(n)                        (((n) & 0x3) << 24)
#define CMU_RF2_EMAWB_MASK                      (0x3 << 24)
#define CMU_RF2_EMAWB_SHIFT                     (24)

// reg_070
#define CMU_CFG_DIV_UART0(n)                    (((n) & 0x1F) << 0)
#define CMU_CFG_DIV_UART0_MASK                  (0x1F << 0)
#define CMU_CFG_DIV_UART0_SHIFT                 (0)
#define CMU_SEL_OSCX2_UART0                     (1 << 5)
#define CMU_SEL_PLL_UART0                       (1 << 6)
#define CMU_EN_PLL_UART0                        (1 << 7)
#define CMU_CFG_DIV_UART1(n)                    (((n) & 0x1F) << 8)
#define CMU_CFG_DIV_UART1_MASK                  (0x1F << 8)
#define CMU_CFG_DIV_UART1_SHIFT                 (8)
#define CMU_SEL_OSCX2_UART1                     (1 << 13)
#define CMU_SEL_PLL_UART1                       (1 << 14)
#define CMU_EN_PLL_UART1                        (1 << 15)
#define CMU_CFG_DIV_UART2(n)                    (((n) & 0x1F) << 16)
#define CMU_CFG_DIV_UART2_MASK                  (0x1F << 16)
#define CMU_CFG_DIV_UART2_SHIFT                 (16)
#define CMU_SEL_OSCX2_UART2                     (1 << 21)
#define CMU_SEL_PLL_UART2                       (1 << 22)
#define CMU_EN_PLL_UART2                        (1 << 23)

// reg_074
#define CMU_CFG_DIV_I2C(n)                      (((n) & 0xFF) << 0)
#define CMU_CFG_DIV_I2C_MASK                    (0xFF << 0)
#define CMU_CFG_DIV_I2C_SHIFT                   (0)
#define CMU_SEL_OSC_I2C                         (1 << 8)
#define CMU_SEL_OSCX2_I2C                       (1 << 9)
#define CMU_SEL_PLL_I2C                         (1 << 10)
#define CMU_EN_PLL_I2C                          (1 << 11)
#define CMU_POL_CLK_PCM_IN                      (1 << 12)
#define CMU_SEL_PCM_CLKIN                       (1 << 13)
#define CMU_EN_CLK_PCM_OUT                      (1 << 14)
#define CMU_POL_CLK_PCM_OUT                     (1 << 15)
#define CMU_POL_CLK_I2S0_IN                     (1 << 16)
#define CMU_SEL_I2S0_CLKIN                      (1 << 17)
#define CMU_EN_CLK_I2S0_OUT                     (1 << 18)
#define CMU_POL_CLK_I2S0_OUT                    (1 << 19)
#define CMU_FORCE_PU_OFF                        (1 << 20)
#define CMU_LOCK_CPU_EN                         (1 << 21)
#define CMU_SEL_ROM_FAST                        (1 << 22)
#define CMU_POL_CLK_I2S1_IN                     (1 << 23)
#define CMU_SEL_I2S1_CLKIN                      (1 << 24)
#define CMU_EN_CLK_I2S1_OUT                     (1 << 25)
#define CMU_POL_CLK_I2S1_OUT                    (1 << 26)

// reg_078
#define CMU_RAM_RET1N0(n)                       (((n) & 0xFFFFFFFF) << 0)
#define CMU_RAM_RET1N0_MASK                     (0xFFFFFFFF << 0)
#define CMU_RAM_RET1N0_SHIFT                    (0)

// reg_07c
#define CMU_RAM_RET1N1(n)                       (((n) & 0xFFFFFFFF) << 0)
#define CMU_RAM_RET1N1_MASK                     (0xFFFFFFFF << 0)
#define CMU_RAM_RET1N1_SHIFT                    (0)

// reg_080
#define CMU_WRITE_UNLOCK_H                      (1 << 0)
#define CMU_WRITE_UNLOCK_STATUS                 (1 << 1)

// reg_084
#define CMU_WAKEUP_IRQ_MASK0(n)                 (((n) & 0xFFFFFFFF) << 0)
#define CMU_WAKEUP_IRQ_MASK0_MASK               (0xFFFFFFFF << 0)
#define CMU_WAKEUP_IRQ_MASK0_SHIFT              (0)

// reg_088
#define CMU_WAKEUP_IRQ_MASK1(n)                 (((n) & 0xFFFFFFFF) << 0)
#define CMU_WAKEUP_IRQ_MASK1_MASK               (0xFFFFFFFF << 0)
#define CMU_WAKEUP_IRQ_MASK1_SHIFT              (0)

// reg_08c
#define CMU_TIMER_WT26(n)                       (((n) & 0xFF) << 0)
#define CMU_TIMER_WT26_MASK                     (0xFF << 0)
#define CMU_TIMER_WT26_SHIFT                    (0)
#define CMU_TIMER_WTPLL(n)                      (((n) & 0xF) << 8)
#define CMU_TIMER_WTPLL_MASK                    (0xF << 8)
#define CMU_TIMER_WTPLL_SHIFT                   (8)
#define CMU_LPU_AUTO_SWITCH26                   (1 << 12)
#define CMU_LPU_AUTO_SWITCHPLL                  (1 << 13)
#define CMU_LPU_STATUS_26M                      (1 << 14)
#define CMU_LPU_STATUS_PLL                      (1 << 15)
#define CMU_LPU_EN_MCU                          (1 << 16)
#define CMU_LPU_EN_A7                           (1 << 17)
#define CMU_OSC_READY_MODE                      (1 << 18)
#define CMU_CFG_SRAM_IN_M33(n)                  (((n) & 0x7F) << 19)
#define CMU_CFG_SRAM_IN_M33_MASK                (0x7F << 19)
#define CMU_CFG_SRAM_IN_M33_SHIFT               (19)

// reg_090
#define CMU_CFG_DIV_TIMER10(n)                  (((n) & 0xFFFF) << 0)
#define CMU_CFG_DIV_TIMER10_MASK                (0xFFFF << 0)
#define CMU_CFG_DIV_TIMER10_SHIFT               (0)
#define CMU_CFG_DIV_TIMER11(n)                  (((n) & 0xFFFF) << 16)
#define CMU_CFG_DIV_TIMER11_MASK                (0xFFFF << 16)
#define CMU_CFG_DIV_TIMER11_SHIFT               (16)

// reg_094
#define CMU_CFG_DIV_TIMER20(n)                  (((n) & 0xFFFF) << 0)
#define CMU_CFG_DIV_TIMER20_MASK                (0xFFFF << 0)
#define CMU_CFG_DIV_TIMER20_SHIFT               (0)
#define CMU_CFG_DIV_TIMER21(n)                  (((n) & 0xFFFF) << 16)
#define CMU_CFG_DIV_TIMER21_MASK                (0xFFFF << 16)
#define CMU_CFG_DIV_TIMER21_SHIFT               (16)

// reg_098
#define CMU_MCU2CP_DATA_DONE_SET                (1 << 0)
#define CMU_MCU2CP_DATA1_DONE_SET               (1 << 1)
#define CMU_MCU2CP_DATA2_DONE_SET               (1 << 2)
#define CMU_MCU2CP_DATA3_DONE_SET               (1 << 3)
#define CMU_CP2MCU_DATA_IND_SET                 (1 << 4)
#define CMU_CP2MCU_DATA1_IND_SET                (1 << 5)
#define CMU_CP2MCU_DATA2_IND_SET                (1 << 6)
#define CMU_CP2MCU_DATA3_IND_SET                (1 << 7)

// reg_09c
#define CMU_MCU2CP_DATA_DONE_CLR                (1 << 0)
#define CMU_MCU2CP_DATA1_DONE_CLR               (1 << 1)
#define CMU_MCU2CP_DATA2_DONE_CLR               (1 << 2)
#define CMU_MCU2CP_DATA3_DONE_CLR               (1 << 3)
#define CMU_CP2MCU_DATA_IND_CLR                 (1 << 4)
#define CMU_CP2MCU_DATA1_IND_CLR                (1 << 5)
#define CMU_CP2MCU_DATA2_IND_CLR                (1 << 6)
#define CMU_CP2MCU_DATA3_IND_CLR                (1 << 7)

// reg_0a0
#define CMU_BT2MCU_DATA_DONE_SET                (1 << 0)
#define CMU_BT2MCU_DATA1_DONE_SET               (1 << 1)
#define CMU_MCU2BT_DATA_IND_SET                 (1 << 2)
#define CMU_MCU2BT_DATA1_IND_SET                (1 << 3)

// reg_0a4
#define CMU_BT2MCU_DATA_DONE_CLR                (1 << 0)
#define CMU_BT2MCU_DATA1_DONE_CLR               (1 << 1)
#define CMU_MCU2BT_DATA_IND_CLR                 (1 << 2)
#define CMU_MCU2BT_DATA1_IND_CLR                (1 << 3)

// reg_0a8
#define CMU_CFG_DIV_MCU(n)                      (((n) & 0x3) << 0)
#define CMU_CFG_DIV_MCU_MASK                    (0x3 << 0)
#define CMU_CFG_DIV_MCU_SHIFT                   (0)
#define CMU_SEL_SMP_MCU(n)                      (((n) & 0x3) << 2)
#define CMU_SEL_SMP_MCU_MASK                    (0x3 << 2)
#define CMU_SEL_SMP_MCU_SHIFT                   (2)
#define CMU_SEL_USB_6M                          (1 << 4)
#define CMU_SEL_USB_SRC(n)                      (((n) & 0x7) << 5)
#define CMU_SEL_USB_SRC_MASK                    (0x7 << 5)
#define CMU_SEL_USB_SRC_SHIFT                   (5)
#define CMU_POL_CLK_USB                         (1 << 8)
#define CMU_USB_ID                              (1 << 9)
#define CMU_CFG_DIV_PCLK(n)                     (((n) & 0x3) << 10)
#define CMU_CFG_DIV_PCLK_MASK                   (0x3 << 10)
#define CMU_CFG_DIV_PCLK_SHIFT                  (10)
#define CMU_CFG_DIV_SPI0(n)                     (((n) & 0xF) << 12)
#define CMU_CFG_DIV_SPI0_MASK                   (0xF << 12)
#define CMU_CFG_DIV_SPI0_SHIFT                  (12)
#define CMU_SEL_OSCX2_SPI0                      (1 << 16)
#define CMU_SEL_PLL_SPI0                        (1 << 17)
#define CMU_EN_PLL_SPI0                         (1 << 18)
#define CMU_CFG_DIV_SPI1(n)                     (((n) & 0xF) << 19)
#define CMU_CFG_DIV_SPI1_MASK                   (0xF << 19)
#define CMU_CFG_DIV_SPI1_SHIFT                  (19)
#define CMU_SEL_OSCX2_SPI1                      (1 << 23)
#define CMU_SEL_PLL_SPI1                        (1 << 24)
#define CMU_EN_PLL_SPI1                         (1 << 25)
#define CMU_SEL_OSCX2_SPI2                      (1 << 26)
#define CMU_DSD_PCM_DMAREQ_SEL                  (1 << 27)

// reg_0ac
#define CMU_DMA_HANDSHAKE_SWAP(n)               (((n) & 0xFFFF) << 0)
#define CMU_DMA_HANDSHAKE_SWAP_MASK             (0xFFFF << 0)
#define CMU_DMA_HANDSHAKE_SWAP_SHIFT            (0)
#define CMU_RESERVED_2(n)                       (((n) & 0xFFFF) << 16)
#define CMU_RESERVED_2_MASK                     (0xFFFF << 16)
#define CMU_RESERVED_2_SHIFT                    (16)

// reg_0b0
#define CMU_MCU2BT_INTISR_MASK0(n)              (((n) & 0xFFFFFFFF) << 0)
#define CMU_MCU2BT_INTISR_MASK0_MASK            (0xFFFFFFFF << 0)
#define CMU_MCU2BT_INTISR_MASK0_SHIFT           (0)

// reg_0b4
#define CMU_MCU2BT_INTISR_MASK1(n)              (((n) & 0xFFFFFFFF) << 0)
#define CMU_MCU2BT_INTISR_MASK1_MASK            (0xFFFFFFFF << 0)
#define CMU_MCU2BT_INTISR_MASK1_SHIFT           (0)

// reg_0b8
#define CMU_CP2MCU_DATA_DONE_SET                (1 << 0)
#define CMU_CP2MCU_DATA1_DONE_SET               (1 << 1)
#define CMU_CP2MCU_DATA2_DONE_SET               (1 << 2)
#define CMU_CP2MCU_DATA3_DONE_SET               (1 << 3)
#define CMU_MCU2CP_DATA_IND_SET                 (1 << 4)
#define CMU_MCU2CP_DATA1_IND_SET                (1 << 5)
#define CMU_MCU2CP_DATA2_IND_SET                (1 << 6)
#define CMU_MCU2CP_DATA3_IND_SET                (1 << 7)

// reg_0bc
#define CMU_CP2MCU_DATA_DONE_CLR                (1 << 0)
#define CMU_CP2MCU_DATA1_DONE_CLR               (1 << 1)
#define CMU_CP2MCU_DATA2_DONE_CLR               (1 << 2)
#define CMU_CP2MCU_DATA3_DONE_CLR               (1 << 3)
#define CMU_MCU2CP_DATA_IND_CLR                 (1 << 4)
#define CMU_MCU2CP_DATA1_IND_CLR                (1 << 5)
#define CMU_MCU2CP_DATA2_IND_CLR                (1 << 6)
#define CMU_MCU2CP_DATA3_IND_CLR                (1 << 7)

// reg_0c0
#define CMU_MEMSC0                              (1 << 0)

// reg_0c4
#define CMU_MEMSC1                              (1 << 0)

// reg_0c8
#define CMU_MEMSC2                              (1 << 0)

// reg_0cc
#define CMU_MEMSC3                              (1 << 0)

// reg_0d0
#define CMU_MEMSC_STATUS0                       (1 << 0)
#define CMU_MEMSC_STATUS1                       (1 << 1)
#define CMU_MEMSC_STATUS2                       (1 << 2)
#define CMU_MEMSC_STATUS3                       (1 << 3)
#define CMU_MEMSC_STATUS4                       (1 << 4)
#define CMU_MEMSC_STATUS5                       (1 << 5)
#define CMU_MEMSC_STATUS6                       (1 << 6)
#define CMU_MEMSC_STATUS7                       (1 << 7)
#define CMU_MEMSC_STATUS8                       (1 << 8)
#define CMU_MEMSC_STATUS9                       (1 << 9)
#define CMU_MEMSC_STATUS10                      (1 << 10)
#define CMU_MEMSC_STATUS11                      (1 << 11)
#define CMU_MEMSC_STATUS12                      (1 << 12)
#define CMU_MEMSC_STATUS13                      (1 << 13)
#define CMU_MEMSC_STATUS14                      (1 << 14)
#define CMU_MEMSC_STATUS15                      (1 << 15)
#define CMU_MEMSC_STATUS16                      (1 << 16)
#define CMU_MEMSC_STATUS17                      (1 << 17)
#define CMU_MEMSC_STATUS18                      (1 << 18)
#define CMU_MEMSC_STATUS19                      (1 << 19)
#define CMU_MEMSC_STATUS20                      (1 << 20)
#define CMU_MEMSC_STATUS21                      (1 << 21)
#define CMU_MEMSC_STATUS22                      (1 << 22)
#define CMU_MEMSC_STATUS23                      (1 << 23)
#define CMU_MEMSC_STATUS24                      (1 << 24)
#define CMU_MEMSC_STATUS25                      (1 << 25)
#define CMU_MEMSC_STATUS26                      (1 << 26)
#define CMU_MEMSC_STATUS27                      (1 << 27)
#define CMU_MEMSC_STATUS28                      (1 << 28)
#define CMU_MEMSC_STATUS29                      (1 << 29)
#define CMU_MEMSC_STATUS30                      (1 << 30)
#define CMU_MEMSC_STATUS31                      (1 << 31)

// reg_0d4
#define CMU_ADMA_CH0_REQ_IDX(n)                 (((n) & 0x3F) << 0)
#define CMU_ADMA_CH0_REQ_IDX_MASK               (0x3F << 0)
#define CMU_ADMA_CH0_REQ_IDX_SHIFT              (0)
#define CMU_ADMA_CH1_REQ_IDX(n)                 (((n) & 0x3F) << 6)
#define CMU_ADMA_CH1_REQ_IDX_MASK               (0x3F << 6)
#define CMU_ADMA_CH1_REQ_IDX_SHIFT              (6)
#define CMU_ADMA_CH2_REQ_IDX(n)                 (((n) & 0x3F) << 12)
#define CMU_ADMA_CH2_REQ_IDX_MASK               (0x3F << 12)
#define CMU_ADMA_CH2_REQ_IDX_SHIFT              (12)
#define CMU_ADMA_CH3_REQ_IDX(n)                 (((n) & 0x3F) << 18)
#define CMU_ADMA_CH3_REQ_IDX_MASK               (0x3F << 18)
#define CMU_ADMA_CH3_REQ_IDX_SHIFT              (18)
#define CMU_ADMA_CH4_REQ_IDX(n)                 (((n) & 0x3F) << 24)
#define CMU_ADMA_CH4_REQ_IDX_MASK               (0x3F << 24)
#define CMU_ADMA_CH4_REQ_IDX_SHIFT              (24)

// reg_0d8
#define CMU_ADMA_CH5_REQ_IDX(n)                 (((n) & 0x3F) << 0)
#define CMU_ADMA_CH5_REQ_IDX_MASK               (0x3F << 0)
#define CMU_ADMA_CH5_REQ_IDX_SHIFT              (0)
#define CMU_ADMA_CH6_REQ_IDX(n)                 (((n) & 0x3F) << 6)
#define CMU_ADMA_CH6_REQ_IDX_MASK               (0x3F << 6)
#define CMU_ADMA_CH6_REQ_IDX_SHIFT              (6)
#define CMU_ADMA_CH7_REQ_IDX(n)                 (((n) & 0x3F) << 12)
#define CMU_ADMA_CH7_REQ_IDX_MASK               (0x3F << 12)
#define CMU_ADMA_CH7_REQ_IDX_SHIFT              (12)
#define CMU_ADMA_CH8_REQ_IDX(n)                 (((n) & 0x3F) << 18)
#define CMU_ADMA_CH8_REQ_IDX_MASK               (0x3F << 18)
#define CMU_ADMA_CH8_REQ_IDX_SHIFT              (18)
#define CMU_ADMA_CH9_REQ_IDX(n)                 (((n) & 0x3F) << 24)
#define CMU_ADMA_CH9_REQ_IDX_MASK               (0x3F << 24)
#define CMU_ADMA_CH9_REQ_IDX_SHIFT              (24)

// reg_0dc
#define CMU_ADMA_CH10_REQ_IDX(n)                (((n) & 0x3F) << 0)
#define CMU_ADMA_CH10_REQ_IDX_MASK              (0x3F << 0)
#define CMU_ADMA_CH10_REQ_IDX_SHIFT             (0)
#define CMU_ADMA_CH11_REQ_IDX(n)                (((n) & 0x3F) << 6)
#define CMU_ADMA_CH11_REQ_IDX_MASK              (0x3F << 6)
#define CMU_ADMA_CH11_REQ_IDX_SHIFT             (6)
#define CMU_ADMA_CH12_REQ_IDX(n)                (((n) & 0x3F) << 12)
#define CMU_ADMA_CH12_REQ_IDX_MASK              (0x3F << 12)
#define CMU_ADMA_CH12_REQ_IDX_SHIFT             (12)
#define CMU_ADMA_CH13_REQ_IDX(n)                (((n) & 0x3F) << 18)
#define CMU_ADMA_CH13_REQ_IDX_MASK              (0x3F << 18)
#define CMU_ADMA_CH13_REQ_IDX_SHIFT             (18)
#define CMU_ADMA_CH14_REQ_IDX(n)                (((n) & 0x3F) << 24)
#define CMU_ADMA_CH14_REQ_IDX_MASK              (0x3F << 24)
#define CMU_ADMA_CH14_REQ_IDX_SHIFT             (24)

// reg_0e0
#define CMU_GDMA_CH0_REQ_IDX(n)                 (((n) & 0x3F) << 0)
#define CMU_GDMA_CH0_REQ_IDX_MASK               (0x3F << 0)
#define CMU_GDMA_CH0_REQ_IDX_SHIFT              (0)
#define CMU_GDMA_CH1_REQ_IDX(n)                 (((n) & 0x3F) << 6)
#define CMU_GDMA_CH1_REQ_IDX_MASK               (0x3F << 6)
#define CMU_GDMA_CH1_REQ_IDX_SHIFT              (6)
#define CMU_GDMA_CH2_REQ_IDX(n)                 (((n) & 0x3F) << 12)
#define CMU_GDMA_CH2_REQ_IDX_MASK               (0x3F << 12)
#define CMU_GDMA_CH2_REQ_IDX_SHIFT              (12)
#define CMU_GDMA_CH3_REQ_IDX(n)                 (((n) & 0x3F) << 18)
#define CMU_GDMA_CH3_REQ_IDX_MASK               (0x3F << 18)
#define CMU_GDMA_CH3_REQ_IDX_SHIFT              (18)
#define CMU_GDMA_CH4_REQ_IDX(n)                 (((n) & 0x3F) << 24)
#define CMU_GDMA_CH4_REQ_IDX_MASK               (0x3F << 24)
#define CMU_GDMA_CH4_REQ_IDX_SHIFT              (24)

// reg_0e4
#define CMU_GDMA_CH5_REQ_IDX(n)                 (((n) & 0x3F) << 0)
#define CMU_GDMA_CH5_REQ_IDX_MASK               (0x3F << 0)
#define CMU_GDMA_CH5_REQ_IDX_SHIFT              (0)
#define CMU_GDMA_CH6_REQ_IDX(n)                 (((n) & 0x3F) << 6)
#define CMU_GDMA_CH6_REQ_IDX_MASK               (0x3F << 6)
#define CMU_GDMA_CH6_REQ_IDX_SHIFT              (6)
#define CMU_GDMA_CH7_REQ_IDX(n)                 (((n) & 0x3F) << 12)
#define CMU_GDMA_CH7_REQ_IDX_MASK               (0x3F << 12)
#define CMU_GDMA_CH7_REQ_IDX_SHIFT              (12)
#define CMU_GDMA_CH8_REQ_IDX(n)                 (((n) & 0x3F) << 18)
#define CMU_GDMA_CH8_REQ_IDX_MASK               (0x3F << 18)
#define CMU_GDMA_CH8_REQ_IDX_SHIFT              (18)
#define CMU_GDMA_CH9_REQ_IDX(n)                 (((n) & 0x3F) << 24)
#define CMU_GDMA_CH9_REQ_IDX_MASK               (0x3F << 24)
#define CMU_GDMA_CH9_REQ_IDX_SHIFT              (24)

// reg_0e8
#define CMU_GDMA_CH10_REQ_IDX(n)                (((n) & 0x3F) << 0)
#define CMU_GDMA_CH10_REQ_IDX_MASK              (0x3F << 0)
#define CMU_GDMA_CH10_REQ_IDX_SHIFT             (0)
#define CMU_GDMA_CH11_REQ_IDX(n)                (((n) & 0x3F) << 6)
#define CMU_GDMA_CH11_REQ_IDX_MASK              (0x3F << 6)
#define CMU_GDMA_CH11_REQ_IDX_SHIFT             (6)
#define CMU_GDMA_CH12_REQ_IDX(n)                (((n) & 0x3F) << 12)
#define CMU_GDMA_CH12_REQ_IDX_MASK              (0x3F << 12)
#define CMU_GDMA_CH12_REQ_IDX_SHIFT             (12)
#define CMU_GDMA_CH13_REQ_IDX(n)                (((n) & 0x3F) << 18)
#define CMU_GDMA_CH13_REQ_IDX_MASK              (0x3F << 18)
#define CMU_GDMA_CH13_REQ_IDX_SHIFT             (18)
#define CMU_GDMA_CH14_REQ_IDX(n)                (((n) & 0x3F) << 24)
#define CMU_GDMA_CH14_REQ_IDX_MASK              (0x3F << 24)
#define CMU_GDMA_CH14_REQ_IDX_SHIFT             (24)

// reg_0ec
#define CMU_GDMA_CH15_REQ_IDX(n)                (((n) & 0x3F) << 0)
#define CMU_GDMA_CH15_REQ_IDX_MASK              (0x3F << 0)
#define CMU_GDMA_CH15_REQ_IDX_SHIFT             (0)

// reg_0f0
#define CMU_RESERVED(n)                         (((n) & 0xFFFFFFFF) << 0)
#define CMU_RESERVED_MASK                       (0xFFFFFFFF << 0)
#define CMU_RESERVED_SHIFT                      (0)

// reg_0f4
#define CMU_DEBUG(n)                            (((n) & 0xFFFFFFFF) << 0)
#define CMU_DEBUG_MASK                          (0xFFFFFFFF << 0)
#define CMU_DEBUG_SHIFT                         (0)

// reg_0f8
#define CMU_SEC_ROM_STR_ADDR(n)                 (((n) & 0xFFFF) << 0)
#define CMU_SEC_ROM_STR_ADDR_MASK               (0xFFFF << 0)
#define CMU_SEC_ROM_STR_ADDR_SHIFT              (0)
#define CMU_SEC_ROM_END_ADDR(n)                 (((n) & 0xFFFF) << 16)
#define CMU_SEC_ROM_END_ADDR_MASK               (0xFFFF << 16)
#define CMU_SEC_ROM_END_ADDR_SHIFT              (16)

// reg_0fc
#define CMU_CPU_ACC_RAM_EN                      (1 << 0)
#define CMU_BCM_ACC_RAM_EN                      (1 << 1)
#define CMU_NONSEC_ACC_RAM_EN                   (1 << 2)
#define CMU_JTAG_ACC_RAM_EN                     (1 << 3)
#define CMU_JTAG_ACC_SECROM_EN                  (1 << 4)
#define CMU_DCODE_ACC_SECROM_EN                 (1 << 5)
#define CMU_SRAM0_ACC_REFUSE                    (1 << 6)

// reg_100
#define CMU_VTOR_CORE1(n)                       (((n) & 0x1FFFFFF) << 7)
#define CMU_VTOR_CORE1_MASK                     (0x1FFFFFF << 7)
#define CMU_VTOR_CORE1_SHIFT                    (7)

// reg_104
#define CMU_MANUAL_XCLK_ENABLE(n)               (((n) & 0xFFFFFFFF) << 0)
#define CMU_MANUAL_XCLK_ENABLE_MASK             (0xFFFFFFFF << 0)
#define CMU_MANUAL_XCLK_ENABLE_SHIFT            (0)

// reg_108
#define CMU_MANUAL_XCLK_DISABLE(n)              (((n) & 0xFFFFFFFF) << 0)
#define CMU_MANUAL_XCLK_DISABLE_MASK            (0xFFFFFFFF << 0)
#define CMU_MANUAL_XCLK_DISABLE_SHIFT           (0)

// reg_10c
#define CMU_MODE_XCLK(n)                        (((n) & 0xFFFFFFFF) << 0)
#define CMU_MODE_XCLK_MASK                      (0xFFFFFFFF << 0)
#define CMU_MODE_XCLK_SHIFT                     (0)

// reg_110
#define CMU_XRESETN_PULSE(n)                    (((n) & 0xFFFFFFFF) << 0)
#define CMU_XRESETN_PULSE_MASK                  (0xFFFFFFFF << 0)
#define CMU_XRESETN_PULSE_SHIFT                 (0)

// reg_114
#define CMU_XRESETN_SET(n)                      (((n) & 0xFFFFFFFF) << 0)
#define CMU_XRESETN_SET_MASK                    (0xFFFFFFFF << 0)
#define CMU_XRESETN_SET_SHIFT                   (0)

// reg_118
#define CMU_XRESETN_CLR(n)                      (((n) & 0xFFFFFFFF) << 0)
#define CMU_XRESETN_CLR_MASK                    (0xFFFFFFFF << 0)
#define CMU_XRESETN_CLR_SHIFT                   (0)

// reg_11c
#define CMU_CFG_DIV_A7(n)                       (((n) & 0x3) << 0)
#define CMU_CFG_DIV_A7_MASK                     (0x3 << 0)
#define CMU_CFG_DIV_A7_SHIFT                    (0)
#define CMU_CFG_DIV_XCLK(n)                     (((n) & 0x3) << 2)
#define CMU_CFG_DIV_XCLK_MASK                   (0x3 << 2)
#define CMU_CFG_DIV_XCLK_SHIFT                  (2)
#define CMU_CFG_DIV_APCLK(n)                    (((n) & 0x3) << 4)
#define CMU_CFG_DIV_APCLK_MASK                  (0x3 << 4)
#define CMU_CFG_DIV_APCLK_SHIFT                 (4)
#define CMU_SEL_32K_TIMER_AP(n)                 (((n) & 0x3) << 6)
#define CMU_SEL_32K_TIMER_AP_MASK               (0x3 << 6)
#define CMU_SEL_32K_TIMER_AP_SHIFT              (6)
#define CMU_SEL_32K_WDT_AP                      (1 << 8)
#define CMU_SEL_TIMER_FAST_AP(n)                (((n) & 0x3) << 9)
#define CMU_SEL_TIMER_FAST_AP_MASK              (0x3 << 9)
#define CMU_SEL_TIMER_FAST_AP_SHIFT             (9)

// reg_120
#define CMU_WAKEUP_IRQ_MASK2(n)                 (((n) & 0xFF) << 0)
#define CMU_WAKEUP_IRQ_MASK2_MASK               (0xFF << 0)
#define CMU_WAKEUP_IRQ_MASK2_SHIFT              (0)

// reg_124
#define CMU_MCU2BT_INTISR_MASK2(n)              (((n) & 0xFF) << 0)
#define CMU_MCU2BT_INTISR_MASK2_MASK            (0xFF << 0)
#define CMU_MCU2BT_INTISR_MASK2_SHIFT           (0)

// reg_128
#define CMU_MCU2WF_INTISR_MASK0(n)              (((n) & 0xFFFFFFFF) << 0)
#define CMU_MCU2WF_INTISR_MASK0_MASK            (0xFFFFFFFF << 0)
#define CMU_MCU2WF_INTISR_MASK0_SHIFT           (0)

// reg_12c
#define CMU_MCU2WF_INTISR_MASK1(n)              (((n) & 0xFFFFFFFF) << 0)
#define CMU_MCU2WF_INTISR_MASK1_MASK            (0xFFFFFFFF << 0)
#define CMU_MCU2WF_INTISR_MASK1_SHIFT           (0)

// reg_130
#define CMU_MCU2WF_INTISR_MASK2(n)              (((n) & 0xFF) << 0)
#define CMU_MCU2WF_INTISR_MASK2_MASK            (0xFF << 0)
#define CMU_MCU2WF_INTISR_MASK2_SHIFT           (0)

// reg_134
#define CMU_M33TOA7_INTISR_MASK0(n)             (((n) & 0xFFFFFFFF) << 0)
#define CMU_M33TOA7_INTISR_MASK0_MASK           (0xFFFFFFFF << 0)
#define CMU_M33TOA7_INTISR_MASK0_SHIFT          (0)

// reg_138
#define CMU_M33TOA7_INTISR_MASK1(n)             (((n) & 0xFFFFFFFF) << 0)
#define CMU_M33TOA7_INTISR_MASK1_MASK           (0xFFFFFFFF << 0)
#define CMU_M33TOA7_INTISR_MASK1_SHIFT          (0)

// reg_13c
#define CMU_M33TOA7_INTISR_MASK2(n)             (((n) & 0xFF) << 0)
#define CMU_M33TOA7_INTISR_MASK2_MASK           (0xFF << 0)
#define CMU_M33TOA7_INTISR_MASK2_SHIFT          (0)

// reg_140
#define CMU_A7TOM33_IRQS_MASK(n)                (((n) & 0xFF) << 0)
#define CMU_A7TOM33_IRQS_MASK_MASK              (0xFF << 0)
#define CMU_A7TOM33_IRQS_MASK_SHIFT             (0)
#define CMU_CA7_L1RSTDISABLE(n)                 (((n) & 0x3) << 8)
#define CMU_CA7_L1RSTDISABLE_MASK               (0x3 << 8)
#define CMU_CA7_L1RSTDISABLE_SHIFT              (8)
#define CMU_CA7_L2RSTDISABLE                    (1 << 10)
#define CMU_CA7_DBGEN(n)                        (((n) & 0x3) << 11)
#define CMU_CA7_DBGEN_MASK                      (0x3 << 11)
#define CMU_CA7_DBGEN_SHIFT                     (11)
#define CMU_CA7_SPIDEN(n)                       (((n) & 0x3) << 13)
#define CMU_CA7_SPIDEN_MASK                     (0x3 << 13)
#define CMU_CA7_SPIDEN_SHIFT                    (13)
#define CMU_CA7_NIDEN(n)                        (((n) & 0x3) << 15)
#define CMU_CA7_NIDEN_MASK                      (0x3 << 15)
#define CMU_CA7_NIDEN_SHIFT                     (15)
#define CMU_CA7_SPNIDEN(n)                      (((n) & 0x3) << 17)
#define CMU_CA7_SPNIDEN_MASK                    (0x3 << 17)
#define CMU_CA7_SPNIDEN_SHIFT                   (17)
#define CMU_CA7_DBGROMADDRV                     (1 << 19)
#define CMU_CA7_DBGSELFADDRV                    (1 << 20)
#define CMU_CA7_DBGRESTART(n)                   (((n) & 0x3) << 21)
#define CMU_CA7_DBGRESTART_MASK                 (0x3 << 21)
#define CMU_CA7_DBGRESTART_SHIFT                (21)
#define CMU_CA7_CP15SDISABLE(n)                 (((n) & 0x3) << 23)
#define CMU_CA7_CP15SDISABLE_MASK               (0x3 << 23)
#define CMU_CA7_CP15SDISABLE_SHIFT              (23)
#define CMU_CA7_TEINIT(n)                       (((n) & 0x3) << 25)
#define CMU_CA7_TEINIT_MASK                     (0x3 << 25)
#define CMU_CA7_TEINIT_SHIFT                    (25)
#define CMU_CA7_CFGSDISABLE                     (1 << 27)
#define CMU_M33TOA7_INTR_MASK_AP                (1 << 28)
#define CMU_COUNTER_EN_AP                       (1 << 29)

// reg_144
#define CMU_CA7_IDATA_DLY_S(n)                  (((n) & 0x3) << 0)
#define CMU_CA7_IDATA_DLY_S_MASK                (0x3 << 0)
#define CMU_CA7_IDATA_DLY_S_SHIFT               (0)
#define CMU_CA7_ITAG_DLY_S(n)                   (((n) & 0x3) << 2)
#define CMU_CA7_ITAG_DLY_S_MASK                 (0x3 << 2)
#define CMU_CA7_ITAG_DLY_S_SHIFT                (2)
#define CMU_CA7_DDATA_DLY_S(n)                  (((n) & 0x3) << 4)
#define CMU_CA7_DDATA_DLY_S_MASK                (0x3 << 4)
#define CMU_CA7_DDATA_DLY_S_SHIFT               (4)
#define CMU_CA7_DTAG_DLY_S(n)                   (((n) & 0x3) << 6)
#define CMU_CA7_DTAG_DLY_S_MASK                 (0x3 << 6)
#define CMU_CA7_DTAG_DLY_S_SHIFT                (6)
#define CMU_CA7_DDIRTY_DLY_S(n)                 (((n) & 0x3) << 8)
#define CMU_CA7_DDIRTY_DLY_S_MASK               (0x3 << 8)
#define CMU_CA7_DDIRTY_DLY_S_SHIFT              (8)
#define CMU_CA7_TLB_DLY_S(n)                    (((n) & 0x3) << 10)
#define CMU_CA7_TLB_DLY_S_MASK                  (0x3 << 10)
#define CMU_CA7_TLB_DLY_S_SHIFT                 (10)
#define CMU_CA7_L1D_DLY_S(n)                    (((n) & 0x3) << 12)
#define CMU_CA7_L1D_DLY_S_MASK                  (0x3 << 12)
#define CMU_CA7_L1D_DLY_S_SHIFT                 (12)
#define CMU_CA7_L2TAG_DLY_S(n)                  (((n) & 0x3) << 14)
#define CMU_CA7_L2TAG_DLY_S_MASK                (0x3 << 14)
#define CMU_CA7_L2TAG_DLY_S_SHIFT               (14)
#define CMU_CA7_EMA_HD_S(n)                     (((n) & 0x7) << 16)
#define CMU_CA7_EMA_HD_S_MASK                   (0x7 << 16)
#define CMU_CA7_EMA_HD_S_SHIFT                  (16)
#define CMU_CA7_EMAW_HD_S(n)                    (((n) & 0x3) << 19)
#define CMU_CA7_EMAW_HD_S_MASK                  (0x3 << 19)
#define CMU_CA7_EMAW_HD_S_SHIFT                 (19)

// reg_148
#define CMU_AWQOS_CA7(n)                        (((n) & 0xF) << 0)
#define CMU_AWQOS_CA7_MASK                      (0xF << 0)
#define CMU_AWQOS_CA7_SHIFT                     (0)
#define CMU_ARQOS_CA7(n)                        (((n) & 0xF) << 4)
#define CMU_ARQOS_CA7_MASK                      (0xF << 4)
#define CMU_ARQOS_CA7_SHIFT                     (4)
#define CMU_AWQOS_DMA_M(n)                      (((n) & 0xF) << 8)
#define CMU_AWQOS_DMA_M_MASK                    (0xF << 8)
#define CMU_AWQOS_DMA_M_SHIFT                   (8)
#define CMU_ARQOS_DMA_M(n)                      (((n) & 0xF) << 12)
#define CMU_ARQOS_DMA_M_MASK                    (0xF << 12)
#define CMU_ARQOS_DMA_M_SHIFT                   (12)

// reg_14c
#define CMU_XDMA_CHL_SECURE(n)                  (((n) & 0xFFFF) << 0)
#define CMU_XDMA_CHL_SECURE_MASK                (0xFFFF << 0)
#define CMU_XDMA_CHL_SECURE_SHIFT               (0)
#define CMU_XDMA_GLOBAL_SECURE_ENABLE           (1 << 16)
#define CMU_XDMA_ID_TYPE                        (1 << 17)
#define CMU_XDMA_SRAM_CLK_ON                    (1 << 18)

// reg_150
#define CMU_MANUAL_APCLK_ENABLE(n)              (((n) & 0xFFFFFFFF) << 0)
#define CMU_MANUAL_APCLK_ENABLE_MASK            (0xFFFFFFFF << 0)
#define CMU_MANUAL_APCLK_ENABLE_SHIFT           (0)

// reg_154
#define CMU_MANUAL_APCLK_DISABLE(n)             (((n) & 0xFFFFFFFF) << 0)
#define CMU_MANUAL_APCLK_DISABLE_MASK           (0xFFFFFFFF << 0)
#define CMU_MANUAL_APCLK_DISABLE_SHIFT          (0)

// reg_158
#define CMU_MODE_APCLK(n)                       (((n) & 0xFFFFFFFF) << 0)
#define CMU_MODE_APCLK_MASK                     (0xFFFFFFFF << 0)
#define CMU_MODE_APCLK_SHIFT                    (0)

// reg_15c
#define CMU_APRESETN_PULSE(n)                   (((n) & 0xFFFFFFFF) << 0)
#define CMU_APRESETN_PULSE_MASK                 (0xFFFFFFFF << 0)
#define CMU_APRESETN_PULSE_SHIFT                (0)

// reg_160
#define CMU_APRESETN_SET(n)                     (((n) & 0xFFFFFFFF) << 0)
#define CMU_APRESETN_SET_MASK                   (0xFFFFFFFF << 0)
#define CMU_APRESETN_SET_SHIFT                  (0)

// reg_164
#define CMU_APRESETN_CLR(n)                     (((n) & 0xFFFFFFFF) << 0)
#define CMU_APRESETN_CLR_MASK                   (0xFFFFFFFF << 0)
#define CMU_APRESETN_CLR_SHIFT                  (0)

// reg_168
#define CMU_CFG_DIV_TIMER00_AP(n)               (((n) & 0xFFFF) << 0)
#define CMU_CFG_DIV_TIMER00_AP_MASK             (0xFFFF << 0)
#define CMU_CFG_DIV_TIMER00_AP_SHIFT            (0)
#define CMU_CFG_DIV_TIMER01_AP(n)               (((n) & 0xFFFF) << 16)
#define CMU_CFG_DIV_TIMER01_AP_MASK             (0xFFFF << 16)
#define CMU_CFG_DIV_TIMER01_AP_SHIFT            (16)

// reg_16c
#define CMU_CFG_DIV_TIMER10_AP(n)               (((n) & 0xFFFF) << 0)
#define CMU_CFG_DIV_TIMER10_AP_MASK             (0xFFFF << 0)
#define CMU_CFG_DIV_TIMER10_AP_SHIFT            (0)
#define CMU_CFG_DIV_TIMER11_AP(n)               (((n) & 0xFFFF) << 16)
#define CMU_CFG_DIV_TIMER11_AP_MASK             (0xFFFF << 16)
#define CMU_CFG_DIV_TIMER11_AP_SHIFT            (16)

// reg_180
#define CMU_MEMSC4                              (1 << 0)

// reg_184
#define CMU_MEMSC5                              (1 << 0)

// reg_188
#define CMU_MEMSC6                              (1 << 0)

// reg_18c
#define CMU_MEMSC7                              (1 << 0)

// reg_190
#define CMU_MEMSC8                              (1 << 0)

// reg_194
#define CMU_MEMSC9                              (1 << 0)

// reg_198
#define CMU_MEMSC10                             (1 << 0)

// reg_19c
#define CMU_MEMSC11                             (1 << 0)

// reg_1a0
#define CMU_MEMSC12                             (1 << 0)

// reg_1a4
#define CMU_MEMSC13                             (1 << 0)

// reg_1a8
#define CMU_MEMSC14                             (1 << 0)

// reg_1ac
#define CMU_MEMSC15                             (1 << 0)

// reg_1b0
#define CMU_MEMSC16                             (1 << 0)

// reg_1b4
#define CMU_MEMSC17                             (1 << 0)

// reg_1b8
#define CMU_MEMSC18                             (1 << 0)

// reg_1bc
#define CMU_MEMSC19                             (1 << 0)

// reg_1c0
#define CMU_MEMSC20                             (1 << 0)

// reg_1c4
#define CMU_MEMSC21                             (1 << 0)

// reg_1c8
#define CMU_MEMSC22                             (1 << 0)

// reg_1cc
#define CMU_MEMSC23                             (1 << 0)

// reg_1d0
#define CMU_MEMSC24                             (1 << 0)

// reg_1d4
#define CMU_MEMSC25                             (1 << 0)

// reg_1d8
#define CMU_MEMSC26                             (1 << 0)

// reg_1dc
#define CMU_MEMSC27                             (1 << 0)

// reg_1e0
#define CMU_MEMSC28                             (1 << 0)

// reg_1e4
#define CMU_MEMSC29                             (1 << 0)

// reg_1e8
#define CMU_MEMSC30                             (1 << 0)

// reg_1ec
#define CMU_MEMSC31                             (1 << 0)

// MCU System AHB Clocks:
#define SYS_HCLK_CORE0                          (1 << 0)
#define SYS_HRST_CORE0                          (1 << 0)
#define SYS_HCLK_CACHE0                         (1 << 1)
#define SYS_HRST_CACHE0                         (1 << 1)
#define SYS_HCLK_CORE1                          (1 << 2)
#define SYS_HRST_CORE1                          (1 << 2)
#define SYS_HCLK_CACHE1                         (1 << 3)
#define SYS_HRST_CACHE1                         (1 << 3)
#define SYS_HCLK_ADMA                           (1 << 4)
#define SYS_HRST_ADMA                           (1 << 4)
#define SYS_HCLK_GDMA                           (1 << 5)
#define SYS_HRST_GDMA                           (1 << 5)
#define SYS_HCLK_BCM                            (1 << 6)
#define SYS_HRST_BCM                            (1 << 6)
#define SYS_HCLK_USBC                           (1 << 7)
#define SYS_HRST_USBC                           (1 << 7)
#define SYS_HCLK_USBH                           (1 << 8)
#define SYS_HRST_USBH                           (1 << 8)
#define SYS_HCLK_I2C_SLAVE                      (1 << 9)
#define SYS_HRST_I2C_SLAVE                      (1 << 9)
#define SYS_HCLK_AX2H_A7                        (1 << 10)
#define SYS_HRST_AX2H_A7                        (1 << 10)
#define SYS_HCLK_AH2H_WF                        (1 << 11)
#define SYS_HRST_AH2H_WF                        (1 << 11)
#define SYS_HCLK_AH2H_BT                        (1 << 12)
#define SYS_HRST_AH2H_BT                        (1 << 12)
#define SYS_HCLK_CODEC                          (1 << 13)
#define SYS_HRST_CODEC                          (1 << 13)
#define SYS_HCLK_AHB1                           (1 << 14)
#define SYS_HRST_AHB1                           (1 << 14)
#define SYS_HCLK_AHB0                           (1 << 15)
#define SYS_HRST_AHB0                           (1 << 15)
#define SYS_HCLK_PSRAM1G                        (1 << 16)
#define SYS_HRST_PSRAM1G                        (1 << 16)
#define SYS_HCLK_PSRAM200                       (1 << 17)
#define SYS_HRST_PSRAM200                       (1 << 17)
#define SYS_HCLK_FLASH                          (1 << 18)
#define SYS_HRST_FLASH                          (1 << 18)
#define SYS_HCLK_RAM9                           (1 << 19)
#define SYS_HRST_RAM9                           (1 << 19)
#define SYS_HCLK_RAM8                           (1 << 20)
#define SYS_HRST_RAM8                           (1 << 20)
#define SYS_HCLK_RAM7                           (1 << 21)
#define SYS_HRST_RAM7                           (1 << 21)
#define SYS_HCLK_RAM3_6                         (1 << 22)
#define SYS_HRST_RAM3_6                         (1 << 22)
#define SYS_HCLK_RAM1_2                         (1 << 23)
#define SYS_HRST_RAM1_2                         (1 << 23)
#define SYS_HCLK_RAM0                           (1 << 24)
#define SYS_HRST_RAM0                           (1 << 24)
#define SYS_HCLK_ROM0                           (1 << 25)
#define SYS_HRST_ROM0                           (1 << 25)
#define SYS_HCLK_BT_DUMP                        (1 << 26)
#define SYS_HRST_BT_DUMP                        (1 << 26)
#define SYS_HCLK_WF_DUMP                        (1 << 27)
#define SYS_HRST_WF_DUMP                        (1 << 27)
#define SYS_HCLK_SDMMC                          (1 << 28)
#define SYS_HRST_SDMMC                          (1 << 28)
#define SYS_HCLK_CHECKSUM                       (1 << 29)
#define SYS_HRST_CHECKSUM                       (1 << 29)
#define SYS_HCLK_CRC                            (1 << 30)
#define SYS_HRST_CRC                            (1 << 30)

// MCU System APB Clocks:
#define SYS_PCLK_CMU                            (1 << 0)
#define SYS_PRST_CMU                            (1 << 0)
#define SYS_PCLK_WDT                            (1 << 1)
#define SYS_PRST_WDT                            (1 << 1)
#define SYS_PCLK_TIMER0                         (1 << 2)
#define SYS_PRST_TIMER0                         (1 << 2)
#define SYS_PCLK_TIMER1                         (1 << 3)
#define SYS_PRST_TIMER1                         (1 << 3)
#define SYS_PCLK_TIMER2                         (1 << 4)
#define SYS_PRST_TIMER2                         (1 << 4)
#define SYS_PCLK_I2C0                           (1 << 5)
#define SYS_PRST_I2C0                           (1 << 5)
#define SYS_PCLK_I2C1                           (1 << 6)
#define SYS_PRST_I2C1                           (1 << 6)
#define SYS_PCLK_SPI                            (1 << 7)
#define SYS_PRST_SPI                            (1 << 7)
#define SYS_PCLK_SLCD                           (1 << 8)
#define SYS_PRST_SLCD                           (1 << 8)
#define SYS_PCLK_SPI_ITN                        (1 << 9)
#define SYS_PRST_SPI_ITN                        (1 << 9)
#define SYS_PCLK_SPI_PHY                        (1 << 10)
#define SYS_PRST_SPI_PHY                        (1 << 10)
#define SYS_PCLK_UART0                          (1 << 11)
#define SYS_PRST_UART0                          (1 << 11)
#define SYS_PCLK_UART1                          (1 << 12)
#define SYS_PRST_UART1                          (1 << 12)
#define SYS_PCLK_UART2                          (1 << 13)
#define SYS_PRST_UART2                          (1 << 13)
#define SYS_PCLK_PCM                            (1 << 14)
#define SYS_PRST_PCM                            (1 << 14)
#define SYS_PCLK_I2S0                           (1 << 15)
#define SYS_PRST_I2S0                           (1 << 15)
#define SYS_PCLK_SPDIF0                         (1 << 16)
#define SYS_PRST_SPDIF0                         (1 << 16)
#define SYS_PCLK_TQWF                           (1 << 17)
#define SYS_PRST_TQWF                           (1 << 17)
#define SYS_PCLK_TQA7                           (1 << 18)
#define SYS_PRST_TQA7                           (1 << 18)
#define SYS_PCLK_TRNG                           (1 << 19)
#define SYS_PRST_TRNG                           (1 << 19)
#define SYS_PCLK_BCM                            (1 << 20)
#define SYS_PRST_BCM                            (1 << 20)
#define SYS_PCLK_TZC                            (1 << 21)
#define SYS_PRST_TZC                            (1 << 21)

// MCU System Other Clocks:
#define SYS_OCLK_SLEEP                          (1 << 0)
#define SYS_ORST_SLEEP                          (1 << 0)
#define SYS_OCLK_USB                            (1 << 1)
#define SYS_ORST_USB                            (1 << 1)
#define SYS_OCLK_USB32K                         (1 << 2)
#define SYS_ORST_USB32K                         (1 << 2)
#define SYS_OCLK_PSRAM1G                        (1 << 3)
#define SYS_ORST_PSRAM1G                        (1 << 3)
#define SYS_OCLK_PSRAM200                       (1 << 4)
#define SYS_ORST_PSRAM200                       (1 << 4)
#define SYS_OCLK_FLASH                          (1 << 5)
#define SYS_ORST_FLASH                          (1 << 5)
#define SYS_OCLK_SDMMC                          (1 << 6)
#define SYS_ORST_SDMMC                          (1 << 6)
#define SYS_OCLK_WDT                            (1 << 7)
#define SYS_ORST_WDT                            (1 << 7)
#define SYS_OCLK_TIMER0                         (1 << 8)
#define SYS_ORST_TIMER0                         (1 << 8)
#define SYS_OCLK_TIMER1                         (1 << 9)
#define SYS_ORST_TIMER1                         (1 << 9)
#define SYS_OCLK_TIMER2                         (1 << 10)
#define SYS_ORST_TIMER2                         (1 << 10)
#define SYS_OCLK_I2C0                           (1 << 11)
#define SYS_ORST_I2C0                           (1 << 11)
#define SYS_OCLK_I2C1                           (1 << 12)
#define SYS_ORST_I2C1                           (1 << 12)
#define SYS_OCLK_SPI                            (1 << 13)
#define SYS_ORST_SPI                            (1 << 13)
#define SYS_OCLK_SLCD                           (1 << 14)
#define SYS_ORST_SLCD                           (1 << 14)
#define SYS_OCLK_SPI_ITN                        (1 << 15)
#define SYS_ORST_SPI_ITN                        (1 << 15)
#define SYS_OCLK_SPI_PHY                        (1 << 16)
#define SYS_ORST_SPI_PHY                        (1 << 16)
#define SYS_OCLK_UART0                          (1 << 17)
#define SYS_ORST_UART0                          (1 << 17)
#define SYS_OCLK_UART1                          (1 << 18)
#define SYS_ORST_UART1                          (1 << 18)
#define SYS_OCLK_UART2                          (1 << 19)
#define SYS_ORST_UART2                          (1 << 19)
#define SYS_OCLK_PCM                            (1 << 20)
#define SYS_ORST_PCM                            (1 << 20)
#define SYS_OCLK_I2S0                           (1 << 21)
#define SYS_ORST_I2S0                           (1 << 21)
#define SYS_OCLK_SPDIF0                         (1 << 22)
#define SYS_ORST_SPDIF0                         (1 << 22)
#define SYS_OCLK_I2S1                           (1 << 23)
#define SYS_ORST_I2S1                           (1 << 23)
#define SYS_OCLK_A7                             (1 << 24)
#define SYS_ORST_A7                             (1 << 24)
#define SYS_OCLK_TSF                            (1 << 25)
#define SYS_ORST_TSF                            (1 << 25)
#define SYS_OCLK_WDT_AP                         (1 << 26)
#define SYS_ORST_WDT_AP                         (1 << 26)
#define SYS_OCLK_TIMER0_AP                      (1 << 27)
#define SYS_ORST_TIMER0_AP                      (1 << 27)
#define SYS_OCLK_TIMER1_AP                      (1 << 28)
#define SYS_ORST_TIMER1_AP                      (1 << 28)

// AXI Clocks:
#define SYS_XCLK_DMA                            (1 << 0)
#define SYS_XRST_DMA                            (1 << 0)
#define SYS_XCLK_NIC                            (1 << 1)
#define SYS_XRST_NIC                            (1 << 1)
#define SYS_XCLK_IMEMLO                         (1 << 2)
#define SYS_XRST_IMEMLO                         (1 << 2)
#define SYS_XCLK_IMEMHI                         (1 << 3)
#define SYS_XRST_IMEMHI                         (1 << 3)
#define SYS_XCLK_PSRAM1G                        (1 << 4)
#define SYS_XRST_PSRAM1G                        (1 << 4)
#define SYS_XCLK_PER                            (1 << 5)
#define SYS_XRST_PER                            (1 << 5)
#define SYS_XCLK_PDBG                           (1 << 6)
#define SYS_XRST_PDBG                           (1 << 6)
#define SYS_XCLK_CORE0                          (1 << 7)
#define SYS_XRST_CORE0                          (1 << 7)
#define SYS_XCLK_CORE1                          (1 << 8)
#define SYS_XRST_CORE1                          (1 << 8)
#define SYS_XCLK_CORE2                          (1 << 9)
#define SYS_XRST_CORE2                          (1 << 9)
#define SYS_XCLK_CORE3                          (1 << 10)
#define SYS_XRST_CORE3                          (1 << 10)
#define SYS_XCLK_DBG                            (1 << 11)
#define SYS_XRST_DBG                            (1 << 11)
#define SYS_XCLK_SCU                            (1 << 12)
#define SYS_XRST_SCU                            (1 << 12)

// A7 APB Clocks:
#define SYS_APCLK_BOOTREG                       (1 << 0)
#define SYS_APRST_BOOTREG                       (1 << 0)
#define SYS_APCLK_WDT                           (1 << 1)
#define SYS_APRST_WDT                           (1 << 1)
#define SYS_APCLK_TIMER0                        (1 << 2)
#define SYS_APRST_TIMER0                        (1 << 2)
#define SYS_APCLK_TIMER1                        (1 << 3)
#define SYS_APRST_TIMER1                        (1 << 3)
#define SYS_APCLK_TQ                            (1 << 4)
#define SYS_APRST_TQ                            (1 << 4)
#define SYS_APCLK_DAP                           (1 << 5)
#define SYS_APRST_DAP                           (1 << 5)

#endif

