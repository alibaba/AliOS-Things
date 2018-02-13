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

#include <stdio.h>
#include "compiler.h"
#include "version.h"
#include "pm/pm.h"

#include "common/board/board.h"
#include "sysinfo.h"
#include "img_ctrl.h"
#include "net_ctrl.h"
#include "sys_ctrl/sys_ctrl.h"
#include "fwk_debug.h"

#if (PRJCONF_SOUNDCARD0_EN || PRJCONF_SOUNDCARD1_EN)
#include "audio/manager/audio_manager.h"
#include "audio/pcm/audio_pcm.h"
#endif
#if PRJCONF_CONSOLE_EN
#include "console/console.h"
#include "command.h"
#endif
#ifdef __PRJ_CONFIG_XIP
#include "driver/chip/hal_xip.h"
#include "sys/image.h"
#endif

#define PLATFORM_SHOW_INFO	0	/* for internal debug only */

/* default app pm mode */
#if (PRJCONF_PM_EN && !defined(PRJCONF_PM_MODE))
#define PRJCONF_PM_MODE     (PM_SUPPORT_SLEEP | \
                             PM_SUPPORT_STANDBY | \
                             PM_SUPPORT_POWEROFF)
#endif

/* default net pm mode */
#if (PRJCONF_NET_PM_EN && !defined(PRJCONF_NET_PM_MODE))
#define PRJCONF_NET_PM_MODE (PM_SUPPORT_HIBERNATION | \
                             PM_SUPPORT_POWEROFF)
#endif


#if PLATFORM_SHOW_INFO
static void platform_show_info(void)
{
	extern uint8_t	__data_end__[];
	extern uint8_t	_edata[];
	extern uint8_t	__bss_start__[];
	extern uint8_t	__bss_end__[];
	extern uint8_t	__end__[];
	extern uint8_t	end[];
	extern uint8_t	__HeapLimit[];
	extern uint8_t	__StackLimit[];
	extern uint8_t	__StackTop[];
	extern uint8_t	__stack[];
	extern uint8_t	_estack[];

	printf("__data_end__  %p\n", __data_end__);
	printf("_edata        %p\n", _edata);
	printf("__bss_start__ %p\n", __bss_start__);
	printf("__bss_end__   %p\n", __bss_end__);
	printf("__end__       %p\n", __end__);
	printf("end           %p\n", end);
	printf("__HeapLimit   %p\n", __HeapLimit);
	printf("__StackLimit  %p\n", __StackLimit);
	printf("__StackTop    %p\n", __StackTop);
	printf("__stack       %p\n", __stack);
	printf("_estack       %p\n", _estack);
	printf("\n");

	printf("heap space [%p, %p), size %u\n\n",
	       __end__, _estack - PRJCONF_MSP_STACK_SIZE,
	       _estack - __end__ - PRJCONF_MSP_STACK_SIZE);

	printf("cpu  clock %u Hz\n", HAL_GetCPUClock());
	printf("ahb1 clock %u Hz\n", HAL_GetAHB1Clock());
	printf("ahb2 clock %u Hz\n", HAL_GetAHB2Clock());
	printf("apb  clock %u Hz\n", HAL_GetAPBClock());
	printf("HF   clock %u Hz\n", HAL_GetHFClock());
	printf("LF   clock %u Hz\n", HAL_GetLFClock());
	printf("\n");
}
#endif /* PLATFORM_SHOW_INFO */

#ifdef __PRJ_CONFIG_XIP
static void platform_xip_init(void)
{
	uint32_t addr;

	addr = image_get_section_addr(IMAGE_APP_XIP_ID);
	if (addr == IMAGE_INVALID_ADDR) {
		FWK_ERR("no xip section\n");
		return;
	}

	/* TODO: check section's validity */

	HAL_Xip_Init(PRJCONF_IMG_FLASH, addr + IMAGE_HEADER_SIZE);
}
#endif /* __PRJ_CONFIG_XIP */

#if PRJCONF_WDG_EN
static void platform_wdg_feed(void *arg)
{
	FWK_DBG("feed wdg @ %u sec\n", OS_GetTime());
	HAL_WDG_Feed();
}

