/**
  * @file  hal_ccmc
  * @author  XRADIO IOT WLAN Team
  */

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

#include "hal_base.h"
#include "pm/pm.h"
#include "sys/xr_debug.h"

/**
 * @brief Configure AHB2 and APB clock
 * @param[in] AHB2Div AHB2 clock divider
 * @param[in] APBSrc APB clock source
 * @param[in] APBDiv APB clock divider
 * @return None
 */
void HAL_CCM_BusSetClock(CCM_AHB2ClkDiv AHB2Div, CCM_APBClkSrc APBSrc, CCM_APBClkDiv APBDiv)
{
	CCM->CPU_BUS_CLKCFG = AHB2Div | APBSrc | APBDiv;
}

/**
 * @brief Get AHB1 clock
 * @return AHB1 clock in Hz
 */
uint32_t HAL_CCM_BusGetAHB1Clock(void)
{
	return HAL_PRCM_GetCPUAClk();
}

/**
 * @brief Get AHB2 clock
 * @return AHB2 clock in Hz
 */
uint32_t HAL_CCM_BusGetAHB2Clock(void)
{
	uint32_t div = ((CCM->CPU_BUS_CLKCFG & CCM_AHB2_CLK_DIV_MASK) >> CCM_AHB2_CLK_DIV_SHIFT) + 1;
	return HAL_PRCM_GetCPUAClk() / div;
}

/**
 * @brief Get APB clock
 * @return APB clock in Hz
 */
uint32_t HAL_CCM_BusGetAPBClock(void)
{
	uint32_t reg = CCM->CPU_BUS_CLKCFG;
	uint32_t freq, div;

	switch (reg & CCM_APB_CLK_SRC_MASK) {
	case CCM_APB_CLK_SRC_HFCLK:
		freq = HAL_GetHFClock();
		break;
	case CCM_APB_CLK_SRC_LFCLK:
		freq = HAL_GetLFClock();
		break;
	case CCM_APB_CLK_SRC_AHB2CLK:
	default:
		freq = HAL_GetAHB2Clock();
		break;
	}

	div = ((reg & CCM_APB_CLK_DIV_MASK) >> CCM_APB_CLK_DIV_SHIFT);
	return (freq >> div);
}

/**
 * @brief Enable peripheral clock
 * @param[in] periphMask Bitmask of peripherals, refer to CCM_BusPeriphBit
 * @return None
 */
void HAL_CCM_BusEnablePeriphClock(uint32_t periphMask)
{
	HAL_SET_BIT(CCM->BUS_PERIPH_CLK_CTRL, periphMask);
}

/**
 * @brief Disable peripheral clock
 * @param[in] periphMask Bitmask of peripherals, refer to CCM_BusPeriphBit
 * @return None
 */
void HAL_CCM_BusDisablePeriphClock(uint32_t periphMask)
{
	HAL_CLR_BIT(CCM->BUS_PERIPH_CLK_CTRL, periphMask);
}

/**
 * @brief Disable all peripherals clock
 * @return None
 */
void HAL_CCM_BusDisableAllPeriphClock(void)
{
	CCM->BUS_PERIPH_CLK_CTRL = 0U;
}

/**
 * @brief Force peripheral reset
 * @param[in] periphMask Bitmask of peripherals, refer to CCM_BusPeriphBit
 * @return None
 */
void HAL_CCM_BusForcePeriphReset(uint32_t periphMask)
{
	HAL_CLR_BIT(CCM->BUS_PERIPH_RST_CTRL, periphMask);
}

/**
 * @brief Release peripheral reset
 * @param[in] periphMask Bitmask of peripherals, refer to CCM_BusPeriphBit
 * @return None
 */
void HAL_CCM_BusReleasePeriphReset(uint32_t periphMask)
{
	HAL_SET_BIT(CCM->BUS_PERIPH_RST_CTRL, periphMask);
}

/**
 * @brief Force all peripherals reset
 * @return None
 */
void HAL_CCM_BusForceAllPeriphReset(void)
{
	CCM->BUS_PERIPH_RST_CTRL = 0U;
}

/**
 * @brief Configure SPI0 module clock
 * @param[in] src SPI0 module clock source
 * @param[in] divN SPI0 module clock divider N
 * @param[in] divM SPI0 module clock divider M
 * @return None
 */
void HAL_CCM_SPI0_SetMClock(CCM_AHBPeriphClkSrc src, CCM_PeriphClkDivN divN, CCM_PeriphClkDivM divM)
{
	HAL_MODIFY_REG(CCM->SPI0_MCLK_CTRL, CCM_PERIPH_CLK_PARAM_MASK, src | divN | divM);
}

