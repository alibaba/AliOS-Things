/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>

#include "yunit/yunit.h"
#include "yunit/yts.h"

#include "aos/kernel.h"
#include "aos/kv.h"

#include "aos/hal/flash.h"

static char *g_key_1 = "key_1";
static char *g_key_2 = "key_2";
static char *g_key_3 = "key_3";
static char *g_key_4 = "key_4";

static char *g_val_1 = "val_1";
static char *g_val_2 = "val_2";
static char *g_val_3 = "val_3";
static char *g_val_4 = "val_4";

#ifdef YTS_LINUX
static char *g_key_update = "test_1000";
static char *g_val_update = "val_19";
static char *g_val_update_2 = "val_30";
#endif

static void test_kv_add(void)
{
    int ret = 0;
    ret = aos_kv_set(g_key_1, g_val_1, strlen(g_val_1),1);
    YUNIT_ASSERT(0 == ret);

    ret = aos_kv_set(g_key_2, g_val_2, strlen(g_val_2),1);
    YUNIT_ASSERT(0 == ret);

    ret = aos_kv_set(g_key_3, g_val_3, strlen(g_val_3),1);
    YUNIT_ASSERT(0 == ret);

    ret = aos_kv_set(g_key_4, g_val_4, strlen(g_val_4),1);
    YUNIT_ASSERT(0 == ret);
}

static void test_kv_find(void)
{
    int ret = 0;
    char buf[10] = {0};
    int len = sizeof(buf);

    ret = aos_kv_get(g_key_1,buf,&len);
    YUNIT_ASSERT(0 == ret);
    YUNIT_ASSERT(len == strlen(g_val_1));

    ret = aos_kv_get(g_key_2,buf,&len);
    YUNIT_ASSERT(0 == ret);
    YUNIT_ASSERT(len == strlen(g_val_2));

    ret = aos_kv_get(g_key_3,buf,&len);
    YUNIT_ASSERT(0 == ret);
    YUNIT_ASSERT(len == strlen(g_val_3));

    ret = aos_kv_get(g_key_4,buf,&len);
    YUNIT_ASSERT(0 == ret);
    YUNIT_ASSERT(len == strlen(g_val_4));
}

static void test_kv_del(void)
{
    int ret = 0;
    char buf[10] = {0};
    int len = sizeof(buf);

    ret = aos_kv_del(g_key_1);
    YUNIT_ASSERT(0 == ret);

    ret = aos_kv_del(g_key_2);
    YUNIT_ASSERT(0 == ret);

    ret = aos_kv_del(g_key_3);
    YUNIT_ASSERT(0 == ret);

    ret = aos_kv_get(g_key_3,buf,&len);
    YUNIT_ASSERT(0 != ret);
    YUNIT_ASSERT(len != strlen(g_val_3)+1);
}

#ifdef YTS_LINUX
static void test_kv_loop(void)
{
    int i, j, count, ret = 0;
    char key[10] = {0};
    char val[10] = {0};
    int len = sizeof(val);

    count = 0;
    for (j = 0; j < 10; j++) {
        for (i = 0; i < 100; i++) {
            snprintf(key, sizeof(key), "test_%d", i);
            snprintf(val, sizeof(val), "val_%d", i);
            ret = aos_kv_set(key, val, strlen(val),1);
            if (ret != 0)
                count++;
            memset(key, 0, sizeof(key));
            memset(val, 0, sizeof(val));
        }

        ret = aos_kv_set(g_key_update, g_val_update, strlen(g_val_update), 1);
        if (ret != 0)
            count++;

        ret = aos_kv_set(g_key_update, g_val_update_2, strlen(g_val_update_2), 1);
        if (ret != 0)
            count++;

        for (i = 0; i < 100; i++) {
            len = sizeof(val);
            snprintf(key, sizeof(key), "test_%d", i);
            ret = aos_kv_get(key, val, &len);
            if ((ret != 0) || (strlen(val) != len))
                count++;
            memset(key, 0, sizeof(key));
            memset(val, 0, sizeof(val));
        }

        for (i = 0; i < 100; i++) {
            snprintf(key, sizeof(key), "test_%d", i);
            ret = aos_kv_del(key);
            if (ret != 0)
                count++;
            memset(key, 0, sizeof(key));
        }

        ret = aos_kv_get(g_key_update, val, &len);
        if ((ret != 0) || (strlen(g_val_update_2) != len))
            count++;

   }

   YUNIT_ASSERT(0 == ret);
}

