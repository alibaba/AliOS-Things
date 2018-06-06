/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <k_ringbuf.h>
#include <test_fw.h>
#include "ringbuf_test.h"

#define MODULE_NAME "ringbuf_break"

#define RINGBUF_SIZE_SMALL  16
#define RINGBUF_SIZE_MIDDLE (256+16)
#define RINGBUF_SIZE_BIG    (16384 * 2 + 32)

#define RINGBUF_FIX_LEN     4

#if (RHINO_CONFIG_RINGBUF_VENDOR > 0)

static k_ringbuf_t  g_fix_ringbuf;
static k_ringbuf_t  g_dyn_ringbuf;

static char *push_data[] = {
    "1111",
    "2222",
    "3333",
    "4444",
    "5555",
    "6666"
};
uint8_t ringbuf_small_buf[RINGBUF_SIZE_SMALL];
uint8_t ringbuf_middle_buf[RINGBUF_SIZE_MIDDLE];
uint8_t dyn_data_big_buf[RINGBUF_SIZE_BIG];

uint8_t dyn_data_small[RINGBUF_SIZE_SMALL / 2 - 2];
uint8_t dyn_data_middle[RINGBUF_SIZE_MIDDLE];
uint8_t dyn_data_big_rev[RINGBUF_SIZE_BIG];
uint8_t dyn_data_big[RINGBUF_SIZE_BIG];

#endif

