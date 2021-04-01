/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */
#ifndef __REG_AONCMU_BEST2001_H__
#define __REG_AONCMU_BEST2001_H__

#include "plat_types.h"

struct AONCMU_T {
    __I  uint32_t CHIP_ID;          // 0x00
    __IO uint32_t TOP_CLK_ENABLE;   // 0x04
    __IO uint32_t TOP_CLK_DISABLE;  // 0x08
    __IO uint32_t RESET_PULSE;      // 0x0C
    __IO uint32_t RESET_SET;        // 0x10
    __IO uint32_t RESET_CLR;        // 0x14
    __IO uint32_t CLK_SELECT;       // 0x18
    __IO uint32_t CLK_OUT;          // 0x1C
    __IO uint32_t WRITE_UNLOCK;     // 0x20
    __IO uint32_t MEMSC[4];         // 0x24
    __I  uint32_t MEMSC_STATUS;     // 0x34
    __IO uint32_t BOOTMODE;         // 0x38
    __IO uint32_t FLS_PSR_CLK;      // 0x3C
    __IO uint32_t MOD_CLK_ENABLE;   // 0x40
    __IO uint32_t MOD_CLK_DISABLE;  // 0x44
    __IO uint32_t MOD_CLK_MODE;     // 0x48
    __IO uint32_t CODEC_DIV;        // 0x4C
    __IO uint32_t TIMER_CLK;        // 0x50
    __IO uint32_t PWM01_CLK;        // 0x54
    __IO uint32_t PWM23_CLK;        // 0x58
    __IO uint32_t LNA_CFG;          // 0x5C
    __IO uint32_t RESERVED_060;     // 0x60
    __IO uint32_t PCM_I2S_CLK;      // 0x64
    __IO uint32_t SPDIF_CLK;        // 0x68
    __IO uint32_t SLEEP_TIMER_OSC;  // 0x6C
    __IO uint32_t SLEEP_TIMER_32K;  // 0x70
    __IO uint32_t STORE_GPIO_MASK;  // 0x74
    __IO uint32_t CODEC_IIR;        // 0x78
    __IO uint32_t BCM_WLOCK;        // 0x7C
    __IO uint32_t BCM_RLOCK;        // 0x80
    __IO uint32_t PD_STAB_TIMER;    // 0x84
    __IO uint32_t TIMER_WDT;        // 0x88
    __IO uint32_t PLL_ENABLE;       // 0x8C
    __IO uint32_t PLL_DISABLE;      // 0x90
    __IO uint32_t DSP_PLL_SELECT;   // 0x94
    __IO uint32_t DDR_CLK;          // 0x98
    __IO uint32_t RESERVED_09C[0x15]; // 0x9C
    __IO uint32_t WAKEUP_PC;        // 0xF0
    __IO uint32_t DEBUG_RES[2];     // 0xF4
    __IO uint32_t CHIP_FEATURE;     // 0xFC
};

// reg_00
#define AON_CMU_CHIP_ID(n)                      (((n) & 0xFFFF) << 0)
#define AON_CMU_CHIP_ID_MASK                    (0xFFFF << 0)
#define AON_CMU_CHIP_ID_SHIFT                   (0)
#define AON_CMU_REVISION_ID(n)                  (((n) & 0xFFFF) << 16)
#define AON_CMU_REVISION_ID_MASK                (0xFFFF << 16)
#define AON_CMU_REVISION_ID_SHIFT               (16)

// reg_04
#define AON_CMU_EN_CLK_TOP_OSC_ENABLE           (1 << 0)
#define AON_CMU_EN_CLK_TOP_OSCX2_ENABLE         (1 << 1)
#define AON_CMU_EN_CLK_TOP_OSCX4_ENABLE         (1 << 2)
#define AON_CMU_EN_CLK_TOP_PLLBB_ENABLE         (1 << 3)
#define AON_CMU_EN_CLK_TOP_PLLA7_ENABLE         (1 << 4)
#define AON_CMU_EN_CLK_TOP_PLLDDR_ENABLE        (1 << 5)
#define AON_CMU_EN_CLK_TOP_PLLUSB_ENABLE        (1 << 6)
#define AON_CMU_EN_CLK_TOP_PLLBB_PS_ENABLE      (1 << 7)
#define AON_CMU_EN_CLK_TOP_PLLBB_WF_ENABLE      (1 << 8)
#define AON_CMU_EN_CLK_TOP_PHY_PS_ENABLE        (1 << 9)
#define AON_CMU_EN_CLK_TOP_JTAG_ENABLE          (1 << 10)
#define AON_CMU_EN_CLK_USB_PLL_ENABLE           (1 << 11)
#define AON_CMU_EN_CLK_BT_ENABLE                (1 << 12)
#define AON_CMU_EN_CLK_WF_ENABLE                (1 << 13)
#define AON_CMU_EN_CLK_CODEC_ENABLE             (1 << 14)
#define AON_CMU_EN_CLK_CODECIIR_ENABLE          (1 << 15)
#define AON_CMU_EN_CLK_CODECRS0_ENABLE          (1 << 16)
#define AON_CMU_EN_CLK_CODECRS1_ENABLE          (1 << 17)
#define AON_CMU_EN_CLK_CODECHCLK_ENABLE         (1 << 18)
#define AON_CMU_EN_CLK_VAD32K_ENABLE            (1 << 19)
#define AON_CMU_EN_CLK_MCU_32K_ENABLE           (1 << 20)
#define AON_CMU_EN_CLK_MCU_OSC_ENABLE           (1 << 21)
#define AON_CMU_EN_CLK_MCU_OSCX2_ENABLE         (1 << 22)
#define AON_CMU_EN_X2_DIG_ENABLE                (1 << 23)
#define AON_CMU_EN_X4_DIG_ENABLE                (1 << 24)

