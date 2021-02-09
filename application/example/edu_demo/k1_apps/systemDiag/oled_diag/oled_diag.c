/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include "aos/cli.h"
#include "ulog/ulog.h"
#include "hal_oled.h"

void ekd_oled_test(char *buf, int len)
{
    OLED_Clear();
    OLED_Show_String(12, 12, buf, 16, 1);
    OLED_Refresh_GRAM();
}