static void test_kv_error_cycle(void)
{
    aos_kv_init();
    test_kv_loop();
    aos_kv_deinit();
}


/* The physical parition for key-value store */
#ifndef KV_CONFIG_PARTITION
#define KV_TEST_PTN    6
#else
#define KV_TEST_PTN    KV_CONFIG_PARTITION
#endif

#define KV_TOTAL_SIZE 8192

static void test_kv_error(void)
{
    int i, blk_size = 4096;
    int blk_nums = KV_TOTAL_SIZE / blk_size;
    uint32_t offset = 0;
    char *buf;

    aos_kv_deinit();
    buf = (char *)aos_malloc(blk_size);
    if (!buf) {
        YUNIT_FAIL("malloc failure");
        return;
    }

    /* case situation : all partition filled by zero */
    memset(buf, 0, blk_size);
    for (i = 0; i < blk_nums; i++) {
        offset = i * blk_size;
        hal_flash_erase(KV_TEST_PTN, offset, blk_size);
        hal_flash_write(KV_TEST_PTN, &offset, buf, blk_size);
    }
    test_kv_error_cycle();

    /* case situation : block header state is error */
    buf[0] = 'K';
    offset = 0;
    hal_flash_erase(KV_TEST_PTN, offset, blk_size);
    hal_flash_write(KV_TEST_PTN, &offset, buf, blk_size);
    test_kv_error_cycle();

    /* case situation : block header is normal, but others is filled by 0 */
    buf[0] = 'K';
    buf[1] = 0xCC;
    for (i = 0; i < blk_nums; i++) {
        offset = i * blk_size;
        hal_flash_erase(KV_TEST_PTN, offset, blk_size);
        hal_flash_write(KV_TEST_PTN, &offset, buf, blk_size);
    }
    test_kv_error_cycle();

    /* case situation : one middle block is abnormal, and other block is clean */
    memset(buf, -1, blk_size);
    buf[0] = 'K';
    buf[1] = 0xEE;
    for (i = 0; i < blk_nums; i++) {
        offset = i * blk_size;
        hal_flash_erase(KV_TEST_PTN, offset, blk_size);
        hal_flash_write(KV_TEST_PTN, &offset, buf, blk_size);
    }
    buf[1] = 0;
    offset = blk_size;
    hal_flash_erase(KV_TEST_PTN, offset, blk_size);
    hal_flash_write(KV_TEST_PTN, &offset, buf, blk_size);
    test_kv_error_cycle();

    /* case situation : one block is clean, one block is dirty, but header means clean */
    memset(buf, -1, blk_size);
    buf[0] = 'K';
    buf[1] = 0xEE;
    offset = 0;
    hal_flash_erase(KV_TEST_PTN, offset, blk_size);
    hal_flash_write(KV_TEST_PTN, &offset, buf, blk_size);

    memset(buf+2, 0, 100);
    offset = blk_size;
    hal_flash_erase(KV_TEST_PTN, offset, blk_size);
    hal_flash_write(KV_TEST_PTN, &offset, buf, blk_size);

    aos_kv_init();
    memset(buf, 0, blk_size);
    offset = blk_size;
    hal_flash_read(KV_TEST_PTN, &offset, buf, blk_size);
    YUNIT_ASSERT(buf[1] == 0x44);
    aos_kv_deinit();


    if(buf)
        aos_free(buf);
    return;
}
#endif

static int init(void)
{
    int ret = 0;

    ret = aos_kv_init();
    YUNIT_ASSERT(ret == 0);
    return 0;
}

static int cleanup(void)
{
    int ret = aos_kv_init();
    YUNIT_ASSERT(ret == 0);
    return 0;
}

static void setup(void)
{

}

static void teardown(void)
{

}

static yunit_test_case_t aos_kv_testcases[] = {
    { "kv_add", test_kv_add },
    { "kv_find", test_kv_find },
    { "kv_del", test_kv_del },
#ifdef YTS_LINUX
    { "kv_loop", test_kv_loop},
    { "kv_error", test_kv_error},
#endif
    YUNIT_TEST_CASE_NULL
};

static yunit_test_suite_t suites[] = {
    { "kv", init, cleanup, setup, teardown, aos_kv_testcases },
    YUNIT_TEST_SUITE_NULL
};

void test_kv(void)
{
    yunit_add_test_suites(suites);
}
AOS_TESTCASE(test_kv);
