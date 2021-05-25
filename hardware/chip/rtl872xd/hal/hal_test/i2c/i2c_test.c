/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "aos/kernel.h"
#include "aos/hal/i2c.h"
#include "aos/hal/rtc.h"
#include "i2c_test.h"

#define RTC_DATA_LEN 7
#define I2C_RTC_DELAY_MS 2000
#define I2C_RTC_TOLERANCE_RANGE 0.01f

static int32_t rtc_i2c_init(void);
static int32_t rtc_i2c_get_time(rtc_time_t *time);
static int32_t rtc_i2c_set_time(rtc_time_t *time);

static i2c_dev_t i2c_dev;

static rtc_time_t rtc_time = {.sec = 0x45, .min = 0x8, .hr = 0x1, .weekday = 0x5, .date = 0x1, .month = 0x3, .year = 0x19};

static rtc_time_t rtc_time_r;

/* a i2c interface rtc(ds1307) must be connetted to the mcu before testing */
void hal_i2c_test(void)
{
    int ret = -1;

    printf("*********** i2c test start ! ***********\n");

    memset(&rtc_time_r, 0, sizeof(rtc_time_t));

    printf("step1: init the i2c !\n");

    ret = rtc_i2c_init();
    if (ret != 0) {
        printf("rtc_i2c_init error !\n");
        return;
    }

    printf("step2: set rtc date to\n");
    printf("        year: %x month: %x date: %x weekday: %x hr: %x min: %d sec: %x\n",
           rtc_time.year, rtc_time.month, rtc_time.date, rtc_time.weekday,
           rtc_time.hr, rtc_time.min, rtc_time.sec);

    ret = rtc_i2c_set_time(&rtc_time);
    if (ret != 0) {
        printf("rtc_i2c_set_time error !\n");
        return;
    }

    ret = rtc_i2c_get_time(&rtc_time_r);
    if (ret != 0) {
        printf("rtc_i2c_get_time error !\n");
        return;
    }

    printf("step4: read rtc, current date is\n");
    printf("        year: %x month: %x date: %x weekday: %x hr: %x min: %x sec: %x\n",
           rtc_time_r.year, rtc_time_r.month, rtc_time_r.date, rtc_time_r.weekday,
           rtc_time_r.hr, rtc_time_r.min, rtc_time_r.sec);

    printf("step3: delay 2s !\n");
    aos_msleep(I2C_RTC_DELAY_MS * (1 + I2C_RTC_TOLERANCE_RANGE));

    ret = rtc_i2c_get_time(&rtc_time_r);
    if (ret != 0) {
        printf("rtc_i2c_get_time error !\n");
        return;
    }

    printf("step4: read rtc, current date is\n");
    printf("        year: %x month: %x date: %x weekday: %x hr: %x min: %x sec: %x\n",
           rtc_time_r.year, rtc_time_r.month, rtc_time_r.date, rtc_time_r.weekday,
           rtc_time_r.hr, rtc_time_r.min, rtc_time_r.sec);

    if ((rtc_time_r.sec != rtc_time.sec + I2C_RTC_DELAY_MS / 1000)
        ||(rtc_time_r.min != rtc_time.min)
        ||(rtc_time_r.hr != rtc_time.hr)
        ||(rtc_time_r.weekday != rtc_time.weekday)
        ||(rtc_time_r.date != rtc_time.date)
        ||(rtc_time_r.month != rtc_time.month)
        ||(rtc_time_r.year != rtc_time.year)) {
        printf("rtc value error !\n");
        return;
    }

    printf("step5: finalize i2c !\n");
    ret = hal_i2c_finalize(&i2c_dev);
    if (ret != 0) {
        printf("hal_i2c_finalize error !\n");
        return;
    }

    printf("rtc test result: succeed !\n");

    printf("*********** i2c test end ! ***********\n");
}

int32_t rtc_i2c_init(void)
{
    int ret = -1;

    i2c_dev.port = PORT_I2C_TEST;
    i2c_dev.config.address_width = I2C_HAL_ADDRESS_WIDTH_7BIT;
    i2c_dev.config.freq = 100000;

#if (I2C_ADD_LSB_AUTO == 0)
    i2c_dev.config.dev_addr = 0xD0;
#else
    i2c_dev.config.dev_addr = 0x68;
#endif

    ret = hal_i2c_init(&i2c_dev);
    if (ret != 0) {
        printf("hal_i2c_init error !\n");
    }

    return ret;
}

int32_t rtc_i2c_get_time(rtc_time_t *time)
{
    int ret = -1;

    //ret = hal_i2c_master_recv(&i2c_dev, i2c_dev.config.dev_addr, (uint8_t *)time,
    //                          RTC_DATA_LEN, AOS_WAIT_FOREVER);

    ret = hal_i2c_mem_read(&i2c_dev, i2c_dev.config.dev_addr, 0, 1, (uint8_t *)time,
                           7, AOS_WAIT_FOREVER);

    return ret;
}

int32_t rtc_i2c_set_time(rtc_time_t *time)
{
    int ret = -1;

    //ret = hal_i2c_master_send(&i2c_dev, i2c_dev.config.dev_addr, (uint8_t *)time,
    //                          RTC_DATA_LEN, AOS_WAIT_FOREVER);
    ret = hal_i2c_mem_write(&i2c_dev, i2c_dev.config.dev_addr, 0, 1, (uint8_t *)time,
                           7, AOS_WAIT_FOREVER);

    return ret;
}
