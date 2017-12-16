/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <aos/kernel.h>
#include <aos/aos.h>
#include <aos/network.h>

#include "vfs.h"
#include "vfs_inode.h"
#include "vfs_register.h"
#include "vfs_err.h"

#include "yunit.h"

#include "device/vfs_adc.h"
#include "hal/soc/adc.h"
#include "device/vfs_device.h"
#include "hal/soc/soc.h"

uint32_t adc_init_count = 0;
uint32_t adc_finalize_count = 0;

adc_dev_t adc_dev_test =
{
    .port = 0xCD,
    .config.sampling_cycle = 0x12345678
};

gpio_dev_t gpio_dev_test =
{
    .port = 0x11,
    .config = INPUT_PULL_UP
};

i2c_dev_t i2c_dev_test =
{
    .port = 0x22,
    .config.address_width = 0x11111111,
    .config.freq = 0x22222222,
    .config.mode = 0x33,
    .config.dev_addr = 0x1234
};

rtc_dev_t rtc_dev_test =
{
    .port = 0x33
};

char* adc_path = "/dev/adc/";
char* gpio_path = "/dev/gpio/";
char* i2c_path = "/dev/i2c/";
char* rtc_path = "/dev/rtc/";

int32_t hal_adc_init(adc_dev_t *adc)
{
    adc_dev_t *adc_dev = adc;
    int32_t ret = -1;

    adc_init_count++;

    if (adc == NULL) {
        return -1;
    }

    if ((adc_dev->port == 0xCD)
       &&(adc_dev->config.sampling_cycle == 0x12345678)) {
          ret = 0;
      }

    return ret;
}

int32_t hal_adc_value_get(adc_dev_t *adc, void *output, uint32_t timeout)
{
    int32_t ret = -1;
    int32_t *pData = (int32_t *)output;

    if ((adc == NULL)||(output == NULL)) {
        return -1;
    }

    pData[0] = 0x87654321;

    ret = 0;

    return ret;
}

int32_t hal_adc_finalize(adc_dev_t *adc)
{
    int32_t ret = -1;

    adc_finalize_count++;

    if (adc == NULL) {
        return -1;
    }

    ret = 0;

    return ret;
}

int32_t hal_gpio_init(gpio_dev_t *gpio)
{
    int ret = -1;

    if(gpio->port == 0x11)
    {
        ret = 0;
    }

    return ret;
}

int32_t hal_gpio_output_high(gpio_dev_t *gpio)
{
    return 1;
}

int32_t hal_gpio_output_low(gpio_dev_t *gpio)
{
    return 2;
}

int32_t hal_gpio_output_toggle(gpio_dev_t *gpio)
{
    return 3;
}

int32_t hal_gpio_input_get(gpio_dev_t *gpio, uint32_t *value)
{
    *value = 0x12345678;
    return 0;
}

int32_t hal_gpio_finalize(gpio_dev_t *gpio)
{
    return 0;
}

int32_t hal_i2c_init(i2c_dev_t *i2c)
{
    int ret = -1;

    if((i2c->port == 0x22) && (i2c->config.dev_addr == 0x1234)
      && (i2c->config.address_width == 0x11111111)
      && (i2c->config.mode == 0x33))
    {
        ret = 0;
    }

    return ret;
}

int32_t hal_i2c_master_send(i2c_dev_t *i2c, uint16_t dev_addr, const uint8_t *data,
    uint16_t size, uint32_t timeout)
{
    int ret = 0;
    int i = 0;

    for (i = 0; i < 10; i++) {
        if(data[i] != i) {
            ret = -1;
        }
    }

    return ret;
}

int32_t hal_i2c_master_recv(i2c_dev_t *i2c, uint16_t dev_addr, uint8_t *data,
    uint16_t size, uint32_t timeout)
{
    int ret = 0;
    int i = 0;

    for (i = 0; i < 10; i++) {
        data[i] = i;
    }

    return ret;
}

int32_t hal_i2c_finalize(i2c_dev_t *i2c)
{
    return 0;
}

int32_t hal_rtc_init(rtc_dev_t *rtc)
{
    int ret = -1;

    if(rtc->port == 0x33)
    {
        ret = 0;
    }

    return ret;
}

int32_t hal_rtc_get_time(rtc_dev_t *rtc, rtc_time_t *time)
{
    time->year = 11;
    time->month = 22;
    time->date = 33;
    time->weekday = 44;
    time->hr = 55;
    time->min = 66;
    time->sec = 77;

    return 0;
}

int32_t hal_rtc_set_time(rtc_dev_t *rtc, const rtc_time_t *time)
{
    int ret = -1;

    if((time->year == 11) && (time->month == 22) && (time->date == 33)
    && (time->weekday == 44) && (time->hr == 55) && (time->min == 66)
    && (time->sec == 77)){
        ret = 0;
    }

    return ret;
}

int32_t hal_rtc_finalize(rtc_dev_t *rtc)
{
    return 0;
}

