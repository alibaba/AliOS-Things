/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include "aos/init.h"
#include "board.h"
#include "k1_apps/menu.h"
#include <aos/errno.h>
#include <aos/kernel.h>
#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>
#include <uservice/eventid.h>
// #include "drivers/lcd/st7789v/st7789v_vfs.h"
// #include "drivers/lcd/st7789v/st7789v.h"

int application_start(int argc, char *argv[])
{
    sh1106_init();
    menu_init();
}
