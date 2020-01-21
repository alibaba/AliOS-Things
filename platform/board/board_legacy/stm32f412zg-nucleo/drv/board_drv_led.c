
#include "hal_gpio_stm32f4.h"

/*
* @ function comments
* stm32f412g-nucleo privide three LED for users to control,
* here is the interface to turn on/off LED1(green) inside the board
* @ input para: bool(1/0) which means gpio active level
* @ return para: void
*/
void board_drv_led_ctrl(bool active)
{
    gpio_dev_t board_led01 = {ON_BOARD_LED01, OUTPUT_PUSH_PULL, NULL};
    if (active)
        hal_gpio_output_high(&board_led01);
    if (!active)
        hal_gpio_output_low(&board_led01);
}

/*
* @ function comments
* stm32f412g-nucleo privide three LED for users to control,
* here is the interface to turn on/off LED2(blue) inside the board
* @ input para: bool(1/0) which means gpio active level
* @ return para: void
*/
void board_drv_led02_ctrl(bool active)
{
    gpio_dev_t board_led02 = {ON_BOARD_LED02, OUTPUT_PUSH_PULL, NULL};
    if (active)
        hal_gpio_output_high(&board_led02);
    if (!active)
        hal_gpio_output_low(&board_led02);
}

/*
* @ function comments
* stm32f412g-nucleo privide three LED for users to control,
* here is the interface to turn on/off LED3(red) inside the board
* @ input para: bool(1/0) which means gpio active level
* @ return para: void
*/
void board_drv_led03_ctrl(bool active)
{
    gpio_dev_t board_led03 = {ON_BOARD_LED03, OUTPUT_PUSH_PULL, NULL};
    if (active)
        hal_gpio_output_high(&board_led03);
    if (!active)
        hal_gpio_output_low(&board_led03);
}