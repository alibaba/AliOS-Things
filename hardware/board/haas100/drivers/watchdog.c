#include "aos/hal/gpio.h"
#include "hal_iomux_haas1000.h"

static gpio_dev_t wdg_gpio = {0, 0, NULL};
static int watchdog_flag = 0;

void watchdog_feeddog(void)
{
    if(watchdog_flag == 1) {
        return;
    }
    if(wdg_gpio.port == 0) {
        wdg_gpio.port   = HAL_IOMUX_PIN_P4_5;
        wdg_gpio.config = OUTPUT_PUSH_PULL;
        hal_gpio_init(&wdg_gpio);
    }
    hal_gpio_output_toggle(&wdg_gpio);
}

void watchdog_stopfeed(void)
{
    watchdog_flag = 1;
}

