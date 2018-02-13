/*
 * Copyright (C) 2017 XRADIO TECHNOLOGY CO., LTD. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the
 *       distribution.
 *    3. Neither the name of XRADIO TECHNOLOGY CO., LTD. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _DRIVER_CHIP_HAL_PRCM_H_
#define _DRIVER_CHIP_HAL_PRCM_H_

#include "driver/chip/hal_def.h"

#ifdef __cplusplus
extern "C" {
#endif

/* prcm system module control */
typedef struct {
    __IO uint32_t SYS_DCDC_CTRL;            /* offset: 0x0000, System DCDC control register */
    __IO uint32_t SYS_LDO_SW_CTRL;          /* offset: 0x0004, System LDO and switch control register */
    __IO uint32_t SYS_LFCLK_CTRL;           /* offset: 0x0008, System LFCLK control register */
    __IO uint32_t SYS_HOSC_CTRL;            /* offset: 0x000C, System HOSC type register */
    __IO uint32_t SYS_RCOSC_CALIB_CTRL;     /* offset: 0x0010, System RCOSC calibration control register */
         uint32_t RESERVED0[3];
    __IO uint32_t SYS_PLL_CTRL;             /* offset: 0x0020, System PLL control register */
    __IO uint32_t SYS_CLK1_CTRL;            /* offset: 0x0024, System clock 1 control register */
    __I  uint32_t SYS_CLK2_CTRL;            /* offset: 0x0028, System clock 2 control register */
    __I  uint32_t SYS_CLK3_CTRL;            /* offset: 0x002C, System clock 3 control register */
    __IO uint32_t AUD_PLL_CTRL;             /* offset: 0x0030, Audio PLL control register */
    __IO uint32_t DEV_CLK_CTRL;             /* offset: 0x0034, Device clock control register */
         uint32_t RESERVED1[3];
    __IO uint32_t AUD_PLL_BIAS;             /* offset: 0x0044, Audio PLL bias register */
         uint32_t RESERVED2[3];
    __IO uint32_t AUD_PLL_PAT_CTRL;         /* offset: 0x0054, Audio PLL pattern control register */
    __IO uint32_t DCXO_CTRL;                /* offset: 0x0058, DCXO control register */
         uint32_t RESERVED3[9];
    __I  uint32_t SYS1_CTRL;                /* offset: 0x0080, System 1 control register */
    __IO uint32_t SYS1_STATUS;              /* offset: 0x0084, System 1 status register */
    __IO uint32_t SYS2_CTRL;                /* offset: 0x0088, System 2 control register */
    __I  uint32_t SYS2_STATUS;              /* offset: 0x008C, System 2 status register */
    __I  uint32_t SYS3_CTRL;                /* offset: 0x0090, System 3 control register */
    __I  uint32_t SYS3_STATUS;              /* offset: 0x0094, System 3 status register */
    __IO uint32_t SYS1_WAKEUP_CTRL;         /* offset: 0x0098, System 1 wakeup control register */
    __I  uint32_t SYS2_WAKEUP_CTRL;         /* offset: 0x009C, System 2 wakeup control register */
    __IO uint32_t SYS1_SLEEP_CTRL;          /* offset: 0x00A0, System 1 sleep control register */
    __I  uint32_t SYS2_SLEEP_CTRL;          /* offset: 0x00A4, System 2 sleep control register */
    __IO uint32_t DCXO_STABLE_REF_TIME;     /* offset: 0x00A8, DCXO stable reference time register */
    __IO uint32_t DPLL_STABLE_REF_TIME;     /* offset: 0x00AC, DPLL stable reference time register */
    __IO uint32_t LDO_STABLE_REF_TIME;      /* offset: 0x00B0, LDO stable reference time register */
    __IO uint32_t DIG_SWITCH_REF_TIME;      /* offset: 0x00B4, Digital switch reference time register */
    __IO uint32_t SRAM_VOLT_CTRL;           /* offset: 0x00B8, SRAM voltage control register */
    __IO uint32_t BANDGAP_STABLE_REF_TIME;  /* offset: 0x00BC, Band gap stable reference time register */
    __IO uint32_t DCDC_STABLE_REF_TIME;     /* offset: 0x00C0, DCDC stable reference time register */
         uint32_t RESERVED4[15];
    __IO uint32_t CPUA_BOOT_FLAG;           /* offset: 0x0100, CPUA boot flag register */
    __IO uint32_t CPUA_BOOT_ADDR;           /* offset: 0x0104, CPUA boot address register */
    __IO uint32_t CPUA_BOOT_ARG;            /* offset: 0x0108, CPUA boot argument register */
    __I  uint32_t CPUN_BOOT_FLAG;           /* offset: 0x010C, CPUN boot flag register */
    __I  uint32_t CPUN_BOOT_ADDR;           /* offset: 0x0110, CPUN boot address register */
    __I  uint32_t CPUN_BOOT_ARG;            /* offset: 0x0114, CPUN boot argument register */
    __IO uint32_t CPUA_PRIV_REG;            /* offset: 0x0118, CPUA private register */
         uint32_t CPUN_PRIV_REG;            /* offset: 0x011C, CPUN private register */
    __IO uint32_t CPUA_WAKE_TIMER_CNT;      /* offset: 0x0120, CPUA wakeup timer counter register */
    __IO uint32_t CPUA_WAKE_TIMER_CMP;      /* offset: 0x0124, CPUA wakeup timer value register */
    __I  uint32_t CPUN_WAKE_TIMER_CNT;      /* offset: 0x0128, CPUN wakeup timer counter register */
    __I  uint32_t CPUN_WAKE_TIMER_CMP;      /* offset: 0x012C, CPUN wakeup timer value register */
    __IO uint32_t CPUA_WAKE_IO_EN;          /* offset: 0x0130, CPUA IO wakeup enable register */
    __IO uint32_t CPUA_WAKE_IO_MODE;        /* offset: 0x0134, CPUA IO wakeup mode register */
    __IO uint32_t CPUA_WAKE_IO_STATUS;      /* offset: 0x0138, CPUA IO wakeup status register */
    __IO uint32_t CPUA_WAKE_IO_HOLD;        /* offset: 0x013C, CPUA IO hold control register */
    __IO uint32_t CPUA_WAKE_IO_GLOBAL_EN;   /* offset: 0x0140, CPUA IO wakeup global enable register */
    __IO uint32_t CPUA_PRCM_REG;            /* offset: 0x0144, CPUA PRCM register */
         uint32_t RESERVED6[46];
    __IO uint32_t DCDC_PARAM_CTRL;          /* offset: 0x0200, DCDC parameter control register */
    __IO uint32_t ANA_BANDGAP;              /* offset: 0x0204, Analog band gap control register */
    __I  uint32_t CLK_LDO_PARAM;            /* offset: 0x0208, Clock LDO parameter register */
    __IO uint32_t DIG_LDO_PARAM;            /* offset: 0x020C, Digital LDO parameter register */
    __IO uint32_t DPLL_STATUS;              /* offset: 0x0210, DPLL status register */
} PRCM_T;

