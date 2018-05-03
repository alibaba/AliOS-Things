/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include "GUIDEMO.h"
#include "soc_init.h"

extern void GUIDEMO_Main(void);

extern int key_flag;
static int old_key_flag;
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	GUI_KEY_A10 key = 0;

	switch (GPIO_Pin) {
		case KEY_1_Pin:
			key = GUI_KEY_1;
      if (key_flag) {
        old_key_flag = key_flag;
        key_flag = 0;
      } else {
        key_flag = old_key_flag;
      }
			break;
		case KEY_2_Pin:
			key = GUI_KEY_2;
      // ++key_flag;
			break;
		case KEY_3_Pin:
			key = GUI_KEY_3;
      if (key_flag)
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
