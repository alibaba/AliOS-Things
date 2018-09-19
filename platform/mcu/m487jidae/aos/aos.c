/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include <k_api.h>
#include <aos/kernel.h>
#include <stdio.h>
#include <stdlib.h>
#include "board.h"

#define AOS_START_STACK 1536

ktask_t *g_aos_init;
static kinit_t kinit;

extern int aos_framework_init(void);
extern void board_init(void);
extern void numicro_soc_init(void);
extern void hw_start_hal(void);

static void var_init()
{
    kinit.argc = 0;
    kinit.argv = NULL;
    kinit.cli_enable = 1;
}

static void sys_init(void)
{
    /* Call initialize functions in every board implementation. */
		numicro_soc_init();

    /* Lock protected registers */
    //SYS_LockReg();

    board_init();

    hw_start_hal();
    
    var_init();
    
    #ifdef WITH_LWIP
    lwip_tcpip_init();
    #endif

    aos_kernel_init(&kinit);
}

int main(void)
{
    /* Specify multiple heap region and size for AliOS-things. */
    aos_heap_set();

    /* Do krhino initialization */
    aos_init();

    /* Create Task for system initialization and demo. */
    krhino_task_dyn_create ( &g_aos_init, "aos-init", 0, AOS_DEFAULT_APP_PRI, 0, AOS_START_STACK, (task_entry_t)sys_init, 1 );

    /* Start scheduling & execute first task. */
    aos_start();

    return 0;
}
