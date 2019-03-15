/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include "aos/kernel.h"
#include "ulog/ulog.h"

#include "aos/hal/gpio.h"

/**
 * Brief:
 * This test code shows how to configure gpio and how to use gpio interrupt.
 *
 * GPIO status:
 * GPIO18: output
 * GPIO4: output
 * GPIO5:  input, pulled up, interrupt from rising edge and falling edge
 *
 * Test:
 * Connect GPIO18 with LED
 * Connect GPIO4 with GPIO5
 * Generate pulses on GPIO4, that triggers interrupt on GPIO5 to blink the led.
 *
 */

#define GPIO_LED_IO     18
#define GPIO_TRIGGER_IO 4
#define GPIO_INPUT_IO   5
// add for st nucleo board
#define GPIO_LED_NUCLEO     5

static void app_trigger_low_action(void *arg);
static void app_trigger_high_action(void *arg);

gpio_dev_t led;
gpio_dev_t trigger;
gpio_dev_t input;
//add for st nucleo board
gpio_dev_t led_nucleo;

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
#ifdef STM32L496xx
    // developerkit 	
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
#else
    //add for st nucleo board , if debug on developerkit board, please comment it
     /* gpio port config */
    led_nucleo.port = GPIO_LED_NUCLEO;
    /* set as output mode */
    led_nucleo.config = OUTPUT_PUSH_PULL;
    /* configure GPIO with the given settings */
    hal_gpio_init(&led_nucleo);

    while (1)
    {
      /* Insert delay 1000 ms */
         aos_msleep(1000);
         printf(" toggle led \n");
         hal_gpio_output_toggle(&led_nucleo);
    }
#endif
    aos_loop_run();
    return 0;
}
