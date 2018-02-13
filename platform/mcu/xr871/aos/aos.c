/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <aos.h>
#include <k_api.h>
#include <aos/kernel.h>
#include <vfs.h>
#include <fatfs.h>

#define AOS_START_STACK 2048

ktask_t *g_aos_init;

extern void board_init(void);
extern void aos_cli_ext_init(void);
extern int application_start(int argc, char **argv);

#include "net/wlan/wlan.h"

static void wifi_join(char* ssid, char* psk)
{
	wlan_sta_set(ssid, strlen(psk), psk);
	wlan_sta_enable();
}

static void aos_system_init(void)
{
	int i;

	soc_driver_init();

	printf("aos_init now...\n");

	soc_system_init();

	hal_init_post();

	//wifi_join("\"AW2\"", "\"1qaz@WSX\"");
	//wifi_join("\"MI\"", "\"xiaomi123\"");
	//wifi_join("\"mao\"", "\"11111111\"");
	//wifi_join("\"mao\"", NULL);

	board_init();

	vfs_init();
	vfs_device_init();

#ifdef CONFIG_AOS_FATFS_SUPPORT
	/* if enable the macro, please also enable CONFIG_AOS_FATFS_SUPPORT_MMC
	 * and include <fatfs.h>, add modules.fs.fatfs to component dependence
	 */
	fatfs_register();
#endif

	aos_cli_init();
	aos_cli_ext_init();
	aos_kv_init();
	aos_loop_init();

	aos_framework_init();

	application_start(0, NULL);
}

void aos_startup(void)
{
	soc_sys_mem_init();

	soc_systick_init();

	krhino_init();

	krhino_task_dyn_create(&g_aos_init, "aos-init", 0, 10, 0, AOS_START_STACK, aos_system_init, 1);

	krhino_start();
}
