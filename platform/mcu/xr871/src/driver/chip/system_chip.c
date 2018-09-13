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
#include "driver/chip/system_chip.h"
#include "pm/pm.h"

uint32_t SystemCoreClock;
extern const unsigned char __RAM_BASE[];	/* SRAM start address */

/**
 * @brief Initialize the chip system, including power, FPU setting, vector
 *        table location, clock, etc.
 * @return None
 */
void SystemInit(void)
{
#ifdef __CONFIG_CHIP_XR871
	HAL_PRCM_SetDCDCVoltage(PRCM_DCDC_VOLT_1V51);
	HAL_PRCM_SetSRAMVoltage(PRCM_SRAM_VOLT_1V10, PRCM_SRAM_VOLT_0V70);
#if 0
	/* overwite efuse values */
	HAL_MODIFY_REG(PRCM->DCDC_PARAM_CTRL,
	               PRCM_DCDC_BANDGAP_TRIM_MASK,
	               1U << PRCM_DCDC_BANDGAP_TRIM_SHIFT);
	HAL_MODIFY_REG(PRCM->DIG_LDO_PARAM,
	               PRCM_DIG_LDO_BANDGAP_TRIM_MASK,
	               2U << PRCM_DIG_LDO_BANDGAP_TRIM_SHIFT);
#endif
#endif

	SCB->VTOR = (uint32_t)__RAM_BASE; /* Vector Table Relocation in Internal SRAM. */

#if ((__FPU_PRESENT == 1) && (__FPU_USED == 1))
	/* FPU settings, set CP10 and CP11 Full Access */
	SCB->CPACR |= ((3UL << 20) | (3UL << 22));
#endif

	/* set clock */
	HAL_BoardIoctl(HAL_BIR_CHIP_CLOCK_INIT, 0, 0);

	SystemCoreClock = HAL_GetCPUClock();
	pm_init();
	HAL_NVIC_Init();
	HAL_CCM_Init();
}

/**
 * @brief DeInitialize the chip system by disabling and cleaning the system IRQ
 * @param flag Rest system clock if bit SYSTEM_DEINIT_FLAG_RESET_CLK is set in
 *             the flag
 * @return None
 */
void SystemDeInit(uint32_t flag)
{
	/* disable irq0~63 */
	NVIC->ICER[0] = NVIC->ISER[0];
	NVIC->ICER[1] = NVIC->ISER[1];

	/* clear irq pending0~63 */
	NVIC->ICPR[0] = NVIC->ISPR[0];
	NVIC->ICPR[1] = NVIC->ISPR[1];

	/* disable systick irq */
	SysTick->CTRL = 0;

	/* clear pendsv irq */
	SCB->ICSR = (SCB->ICSR & SCB_ICSR_PENDSVSET_Msk) >> (SCB_ICSR_PENDSVSET_Pos - SCB_ICSR_PENDSVCLR_Pos);

	/* clear systick irq */
	SCB->ICSR = (SCB->ICSR & SCB_ICSR_PENDSTSET_Msk) >> (SCB_ICSR_PENDSTSET_Pos - SCB_ICSR_PENDSTCLR_Pos);

	if (flag & SYSTEM_DEINIT_FLAG_RESET_CLK) {
		/* reset clock for restart */
		HAL_PRCM_SetCPUAClk(PRCM_CPU_CLK_SRC_HFCLK, PRCM_SYS_CLK_FACTOR_80M);
		HAL_PRCM_DisCLK1(PRCM_SYS_CLK_FACTOR_80M);
		HAL_CCM_BusSetClock(CCM_AHB2_CLK_DIV_1, CCM_APB_CLK_SRC_HFCLK, CCM_APB_CLK_DIV_2);
		HAL_PRCM_DisableSysPLL();
	}
}

/**
 * @brief Update system core (cpu) clock
 * @return None
 */
void SystemCoreClockUpdate(void)
{
	SystemCoreClock = HAL_GetCPUClock();
}
