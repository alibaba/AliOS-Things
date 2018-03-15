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

#include "driver/chip/hal_prcm.h"
#include "hal_base.h"

/*
 * Power
 *   - DCDC
 *   - Power switch
 *   - LDO
 */
void HAL_PRCM_SetDCDCVoltage(PRCM_DCDCVolt volt)
{
	HAL_MODIFY_REG(PRCM->SYS_DCDC_CTRL, PRCM_DCDC_VOLT_MASK, volt);
}

uint32_t HAL_PRCM_GetSysPowerEnableFlags(void)
{
	return HAL_GET_BIT(PRCM->SYS_LDO_SW_CTRL,
					   PRCM_SYS1_PWR1_EN_BIT |
					   PRCM_SYS1_PWR2_EN_BIT |
					   PRCM_SYS2_PWR3_EN_BIT |
					   PRCM_SYS3_PWR4_EN_BIT |
					   PRCM_SYS3_PWR5_EN_BIT |
	  				   PRCM_SYS1_SRAM_PWR1_EN_BIT |
	  				   PRCM_SYS2_SRAM_PWR2_EN_BIT |
	  				   PRCM_SYS3_SRAM_PWR3_EN_BIT);
}

void HAL_PRCM_EnableSys2Power(void)
{
	HAL_SET_BIT(PRCM->SYS_LDO_SW_CTRL,
		        PRCM_SYS2_PWR3_EN_BIT | PRCM_SYS2_SRAM_PWR2_EN_BIT);
}

void HAL_PRCM_DisableSys2Power(void)
{
	HAL_CLR_BIT(PRCM->SYS_LDO_SW_CTRL,
		        PRCM_SYS2_PWR3_EN_BIT | PRCM_SYS2_SRAM_PWR2_EN_BIT);
}

void HAL_PRCM_SetLDO1Voltage(PRCM_LDO1Volt volt)
{
	HAL_MODIFY_REG(PRCM->SYS_LDO_SW_CTRL, PRCM_LDO1_VOLT_MASK, volt);
}

uint32_t HAL_PRCM_GetLDOEnableFlags(void)
{
	return HAL_GET_BIT(PRCM->SYS_LDO_SW_CTRL,
					   PRCM_LDO1_EN_BIT |
					   PRCM_PLL_LDO_EN_BIT |
					   PRCM_SRAM_LDO_EN_BIT);
}

/*
 * Clock
 */
void HAL_PRCM_SetLFCLKSource(PRCM_LFCLKSrc src)
{
	/* always enable inter 32K */
	uint32_t clr_mask = PRCM_LFCLK_SRC_MASK | PRCM_LFCLK_EXT32K_EN_BIT;
	uint32_t set_mask = src | PRCM_LFCLK_INTER32K_EN_BIT;
	if (src == PRCM_LFCLK_SRC_EXT32K) {
		set_mask |= PRCM_LFCLK_EXT32K_EN_BIT;
	}
	HAL_MODIFY_REG(PRCM->SYS_LFCLK_CTRL, clr_mask, set_mask);
}

void HAL_PRCM_SetHOSCType(PRCM_HOSCType type)
{
	HAL_MODIFY_REG(PRCM->SYS_HOSC_CTRL, PRCM_HOSC_TYPE_MASK, type);
}

uint32_t HAL_PRCM_GetHOSCType(void)
{
	return HAL_GET_BIT(PRCM->SYS_HOSC_CTRL, PRCM_HOSC_TYPE_MASK);
}

uint32_t HAL_PRCM_GetHFClock(void)
{
	static const uint32_t PRCM_HOSCClock[] =
		{ HOSC_CLOCK_26M, HOSC_CLOCK_40M, HOSC_CLOCK_24M, HOSC_CLOCK_52M };

	uint32_t val;

	val = HAL_GET_BIT_VAL(PRCM->SYS_HOSC_CTRL,
	                      PRCM_HOSC_TYPE_SHIFT,
	                      PRCM_HOSC_TYPE_VMASK);
	return PRCM_HOSCClock[val];
}

