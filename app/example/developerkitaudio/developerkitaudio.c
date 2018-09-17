/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <aos/aos.h>
#include "soc_init.h"
#include "isd9160.h"

#define AUDIO_FILE_SDCARD       "audio.data"
#define FIRMWARE_FILE_SDCARD    "ldrom_aprom_upg.bin"

static int key_flag = 0;
static int stop_flag = 0;

static void isd9160_loop(void *arg)
{
    int ret = 0;

    krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND);
    while (1) {
        isd9160_loop_once();
        if (key_flag == 1) {
            stop_flag = 0;
            printf("handle_record begin, press the same key again to stop it\n");
            ret = handle_record(AUDIO_FILE_SDCARD, &stop_flag);
            printf("handle_record return %d\n", ret);
            key_flag = 0;
        } else if (key_flag == 2) {
            printf("handle_playback begin\n");
            ret = handle_playback(AUDIO_FILE_SDCARD);
            printf("handle_playback return %d\n", ret);
            key_flag = 0;
        } else if (key_flag == 3) {
            printf("handle_upgrade begin\n");
            ret = handle_upgrade(FIRMWARE_FILE_SDCARD);
            printf("handle_upgrade return %d\n", ret);
            key_flag = 0;
        }
        krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND);
    }
}

int handing_shake()
{
    static sys_time_t last_time = 0;
    sys_time_t        now_time  = 0;
    int               ret       = 0;

    now_time = krhino_sys_time_get();
    if (now_time - last_time < 200) {
        ret = 1;
    }
    last_time = now_time;

    return ret;
}

void key1_handle(void *arg)
{
    if (handing_shake())
        return;
    if (key_flag != 0 && stop_flag == 0) {
        stop_flag = 1;
    }
    key_flag = 1;
}

void key2_handle(void *arg)
{
    if (handing_shake())
        return;
    key_flag = 2;
}

void key3_handle(void *arg)
{
    if (handing_shake())
        return;
    key_flag = 3;
}

int application_start(int argc, char *argv[])
{
    int ret = 0;

    LOG("application started.");
    ret = fatfs_register();
    if (ret != 0) {
        KIDS_A10_PRT("fatfs_register return failed.\n");
    }
    isd9160_i2c_init();
    audio_init();
    ret |= hal_gpio_enable_irq(&brd_gpio_table[GPIO_KEY_1],
                               IRQ_TRIGGER_RISING_EDGE, key1_handle, NULL);
    ret |= hal_gpio_enable_irq(&brd_gpio_table[GPIO_KEY_2],
                               IRQ_TRIGGER_RISING_EDGE, key2_handle, NULL);
    ret |= hal_gpio_enable_irq(&brd_gpio_table[GPIO_KEY_3],
                               IRQ_TRIGGER_RISING_EDGE, key3_handle, NULL);
    if (ret != 0) {
        printf("hal_gpio_enable_irq key return failed.\n");
    }
    aos_task_new("isd9160_loop", isd9160_loop, NULL, 4096);

    return 0;
}
