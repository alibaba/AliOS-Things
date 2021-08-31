/*
 * Copyright (C) 2020-2023 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <ulog/ulog.h>
#include "oled/oled.h"

void oled_init(void)
{
    sh1106_init();
}

void oled_show(oled_str_e str)
{
    OLED_Clear();
    if (str == OLED_STR_WAKEUP) {
        OLED_Show_String(14, 24, "Hi, I am here!", 16, 1);
    } else if (str == OLED_STR_IDLE) {
        OLED_Show_String(6, 12, "I am your HaaS", 16, 1);
        OLED_Show_String(4, 30, "voice assistant!", 16, 1);
    } else if (str == OLED_STR_LIGHT_ON) {
        OLED_Show_String(14, 24, "Turn on ...", 16, 1);
    } else if (str == OLED_STR_LIGHT_OFF) {
        OLED_Show_String(14, 24, "Turn off ...", 16, 1);
    }
    OLED_Refresh_GRAM();
}