uint32_t HAL_PRCM_GetInter32KFreq(void)
{
	return (10 * HAL_GET_BIT_VAL(PRCM->SYS_RCOSC_CALIB_CTRL,
		                         PRCM_RCOSC_CALIB_FREQ_SHIFT,
		                         PRCM_RCOSC_CALIB_FREQ_VMASK));
}

uint32_t HAL_PRCM_EnableInter32KCalib(void)
{
	return HAL_SET_BIT(PRCM->SYS_RCOSC_CALIB_CTRL, PRCM_RCOSC_CALIB_EN_BIT);
}

uint32_t HAL_PRCM_DisableInter32KCalib(void)
{
	return HAL_CLR_BIT(PRCM->SYS_RCOSC_CALIB_CTRL, PRCM_RCOSC_CALIB_EN_BIT);
}

#if 0
void HAL_PRCM_SetSysPLLParam(PRCM_SysPLLParam param)
{
	HAL_MODIFY_REG(PRCM->SYS_PLL_CTRL, PRCM_SYS_PLL_PARAM_MASK, param);
}

void HAL_PRCM_EnableSysPLL(void)
{
	HAL_SET_BIT(PRCM->SYS_PLL_CTRL, PRCM_SYS_PLL_EN_BIT);
}
#else
void HAL_PRCM_SetSysPLL(PRCM_SysPLLParam param)
{
	PRCM->SYS_PLL_CTRL = PRCM_SYS_PLL_EN_BIT | param; /* NB: enable system PLL */
}
#endif

void HAL_PRCM_DisableSysPLL(void)
{
	HAL_CLR_BIT(PRCM->SYS_PLL_CTRL, PRCM_SYS_PLL_EN_BIT);
}

void HAL_PRCM_SetCPUAClk(PRCM_CPUClkSrc src, PRCM_SysClkFactor factor)
{
	switch (src) {
	case PRCM_CPU_CLK_SRC_HFCLK:
		HAL_MODIFY_REG(PRCM->SYS_CLK1_CTRL, PRCM_CPU_CLK_SRC_MASK, PRCM_CPU_CLK_SRC_HFCLK);
		break;
	case PRCM_CPU_CLK_SRC_LFCLK:
		HAL_MODIFY_REG(PRCM->SYS_CLK1_CTRL, PRCM_CPU_CLK_SRC_MASK, PRCM_CPU_CLK_SRC_LFCLK);
		break;
	case PRCM_CPU_CLK_SRC_SYSCLK:
	default:
#if 1
		HAL_MODIFY_REG(PRCM->SYS_CLK1_CTRL, PRCM_SYS_CLK_FACTOR_MASK, factor);
		HAL_SET_BIT(PRCM->SYS_CLK1_CTRL, PRCM_SYS_CLK_EN_BIT);
		HAL_MODIFY_REG(PRCM->SYS_CLK1_CTRL, PRCM_CPU_CLK_SRC_MASK, PRCM_CPU_CLK_SRC_SYSCLK);
#else
		PRCM->SYS_CLK1_CTRL = PRCM_SYS_CLK_EN_BIT | factor | PRCM_CPU_CLK_SRC_SYSCLK;
#endif
		break;
	}
}

void HAL_PRCM_DisCLK1(PRCM_SysClkFactor factor)
{
	HAL_MODIFY_REG(PRCM->SYS_CLK1_CTRL, PRCM_SYS_CLK_FACTOR_MASK, factor);
	HAL_CLR_BIT(PRCM->SYS_CLK1_CTRL, PRCM_SYS_CLK_EN_BIT);
}

