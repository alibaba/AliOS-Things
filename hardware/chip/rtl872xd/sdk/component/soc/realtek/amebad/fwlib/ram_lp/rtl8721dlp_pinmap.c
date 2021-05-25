/**
  ******************************************************************************
  * @file    rtl8721dlp_pinmap.c
  * @author
  * @version V1.0.0
  * @date    2016-05-17
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities of pin control:
  *           - pinmux
  *           - active pad pull up & pull down
  *           - sleep pad pull up & pull down
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
  ****************************************************************************** 
  */

#include "ameba_soc.h"

extern const PMAP_TypeDef pmap_func[];
extern const PMAP_TypeDef pmap_func_sleep[];

CONFIG_FW_CRITICAL_CODE_SECTION
static u32 pinmap_touchpin(u32 PinName)
{
	if ((PinName == _PB_4) || (PinName == _PB_5) ||
		(PinName == _PB_6) || (PinName == _PB_7)) {
		return TRUE;
	}

	return FALSE;
}

/**
  *  @brief Config all pins to the right function and pull state based on pmap_func table.
  *  @note should set pmap_func. Function & FuncPuPd corectly based on your board.
  *  @retval None       
  */
CONFIG_FW_CRITICAL_CODE_SECTION
void pinmap_init(void)
{
	int i = 0;
	u32 dslp_wakeup = SOCPS_DsleepWakeStatusGet();

	for (;;) {
		/*  Check if search to end */
		if (pmap_func[i].PinName == _PNC) {
			break;
		}

		/* keep active level when GPIO_PuPd_KEEP */
		if (pmap_func[i].FuncPuPd == GPIO_PuPd_KEEP)
			goto next;

		if (dslp_wakeup) {
			/* low power pin, dont pull low power pins ex. keyscan */
			if (pmap_func[i].LowPowerPin)
				goto next;

			/* touch pin, dont pull captouch enabled pin */
			if (pinmap_touchpin(pmap_func[i].PinName) == TRUE) {
				if (CapTouch_GetChStatus(CAPTOUCH_DEV, (pmap_func[i].PinName - _PB_4))) {
					goto next;
				}
			}
		}

		/* active PuPd set */
		if (pmap_func[i].FuncPuPd == GPIO_PuPd_SHUTDOWN) {
			PAD_CMD(pmap_func[i].PinName, DISABLE);
		} else {
			PAD_PullCtrl(pmap_func[i].PinName, pmap_func[i].FuncPuPd);
		}

next:		
		i++;
	}
}

/**
  *  @brief Config all pins to the right pull state based on pmap_func table before soc sleep.
  *  @note should set pmap_func.SleepPuPd corectly based on your board.
  *  @retval None       
  */
CONFIG_FW_CRITICAL_CODE_SECTION
void pinmap_sleep(void)
{
	int i = 0;
	int PinName = 0;

	for (;;) {
		/*  Check if search to end */
		if (pmap_func_sleep[i].PinName == _PNC) {
			break;
		}

		/* keep active level when GPIO_PuPd_KEEP */
		if (pmap_func_sleep[i].SleepPuPd == GPIO_PuPd_KEEP)
			goto next;
		PinName = pmap_func_sleep[i].PinName;
		
		/* sleep PuPd set */
		if (pmap_func_sleep[i].SleepPuPd == GPIO_PuPd_SHUTDOWN) {
			PAD_CMD(PinName, DISABLE);
		} else {
			PAD_CMD(PinName, ENABLE);
			PAD_PullCtrl(PinName, pmap_func_sleep[i].SleepPuPd);
			//DBG_8195A("%08x: %08x\n", &(PINMUX->PADCTR[pmap_func[i].PinName]), (PINMUX->PADCTR[pmap_func[i].PinName]));
		}

next:	
		i++;
	}
}

/**
  *  @brief Config all pins to the right pull state based on pmap_func table before soc deep sleep.
  *  @note should set pmap_func.DSleepPuPd corectly based on your board.
  *  @retval None       
  */
CONFIG_FW_CRITICAL_CODE_SECTION
void pinmap_deepsleep(void)
{
	int i = 0;

	for (;;) {
		/*  Check if search to end */
		if (pmap_func[i].PinName == _PNC) {
			break;
		}

		/* keep active level when GPIO_PuPd_NOPULL */
		if (pmap_func[i].DSleepPuPd == GPIO_PuPd_KEEP)
			goto next;
		
		/* sleep PuPd set */
		if (pmap_func[i].DSleepPuPd == GPIO_PuPd_SHUTDOWN)
			PAD_CMD(pmap_func[i].PinName, DISABLE);
		else
			PAD_PullCtrl(pmap_func[i].PinName, pmap_func[i].DSleepPuPd);

next:	
		i++;
	}
}


/**
  *  @brief Config all pins to the right pull state based on pmap_func table after soc wakeup from sleep.
  *  @note should set pmap_func.FuncPuPd corectly based on your board.
  *  @retval None       
  */
CONFIG_FW_CRITICAL_CODE_SECTION
void pinmap_wake(void)
{
	int i = 0;
	int PinName = 0;

	for (;;) {
		/*  Check if search to end */
		if (pmap_func_sleep[i].PinName == _PNC) {
			break;
		}

		/* pupd not change when sleep */
		if (pmap_func_sleep[i].SleepPuPd == GPIO_PuPd_KEEP)
			goto next;
		PinName = pmap_func_sleep[i].PinName;
		
		/* active PuPd set */
		if (pmap_func[PinName].FuncPuPd == GPIO_PuPd_SHUTDOWN) {
			PAD_CMD(PinName, DISABLE);
		} else {
			PAD_PullCtrl(PinName, pmap_func[PinName].FuncPuPd);
		}

next:
		i++;
	}
}

/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