static uint8_t ringbuf_break_case_fix(void)
{
#if (RHINO_CONFIG_RINGBUF_VENDOR > 0)
    kstat_t  ret;
    uint8_t  rev[RINGBUF_FIX_LEN];
    size_t   len;
    size_t   i = 0;

    ret = krhino_ringbuf_init(NULL, NULL, 0, 0, 0);
    MYASSERT(ret == RHINO_NULL_PTR);

    ret = krhino_ringbuf_init(&g_fix_ringbuf, NULL, 0, RINGBUF_TYPE_FIX, 0);
    MYASSERT(ret == RHINO_NULL_PTR);

    ret = krhino_ringbuf_init(&g_fix_ringbuf, (void *)ringbuf_small_buf, 0,
                              RINGBUF_TYPE_FIX, 0);
    MYASSERT(ret == RHINO_INV_PARAM);

    ret = krhino_ringbuf_init(&g_fix_ringbuf, (void *)ringbuf_small_buf,
                              sizeof(ringbuf_small_buf), 3, 0);
    MYASSERT(ret == RHINO_INV_PARAM);

    ret = krhino_ringbuf_init(&g_fix_ringbuf, (void *)ringbuf_small_buf,
                              sizeof(ringbuf_small_buf), RINGBUF_TYPE_FIX, 0);
    MYASSERT(ret == RHINO_INV_PARAM);

    ret = krhino_ringbuf_init(&g_fix_ringbuf, (void *)ringbuf_small_buf,
                              sizeof(ringbuf_small_buf), RINGBUF_TYPE_FIX, RINGBUF_FIX_LEN);
    MYASSERT(ret == RHINO_SUCCESS);

    ret = krhino_ringbuf_push(NULL, NULL, 0);
    MYASSERT(ret == RHINO_NULL_PTR);

    ret = krhino_ringbuf_push(&g_fix_ringbuf, NULL, 0);
    MYASSERT(ret == RHINO_NULL_PTR);


    ret = krhino_ringbuf_push(&g_fix_ringbuf, (void *)push_data[0], 1);
    MYASSERT(ret == RHINO_INV_PARAM);

    ret = krhino_ringbuf_push(&g_fix_ringbuf, (void *)push_data[0], 4);
    MYASSERT(ret == RHINO_SUCCESS);

    ret = krhino_ringbuf_pop(NULL, NULL, 0);
    MYASSERT(ret == RHINO_NULL_PTR);

    ret = krhino_ringbuf_pop(&g_fix_ringbuf, NULL, 0);
    MYASSERT(ret == RHINO_NULL_PTR);

    ret = krhino_ringbuf_pop(&g_fix_ringbuf, (void *)&rev[0], 0);
    MYASSERT(ret == RHINO_SUCCESS);

    ret = memcmp(rev, push_data[0], 4);
    MYASSERT(ret == RHINO_SUCCESS);

    /*empty now,push full */
    ret = krhino_ringbuf_push(&g_fix_ringbuf, (void *)push_data[1], 4);
    MYASSERT(ret == RHINO_SUCCESS);

    ret = krhino_ringbuf_push(&g_fix_ringbuf, (void *)push_data[1], 4);
    MYASSERT(ret == RHINO_SUCCESS);

    ret = krhino_ringbuf_push(&g_fix_ringbuf, (void *)push_data[2], 4);
    MYASSERT(ret == RHINO_SUCCESS);

    ret = krhino_ringbuf_push(&g_fix_ringbuf, (void *)push_data[3], 4);
    MYASSERT(ret == RHINO_SUCCESS);

    /* test ringbuf full */
    ret = krhino_ringbuf_push(&g_fix_ringbuf, (void *)push_data[4], 4);
    MYASSERT(ret == RHINO_RINGBUF_FULL);

    ret = krhino_ringbuf_is_full(&g_fix_ringbuf);
    MYASSERT(ret == true);

    ret = krhino_ringbuf_pop(&g_fix_ringbuf, (void *)&rev[0], NULL);
    MYASSERT(ret == RHINO_SUCCESS);

    ret = memcmp(rev, (uint8_t *)push_data[1], 4);
    MYASSERT(ret == RHINO_SUCCESS);

    ret = krhino_ringbuf_push(&g_fix_ringbuf, (void *)push_data[4], 4);
    MYASSERT(ret == RHINO_SUCCESS);

    ret = krhino_ringbuf_pop(&g_fix_ringbuf, (void *)&rev[0], &len);
    MYASSERT(ret == RHINO_SUCCESS);

    ret = memcmp(rev, (uint8_t *)push_data[1], 4);
    MYASSERT(ret == RHINO_SUCCESS);

    /* test tail==end */
    ret = krhino_ringbuf_push(&g_fix_ringbuf, (void *)push_data[5], 4);
    MYASSERT(ret == RHINO_SUCCESS);

    ret = krhino_ringbuf_pop(&g_fix_ringbuf, (void *)&rev[0], &len);
    MYASSERT(ret == RHINO_SUCCESS);

    ret = memcmp(rev, (uint8_t *)push_data[2], 4);
    MYASSERT(ret == RHINO_SUCCESS);

    for (i = 0; i < 4; ++i) {
        ret = krhino_ringbuf_push(&g_fix_ringbuf, (void *)push_data[0], 4);
        MYASSERT(ret == RHINO_SUCCESS);

        ret = krhino_ringbuf_pop(&g_fix_ringbuf, (void *)&rev[0], &len);
        MYASSERT(ret == RHINO_SUCCESS);
    }

    ret = krhino_ringbuf_reset(&g_fix_ringbuf);
    MYASSERT(ret == RHINO_SUCCESS);

    ret = krhino_ringbuf_is_empty(&g_fix_ringbuf);
    MYASSERT(ret == true);
#endif
    return 0;
}
static uint8_t ringbuf_break_case_dyn(void)
{
#if (RHINO_CONFIG_RINGBUF_VENDOR > 0)
    kstat_t ret;
    size_t  data_len;
    size_t  len;
    size_t  i = 0;

    memset(dyn_data_small, '1', sizeof(dyn_data_small));
    memset(dyn_data_middle, '2', sizeof(dyn_data_middle));

    ret = krhino_ringbuf_init(NULL, NULL, 0, 0, 0);
    MYASSERT(ret == RHINO_NULL_PTR);

    /* small buffer test */
    ret = krhino_ringbuf_init(&g_dyn_ringbuf, NULL, 0, RINGBUF_TYPE_DYN, 0);
    MYASSERT(ret == RHINO_NULL_PTR);

    ret = krhino_ringbuf_init(&g_dyn_ringbuf, ringbuf_small_buf, 0, RINGBUF_TYPE_DYN,
                              0);
    MYASSERT(ret == RHINO_INV_PARAM);

    ret = krhino_ringbuf_init(&g_dyn_ringbuf, ringbuf_small_buf, RINGBUF_SIZE_SMALL,
                              3, 0);
    MYASSERT(ret == RHINO_INV_PARAM);

    ret = krhino_ringbuf_init(&g_dyn_ringbuf, ringbuf_small_buf, RINGBUF_SIZE_SMALL,
                              RINGBUF_TYPE_DYN, 0);
    MYASSERT(ret == RHINO_SUCCESS);

    ret = krhino_ringbuf_push(&g_dyn_ringbuf, dyn_data_small,
                              sizeof(dyn_data_small));
    MYASSERT(ret == RHINO_SUCCESS);

    ret = krhino_ringbuf_pop(&g_dyn_ringbuf, NULL, NULL);
    MYASSERT(ret == RHINO_NULL_PTR);

    ret = krhino_ringbuf_pop(&g_dyn_ringbuf, dyn_data_big_rev, NULL);
    MYASSERT(ret == RHINO_INV_PARAM);

    ret = krhino_ringbuf_pop(&g_dyn_ringbuf, dyn_data_big_rev, &len);
    MYASSERT(ret == RHINO_SUCCESS);
    MYASSERT(len == sizeof(dyn_data_small));

    ret = krhino_ringbuf_pop(&g_dyn_ringbuf, dyn_data_big_rev, &len);
    MYASSERT(ret == RHINO_RINGBUF_EMPTY);

    ret = krhino_ringbuf_push(&g_dyn_ringbuf, dyn_data_small,
                              sizeof(dyn_data_small));
    MYASSERT(ret == RHINO_SUCCESS);

    ret = krhino_ringbuf_push(&g_dyn_ringbuf, dyn_data_small,
                              sizeof(dyn_data_small));
    MYASSERT(ret == RHINO_RINGBUF_FULL);

    ret = krhino_ringbuf_push(&g_dyn_ringbuf, dyn_data_small,
                              sizeof(dyn_data_small));
    MYASSERT(ret == RHINO_RINGBUF_FULL);

    ret = krhino_ringbuf_pop(&g_dyn_ringbuf, dyn_data_big_rev, &len);
    MYASSERT(ret == RHINO_SUCCESS);
    MYASSERT(len == sizeof(dyn_data_small));

    ret = krhino_ringbuf_push(&g_dyn_ringbuf, dyn_data_small,
                              sizeof(dyn_data_small));
    MYASSERT(ret == RHINO_SUCCESS);

    ret = krhino_ringbuf_pop(&g_dyn_ringbuf, dyn_data_big_rev, &len);
    MYASSERT(ret == RHINO_SUCCESS);
    MYASSERT(len == sizeof(dyn_data_small));

    /* middle buffer */
    ret = krhino_ringbuf_init(&g_dyn_ringbuf, ringbuf_middle_buf,
                              RINGBUF_SIZE_MIDDLE, RINGBUF_TYPE_DYN, 0);
    MYASSERT(ret == RHINO_SUCCESS);

    data_len = sizeof(dyn_data_middle) - sizeof(size_t);

    ret = krhino_ringbuf_push(&g_dyn_ringbuf, dyn_data_middle, data_len);
    MYASSERT(ret == RHINO_SUCCESS);

    ret = krhino_ringbuf_is_full(&g_dyn_ringbuf);
    MYASSERT(ret == true);

    ret = krhino_ringbuf_pop(&g_dyn_ringbuf, dyn_data_big_rev, &len);
    MYASSERT(ret == RHINO_SUCCESS);
    MYASSERT(len == data_len);
    ret = memcmp(dyn_data_big_rev, dyn_data_middle, data_len);
    MYASSERT(ret == RHINO_SUCCESS);

    data_len = sizeof(dyn_data_middle) / 2 - 4;

    memset(dyn_data_middle, '3', sizeof(dyn_data_middle));

    ret = krhino_ringbuf_push(&g_dyn_ringbuf, dyn_data_middle, data_len);
    MYASSERT(ret == RHINO_SUCCESS);

    ret = krhino_ringbuf_pop(&g_dyn_ringbuf, dyn_data_big_rev, &len);
    MYASSERT(ret == RHINO_SUCCESS);
    MYASSERT(len == data_len);
    ret = memcmp(dyn_data_big_rev, dyn_data_middle, data_len);
    MYASSERT(ret == RHINO_SUCCESS);

    ret = krhino_ringbuf_pop(&g_dyn_ringbuf, dyn_data_big_rev, &len);
    MYASSERT(ret == RHINO_RINGBUF_EMPTY);
    ret = memcmp(dyn_data_big_rev, dyn_data_middle, data_len);
    MYASSERT(ret == RHINO_SUCCESS);

    memset(dyn_data_middle, '5', sizeof(dyn_data_middle));
    ret = krhino_ringbuf_push(&g_dyn_ringbuf, dyn_data_middle, data_len);
    MYASSERT(ret == RHINO_SUCCESS);

    memset(dyn_data_middle, '4', sizeof(dyn_data_middle));
    ret = krhino_ringbuf_push(&g_dyn_ringbuf, dyn_data_middle, data_len);
    MYASSERT(ret == RHINO_SUCCESS);

    ret = krhino_ringbuf_push(&g_dyn_ringbuf, dyn_data_middle, data_len);
    MYASSERT(ret == RHINO_RINGBUF_FULL);

    memset(dyn_data_middle, '5', sizeof(dyn_data_middle));
    ret = krhino_ringbuf_pop(&g_dyn_ringbuf, dyn_data_big_rev, &len);
    MYASSERT(ret == RHINO_SUCCESS);
    MYASSERT(len == data_len);
    ret = memcmp(dyn_data_big_rev, dyn_data_middle, data_len);
    MYASSERT(ret == RHINO_SUCCESS);

    memset(dyn_data_middle, '6', sizeof(dyn_data_middle));
    ret = krhino_ringbuf_push(&g_dyn_ringbuf, (void *)dyn_data_middle, data_len);
    MYASSERT(ret == RHINO_SUCCESS);

    memset(dyn_data_middle, '4', sizeof(dyn_data_middle));
    ret = krhino_ringbuf_pop(&g_dyn_ringbuf, dyn_data_big_rev, &len);
    MYASSERT(ret == RHINO_SUCCESS);
    MYASSERT(len == data_len);
    ret = memcmp(dyn_data_big_rev, dyn_data_middle, data_len);
    MYASSERT(ret == RHINO_SUCCESS);

    memset(dyn_data_middle, '6', sizeof(dyn_data_middle));
    ret = krhino_ringbuf_pop(&g_dyn_ringbuf, dyn_data_big_rev, &len);
    MYASSERT(ret == RHINO_SUCCESS);
    MYASSERT(len == data_len);
    ret = memcmp(dyn_data_big_rev, dyn_data_middle, data_len);
    MYASSERT(ret == RHINO_SUCCESS);

    ret = krhino_ringbuf_reset(&g_dyn_ringbuf);
    MYASSERT(ret == RHINO_SUCCESS);

    /* random test */
    for (i = 0; i < 5; ++i) {
        data_len = sizeof(dyn_data_middle) / 2 - i;
        memset(dyn_data_middle, i, sizeof(dyn_data_middle));

        ret = krhino_ringbuf_push(&g_dyn_ringbuf, dyn_data_middle, data_len);
        MYASSERT(ret == RHINO_SUCCESS);

        ret = krhino_ringbuf_pop(&g_dyn_ringbuf, dyn_data_big_rev, &len);
        MYASSERT(ret == RHINO_SUCCESS);
        MYASSERT(len == data_len);
        ret = memcmp(dyn_data_big_rev, dyn_data_middle, data_len);
        MYASSERT(ret == RHINO_SUCCESS);

    }

    /* big data test */

    memset(dyn_data_big, '3', RINGBUF_SIZE_BIG);


    ret = krhino_ringbuf_init(&g_dyn_ringbuf, dyn_data_big_buf, RINGBUF_SIZE_BIG,
                              RINGBUF_TYPE_DYN, 0);
    MYASSERT(ret == RHINO_SUCCESS);

    data_len = sizeof(dyn_data_big_buf) - 4;

    ret = krhino_ringbuf_push(&g_dyn_ringbuf, (void *)dyn_data_big, data_len);
    MYASSERT(ret == RHINO_SUCCESS);

    ret = krhino_ringbuf_pop(&g_dyn_ringbuf, dyn_data_big_rev, &len);
    MYASSERT(ret == RHINO_SUCCESS);
    MYASSERT(len == data_len);
    ret = memcmp(dyn_data_big_rev, dyn_data_big, data_len);
    MYASSERT(ret == RHINO_SUCCESS);

    data_len = sizeof(dyn_data_big_buf) / 2 - 4;

    ret = krhino_ringbuf_push(&g_dyn_ringbuf, (void *)dyn_data_big, data_len);
    MYASSERT(ret == RHINO_SUCCESS);

    ret = krhino_ringbuf_pop(&g_dyn_ringbuf, dyn_data_big_rev, &len);
    MYASSERT(ret == RHINO_SUCCESS);
    MYASSERT(len == data_len);
    ret = memcmp(dyn_data_big_rev, dyn_data_big, data_len);
    MYASSERT(ret == RHINO_SUCCESS);

    ret = krhino_ringbuf_pop(&g_dyn_ringbuf, dyn_data_big_rev, &len);
    MYASSERT(ret == RHINO_RINGBUF_EMPTY);

    ret = krhino_ringbuf_push(&g_dyn_ringbuf, (void *)dyn_data_big, data_len);
    MYASSERT(ret == RHINO_SUCCESS);

    ret = krhino_ringbuf_push(&g_dyn_ringbuf, (void *)dyn_data_big, data_len);
    MYASSERT(ret == RHINO_SUCCESS);

    ret = krhino_ringbuf_push(&g_dyn_ringbuf, (void *)dyn_data_big, data_len);
    MYASSERT(ret == RHINO_RINGBUF_FULL);

    ret = krhino_ringbuf_pop(&g_dyn_ringbuf, dyn_data_big_rev, &len);
    MYASSERT(ret == RHINO_SUCCESS);
    MYASSERT(len == data_len);

    ret = krhino_ringbuf_push(&g_dyn_ringbuf, (void *)dyn_data_big, data_len);
    MYASSERT(ret == RHINO_SUCCESS);

    ret = krhino_ringbuf_pop(&g_dyn_ringbuf, dyn_data_big_rev, &len);
    MYASSERT(ret == RHINO_SUCCESS);
    MYASSERT(len == data_len);

#endif

    return 0;
}

static const test_func_t ringbuf_func_runner[] = {
    ringbuf_break_case_fix,
    ringbuf_break_case_dyn,
    NULL
};

void ringbuf_break_test(void)
{
    kstat_t ret;

    task_ringbuf_entry_register(MODULE_NAME, (test_func_t *)ringbuf_func_runner,
                                sizeof(ringbuf_func_runner) / sizeof(test_func_t));

    ret = krhino_task_dyn_create(&task_ringbuf, MODULE_NAME, 0, TASK_RINGBUF_PRI,
                                 0, TASK_TEST_STACK_SIZE, task_ringbuf_entry, 1);

    if ((ret != RHINO_SUCCESS) && (ret != RHINO_STOPPED)) {
        test_case_fail++;
        PRINT_RESULT(MODULE_NAME, FAIL);
    }
}