uint32_t HAL_PRCM_GetCPUAClk(void)
{
	uint32_t reg = PRCM->SYS_CLK1_CTRL;
	uint32_t freq, div;

	switch (reg & PRCM_CPU_CLK_SRC_MASK) {
	case PRCM_CPU_CLK_SRC_HFCLK:
		freq = HAL_GetHFClock();
		break;
	case PRCM_CPU_CLK_SRC_LFCLK:
		freq = HAL_GetLFClock();
		break;
	case PRCM_CPU_CLK_SRC_SYSCLK:
	default:
		div = HAL_GET_BIT_VAL(reg,
			                  PRCM_SYS_CLK_FACTOR_SHIFT,
			                  PRCM_SYS_CLK_FACTOR_VMASK) + 1;
		freq = SYS_PLL_CLOCK / div;
		break;
	}
	return freq;
}

void HAL_PRCM_SetAudioPLLParam(PRCM_AudPLLParam param)
{
	PRCM->AUD_PLL_CTRL = param; /* NB: it will disable system PLL */
}

void HAL_PRCM_EnableAudioPLL(void)
{
	HAL_SET_BIT(PRCM->AUD_PLL_CTRL, PRCM_AUD_PLL_EN_BIT);
}

void HAL_PRCM_DisableAudioPLL(void)
{
	HAL_CLR_BIT(PRCM->AUD_PLL_CTRL, PRCM_AUD_PLL_EN_BIT);
}

void HAL_PRCM_SetDevClock(PRCM_DevClkFactor factor)
{
	PRCM->DEV_CLK_CTRL = factor;
}

uint32_t HAL_PRCM_GetDevClock(void)
{
	uint32_t div = HAL_GET_BIT_VAL(PRCM->DEV_CLK_CTRL,
		                           PRCM_DEV_CLK_FACTOR_SHIFT,
		                           PRCM_DEV_CLK_FACTOR_VMASK) + 1;
	return (SYS_PLL_CLOCK / div);
}

void HAL_PRCM_SetAudioPLLPatternParam(PRCM_AudPLLPatParam param)
{
	PRCM->AUD_PLL_PAT_CTRL = param; /* NB: it will disable system PLL */
}

void HAL_PRCM_EnableAudioPLLPattern(void)
{
	HAL_SET_BIT(PRCM->AUD_PLL_PAT_CTRL, PRCM_AUD_DIG_DELT_PAT_EN_BIT);
}

void HAL_PRCM_DisableAudioPLLPattern(void)
{
	HAL_CLR_BIT(PRCM->AUD_PLL_PAT_CTRL, PRCM_AUD_DIG_DELT_PAT_EN_BIT);
}

int HAL_PRCM_IsCPUAResetRelease(void)
{
	return HAL_GET_BIT(PRCM->SYS1_CTRL, PRCM_CPUA_RESET_BIT);
}

int HAL_PRCM_IsSys1ResetRelease(void)
{
	return HAL_GET_BIT(PRCM->SYS1_CTRL, PRCM_SYS1_RESET_BIT);
}

void HAL_PRCM_AllowCPUNDeepSleep(void)
{
	HAL_CLR_BIT(PRCM->SYS1_STATUS, PRCM_CPUN_DEEPSLEEP_LOCK_BIT);
}

void HAL_PRCM_DisallowCPUNDeepSleep(void)
{
	HAL_SET_BIT(PRCM->SYS1_STATUS, PRCM_CPUN_DEEPSLEEP_LOCK_BIT);
}

int HAL_PRCM_IsCPUNDeepSleepAllowed(void)
{
	return !HAL_GET_BIT(PRCM->SYS1_STATUS, PRCM_CPUN_DEEPSLEEP_LOCK_BIT);
}

int HAL_PRCM_IsCPUASleep(void)
{
	return HAL_GET_BIT(PRCM->SYS1_STATUS, PRCM_CPUA_SLEEP_STATUS_BIT);
}

int HAL_PRCM_IsCPUADeepSleep(void)
{
	return HAL_GET_BIT(PRCM->SYS1_STATUS, PRCM_CPUA_DEEPSLEEP_STATUS_BIT);
}

int HAL_PRCM_IsSys1Alive(void)
{
	return HAL_GET_BIT(PRCM->SYS1_STATUS, PRCM_SYS1_ALIVE_BIT);
}

