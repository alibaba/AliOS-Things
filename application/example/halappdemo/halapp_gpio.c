/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <stdlib.h>
#include <aos/kernel.h>
#include "aos/init.h"
#include "aos/hal/gpio.h"
#include "board.h"
#include <k_api.h>

#if (AOS_HAL_GPIO_ENABLED > 0)

/*
Set gpio_out equal to gpio_in
*/
void hal_gpio_app_out(void)
{
    int        cnt = 20;
    uint32_t   val;
    gpio_dev_t gpio_out;
    gpio_dev_t gpio_in;
    int        ret;

    printf("hal_gpio_app_out start\r\n");

    gpio_out.port   = HALAPP_GPIO_OUT;
    gpio_out.config = OUTPUT_PUSH_PULL;
    gpio_out.priv   = NULL;
    hal_gpio_init(&gpio_out);

    gpio_in.port   = HALAPP_GPIO_IN;
    gpio_in.config = INPUT_HIGH_IMPEDANCE;
    gpio_in.priv   = NULL;
    hal_gpio_init(&gpio_in);

    while ((cnt--) > 0) {
        ret = hal_gpio_input_get(&gpio_in, &val);
        if (0 == ret) {
            if (0 == val) {
                hal_gpio_output_low(&gpio_out);
            } else if (1 == val) {
                hal_gpio_output_high(&gpio_out);
            }
        }
        aos_msleep(1000);
    }

    printf("hal_gpio_app_out end\r\n");
}

/*
Set gpio_out opposite to gpio_in
*/
void hal_gpio_app_toggle(void)
{
    int        cnt = 20;
    uint32_t   val;
    gpio_dev_t gpio_out;
    gpio_dev_t gpio_in;
    int        ret;

    printf("hal_gpio_app_toggle start\r\n");

    gpio_out.port   = HALAPP_GPIO_OUT;
    gpio_out.config = OUTPUT_PUSH_PULL;
    gpio_out.priv   = NULL;
    hal_gpio_init(&gpio_out);

    gpio_in.port   = HALAPP_GPIO_IN;
    gpio_in.config = INPUT_HIGH_IMPEDANCE;
    gpio_in.priv   = NULL;
    hal_gpio_init(&gpio_in);

    while ((cnt--) > 0) {
        ret = hal_gpio_input_get(&gpio_in, &val);

        if (0 == ret) {
            if (0 == val) {
                hal_gpio_output_high(&gpio_out);
            } else if (1 == val) {
                hal_gpio_output_low(&gpio_out);
            }
        }
        aos_msleep(1000);
    }

    printf("hal_gpio_app_toggle end\r\n");
}

static gpio_dev_t g_gpio_out;

static void hal_gpio_int_fun(gpio_dev_t *gpio)
{
    if (NULL == gpio) {
        return;
    }

    hal_gpio_output_toggle(gpio);
}

/* toggle gpio_out in interrupt of gpio_int */
void hal_gpio_app_int(void)
{
    uint32_t   val;
    gpio_dev_t gpio_int;
    int        ret;

    printf("hal_gpio_app_int start\r\n");

    memset(&g_gpio_out, 0, sizeof(gpio_dev_t));

    g_gpio_out.port   = HALAPP_GPIO_OUT;
    g_gpio_out.config = OUTPUT_PUSH_PULL;
    g_gpio_out.priv   = NULL;
    hal_gpio_init(&g_gpio_out);

    gpio_int.port   = HALAPP_GPIO_IN;
    gpio_int.config = IRQ_MODE;
    gpio_int.priv   = NULL;
    hal_gpio_enable_irq(&gpio_int, IRQ_TRIGGER_RISING_EDGE, hal_gpio_int_fun, &g_gpio_out);

    /* sleep 10s */
    aos_msleep(10000);

    hal_gpio_disable_irq(&gpio_int);

    printf("hal_gpio_app_int end\r\n");
}

#endif
