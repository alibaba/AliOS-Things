/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <aos/aos.h>
#include "GUIDEMO.h"
#include "soc_init.h"

extern void GUIDEMO_Main(void);

extern int key_flag;
extern int sound_record;
extern int sound_play;

int handing_shake()
{
	static sys_time_t last_time = 0;
	sys_time_t now_time = 0;
	int ret = 0;

	now_time = krhino_sys_time_get();
	if (now_time - last_time < 200) {
		ret = 1;
	}
	last_time = now_time;

	return ret;
}

void keyA_handle(void *arg)
{
	if (handing_shake())
		return;
	sound_record = 1;
	GUI_StoreKeyMsg(GUI_KEY_FUNC_A, 1);
	GUI_StoreKeyMsg(GUI_KEY_FUNC_A, 0);
}

void keyB_handle(void *arg)
{
	if (handing_shake())
		return;
	sound_play = 1;
	GUI_StoreKeyMsg(GUI_KEY_FUNC_B, 1);
	GUI_StoreKeyMsg(GUI_KEY_FUNC_B, 0);
}

void keyWIFI_handle(void *arg)
{
	if (handing_shake())
		return;
	++key_flag;
	GUI_StoreKeyMsg(GUI_KEY_WIFI, 1);
	GUI_StoreKeyMsg(GUI_KEY_WIFI, 0);
}

static void app_delayed_action(void *arg)
{
	int ret = 0;
	ret |= hal_gpio_enable_irq(&brd_gpio_table[GPIO_SW_FUNC_A], IRQ_TRIGGER_RISING_EDGE, keyA_handle, NULL);
	ret |= hal_gpio_enable_irq(&brd_gpio_table[GPIO_SW_FUNC_B], IRQ_TRIGGER_RISING_EDGE, keyB_handle, NULL);
	ret |= hal_gpio_enable_irq(&brd_gpio_table[GPIO_SW_WIFI], IRQ_TRIGGER_RISING_EDGE, keyWIFI_handle, NULL);
	if (ret != 0) {
		printf("hal_gpio_enable_irq key return failed.\n");
	}
	GUIDEMO_Main();
    LOG("helloworld %s:%d %s\r\n", __func__, __LINE__, aos_task_name());
    aos_post_delayed_action(5000, app_delayed_action, NULL);
}

int application_start(int argc, char *argv[])
{
    aos_post_delayed_action(1000, app_delayed_action, NULL);
    BSP_GUI_init();
    aos_loop_run();

    return 0;
}