void HAL_PRCM_DisableSys2(void)
{
	HAL_CLR_BIT(PRCM->SYS2_CTRL, PRCM_SYS2_ISOLATION_EN_BIT |
	                             PRCM_CPUN_RESET_BIT |
	                             PRCM_SYS2_RESET_BIT);
}

void HAL_PRCM_EnableSys2Isolation(void)
{
	HAL_CLR_BIT(PRCM->SYS2_CTRL, PRCM_SYS2_ISOLATION_EN_BIT);
}

void HAL_PRCM_DisableSys2Isolation(void)
{
	HAL_SET_BIT(PRCM->SYS2_CTRL, PRCM_SYS2_ISOLATION_EN_BIT);
}

void HAL_PRCM_ForceCPUNReset(void)
{
	HAL_CLR_BIT(PRCM->SYS2_CTRL, PRCM_CPUN_RESET_BIT);
}

void HAL_PRCM_ReleaseCPUNReset(void)
{
	HAL_SET_BIT(PRCM->SYS2_CTRL, PRCM_CPUN_RESET_BIT);
}

int HAL_PRCM_IsCPUNReleased(void)
{
	return HAL_GET_BIT(PRCM->SYS2_CTRL, PRCM_CPUN_RESET_BIT);
}

void HAL_PRCM_ForceSys2Reset(void)
{
	HAL_CLR_BIT(PRCM->SYS2_CTRL, PRCM_SYS2_RESET_BIT);
}

void HAL_PRCM_ReleaseSys2Reset(void)
{
	HAL_SET_BIT(PRCM->SYS2_CTRL, PRCM_SYS2_RESET_BIT);
}

int HAL_PRCM_IsCPUADeepSleepAllowed(void)
{
	return !HAL_GET_BIT(PRCM->SYS2_STATUS, PRCM_CPUA_DEEPSLEEP_LOCK_BIT);
}

int HAL_PRCM_IsCPUNSleep(void)
{
	return HAL_GET_BIT(PRCM->SYS2_STATUS, PRCM_CPUN_SLEEP_STATUS_BIT);
}

int HAL_PRCM_IsCPUNDeepSleep(void)
{
	return HAL_GET_BIT(PRCM->SYS2_STATUS, PRCM_CPUN_DEEPSLEEP_STATUS_BIT);
}

int HAL_PRCM_IsSys2Alive(void)
{
	return HAL_GET_BIT(PRCM->SYS2_STATUS, PRCM_SYS2_ALIVE_BIT);
}

int HAL_PRCM_IsSys3Alive(void)
{
	return HAL_GET_BIT(PRCM->SYS3_STATUS, PRCM_SYS3_ALIVE_BIT);
}

void HAL_PRCM_SetSys1WakeupPowerFlags(uint32_t flags)
{
	PRCM->SYS1_WAKEUP_CTRL = flags & PRCM_SYS_WS_PWR_FLAGS_MASK;
}

uint32_t HAL_PRCM_GetSys2WakeupPowerFlags(void)
{
	return (PRCM->SYS2_WAKEUP_CTRL & PRCM_SYS_WS_PWR_FLAGS_MASK);
}

uint32_t HAL_PRCM_GetSys1SleepPowerFlags(void)
{
	return (PRCM->SYS1_SLEEP_CTRL & PRCM_SYS_WS_PWR_FLAGS_MASK);
}

void HAL_PRCM_SetSys1SleepPowerFlags(uint32_t flags)
{
	PRCM->SYS1_SLEEP_CTRL = flags & PRCM_SYS_WS_PWR_FLAGS_MASK;
}

uint32_t HAL_PRCM_GetSys2SleepPowerFlags(void)
{
	return (PRCM->SYS2_SLEEP_CTRL & PRCM_SYS_WS_PWR_FLAGS_MASK);
}

void HAL_PRCM_SetSRAMVoltage(PRCM_SRAMVolt workVolt, PRCM_SRAMVolt retenVolt)
{
	PRCM->SRAM_VOLT_CTRL =
		((workVolt << PRCM_SRAM_WORK_VOLT_SHIFT) & PRCM_SRAM_WORK_VOLT_MASK) |
		((retenVolt << PRCM_SRAM_RETEN_VOLT_SHIFT) & PRCM_SRAM_RETEN_VOLT_MASK);
}

