/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */


#include "aos/hal/wdg.h"
#include "wdt_api.h"

#ifndef TRUE
#define TRUE  1
#endif
#ifndef FALSE
#define FALSE 0
#endif

static bool is_enable_handler = FALSE;

void watchdog_irq_handler(uint32_t callback_id) 
{	
	printf("watchdog_irq_handler, callback_id: %d\n", callback_id);
	sys_reset( ) ;	
}
void watchdog_irq_set(void)
{
	if(FALSE == is_enable_handler) {
		watchdog_irq_init(watchdog_irq_handler, 0);
		is_enable_handler = TRUE;
	}
	printf("watchdog_irq_set:  is_enable_handler =%d\n", is_enable_handler);
	return;	
}

int32_t hal_wdg_init(wdg_dev_t *wdg)
{
	uint32_t msecs = wdg->config.timeout;
	printf("hal_wdg_init, set timeout:%d ms\n", msecs);
	watchdog_init(msecs);
	watchdog_irq_set();
	watchdog_start();
	return 0;
}

void hal_wdg_reload(wdg_dev_t *wdg)
{
	watchdog_refresh();
}

int32_t hal_wdg_finalize(wdg_dev_t *wdg)
{
	watchdog_stop();
	return 0;
}

