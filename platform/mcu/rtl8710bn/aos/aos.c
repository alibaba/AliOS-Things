/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include <k_api.h>
#include <aos/kernel.h>
#include <stdio.h>
#include <stdlib.h>

#include "diag.h"
#include "platform_stdlib.h"
#include "rtl8710b.h"
#include "core_cm4.h"

#include "wifi_conf.h"
#include "wlan_intf.h"
#include "wifi_constants.h"

#include "osdep_service.h"

#define AOS_START_STACK 2048

ktask_t *g_aos_init;

static kinit_t kinit;

extern int application_start(int argc, char **argv);
extern int aos_framework_init(void);
extern void board_init(void);

extern uint32_t SystemCoreClock;

static void hal_init()
{
    board_init();
}

extern void hw_start_hal(void);

static void board_cli_init(void)
{
     kinit.argc = 0;
     kinit.argv = NULL;
     kinit.cli_enable = 1;
}


void dump_mem_info(){}
static void hal_wlan_init()
{
	if ( rtl_cryptoEngine_init() != 0 ) {
		DiagPrintf("crypto engine init failed\r\n");
	}

	/* Initialize log uart and at command service */
	ReRegisterPlatformLogUart();

#if CONFIG_LWIP_LAYER
	/* Initilaize the LwIP stack */
	LwIP_Init();
#endif

#if CONFIG_WLAN
	wifi_on(RTW_MODE_STA);
	wifi_disable_powersave();
#if CONFIG_AUTO_RECONNECT
	//setup reconnection flag
	wifi_set_autoreconnect(RTW_AUTORECONNECT_INFINITE);
#endif
	printf("\n\r%s(%d), Available heap 0x%x", __FUNCTION__, __LINE__, rtw_getFreeHeapSize());	
#endif
}

 void sys_init_func(void)
{
    hal_init();

    hw_start_hal();

    hal_wlan_init();
        
    board_cli_init();

    aos_kernel_init(&kinit);

    krhino_task_dyn_del(NULL);
}

void main(void)
{
    aos_init();

    krhino_task_dyn_create(&g_aos_init, "aos-init", 0, AOS_DEFAULT_APP_PRI , 0, AOS_START_STACK, (task_entry_t)sys_init_func, 1);
    
    SysTick_Config(SystemCoreClock/RHINO_CONFIG_TICKS_PER_SECOND);

    aos_start();

    return;
}
