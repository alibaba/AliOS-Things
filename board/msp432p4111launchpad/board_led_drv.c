/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdbool.h>
#include <stddef.h>

#include "aos/hal/gpio.h"
#include "Board.h"

/*
* @ function comments
* msp432p4111 privide one LED for users to control,
* here is the interface to turn on/off this LED inside the board
* @ input para: bool(1/0) which means gpio active level
* @ return para: void
*/
void board_drv_led_ctrl(bool active)
{
    gpio_dev_t gpio_led = {Board_GPIO_LED0, OUTPUT_PUSH_PULL, NULL};
    if (active)
        hal_gpio_output_high(&gpio_led);
    else
        hal_gpio_output_low(&gpio_led);
}

void board_drv_redled_ctrl(bool active)
{
    gpio_dev_t gpio_led = {Board_GPIO_LED1, OUTPUT_PUSH_PULL, NULL};
    if (active)
        hal_gpio_output_high(&gpio_led);
    else
        hal_gpio_output_low(&gpio_led);
}



