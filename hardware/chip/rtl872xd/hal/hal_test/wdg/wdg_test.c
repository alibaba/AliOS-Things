/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "aos/kernel.h"
#include "aos/hal/wdg.h"
#include "aos/hal/flash.h"
#include "wdg_test.h"

/* if WGD_TIMEOUT_MS is 1000, WGD_TIMEOUT_TOLERANCE_RANGE is 0.3f, wdg test will be 
   OK if the real timeout in the range [700, 1300] */
#define WGD_TIMEOUT_MS 1000
#define WGD_TIMEOUT_TOLERANCE_RANGE 0.2f

static wdg_dev_t wdg_dev;

static hal_logic_partition_t *partition_info = NULL;

static int success_flag = 0;

static void wdg_timeout_test(void);
static void wdg_normal_test(void);
static uint16_t read_wdg_flag(void);
static void write_wdg_flag(uint16_t value);
static void clear_wdg_flag(void);

void hal_wdg_test(void)
{
    int ret = -1;

    uint16_t wdg_flag = 0;

    printf("*********** wdg test begin ! ***********\n");

    /* init the wdg */
    wdg_dev.port = PORT_WDG_TEST;
    wdg_dev.config.timeout = WGD_TIMEOUT_MS;
    ret = hal_wdg_init(&wdg_dev);
    if (ret != 0) {
        printf("hal_wdg_init error !\n");
        return;
    }

    partition_info = hal_flash_get_info(PARTITION_WDG_TEST);
    if (partition_info == NULL) {
        printf("wdg_flag get error !\n");
    }

    /* get the wdg flag */
    wdg_flag = read_wdg_flag();

    switch(wdg_flag) {
        case 0xF000:
            printf("wdg_normal_test failed !\n");
            success_flag = 0;
            break;
        case 0xFF00:
            printf("wdg_timeout_test failed !\n");
            success_flag = 0;
            break;
        case 0xFFF0:
            printf("wdg_normal_test begin !\n");
            wdg_normal_test();

            wdg_flag = read_wdg_flag();
            if (wdg_flag == 0x0000) {
                success_flag = 1;
            } else {
                printf("wdg_normal_test failed !\n");
                success_flag = 0;
            }
            break;
        case 0xFFFF:
            printf("wdg_timeout_test begin !\n");
            wdg_timeout_test();
            printf("wdg_timeout_test failed !\n");
            success_flag = 0;
            break;
        default:
            printf("wdg flag error !\n");
            success_flag = 0;
            break;
    }

    ret = hal_wdg_finalize(&wdg_dev);
    if (ret != 0) {
        printf("hal_wdg_finalize error !\n");
    }

    if (success_flag == 0) {
        printf("wdg test result: failed !\n");
    } else {
        printf("wdg test result: succeed !\n");
    }

    clear_wdg_flag();

    printf("*********** wdg test end ! ***********\n");
}

void wdg_timeout_test(void)
{
    write_wdg_flag(0xFFF0);
    hal_wdg_reload(&wdg_dev);

    /* delay timeout to make cpu restart */
    aos_msleep(WGD_TIMEOUT_MS * (1 + WGD_TIMEOUT_TOLERANCE_RANGE));

    /* this code will not be executed because wdg timeout and cpu restart */
    write_wdg_flag(0xFF00);
}

void wdg_normal_test(void)
{
    write_wdg_flag(0xF000);
    hal_wdg_reload(&wdg_dev);

    /* delay no timeout */
    aos_msleep(WGD_TIMEOUT_MS * (1 - WGD_TIMEOUT_TOLERANCE_RANGE));

    /* this code must be executed because sleep time is shorter than timeout */
    write_wdg_flag(0x0000);
}

uint16_t read_wdg_flag(void)
{
    int      ret    = -1;
    uint16_t value  = 0;
    uint32_t offset = 0;

    offset = 0;
    ret = hal_flash_read(PARTITION_WDG_TEST, &offset, &value, sizeof(uint16_t));
    if (ret != 0) {
        printf("hal_flash_read error ! test failed\n");
        return 0;
    }

    return value;
}

void write_wdg_flag(uint16_t value)
{
    int      ret    = -1;
    uint32_t offset = 0;

    offset = 0;
    ret = hal_flash_write(PARTITION_WDG_TEST, &offset, &value, sizeof(uint16_t));
    if (ret != 0) {
        printf("hal_flash_write error ! test failed\n");
    }
}

void clear_wdg_flag(void)
{
    int ret = -1;

    ret = hal_flash_erase(PARTITION_WDG_TEST, 0, partition_info->partition_length);
    if (ret != 0) {
        printf("hal_flash_erase error ! test failed\n");
    }
}