void HAL_PRCM_SetBANDGAPSTABLE_TIME(uint32_t time)
{
	PRCM->BANDGAP_STABLE_REF_TIME = (time & PRCM_BANDGAP_STABLE_REF_TIME_MASK);
}

uint32_t HAL_PRCM_GetBANDGAPSTABLE_TIME(void)
{
	return (PRCM->BANDGAP_STABLE_REF_TIME & PRCM_BANDGAP_STABLE_REF_TIME_MASK);
}

void HAL_PRCM_SetDCDCSTABLE_TIME(uint32_t time)
{
	PRCM->DCDC_STABLE_REF_TIME = (time & PRCM_DCDC_STABLE_REF_TIME_MASK);
}

uint32_t HAL_PRCM_GetDCDCSTABLE_TIME(void)
{
	return (PRCM->DCDC_STABLE_REF_TIME & PRCM_DCDC_STABLE_REF_TIME_MASK);
}

void HAL_PRCM_SetCPUABootFlag(PRCM_CPUABootFlag flag)
{
	PRCM->CPUA_BOOT_FLAG = PRCM_CPUA_BOOT_FLAG_WR_LOCK | flag;
}

uint32_t HAL_PRCM_GetCPUABootFlag(void)
{
	return HAL_GET_BIT(PRCM->CPUA_BOOT_FLAG, PRCM_CPUA_BOOT_FLAG_MASK);
}

void HAL_PRCM_SetCPUABootAddr(uint32_t addr)
{
	PRCM->CPUA_BOOT_ADDR = addr;
}

uint32_t HAL_PRCM_GetCPUABootAddr(void)
{
	return PRCM->CPUA_BOOT_ADDR;
}

void HAL_PRCM_SetCPUABootArg(uint32_t arg)
{
	PRCM->CPUA_BOOT_ARG = arg;
}

uint32_t HAL_PRCM_GetCPUABootArg(void)
{
	return PRCM->CPUA_BOOT_ARG;
}

void HAL_PRCM_SetCPUAPrivateData(uint32_t data)
{
	PRCM->CPUA_PRIV_REG = data;
}

uint32_t HAL_PRCM_GetCPUAPrivateData(void)
{
	return PRCM->CPUA_PRIV_REG;
}

uint32_t HAL_PRCM_GetWakeupTimerEnable(void)
{
	return (PRCM->CPUA_WAKE_TIMER_CNT & PRCM_CPUx_WAKE_TIMER_EN_BIT);
}

void HAL_PRCM_WakeupTimerEnable(void)
{
	HAL_SET_BIT(PRCM->CPUA_WAKE_TIMER_CNT, PRCM_CPUx_WAKE_TIMER_EN_BIT);
}

void HAL_PRCM_WakeupTimerDisable(void)
{
	HAL_CLR_BIT(PRCM->CPUA_WAKE_TIMER_CNT, PRCM_CPUx_WAKE_TIMER_EN_BIT);
}

uint32_t HAL_PRCM_WakeupTimerGetCurrentValue(void)
{
	return (PRCM->CPUA_WAKE_TIMER_CNT & PRCM_CPUx_WAKE_TIMER_CUR_VAL_MASK);
}

uint32_t HAL_PRCM_GetWakeupTimerPending(void)
{
	return (PRCM->CPUA_WAKE_TIMER_CMP & PRCM_CPUx_WAKE_TIMER_PENDING_BIT);
}

void HAL_PRCM_ClearWakeupTimerPending(void)
{
	HAL_SET_BIT(PRCM->CPUA_WAKE_TIMER_CMP, PRCM_CPUx_WAKE_TIMER_PENDING_BIT);
}

void HAL_PRCM_WakeupTimerSetCompareValue(uint32_t val)
{
	PRCM->CPUA_WAKE_TIMER_CMP = val & PRCM_CPUx_WAKE_TIMER_CMP_VAL_MASK;
}

