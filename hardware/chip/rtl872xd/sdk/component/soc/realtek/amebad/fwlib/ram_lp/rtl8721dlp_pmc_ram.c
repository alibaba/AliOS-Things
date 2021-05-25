/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "ameba_soc.h"

/**
  * @brief  set wake up event.
  * @param  Option:
  *   This parameter can be any combination of the following values:
  *		 @arg BIT_LP_WEVT_HS_MSK
  *		 @arg BIT_LP_WEVT_AON_MSK
  *		 @arg BIT_LP_WEVT_SGPIO_MSK
  *		 @arg BIT_LP_WEVT_COMP_MSK
  *		 @arg BIT_LP_WEVT_ADC_MSK
  *		 @arg BIT_LP_WEVT_I2C_MSK
  *		 @arg BIT_LP_WEVT_UART_MSK
  *		 @arg BIT_LP_WEVT_BOD_MSK
  *		 @arg BIT_LP_WEVT_WLAN_MSK
  *		 @arg BIT_LP_WEVT_GPIO_MSK
  *		 @arg BIT_LP_WEVT_SWRD_OCP_MSK
  *		 @arg BIT_LP_WEVT_TIMER_MSK 
  * @param  NewStatus: TRUE/FALSE.
  * @retval None
  */
IMAGE2_RAM_TEXT_SECTION
void SOCPS_SetWakeEvent(u32 Option, u32 NewStatus)
{
	u32 WakeEvent = 0;
	
	/* Mask System IRQ INT */
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LP_PWRMGT_CTRL, (HAL_READ32(SYSTEM_CTRL_BASE, REG_LP_PWRMGT_CTRL) | BIT_LSYS_SYSIRQ_MASK_POLL));
	
	/* Set Event */
	WakeEvent = HAL_READ32(SYSTEM_CTRL_BASE, REG_LP_SLP_WAKE_EVENT_MSK0);
	if (NewStatus == ENABLE) {
		WakeEvent |= Option;
	} else {
		WakeEvent &= ~Option;
	}
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LP_SLP_WAKE_EVENT_MSK0, WakeEvent); 
}

/**
  * @brief  set wake up event.
  * @param  Option:
  *   This parameter can be any combination of the following values:
  *		 @arg BIT_KEYSCAN_WAKE_MSK
  *		 @arg BIT_DLPS_TSF_WAKE_MSK
  *		 @arg BIT_RTC_WAKE_MSK
  *		 @arg BIT_AON_WAKE_TIM0_MSK
  *		 @arg BIT_GPIO_WAKE_MSK
  * @param  NewStatus: TRUE/FALSE.
  * @retval None
  */
IMAGE2_RAM_TEXT_SECTION
void SOCPS_SetWakeEventAON(u32 Option, u32 NewStatus)
{
	u32 WakeEvent = 0;
	
	/* Set Event */
	WakeEvent = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_AON_WAKE_OPT_MSK);
	if (NewStatus == ENABLE) {
		WakeEvent |= Option;
	} else {
		WakeEvent &= ~Option;
	}
	HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_AON_WAKE_OPT_MSK, WakeEvent);
}

/**
  * @brief  clear all of wake_pin interrupt(Max. 4 wake_pin).
  * @retval None
  *
  */
IMAGE2_RAM_TEXT_SECTION
void SOCPS_ClearWakePin(void)
{
	u32 Rtemp = HAL_READ32(SYSTEM_CTRL_BASE, REG_AON_WAKE_GPIO_CTRL2);
	
	Rtemp |= (BIT_AON_MASK_GPIO_WAKE_EVENT << BIT_AON_SHIFT_GPIO_WAKE_EVENT);//0x0F000000;
	Rtemp &= (~(BIT_AON_MASK_GPIO_WAKE_PUPD_EN << BIT_AON_SHIFT_GPIO_WAKE_PUPD_EN)); /* close internal PU/PD resistence */
	
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_AON_WAKE_GPIO_CTRL2, Rtemp);
}

/**
  * @brief  clear wake event.
  * @retval None
  */
IMAGE2_RAM_TEXT_SECTION
void SOCPS_ClearWakeEvent(void)
{
	u32 Rtemp = 0;

	Rtemp = HAL_READ32(SYSTEM_CTRL_BASE, REG_LP_SLP_WAKE_EVENT_STATUS0);
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LP_SLP_WAKE_EVENT_STATUS0, Rtemp);

	Rtemp = HAL_READ32(SYSTEM_CTRL_BASE, REG_LP_SLP_WAKE_EVENT_STATUS1);
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LP_SLP_WAKE_EVENT_STATUS1, Rtemp);

	/* clear Deep sleep wake event */
	SOCPS_AONWakeClear(BIT_ALL_WAKE_STS & (~BIT_KEYSCAN_WAKE_STS));
}

