/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include "GUIDEMO.h"
#include "soc_init.h"

extern void GUIDEMO_Main(void);

extern int key_flag;
extern int sound_record;
extern int sound_play;
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	GUI_KEY_A10 key = 0;

	switch (GPIO_Pin) {
		case SW_FUNC_A_Pin:
			key = GUI_KEY_FUNC_A;
      sound_record = 1;
			break;
		case SW_FUNC_B_Pin:
			key = GUI_KEY_FUNC_B;
      sound_play = 1;
			break;
		case SW_WIFI_Pin:
			key = GUI_KEY_WIFI;
      ++key_flag;
			break;
		default:
			return;
	}
	GUI_StoreKeyMsg(key, 1);
	GUI_StoreKeyMsg(key, 0);
}

static void app_delayed_action(void *arg)
{
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