static void platform_wdg_start(void)
{
	WDG_InitParam param;
	OS_Timer_t timer;

	/* init watchdog */
	param.event = WDG_EVT_RESET;
	param.timeout = PRJCONF_WDG_TIMEOUT;
	param.resetCycle = WDG_DEFAULT_RESET_CYCLE;
	HAL_WDG_Init(&param);

	/* create OS timer to feed watchdog */
	OS_TimerSetInvalid(&timer);
	if (OS_TimerCreate(&timer, OS_TIMER_PERIODIC, platform_wdg_feed, NULL,
	                   PRJCONF_WDG_FEED_PERIOD) != OS_OK) {
		FWK_WRN("wdg timer create failed\n");
		HAL_WDG_DeInit();
		return;
	}

	HAL_WDG_Start(); /* start watchdog */
	OS_TimerStart(&timer); /* start OS timer to feed watchdog */
}
#endif /* PRJCONF_WDG_EN */

/* init basic platform hardware and services */
__weak void platform_init_level0(void)
{
	HAL_Flash_Init(PRJCONF_IMG_FLASH);
#if PRJCONF_CE_EN
	HAL_CE_Init();
#endif

	img_ctrl_init(PRJCONF_IMG_FLASH, PRJCONF_IMG_ADDR, PRJCONF_IMG_SIZE);
#if (defined(__PRJ_CONFIG_XIP))
	platform_xip_init();
#endif
}

/* init standard platform hardware and services */
__weak void platform_init_level1(void)
{
#if PRJCONF_UART_EN
	if ((BOARD_SUB_UART_ID < UART_NUM) &&
	    (BOARD_SUB_UART_ID != BOARD_MAIN_UART_ID)) {
		board_uart_init(BOARD_SUB_UART_ID);
	}
#endif

#if PRJCONF_SYS_CTRL_EN
	sys_ctrl_create();
  #if PRJCONF_NET_EN
	net_ctrl_init();
  #endif
#endif

	sysinfo_init();

#if PRJCONF_CONSOLE_EN
	console_param_t cparam;
	cparam.uart_id = BOARD_MAIN_UART_ID;
	cparam.cmd_exec = main_cmd_exec;
	console_start(&cparam);
#endif

#if PRJCONF_PM_EN
	pm_mode_platform_select(PRJCONF_PM_MODE);
#endif

#if PRJCONF_NET_EN
	net_sys_init();

	struct sysinfo *sysinfo = sysinfo_get();
	net_sys_start(sysinfo->wlan_mode);

  #if PRJCONF_NET_PM_EN
	pm_register_wlan_power_onoff(net_sys_onoff, PRJCONF_NET_PM_MODE);
  #endif
#endif /* PRJCONF_NET_EN */

#if PRJCONF_WDG_EN
	platform_wdg_start();
#endif
}

/* init extern platform hardware and services */
__weak void platform_init_level2(void)
{
#if PRJCONF_SPI_EN
	board_spi_init(BOARD_SPI_PORT);
#endif

#if PRJCONF_MMC_EN
	SDC_InitTypeDef sdc_param;
  #ifdef CONFIG_DETECT_CARD
	sdc_param.cd_mode = PRJCONF_MMC_DETECT_MODE;
  #endif
	HAL_SDC_Init(0, &sdc_param);
#endif

#if (PRJCONF_SOUNDCARD0_EN || PRJCONF_SOUNDCARD1_EN)
	aud_mgr_init();
	snd_pcm_init();
  #if PRJCONF_SOUNDCARD0_EN
	board_soundcard0_init();
  #endif
  #if PRJCONF_SOUNDCARD1_EN
	board_soundcard1_init();
  #endif
#endif
}

void platform_init(void)
{
	printf("XRadio IoT WLAN SDK %s\n\n", SDK_VERSION_STR);
#if PLATFORM_SHOW_INFO
	platform_show_info();
#endif

	platform_init_level0();
	platform_init_level1();
	platform_init_level2();
}