#define PRCM ((PRCM_T *)PRCM_BASE)          /* address: 0x40040000 */

/*
 * bit field definition of PRCM->SYS_DCDC_CTRL
 */
#define PRCM_DCDC_VOLT_SHIFT	16	/* R/W */
#define PRCM_DCDC_VOLT_MASK		(0xFU << PRCM_DCDC_VOLT_SHIFT)
typedef enum {
	PRCM_DCDC_VOLT_1V80	= (0x0U << PRCM_DCDC_VOLT_SHIFT),
	PRCM_DCDC_VOLT_1V77	= (0x1U << PRCM_DCDC_VOLT_SHIFT),
	PRCM_DCDC_VOLT_1V75	= (0x2U << PRCM_DCDC_VOLT_SHIFT),
	PRCM_DCDC_VOLT_1V72	= (0x3U << PRCM_DCDC_VOLT_SHIFT),
	PRCM_DCDC_VOLT_1V69	= (0x4U << PRCM_DCDC_VOLT_SHIFT),
	PRCM_DCDC_VOLT_1V67	= (0x5U << PRCM_DCDC_VOLT_SHIFT),
	PRCM_DCDC_VOLT_1V64	= (0x6U << PRCM_DCDC_VOLT_SHIFT),
	PRCM_DCDC_VOLT_1V61	= (0x7U << PRCM_DCDC_VOLT_SHIFT),
	PRCM_DCDC_VOLT_1V58	= (0x8U << PRCM_DCDC_VOLT_SHIFT),
	PRCM_DCDC_VOLT_1V56	= (0x9U << PRCM_DCDC_VOLT_SHIFT),
	PRCM_DCDC_VOLT_1V53	= (0xAU << PRCM_DCDC_VOLT_SHIFT),
	PRCM_DCDC_VOLT_1V51	= (0xBU << PRCM_DCDC_VOLT_SHIFT),
	PRCM_DCDC_VOLT_1V48	= (0xCU << PRCM_DCDC_VOLT_SHIFT),
	PRCM_DCDC_VOLT_1V45	= (0xDU << PRCM_DCDC_VOLT_SHIFT),
	PRCM_DCDC_VOLT_1V43	= (0xEU << PRCM_DCDC_VOLT_SHIFT),
	PRCM_DCDC_VOLT_1V40	= (0xFU << PRCM_DCDC_VOLT_SHIFT)
} PRCM_DCDCVolt;

#define PRCM_DCDC_EN_BIT		HAL_BIT(0)	/* R */

/*
 * bit field definition of PRCM->SYS_LDO_SW_CTRL
 */
#define PRCM_SYS3_SRAM_PWR3_EN_BIT	HAL_BIT(15)	/* R */
#define PRCM_SYS3_PWR5_EN_BIT		HAL_BIT(14)	/* R */
#define PRCM_SYS3_PWR4_EN_BIT		HAL_BIT(13)	/* R */

#define PRCM_SYS2_PWR3_EN_BIT		HAL_BIT(12)	/* R/W */
#define PRCM_SYS2_SRAM_PWR2_EN_BIT	HAL_BIT(11)	/* R/W */

#define PRCM_SYS1_SRAM_PWR1_EN_BIT	HAL_BIT(10)	/* R */
#define PRCM_SYS1_PWR2_EN_BIT		HAL_BIT(9)	/* R */
#define PRCM_SYS1_PWR1_EN_BIT		HAL_BIT(8)	/* R */

