#include <stdio.h>
//#include <aos/kernel.h>
#include "aos/hal/gpio.h"
#include "hal_iomux_haas1000.h"
#include "key.h"

static key_cfg_t key_cfg;
static gpio_dev_t key_gpio;
static void key_rising_edge_handle();
static long long key_falling_ms = 0;

static void key_falling_edge_handle()
{
    hal_gpio_clear_irq(&key_gpio);
    hal_gpio_disable_irq(&key_gpio);

    key_falling_ms = aos_now_ms();

    hal_gpio_enable_irq(&key_gpio, IRQ_TRIGGER_RISING_EDGE, key_rising_edge_handle, (void *)HAL_IOMUX_PIN_P3_2);
}

static void key_rising_edge_handle()
{
    long long ms;
    uint32_t press_ms;

    hal_gpio_clear_irq(&key_gpio);
    hal_gpio_disable_irq(&key_gpio);

    ms = aos_now_ms();
    press_ms = (uint32_t)(ms - key_falling_ms);

    if((press_ms < key_cfg.short_press_max_ms) && (press_ms > KEY_MIN_PRESS_MS)) {
        if(key_cfg.short_press_handler != NULL) {
            key_cfg.short_press_handler();
        }
    } else if(press_ms > key_cfg.long_press_min_ms) {
        if(key_cfg.long_press_handler != NULL) {
            key_cfg.long_press_handler();
        }
    }
    key_falling_ms = 0;
    hal_gpio_enable_irq(&key_gpio, IRQ_TRIGGER_FALLING_EDGE, key_falling_edge_handle, (void *)HAL_IOMUX_PIN_P3_2);
}

int key_init(key_cfg_t *cfg)
{
    int ret = 0;

    if(cfg == NULL) {
        return -1;
    }

    if((cfg->short_press_handler == NULL) && (cfg->long_press_handler == NULL)) {
        return -1;
    }

    memcpy(&key_cfg, cfg, sizeof(key_cfg_t));
    if(key_cfg.short_press_max_ms == 0) {
        key_cfg.short_press_max_ms = KEY_SHORT_PRESS_MS;
    }

    if(key_cfg.long_press_min_ms == 0) {
        key_cfg.long_press_min_ms = KEY_LONG_PRESS_MS;
    }

    key_gpio.port   = HAL_IOMUX_PIN_P3_2;
    key_gpio.config = IRQ_MODE;
    hal_gpio_init(&key_gpio);

    ret = hal_gpio_enable_irq(&key_gpio, IRQ_TRIGGER_FALLING_EDGE, key_falling_edge_handle, (void *)HAL_IOMUX_PIN_P3_2);

    return ret;
}
