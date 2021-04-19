#include "led.h"
#include "aos/hal/gpio.h"
#include "hal_iomux_haas1000.h"

void led_switch(led_num_e id, led_e onoff)
{
    int        ret = 0;
    gpio_dev_t led;

    /* gpio port config */
    switch (id) {
        case 1: led.port = HAL_IOMUX_PIN_P4_4;
            break;
        case 2: led.port = HAL_IOMUX_PIN_P4_3;
            break;
        case 3: led.port = HAL_IOMUX_PIN_P4_2;
            break;
        default:
            return;
    }

    /* set as output mode */
    led.config = OUTPUT_PUSH_PULL;

    ret = hal_gpio_init(&led);
    if (ret != 0) {
        printf("hal_gpio_init %d failed, ret=%d\n", id, ret);
        return;
    }

    if(onoff == LED_ON) {
        ret = hal_gpio_output_high(&led);
    } else {
        ret = hal_gpio_output_low(&led);
    }
    if (ret != 0) {
        printf("hal_gpio_output %d failed, ret=%d\n", id, ret);
        return;
    }
}