/**
 * @brief Enable SPI0 module clock
 * @return None
 */
void HAL_CCM_SPI0_EnableMClock(void)
{
	HAL_SET_BIT(CCM->SPI0_MCLK_CTRL, CCM_PERIPH_CLK_EN_BIT);
}

/**
 * @brief Disable SPI0 module clock
 * @return None
 */
void HAL_CCM_SPI0_DisableMClock(void)
{
	HAL_CLR_BIT(CCM->SPI0_MCLK_CTRL, CCM_PERIPH_CLK_EN_BIT);
}

/**
 * @brief Configure SPI1 module clock
 * @param[in] src SPI1 module clock source
 * @param[in] divN SPI1 module clock divider N
 * @param[in] divM SPI1 module clock divider M
 * @return None
 */
void HAL_CCM_SPI1_SetMClock(CCM_AHBPeriphClkSrc src, CCM_PeriphClkDivN divN, CCM_PeriphClkDivM divM)
{
	HAL_MODIFY_REG(CCM->SPI1_MCLK_CTRL, CCM_PERIPH_CLK_PARAM_MASK, src | divN | divM);
}

/**
 * @brief Enable SPI1 module clock
 * @return None
 */
void HAL_CCM_SPI1_EnableMClock(void)
{
	HAL_SET_BIT(CCM->SPI1_MCLK_CTRL, CCM_PERIPH_CLK_EN_BIT);
}

/**
 * @brief Disable SPI1 module clock
 * @return None
 */
void HAL_CCM_SPI1_DisableMClock(void)
{
	HAL_CLR_BIT(CCM->SPI1_MCLK_CTRL, CCM_PERIPH_CLK_EN_BIT);
}

/**
 * @brief Configure SD controller module clock
 * @param[in] src SD controller module clock source
 * @param[in] divN SD controller module clock divider N
 * @param[in] divM SD controller module clock divider M
 * @return None
 */
void HAL_CCM_SDC_SetMClock(CCM_AHBPeriphClkSrc src, CCM_PeriphClkDivN divN, CCM_PeriphClkDivM divM)
{
	HAL_MODIFY_REG(CCM->SDC_MCLK_CTRL, CCM_PERIPH_CLK_PARAM_MASK, src | divN | divM);
}

/**
 * @brief Enable SD controller module clock
 * @return None
 */
void HAL_CCM_SDC_EnableMClock(void)
{
	HAL_SET_BIT(CCM->SDC_MCLK_CTRL, CCM_PERIPH_CLK_EN_BIT);
}

/**
 * @brief Disable SD controller module clock
 * @return None
 */
void HAL_CCM_SDC_DisableMClock(void)
{
	HAL_CLR_BIT(CCM->SDC_MCLK_CTRL, CCM_PERIPH_CLK_EN_BIT);
}

/**
 * @brief Configure Crypto Engine module clock
 * @param[in] src Crypto Engine module clock source
 * @param[in] divN Crypto Engine module clock divider N
 * @param[in] divM Crypto Engine module clock divider M
 * @return None
 */
void HAL_CCM_CE_SetMClock(CCM_AHBPeriphClkSrc src, CCM_PeriphClkDivN divN, CCM_PeriphClkDivM divM)
{
	HAL_MODIFY_REG(CCM->CE_MCLK_CTRL, CCM_PERIPH_CLK_PARAM_MASK, src | divN | divM);
}

/**
 * @brief Enable Crypto Engine module clock
 * @return None
 */
void HAL_CCM_CE_EnableMClock(void)
{
	HAL_SET_BIT(CCM->CE_MCLK_CTRL, CCM_PERIPH_CLK_EN_BIT);
}

/**
 * @brief Disable Crypto Engine module clock
 * @return None
 */
void HAL_CCM_CE_DisableMClock(void)
{
	HAL_CLR_BIT(CCM->CE_MCLK_CTRL, CCM_PERIPH_CLK_EN_BIT);
}

/**
 * @brief Configure CSI output clock
 * @param[in] src CSI output clock source
 * @param[in] divN CSI output clock divider N
 * @param[in] divM CSI output clock divider M
 * @return None
 */
void HAL_CCM_CSI_SetOutClock(CCM_AHBPeriphClkSrc src, CCM_PeriphClkDivN divN, CCM_PeriphClkDivM divM)
{
	HAL_MODIFY_REG(CCM->CSI_OCLK_CTRL, CCM_PERIPH_CLK_PARAM_MASK, src | divN | divM);
}