IMAGE2_RAM_TEXT_SECTION
void SOCPS_AudioLDO(u32 NewStatus)
{
	u32 temp = 0;

	if (NewStatus == DISABLE) {
		//0x4800_0344[8] = 1'b1 (BIT_LSYS_AC_MBIAS_POW)
		temp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_AUDIO_SHARE_PAD_CTRL);
		temp &= ~(BIT_LSYS_AC_MBIAS_POW | BIT_LSYS_AC_LDO_POW);
		HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_AUDIO_SHARE_PAD_CTRL, temp);

		//0x4800_0280[2:0] = 3'h7 (Enable BG)
		temp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_LP_SYSPLL_CTRL0);
		temp &= ~(BIT_LP_PLL_BG_EN | BIT_LP_PLL_BG_I_EN | BIT_LP_PLL_MBIAS_EN);
		HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_LP_SYSPLL_CTRL0, temp);
	
	} else {
		if (wifi_config.wifi_ultra_low_power) {
			return;
		}
	
		/* if audio not use, save power */
		if (ps_config.km0_audio_pad_enable == FALSE) {
			return;
		}
		
		temp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_AUDIO_SHARE_PAD_CTRL);
		if ((temp & BIT_LSYS_AC_LDO_POW) != 0) {
			return;
		}
		
		//0x4800_0280[2:0] = 3'h7 (Enable BG)
		temp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_LP_SYSPLL_CTRL0);
		temp |= (BIT_LP_PLL_BG_EN | BIT_LP_PLL_BG_I_EN | BIT_LP_PLL_MBIAS_EN);
		HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_LP_SYSPLL_CTRL0, temp);
	
		//0x4800_0344[8] = 1'b1 (BIT_LSYS_AC_MBIAS_POW)
		temp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_AUDIO_SHARE_PAD_CTRL);
		temp |= BIT_LSYS_AC_MBIAS_POW;
		HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_AUDIO_SHARE_PAD_CTRL, temp);

		//Delay 2us,  0x4800_0344[0] = 1'b1 (BIT_LSYS_AC_LDO_POW)
		DelayUs(5);

		//enable Audio LDO.
		temp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_AUDIO_SHARE_PAD_CTRL);
		temp &= ~(BIT_LSYS_MASK_AC_LDO_REG << BIT_LSYS_SHIFT_AC_LDO_REG);
		if (is_power_supply18()){
			temp |= ((u32)0x68) << BIT_LSYS_SHIFT_AC_LDO_REG;
		}
		else{
			temp |= ((u32)0x2A) << BIT_LSYS_SHIFT_AC_LDO_REG;
		}
		HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_AUDIO_SHARE_PAD_CTRL, temp);
		
		temp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_AUDIO_SHARE_PAD_CTRL);
		temp |= BIT_LSYS_AC_LDO_POW;
		HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_AUDIO_SHARE_PAD_CTRL, temp);
	}
}

//IMAGE2_RAM_TEXT_SECTION
VOID SOCPS_OSC2M_Cmd(u32 new_status)
{
	u32 Temp = 0;
	
	Temp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_LP_OSC2M_CTRL);
	if (new_status == DISABLE) {
		Temp &= ~BIT_OSC2M_EN;
	} else {
		Temp |= BIT_OSC2M_EN;
	}
	HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_LP_OSC2M_CTRL, Temp);
}