#define PRCM_LDO1_VOLT_SHIFT		4	/* R/W */
#define PRCM_LDO1_VOLT_MASK			(0x7U << PRCM_LDO1_VOLT_SHIFT)
typedef enum {
	PRCM_LDO1_VOLT_1V10	= (0x0U << PRCM_LDO1_VOLT_SHIFT),
	PRCM_LDO1_VOLT_1V05	= (0x1U << PRCM_LDO1_VOLT_SHIFT),
	PRCM_LDO1_VOLT_1V00	= (0x2U << PRCM_LDO1_VOLT_SHIFT),
	PRCM_LDO1_VOLT_0V95	= (0x3U << PRCM_LDO1_VOLT_SHIFT),
	PRCM_LDO1_VOLT_0V90	= (0x4U << PRCM_LDO1_VOLT_SHIFT)
} PRCM_LDO1Volt;

#define PRCM_SRAM_LDO_EN_BIT		HAL_BIT(2)	/* R */
#define PRCM_PLL_LDO_EN_BIT			HAL_BIT(1)	/* R/W */
#define PRCM_LDO1_EN_BIT			HAL_BIT(0)	/* R */

/*
 * bit field definition of PRCM->SYS_LFCLK_CTRL
 */
#define PRCM_LFCLK_EXT32K_EN_BIT	HAL_BIT(31)	/* R/W */
#define PRCM_LFCLK_INTER32K_EN_BIT	HAL_BIT(30)	/* R/W */

#define PRCM_LFCLK_SRC_SHIFT		24	/* R/W */
#define PRCM_LFCLK_SRC_MASK			(0x1U << PRCM_LFCLK_SRC_SHIFT)
typedef enum {
	PRCM_LFCLK_SRC_INTER32K = (0x0U << PRCM_LFCLK_SRC_SHIFT),
	PRCM_LFCLK_SRC_EXT32K 	= (0x1U << PRCM_LFCLK_SRC_SHIFT)
} PRCM_LFCLKSrc;

/*
 * bit field definition of PRCM->SYS_HOSC_CTRL
 */
#define PRCM_HOSC_TYPE_SHIFT	0	/* R/W */
#define PRCM_HOSC_TYPE_VMASK	0x3U
#define PRCM_HOSC_TYPE_MASK		(PRCM_HOSC_TYPE_VMASK << PRCM_HOSC_TYPE_SHIFT)
typedef enum {
	PRCM_HOSC_TYPE_26M = (0U << PRCM_HOSC_TYPE_SHIFT),
	PRCM_HOSC_TYPE_40M = (1U << PRCM_HOSC_TYPE_SHIFT),
	PRCM_HOSC_TYPE_24M = (2U << PRCM_HOSC_TYPE_SHIFT),
	PRCM_HOSC_TYPE_52M = (3U << PRCM_HOSC_TYPE_SHIFT)
} PRCM_HOSCType;

/*
 * bit field definition of PRCM->SYS_RCOSC_CALIB_CTRL
 */
#define PRCM_RCOSC_CALIB_FREQ_SHIFT	8	/* R */
#define PRCM_RCOSC_CALIB_FREQ_VMASK	0xFFFFFU

#define PRCM_RCOSC_CALIB_EN_BIT		HAL_BIT(0)	/* R/W */

/*
 * bit field definition of PRCM->SYS_PLL_CTRL
 */
#define PRCM_SYS_PLL_EN_BIT		HAL_BIT(31)	/* R/W */

#define PRCM_SYS_PLL_PARAM_SHIFT	0
#define PRCM_SYS_PLL_PARAM_MASK		0x7FFFFFFFU
typedef enum {
	PRCM_SYS_PLL_PARAM_HOSC24M	= 0x00001141U,
	PRCM_SYS_PLL_PARAM_HOSC26M	= 0x0EC4F121U,
	PRCM_SYS_PLL_PARAM_HOSC40M	= 0x000010C1U,
	PRCM_SYS_PLL_PARAM_HOSC52M	= 0x07627091U
} PRCM_SysPLLParam;

/*
 * bit field definition of
 *   - PRCM->SYS_CLK1_CTRL (R/W)
 *   - PRCM->SYS_CLK2_CTRL (R)
 *   - PRCM->SYS_CLK3_CTRL (R)
 */
#define PRCM_SYS_CLK_EN_BIT			HAL_BIT(31)	/* R/W, R, R */

/* NB: the following bits are for SYS_CLK1_CTRL, SYS_CLK2_CTRL only */
#define PRCM_CPU_CLK_SRC_SHIFT		16	/* R/W, R */
#define PRCM_CPU_CLK_SRC_MASK		(0x3 << PRCM_CPU_CLK_SRC_SHIFT)
typedef enum {
	PRCM_CPU_CLK_SRC_HFCLK		= (0x0U << PRCM_CPU_CLK_SRC_SHIFT),
	PRCM_CPU_CLK_SRC_LFCLK		= (0x1U << PRCM_CPU_CLK_SRC_SHIFT),
	PRCM_CPU_CLK_SRC_SYSCLK		= (0x2U << PRCM_CPU_CLK_SRC_SHIFT)
} PRCM_CPUClkSrc;

