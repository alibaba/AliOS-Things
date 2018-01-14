/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include <hal/soc/soc.h>

#define BLINK_GPIO 5

gpio_dev_t gpio;

static void app_delayed_action(void *arg)
{
    /* Blink off (output low) */
    hal_gpio_output_low(&gpio);
    aos_msleep(1000);
    /* Blink on (output high) */
    hal_gpio_output_high(&gpio);
    aos_msleep(1000);

    aos_post_delayed_action(0, app_delayed_action, NULL);
}

int application_start(int argc, char *argv[])
{
    gpio.port = BLINK_GPIO;
    gpio.config = OUTPUT_PUSH_PULL;
    hal_gpio_init(&gpio);

    aos_post_delayed_action(1000, app_delayed_action, NULL);
    aos_loop_run();

    return 0;
}