IMAGE2_RAM_TEXT_SECTION
VOID SOCPS_SWRLDO_Suspend(u32 new_status)
{
	u32 Temp;

	/* suspend */
	if (new_status == ENABLE) {
		/* BIT_SHIFT_SWR_OCP_L1 to fix PFM pulse 500uA issue, PFM pulse is 100uA */
		//Temp=HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_SYS_EFUSE_SYSCFG1);
		//Temp &= ~BIT_MASK_SWR_REG_ZCDC_H;
		//Temp &= ~BIT_MASK_SWR_OCP_L1;
		//Temp |= (0x03 << BIT_SHIFT_SWR_OCP_L1); /* PWM:600 PFM: 250, default is 0x04 */
		//HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_SYS_EFUSE_SYSCFG1,Temp); /*move OCP_L1 setting to app_pmu_init(), set 0x03 in both sleep and active*/

		/* LP_LDO can not set 0.692V under DSLP mode, because DSLP snooze use it */
		/* LP_LDO not use in other case */
		/* set LP_LDO to 0.782V(we dont use it) to fix SWR 0.9V(REG_SWR_PSW_CTRL = 0x8765) power leakage issue */
		Temp=HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_AON_LDO_CTRL1);
		Temp &= ~(BIT_MASK_LDO_LP_ADJ << BIT_SHIFT_LDO_LP_ADJ);
		Temp |= (0x03 << BIT_SHIFT_LDO_LP_ADJ); /* 0.692V, this LDO is not used */
		Temp |= BIT_LDO_LP_SLEEP_EN; /* 7uA */
		HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_AON_LDO_CTRL1,Temp);

		/* set AON LDO sleep mode, voltage default is 0x09=0.899V */
		Temp=HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_AON_LDO_CTRL0);
		Temp &= ~(BIT_MASK_AON_LDO_V09_ADJ << BIT_SHIFT_AON_LDO_V09_ADJ);
		Temp |= (0x09 << BIT_SHIFT_AON_LDO_V09_ADJ); //>0.81V is save for MP
		Temp |= BIT_AON_LDO_SLP_EN; /* 11uA */
		HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_AON_LDO_CTRL0,Temp);
	} else {
		/* BIT_SHIFT_SWR_OCP_L1 fix active cost 2mA more issue */
		//Temp=HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_SYS_EFUSE_SYSCFG1);
		//Temp &= ~BIT_MASK_SWR_REG_ZCDC_H;
		//Temp |= (0x02 << BIT_SHIFT_SWR_REG_ZCDC_H);
		//Temp &= ~BIT_MASK_SWR_OCP_L1;
		//Temp |= (0x04 << BIT_SHIFT_SWR_OCP_L1);
		//HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_SYS_EFUSE_SYSCFG1,Temp);/*move OCP_L1 setting to app_pmu_init()*/

		/* set LP_LDO to 0.782V(we dont use it) to fix SWR 0.9V(REG_SWR_PSW_CTRL = 0x8765) power leakage issue */
		Temp=HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_AON_LDO_CTRL1);
		Temp &= ~(BIT_MASK_LDO_LP_ADJ << BIT_SHIFT_LDO_LP_ADJ);
		Temp |= (0x09 << BIT_SHIFT_LDO_LP_ADJ);
		Temp &= ~BIT_LDO_LP_SLEEP_EN;
		HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_AON_LDO_CTRL1,Temp);

		/* set AON LDO sleep mode, voltage default is 0x09=0.899V */
		Temp=HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_AON_LDO_CTRL0);
		Temp &= ~(BIT_MASK_AON_LDO_V09_ADJ << BIT_SHIFT_AON_LDO_V09_ADJ);
		Temp |= (0x09 << BIT_SHIFT_AON_LDO_V09_ADJ);
		Temp &= ~BIT_AON_LDO_SLP_EN;
		HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_AON_LDO_CTRL0,Temp);
	}
}

/**
  * @brief  enter sleep power gate mode.
  * @retval None
  *
  */
