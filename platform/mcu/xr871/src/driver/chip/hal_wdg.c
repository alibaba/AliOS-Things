/**
  * @file  hal_wdg.c
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

#include "pm/pm.h"

#include "driver/chip/hal_wdg.h"
#include "hal_base.h"

/* Note: Has no 32000HZ clock source, use WDG_CLK_32768HZ by default  */
#define WDG_CLK_SRC_SHIFT	8
#define WDG_CLK_SRC_MASK	(0x1U << WDG_CLK_SRC_SHIFT)
typedef enum {
	WDG_CLK_32000HZ = (0U << WDG_CLK_SRC_SHIFT),
	WDG_CLK_32768HZ = (1U << WDG_CLK_SRC_SHIFT)
} WDG_ClkSrc;

#define WDG_RELOAD_VAL	((0xA57 << 1) | 1)

typedef struct {
	WDG_IRQCallback		callback;
	void               *arg;
} WDG_Private;

static WDG_Private gWdgPrivate;

static void WDG_EnableIRQ(void)
{
	HAL_SET_BIT(WDG->IRQ_EN, WDG_IRQ_EN_BIT);
}

static void WDG_DisableIRQ(void)
{
	HAL_CLR_BIT(WDG->IRQ_EN, WDG_IRQ_EN_BIT);
}

static int WDG_IsPendingIRQ(void)
{
	return HAL_GET_BIT(WDG->IRQ_STATUS, WDG_IRQ_PENDING_BIT);
}

static void WDG_ClearPendingIRQ(void)
{
	HAL_SET_BIT(WDG->IRQ_STATUS, WDG_IRQ_PENDING_BIT);
}

void WDG_IRQHandler(void)
{
	if (WDG_IsPendingIRQ()) {
		WDG_ClearPendingIRQ();
		if (gWdgPrivate.callback) {
			gWdgPrivate.callback(gWdgPrivate.arg);
		}
	}
}

#ifdef CONFIG_PM
static WDG_InitParam hal_wdg_param;
static uint32_t hal_wdg_suspending = 0;
static uint32_t hal_wdg_runing = 0;

static int wdg_suspend(struct soc_device *dev, enum suspend_state_t state)
{
	hal_wdg_suspending = 1;

	HAL_WDG_DeInit();
	//HAL_DBG("%s okay\n", __func__);

	return 0;
}

static int wdg_resume(struct soc_device *dev, enum suspend_state_t state)
{
	HAL_WDG_Init(&hal_wdg_param);
	//HAL_DBG("%s okay\n", __func__);

	if (hal_wdg_runing)
		HAL_WDG_Start();
	else
		HAL_WDG_Stop();

	hal_wdg_suspending = 0;

	return 0;
}

static struct soc_device_driver wdg_drv = {
	.name = "wdg",
	.suspend = wdg_suspend,
	.resume = wdg_resume,
};

static struct soc_device wdg_dev = {
	.name = "wdg",
	.driver = &wdg_drv,
};

#define WDG_DEV (&wdg_dev)
#endif

/**
 * @brief Initialize the watchdog according to the specified parameters
 * @param[in] param Pointer to WDG_InitParam structure
 * @retval HAL_Status, HAL_OK on success
 */
HAL_Status HAL_WDG_Init(const WDG_InitParam *param)
{
	/* enable clock */

	/* release reset */

	/* init parameters */
	WDG->MODE = param->timeout; /* NB: it will clear WDG_IRQ_EN_BIT (stop WDG) */
	WDG->CFG = param->event | WDG_CLK_32768HZ;

	if (param->event == WDG_EVT_RESET) {
		WDG->RESET_CTRL = ((param->resetCycle << WDG_RESET_CYCLE_SHIFT) &
		                   WDG_RESET_CYCLE_MASK);
	} else if (param->event == WDG_EVT_INTERRUPT) {
		gWdgPrivate.callback = param->callback;
		gWdgPrivate.arg = param->arg;
		if (WDG_IsPendingIRQ()) {
			WDG_ClearPendingIRQ();
		}
		WDG_EnableIRQ();
		HAL_NVIC_SetPriority(WDG_IRQn, NVIC_PERIPHERAL_PRIORITY_DEFAULT);
		HAL_NVIC_EnableIRQ(WDG_IRQn);
	} else {
		HAL_ERR("Invalid event type 0x%x\n", param->event);
		return HAL_ERROR;
	}

#ifdef CONFIG_PM
	if (!hal_wdg_suspending) {
		memcpy(&hal_wdg_param, param, sizeof(WDG_InitParam));
		pm_register_ops(WDG_DEV);
	}
#endif

	return HAL_OK;
}

/**
 * @brief DeInitialize the watchdog
 * @retval HAL_Status, HAL_OK on success
 */
HAL_Status HAL_WDG_DeInit(void)
{
	HAL_WDG_Stop();

#ifdef CONFIG_PM
	if (!hal_wdg_suspending) {
		pm_unregister_ops(WDG_DEV);
	}
#endif

	/* disable IRQ */
	HAL_NVIC_DisableIRQ(WDG_IRQn);
	WDG_DisableIRQ();
	if (WDG_IsPendingIRQ()) {
		WDG_ClearPendingIRQ();
	}

	gWdgPrivate.callback = NULL;
	gWdgPrivate.arg = NULL;

	/* force reset */

	/* disable clock */

	return HAL_OK;
}

/**
 * @brief Feed the watchdog
 * @note When watchdog running, reset system or IRQ event will be triggered if
 *       no feeding executed in the interval configured by HAL_WDG_Init().
 * @return None
 */
void HAL_WDG_Feed(void)
{
	WDG->CTRL = WDG_RELOAD_VAL;
}

/**
 * @brief Start the watchdog
 * @return None
 */
void HAL_WDG_Start(void)
{
	HAL_WDG_Feed();
#ifdef CONFIG_PM
	if (!hal_wdg_suspending)
		hal_wdg_runing = 1;
#endif
	HAL_SET_BIT(WDG->MODE, WDG_EN_BIT);
}

/**
 * @brief Stop the watchdog
 * @return None
 */
void HAL_WDG_Stop(void)
{
	HAL_CLR_BIT(WDG->MODE, WDG_EN_BIT);
#ifdef CONFIG_PM
	if (!hal_wdg_suspending)
		hal_wdg_runing = 0;
#endif
}

/**
 * @brief Reboot system using the watchdog
 * @return None
 */
void HAL_WDG_Reboot(void)
{
	HAL_DisableIRQ();
	HAL_PRCM_DisableSys2();
	HAL_PRCM_DisableSys2Power();
	WDG->MODE = WDG_TIMEOUT_500MS; /* NB: it will clear WDG_IRQ_EN_BIT (stop WDG) */
	WDG->CFG = WDG_EVT_RESET | WDG_CLK_32768HZ;
	WDG->RESET_CTRL = ((WDG_DEFAULT_RESET_CYCLE << WDG_RESET_CYCLE_SHIFT) &
	                   WDG_RESET_CYCLE_MASK);
	HAL_WDG_Start();
	while (1) ;
}
