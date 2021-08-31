/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include "aos/init.h"
#include "board.h"
#include <aos/kernel.h>
#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>
#include "build_version.h"

#ifndef AOS_BINS
extern int application_start(int argc, char *argv[]);
#endif

/*
If board have no component for example board_xx_init, it indicates that this app
does not support this board. Set the correspondence in file
platform\board\aaboard_demo\ucube.py.
*/
extern void board_tick_init(void);
extern void board_stduart_init(void);
extern void board_detect();
extern void board_dma_init(void);
extern void board_gpio_init(void);
extern void board_kinit_init(kinit_t *init_args);

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
    board_stduart_init();
    board_dma_init();
    board_gpio_init();
}

void aos_maintask(void *arg)
{
    board_init();
    board_kinit_init(&kinit);
    aos_components_init(&kinit);
    board_detect();
#if (ENABLE_FACTORY_TEST == 1)
    uint8_t image_version[22];
    sprintf(image_version, "VER: %s", BUILD_VERSION);
    printf("\r\n Enter HaaSEDUk1 factorytest model, Version : %s \r\n", image_version);
    int value = 0;
    int re_value = 0;
    int len = 32;
    if (0 != aos_kv_get("factory_test", &value, &len)) {
        if (0 == aos_kv_get("reboot_times", &re_value, &len)) {
            if (re_value++ < 3) {
                aos_kv_set("reboot_times", &re_value, len, 1);
                printf("reboot_times is avild, add it %d!\r\n", re_value);
            } else {
                goto normal_mode;
            }
        } else {
            re_value++;
            printf("reboot_times is not avild, create it %d!\r\n", re_value);
            aos_kv_set("reboot_times", &re_value, len, 1);
        }
    }
factorytest_mode:
        printf("board_test entry here!\r\n");
        board_test();
normal_mode:
#endif
#ifndef AOS_BINS
    application_start(kinit.argc, kinit.argv); /* jump to app entry */
#endif
}
