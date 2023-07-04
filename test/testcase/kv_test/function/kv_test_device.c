/*
 * Copyright (C) 2015-2023 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <yunit/yunit.h>
#include <aos/kernel.h>
#include "aos_hal_flash.h"
#include "kv_adapt.h"
#include "aos_system.h"

/* module name used by log print*/
#define MODULE_NAME "kv_test"

/* module parameters */
const char *test_key = "test_kv_key";
const char *test_val = "test_kv_value";
static void *kv_test_sem = NULL;
static uint8_t sem_flag = 0;

/* test kv init */
void kv_test_init(void)
{
    int ret;
    ret=aos_hal_flash_init(HAL_PARTITION_APPLICATION);
    YUNIT_ASSERT_EQUAL(ret, 0);
}

/* test kv flash */
void kv_test_flash(void)
{
    int ret;
    uint8_t txdata = 0x5a;
    uint8_t rxdata;
    uint8_t offset = 0;
    ret = kv_flash_write(0, &txdata, 1);
    YUNIT_ASSERT_EQUAL(ret, 0);
    kv_flash_read(offset, &rxdata, 1);
    YUNIT_ASSERT_EQUAL(rxdata, txdata);
    ret = kv_flash_erase(0, 1);
    YUNIT_ASSERT_EQUAL(ret, 0);
    kv_flash_read(offset, &rxdata, 1);
    YUNIT_ASSERT_EQUAL(rxdata, 0xff);
}

/* test kv lock */
void kv_test_lock(void)
{
    int ret;
    void *lock_pin = NULL;
    lock_pin = kv_lock_create();
    YUNIT_ASSERT_TRUE(lock_pin != NULL);
    ret = kv_lock(lock_pin);
    YUNIT_ASSERT_EQUAL(ret, 0);
    ret = kv_unlock(lock_pin);
    YUNIT_ASSERT_EQUAL(ret, 0);
    ret = kv_lock_free(lock_pin);
    YUNIT_ASSERT_EQUAL(ret, 0);
}

/* test kv malloc */
void kv_test_malloc(void)
{
    int ret;
    void *malloc_pin = NULL;
    malloc_pin = kv_malloc(1);
    YUNIT_ASSERT_TRUE(malloc_pin != NULL);
    kv_free(malloc_pin);
}

/* test kv task */
void _kv_test_process(void *arg)
{
    while (1)
    {
        aos_printf("[yunit test]test kc task\n");
        kv_sem_wait(kv_test_sem);
        sem_flag = 1;
        aos_msleep(100);
        break;
    }
    kv_delete_task();
}

void kv_test_task(void)
{
    int ret;
    kv_test_sem = kv_sem_create(); /* test kv sem */
    YUNIT_ASSERT_TRUE(kv_test_sem != NULL);
    ret = kv_start_task("kv_test", _kv_test_process, NULL, 4096); /* test kv task */
    YUNIT_ASSERT_EQUAL(ret, 0);
    if (ret == 0)
    {
        ret = kv_sem_post_all(kv_test_sem);
        YUNIT_ASSERT_EQUAL(ret, 0);
    }
}