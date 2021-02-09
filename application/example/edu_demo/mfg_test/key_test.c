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
#include "key.h"
#include "hal_gpio.h"
#include "hal_iomux_haas1000.h"

extern uint32_t led_test_flag;
extern uint32_t auto_test_flag;

static void key_event_handle(key_code_t key_code)
{
     switch (key_code)
     {
        case EDK_KEY_1:
            printf("key 1 press\n");
            led_test_flag = 0;
            break;
        case EDK_KEY_2:
            printf("key 2 press\n");
            led_test_flag = 0;
            break;
        case EDK_KEY_3:
            printf("key 3 press\n");
            led_test_flag = 0;
            break;
        case EDK_KEY_4:
            printf("key 4 press\n");
            led_test_flag = 0;
            break;
        case EDK_KEY_1 | EDK_KEY_2:
            printf("Enter auto_factory_test mode\n");
            auto_test_flag = 1;
        break;
     }
}

void key_test(void)
{
    key_init(key_event_handle);
}