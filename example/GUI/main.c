/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>
#include "stm32l4xx_hal.h"
#include "GUIDEMO.h"
#include <aos/aos.h>
#include <aos/uData.h>

#define DEMO_TASK_STACKSIZE    1024 //512*cpu_stack_t = 2048byte
#define DEMO_TASK_PRIORITY     20

extern void stm32_soc_init(void);
static ktask_t demo_task_obj;
cpu_stack_t demo_task_buf[DEMO_TASK_STACKSIZE];
static kinit_t kinit;
extern int key_flag;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	GUI_KEY_A10 key = 0;

	switch (GPIO_Pin) {
		case SW_FUNC_A_Pin:
			key = GUI_KEY_FUNC_A;
      key_flag = 1;
			break;
		case SW_FUNC_B_Pin:
			key = GUI_KEY_FUNC_B;
      key_flag = 1;
			break;
		case SW_WIFI_Pin:
			key = GUI_KEY_WIFI;
			break;
		default:
			return;
	}
	GUI_StoreKeyMsg(key, 1);
	GUI_StoreKeyMsg(key, 0);
}

void uData_report_demo(input_event_t *event, void *priv_data)
{
    udata_pkg_t buf;
    if ((event == NULL)||(event->type != EV_UDATA)) {
        return;
    }

    if(event->code == CODE_UDATA_REPORT_PUBLISH){
        int ret = 0;
        ret = uData_report_publish(&buf);
        if(ret == 0){
            barometer_data_t* data = (barometer_data_t*)buf.payload;
            printf("uData_application::::::::::::::type = (%d)\n", buf.type);
            printf("uData_application::::::::::::::data = (%d)\n", data->p);
            printf("uData_application:::::::::timestamp = (%d)\n", data->timestamp);
        }
    }
}

int application_start(int argc, char *argv[])
{
    int ret = 0;

    aos_register_event_filter(EV_UDATA, uData_report_demo, NULL);

    ret = uData_subscribe(UDATA_SERVICE_BARO);
    if(ret != 0){
        printf("%s %s %s %d\n", uDATA_STR, __func__, ERROR_LINE, __LINE__);
        return -1;
    }
    aos_loop_run();

    return 0;
}

void demo_task(void *arg)
{
    int ret = 0;
    int count = 0;

    stm32_soc_init();
    kinit.argc = 0;
    kinit.argv = NULL; 
    kinit.cli_enable = 1;
    aos_kernel_init(&kinit);
    GUIDEMO_Main();

    while (1)
    {
        printf("hello world! count %d\n", count++);

        //sleep 1 second
        //krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND);
        HAL_Delay(1000);
    };
}

int main(void)
{
    krhino_init();
    krhino_task_create(&demo_task_obj, "demo_task", 0, DEMO_TASK_PRIORITY, 
        50, demo_task_buf, DEMO_TASK_STACKSIZE, demo_task, 1);

    krhino_start();
    
    return 0;
}

