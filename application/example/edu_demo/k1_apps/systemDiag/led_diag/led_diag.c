/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include "aos/cli.h"
#include "ulog/ulog.h"
#include "led_diag.h"
#include "../../menu.h"
#include "../systemDiag.h"

MENU_COVER_TYP led_diag_cover = {
    MENU_COVER_TEXT,
    "led_diag",
};
MENU_TASK_TYP led_diag_tasks = {
    led_diag_init,
    led_diag_uninit // NULL remember to check! elliott
};
MENU_TYP led_diag = {
    "led_diag",
    &led_diag_cover,
    &led_diag_tasks, NULL};

int led_diag_init(void)
{
    printf("led_diag_init\n");
    return 0;
}

int led_diag_uninit(void)
{
    printf("led_diag_uninit\n");
    return 0;
}