/**
 * @brief Enable CSI output clock
 * @return None
 */
void HAL_CCM_CSI_EnableOutClock(void)
{
	HAL_SET_BIT(CCM->CSI_OCLK_CTRL, CCM_PERIPH_CLK_EN_BIT);
}

/**
 * @brief Disable CSI output clock
 * @return None
 */
void HAL_CCM_CSI_DisableOutClock(void)
{
	HAL_CLR_BIT(CCM->CSI_OCLK_CTRL, CCM_PERIPH_CLK_EN_BIT);
}

/**
 * @brief Configure Digital audio module clock
 * @param[in] src Digital audio module source
 * @return None
 */
void HAL_CCM_DAUDIO_SetMClock(CCM_DAudioClkSrc src)
{
	HAL_MODIFY_REG(CCM->DAUDIO_MCLK_CTRL, CCM_DAUDIO_MCLK_SRC_MASK, src);
}

/**
 * @brief Enable Digital audio module clock
 * @return None
 */
void HAL_CCM_DAUDIO_EnableMClock(void)
{
	HAL_SET_BIT(CCM->DAUDIO_MCLK_CTRL, CCM_PERIPH_CLK_EN_BIT);
}

/**
 * @brief Disable Digital audio module clock
 * @return None
 */
void HAL_CCM_DAUDIO_DisableMClock(void)
{
	HAL_CLR_BIT(CCM->DAUDIO_MCLK_CTRL, CCM_PERIPH_CLK_EN_BIT);
}

/**
 * @brief Configure IR RX module clock
 * @param[in] src IR RX module clock source
 * @param[in] divN IR RX module clock divider N
 * @param[in] divM IR RX module clock divider M
 * @return None
 */
void HAL_CCM_IRRX_SetMClock(CCM_APBPeriphClkSrc src, CCM_PeriphClkDivN divN, CCM_PeriphClkDivM divM)
{
	HAL_MODIFY_REG(CCM->IRRX_MCLK_CTRL, CCM_PERIPH_CLK_PARAM_MASK, src | divN | divM);
}

/**
 * @brief Enable IR RX module clock
 * @return None
 */
void HAL_CCM_IRRX_EnableMClock(void)
{
	HAL_SET_BIT(CCM->IRRX_MCLK_CTRL, CCM_PERIPH_CLK_EN_BIT);
}

/**
 * @brief Disable IR RX module clock
 * @return None
 */
void HAL_CCM_IRRX_DisableMClock(void)
{
	HAL_CLR_BIT(CCM->IRRX_MCLK_CTRL, CCM_PERIPH_CLK_EN_BIT);
}

/**
 * @brief Configure IR TX module clock
 * @param[in] src IR TX module clock source
 * @param[in] divN IR TX module clock divider N
 * @param[in] divM IR TX module clock divider M
 * @return None
 */
void HAL_CCM_IRTX_SetMClock(CCM_APBPeriphClkSrc src, CCM_PeriphClkDivN divN, CCM_PeriphClkDivM divM)
{
	HAL_MODIFY_REG(CCM->IRTX_MCLK_CTRL, CCM_PERIPH_CLK_PARAM_MASK, src | divN | divM);
}

/**
 * @brief Enable IR TX module clock
 * @return None
 */
void HAL_CCM_IRTX_EnableMClock(void)
{
	HAL_SET_BIT(CCM->IRTX_MCLK_CTRL, CCM_PERIPH_CLK_EN_BIT);
}

/**
 * @brief Disable IR TX module clock
 * @return None
 */
void HAL_CCM_IRTX_DisableMClock(void)
{
	HAL_CLR_BIT(CCM->IRTX_MCLK_CTRL, CCM_PERIPH_CLK_EN_BIT);
}

/**
 * @brief Configure system tick reference clock
 * @param[in] src system tick reference clock source
 * @param[in] divN system tick reference clock divider N
 * @param[in] divM system tick reference clock divider M
 * @return None
 */
void HAL_CCM_SYSTICK_SetRefClock(CCM_APBPeriphClkSrc src, CCM_PeriphClkDivN divN, CCM_PeriphClkDivM divM)
{
	HAL_MODIFY_REG(CCM->SYSTICK_REFCLK_CTRL, CCM_PERIPH_CLK_PARAM_MASK, src | divN | divM);
}

/**
 * @brief Enable system tick reference clock
 * @return None
 */
