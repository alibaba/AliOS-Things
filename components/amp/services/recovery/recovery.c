/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "amp_utils.h"
#include "amp_system.h"
#include "amp_defines.h"
#include "amp_hal_uart.h"
#include "amp_hal_gpio.h"

#include "recovery.h"
#include "app_mgr.h"

#define MOD_STR "AMP_RECOVERY"
static uart_dev_t g_uart;

static gpio_dev_t amp_status_led;
static gpio_dev_t amp_rec_switch;

/* status led mode */
static AMP_STATUS amp_status = AMP_STATUS_NORMAL;

extern int ymodem_upgrade(void (*func)(unsigned char *, int));

extern int download_apppack(uint8_t *buf, int32_t buf_len);

extern int write_app_pack(const char *filename, int32_t file_size, int32_t type,
                          int32_t offset, uint8_t *buf, int32_t buf_len,
                          int32_t complete);

void ymodem_appbin_init(void)
{
    apppack_init(write_app_pack);
}

void ymodem_appbin_finish(void)
{
    apppack_final();
    amp_status_set(AMP_STATUS_NORMAL);
    amp_debug(MOD_STR, "ymodem_appbin_finish");
}

void ymodem_appbin_write(unsigned char *buf, int size)
{
    hexdump("ymodem data", buf, size);
    amp_status_set(AMP_STATUS_UPDATING);
    download_apppack(buf, size);
}


void amp_status_set(AMP_STATUS mode)
{
    if (mode >= AMP_STATUS_END) {
        return;
    }
    amp_status = mode;
}

void _amp_status_led_on(void)
{
    AMP_STATUS_IO_ON ? amp_hal_gpio_output_high(&amp_status_led) : amp_hal_gpio_output_low(&amp_status_led);
}

void _amp_status_led_off(void)
{
    AMP_STATUS_IO_ON ? amp_hal_gpio_output_low(&amp_status_led) : amp_hal_gpio_output_high(&amp_status_led);
}

void *_amp_status_led_task(void *param)
{
    (void *)param;

    while (1) {
        // amp_debug(MOD_STR, "status_led_mode set");
        switch (amp_status) {
            case AMP_STATUS_NORMAL:
                /* normal running */
                _amp_status_led_on();
                HAL_SleepMs(500);
                _amp_status_led_off();
                HAL_SleepMs(2500);
                break;
            case AMP_STATUS_SERVICE_AVAILABLE:
                /* service available */
                _amp_status_led_on();
                HAL_SleepMs(2500);
                _amp_status_led_off();
                HAL_SleepMs(500);
                break;
            case AMP_STATUS_RECOVERY:
                /* recovery mode */
                _amp_status_led_on();
                HAL_SleepMs(100);
                _amp_status_led_off();
                HAL_SleepMs(100);
                _amp_status_led_on();
                HAL_SleepMs(100);
                _amp_status_led_off();
                HAL_SleepMs(850);
                break;
            case AMP_STATUS_UPDATING:
                /* upgrading */
                _amp_status_led_on();
                HAL_SleepMs(50);
                _amp_status_led_off();
                HAL_SleepMs(50);
                _amp_status_led_on();
                HAL_SleepMs(50);
                _amp_status_led_off();
                HAL_SleepMs(50);
                break;
            case AMP_STATUS_JSERROR:
                /* JS Error */
                _amp_status_led_on();
                HAL_SleepMs(1000);
                break;
            case AMP_STATUS_COREDUMP:
                /* JS Error */
                _amp_status_led_on();
                HAL_SleepMs(1000);
                break;
            default:
                amp_debug(MOD_STR, "wrong status mode");
                break;
        }
    }
    return NULL;
}

int amp_recovery_init(void)
{
    int delay = 0, ret = -1, value = 0, status_off = 1;
    void *status_led_task;
    int status_task_stack_used;
    amp_os_thread_param_t status_task_params = {0};

    amp_debug(MOD_STR, "recovery entry");
    amp_rec_switch.port   = AMP_REC_IO;
    amp_rec_switch.config = INPUT_PULL_DOWN;
    amp_status_led.port   = AMP_STATUS_IO;
    amp_status_led.config = OUTPUT_OPEN_DRAIN_PULL_UP;

    /* configure GPIO with the given settings */

    if ((ret = amp_hal_gpio_init(&amp_rec_switch)) != 0) {
        amp_error(MOD_STR, "recovery switch gpio init error!");
        return 0;
    }


    if ((ret = amp_hal_gpio_init(&amp_status_led)) != 0) {
        amp_error(MOD_STR, "recovery status gpio init error!");
        return 0;
    }

    status_task_params.name = "amp status task";
    status_task_params.priority = ADDON_TSK_PRIORRITY;
    status_task_params.stack_size = 1024;
    HAL_ThreadCreate(NULL, _amp_status_led_task, NULL, &status_task_params, NULL);

    // for debounce
    while (delay++ <= 10) {
        amp_hal_gpio_input_get(&amp_rec_switch, &value);
        // recovery switch
        if (value == !AMP_REC_IO_ON) {
            status_off = 0;
        }

        amp_debug(MOD_STR, "gpio status: %d", value);
        HAL_SleepMs(10);
        delay += 1;
    }

    if (status_off) {
        /* enter to recovery mode */
        amp_debug(MOD_STR, "enter to recovery, not to start JS code");
        amp_status_set(AMP_STATUS_RECOVERY);
        amp_hal_gpio_finalize(&amp_rec_switch);
        return -1;
    }
    amp_hal_gpio_finalize(&amp_rec_switch);
    return 0;
}

void uart_send_byte(unsigned char c)
{
    int send_byte = 0;
    if ((send_byte = amp_hal_uart_send(&g_uart, &c, 1, osWaitForever)) != 1) {
        amp_error(MOD_STR, "uart_send_byte error: %d", send_byte);
    }
}

unsigned char uart_recv_byte(unsigned char *c)
{
    int read_byte = 0;
    if (amp_hal_uart_recv_II(&g_uart, c, 1, &read_byte, 0) != 0) {
        return -1;
    }
    // amp_debug(MOD_STR, "YMODEM: %02x", *c);
    return 1;
}

extern int ymodem_upgrade(void (*func)(unsigned char *, int));

int amp_recovery_entry(void)
{
    g_uart.config.baud_rate = 115200;
    g_uart.port = AMP_REC_UART;
    int ret = amp_hal_uart_init(&g_uart);
    if (ret) {
        amp_debug(MOD_STR, "open uart[%d] failed! ", ret);
        return -1;
    }

    ymodem_appbin_init();
    ymodem_upgrade(ymodem_appbin_write);
    ymodem_appbin_finish();
    amp_debug(MOD_STR, "ymodem_upgrade done");
    while (1) {
        HAL_SleepMs(1000);
    }

    return 0;
}