/* NB: the following bits are for SYS_CLK1_CTRL, SYS_CLK2_CTRL only */
#define PRCM_SYS_CLK_FACTOR_SHIFT	0	/* R/W */
#define PRCM_SYS_CLK_FACTOR_VMASK	0xFU
#define PRCM_SYS_CLK_FACTOR_MASK	(PRCM_SYS_CLK_FACTOR_VMASK << PRCM_SYS_CLK_FACTOR_SHIFT)
typedef enum {
//	PRCM_SYS_CLK_FACTOR_960M = (0U  << PRCM_SYS_CLK_FACTOR_SHIFT),
//	PRCM_SYS_CLK_FACTOR_480M = (1U  << PRCM_SYS_CLK_FACTOR_SHIFT),
//	PRCM_SYS_CLK_FACTOR_320M = (2U  << PRCM_SYS_CLK_FACTOR_SHIFT),
	PRCM_SYS_CLK_FACTOR_240M = (3U  << PRCM_SYS_CLK_FACTOR_SHIFT),
	PRCM_SYS_CLK_FACTOR_192M = (4U  << PRCM_SYS_CLK_FACTOR_SHIFT),
	PRCM_SYS_CLK_FACTOR_160M = (5U  << PRCM_SYS_CLK_FACTOR_SHIFT),
	PRCM_SYS_CLK_FACTOR_137M = (6U  << PRCM_SYS_CLK_FACTOR_SHIFT),
	PRCM_SYS_CLK_FACTOR_120M = (7U  << PRCM_SYS_CLK_FACTOR_SHIFT),
	PRCM_SYS_CLK_FACTOR_107M = (8U  << PRCM_SYS_CLK_FACTOR_SHIFT),
	PRCM_SYS_CLK_FACTOR_96M  = (9U  << PRCM_SYS_CLK_FACTOR_SHIFT),
	PRCM_SYS_CLK_FACTOR_87M  = (10U << PRCM_SYS_CLK_FACTOR_SHIFT),
	PRCM_SYS_CLK_FACTOR_80M  = (11U << PRCM_SYS_CLK_FACTOR_SHIFT),
	PRCM_SYS_CLK_FACTOR_74M  = (12U << PRCM_SYS_CLK_FACTOR_SHIFT),
	PRCM_SYS_CLK_FACTOR_69M  = (13U << PRCM_SYS_CLK_FACTOR_SHIFT),
	PRCM_SYS_CLK_FACTOR_64M  = (14U << PRCM_SYS_CLK_FACTOR_SHIFT),
	PRCM_SYS_CLK_FACTOR_60M  = (15U << PRCM_SYS_CLK_FACTOR_SHIFT),
} PRCM_SysClkFactor;

/*
 * bit field definition of PRCM->AUD_PLL_CTRL
 */
#define PRCM_AUD_PLL_EN_BIT			HAL_BIT(31)	/* R/W */

#define PRCM_AUD_PLL_PARAM_SHIFT	0
#define PRCM_AUD_PLL_PARAM_MASK		0x7FFFFFFFU
typedef enum {
	PRCM_AUD_PLL24M_PARAM_HOSC13M	= 0x8003780FU,
	PRCM_AUD_PLL24M_PARAM_HOSC19M2	= 0x80037F18U,
	PRCM_AUD_PLL24M_PARAM_HOSC24M	= 0x80035514U,
	PRCM_AUD_PLL24M_PARAM_HOSC26M	= 0x8003781FU,
	PRCM_AUD_PLL24M_PARAM_HOSC40M	= 0x80033A17U,
	PRCM_AUD_PLL24M_PARAM_HOSC50M	= 0x8003381CU,
	PRCM_AUD_PLL24M_PARAM_HOSC52M	= 0x81030603U,
	/*22.5792*/
	PRCM_AUD_PLL22M_PARAM_HOSC13M	= 0x81031A03U,
	PRCM_AUD_PLL22M_PARAM_HOSC19M2	= 0x80037E1AU,
	PRCM_AUD_PLL22M_PARAM_HOSC24M	= 0x80034E14U,
	PRCM_AUD_PLL22M_PARAM_HOSC26M	= 0x80034112U,
	PRCM_AUD_PLL22M_PARAM_HOSC40M	= 0x8003451EU,
	PRCM_AUD_PLL22M_PARAM_HOSC50M	= 0x8003371EU,
	PRCM_AUD_PLL22M_PARAM_HOSC52M	= 0x80032012U
} PRCM_AudPLLParam;

/*
 * bit field definition of PRCM->DEV_CLK_CTRL
 */
