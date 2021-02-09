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

extern uint32_t led_test_flag;

void led_test_process(void)
{
    unsigned int led_id = 1;

    printf("\r\n\r\n");
    printf("***************************************************************\r\n");
    printf("************************* LED Test ****************************\r\n");
    printf("***************************************************************\r\n");
    printf("*How to test: If the light flashes normally, press the function key *\r\n");
    printf("***************************************************************\r\n");
    printf("===== LED test : Start=====\r\n");

    led_blink();

}

void led_blink(void)
{
    unsigned int led_id = 0;

    while(led_test_flag) {
        if(led_id % 2 == 0) {
            led_switch(1, LED_ON);
            led_switch(2, LED_ON);
            led_switch(3, LED_ON);
        } else {
            led_switch(1, LED_OFF);
            led_switch(2, LED_OFF);
            led_switch(3, LED_OFF);
        }
        aos_msleep(500);
        led_id ++;
    }
}

static void handle_led_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    int id;
    char *onoff;

    if(argc < 2) {
        printf("Usage: led id on/off\n");
        printf("Example: led 3 on\n");
        return;
    }
    id = atoi(argv[1]);
    onoff = argv[2];

    if(0 == strcmp(onoff, "on")) {
        led_switch(id, LED_ON);
    } else if(0 == strcmp(onoff, "off")) {
        led_switch(id, LED_OFF);
    }
}

static struct cli_command led_cmd = {
    .name     = "led",
    .help     = "led [3, on/off]",
    .function = handle_led_cmd
};

void led_test(void)
{
    aos_cli_register_command(&led_cmd);
    led_test_flag = 1;
    led_test_process();
}