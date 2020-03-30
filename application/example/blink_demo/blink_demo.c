/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include "aos/kernel.h"
#include "ulog/ulog.h"
#include "board.h"
#include "aos/hal/gpio.h"

/**
 * Brief:
 * This test code shows how to configure LED gpio.
 */
#ifdef LED1
#define GPIO_LED_IO         LED1
#elif LED2
#define GPIO_LED_IO         LED2
#elif LED3
#define GPIO_LED_IO         LED3
#elif LED4
#define GPIO_LED_IO         LED4
#else
#define GPIO_LED_IO         0xffff
#endif

gpio_dev_t led_nucleo;

int application_start(int argc, char *argv[])
{
     /* gpio port config */
    led_nucleo.port = GPIO_LED_IO;
    /* set as output mode */
    led_nucleo.config = OUTPUT_PUSH_PULL;
    /* configure GPIO with the given settings */
    hal_gpio_init(&led_nucleo);

    while (1)
    {
        /* Insert delay 1000 ms */
        aos_msleep(1000);
        printf(" toggle led \n");
        hal_gpio_output_toggle(&led_nucleo);
    }

    return 0;
}