// reg_08
#define AON_CMU_EN_CLK_TOP_OSC_DISABLE          (1 << 0)
#define AON_CMU_EN_CLK_TOP_OSCX2_DISABLE        (1 << 1)
#define AON_CMU_EN_CLK_TOP_OSCX4_DISABLE        (1 << 2)
#define AON_CMU_EN_CLK_TOP_PLLBB_DISABLE        (1 << 3)
#define AON_CMU_EN_CLK_TOP_PLLA7_DISABLE        (1 << 4)
#define AON_CMU_EN_CLK_TOP_PLLDDR_DISABLE       (1 << 5)
#define AON_CMU_EN_CLK_TOP_PLLUSB_DISABLE       (1 << 6)
#define AON_CMU_EN_CLK_TOP_PLLBB_PS_DISABLE     (1 << 7)
#define AON_CMU_EN_CLK_TOP_PLLBB_WF_DISABLE     (1 << 8)
#define AON_CMU_EN_CLK_TOP_PHY_PS_DISABLE       (1 << 9)
#define AON_CMU_EN_CLK_TOP_JTAG_DISABLE         (1 << 10)
#define AON_CMU_EN_CLK_USB_PLL_DISABLE          (1 << 11)
#define AON_CMU_EN_CLK_BT_DISABLE               (1 << 12)
#define AON_CMU_EN_CLK_WF_DISABLE               (1 << 13)
#define AON_CMU_EN_CLK_CODEC_DISABLE            (1 << 14)
#define AON_CMU_EN_CLK_CODECIIR_DISABLE         (1 << 15)
#define AON_CMU_EN_CLK_CODECRS0_DISABLE         (1 << 16)
#define AON_CMU_EN_CLK_CODECRS1_DISABLE         (1 << 17)
#define AON_CMU_EN_CLK_CODECHCLK_DISABLE        (1 << 18)
#define AON_CMU_EN_CLK_VAD32K_DISABLE           (1 << 19)
#define AON_CMU_EN_CLK_MCU_32K_DISABLE          (1 << 20)
#define AON_CMU_EN_CLK_MCU_OSC_DISABLE          (1 << 21)
#define AON_CMU_EN_CLK_MCU_OSCX2_DISABLE        (1 << 22)
#define AON_CMU_EN_X2_DIG_DISABLE               (1 << 23)
#define AON_CMU_EN_X4_DIG_DISABLE               (1 << 24)

#define AON_ARST_NUM                            10
#define AON_ORST_NUM                            11
#define AON_ACLK_NUM                            AON_ARST_NUM
#define AON_OCLK_NUM                            AON_ORST_NUM

// reg_0c
#define AON_CMU_ARESETN_PULSE(n)                (((n) & 0xFFFFFFFF) << 0)
#define AON_CMU_ARESETN_PULSE_MASK              (0xFFFFFFFF << 0)
#define AON_CMU_ARESETN_PULSE_SHIFT             (0)
#define AON_CMU_ORESETN_PULSE(n)                (((n) & 0xFFFFFFFF) << AON_ARST_NUM)
#define AON_CMU_ORESETN_PULSE_MASK              (0xFFFFFFFF << AON_ARST_NUM)
#define AON_CMU_ORESETN_PULSE_SHIFT             (AON_ARST_NUM)
#define AON_CMU_SOFT_RSTN_A7_PULSE              (1 << (AON_ARST_NUM+AON_ORST_NUM+32-10-AON_ARST_NUM-AON_ORST_NUM+1-1))
#define AON_CMU_SOFT_RSTN_A7CPU_PULSE           (1 << (AON_ARST_NUM+AON_ORST_NUM+32-10-AON_ARST_NUM-AON_ORST_NUM+1+1-1))
#define AON_CMU_SOFT_RSTN_MCU_PULSE             (1 << (AON_ARST_NUM+AON_ORST_NUM+32-10-AON_ARST_NUM-AON_ORST_NUM+1+1+1-1))
#define AON_CMU_SOFT_RSTN_CODEC_PULSE           (1 << (AON_ARST_NUM+AON_ORST_NUM+32-10-AON_ARST_NUM-AON_ORST_NUM+1+1+1+1-1))
#define AON_CMU_SOFT_RSTN_WF_PULSE              (1 << (AON_ARST_NUM+AON_ORST_NUM+32-10-AON_ARST_NUM-AON_ORST_NUM+1+1+1+1+1-1))
#define AON_CMU_SOFT_RSTN_BT_PULSE              (1 << (AON_ARST_NUM+AON_ORST_NUM+32-10-AON_ARST_NUM-AON_ORST_NUM+1+1+1+1+1+1-1))
#define AON_CMU_SOFT_RSTN_MCUCPU_PULSE          (1 << (AON_ARST_NUM+AON_ORST_NUM+32-10-AON_ARST_NUM-AON_ORST_NUM+1+1+1+1+1+1+1-1))
#define AON_CMU_SOFT_RSTN_WFCPU_PULSE           (1 << (AON_ARST_NUM+AON_ORST_NUM+32-10-AON_ARST_NUM-AON_ORST_NUM+1+1+1+1+1+1+1+1-1))
#define AON_CMU_SOFT_RSTN_BTCPU_PULSE           (1 << (AON_ARST_NUM+AON_ORST_NUM+32-10-AON_ARST_NUM-AON_ORST_NUM+1+1+1+1+1+1+1+1+1-1))
#define AON_CMU_GLOBAL_RESETN_PULSE             (1 << (AON_ARST_NUM+AON_ORST_NUM+32-10-AON_ARST_NUM-AON_ORST_NUM+1+1+1+1+1+1+1+1+1+1-1))

