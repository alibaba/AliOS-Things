/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <aos/kernel.h>
#include "aos/init.h"
#include "board.h"
#include <k_api.h>

#ifndef AOS_BINS
extern int application_start(int argc, char *argv[]);
#endif

extern void board_tick_init(void);
extern void board_stduart_init(void);
extern void board_dma_init(void);
extern void board_gpio_init(void);
extern void board_wifi_init(void);

/*
main task stask size(byte)
*/
#ifndef AOS_MAIN_TASK_STACK_SIZE
#define AOS_MAIN_TASK_STACK_SIZE 1024
#endif

/*
If board have no component for example board_xx_init, it indicates that this app does not support this board.
Set the correspondence in file platform\board\aaboard_demo\ucube.py.
*/
extern void board_basic_init(void);

/*  For user config
    kinit.argc = 0;
    kinit.argv = NULL;
    kinit.cli_enable = 1;
*/
static kinit_t kinit = {0, NULL, 1};
static ktask_t *g_main_task;

/**
  * @brief Board Initialization Function
  * @param None
  * @retval None
  */
void board_init(void)
{
    board_tick_init();
    board_stduart_init();
    board_dma_init();
    board_gpio_init();
}

void maintask(void* arg)
{
    aos_components_init(&kinit);

#ifndef AOS_BINS
    application_start(kinit.argc, kinit.argv);  /* jump to app entry */
#endif
}

/*
aos_init\aos_start has been called in boot.
user_init is in a task also called in boot.
*/
int user_init(void)
{
    board_init();
    board_wifi_init();

    /* main task to run */
    aos_task_new("main_task", maintask, NULL ,AOS_MAIN_TASK_STACK_SIZE);

    return 0;
}
