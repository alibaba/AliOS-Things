/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <stdlib.h>
#include <aos/errno.h>
#include <aos/kernel.h>
#include "aos/init.h"
#include "board.h"
#include <k_api.h>
#include "aos/cli.h"
#include "led.h"
#include "hal_gpio.h"
#include "hal_iomux_haas1000.h"
#include "hal_oled.h"
extern uint32_t oled_test_flag;

void oled_test_process()
{
    printf("\r\n\r\n");
    printf("***************************************************************\r\n");
    printf("************************* OLED Test ****************************\r\n");
    printf("***************************************************************\r\n");
    printf("***************************************************************\r\n");
    printf("===== OLED test : Start=====\r\n");

    OLED_Clear();
    OLED_Show_String(12, 12, "Enter factory_test", 16, 1);
    OLED_Refresh_GRAM();
    aos_msleep(1000);
    oled_blink();
}

void oled_blink(void)
{
    unsigned int oled_id = 0;

    while(oled_test_flag) {
        if(oled_id % 2 == 0) {
            OLED_test(1);
        } else {
            OLED_test(0);
        }
        aos_msleep(1000);
        oled_id ++;
    }
}

void ekd_oled_show(char *buf, int len)
{
    OLED_Clear();
    OLED_Show_String(12, 12, buf, 12, 1);
    OLED_Refresh_GRAM();
}
static void handle_oled_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    oled_test_process();
}

static struct cli_command oled_cmd = {
    .name     = "oledtest",
    .help     = "oledtest",
    .function = handle_oled_cmd
};

void oled_test(void)
{
    aos_cli_register_command(&oled_cmd);
    oled_test_process();
}