// reg_10
#define AON_CMU_ARESETN_SET(n)                  (((n) & 0xFFFFFFFF) << 0)
#define AON_CMU_ARESETN_SET_MASK                (0xFFFFFFFF << 0)
#define AON_CMU_ARESETN_SET_SHIFT               (0)
#define AON_CMU_ORESETN_SET(n)                  (((n) & 0xFFFFFFFF) << AON_ARST_NUM)
#define AON_CMU_ORESETN_SET_MASK                (0xFFFFFFFF << AON_ARST_NUM)
#define AON_CMU_ORESETN_SET_SHIFT               (AON_ARST_NUM)
#define AON_CMU_SOFT_RSTN_A7_SET                (1 << (AON_ARST_NUM+AON_ORST_NUM+32-10-AON_ARST_NUM-AON_ORST_NUM+1-1))
#define AON_CMU_SOFT_RSTN_A7CPU_SET             (1 << (AON_ARST_NUM+AON_ORST_NUM+32-10-AON_ARST_NUM-AON_ORST_NUM+1+1-1))
#define AON_CMU_SOFT_RSTN_MCU_SET               (1 << (AON_ARST_NUM+AON_ORST_NUM+32-10-AON_ARST_NUM-AON_ORST_NUM+1+1+1-1))
#define AON_CMU_SOFT_RSTN_CODEC_SET             (1 << (AON_ARST_NUM+AON_ORST_NUM+32-10-AON_ARST_NUM-AON_ORST_NUM+1+1+1+1-1))
#define AON_CMU_SOFT_RSTN_WF_SET                (1 << (AON_ARST_NUM+AON_ORST_NUM+32-10-AON_ARST_NUM-AON_ORST_NUM+1+1+1+1+1-1))
#define AON_CMU_SOFT_RSTN_BT_SET                (1 << (AON_ARST_NUM+AON_ORST_NUM+32-10-AON_ARST_NUM-AON_ORST_NUM+1+1+1+1+1+1-1))
#define AON_CMU_SOFT_RSTN_MCUCPU_SET            (1 << (AON_ARST_NUM+AON_ORST_NUM+32-10-AON_ARST_NUM-AON_ORST_NUM+1+1+1+1+1+1+1-1))
#define AON_CMU_SOFT_RSTN_WFCPU_SET             (1 << (AON_ARST_NUM+AON_ORST_NUM+32-10-AON_ARST_NUM-AON_ORST_NUM+1+1+1+1+1+1+1+1-1))
#define AON_CMU_SOFT_RSTN_BTCPU_SET             (1 << (AON_ARST_NUM+AON_ORST_NUM+32-10-AON_ARST_NUM-AON_ORST_NUM+1+1+1+1+1+1+1+1+1-1))
#define AON_CMU_GLOBAL_RESETN_SET               (1 << (AON_ARST_NUM+AON_ORST_NUM+32-10-AON_ARST_NUM-AON_ORST_NUM+1+1+1+1+1+1+1+1+1+1-1))

// reg_14
#define AON_CMU_ARESETN_CLR(n)                  (((n) & 0xFFFFFFFF) << 0)
#define AON_CMU_ARESETN_CLR_MASK                (0xFFFFFFFF << 0)
#define AON_CMU_ARESETN_CLR_SHIFT               (0)
#define AON_CMU_ORESETN_CLR(n)                  (((n) & 0xFFFFFFFF) << AON_ARST_NUM)
#define AON_CMU_ORESETN_CLR_MASK                (0xFFFFFFFF << AON_ARST_NUM)
#define AON_CMU_ORESETN_CLR_SHIFT               (AON_ARST_NUM)
#define AON_CMU_SOFT_RSTN_A7_CLR                (1 << (AON_ARST_NUM+AON_ORST_NUM+32-10-AON_ARST_NUM-AON_ORST_NUM+1-1))
#define AON_CMU_SOFT_RSTN_A7CPU_CLR             (1 << (AON_ARST_NUM+AON_ORST_NUM+32-10-AON_ARST_NUM-AON_ORST_NUM+1+1-1))
#define AON_CMU_SOFT_RSTN_MCU_CLR               (1 << (AON_ARST_NUM+AON_ORST_NUM+32-10-AON_ARST_NUM-AON_ORST_NUM+1+1+1-1))
#define AON_CMU_SOFT_RSTN_CODEC_CLR             (1 << (AON_ARST_NUM+AON_ORST_NUM+32-10-AON_ARST_NUM-AON_ORST_NUM+1+1+1+1-1))
#define AON_CMU_SOFT_RSTN_WF_CLR                (1 << (AON_ARST_NUM+AON_ORST_NUM+32-10-AON_ARST_NUM-AON_ORST_NUM+1+1+1+1+1-1))
#define AON_CMU_SOFT_RSTN_BT_CLR                (1 << (AON_ARST_NUM+AON_ORST_NUM+32-10-AON_ARST_NUM-AON_ORST_NUM+1+1+1+1+1+1-1))
#define AON_CMU_SOFT_RSTN_MCUCPU_CLR            (1 << (AON_ARST_NUM+AON_ORST_NUM+32-10-AON_ARST_NUM-AON_ORST_NUM+1+1+1+1+1+1+1-1))
#define AON_CMU_SOFT_RSTN_WFCPU_CLR             (1 << (AON_ARST_NUM+AON_ORST_NUM+32-10-AON_ARST_NUM-AON_ORST_NUM+1+1+1+1+1+1+1+1-1))
#define AON_CMU_SOFT_RSTN_BTCPU_CLR             (1 << (AON_ARST_NUM+AON_ORST_NUM+32-10-AON_ARST_NUM-AON_ORST_NUM+1+1+1+1+1+1+1+1+1-1))
#define AON_CMU_GLOBAL_RESETN_CLR               (1 << (AON_ARST_NUM+AON_ORST_NUM+32-10-AON_ARST_NUM-AON_ORST_NUM+1+1+1+1+1+1+1+1+1+1-1))

