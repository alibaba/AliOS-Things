/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <syscall_test.h>

/* run function @f, compare the return value with @e */
#define RUN_TEST_CASE(f, e) \
    do { \
        if (!f) { \
            printf("invalid func %s\r\n", #f); \
        } else { \
            int ret; \
            printf("------------------------\r\n"); \
            ret = f(); \
            if (ret == e) { \
                printf("%s pass!\r\n", #f); \
            } else { \
                printf("%s fail!, ret code 0x%x\r\n", \
                       #f, ret); \
            } \
            printf("------------------------\r\n\r\n"); \
        } \
    } while (0)

void run_test_case(void)
{
    RUN_TEST_CASE(task_test, 0);
    RUN_TEST_CASE(time_test, 0);
    RUN_TEST_CASE(timer_test, 0);
    RUN_TEST_CASE(mutex_test, 0);
    RUN_TEST_CASE(semphore_test, 0);
    RUN_TEST_CASE(buf_queue_test, 0);
}