void HAL_CCM_SYSTICK_EnableRefClock(void)
{
	HAL_SET_BIT(CCM->SYSTICK_REFCLK_CTRL, CCM_PERIPH_CLK_EN_BIT);
}

/**
 * @brief Disable system tick reference clock
 * @return None
 */
void HAL_CCM_SYSTICK_DisableRefClock(void)
{
	HAL_CLR_BIT(CCM->SYSTICK_REFCLK_CTRL, CCM_PERIPH_CLK_EN_BIT);
}

/**
 * @brief Enable system tick reference
 * @return None
 */
void HAL_CCM_SYSTICK_EnableRef(void)
{
	HAL_CLR_BIT(CCM->SYSTICK_CALIB_CTRL, CCM_SYSTICK_NOREF_BIT);
}

/**
 * @brief Disable system tick reference
 * @return None
 */
void HAL_CCM_SYSTICK_DisableRef(void)
{
	HAL_SET_BIT(CCM->SYSTICK_CALIB_CTRL, CCM_SYSTICK_NOREF_BIT);
}

/**
 * @brief Enable system tick skew, which means calibration value is not exactly 10 ms
 * @return None
 */
void HAL_CCM_SYSTICK_EnableSkew(void)
{
	HAL_SET_BIT(CCM->SYSTICK_CALIB_CTRL, CCM_SYSTICK_SKEW_BIT);
}

/**
 * @brief Disable system tick skew, which means calibration value is accurate
 * @return None
 */
void HAL_CCM_SYSTICK_DisableSkew(void)
{
	HAL_CLR_BIT(CCM->SYSTICK_CALIB_CTRL, CCM_SYSTICK_SKEW_BIT);
}

/**
 * @brief Set calibration value for 10 ms
 * @return None
 */
void HAL_CCM_SYSTICK_SetTENMS(uint32_t cnt)
{
	HAL_MODIFY_REG(CCM->SYSTICK_CALIB_CTRL,
	               CCM_SYSTICK_TENMS_MASK,
	               (cnt << CCM_SYSTICK_TENMS_SHIFT) & CCM_SYSTICK_TENMS_MASK);
}

/**
 * @brief Enable DMIC module clock
 * @return None
 */
void HAL_CCM_DMIC_EnableMClock(void)
{
	HAL_SET_BIT(CCM->DMIC_MCLK_CTRL, CCM_PERIPH_CLK_EN_BIT);
}

/**
 * @brief Disable DMIC module clock
 * @return None
 */
void HAL_CCM_DMIC_DisableMClock(void)
{
	HAL_CLR_BIT(CCM->DMIC_MCLK_CTRL, CCM_PERIPH_CLK_EN_BIT);
}

/**
 * @brief Configure GPADC module clock
 * @param[in] src GPADC module clock source
 * @param[in] divN GPADC module clock divider N
 * @param[in] divM GPADC module clock divider M
 * @return None
 */
void HAL_CCM_GPADC_SetMClock(CCM_APBPeriphClkSrc src, CCM_PeriphClkDivN divN, CCM_PeriphClkDivM divM)
{
	HAL_MODIFY_REG(CCM->GPADC_MCLK_CTRL, CCM_PERIPH_CLK_PARAM_MASK, src | divN | divM);
}

/**
 * @brief Enable GPADC module clock
 * @return None
 */
void HAL_CCM_GPADC_EnableMClock(void)
{
	HAL_SET_BIT(CCM->GPADC_MCLK_CTRL, CCM_PERIPH_CLK_EN_BIT);
}

/**
 * @brief Disable GPADC module clock
 * @return None
 */
void HAL_CCM_GPADC_DisableMClock(void)
{
	HAL_CLR_BIT(CCM->GPADC_MCLK_CTRL, CCM_PERIPH_CLK_EN_BIT);
}

/**
 * @brief Configure CSI module clock
 * @param[in] src CSI module clock source
 * @param[in] divN CSI module clock divider N
 * @param[in] divM CSI module clock divider M
 * @return None
 */
void HAL_CCM_CSI_SetMClock(CCM_AHBPeriphClkSrc src, CCM_PeriphClkDivN divN, CCM_PeriphClkDivM divM)
{
	HAL_MODIFY_REG(CCM->CSI_MCLK_CTRL, CCM_PERIPH_CLK_PARAM_MASK, src | divN | divM);
}

/**
 * @brief Enable CSI module clock
 * @return None
 */
void HAL_CCM_CSI_EnableMClock(void)
{
	HAL_SET_BIT(CCM->CSI_MCLK_CTRL, CCM_PERIPH_CLK_EN_BIT);
}