// reg_18
#define AON_CMU_BYPASS_DIV_BTSYS                (1 << 0)
#define AON_CMU_CFG_DIV_BTSYS(n)                (((n) & 0x3) << 1)
#define AON_CMU_CFG_DIV_BTSYS_MASK              (0x3 << 1)
#define AON_CMU_CFG_DIV_BTSYS_SHIFT             (1)
#define AON_CMU_SEL_BT_PLL                      (1 << 3)
#define AON_CMU_EN_BT_2M                        (1 << 4)
#define AON_CMU_SEL_USB_PLLUSB                  (1 << 5)
#define AON_CMU_CFG_DIV_USB(n)                  (((n) & 0x7) << 6)
#define AON_CMU_CFG_DIV_USB_MASK                (0x7 << 6)
#define AON_CMU_CFG_DIV_USB_SHIFT               (6)
#define AON_CMU_SEL_USB_OSCX2                   (1 << 9)
#define AON_CMU_CFG_DIV_PER(n)                  (((n) & 0x3) << 10)
#define AON_CMU_CFG_DIV_PER_MASK                (0x3 << 10)
#define AON_CMU_CFG_DIV_PER_SHIFT               (10)
#define AON_CMU_BYPASS_DIV_PER                  (1 << 12)
#define AON_CMU_RSTN_DIV_PER                    (1 << 13)
#define AON_CMU_SEL_OSCX2_DIG                   (1 << 14)
#define AON_CMU_SEL_X2_PHASE(n)                 (((n) & 0x1F) << 15)
#define AON_CMU_SEL_X2_PHASE_MASK               (0x1F << 15)
#define AON_CMU_SEL_X2_PHASE_SHIFT              (15)
#define AON_CMU_SEL_OSCX4_DIG                   (1 << 20)
#define AON_CMU_SEL_X4_PHASE(n)                 (((n) & 0x1F) << 21)
#define AON_CMU_SEL_X4_PHASE_MASK               (0x1F << 21)
#define AON_CMU_SEL_X4_PHASE_SHIFT              (21)
#define AON_CMU_PU_MCU_PLLBB_MASK               (1 << 26)
#define AON_CMU_PU_MCU_PLLUSB_MASK              (1 << 27)
#define AON_CMU_PU_MCU_PLLA7_MASK               (1 << 28)

// reg_1c
#define AON_CMU_EN_CLK_OUT                      (1 << 0)
#define AON_CMU_SEL_CLK_OUT(n)                  (((n) & 0x7) << 1)
#define AON_CMU_SEL_CLK_OUT_MASK                (0x7 << 1)
#define AON_CMU_SEL_CLK_OUT_SHIFT               (1)
#define AON_CMU_CFG_CLK_OUT(n)                  (((n) & 0x1F) << 4)
#define AON_CMU_CFG_CLK_OUT_MASK                (0x1F << 4)
#define AON_CMU_CFG_CLK_OUT_SHIFT               (4)
#define AON_CMU_CFG_DIV_DCDC(n)                 (((n) & 0xF) << 9)
#define AON_CMU_CFG_DIV_DCDC_MASK               (0xF << 9)
#define AON_CMU_CFG_DIV_DCDC_SHIFT              (9)
#define AON_CMU_BYPASS_DIV_DCDC                 (1 << 13)
#define AON_CMU_SEL_DCDC_OSC                    (1 << 14)
#define AON_CMU_SEL_DCDC_PLL                    (1 << 15)
#define AON_CMU_CLK_DCDC_DRV(n)                 (((n) & 0x3) << 16)
#define AON_CMU_CLK_DCDC_DRV_MASK               (0x3 << 16)
#define AON_CMU_CLK_DCDC_DRV_SHIFT              (16)
#define AON_CMU_SEL_DCDC_PHASE0(n)              (((n) & 0x1F) << 18)
#define AON_CMU_SEL_DCDC_PHASE0_MASK            (0x1F << 18)
#define AON_CMU_SEL_DCDC_PHASE0_SHIFT           (18)
#define AON_CMU_SEL_DCDC_PHASE1(n)              (((n) & 0x1F) << 23)
#define AON_CMU_SEL_DCDC_PHASE1_MASK            (0x1F << 23)
#define AON_CMU_SEL_DCDC_PHASE1_SHIFT           (23)
#define AON_CMU_EN_CLK_DCDC0                    (1 << 28)
#define AON_CMU_EN_CLK_DCDC1                    (1 << 29)

// reg_20
#define AON_CMU_WRITE_UNLOCK_H                  (1 << 0)
#define AON_CMU_WRITE_UNLOCK_STATUS             (1 << 1)

// reg_24
#define AON_CMU_MEMSC0                          (1 << 0)

// reg_28
#define AON_CMU_MEMSC1                          (1 << 0)

// reg_2c
#define AON_CMU_MEMSC2                          (1 << 0)

// reg_30
#define AON_CMU_MEMSC3                          (1 << 0)

// reg_34
#define AON_CMU_MEMSC_STATUS0                   (1 << 0)
#define AON_CMU_MEMSC_STATUS1                   (1 << 1)
#define AON_CMU_MEMSC_STATUS2                   (1 << 2)
#define AON_CMU_MEMSC_STATUS3                   (1 << 3)

// reg_38
#define AON_CMU_WATCHDOG_RESET                  (1 << 0)
#define AON_CMU_SOFT_GLOBLE_RESET               (1 << 1)
#define AON_CMU_RTC_INTR_H                      (1 << 2)
#define AON_CMU_CHG_INTR_H                      (1 << 3)
#define AON_CMU_SOFT_BOOT_MODE(n)               (((n) & 0xFFFFFFF) << 4)
#define AON_CMU_SOFT_BOOT_MODE_MASK             (0xFFFFFFF << 4)
#define AON_CMU_SOFT_BOOT_MODE_SHIFT            (4)