#define PRCM_DEV_CLK_FACTOR_SHIFT	0	/* R/W */
#define PRCM_DEV_CLK_FACTOR_VMASK	0xFU
#define PRCM_DEV_CLK_FACTOR_MASK	(PRCM_DEV_CLK_FACTOR_VMASK << PRCM_DEV_CLK_FACTOR_SHIFT)
typedef enum {
//	PRCM_DEV_CLK_FACTOR_960M = (0U  << PRCM_DEV_CLK_FACTOR_SHIFT),
//	PRCM_DEV_CLK_FACTOR_480M = (1U  << PRCM_DEV_CLK_FACTOR_SHIFT),
//	PRCM_DEV_CLK_FACTOR_320M = (2U  << PRCM_DEV_CLK_FACTOR_SHIFT),
//	PRCM_DEV_CLK_FACTOR_240M = (3U  << PRCM_DEV_CLK_FACTOR_SHIFT),
	PRCM_DEV_CLK_FACTOR_192M = (4U  << PRCM_DEV_CLK_FACTOR_SHIFT),
	PRCM_DEV_CLK_FACTOR_160M = (5U  << PRCM_DEV_CLK_FACTOR_SHIFT),
	PRCM_DEV_CLK_FACTOR_137M = (6U  << PRCM_DEV_CLK_FACTOR_SHIFT),
	PRCM_DEV_CLK_FACTOR_120M = (7U  << PRCM_DEV_CLK_FACTOR_SHIFT),
	PRCM_DEV_CLK_FACTOR_107M = (8U  << PRCM_DEV_CLK_FACTOR_SHIFT),
	PRCM_DEV_CLK_FACTOR_96M  = (9U  << PRCM_DEV_CLK_FACTOR_SHIFT),
	PRCM_DEV_CLK_FACTOR_87M  = (10U << PRCM_DEV_CLK_FACTOR_SHIFT),
	PRCM_DEV_CLK_FACTOR_80M  = (11U << PRCM_DEV_CLK_FACTOR_SHIFT),
	PRCM_DEV_CLK_FACTOR_74M  = (12U << PRCM_DEV_CLK_FACTOR_SHIFT),
	PRCM_DEV_CLK_FACTOR_69M  = (13U << PRCM_DEV_CLK_FACTOR_SHIFT),
	PRCM_DEV_CLK_FACTOR_64M  = (14U << PRCM_DEV_CLK_FACTOR_SHIFT),
	PRCM_DEV_CLK_FACTOR_60M  = (15U << PRCM_DEV_CLK_FACTOR_SHIFT),
} PRCM_DevClkFactor;

/*
 * bit field definition of PRCM->AUD_PLL_BIAS
 */
#define PRCM_AUD_PLL_VCO_BIAS_SHIFT	24	/* R/W */
#define PRCM_AUD_PLL_VCO_BIAS_MASK	(0x1F << PRCM_AUD_PLL_VCO_BIAS_SHIFT)

#define PRCM_AUD_PLL_CUR_BIAS_SHIFT	16	/* R/W */
#define PRCM_AUD_PLL_CUR_BIAS_MASK	(0x1F << PRCM_AUD_PLL_CUR_BIAS_SHIFT)

/*
 * bit field definition of PRCM->AUD_PLL_PAT_CTRL
 */
#define PRCM_AUD_DIG_DELT_PAT_EN_BIT	HAL_BIT(31)	/* R/W */

#define PRCM_AUD_PLL_PAT_PARAM_SHIFT	0
#define PRCM_AUD_PLL_PAT_PARAM_MASK		0x7FFFFFFFU
typedef enum {
	PRCM_AUD_PLL24M_PAT_PARAM_HOSC13M	= 0x00000000U,
	PRCM_AUD_PLL24M_PAT_PARAM_HOSC19M2	= 0x00000000U,
	PRCM_AUD_PLL24M_PAT_PARAM_HOSC24M	= 0x00000000U,
	PRCM_AUD_PLL24M_PAT_PARAM_HOSC26M	= 0x00000000U,
	PRCM_AUD_PLL24M_PAT_PARAM_HOSC40M	= 0x00000000U,
	PRCM_AUD_PLL24M_PAT_PARAM_HOSC50M	= 0x00000000U,
	PRCM_AUD_PLL24M_PAT_PARAM_HOSC52M	= 0xc0011FAAU,
	PRCM_AUD_PLL22M_PAT_PARAM_HOSC13M	= 0xC0019468U,
	PRCM_AUD_PLL22M_PAT_PARAM_HOSC19M2	= 0x00000000U,
	PRCM_AUD_PLL22M_PAT_PARAM_HOSC24M	= 0x00000000U,
	PRCM_AUD_PLL22M_PAT_PARAM_HOSC26M	= 0x00000000U,
	PRCM_AUD_PLL22M_PAT_PARAM_HOSC40M	= 0x00000000U,
	PRCM_AUD_PLL22M_PAT_PARAM_HOSC50M	= 0x00000000U,
	PRCM_AUD_PLL22M_PAT_PARAM_HOSC52M	= 0x00000000U
} PRCM_AudPLLPatParam;

/*
 * bit field definition of PRCM->DCXO_CTRL
 */
#define PRCM_DCXO_EN_BIT				HAL_BIT(31)	/* R/W */

/*
 * bit field definition of PRCM->SYS1_CTRL
 */
#define PRCM_CPUA_RESET_BIT		HAL_BIT(1)	/* R */
#define PRCM_SYS1_RESET_BIT		HAL_BIT(0)	/* R */

/*
 * bit field definition of PRCM->SYS1_STATUS
 */
#define PRCM_CPUN_DEEPSLEEP_LOCK_BIT	HAL_BIT(3)	/* R/W */
#define PRCM_CPUA_SLEEP_STATUS_BIT		HAL_BIT(2)	/* R */
#define PRCM_CPUA_DEEPSLEEP_STATUS_BIT	HAL_BIT(1)	/* R */
#define PRCM_SYS1_ALIVE_BIT				HAL_BIT(0)	/* R */

/*
 * bit field definition of PRCM->SYS2_CTRL
 */
#define PRCM_SYS2_ISOLATION_EN_BIT	HAL_BIT(2)	/* R/W */
#define PRCM_CPUN_RESET_BIT			HAL_BIT(1)	/* R/W */
#define PRCM_SYS2_RESET_BIT			HAL_BIT(0)	/* R/W */

/*
 * bit field definition of PRCM->SYS2_STATUS
 */