IMAGE2_RAM_TEXT_SECTION
void SOCPS_SleepCG_RAM(VOID)
{
	u32 Rtemp = 0;

	pinmap_sleep(); //50us
	km4_flash_highspeed_suspend(FALSE);//80us

	if (ps_config.km0_osc2m_close) {
		SOCPS_OSC2M_Cmd(DISABLE);//10us
	}

	FLASH_DeepPowerDown(ENABLE);//120us

	SOCPS_AudioLDO(DISABLE);//15us

	/* don't close CPU */
	Rtemp = HAL_READ32(SYSTEM_CTRL_BASE, REG_LP_PWRMGT_OPTION);
  	Rtemp |= BIT_LSYS_PST_SLEP_ESOC;
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LP_PWRMGT_OPTION, Rtemp);//10us

	if(SLEEP_PG == km4_sleep_type) {
		/* Disable HS Platform/HSSYSON power */
		Rtemp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_LP_PWR_ISO_CTRL);
		Rtemp |= BIT_LSYS_ISO_HSOC; //enable isolation
		HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_LP_PWR_ISO_CTRL, Rtemp);
		Rtemp &= ~(BIT_MASK_LSYS_HPLAT_PWC_EN_BIT2 | BIT_MASK_LSYS_HPLAT_PWC_EN_BIT3);//close power
		HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_LP_PWR_ISO_CTRL, Rtemp);
	}

	/* if OSC4M closed, WIFI can not wakeup, we should open TSF wakeup */
	if(SYSCFG_CUTVersion() == SYSCFG_CUT_VERSION_A) {
		if ((Rtemp & BIT_LSYS_PST_SLEP_EACK) == 0) {
			SOCPS_SetWakeEventAON(BIT_DLPS_TSF_WAKE_MSK, ENABLE);
		}
	}

	/* set power mode */
	Rtemp = HAL_READ32(SYSTEM_CTRL_BASE, REG_LP_PWRMGT_CTRL);
	Rtemp |= (BIT_LSYS_PMC_PMEN_SLEP | BIT_LSYS_PMC_OPT_BKPOFIMK1 | BIT_LSYS_PMC_OPT_BKPOFIMK0);
	Rtemp &= ~BIT_LSYS_PMC_PMEN_DSLP;
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LP_PWRMGT_CTRL, Rtemp);

	/* it will take 6.5us to enter clock gating mode after register set */
	__WFI();
	/* cpu bug workaround or cpu may cannot wakeup */
	asm volatile("nop");

	/* use to check wakeup time in 66319D */
	//BOOT_ROM_CM4PON((u32)HSPWR_ON_SEQ_TEST);
	//DelayMs(3);
	//BOOT_ROM_CM4PON((u32)HSPWR_OFF_SEQ_TEST);
	TIMM05->CNT = 0;
	ps_config.km0_wake_time = RTIM_TestTimer_GetCount();

	 /*enable HS platform power for high 8k txpktbuffer only for A cut*/
	Rtemp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_LP_PWR_ISO_CTRL);
	Rtemp |= BIT_MASK_LSYS_HPLAT_PWC_EN_BIT2;
	HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_LP_PWR_ISO_CTRL, Rtemp);

	//move to km4 on to shrink wakeup time
	//SOCPS_AudioLDO(ENABLE);//84

	/* wakeup */
	FLASH_DeepPowerDown(DISABLE);//257

	if(SLEEP_PG == km4_sleep_type) {
		/*release HS platform power Isolation only for A cut*/
		Rtemp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_LP_PWR_ISO_CTRL);
		Rtemp |= BIT_MASK_LSYS_HPLAT_PWC_EN_BIT3;//power on hs platform for txpktbuffer high 8k
		Rtemp &= ~BIT_LSYS_ISO_HSOC;
		HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_LP_PWR_ISO_CTRL, Rtemp);
	}

	if (ps_config.km0_osc2m_close) {
		SOCPS_OSC2M_Cmd(ENABLE);//267
	}

	km4_flash_highspeed_resume(FALSE);//921
	
	pinmap_wake();//954
}

/**
  * @brief  enter sleep power gate mode.
  * @retval None
  *
  */