// reg_3c
#define AON_CMU_SEL_MCU_PLLBB_PS                (1 << 0)
#define AON_CMU_SEL_MCU_PLLUSB                  (1 << 1)
#define AON_CMU_SEL_MCU_PLLA7USB                (1 << 2)
#define AON_CMU_RSTN_DIV_FLS                    (1 << 3)
#define AON_CMU_CFG_DIV_FLS(n)                  (((n) & 0x7) << 4)
#define AON_CMU_CFG_DIV_FLS_MASK                (0x7 << 4)
#define AON_CMU_CFG_DIV_FLS_SHIFT               (4)
#define AON_CMU_BYPASS_DIV_FLS                  (1 << 7)
#define AON_CMU_SEL_FLS_OSCX2                   (1 << 8)
#define AON_CMU_SEL_FLS_OSCX4                   (1 << 9)
#define AON_CMU_SEL_FLS_PLL                     (1 << 10)
#define AON_CMU_RSTN_DIV_PSR                    (1 << 11)
#define AON_CMU_CFG_DIV_PSR(n)                  (((n) & 0x3) << 12)
#define AON_CMU_CFG_DIV_PSR_MASK                (0x3 << 12)
#define AON_CMU_CFG_DIV_PSR_SHIFT               (12)
#define AON_CMU_BYPASS_DIV_PSR                  (1 << 14)
#define AON_CMU_SEL_PSR_OSCX2                   (1 << 15)
#define AON_CMU_SEL_PSR_OSCX4                   (1 << 16)
#define AON_CMU_SEL_PSR_PLL                     (1 << 17)
#define AON_CMU_SEL_PSR_INT                     (1 << 18)

// reg_40
#define AON_CMU_MANUAL_ACLK_ENABLE(n)           (((n) & 0xFFFFFFFF) << 0)
#define AON_CMU_MANUAL_ACLK_ENABLE_MASK         (0xFFFFFFFF << 0)
#define AON_CMU_MANUAL_ACLK_ENABLE_SHIFT        (0)
#define AON_CMU_MANUAL_OCLK_ENABLE(n)           (((n) & 0xFFFFFFFF) << AON_ACLK_NUM)
#define AON_CMU_MANUAL_OCLK_ENABLE_MASK         (0xFFFFFFFF << AON_ACLK_NUM)
#define AON_CMU_MANUAL_OCLK_ENABLE_SHIFT        (AON_ACLK_NUM)

// reg_44
#define AON_CMU_MANUAL_ACLK_DISABLE(n)          (((n) & 0xFFFFFFFF) << 0)
#define AON_CMU_MANUAL_ACLK_DISABLE_MASK        (0xFFFFFFFF << 0)
#define AON_CMU_MANUAL_ACLK_DISABLE_SHIFT       (0)
#define AON_CMU_MANUAL_OCLK_DISABLE(n)          (((n) & 0xFFFFFFFF) << AON_ACLK_NUM)
#define AON_CMU_MANUAL_OCLK_DISABLE_MASK        (0xFFFFFFFF << AON_ACLK_NUM)
#define AON_CMU_MANUAL_OCLK_DISABLE_SHIFT       (AON_ACLK_NUM)

// reg_48
#define AON_CMU_MODE_ACLK(n)                    (((n) & 0xFFFFFFFF) << 0)
#define AON_CMU_MODE_ACLK_MASK                  (0xFFFFFFFF << 0)
#define AON_CMU_MODE_ACLK_SHIFT                 (0)
#define AON_CMU_MODE_OCLK(n)                    (((n) & 0xFFFFFFFF) << AON_ACLK_NUM)
#define AON_CMU_MODE_OCLK_MASK                  (0xFFFFFFFF << AON_ACLK_NUM)
#define AON_CMU_MODE_OCLK_SHIFT                 (AON_ACLK_NUM)

// reg_4c
#define AON_CMU_SEL_AON_OSC                     (1 << 0)
#define AON_CMU_SEL_AON_OSCX2                   (1 << 1)
#define AON_CMU_SEL_AON_SPI_OSCX2               (1 << 2)
#define AON_CMU_SEL_CODEC_ANA_2                 (1 << 3)
#define AON_CMU_SEL_CODEC_OSC_2                 (1 << 4)
#define AON_CMU_SEL_CODEC_OSC                   (1 << 5)
#define AON_CMU_SEL_CODECHCLK_OSCX2             (1 << 6)
#define AON_CMU_SEL_CODECHCLK_PLL               (1 << 7)
#define AON_CMU_SEL_CODECHCLK_MCU               (1 << 8)
#define AON_CMU_EN_I2S_MCLK                     (1 << 9)
#define AON_CMU_SEL_I2S_MCLK(n)                 (((n) & 0xF) << 10)
#define AON_CMU_SEL_I2S_MCLK_MASK               (0xF << 10)
#define AON_CMU_SEL_I2S_MCLK_SHIFT              (10)
#define AON_CMU_POL_SPI_CS(n)                   (((n) & 0x7) << 14)
#define AON_CMU_POL_SPI_CS_MASK                 (0x7 << 14)
#define AON_CMU_POL_SPI_CS_SHIFT                (14)
#define AON_CMU_CFG_SPI_ARB(n)                  (((n) & 0x7) << 17)
#define AON_CMU_CFG_SPI_ARB_MASK                (0x7 << 17)
#define AON_CMU_CFG_SPI_ARB_SHIFT               (17)
#define AON_CMU_PU_FLASH_IO                     (1 << 20)
#define AON_CMU_EN_VAD_IIR                      (1 << 21)
#define AON_CMU_EN_VAD_RS                       (1 << 22)
#define AON_CMU_EN_MCU_PLLBB_MASK               (1 << 23)
#define AON_CMU_EN_MCU_PLLUSB_MASK              (1 << 24)
#define AON_CMU_EN_MCU_PLLPS_MASK               (1 << 25)
#define AON_CMU_EN_MCU_PLLA7_MASK               (1 << 26)
#define AON_CMU_LPU_AUTO_SWITCH26               (1 << 27)
#define AON_CMU_POR_SLEEP_MODE                  (1 << 28)