#define PRCM_CPUA_DEEPSLEEP_LOCK_BIT	HAL_BIT(3)	/* R */
#define PRCM_CPUN_SLEEP_STATUS_BIT		HAL_BIT(2)	/* R */
#define PRCM_CPUN_DEEPSLEEP_STATUS_BIT	HAL_BIT(1)	/* R */
#define PRCM_SYS2_ALIVE_BIT				HAL_BIT(0)	/* R */

/*
 * bit field definition of PRCM->SYS3_CTRL
 */
#define PRCM_SYS3_RESET_BIT 	HAL_BIT(0)	/* R */

/*
 * bit field definition of PRCM->SYS3_STATUS
 */
#define PRCM_CPUW_SLEEP_STATUS_BIT		HAL_BIT(2)	/* R */
#define PRCM_CPUW_DEEPSLEEP_STATUS_BIT	HAL_BIT(1)	/* R */
#define PRCM_SYS3_ALIVE_BIT 			HAL_BIT(0)	/* R */

/*
 * bit field definition of
 *   - PRCM->SYS1_WAKEUP_CTRL (R/W)
 *   - PRCM->SYS2_WAKEUP_CTRL (R)
 *   - PRCM->SYS1_SLEEP_CTRL  (R/W)
 *   - PRCM->SYS2_SLEEP_CTRL  (R)
 */
#define PRCM_SYS_WS_PWR_VAL_KEEP	0U /* for all */
#define PRCM_SYS_WS_PWR_VAL_ON		1U /* for PRCM->SYSx_WAKEUP_CTRL */
#define PRCM_SYS_WS_PWR_VAL_OFF 	1U /* for PRCM->SYSx_SLEEP_CTRL */

#define PRCM_SYS_WS_PWR_TYPE_LDO1			0U
#define PRCM_SYS_WS_PWR_TYPE_SYS1_PWR1 		1U
#define PRCM_SYS_WS_PWR_TYPE_SYS1_PWR2 		2U
#define PRCM_SYS_WS_PWR_TYPE_SYS1_SRAM_PWR1	3U
#define PRCM_SYS_WS_PWR_TYPE_SYS2_SRAM_PWR2	4U
#define PRCM_SYS_WS_PWR_TYPE_SYS2_PWR3		5U

#define PRCM_SYS_WS_PWR_FLAG(val, type)		((val) << (type))
#define PRCM_SYS_WS_PWR_FLAGS_MASK			0x3FU

/* DCXO_STABLE_REF_TIME */

/* DPLL_STABLE_REF_TIME */

/* LDO_STABLE_REF_TIME */

/* DIG_SWITCH_REF_TIME */

/*
 * bit field definition of PRCM->SRAM_VOLT_CTRL
 */
typedef enum {
	PRCM_SRAM_VOLT_1V10	= 0U,
	PRCM_SRAM_VOLT_1V00	= 1U,
	PRCM_SRAM_VOLT_0V90	= 2U,
	PRCM_SRAM_VOLT_0V80	= 3U,
	PRCM_SRAM_VOLT_0V75	= 4U,
	PRCM_SRAM_VOLT_0V70	= 5U,
	PRCM_SRAM_VOLT_0V65	= 6U,
	PRCM_SRAM_VOLT_0V60	= 7U
} PRCM_SRAMVolt;

#define PRCM_SRAM_WORK_VOLT_SHIFT	16	/* R/W */
#define PRCM_SRAM_WORK_VOLT_MASK	(0x7U << PRCM_SRAM_WORK_VOLT_SHIFT)

#define PRCM_SRAM_RETEN_VOLT_SHIFT	0	/* R/W */
#define PRCM_SRAM_RETEN_VOLT_MASK	(0x7U << PRCM_SRAM_RETEN_VOLT_SHIFT)

/* BANDGAP_STABLE_REF_TIME */
#define PRCM_BANDGAP_STABLE_REF_TIME_MASK       0x0FU

/* DCDC_STABLE_REF_TIME */
#define PRCM_LDOTOPWM_STABLE_RFE_TIME_SHIFT     16	/* R/W */
#define PRCM_LDOTOPWM_STABLE_RFE_TIME_MASK      0x3FFU
#define PRCM_DCDC_STABLE_REF_TIME_MASK          0x3FU

/*
 * bit field definition of PRCM->CPUA_BOOT_FLAG
 */
#define PRCM_CPUA_BOOT_FLAG_WR_LOCK		(0x429BU << 16)	/* W */

#define PRCM_CPUA_BOOT_FLAG_SHIFT		0	/* R/W */
#define PRCM_CPUA_BOOT_FLAG_MASK		(0xFU << PRCM_CPUA_BOOT_FLAG_SHIFT)
typedef enum {
	PRCM_CPUA_BOOT_FROM_COLD_RESET	= (0U << PRCM_CPUA_BOOT_FLAG_SHIFT),
	PRCM_CPUA_BOOT_FROM_DEEPSLEEP	= (1U << PRCM_CPUA_BOOT_FLAG_SHIFT),
	PRCM_CPUA_BOOT_FROM_SYS_UPDATE	= (2U << PRCM_CPUA_BOOT_FLAG_SHIFT)
} PRCM_CPUABootFlag;

/* PRCM->CPUA_BOOT_ADDR */

/* PRCM->CPUA_BOOT_ARG */

/*
 * bit field definition of PRCM->CPUN_BOOT_FLAG
 */
