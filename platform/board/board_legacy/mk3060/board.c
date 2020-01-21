/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "aos/hal/gpio.h"
#include <aos/kernel.h>
#include "aos/kernel.h"
#include "aos/yloop.h"

void qc_test(void);

#define KEY_STATUS 1
#define KEY_ELINK  2
#define KEY_BOOT   7

static uint64_t   elink_time = 0;
static gpio_dev_t gpio_key_boot, gpio_key_status;

static void key_poll_func(void *arg)
{
    uint32_t level;
    uint64_t diff;

    hal_gpio_input_get(&gpio_key_boot, &level);

    if (level == 0) {
        aos_post_delayed_action(10, key_poll_func, NULL);
    } else {
        diff = aos_now_ms() - elink_time;
        if (diff > 6000) { /*long long press */
            elink_time = 0;
            aos_post_event(EV_KEY, CODE_BOOT, VALUE_KEY_LLTCLICK);
        } else if (diff > 2000) { /* long press */
            elink_time = 0;
            aos_post_event(EV_KEY, CODE_BOOT, VALUE_KEY_LTCLICK);
        } else if (diff > 40) { /* short press */
            elink_time = 0;
            aos_post_event(EV_KEY, CODE_BOOT, VALUE_KEY_CLICK);
        } else {
            aos_post_delayed_action(10, key_poll_func, NULL);
        }
    }
}

static void key_proc_work(void *arg)
{
    aos_schedule_call(key_poll_func, NULL);
}

static void handle_elink_key(void *arg)
{
    uint32_t gpio_value;

    hal_gpio_input_get(&gpio_key_boot, &gpio_value);
    if (gpio_value == 0 && elink_time == 0) {
        elink_time = aos_now_ms();
        aos_loop_schedule_work(0, key_proc_work, NULL, NULL, NULL);
    }
}

/* For QC test */
static void board_qc_check(void)
{
    uint32_t gpio_value = 1;

    gpio_key_boot.port = KEY_BOOT;
    gpio_key_boot.config = INPUT_PULL_UP;
    hal_gpio_init(&gpio_key_boot);
    hal_gpio_input_get(&gpio_key_boot, &gpio_value);
    
    if (gpio_value != 0) {
        return;
    }

    gpio_value = 1;
    gpio_key_status.port = KEY_STATUS;
    gpio_key_status.config = INPUT_PULL_UP;
    hal_gpio_init(&gpio_key_status);
    hal_gpio_input_get(&gpio_key_status, &gpio_value);
    if (gpio_value != 0) {
        return;
    }

    // QC:
    printf("Enter QC mode\r\n");
    qc_test();
    return;
}

void board_init(void)
{
    gpio_key_boot.port = KEY_BOOT;

    board_qc_check();
    hal_gpio_clear_irq(&gpio_key_boot);
    hal_gpio_enable_irq(&gpio_key_boot, IRQ_TRIGGER_FALLING_EDGE, handle_elink_key, NULL);
}