// reg_50
#define AON_CMU_CFG_DIV_TIMER0(n)               (((n) & 0xFFFF) << 0)
#define AON_CMU_CFG_DIV_TIMER0_MASK             (0xFFFF << 0)
#define AON_CMU_CFG_DIV_TIMER0_SHIFT            (0)
#define AON_CMU_CFG_DIV_TIMER1(n)               (((n) & 0xFFFF) << 16)
#define AON_CMU_CFG_DIV_TIMER1_MASK             (0xFFFF << 16)
#define AON_CMU_CFG_DIV_TIMER1_SHIFT            (16)

// reg_54
#define AON_CMU_CFG_DIV_PWM0(n)                 (((n) & 0xFFF) << 0)
#define AON_CMU_CFG_DIV_PWM0_MASK               (0xFFF << 0)
#define AON_CMU_CFG_DIV_PWM0_SHIFT              (0)
#define AON_CMU_SEL_PWM0_OSC                    (1 << 12)
#define AON_CMU_EN_CLK_PWM0_OSC                 (1 << 13)
#define AON_CMU_CFG_DIV_PWM1(n)                 (((n) & 0xFFF) << 16)
#define AON_CMU_CFG_DIV_PWM1_MASK               (0xFFF << 16)
#define AON_CMU_CFG_DIV_PWM1_SHIFT              (16)
#define AON_CMU_SEL_PWM1_OSC                    (1 << 28)
#define AON_CMU_EN_CLK_PWM1_OSC                 (1 << 29)

// reg_58
#define AON_CMU_CFG_DIV_PWM2(n)                 (((n) & 0xFFF) << 0)
#define AON_CMU_CFG_DIV_PWM2_MASK               (0xFFF << 0)
#define AON_CMU_CFG_DIV_PWM2_SHIFT              (0)
#define AON_CMU_SEL_PWM2_OSC                    (1 << 12)
#define AON_CMU_EN_CLK_PWM2_OSC                 (1 << 13)
#define AON_CMU_CFG_DIV_PWM3(n)                 (((n) & 0xFFF) << 16)
#define AON_CMU_CFG_DIV_PWM3_MASK               (0xFFF << 16)
#define AON_CMU_CFG_DIV_PWM3_SHIFT              (16)
#define AON_CMU_SEL_PWM3_OSC                    (1 << 28)
#define AON_CMU_EN_CLK_PWM3_OSC                 (1 << 29)

// reg_5c
#define AON_CMU_EXT_LNA_AT_GAIN_STEP0(n)        (((n) & 0x7) << 0)
#define AON_CMU_EXT_LNA_AT_GAIN_STEP0_MASK      (0x7 << 0)
#define AON_CMU_EXT_LNA_AT_GAIN_STEP0_SHIFT     (0)
#define AON_CMU_EXT_LNA_AT_GAIN_STEP1(n)        (((n) & 0x7) << 3)
#define AON_CMU_EXT_LNA_AT_GAIN_STEP1_MASK      (0x7 << 3)
#define AON_CMU_EXT_LNA_AT_GAIN_STEP1_SHIFT     (3)
#define AON_CMU_EXT_LNA_AT_GAIN_STEP2(n)        (((n) & 0x7) << 6)
#define AON_CMU_EXT_LNA_AT_GAIN_STEP2_MASK      (0x7 << 6)
#define AON_CMU_EXT_LNA_AT_GAIN_STEP2_SHIFT     (6)
#define AON_CMU_EXT_LNA_AT_GAIN_STEP3(n)        (((n) & 0x7) << 9)
#define AON_CMU_EXT_LNA_AT_GAIN_STEP3_MASK      (0x7 << 9)
#define AON_CMU_EXT_LNA_AT_GAIN_STEP3_SHIFT     (9)
#define AON_CMU_EXT_LNA_ENABLE                  (1 << 12)
#define AON_CMU_EXT_LNA_ENABLE_DR               (1 << 13)
#define AON_CMU_EXT_LNA_EN_BYPASS_RXON          (1 << 14)
#define AON_CMU_FEM_SWTCH_WF_MUX_CFG(n)         (((n) & 0x3) << 15)
#define AON_CMU_FEM_SWTCH_WF_MUX_CFG_MASK       (0x3 << 15)
#define AON_CMU_FEM_SWTCH_WF_MUX_CFG_SHIFT      (15)

// reg_64
#define AON_CMU_CFG_DIV_PCM(n)                  (((n) & 0x1FFF) << 0)
#define AON_CMU_CFG_DIV_PCM_MASK                (0x1FFF << 0)
#define AON_CMU_CFG_DIV_PCM_SHIFT               (0)
#define AON_CMU_EN_CLK_PLL_PCM                  (1 << 13)
#define AON_CMU_CFG_DIV_I2S0(n)                 (((n) & 0x1FFF) << 16)
#define AON_CMU_CFG_DIV_I2S0_MASK               (0x1FFF << 16)
#define AON_CMU_CFG_DIV_I2S0_SHIFT              (16)
#define AON_CMU_EN_CLK_PLL_I2S0                 (1 << 29)
#define AON_CMU_SEL_AUD_PLLUSB                  (1 << 30)
#define AON_CMU_SEL_AUD_X4                      (1 << 31)

// reg_68
#define AON_CMU_CFG_DIV_SPDIF0(n)               (((n) & 0x1FFF) << 0)
#define AON_CMU_CFG_DIV_SPDIF0_MASK             (0x1FFF << 0)
#define AON_CMU_CFG_DIV_SPDIF0_SHIFT            (0)
#define AON_CMU_EN_CLK_PLL_SPDIF0               (1 << 13)
#define AON_CMU_CFG_DIV_I2S1(n)                 (((n) & 0x1FFF) << 16)
#define AON_CMU_CFG_DIV_I2S1_MASK               (0x1FFF << 16)
#define AON_CMU_CFG_DIV_I2S1_SHIFT              (16)
#define AON_CMU_EN_CLK_PLL_I2S1                 (1 << 29)

