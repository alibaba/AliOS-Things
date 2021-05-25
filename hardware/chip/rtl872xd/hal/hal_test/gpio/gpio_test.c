/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "aos/kernel.h"
#include "aos/hal/gpio.h"
#include "gpio_test.h"

static aos_timer_t gpio_test_timer;
static aos_sem_t   gpio_test_sem;
static gpio_dev_t  gpio_out;
static gpio_dev_t  gpio_in;

int gpio_irq_flag = 0;

static void gpio_test_end(void);
static void gpio_test_task(void *arg1 , void *arg2);
static void gpio_output_irq_test(void);
static void gpio_output_in_test(void);
static void gpio_in_handler(void *arg);

void hal_gpio_test(void)
{
    int ret = -1;

    printf("*********** gpio test start ! ***********\n");

    /* init gpio out in */
    gpio_out.port = PORT_GPIO_TEST_OUT;
    gpio_out.config = OUTPUT_PUSH_PULL;
    gpio_in.port = PORT_GPIO_TEST_IN;

    ret = hal_gpio_init(&gpio_out);
    if (ret != 0) {
        printf("gpio_out init error !\n");
    }

    /* create a sem and a timer to test gpio */
    aos_sem_new(&gpio_test_sem, 0);
    aos_timer_new(&gpio_test_timer, gpio_test_task, NULL, 100, 1);

    /* wait the test end */
    aos_sem_wait(&gpio_test_sem, AOS_WAIT_FOREVER);

    hal_gpio_finalize(&gpio_in);
    hal_gpio_finalize(&gpio_out);

    aos_sem_free(&gpio_test_sem);
    aos_timer_free(&gpio_test_timer);

    printf("*********** gpio test end ! ***********\n");
}

void gpio_test_end(void)
{
    aos_task_exit(0);
    aos_sem_signal(&gpio_test_sem);
}

void gpio_test_task(void *arg1 , void *arg2)
{
    static int gpio_test_cnt = 0;

    gpio_test_cnt++;

    //printf("gpio_test_cnt %d\n", gpio_test_cnt);

    if ((gpio_test_cnt >= 1) && (gpio_test_cnt <= 100)) {
        gpio_output_irq_test();
    }

    if ((gpio_test_cnt >= 101) && (gpio_test_cnt <= 200)) {
        gpio_output_in_test();
    }

    if (gpio_test_cnt > 200) {
        gpio_test_end();
    }
}

void gpio_output_irq_test(void)
{
    static int test_cnt       = 0;
    static int test_fail_flag = 0;
    static int arg = 0;

    int ret = -1;

    test_cnt++;

    //printf("test_cnt %d\n", test_cnt);

    if (test_cnt == 1) {
        printf("gpio_output_irq_test begin !\n");

        ret = hal_gpio_init(&gpio_out);
        if (ret != 0) {
            printf("gpio_out init error !\n");
        }

        hal_gpio_output_low(&gpio_out);

        /* init gpio in */
        gpio_in.config = IRQ_MODE;

        ret = hal_gpio_init(&gpio_in);
        if (ret != 0) {
            printf("gpio_in init error !\n");
        }

        /* enable the irg mode */
        ret = hal_gpio_enable_irq(&gpio_in, IRQ_TRIGGER_BOTH_EDGES, gpio_in_handler, &arg);
        if (ret != 0) {
            printf("gpio_in init irq error !\n");
        }
    }

    hal_gpio_output_toggle(&gpio_out);

    if (gpio_irq_flag == 1) {
        gpio_irq_flag = 0;
    } else {
        test_fail_flag = 1;
    }

    if (test_cnt == 100) {
        if (test_fail_flag == 1) {
            printf("gpio_output_irq_test failed !\n");
        } else {
            printf("gpio_output_irq_test succeed !\n");
        }
    }
}

void gpio_output_in_test(void)
{
    static int test_cnt       = 0;
    static int test_fail_flag = 0;

    uint32_t value = 0;
    int      ret   = -1;

    test_cnt++;

    if (test_cnt == 1) {
        printf("gpio_output_in_test begin !\n");

        hal_gpio_finalize(&gpio_in);

        gpio_in.config = INPUT_HIGH_IMPEDANCE;
        ret = hal_gpio_init(&gpio_in);
        if (ret != 0) {
            printf("gpio_in init error !\n");
        }
    }

    hal_gpio_output_high(&gpio_out);
    hal_gpio_input_get(&gpio_in, &value);

    if (value == 1) {
        gpio_irq_flag = 0;
    } else {
        test_fail_flag = 1;
    }

    hal_gpio_output_low(&gpio_out);
    hal_gpio_input_get(&gpio_in, &value);

    if (value == 0) {
        gpio_irq_flag = 0;
    } else {
        test_fail_flag = 1;
    }

    hal_gpio_output_toggle(&gpio_out);
    hal_gpio_input_get(&gpio_in, &value);

    if (value == 1) {
        gpio_irq_flag = 0;
    } else {
        test_fail_flag = 1;
    }

    if (test_cnt == 100) {
        if (test_fail_flag == 1) {
            printf("gpio_output_in_test failed !\n");
        } else {
            printf("gpio_output_in_test succeed !\n");
        }
    } 
}

void gpio_in_handler(void *arg)
{
    gpio_irq_flag = 1;
}