#define PRCM_CPUN_BOOT_FLAG_WR_LOCK		(0x429CU << 16)	/* R */

#define PRCM_CPUN_BOOT_FLAG_SHIFT		0	/* R */
#define PRCM_CPUN_BOOT_FLAG_MASK		(0xFU << PRCM_CPUN_BOOT_FLAG_SHIFT)
typedef enum {
	PRCM_CPUN_BOOT_FROM_COLD_RESET = (0 << PRCM_CPUN_BOOT_FLAG_SHIFT),
	PRCM_CPUN_BOOT_FROM_DEEPSLEEP  = (1 << PRCM_CPUN_BOOT_FLAG_SHIFT)
} PRCM_CPUNBootFlag;

/* PRCM->CPUN_BOOT_ADDR */

/* PRCM->CPUN_BOOT_ARG */

/* PRCM->CPUA_PRIV_REG */

/*
 * bit field definition of
 *   - PRCM->CPUA_WAKE_TIMER_CNT
 *   - PRCM->CPUN_WAKE_TIMER_CNT
 */
#define PRCM_CPUx_WAKE_TIMER_EN_BIT			HAL_BIT(31)	/* R/W */
#define PRCM_CPUx_WAKE_TIMER_CUR_VAL_MASK	0x7FFFFFFFU	/* R */

/*
 * bit field definition of
 *   - PRCM->CPUA_WAKE_TIMER_CMP
 *   - PRCM->CPUN_WAKE_TIMER_CMP
 */
#define PRCM_CPUx_WAKE_TIMER_PENDING_BIT		HAL_BIT(31)	/* R/W */
#define PRCM_CPUx_WAKE_TIMER_CMP_VAL_MASK	0x7FFFFFFFU	/* R/W */

/*
 * bit field definition of
 *   - PRCM->CPUA_WAKE_IO_EN
 *   - PRCM->CPUA_WAKE_IO_MODE
 *   - PRCM->CPUA_WAKE_IO_STATUS
 *   - PRCM->CPUA_WAKE_IO_HOLD
 */

#define PRCM_WAKE_IO_MASK	(0x3FFU)
typedef enum {
	PRCM_WAKE_IO_0 = HAL_BIT(0),
	PRCM_WAKE_IO_1 = HAL_BIT(1),
	PRCM_WAKE_IO_2 = HAL_BIT(2),
	PRCM_WAKE_IO_3 = HAL_BIT(3),
	PRCM_WAKE_IO_4 = HAL_BIT(4),
	PRCM_WAKE_IO_5 = HAL_BIT(5),
	PRCM_WAKE_IO_6 = HAL_BIT(6),
	PRCM_WAKE_IO_7 = HAL_BIT(7),
	PRCM_WAKE_IO_8 = HAL_BIT(8),
	PRCM_WAKE_IO_9 = HAL_BIT(9)
} PRCM_WakeIO;

typedef enum {
	PRCM_WAKE_IO_EVT_FALLING_EDGE = 0U,
	PRCM_WAKE_IO_EVT_RISING_EDGE  = 1U,
} PRCM_WakeIOEvent;

/*
 * bit field definition of PRCM->CPUA_WAKE_IO_GLOBAL_EN
 */
#define PRCM_WAKE_IO_GLOBAL_EN_BIT			HAL_BIT(0)

/*
 * bit field definition of PRCM->CPUA_PRCM_REG
 */
#define PRCM_CPUA_PRCM_REG_BIT				HAL_BIT(0)

/* DCDC_PARAM_CTRL */
#define PRCM_DCDC_BANDGAP_TRIM_SHIFT	25
#define PRCM_DCDC_BANDGAP_TRIM_MASK		(0xFU << PRCM_DCDC_BANDGAP_TRIM_SHIFT)

/* ANA_BANDGAP */

/* CLK_LDO_PARAM */

/* DIG_LDO_PARAM */
#define PRCM_DIG_LDO_BANDGAP_TRIM_SHIFT	8
#define PRCM_DIG_LDO_BANDGAP_TRIM_MASK	(0xFU << PRCM_DIG_LDO_BANDGAP_TRIM_SHIFT)

/* DPLL_STATUS */

/******************************************************************************/
void HAL_PRCM_SetDCDCVoltage(PRCM_DCDCVolt volt);
uint32_t HAL_PRCM_GetSysPowerEnableFlags(void);
void HAL_PRCM_EnableSys2Power(void);
void HAL_PRCM_DisableSys2Power(void);
void HAL_PRCM_SetLDO1Voltage(PRCM_LDO1Volt volt);
uint32_t HAL_PRCM_GetLDOEnableFlags(void);

