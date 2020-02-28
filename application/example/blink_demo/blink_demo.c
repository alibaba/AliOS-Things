/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include "aos/kernel.h"
#include "ulog/ulog.h"

#include "aos/hal/gpio.h"

/**
 * Brief:
 * This test code shows how to configure gpio and how to use gpio interrupt.
 */
#ifdef LED1
#define GPIO_LED_IO         LED1
#else
#define GPIO_LED_IO         0xffff
#endif

#ifdef LED2
#define GPIO_TRIGGER_IO     LED2
#else
#define GPIO_TRIGGER_IO     0xffff
#endif

#ifdef KEY1
#define GPIO_INPUT_IO       KEY1
#else
#define GPIO_INPUT_IO       0xffff
#endif

static void app_trigger_low_action(void *arg);
static void app_trigger_high_action(void *arg);

gpio_dev_t led;
gpio_dev_t trigger;
gpio_dev_t input;

static void gpio_isr_handler(void* arg)
{
    uint32_t gpio_num = (uint32_t) arg;
    uint32_t value = 0;
    hal_gpio_input_get(&input, &value);
    hal_gpio_output_toggle(&led);
    LOG("GPIO[%u] intr, val: %u\n", gpio_num, value);
}

static void app_trigger_low_action(void *arg)
{
    hal_gpio_output_low(&trigger);
    aos_post_delayed_action(1000, app_trigger_high_action, NULL);
}

static void app_trigger_high_action(void *arg)
{
    hal_gpio_output_high(&trigger);
    aos_post_delayed_action(1000, app_trigger_low_action, NULL);
}

int application_start(int argc, char *argv[])
{
    /* gpio port config */
    led.port = GPIO_LED_IO;
    /* set as output mode */
    led.config = OUTPUT_PUSH_PULL;
    /* configure GPIO with the given settings */
    hal_gpio_init(&led);

    /* gpio port config */
    trigger.port = GPIO_TRIGGER_IO;
    /* set as output mode */
    trigger.config = OUTPUT_PUSH_PULL;
    /* configure GPIO with the given settings */
    hal_gpio_init(&trigger);

    /* input pin config */
    input.port = GPIO_INPUT_IO;
    /* set as interrupt mode */
    input.config = IRQ_MODE;
    /* configure GPIO with the given settings */
    hal_gpio_init(&input);

    /* gpio interrupt config */
    hal_gpio_enable_irq(&input, IRQ_TRIGGER_BOTH_EDGES, gpio_isr_handler, (void *) GPIO_INPUT_IO);

    aos_post_delayed_action(1000, app_trigger_low_action, NULL);

    aos_loop_run();
    return 0;
}