IMAGE2_RAM_TEXT_SECTION
void SOCPS_SleepPG_RAM(VOID)
{
	u32 Rtemp = 0;

	pinmap_sleep(); //50us
	km4_flash_highspeed_suspend(FALSE); //80us

	if (ps_config.km0_osc2m_close) {
		SOCPS_OSC2M_Cmd(DISABLE);
	}

	FLASH_DeepPowerDown(ENABLE);

	SOCPS_AudioLDO(DISABLE);

	/* close CPU */
	Rtemp = HAL_READ32(SYSTEM_CTRL_BASE, REG_LP_PWRMGT_OPTION);
	Rtemp &= ~BIT_LSYS_PST_SLEP_ESOC;
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LP_PWRMGT_OPTION, Rtemp);
	
	/*reset flash to solve wakeup no spic clock issue*/
	RCC_PeriphClockCmd(APBPeriph_FLASH, APBPeriph_FLASH_CLOCK_XTAL, DISABLE);

	/* Disable HS Platform/HSSYSON power */
	Rtemp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_LP_PWR_ISO_CTRL);
	Rtemp |= BIT_LSYS_ISO_HSOC; //enable isolation
	HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_LP_PWR_ISO_CTRL, Rtemp);
	Rtemp &= ~(BIT_MASK_LSYS_HPLAT_PWC_EN_BIT2 | BIT_MASK_LSYS_HPLAT_PWC_EN_BIT3);//close power
	HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_LP_PWR_ISO_CTRL, Rtemp);

	/* if OSC4M closed, WIFI can not wakeup, we should open TSF wakeup */
	if(SYSCFG_CUTVersion() == SYSCFG_CUT_VERSION_A) {
		if ((Rtemp & BIT_LSYS_PST_SLEP_EACK) == 0) {
			SOCPS_SetWakeEventAON(BIT_DLPS_TSF_WAKE_MSK, ENABLE);
		}
	}

	/* set power mode */
	Rtemp = HAL_READ32(SYSTEM_CTRL_BASE, REG_LP_PWRMGT_CTRL);
	Rtemp |= (BIT_LSYS_PMC_PMEN_SLEP | BIT_LSYS_PMC_OPT_BKPOFIMK1 | BIT_LSYS_PMC_OPT_BKPOFIMK0);
	Rtemp &= ~BIT_LSYS_PMC_PMEN_DSLP;
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LP_PWRMGT_CTRL, Rtemp);

	/* it will take 6.5us to enter clock gating mode after register set */
	__WFI();
	/* cpu bug workaround or cpu may cannot wakeup */
	asm volatile("nop");

	/*enable HS platform power for high 8k txpktbuffer.*/
	Rtemp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_LP_PWR_ISO_CTRL);
	Rtemp |= BIT_MASK_LSYS_HPLAT_PWC_EN_BIT2;
	HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_LP_PWR_ISO_CTRL, Rtemp);

	/* if wakeup happened when sleep, following code will be run */
	
	/*for break power-off condition to reinit spic*/
	RCC_PeriphClockCmd(APBPeriph_FLASH, APBPeriph_FLASH_CLOCK_XTAL, ENABLE);

	FLASH_Init(flash_init_para.FLASH_cur_bitmode);

	Cache_Enable(ENABLE);

	/* soc sleep need it, or wakeup will fail */
	FLASH_DeepPowerDown(DISABLE);

	/*release HS platform power Isolation.*/
	Rtemp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_LP_PWR_ISO_CTRL);
	Rtemp |= BIT_MASK_LSYS_HPLAT_PWC_EN_BIT3;//power on hs platform for txpktbuffer high 8k
	Rtemp &= ~BIT_LSYS_ISO_HSOC;
	HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_LP_PWR_ISO_CTRL, Rtemp);

	SOCPS_MMUReFill();

	km4_flash_highspeed_resume(FALSE);

	//move to km4 on to shrink wakeup time
	//SOCPS_AudioLDO(ENABLE);
}

/**
  * @brief  enter deep sleep mode, patch of SOCPS_DeepSleep.
  * @retval None
  *
  */
IMAGE2_RAM_TEXT_SECTION
void SOCPS_DeepSleep_RAM(void)
{
	u32 Rtemp = 0;

	DBG_8195A("M0DS \n");
	
	/* init wake event */
	SOCPS_DsleepInit();

	/* pin power leakage */
	pinmap_deepsleep();
	
	/* clear wake event */
	SOCPS_ClearWakeEvent();

	/* Interrupt Clear Pending Register, clear all interrupt */
	//NVIC->ICPR[0] = NVIC_ICPR0_PS_MASK;

	/* Enable low power mode */
	FLASH_DeepPowerDown(ENABLE);

	/* set LP_LDO to 0.899V to fix RTC Calibration XTAL power leakage issue */
	Rtemp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_AON_LDO_CTRL1);
	Rtemp &= ~(BIT_MASK_LDO_LP_ADJ << BIT_SHIFT_LDO_LP_ADJ);
	Rtemp |= (0x09 << BIT_SHIFT_LDO_LP_ADJ); /* 0.899V, or ADC/XTAL will work abnormal in deepsleep mode */
	HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_AON_LDO_CTRL1,Rtemp);

	/* Shutdown LSPAD(no AON) at the end of DSLP flow */
	/* Fix ACUT power leakage issue */
	Rtemp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_AON_PWR_CTRL);
	Rtemp &= ~BIT_DSLP_SNOOZE_MODE_LSPAD_SHUTDOWN;
	HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_AON_PWR_CTRL, Rtemp);
	
	/* set power mode */
	Rtemp = HAL_READ32(SYSTEM_CTRL_BASE, REG_LP_PWRMGT_CTRL);
	Rtemp &= ~BIT_LSYS_PMC_PMEN_SLEP;
	Rtemp |= BIT_LSYS_PMC_PMEN_DSLP;
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LP_PWRMGT_CTRL, Rtemp);

	/* Wait CHIP enter deep sleep mode */
	__WFI();
}