uint32_t HAL_PRCM_WakeupTimerGetCompareValue(void)
{
	return (PRCM->CPUA_WAKE_TIMER_CMP & PRCM_CPUx_WAKE_TIMER_CMP_VAL_MASK);
}

#if 0
uint32_t HAL_PRCM_GetCPUNWakeupTimerCurrentValue(void)
{
	return (PRCM->CPUN_WAKE_TIMER_CNT & PRCM_CPUx_WAKE_TIMER_CUR_VAL_MASK);
}

void HAL_PRCM_SetCPUNWakeupTimerCompareValue(uint32_t val)
{
	PRCM->CPUN_WAKE_TIMER_CMP = val & PRCM_CPUx_WAKE_TIMER_CMP_VAL_MASK);
}

uint32_t HAL_PRCM_GetCPUNWakeupTimerCompareValue(void)
{
	return (PRCM->CPUN_WAKE_TIMER_CMP & PRCM_CPUx_WAKE_TIMER_CMP_VAL_MASK);
}
#endif

void HAL_PRCM_WakeupIOEnable(uint32_t ioMask)
{
	HAL_SET_BIT(PRCM->CPUA_WAKE_IO_EN, ioMask);
}

void HAL_PRCM_WakeupIODisable(uint32_t ioMask)
{
	HAL_CLR_BIT(PRCM->CPUA_WAKE_IO_EN, ioMask);
}

void HAL_PRCM_WakeupIOSetRisingEvent(uint32_t ioMask)
{
	HAL_SET_BIT(PRCM->CPUA_WAKE_IO_MODE, ioMask);
}

void HAL_PRCM_WakeupIOSetFallingEvent(uint32_t ioMask)
{
	HAL_CLR_BIT(PRCM->CPUA_WAKE_IO_MODE, ioMask);
}

uint32_t HAL_PRCM_WakeupIOGetEventStatus(void)
{
	return HAL_GET_BIT(PRCM->CPUA_WAKE_IO_STATUS, PRCM_WAKE_IO_MASK);
}

int HAL_PRCM_WakeupIOIsEventDetected(uint32_t ioMask)
{
	return HAL_GET_BIT(PRCM->CPUA_WAKE_IO_STATUS, ioMask);
}

void HAL_PRCM_WakeupIOClearEventDetected(uint32_t ioMask)
{
	HAL_SET_BIT(PRCM->CPUA_WAKE_IO_STATUS, ioMask);
}

void HAL_PRCM_WakeupIOEnableCfgHold(uint32_t ioMask)
{
	HAL_SET_BIT(PRCM->CPUA_WAKE_IO_HOLD, ioMask);
}

void HAL_PRCM_WakeupIODisableCfgHold(uint32_t ioMask)
{
	HAL_CLR_BIT(PRCM->CPUA_WAKE_IO_HOLD, ioMask);
}

void HAL_PRCM_WakeupIOEnableGlobal(void)
{
	HAL_SET_BIT(PRCM->CPUA_WAKE_IO_GLOBAL_EN, PRCM_WAKE_IO_GLOBAL_EN_BIT);
}

void HAL_PRCM_WakeupIODisableGlobal(void)
{
	HAL_CLR_BIT(PRCM->CPUA_WAKE_IO_GLOBAL_EN, PRCM_WAKE_IO_GLOBAL_EN_BIT);
}

void HAL_PRCM_Start(void)
{
	HAL_CLR_BIT(PRCM->CPUA_PRCM_REG, PRCM_CPUA_PRCM_REG_BIT);
}

#if 0
void HAL_PRCM_EnableCPUAWakeupTimer(void)
{
	HAL_SET_BIT(PRCM->CPUA_WAKE_TIMER_CNT, PRCM_CPUx_WAKE_TIMER_EN_BIT);
}

