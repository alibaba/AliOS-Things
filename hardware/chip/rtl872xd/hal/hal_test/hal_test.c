/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "aos/kernel.h"
#include "aos/hal/uart.h"
#include "yunit.h"

extern void hal_uart_test(void);
extern void hal_gpio_test(void);
extern void hal_i2c_test(void);
extern void hal_spi_test(void);
extern void hal_flash_test(void);
extern void hal_adc_test(void);
extern void hal_dac_test(void);
extern void hal_pwm_test_init(void);
extern void hal_pwm_test_output(char *msg);
extern void hal_rtc_test(void);
extern void hal_timer_test(void);
extern void hal_wdg_test(void);
extern void hal_rng_test(void);

#define MAX_CMD_LEN 32

static int hal_test_getchar(char *c);
static int hal_test_cmd_read(char *buf, int size);
static void hal_test_task(void *arg);
static void device_print(void);

char hal_test_cmd_buf[MAX_CMD_LEN];

void hal_test(void)
{
    printf("======================  AliOS Things HAL TEST ===================\n");
    printf("The supported devices is as follows: \n");
    printf("-----------------------------------------------------------------\n");

    device_print();

    printf("-----------------------------------------------------------------\n");
    printf("please enter the device name you want to test !\n");

    aos_task_new("hal test task", hal_test_task, NULL, 2048);
}

void hal_test_task(void *arg)
{
    int ret = -1;

    while(1) {
        ret = hal_test_cmd_read(hal_test_cmd_buf, MAX_CMD_LEN);
        if (ret == 0) {
            printf("%s\n", hal_test_cmd_buf);

        #ifdef ENABLE_UART
            if (strcmp(hal_test_cmd_buf, "uart") == 0) {
                hal_uart_test();
            }
        #endif

        #ifdef ENABLE_GPIO
            if (strcmp(hal_test_cmd_buf, "gpio") == 0) {
                hal_gpio_test();
            }
        #endif

        #ifdef ENABLE_I2C
            if (strcmp(hal_test_cmd_buf, "i2c") == 0) {
                hal_i2c_test();
            }
        #endif

        #ifdef ENABLE_SPI
            if (strcmp(hal_test_cmd_buf, "spi") == 0) {
                hal_spi_test();
            }
        #endif

        #ifdef ENABLE_FLASH
            if (strcmp(hal_test_cmd_buf, "flash") == 0) {
                hal_flash_test();
            }
        #endif

        #ifdef ENABLE_ADC
            if (strcmp(hal_test_cmd_buf, "adc") == 0) {
                hal_adc_test();
            }
        #endif

        #ifdef ENABLE_DAC
            if (strcmp(hal_test_cmd_buf, "dac") == 0) {
                hal_dac_test();
            }
        #endif

        #ifdef ENABLE_PWM
            if (strcmp(hal_test_cmd_buf, "pwm") == 0) {
                hal_pwm_test_init();
            }

            if (strncmp(hal_test_cmd_buf, "pwm_testcase", 12) == 0) {
                hal_pwm_test_output(&hal_test_cmd_buf[12]);
            }
        #endif

        #ifdef ENABLE_RTC
            if (strcmp(hal_test_cmd_buf, "rtc") == 0) {
                hal_rtc_test();
            }
        #endif

        #ifdef ENABLE_TIMER
            if (strcmp(hal_test_cmd_buf, "timer") == 0) {
                hal_timer_test();
            }
        #endif

        #ifdef ENABLE_WDG
            if (strcmp(hal_test_cmd_buf, "wdg") == 0) {
                hal_wdg_test();
            }
        #endif

        #ifdef ENABLE_RNG
            if (strcmp(hal_test_cmd_buf, "rng") == 0) {
                hal_rng_test();
            }
        #endif

        }

        aos_msleep(1000);
    };
}

int hal_test_getchar(char *c)
{
    uart_dev_t uart_stdio;

    int32_t  ret       = 0;
    uint32_t recv_size = 0;

    memset(&uart_stdio, 0, sizeof(uart_dev_t));
    uart_stdio.port = 0;

    do {
        ret = hal_uart_recv_II(&uart_stdio, c, 1, &recv_size, HAL_WAIT_FOREVER);
    } while (ret != 0);

    if ((ret == 0) && (recv_size == 1)) {
        return 0;
    } else {
        return -1;
    }
}

int hal_test_cmd_read(char *buf, int size)
{
    char c = 0;
    int pos = 0;

    while (hal_test_getchar(&c) == 0) {
        if ((c == '\n') || (c == '\r')) { /* end of input line */
            buf[pos] = '\0';
            return 0;
        } else {
            buf[pos] = c;
        }

        pos++;

        if(pos >= size) {
            return -1;
        }
    }

    return 0;
}

void device_print(void)
{
    printf("| ");

#ifdef ENABLE_UART
    printf("uart |");
#endif

#ifdef ENABLE_GPIO
    printf("gpio |");
#endif

#ifdef ENABLE_I2C
    printf("i2c |");
#endif

#ifdef ENABLE_SPI
    printf("spi |");
#endif

#ifdef ENABLE_FLASH
    printf("flash |");
#endif

#ifdef ENABLE_ADC
    printf("adc |");
#endif

#ifdef ENABLE_DAC
    printf("dac |");
#endif

#ifdef ENABLE_PWM
    printf("pwm |");
#endif

#ifdef ENABLE_RTC
    printf("rtc |");
#endif

#ifdef ENABLE_TIMER
    printf("timer |");
#endif

#ifdef ENABLE_WDG
    printf("wdg |");
#endif

#ifdef ENABLE_RNG
    printf("rng |");
#endif

    printf("\n");
}

static yunit_test_case_t aos_hal_testcases[] = {
    { "hal_test", hal_test},
    YUNIT_TEST_CASE_NULL
};

static int init(void)
{
    return 0;
}

static int cleanup(void)
{
    return 0;
}

static void setup(void)
{
}

static void teardown(void)
{
}

static yunit_test_suite_t suites[] = {
    { "hal test", init, cleanup, setup, teardown, aos_hal_testcases },
    YUNIT_TEST_SUITE_NULL
};

void test_hal(void)
{
    yunit_add_test_suites(suites);
}
AOS_TESTCASE(test_hal);
