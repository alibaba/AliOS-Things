/*
 *  Hello World
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

//#include "FreeRTOS.h"
//#include "task.h"
//#include "semphr.h" 

#include "main.h"
//#include "main_test.h"
#if CONFIG_WLAN
#include "wifi_conf.h"
#include "wlan_intf.h"
#include "wifi_constants.h"
#endif
#include "lwip_netconf.h"
#include <platform/platform_stdlib.h>
#include "osdep_service.h"


#ifndef CONFIG_INIT_NET
#define CONFIG_INIT_NET             1
#endif
#ifndef CONFIG_INTERACTIVE_MODE
#define CONFIG_INTERACTIVE_MODE     1
#endif

#define STACKSIZE                   (512 + 768)

//xSemaphoreHandle uart_rx_interrupt_sema = NULL;

void init_thread(void *param)
{

#if CONFIG_INIT_NET
#if CONFIG_LWIP_LAYER
	/* Initilaize the LwIP stack */
	LwIP_Init();
#endif
#endif
#if CONFIG_WIFI_IND_USE_THREAD
	wifi_manager_init();
#endif
#if CONFIG_WLAN
	wifi_on(RTW_MODE_STA);
#if CONFIG_AUTO_RECONNECT
	//setup reconnection flag
	wifi_set_autoreconnect(1);
#endif
	printf("\n\r%s(%d), Available heap 0x%x", __FUNCTION__, __LINE__, rtw_getFreeHeapSize());	
#endif

#if CONFIG_INTERACTIVE_MODE
 	/* Initial uart rx swmaphore*/
	//vSemaphoreCreateBinary(uart_rx_interrupt_sema);
	//xSemaphoreTake(uart_rx_interrupt_sema, 1/portTICK_RATE_MS);
	//start_interactive_mode();
#endif	

	/* Kill init thread after all init tasks done */
	krhino_task_dyn_del(NULL);
}


void wlan_network()
{
        ktask_t *g_wlan_init_task;
	if(krhino_task_dyn_create(&g_wlan_init_task, "wlan_init", 0, 6, 0, STACKSIZE, (task_entry_t)init_thread, 1) != RHINO_SUCCESS)
		printf("\n\r%s xTaskCreate(init_thread) failed", __FUNCTION__);
}
