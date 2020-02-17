/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <k_api.h>
#include "aos/yloop.h"
#include "aos/hal/gpio.h"
#include <aos/kernel.h>
#include "aos/kernel.h"
#include "ameba_soc.h"

#define KEY_AWSS   12

extern uint32_t SystemCoreClock;

static uint64_t   awss_time = 0;
static gpio_dev_t gpio_key_awss;

static void key_poll_func(void *arg)
{
    uint32_t level;
    uint64_t diff;

    hal_gpio_input_get(&gpio_key_awss, &level);

    if (level == 0) {
        aos_post_delayed_action(10, key_poll_func, NULL);
    } else {
        diff = aos_now_ms() - awss_time;
        if (diff > 6000) { /*long long press */
            awss_time = 0;
            aos_post_event(EV_KEY, CODE_BOOT, VALUE_KEY_LLTCLICK);
        } else if (diff > 2000) { /* long press */
            awss_time = 0;
            aos_post_event(EV_KEY, CODE_BOOT, VALUE_KEY_LTCLICK);
        } else if (diff > 40) { /* short press */
            awss_time = 0;
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

static void handle_awss_key(void *arg)
{
    uint32_t gpio_value;

    hal_gpio_input_get(&gpio_key_awss, &gpio_value);
    if (gpio_value == 0 && awss_time == 0) {
        awss_time = aos_now_ms();
        aos_loop_schedule_work(0, key_proc_work, NULL, NULL, NULL);
    }
}

void board_init(void)
{
    gpio_key_awss.port = KEY_AWSS;
    gpio_key_awss.config = INPUT_PULL_UP;

    hal_gpio_init(&gpio_key_awss);
    hal_gpio_enable_irq(&gpio_key_awss, IRQ_TRIGGER_FALLING_EDGE, handle_awss_key, NULL);
}

#ifdef AOS_COMP_DEBUG
typedef void (*HAL_VECTOR_FUN) (void );
extern HAL_VECTOR_FUN  NewVectorTable[];
extern void HardFault_Handler(void);
#endif

void board_basic_init(void)
{
    hal_wdg_finalize(0);

#ifdef AOS_COMP_DEBUG
    /* AliOS-Things taking over the exception */
    /* replace HardFault Vector */
    NewVectorTable[3] = HardFault_Handler;
    /* replace MemManage Vector */
    NewVectorTable[4] = HardFault_Handler;
    /* replace BusFault Vector */
    NewVectorTable[5] = HardFault_Handler;
    /* replace UsageFault Vector */
    NewVectorTable[6] = HardFault_Handler;
#endif
}

void board_tick_init(void)
{
    SysTick_Config(SystemCoreClock/RHINO_CONFIG_TICKS_PER_SECOND);
}
