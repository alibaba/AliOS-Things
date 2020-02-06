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

/* gpio app macro default value
   need to ajust for each board
*/
#ifndef HALAPP_GPIO_OUT
#define HALAPP_GPIO_OUT     8   //PA8
#endif

#ifndef HALAPP_GPIO_IN
#define HALAPP_GPIO_IN      9
#endif

#ifndef HALAPP_GPIO_INT
#define HALAPP_GPIO_INT     9
#endif

#ifndef ON_BOARD_LED02
#define ON_BOARD_LED02      23 //PB0
#endif

/*
Set gpio_out equal to gpio_in
*/
void hal_gpio_app_out(void)
{
    int cnt = 20;
    int ret;
    uint32_t val;
    gpio_dev_t gpio_out; /*GPIO8*/
    gpio_dev_t gpio_in; /*GPIO9*/

    printf("hal_gpio_app_out in\r\n");

    gpio_out.port = HALAPP_GPIO_OUT;
    gpio_out.config = OUTPUT_PUSH_PULL;
    gpio_out.priv = NULL;
    hal_gpio_init(&gpio_out);

    gpio_in.port = HALAPP_GPIO_IN;
    gpio_in.config = INPUT_HIGH_IMPEDANCE;
    gpio_in.priv = NULL;
    hal_gpio_init(&gpio_in);

    while((cnt--) > 0){
        ret = hal_gpio_input_get(&gpio_in, &val);

        if(0 == ret) {
            if(0 == val){
                hal_gpio_output_low(&gpio_out);
            }else if( 1 == val ){
                hal_gpio_output_high(&gpio_out);
            }
        }
        aos_msleep(10);
    }

}

/*
Set gpio_out opposite to gpio_in
*/
void hal_gpio_app_toggle(void)
{
    int cnt = 20;
    int ret;
    uint32_t val;
    gpio_dev_t gpio_out; /*GPIO8*/
    gpio_dev_t gpio_in; /*GPIO9*/

    printf("hal_gpio_app_toggle in\r\n");

    gpio_out.port = HALAPP_GPIO_OUT;
    gpio_out.config = OUTPUT_PUSH_PULL;
    gpio_out.priv = NULL;
    hal_gpio_init(&gpio_out);

    gpio_in.port = HALAPP_GPIO_IN;
    gpio_in.config = INPUT_HIGH_IMPEDANCE;
    gpio_in.priv = NULL;
    hal_gpio_init(&gpio_in);

    while((cnt--) > 0){
        ret = hal_gpio_input_get(&gpio_in, &val);

        if(0 == ret) {
            if(0 == val){
                hal_gpio_output_high(&gpio_out);
            }else if( 1 == val ){
                hal_gpio_output_low(&gpio_out);
            }
        }
        aos_msleep(10);
    }

}

static void hal_gpio_int_fun(gpio_dev_t * gpio)
{
    if(NULL == gpio) {
        return;
    }

    hal_gpio_output_toggle(gpio);
}

/* toggle gpio_out in interrupt of gpio_int */
void hal_gpio_app_int(void)
{
    int cnt = 20;
    int ret;
    uint32_t val;
    gpio_dev_t * pgpio_out = malloc(sizeof(gpio_dev_t)); /*GPIO8*/
    if(NULL == pgpio_out) {
        return;
    }

    printf("hal_gpio_app_int in\r\n");

    memset(pgpio_out,0, sizeof(gpio_dev_t));
    gpio_dev_t gpio_int; /*GPIO9*/

    pgpio_out->port = HALAPP_GPIO_OUT;
    pgpio_out->config = OUTPUT_PUSH_PULL;
    pgpio_out->priv = NULL;
    hal_gpio_init(pgpio_out);

    gpio_int.port = HALAPP_GPIO_IN;
    gpio_int.config = IRQ_MODE;
    gpio_int.priv = NULL;
    hal_gpio_enable_irq(&gpio_int, IRQ_TRIGGER_RISING_EDGE, hal_gpio_int_fun, pgpio_out);
}

#endif
