/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <aos_main.h>
#include <k_api.h>
#include <aos/kernel.h>
#include <aos/init.h>


#define AOS_START_STACK 512
ktask_t *g_aos_init;


static kinit_t kinit = {
    .argc = 0,
    .argv = NULL,
    .cli_enable = 0
};

unsigned int qq_tick;
static void sys_init(void)
{
    int ret = 0;
	
	set_timer();

    printf("core freq at %d Hz\n", get_cpu_freq());

    while(1) {
        
        printf("tick = %d\r\n", qq_tick++);

        //aos_msleep(100);
        krhino_task_sleep(100);

    }
}


void main(void)
{
	_init();
    aos_init();

    krhino_task_dyn_create(&g_aos_init, "aos-init", 0, 5, 0, AOS_START_STACK, sys_init, 1);

    aos_start();
}

