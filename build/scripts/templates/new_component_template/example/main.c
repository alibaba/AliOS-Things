/*
 * Copyright (C) 2015-2020 @manufacturename@
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

/*
If board have no component for example board_xx_init, it indicates that this app does not support this board.
Set the correspondence in file platform\board\aaboard_demo\ucube.py.
*/
extern void board_tick_init(void);

/*  For user config
    kinit.argc = 0;
    kinit.argv = NULL;
    kinit.cli_enable = 1;
*/
static kinit_t kinit = {0, NULL, 1};

/**
  * @brief Board Initialization Function
  * @param None
  * @retval None
  */
void board_init(void)
{
    board_tick_init();
}

void aos_maintask(void* arg)
{
    board_init();
    aos_components_init(&kinit);

#ifndef AOS_BINS
    application_start(kinit.argc, kinit.argv);  /* jump to app entry */
#endif
}