void HAL_PRCM_SetLFCLKSource(PRCM_LFCLKSrc src);
void HAL_PRCM_SetHOSCType(PRCM_HOSCType type);
uint32_t HAL_PRCM_GetHOSCType(void);
uint32_t HAL_PRCM_GetHFClock(void);
uint32_t HAL_PRCM_GetInter32KFreq(void);
uint32_t HAL_PRCM_EnableInter32KCalib(void);
uint32_t HAL_PRCM_DisableInter32KCalib(void);
#if 0
void HAL_PRCM_SetSysPLLParam(PRCM_SysPLLParam param);
void HAL_PRCM_EnableSysPLL(void);
#else
void HAL_PRCM_SetSysPLL(PRCM_SysPLLParam param);
#endif
void HAL_PRCM_DisableSysPLL(void);
void HAL_PRCM_SetCPUAClk(PRCM_CPUClkSrc src, PRCM_SysClkFactor factor);
void HAL_PRCM_DisCLK1(PRCM_SysClkFactor factor);
uint32_t HAL_PRCM_GetCPUAClk(void);
void HAL_PRCM_SetAudioPLLParam(PRCM_AudPLLParam param);
void HAL_PRCM_EnableAudioPLL(void);
void HAL_PRCM_DisableAudioPLL(void);
void HAL_PRCM_SetDevClock(PRCM_DevClkFactor factor);
uint32_t HAL_PRCM_GetDevClock(void);
void HAL_PRCM_SetAudioPLLPatternParam(PRCM_AudPLLPatParam param);
void HAL_PRCM_EnableAudioPLLPattern(void);
void HAL_PRCM_DisableAudioPLLPattern(void);

int HAL_PRCM_IsCPUAResetRelease(void);
int HAL_PRCM_IsSys1ResetRelease(void);
void HAL_PRCM_AllowCPUNDeepSleep(void);
void HAL_PRCM_DisallowCPUNDeepSleep(void);
int HAL_PRCM_IsCPUNDeepSleepAllowed(void);
int HAL_PRCM_IsCPUASleep(void);
int HAL_PRCM_IsCPUADeepSleep(void);
int HAL_PRCM_IsSys1Alive(void);

void HAL_PRCM_DisableSys2(void);
void HAL_PRCM_EnableSys2Isolation(void);
void HAL_PRCM_DisableSys2Isolation(void);
void HAL_PRCM_ForceCPUNReset(void);
void HAL_PRCM_ReleaseCPUNReset(void);
int HAL_PRCM_IsCPUNReleased(void);
void HAL_PRCM_ForceSys2Reset(void);
void HAL_PRCM_ReleaseSys2Reset(void);

int HAL_PRCM_IsCPUADeepSleepAllowed(void);
int HAL_PRCM_IsCPUNSleep(void);
int HAL_PRCM_IsCPUNDeepSleep(void);
int HAL_PRCM_IsSys2Alive(void);
int HAL_PRCM_IsSys3Alive(void);
void HAL_PRCM_SetSys1WakeupPowerFlags(uint32_t flags);
uint32_t HAL_PRCM_GetSys2WakeupPowerFlags(void);
uint32_t HAL_PRCM_GetSys1SleepPowerFlags(void);
void HAL_PRCM_SetSys1SleepPowerFlags(uint32_t flags);
uint32_t HAL_PRCM_GetSys2SleepPowerFlags(void);
void HAL_PRCM_SetSRAMVoltage(PRCM_SRAMVolt workVolt, PRCM_SRAMVolt retenVolt);
void HAL_PRCM_SetBANDGAPSTABLE_TIME(uint32_t time);
void HAL_PRCM_SetDCDCSTABLE_TIME(uint32_t time);
uint32_t HAL_PRCM_GetBANDGAPSTABLE_TIME(void);
uint32_t HAL_PRCM_GetDCDCSTABLE_TIME(void);
void HAL_PRCM_SetCPUABootFlag(PRCM_CPUABootFlag flag);
uint32_t HAL_PRCM_GetCPUABootFlag(void);
void HAL_PRCM_SetCPUABootAddr(uint32_t addr);
uint32_t HAL_PRCM_GetCPUABootAddr(void);
void HAL_PRCM_SetCPUABootArg(uint32_t arg);
uint32_t HAL_PRCM_GetCPUABootArg(void);
void HAL_PRCM_SetCPUAPrivateData(uint32_t data);
uint32_t HAL_PRCM_GetCPUAPrivateData(void);

uint32_t HAL_PRCM_GetWakeupTimerEnable(void);
void HAL_PRCM_WakeupTimerEnable(void);
void HAL_PRCM_WakeupTimerDisable(void);
uint32_t HAL_PRCM_WakeupTimerGetCurrentValue(void);
uint32_t HAL_PRCM_GetWakeupTimerPending(void);
void HAL_PRCM_ClearWakeupTimerPending(void);
void HAL_PRCM_WakeupTimerSetCompareValue(uint32_t val);
uint32_t HAL_PRCM_WakeupTimerGetCompareValue(void);

void HAL_PRCM_WakeupIOEnable(uint32_t ioMask);
void HAL_PRCM_WakeupIODisable(uint32_t ioMask);
void HAL_PRCM_WakeupIOSetRisingEvent(uint32_t ioMask);
void HAL_PRCM_WakeupIOSetFallingEvent(uint32_t ioMask);
uint32_t HAL_PRCM_WakeupIOGetEventStatus(void);
int HAL_PRCM_WakeupIOIsEventDetected(uint32_t ioMask);
void HAL_PRCM_WakeupIOClearEventDetected(uint32_t ioMask);
void HAL_PRCM_WakeupIOEnableCfgHold(uint32_t ioMask);
void HAL_PRCM_WakeupIODisableCfgHold(uint32_t ioMask);
void HAL_PRCM_WakeupIOEnableGlobal(void);
void HAL_PRCM_WakeupIODisableGlobal(void);
void HAL_PRCM_Start(void);

#ifdef __cplusplus
}
#endif

#endif /* _DRIVER_CHIP_HAL_PRCM_H_ */