/**
 * @brief Disable CSI module clock
 * @return None
 */
void HAL_CCM_CSI_DisableMClock(void)
{
	HAL_CLR_BIT(CCM->CSI_MCLK_CTRL, CCM_PERIPH_CLK_EN_BIT);
}

/**
 * @brief Configure Flash controller module clock
 * @param[in] src Flash controller module clock source
 * @param[in] divN Flash controller module clock divider N
 * @param[in] divM Flash controller module clock divider M
 * @return None
 */
void HAL_CCM_FLASHC_SetMClock(CCM_AHBPeriphClkSrc src, CCM_PeriphClkDivN divN, CCM_PeriphClkDivM divM)
{
	HAL_MODIFY_REG(CCM->FLASHC_MCLK_CTRL, CCM_PERIPH_CLK_PARAM_MASK, src | divN | divM);
}

/**
 * @brief Enable Flash controller module clock
 * @return None
 */
void HAL_CCM_FLASHC_EnableMClock(void)
{
	HAL_SET_BIT(CCM->FLASHC_MCLK_CTRL, CCM_PERIPH_CLK_EN_BIT);
}

/**
 * @brief Disable Flash controller module clock
 * @return None
 */
void HAL_CCM_FLASHC_DisableMClock(void)
{
	HAL_CLR_BIT(CCM->FLASHC_MCLK_CTRL, CCM_PERIPH_CLK_EN_BIT);
}

#ifdef CONFIG_PM
struct ccm_regs {
	uint32_t cpu_bus_clkcfg;        /* cpu bus clock configure register */
	uint32_t bus_clk_gating;        /* bus clock gating control Register */
	uint32_t bus_reset_ctrl;        /* bus device reset control register */
	uint32_t systick_refclk_ctrl;   /* system tick reference clock register */
	uint32_t systick_calib_ctrl;    /* system tick clock calibration register */
};
static struct ccm_regs ccm_reg_store;

static int ccm_suspend(struct soc_device *dev, enum suspend_state_t state)
{
	switch (state) {
	case PM_MODE_SLEEP:
		break;
	case PM_MODE_STANDBY:
	case PM_MODE_HIBERNATION:
		ccm_reg_store.bus_clk_gating = CCM->BUS_PERIPH_CLK_CTRL; /* it is better to do in every devices driver */
		ccm_reg_store.bus_reset_ctrl = CCM->BUS_PERIPH_RST_CTRL;

		ccm_reg_store.cpu_bus_clkcfg = CCM->CPU_BUS_CLKCFG;
		ccm_reg_store.systick_refclk_ctrl = CCM->SYSTICK_REFCLK_CTRL;
		ccm_reg_store.systick_calib_ctrl = CCM->SYSTICK_CALIB_CTRL;
		HAL_DBG("%s ok\n", __func__);
		break;
	default:
		break;
	}

	return 0;
}

static int ccm_resume(struct soc_device *dev, enum suspend_state_t state)
{
	switch (state) {
	case PM_MODE_SLEEP:
		break;
	case PM_MODE_STANDBY:
	case PM_MODE_HIBERNATION:
		CCM->CPU_BUS_CLKCFG = ccm_reg_store.cpu_bus_clkcfg; /* write directly is ok for cpu clk has been restored after resume */
		CCM->SYSTICK_REFCLK_CTRL = ccm_reg_store.systick_refclk_ctrl;
		CCM->SYSTICK_CALIB_CTRL = ccm_reg_store.systick_calib_ctrl;

		CCM->BUS_PERIPH_RST_CTRL = ccm_reg_store.bus_reset_ctrl;
		CCM->BUS_PERIPH_CLK_CTRL = ccm_reg_store.bus_clk_gating;
		__DSB();
		__ISB();
		HAL_DBG("%s ok\n", __func__);
		break;
	default:
		break;
	}

	return 0;
}

void ccm_print_regs(void)
{
	hex_dump_bytes(&ccm_reg_store, sizeof(ccm_reg_store));
}

static struct soc_device_driver ccm_drv = {
	.name = "ccm",
	.suspend_noirq = ccm_suspend,
	.resume_noirq = ccm_resume,
};

static struct soc_device ccm_dev = {
	.name = "ccm",
	.driver = &ccm_drv,
};

#define CCM_DEV (&ccm_dev)
#endif /* CONFIG_PM */

/**
 * @brief Initializes CCM module
 * @return None
 */
void HAL_CCM_Init(void)
{
#ifdef CONFIG_PM
	pm_register_ops(CCM_DEV);
#endif
}