static void test_vfs_device_io_case(void)
{
    int fd1 = 0;
    int fd2 = 0;
    int fd_gpio = 0;
    int fd_i2c = 0;
    int fd_rtc = 0;
    int i = 0;
    int ret = -1;
    int res = 0;
    uint32_t gpio_value = 0;
    int32_t adc_val = 0;
    uint8_t write_buf[10] = {0,1,2,3,4,5,6,7,8,9};
    uint8_t read_buf[10] = {0};
    rtc_time_t rtc_time;

    memset(&rtc_time, 0, sizeof(rtc_time));

    ret = aos_register_driver(adc_path, &adc_ops, &adc_dev_test);
    YUNIT_ASSERT(ret == 0);

    /* The device can be opened several times, but is only initialized when it is first opened */
    fd1 = aos_open(adc_path,0);
    YUNIT_ASSERT((fd1 > 64)&&(adc_init_count == 1));

    fd2 = aos_open(adc_path,0);
    YUNIT_ASSERT((fd2 > 64)&&(adc_init_count == 1));

    ret = aos_read(fd1, &adc_val, sizeof(adc_val));
    YUNIT_ASSERT((ret == 4)&&(adc_val == 0x87654321));

    ret = aos_read(fd2, &adc_val, sizeof(adc_val));
    YUNIT_ASSERT((ret == 4)&&(adc_val == 0x87654321));

    /* When the device is opened many times, the hardware is only shut down at the last close */
    ret = aos_close(fd1);
    YUNIT_ASSERT((ret == 0)&&(adc_finalize_count == 0));

    ret = aos_close(fd2);
    YUNIT_ASSERT((ret == 0)&&(adc_finalize_count == 1));

    /* example of gpio */
    ret = aos_register_driver(gpio_path, &gpio_ops, &gpio_dev_test);
    YUNIT_ASSERT(ret == 0);

    fd_gpio = aos_open(gpio_path,0);
    YUNIT_ASSERT(fd1 > 64);

    /* read data from gpio */
    ret = aos_read(fd_gpio, &gpio_value, sizeof(gpio_value));
    YUNIT_ASSERT((ret == sizeof(gpio_value))&&(gpio_value == 0x12345678));

    /* output high */
    ret = aos_ioctl(fd_gpio, IOCTL_GPIO_OUTPUT_HIGHT, 0);
    YUNIT_ASSERT(ret == 1);

    /* output low */
    ret = aos_ioctl(fd_gpio, IOCTL_GPIO_OUTPUT_LOW, 0);
    YUNIT_ASSERT(ret == 2);

    /* toggle output */
    ret = aos_ioctl(fd_gpio, IOCTL_GPIO_OUTPUT_TOGGLE, 0);
    YUNIT_ASSERT(ret == 3);

    ret = aos_close(fd_gpio);
    YUNIT_ASSERT(ret == 0);

    /* example of i2c */
    ret = aos_register_driver(i2c_path, &i2c_ops, &i2c_dev_test);
    YUNIT_ASSERT(ret == 0);

    fd_i2c = aos_open(i2c_path,0);
    YUNIT_ASSERT(fd_i2c > 64);

    ret = aos_read(fd_i2c, read_buf, sizeof(read_buf));

    for (i = 0; i < 10; i++) {
        if(read_buf[i] != i) {
            res = -1;
        }
    }
    YUNIT_ASSERT((ret == sizeof(read_buf))&&(res == 0));

    ret = aos_write(fd_i2c, write_buf, sizeof(read_buf));
    YUNIT_ASSERT(ret == sizeof(read_buf));

    ret = aos_close(fd_i2c);
    YUNIT_ASSERT(ret == 0);

    /* example of rtc */
    ret = aos_register_driver(rtc_path, &rtc_ops, &rtc_dev_test);
    YUNIT_ASSERT(ret == 0);

    fd_rtc = aos_open(rtc_path,0);
    YUNIT_ASSERT(fd_rtc > 64);

    ret = aos_read(fd_rtc, &rtc_time, sizeof(rtc_time));

    if((rtc_time.year == 11) && (rtc_time.month == 22) && (rtc_time.date == 33)
    && (rtc_time.weekday == 44) && (rtc_time.hr == 55) && (rtc_time.min == 66)
    && (rtc_time.sec == 77)){
        res = 2;
    }
    YUNIT_ASSERT((ret == sizeof(rtc_time))&&(res == 2));

    ret = aos_write(fd_rtc, &rtc_time, sizeof(rtc_time));
    YUNIT_ASSERT(ret == sizeof(rtc_time));

    ret = aos_close(fd_rtc);
    YUNIT_ASSERT(ret == 0);
}

static yunit_test_case_t aos_deviceIO_testcases[] = {
    { "device_io", test_vfs_device_io_case},
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
    { "device_io", init, cleanup, setup, teardown, aos_deviceIO_testcases },
    YUNIT_TEST_SUITE_NULL
};

void test_deviceIO(void)
{
    yunit_add_test_suites(suites);
}
AOS_TESTCASE(test_deviceIO);
