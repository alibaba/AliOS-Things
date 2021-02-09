/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include "systemDiag.h"
#include "../menu.h"
#include "led_diag/led_diag.h"

MENU_COVER_TYP systemDiag_cover = {MENU_COVER_TEXT,
                                   "systemDiag"};
MENU_TASK_TYP systemDiag_tasks = {
    systemDiag_init,
    systemDiag_uninit};
MENU_TYP* systemDiag_children[] = {
    &led_diag
};
MENU_LIST_TYP systemDiag_child_list = {
    &systemDiag_children,
    1};

MENU_TYP systemDiag = {
    "systemDiag",
    &systemDiag_cover,
    &systemDiag_tasks,
    NULL,
    &systemDiag_child_list,
    };

int systemDiag_init(void)
{
    return 0;
}

int systemDiag_uninit(void)
{
    return 0;
}