// reg_6c
#define AON_CMU_SLEEP_TIMER_OSC(n)              (((n) & 0x7FF) << 0)
#define AON_CMU_SLEEP_TIMER_OSC_MASK            (0x7FF << 0)
#define AON_CMU_SLEEP_TIMER_OSC_SHIFT           (0)
#define AON_CMU_STORE_GPIO_EN                   (1 << 30)
#define AON_CMU_STORE_TIMER                     (1 << 31)

// reg_70
#define AON_CMU_SLEEP_TIMER_32K(n)              (((n) & 0xFFFFFF) << 0)
#define AON_CMU_SLEEP_TIMER_32K_MASK            (0xFFFFFF << 0)
#define AON_CMU_SLEEP_TIMER_32K_SHIFT           (0)

// reg_74
#define AON_CMU_STORE_GPIO_MASK(n)              (((n) & 0xFFFFFFFF) << 0)
#define AON_CMU_STORE_GPIO_MASK_MASK            (0xFFFFFFFF << 0)
#define AON_CMU_STORE_GPIO_MASK_SHIFT           (0)

// reg_78
#define AON_CMU_CFG_DIV_CODECIIR(n)             (((n) & 0xF) << 0)
#define AON_CMU_CFG_DIV_CODECIIR_MASK           (0xF << 0)
#define AON_CMU_CFG_DIV_CODECIIR_SHIFT          (0)
#define AON_CMU_SEL_CODECIIR_OSC                (1 << 4)
#define AON_CMU_SEL_CODECIIR_OSCX2              (1 << 5)
#define AON_CMU_BYPASS_DIV_CODECIIR             (1 << 6)
#define AON_CMU_CFG_DIV_CODECRS0(n)             (((n) & 0xF) << 8)
#define AON_CMU_CFG_DIV_CODECRS0_MASK           (0xF << 8)
#define AON_CMU_CFG_DIV_CODECRS0_SHIFT          (8)
#define AON_CMU_SEL_CODECRS0_OSC                (1 << 12)
#define AON_CMU_SEL_CODECRS0_OSCX2              (1 << 13)
#define AON_CMU_BYPASS_DIV_CODECRS0             (1 << 14)
#define AON_CMU_CFG_DIV_CODECRS1(n)             (((n) & 0xF) << 16)
#define AON_CMU_CFG_DIV_CODECRS1_MASK           (0xF << 16)
#define AON_CMU_CFG_DIV_CODECRS1_SHIFT          (16)
#define AON_CMU_SEL_CODECRS1_OSC                (1 << 20)
#define AON_CMU_SEL_CODECRS1_OSCX2              (1 << 21)
#define AON_CMU_BYPASS_DIV_CODECRS1             (1 << 22)

// reg_7c
#define AON_CMU_OTP_WR_LOCK(n)                  (((n) & 0xFFFF) << 0)
#define AON_CMU_OTP_WR_LOCK_MASK                (0xFFFF << 0)
#define AON_CMU_OTP_WR_LOCK_SHIFT               (0)
#define AON_CMU_OTP_WR_UNLOCK                   (1 << 31)

// reg_80
#define AON_CMU_OTP_RD_LOCK(n)                  (((n) & 0xFFFF) << 0)
#define AON_CMU_OTP_RD_LOCK_MASK                (0xFFFF << 0)
#define AON_CMU_OTP_RD_LOCK_SHIFT               (0)
#define AON_CMU_OTP_RD_UNLOCK                   (1 << 31)

// reg_84
#define AON_CMU_CFG_PD_STAB_TIMER(n)            (((n) & 0xF) << 0)
#define AON_CMU_CFG_PD_STAB_TIMER_MASK          (0xF << 0)
#define AON_CMU_CFG_PD_STAB_TIMER_SHIFT         (0)

// reg_88
#define AON_CMU_SEL_32K_TIMER                   (1 << 0)
#define AON_CMU_SEL_32K_WDT                     (1 << 1)
#define AON_CMU_SEL_TIMER_FAST                  (1 << 2)
#define AON_CMU_EN_MCU_WDG_RESET                (1 << 3)
#define AON_CMU_SEL_32K_WF                      (1 << 4)
#define AON_CMU_SEL_OSC_WF                      (1 << 5)

// reg_8c
#define AON_CMU_PU_PLLBB_ENABLE                 (1 << 0)
#define AON_CMU_PU_PLLUSB_ENABLE                (1 << 1)
#define AON_CMU_PU_PLLDDR_ENABLE                (1 << 2)
#define AON_CMU_PU_PLLA7_ENABLE                 (1 << 3)
#define AON_CMU_PU_PLLBB_DIV_WF_ENABLE          (1 << 4)
#define AON_CMU_PU_PLLBB_DIV_PS_ENABLE          (1 << 5)
#define AON_CMU_PU_PLLBB_DIV_MCU_ENABLE         (1 << 6)
#define AON_CMU_PU_OSC_ENABLE                   (1 << 7)
#define AON_CMU_PU_OSCX2_ENABLE                 (1 << 8)
#define AON_CMU_PU_OSCX4_ENABLE                 (1 << 9)

// reg_90
#define AON_CMU_PU_PLLBB_DISABLE                (1 << 0)
#define AON_CMU_PU_PLLUSB_DISABLE               (1 << 1)
#define AON_CMU_PU_PLLDDR_DISABLE               (1 << 2)
#define AON_CMU_PU_PLLA7_DISABLE                (1 << 3)
#define AON_CMU_PU_PLLBB_DIV_WF_DISABLE         (1 << 4)
#define AON_CMU_PU_PLLBB_DIV_PS_DISABLE         (1 << 5)
#define AON_CMU_PU_PLLBB_DIV_MCU_DISABLE        (1 << 6)
#define AON_CMU_PU_OSC_DISABLE                  (1 << 7)
#define AON_CMU_PU_OSCX2_DISABLE                (1 << 8)
#define AON_CMU_PU_OSCX4_DISABLE                (1 << 9)

