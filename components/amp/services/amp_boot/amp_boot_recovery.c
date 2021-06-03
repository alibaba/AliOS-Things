/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "amp_config.h"
#include "amp_utils.h"
#include "aos_system.h"
#include "amp_defines.h"
#include "aos_hal_uart.h"
#include "aos_hal_gpio.h"

#include "amp_boot_recovery.h"

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
    //hexdump("ymodem data", buf, size);
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
    AMP_STATUS_IO_ON ? aos_hal_gpio_output_high(&amp_status_led) : aos_hal_gpio_output_low(&amp_status_led);
}

void _amp_status_led_off(void)
{
    AMP_STATUS_IO_ON ? aos_hal_gpio_output_low(&amp_status_led) : aos_hal_gpio_output_high(&amp_status_led);
}

void _amp_status_led_task(void *param)
{
    (void *)param;

    while (1) {
        // amp_debug(MOD_STR, "status_led_mode set");
        switch (amp_status) {
            case AMP_STATUS_NORMAL:
                /* normal running */
                _amp_status_led_on();
                aos_msleep(500);
                _amp_status_led_off();
                aos_msleep(2500);
                break;
            case AMP_STATUS_SERVICE_AVAILABLE:
                /* service available */
                _amp_status_led_on();
                aos_msleep(2500);
                _amp_status_led_off();
                aos_msleep(500);
                break;
            case AMP_STATUS_RECOVERY:
                /* recovery mode */
                _amp_status_led_on();
                aos_msleep(100);
                _amp_status_led_off();
                aos_msleep(100);
                _amp_status_led_on();
                aos_msleep(100);
                _amp_status_led_off();
                aos_msleep(850);
                break;
            case AMP_STATUS_UPDATING:
                /* upgrading */
                _amp_status_led_on();
                aos_msleep(50);
                _amp_status_led_off();
                aos_msleep(50);
                _amp_status_led_on();
                aos_msleep(50);
                _amp_status_led_off();
                aos_msleep(50);
                break;
            case AMP_STATUS_JSERROR:
                /* JS Error */
                _amp_status_led_on();
                aos_msleep(1000);
                break;
            case AMP_STATUS_COREDUMP:
                /* JS Error */
                _amp_status_led_on();
                aos_msleep(1000);
                break;
            default:
                amp_debug(MOD_STR, "wrong status mode");
                break;
        }
    }
    return;
}

int amp_recovery_init(void)
{
    int delay = 0, ret = -1, value = 0, status_off = 1;
    void *status_led_task;
    aos_task_t amp_stat_task;

    amp_debug(MOD_STR, "recovery entry");
    amp_rec_switch.port   = AMP_REC_IO;
    amp_rec_switch.config = INPUT_PULL_UP;
    amp_status_led.port   = AMP_STATUS_IO;
    amp_status_led.config = OUTPUT_OPEN_DRAIN_PULL_UP;

    amp_error(MOD_STR, "recovery switch io is %d led port is %d", AMP_REC_IO, AMP_STATUS_IO);
    /* configure GPIO with the given settings */

    ret = aos_hal_gpio_init(&amp_rec_switch);
    if (ret != 0) {
        amp_error(MOD_STR, "recovery switch gpio init error!");
        return 0;
    }

    ret = aos_hal_gpio_init(&amp_status_led);
    if (ret != 0) {
        amp_error(MOD_STR, "recovery status gpio init error!");
        return 0;
    }

    // for debounce
    while (delay++ <= 10) {
        aos_hal_gpio_input_get(&amp_rec_switch, &value);
        // recovery switch
        if (value == !AMP_REC_IO_ON) {
            status_off = 0;
        }

        amp_debug(MOD_STR, "gpio status: %d", value);
        aos_msleep(10);
        delay += 1;
    }

    if (status_off) {
        /* enter to recovery mode */
        amp_debug(MOD_STR, "enter to recovery, not to start JS code");
        aos_task_new_ext(&amp_stat_task, "amp status task", _amp_status_led_task, NULL, 1024, ADDON_TSK_PRIORRITY);

        amp_status_set(AMP_STATUS_RECOVERY);
        aos_hal_gpio_finalize(&amp_rec_switch);
        return -1;
    }
    aos_hal_gpio_finalize(&amp_rec_switch);
    return 0;
}

extern int ymodem_upgrade(void (*func)(unsigned char *, int));

int amp_recovery_entry(void)
{
    ymodem_appbin_init();
    ymodem_upgrade(ymodem_appbin_write);
    ymodem_appbin_finish();
    amp_debug(MOD_STR, "ymodem_upgrade done");
    while (1) {
        aos_msleep(1000);
    }

    return 0;
}

int amp_recovery_appbin()
{
    ymodem_appbin_init();
    ymodem_upgrade(ymodem_appbin_write);
    ymodem_appbin_finish();
}
