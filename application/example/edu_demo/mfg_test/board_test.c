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
#include "aos/hal/flash.h"

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
    TEST_SPI,
    TEST_I2C,
    TEST_PWM,
    TEST_ADC,
    TEST_SD,
    TEST_WIFI_CONN,
    TEST_WATCHDOG,
    TEST_USB,
    TEST_AUDIO,
    TEST_OLED,
// i2c devices
    TEST_SI7006,
    TEST_MPU6050,
    TEST_QMC5883L,
    TEST_SPL06,
    TEST_AP3216C,
    TEST_MAX,
};

typedef struct {
    char *name;
    int result;
} test_record_t;

extern void led_test(void);
extern int key_test(void);
extern int uart_ttl_test();
extern int i2c_test();
extern int spi_test();
extern int pwm_test();
extern int adc_test();
extern int sdcard_test(void);
extern void watchdog_test();
extern void led_blink(void);
extern void oled_blink(void);
extern int audio_test(void);
extern int usb_test(void);
extern uint8_t sensors_test();
extern void ekd_oled_show(char *buf, int len);

test_record_t test_rec_group[TEST_MAX] = {
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
        .name            = "UART",
        .result          = UNKNOWN_RESULT,
	},
    [TEST_SPI] =
	{
        .name            = "SPI",
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
//extraboard_test
    [TEST_USB] =
	{
        .name            = "USB",
        .result          = UNKNOWN_RESULT,
	},
    [TEST_AUDIO] =
	{
        .name            = "AUDIO",
        .result          = UNKNOWN_RESULT,
	},
    [TEST_OLED] =
	{
        .name            = "OLED",
        .result          = UNKNOWN_RESULT,
	},
    [TEST_SI7006] =
	{
        .name            = "SI7006",
        .result          = UNKNOWN_RESULT,
	},
    [TEST_MPU6050] =
	{
        .name            = "MPU6050",
        .result          = UNKNOWN_RESULT,
	},
    [TEST_QMC5883L] =
	{
        .name            = "QMC5883L",
        .result          = UNKNOWN_RESULT,
	},
    [TEST_SPL06] =
	{
        .name            = "SPL06",
        .result          = UNKNOWN_RESULT,
	},
    [TEST_AP3216C] =
	{
        .name            = "AP3216C",
        .result          = UNKNOWN_RESULT,
	},
//extraboard_test end
};

uint32_t led_test_flag = 0;
uint32_t oled_test_flag = 0;
uint32_t auto_test_flag = 0;

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

    if(0 == aos_kv_get(test_rec_group[1].name, buff, &len))
    {
        ekd_oled_show("  Test end,        show Result.",10);
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
    uint8_t value = 0;
    int len = 1;
    int ret = 0;
    printf("Haas 100 test entry here! version build time %s:%s \r\n", __DATE__, __TIME__);
    sh1106_init();

    ret = show_last_autotest_results();
    if(ret == 0) {
        return;
    }

    key_test();
    led_test_flag = 1;
    oled_test_flag = 1;
    aos_task_new("oled_blink", oled_blink, NULL, 4096);
    led_test();
    ekd_oled_show("pls press any key!",10);
    while(auto_test_flag != 1) {
        if(led_test_flag == 0) {
            printf("\r\n\r\n******************************************\r\n\r\n");
            printf("\r\n\r\n====Result: LED & OELD test PASS !!!=== \r\n\r\n");
            printf("\r\n\r\n******************************************\r\n\r\n");
            test_rec_group[TEST_LED].result = 0;
            test_rec_group[TEST_OLED].result = 0;
            led_test_flag = 1;
            oled_test_flag = 0;
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
    ekd_oled_show("Auto Testing!",10);
    test_rec_group[TEST_ADC].result = adc_test();
    test_rec_group[TEST_UART_TTL].result = uart_ttl_test();
    test_rec_group[TEST_SD].result = sdcard_test();
    test_rec_group[TEST_PWM].result = pwm_test();
    test_rec_group[TEST_SI7006].result = sensors_test(0);
    test_rec_group[TEST_MPU6050].result = sensors_test(1);
    test_rec_group[TEST_QMC5883L].result = sensors_test(2);
    test_rec_group[TEST_SPL06].result = sensors_test(3);
    test_rec_group[TEST_AP3216C].result = sensors_test(4);

    printf("\r\n\r\n");
    printf("***************************************************************\r\n");
    printf("************************* WIFI Connect Test *******************\r\n");
    printf("*How to test: connect wifi ssid:haas-open paasword:12345678 *****\r\n");
    printf("***************************************************************\r\n");
    printf("=====WIFI Connect test : Start=====\r\n");
    test_rec_group[TEST_WIFI_CONN].result = netmgr_connect("haas-open", "12345678", 5000);

    printf("\r\n\r\n******************************************\r\n\r\n");
    if(test_rec_group[TEST_WIFI_CONN].result == 0) {
        printf("\r\n\r\n====Result: wifi connect test PASS !!!=== \r\n\r\n");
    }
    printf("\r\n\r\n******************************************\r\n\r\n");
    printf("--------------------------------------------------------------\r\n");
    for(int i = 1; i < TEST_MAX; i++) {
        if(test_rec_group[i].result != UNKNOWN_RESULT)
        {
            aos_kv_set(test_rec_group[i].name, (test_rec_group[i].result == 0) ? "PASS" : "FAIL", 4, 1);
        }
    }

    aos_kv_set(test_rec_group[TEST_WATCHDOG].name, "WAIT", 4, 1);
    watchdog_test();
}

static void handle_haas_cmd(char *pwbuf, int blen, int argc, char **argv)
{
	if ((0 == strcmp(argv[1], "key")) && (0 == strcmp(argv[2], "short")))
	{
        printf("key show pressed\n");
        led_test_flag = 0;
	}else if ((0 == strcmp(argv[1], "key")) && (0 == strcmp(argv[2], "long")))
    {
        printf("key long pressed\n");
        auto_test_flag = 1;
    }else if ((0 == strcmp(argv[1], "kv")) && (0 == strcmp(argv[2], "erase")))
    {
        hal_flash_erase(HAL_PARTITION_PARAMETER_2, 0, 0xd000);
    }
}

static struct cli_command haas_cmd = {
	.name = "haas",
    .help = "haas [set]",
    .function = handle_haas_cmd
};

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

