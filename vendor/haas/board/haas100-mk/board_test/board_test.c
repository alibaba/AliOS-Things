/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <aos/errno.h>
#include <aos/kernel.h>
#include "aos/init.h"
#include "board.h"
#include <k_api.h>
#include "aos/cli.h"
#include "aos/hal/spi.h"
#include "aos/yloop.h"

#include "led.h"
#include "key.h"
#include "hal_gpio.h"
#include "hal_iomux_haas1000.h"
#include "netmgr.h"

#define UNKNOWN_RESULT 99

typedef enum {
    TEST_NULL,
    TEST_LED,
    TEST_KEY,
    TEST_UART_TTL,
    TEST_UART_232,
    TEST_UART_485,
    TEST_CH395_SPI1,
    TEST_I2C,
    TEST_PWM,
    TEST_ADC,
    TEST_SD,
    TEST_WIFI_CONN,
    TEST_WATCHDOG,
    TEST_MAX,
};

typedef struct {
    char *name;
    int result;
} test_record_t;

extern void led_test(void);
extern void key_test(void);
extern int uart_ttl_test();
extern void uart_232_test();
extern void rs485_test();
extern int ch395_spi_test();
extern int i2c_test();
extern void pwm_test();
extern int adc_test();
extern int sdcard_test(void);
extern void watchdog_test();
extern void led_blink(void);

test_record_t test_rec_group[ ] = {
    [TEST_LED] =
	{
        .name            = "LED",
        .result          = UNKNOWN_RESULT,
	},
    [TEST_KEY] =
	{
        .name            = "KEY",
        .result          = UNKNOWN_RESULT,
	},
    [TEST_UART_TTL] =
	{
        .name            = "UART TTL",
        .result          = UNKNOWN_RESULT,
	},
    [TEST_UART_232] =
	{
        .name            = "UART 232",
        .result          = UNKNOWN_RESULT,
	},
    [TEST_UART_485] =
	{
        .name            = "UART 485",
        .result          = UNKNOWN_RESULT,
	},
    [TEST_CH395_SPI1] =
	{
        .name            = "CH395 SPI1",
        .result          = UNKNOWN_RESULT,
	},
    [TEST_I2C] =
	{
        .name            = "I2C",
        .result          = UNKNOWN_RESULT,
	},
    [TEST_PWM] =
	{
        .name            = "PWM",
        .result          = UNKNOWN_RESULT,
	},
    [TEST_ADC] =
	{
        .name            = "ADC",
        .result          = UNKNOWN_RESULT,
	},
    [TEST_SD] =
	{
        .name            = "SD",
        .result          = UNKNOWN_RESULT,
	},
    [TEST_WIFI_CONN] =
	{
        .name            = "WIFI CONNECT",
        .result          = UNKNOWN_RESULT,
	},
    [TEST_WATCHDOG] =
	{
        .name            = "WATCHDOG",
        .result          = UNKNOWN_RESULT,
	},
};

uint32_t led_test_flag = 0;
uint32_t auto_test_flag = 0;

static void handle_spi_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    int port;
    int bd;
    int cmd;
    if(argc < 3) {
        printf("Usage: spi port[0-1], baudrate[500k - 2000k], cmd[1 byte]\n");
        return;
    }
    port = atoi(argv[1]);
    bd = atoi(argv[2]);
    cmd = atoi(argv[3]);

    if((bd != 500)&&(bd != 1000)&&(bd != 2000))
    {
        printf("Baudrate is not valid\n");
        printf("Usage: spi port[0-1], baudrate[500k - 2000k], cmd[1 byte]\n");
        return;
    }

    if((cmd > 256)||(cmd < 0))
    {
        printf("CMD is not valid\n");
        printf("Usage: spi port[0-1], baudrate[500k - 2000k], cmd[1 byte]\n");
        return;
    }

    _hal_spi_test_arg(port, bd, cmd);
}

static struct cli_command spi_cmd = {
    .name     = "spi",
    .help     = "spi port[0-1], baudrate[500/1000/2000 unit:kHz], cmd[0-255]\n",
    .function = handle_spi_cmd

};

void show_autotest_results()
{
    printf("\r\n\r\n");
    printf("**************************************************************\r\n");
    printf("**************************AUTO TEST RESULT********************\r\n");
    printf("--------------------------------------------------------------\r\n");
    printf("Test Items                Result \r\n");
    printf("--------------------------------------------------------------\r\n");
    for(int i = 1; i < TEST_MAX; i++) {
        if(test_rec_group[i].result != UNKNOWN_RESULT) {
            printf("%10s                 %s\n", test_rec_group[i].name, (test_rec_group[i].result == 0) ? "PASS" : "FAIL");
        }
    }
    printf("**************************************************************\r\n");
    printf("**************************************************************\r\n");
    printf("**************************************************************\r\n");
}

