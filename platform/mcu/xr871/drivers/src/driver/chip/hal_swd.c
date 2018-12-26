/**
  * @file  hal_nvic.c
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
#include "driver/chip/hal_swd.h"
#include "pm/pm.h"

#ifdef CONFIG_PM

static int8_t g_swd_suspending = 0;

static int swd_suspend(struct soc_device *dev, enum suspend_state_t state)
{
	g_swd_suspending = 1;

	switch (state) {
	case PM_MODE_SLEEP:
		break;
	case PM_MODE_STANDBY:
	case PM_MODE_HIBERNATION:
	case PM_MODE_POWEROFF:
		HAL_SWD_DeInit();
		break;
	default:
		break;
	}

	return 0;
}

static int swd_resume(struct soc_device *dev, enum suspend_state_t state)
{
	switch (state) {
	case PM_MODE_SLEEP:
		break;
	case PM_MODE_STANDBY:
	case PM_MODE_HIBERNATION:
		HAL_SWD_Init();
		break;
	default:
		break;
	}

	g_swd_suspending = 0;
	return 0;
}

static struct soc_device_driver swd_drv = {
	.name = "swd",
	.suspend_noirq = swd_suspend,
	.resume_noirq = swd_resume,
};

static struct soc_device swd_dev = {
	.name = "swd",
	.driver = &swd_drv,
};

#define SWD_DEV (&swd_dev)

#endif /* CONFIG_PM */

/**
 * @brief Initialize the SWD
 * @return None
 */
void HAL_SWD_Init(void)
{
	/* config pinmux */
	HAL_BoardIoctl(HAL_BIR_PINMUX_INIT, HAL_MKDEV(HAL_DEV_MAJOR_SWD, 0), 0);

#ifdef CONFIG_PM
	if (!g_swd_suspending) {
		pm_register_ops(SWD_DEV);
	}
#endif
}

/**
 * @brief DeInitialize the SWD
 * @return None
 */
void HAL_SWD_DeInit(void)
{
#ifdef CONFIG_PM
	if (!g_swd_suspending) {
		pm_unregister_ops(SWD_DEV);
	}
#endif

	/* config pinmux */
	HAL_BoardIoctl(HAL_BIR_PINMUX_DEINIT, HAL_MKDEV(HAL_DEV_MAJOR_SWD, 0), 0);
}
