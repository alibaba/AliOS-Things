#include "hal/hal.h"
#include "hal_gpio_stm32l4.h"

enum {
    LOW = 0,
    HIGH,
    INVAILD
};

#define LED1_ALPS    HAL_GPIO_22

/*
* @ function comments
* stm32l476rg-nucleo privide one LED for users to control,
* here is the interface to turn on/off this LED inside the board
* @ input para: bool(1/0) which means gpio active level
* @ return para: void
*/
void board_drv_led_ctrl(bool active)
{
    uint8_t gpio_set = 1;
    uint8_t gpio_reset = 0;
    gpio_dev_t gpio_led1 = {LED1_ALPS, OUTPUT_PUSH_PULL, &gpio_set};
    if (HIGH == active)
        hal_gpio_output_high(&gpio_led1);
    if (LOW == active)
        hal_gpio_output_low(&gpio_led1);
}