void HAL_PRCM_DisableCPUAWakeupTimer(void)
{
	HAL_CLR_BIT(PRCM->CPUA_WAKE_TIMER_CNT, PRCM_CPUx_WAKE_TIMER_EN_BIT);
}

uint32_t HAL_PRCM_GetCPUAWakeupTimerCurrentValue(void)
{
	return (PRCM->CPUA_WAKE_TIMER_CNT & PRCM_CPUx_WAKE_TIMER_CUR_VAL_MASK);
}

void HAL_PRCM_SetCPUAWakeupTimerCompareValue(uint32_t val)
{
	PRCM->CPUA_WAKE_TIMER_CMP = val & PRCM_CPUx_WAKE_TIMER_CMP_VAL_MASK;
}

uint32_t HAL_PRCM_GetCPUAWakeupTimerCompareValue(void)
{
	return (PRCM->CPUA_WAKE_TIMER_CMP & PRCM_CPUx_WAKE_TIMER_CMP_VAL_MASK);
}

#if 0
uint32_t HAL_PRCM_GetCPUNWakeupTimerCurrentValue(void)
{
	return (PRCM->CPUN_WAKE_TIMER_CNT & PRCM_CPUx_WAKE_TIMER_CUR_VAL_MASK);
}

void HAL_PRCM_SetCPUNWakeupTimerCompareValue(uint32_t val)
{
	PRCM->CPUN_WAKE_TIMER_CMP = val & PRCM_CPUx_WAKE_TIMER_CMP_VAL_MASK);
}

uint32_t HAL_PRCM_GetCPUNWakeupTimerCompareValue(void)
{
	return (PRCM->CPUN_WAKE_TIMER_CMP & PRCM_CPUx_WAKE_TIMER_CMP_VAL_MASK);
}
#endif

void HAL_PRCM_EnableWakeupIO(uint32_t ioMask)
{
	HAL_SET_BIT(PRCM->CPUA_WAKE_IO_EN, ioMask);
}

void HAL_PRCM_DisableWakeupIO(uint32_t ioMask)
{
	HAL_CLR_BIT(PRCM->CPUA_WAKE_IO_EN, ioMask);
}

void HAL_PRCM_SetWakeupIORisingEvent(uint32_t ioMask)
{
	HAL_SET_BIT(PRCM->CPUA_WAKE_IO_MODE, ioMask);
}

void HAL_PRCM_SetWakeupIOFallingEvent(uint32_t ioMask)
{
	HAL_CLR_BIT(PRCM->CPUA_WAKE_IO_MODE, ioMask);
}

uint32_t HAL_PRCM_GetWakeupIOEventStatus(void)
{
	return HAL_GET_BIT(PRCM->CPUA_WAKE_IO_STATUS, PRCM_WAKE_IO_MASK);
}

int HAL_PRCM_IsWakeupIOEventDetected(uint32_t ioMask)
{
	return HAL_GET_BIT(PRCM->CPUA_WAKE_IO_STATUS, ioMask);
}

void HAL_PRCM_ClearWakeupIOEventDetected(uint32_t ioMask)
{
	HAL_SET_BIT(PRCM->CPUA_WAKE_IO_STATUS, ioMask);
}

void HAL_PRCM_EnableWakeupIOCfgHold(uint32_t ioMask)
{
	HAL_SET_BIT(PRCM->CPUA_WAKE_IO_HOLD, ioMask);
}

void HAL_PRCM_DisableWakeupIOCfgHold(uint32_t ioMask)
{
	HAL_CLR_BIT(PRCM->CPUA_WAKE_IO_HOLD, ioMask);
}

void HAL_PRCM_EnableWakeupIOGlobal(void)
{
	HAL_SET_BIT(PRCM->CPUA_WAKE_IO_GLOBAL_EN, PRCM_WAKE_IO_GLOBAL_EN_BIT);
}

void HAL_PRCM_DisableWakeupIOGlobal(void)
{
	HAL_CLR_BIT(PRCM->CPUA_WAKE_IO_GLOBAL_EN, PRCM_WAKE_IO_GLOBAL_EN_BIT);
}
#endif