// reg_94
#define AON_CMU_SEL_A7_PLLUSB                   (1 << 0)
#define AON_CMU_SEL_A7_PLLBB                    (1 << 1)
#define AON_CMU_EN_A7_PLLBB_MASK                (1 << 2)
#define AON_CMU_EN_A7_PLLUSB_MASK               (1 << 3)
#define AON_CMU_EN_A7_PLLA7_MASK                (1 << 4)
#define AON_CMU_PU_A7_PLLBB_MASK                (1 << 5)
#define AON_CMU_PU_A7_PLLUSB_MASK               (1 << 6)
#define AON_CMU_PU_A7_PLLA7_MASK                (1 << 7)

// reg_98
#define AON_CMU_RSTN_DIV_DDR                    (1 << 0)
#define AON_CMU_CFG_DIV_DDR(n)                  (((n) & 0x3) << 1)
#define AON_CMU_CFG_DIV_DDR_MASK                (0x3 << 1)
#define AON_CMU_CFG_DIV_DDR_SHIFT               (1)
#define AON_CMU_BYPASS_DIV_DDR                  (1 << 3)
#define AON_CMU_SEL_DDR_OSCX2                   (1 << 4)
#define AON_CMU_SEL_DDR_OSCX4                   (1 << 5)
#define AON_CMU_SEL_DDR_PLL                     (1 << 6)

// reg_e8
#define AON_CMU_RESERVED0(n)                    (((n) & 0xFFFFFFFF) << 0)
#define AON_CMU_RESERVED0_MASK                  (0xFFFFFFFF << 0)
#define AON_CMU_RESERVED0_SHIFT                 (0)

// reg_ec
#define AON_CMU_RESERVED1(n)                    (((n) & 0xFFFFFFFF) << 0)
#define AON_CMU_RESERVED1_MASK                  (0xFFFFFFFF << 0)
#define AON_CMU_RESERVED1_SHIFT                 (0)

// reg_f0
#define AON_CMU_DEBUG0(n)                       (((n) & 0xFFFFFFFF) << 0)
#define AON_CMU_DEBUG0_MASK                     (0xFFFFFFFF << 0)
#define AON_CMU_DEBUG0_SHIFT                    (0)

// reg_f4
#define AON_CMU_DEBUG1(n)                       (((n) & 0xFFFFFFFF) << 0)
#define AON_CMU_DEBUG1_MASK                     (0xFFFFFFFF << 0)
#define AON_CMU_DEBUG1_SHIFT                    (0)

// reg_f8
#define AON_CMU_DEBUG2(n)                       (((n) & 0xFFFFFFFF) << 0)
#define AON_CMU_DEBUG2_MASK                     (0xFFFFFFFF << 0)
#define AON_CMU_DEBUG2_SHIFT                    (0)

// reg_fc
#define AON_CMU_EFUSE(n)                        (((n) & 0xFFFF) << 0)
#define AON_CMU_EFUSE_MASK                      (0xFFFF << 0)
#define AON_CMU_EFUSE_SHIFT                     (0)
#define AON_CMU_EFUSE_LOCK                      (1 << 31)

// APB and AHB Clocks:
#define AON_ACLK_CMU                            (1 << 0)
#define AON_ARST_CMU                            (1 << 0)
#define AON_ACLK_GPIO                           (1 << 1)
#define AON_ARST_GPIO                           (1 << 1)
#define AON_ACLK_GPIO_INT                       (1 << 2)
#define AON_ARST_GPIO_INT                       (1 << 2)
#define AON_ACLK_WDT                            (1 << 3)
#define AON_ARST_WDT                            (1 << 3)
#define AON_ACLK_PWM                            (1 << 4)
#define AON_ARST_PWM                            (1 << 4)
#define AON_ACLK_TIMER                          (1 << 5)
#define AON_ARST_TIMER                          (1 << 5)
#define AON_ACLK_IOMUX                          (1 << 6)
#define AON_ARST_IOMUX                          (1 << 6)
#define AON_ACLK_SPIDPD                         (1 << 7)
#define AON_ARST_SPIDPD                         (1 << 7)
#define AON_ACLK_APBC                           (1 << 8)
#define AON_ARST_APBC                           (1 << 8)
#define AON_ACLK_H2H_MCU                        (1 << 9)
#define AON_ARST_H2H_MCU                        (1 << 9)

// AON other Clocks:
#define AON_OCLK_WDT                            (1 << 0)
#define AON_ORST_WDT                            (1 << 0)
#define AON_OCLK_TIMER                          (1 << 1)
#define AON_ORST_TIMER                          (1 << 1)
#define AON_OCLK_GPIO                           (1 << 2)
#define AON_ORST_GPIO                           (1 << 2)
#define AON_OCLK_PWM0                           (1 << 3)
#define AON_ORST_PWM0                           (1 << 3)
#define AON_OCLK_PWM1                           (1 << 4)
#define AON_ORST_PWM1                           (1 << 4)
#define AON_OCLK_PWM2                           (1 << 5)
#define AON_ORST_PWM2                           (1 << 5)
#define AON_OCLK_PWM3                           (1 << 6)
#define AON_ORST_PWM3                           (1 << 6)
#define AON_OCLK_IOMUX                          (1 << 7)
#define AON_ORST_IOMUX                          (1 << 7)
#define AON_OCLK_SLP32K                         (1 << 8)
#define AON_ORST_SLP32K                         (1 << 8)
#define AON_OCLK_SLP26M                         (1 << 9)
#define AON_ORST_SLP26M                         (1 << 9)
#define AON_OCLK_SPIDPD                         (1 << 10)
#define AON_ORST_SPIDPD                         (1 << 10)

#endif