int show_last_autotest_results()
{
    int len = 4;
    char buff[5] = {0};

    if(0 == aos_kv_get(test_rec_group[1].name, buff, &len)) {
        printf("\r\n\r\n");
        printf("**************************************************************\r\n");
        printf("**************************AUTO TEST RESULT********************\r\n");
        printf("--------------------------------------------------------------\r\n");
        printf("Test Items                  Result \r\n");
        printf("--------------------------------------------------------------\r\n");
        for(int i = 1; i < TEST_MAX; i++) {
            int ret = 0;
            memset(buff, 0, sizeof(buff));
            ret = aos_kv_get(test_rec_group[i].name, buff, &len);
            if(0 == strncmp(buff, "WAIT", 4)) {
                memcpy(buff, "PASS", 4);
            }
            if(ret == 0) {
                printf("%12s                 %s\n", test_rec_group[i].name, buff);
            }
            aos_kv_del(test_rec_group[i].name);
        }
        printf("**************************************************************\r\n");
        printf("**************************************************************\r\n");
        printf("**************************************************************\r\n");
        return 0;
    }

    return 1;
}

static void wifi_service_event(input_event_t *event, void *priv_data)
{
    if (event->type != EV_WIFI) {
        return;
    }

    if (event->code != CODE_WIFI_ON_GOT_IP) {
        return;
    }
}

void test_process_entry()
{
    int ret = 0;
    printf("Haas 100 test entry here! version build time %s:%s \r\n", __DATE__, __TIME__);

    ret = show_last_autotest_results();
    if(ret  == 0) {
        return;
    }

    key_test();
    led_test_flag = 1;
    led_test();
    while(auto_test_flag != 1) {
        if(led_test_flag == 0) {
            printf("\r\n\r\n******************************************\r\n\r\n");
            printf("\r\n\r\n====Result: LED test PASS !!!=== \r\n\r\n");
            printf("\r\n\r\n******************************************\r\n\r\n");
            test_rec_group[TEST_LED].result = 0;
            led_test_flag = 1;
        }
        aos_msleep(100);
    }

    aos_task_new("led_blink", led_blink, NULL, 4096);

    printf("\r\n\r\n");
    printf("***************************************************************\r\n");
    printf("************************* KEY Test ****************************\r\n");
    printf("***************************************************************\r\n");
    printf("***************************************************************\r\n");
    printf("===== KEY test : Start=====\r\n");

    printf("\r\n\r\n******************************************\r\n\r\n");
    printf("\r\n\r\n====Result: KEY test PASS !!!=== \r\n\r\n");
    printf("\r\n\r\n******************************************\r\n\r\n");
    test_rec_group[TEST_KEY].result = 0;

    printf("\r\n\r\n\r\n\r\n");
    printf("!!!!!!!!!!!!!!!!!!!!AUTO TEST BEGIN!!!!!!!!!!!!!!!!!!!!!!!!!\r\n");

    test_rec_group[TEST_I2C].result = i2c_test();
    test_rec_group[TEST_ADC].result = adc_test();
    test_rec_group[TEST_UART_TTL].result = uart_ttl_test();
    test_rec_group[TEST_CH395_SPI1].result = ch395_spi_test();
    test_rec_group[TEST_SD].result = sdcard_test();

    printf("\r\n\r\n");
    printf("!!!!!!!!!!!!!!!!!!MANUAL TEST BEGIN!!!!!!!!!!!!!!!!!!!!!!!\r\n");

    rs485_test();
    pwm_test();

    printf("\r\n\r\n");
    printf("***************************************************************\r\n");
    printf("************************* WIFI Connect Test *******************\r\n");
    printf("*How to test: connect wifi ssid:haas100 paasword:12345678 *****\r\n");
    printf("***************************************************************\r\n");
    printf("=====WIFI Connect test : Start=====\r\n");
    test_rec_group[TEST_WIFI_CONN].result = netmgr_connect("haas100", "12345678", 5000);

    printf("\r\n\r\n******************************************\r\n\r\n");
    if(test_rec_group[TEST_WIFI_CONN].result == 0) {
        printf("\r\n\r\n====Result: wifi connect test PASS !!!=== \r\n\r\n");
    }
    printf("\r\n\r\n******************************************\r\n\r\n");
    printf("--------------------------------------------------------------\r\n");
    for(int i = 1; i < TEST_MAX; i++) {
        if(test_rec_group[i].result != UNKNOWN_RESULT) {
            aos_kv_set(test_rec_group[i].name, (test_rec_group[i].result == 0) ? "PASS" : "FAIL", 4, 1);
        }
    }

    aos_kv_set(test_rec_group[TEST_WATCHDOG].name, "WAIT", 4, 1);
    watchdog_test();
}

extern uint32_t g_wifi_factory_test;
int board_test()
{
    int count = 0;

    if(g_wifi_factory_test == 0) {
        netmgr_init();
        aos_register_event_filter(EV_WIFI, wifi_service_event, NULL);

        aos_task_new("test_process", test_process_entry, NULL, 8192);
        aos_loop_run();
    }

    while(1) {
        aos_msleep(1000);
    };
}

