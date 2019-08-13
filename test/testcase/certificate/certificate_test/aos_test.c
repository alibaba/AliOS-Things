/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <aos/kernel.h>

#include "yunit/yunit.h"

#include "cutest/cut.h"

#ifndef SYSINFO_ARCH
#define SYSINFO_ARCH        "unknown"
#endif
#ifndef SYSINFO_MCU
#define SYSINFO_MCU         "unknown"
#endif
#ifndef SYSINFO_DEVICE_NAME
#define SYSINFO_DEVICE_NAME "unknown"
#endif
#ifndef SYSINFO_APP_VERSION
#define SYSINFO_APP_VERSION "unknown"
#endif
#define SYSINFO_KERNEL      "AOS"

/* dynamic stack size */
#ifndef TEST_CONFIG_STACK_SIZE
#define TEST_CONFIG_STACK_SIZE                  (1024)
#endif

/* dynamic memory alloc test */
#ifndef TEST_CONFIG_MM_ENABLED
#define TEST_CONFIG_MM_ENABLED                  (1)
#endif

#if (TEST_CONFIG_MM_ENABLED > 0)

#ifndef TEST_CONFIG_MALLOC_MAX_SIZE
#define TEST_CONFIG_MALLOC_MAX_SIZE             (1024)
#endif

#ifndef TEST_CONFIG_MALLOC_FREE_TIMES
#define TEST_CONFIG_MALLOC_FREE_TIMES           (100000)
#endif

#endif

/* task test */
#ifndef TEST_CONFIG_TASK_ENABLED
#define TEST_CONFIG_TASK_ENABLED                (1)
#endif

#if (TEST_CONFIG_TASK_ENABLED > 0)

#ifndef TEST_CONFIG_MAX_TASK_COUNT
#define TEST_CONFIG_MAX_TASK_COUNT              (10)
#endif

#ifndef TEST_CONFIG_CREATE_TASK_TIMES
#define TEST_CONFIG_CREATE_TASK_TIMES           (10000)
#endif

#endif

/* task communication test */
#ifndef TEST_CONFIG_TASK_COMM_ENABLED
#define TEST_CONFIG_TASK_COMM_ENABLED           (1)
#endif

#if (TEST_CONFIG_TASK_COMM_ENABLED > 0)

#ifndef TEST_CONFIG_SYNC_TIMES
#define TEST_CONFIG_SYNC_TIMES                  (100000)
#endif

#ifndef TEST_CONFIG_QUEUE_BUF_SIZE
#define TEST_CONFIG_QUEUE_BUF_SIZE              (32)
#endif

#endif

/* timer test */
#ifndef TEST_CONFIG_TIMER_ENABLED
#define TEST_CONFIG_TIMER_ENABLED               (1)
#endif

/* kv test */
#ifndef TEST_CONFIG_KV_ENABLED
#define TEST_CONFIG_KV_ENABLED                  (1)
#endif

#if (TEST_CONFIG_KV_ENABLED > 0)

#ifndef TEST_CONFIG_KV_TIMES
#define TEST_CONFIG_KV_TIMES                    (10000)
#endif

#endif

static unsigned int g_var = 0;
static aos_sem_t    g_sem_taskexit_sync;
static aos_mutex_t  g_mutex;
static aos_sem_t    g_sem;
static aos_queue_t  g_queue;
static aos_timer_t  g_timer;

static char         queue_buf[TEST_CONFIG_QUEUE_BUF_SIZE];
static aos_queue_t  g_queue1;
static char         queue1_buf[TEST_CONFIG_QUEUE_BUF_SIZE];
static aos_queue_t  g_queue2;
static char         queue2_buf[TEST_CONFIG_QUEUE_BUF_SIZE];
static aos_queue_t  g_queue3;
static char         queue3_buf[TEST_CONFIG_QUEUE_BUF_SIZE];

static int dump_test_config(void)
{
#define _PARSE(x) #x
#define PARSE(x) _PARSE(x)
#define PRINT_CONFIG(x) printf("\33[0;35m%s=%s\33[0m\r\n", #x, PARSE(x))
    if (strlen(SYSINFO_ARCH)==0 || strlen(SYSINFO_MCU) == 0 || strlen(SYSINFO_DEVICE_NAME)==0) {
        printf("Please set your device info first!\r\n");
        return -1;
    }
    else {
        PRINT_CONFIG(SYSINFO_ARCH);
        PRINT_CONFIG(SYSINFO_MCU);
        PRINT_CONFIG(SYSINFO_DEVICE_NAME);
        PRINT_CONFIG(SYSINFO_KERNEL);
        PRINT_CONFIG(aos_version_get());
        PRINT_CONFIG(SYSINFO_APP_VERSION);
    }

    PRINT_CONFIG(TEST_CONFIG_MM_ENABLED);
#if (TEST_CONFIG_MM_ENABLED > 0)
    PRINT_CONFIG(TEST_CONFIG_MALLOC_MAX_SIZE);
    PRINT_CONFIG(TEST_CONFIG_MALLOC_FREE_TIMES);
#endif

    PRINT_CONFIG(TEST_CONFIG_TASK_ENABLED);
#if (TEST_CONFIG_TASK_ENABLED > 0)
    PRINT_CONFIG(TEST_CONFIG_MAX_TASK_COUNT);
    PRINT_CONFIG(TEST_CONFIG_CREATE_TASK_TIMES);
#endif

    PRINT_CONFIG(TEST_CONFIG_TASK_COMM_ENABLED);
#if (TEST_CONFIG_TASK_COMM_ENABLED > 0)
    PRINT_CONFIG(TEST_CONFIG_SYNC_TIMES);
#endif

    PRINT_CONFIG(TEST_CONFIG_TIMER_ENABLED);

    PRINT_CONFIG(TEST_CONFIG_KV_ENABLED);
#if (TEST_CONFIG_KV_ENABLED > 0)
    PRINT_CONFIG(TEST_CONFIG_KV_TIMES);
#endif

    return 0;
}

#if (TEST_CONFIG_MM_ENABLED > 0)
CASE(test_mm, aos_1_001)
{
    unsigned int size = 512;
    unsigned char *ptr = NULL;

    ptr = aos_malloc(size);
    ASSERT_NOT_NULL(ptr);

    aos_free(ptr);
}

CASE(test_mm, aos_1_002)
{
    unsigned int size = 512;
    unsigned char *ptr = NULL;
    int i = 0;

    ptr = aos_zalloc(size);
    ASSERT_NOT_NULL(ptr);

    for (; i<size; i++) {
        if (*(ptr+i) != 0) {
            aos_free(ptr);
            ASSERT_FAIL();
        }
    }
    aos_free(ptr);
}

CASE(test_mm, aos_1_003)
{
    unsigned int size1 = 512;
    unsigned int size2 = 1024;
    unsigned char *ptr = NULL;
    int i = 0;

    ptr = aos_malloc(size1);
    ASSERT_NOT_NULL(ptr);
    memset(ptr, 0x5A, size1);

    ptr = aos_realloc(ptr, size2);
    ASSERT_NOT_NULL(ptr);
    memset(ptr+size1, 0xA5, size2-size1);

    for(i=0; i<size1; i++) {
        if(*(ptr+i) != 0x5A) {
            aos_free(ptr);
            ASSERT_FAIL();
        }
    }
    for(; i<size2; i++) {
        if(*(ptr+i) != 0xA5) {
            aos_free(ptr);
            ASSERT_FAIL();
        }
    }
    aos_free(ptr);
}

CASE(test_mm, aos_1_004)
{
    unsigned int size1 = 512;
    unsigned int size2 = 256;
    unsigned char *ptr = NULL;
    int i = 0;

    ptr = aos_malloc(size1);
    ASSERT_NOT_NULL(ptr);
    memset(ptr, 0x5A, size1);

    ptr = aos_realloc(ptr, size2);
    ASSERT_NOT_NULL(ptr);

    for(i=0; i<size2; i++) {
        if(*(ptr+i) != 0x5A) {
            aos_free(ptr);
            ASSERT_FAIL();
        }
    }
    aos_free(ptr);
}

CASE(test_mm, aos_1_005)
{
    unsigned int size = 1024;
    unsigned char *ptr = NULL;
    int i = TEST_CONFIG_MALLOC_FREE_TIMES;

    while(i--) {
        ptr = aos_malloc(size);
        ASSERT_NOT_NULL(ptr);
        memset(ptr, 0x5A, size);
        aos_free(ptr);
    }
}
#endif /* TEST_CONFIG_MM_ENABLED */

/* memory manager test suite */
SUITE(test_mm) = {
#if (TEST_CONFIG_MM_ENABLED > 0)
    ADD_CASE(test_mm, aos_1_001),
    ADD_CASE(test_mm, aos_1_002),
    ADD_CASE(test_mm, aos_1_003),
    ADD_CASE(test_mm, aos_1_004),
    ADD_CASE(test_mm, aos_1_005),
#endif
    ADD_CASE_NULL
};

static void task0(void *arg)
{
    aos_sem_signal(&g_sem_taskexit_sync);
    aos_task_exit(0);
}

/* task: print task name */
static void task1(void *arg)
{
    printf("task name: %s\r\n", aos_task_name());
    aos_sem_signal(&g_sem_taskexit_sync);
    aos_task_exit(0);
}

/* task: create task key */
static void task2(void *arg)
{
    int i = 0;
    int ret = -1;
    aos_task_key_t task_key;

    for(; i<10; i++) {
        ret = aos_task_key_create(&task_key);
        ASSERT_EQ(ret, 0);
        printf("%s task key: %d\r\n", aos_task_name(), task_key);
        aos_task_key_delete(task_key);
    }
    aos_sem_signal(&g_sem_taskexit_sync);
    aos_task_exit(0);
}

/* task: set/get task-specific value */
static void task3(void *arg)
{
    int ret = -1;
    aos_task_key_t task_key;
    void *task_value = NULL;

    ret = aos_task_key_create(&task_key);
    ASSERT_EQ(ret, 0);
    printf("%s task key: %d\r\n", aos_task_name(), task_key);

    g_var = 0x5A5A;
    ret = aos_task_setspecific(task_key, &g_var);
    ASSERT_EQ(ret, 0);

    task_value = aos_task_getspecific(task_key);
    ASSERT_NOT_NULL(task_value);
    ASSERT_EQ(*(int*)(task_value), g_var);

    g_var = 0xA5A5;
    task_value = aos_task_getspecific(task_key);
    ASSERT_NOT_NULL(task_value);
    ASSERT_EQ(*(int*)(task_value), g_var);

    aos_task_key_delete(task_key);
    aos_sem_signal(&g_sem_taskexit_sync);
    aos_task_exit(0);
}

/* task: decrease g_var with mutex*/
static void task4(void *arg)
{
    int i = 0;

    printf("task name %s: decrease\r\n", aos_task_name());
    for(i=0; i<TEST_CONFIG_SYNC_TIMES; i++) {
        aos_mutex_lock(&g_mutex, -1);
        g_var--;
        aos_mutex_unlock(&g_mutex);
    }
    aos_sem_signal(&g_sem_taskexit_sync);
    aos_task_exit(0);
}

/* task: increase g_var with mutex*/
static void task5(void *arg)
{
    int i = 0;

    printf("task name %s: increase\r\n", aos_task_name());
    for(i=0; i<TEST_CONFIG_SYNC_TIMES; i++) {
        aos_mutex_lock(&g_mutex, -1);
        g_var++;
        aos_mutex_unlock(&g_mutex);
    }
    aos_sem_signal(&g_sem_taskexit_sync);
    aos_task_exit(0);
}

/* task: decrease g_var with sem */
static void task6(void *arg)
{
    int i = 0;

    printf("task name %s: decrease\r\n", aos_task_name());
    for(i=0; i<TEST_CONFIG_SYNC_TIMES; i++) {
        aos_sem_wait(&g_sem, -1);
        g_var--;
        aos_sem_signal(&g_sem);
    }
    aos_sem_signal(&g_sem_taskexit_sync);
    aos_task_exit(0);
}

/* task: increase g_var with sem */
static void task7(void *arg)
{
    int i = 0;

    printf("task name %s: increase\r\n", aos_task_name());
    for(i=0; i<TEST_CONFIG_SYNC_TIMES; i++) {
        aos_sem_wait(&g_sem, -1);
        g_var++;
        aos_sem_signal(&g_sem);
    }
    aos_sem_signal(&g_sem_taskexit_sync);
    aos_task_exit(0);
}

/* task: g_queue1 -> g_queue2 */
static void task8(void *arg)
{
    int msg = -1;
    unsigned int size = 0;

    while(1) {
        aos_queue_recv(&g_queue1, -1, &msg, &size);
        aos_queue_send(&g_queue2, &msg, size);
        if(msg == TEST_CONFIG_SYNC_TIMES) {
            break;
        }
    }
    printf("%s exit!\r\n", aos_task_name());
    aos_task_exit(0);
}

/* task: g_queue2 -> g_queue3 */
static void task9(void *arg)
{
    int msg = -1;
    unsigned int size = 0;

    while(1) {
        aos_queue_recv(&g_queue2, -1, &msg, &size);
        aos_queue_send(&g_queue3, &msg, size);
        if(msg == TEST_CONFIG_SYNC_TIMES) {
            break;
        }
    }
    printf("%s exit!\r\n", aos_task_name());
    aos_task_exit(0);
}

#if (TEST_CONFIG_TASK_ENABLED > 0)
CASE(test_task, aos_1_006)
{
    unsigned int stack_size = TEST_CONFIG_STACK_SIZE;
    int ret = -1;

    aos_sem_new(&g_sem_taskexit_sync, 0);

    ret = aos_task_new("task1", task1, NULL, stack_size);
    ASSERT_EQ(ret, 0);

    aos_sem_wait(&g_sem_taskexit_sync, -1);
    printf("task1 exit!\r\n");

    aos_sem_free(&g_sem_taskexit_sync);
}

CASE(test_task, aos_1_007)
{
    unsigned int stack_size = TEST_CONFIG_STACK_SIZE;
    aos_task_t task;
    int ret = -1;

    aos_sem_new(&g_sem_taskexit_sync, 0);

    ret = aos_task_new_ext(&task, "task1", task1, NULL, stack_size, 10);
    ASSERT_EQ(ret, 0);

    aos_sem_wait(&g_sem_taskexit_sync, -1);
    printf("task1 exit!\r\n");

    aos_sem_free(&g_sem_taskexit_sync);
}

CASE(test_task, aos_1_008)
{
    unsigned int stack_size = TEST_CONFIG_STACK_SIZE;
    int ret = -1;

    aos_sem_new(&g_sem_taskexit_sync, 0);

    ret = aos_task_new("task2", task2, NULL, stack_size);
    ASSERT_EQ(ret, 0);

    aos_sem_wait(&g_sem_taskexit_sync, -1);
    printf("task2 exit!\r\n");

    aos_sem_free(&g_sem_taskexit_sync);
}

CASE(test_task, aos_1_009)
{
    unsigned int stack_size = TEST_CONFIG_STACK_SIZE;
    int ret = -1;

    aos_sem_new(&g_sem_taskexit_sync, 0);

    ret = aos_task_new("task2", task2, NULL, stack_size);
    ASSERT_EQ(ret, 0);

    aos_sem_wait(&g_sem_taskexit_sync, -1);
    printf("task2 exit!\r\n");

    aos_sem_free(&g_sem_taskexit_sync);
}

CASE(test_task, aos_1_010)
{
    unsigned int stack_size = TEST_CONFIG_STACK_SIZE;
    int ret = -1;

    aos_sem_new(&g_sem_taskexit_sync, 0);

    ret = aos_task_new("task3", task3, NULL, stack_size);
    ASSERT_EQ(ret, 0);

    aos_sem_wait(&g_sem_taskexit_sync, -1);
    printf("task3 exit!\r\n");

    aos_sem_free(&g_sem_taskexit_sync);
}

CASE(test_task, aos_1_011)
{
    unsigned int stack_size = TEST_CONFIG_STACK_SIZE;
    char task_name[10] = {0};
    int ret = -1;
    int i = 0;

    aos_sem_new(&g_sem_taskexit_sync, 0);

    for(i=0; i<TEST_CONFIG_MAX_TASK_COUNT; i++) {
        memset(task_name, 0, sizeof(task_name));
        snprintf(task_name, 10, "task%02d", i);
        ret = aos_task_new(task_name, task1, NULL, stack_size);
        ASSERT_EQ(ret, 0);
        aos_msleep(1);
    }
    for(i=0; i<TEST_CONFIG_MAX_TASK_COUNT; i++) {
        aos_sem_wait(&g_sem_taskexit_sync, -1);
    }
    printf("%d tasks exit!\r\n", TEST_CONFIG_MAX_TASK_COUNT);

    aos_sem_free(&g_sem_taskexit_sync);
}

CASE(test_task, aos_1_012)
{
    unsigned int stack_size = TEST_CONFIG_STACK_SIZE;
    char task_name[10] = {0};
    int ret = -1;
    int i = 0;


    for(i=0; i<TEST_CONFIG_CREATE_TASK_TIMES; i++) {
        memset(task_name, 0, sizeof(task_name));
        snprintf(task_name, 10, "task%02d", i);
        if(i % 500 == 0) {
            printf("create task: %d/%d\r\n", i, TEST_CONFIG_CREATE_TASK_TIMES);
        }

        aos_sem_new(&g_sem_taskexit_sync, 0);
        ret = aos_task_new(task_name, task0, NULL, stack_size);
        ASSERT_EQ(ret, 0);

        aos_sem_wait(&g_sem_taskexit_sync, -1);
        aos_sem_free(&g_sem_taskexit_sync);
        aos_msleep(1);
    }
}
#endif /* TEST_CONFIG_TASK_ENABLED */

/* task test suite */
SUITE(test_task) = {
#if (TEST_CONFIG_TASK_ENABLED > 0)
    ADD_CASE(test_task, aos_1_006),
    ADD_CASE(test_task, aos_1_007),
    ADD_CASE(test_task, aos_1_008),
    ADD_CASE(test_task, aos_1_009),
    ADD_CASE(test_task, aos_1_010),
    ADD_CASE(test_task, aos_1_011),
    ADD_CASE(test_task, aos_1_012),
#endif
    ADD_CASE_NULL
};

#if (TEST_CONFIG_TASK_COMM_ENABLED > 0)
CASE(test_task_comm, aos_1_013)
{
    int ret = -1;

    ret = aos_mutex_new(&g_mutex);
    ASSERT_EQ(ret, 0);

    ret = aos_mutex_is_valid(&g_mutex);
    ASSERT_EQ(ret, 1);

    ret = aos_mutex_lock(&g_mutex, 1000);
    ASSERT_EQ(ret, 0);

    ret = aos_mutex_lock(&g_mutex, 1000);
    ASSERT_EQ(ret, 0);

    ret = aos_mutex_unlock(&g_mutex);
    ASSERT_EQ(ret, 0);

    aos_mutex_free(&g_mutex);
}

CASE(test_task_comm, aos_1_014)
{
    unsigned int stack_size = TEST_CONFIG_STACK_SIZE;
    char task_name[10] = {0};
    unsigned int task_count = 4;
    int ret = -1;
    int i = 0;

    ASSERT_TRUE(task_count%2 == 0);

    aos_sem_new(&g_sem_taskexit_sync, 0);
    aos_mutex_new(&g_mutex);
    g_var = 0;

    for(i=0; i<4; i++) {
        snprintf(task_name, 10, "task%d", i+1);
        if(i < 2) {
            ret = aos_task_new(task_name, task4, NULL, stack_size);
            ASSERT_EQ(ret, 0);
        }
        else {
            ret = aos_task_new(task_name, task5, NULL, stack_size);
            ASSERT_EQ(ret, 0);
        }
        aos_msleep(10);
    }
    for(i=0; i<4; i++) {
        aos_sem_wait(&g_sem_taskexit_sync, -1);
    }
    printf("g_var = %d\r\n", g_var);
    ASSERT_EQ(g_var, 0);

    aos_sem_free(&g_sem_taskexit_sync);
    aos_mutex_free(&g_mutex);
}

CASE(test_task_comm, aos_1_015)
{
    int ret = -1;

    ret = aos_sem_new(&g_sem, 0);
    ASSERT_EQ(ret, 0);

    ret = aos_sem_is_valid(&g_sem);
    ASSERT_EQ(ret, 1);

    ret = aos_sem_wait(&g_sem, 100);
    ASSERT_NE(ret, 0);

    aos_sem_signal(&g_sem);

    ret = aos_sem_wait(&g_sem, 100);
    ASSERT_EQ(ret, 0);

    aos_sem_free(&g_sem);
}

CASE(test_task_comm, aos_1_016)
{
    unsigned int stack_size = TEST_CONFIG_STACK_SIZE;
    char task_name[10] = {0};
    unsigned int task_count = 4;
    int ret = -1;
    int i = 0;

    ASSERT_TRUE(task_count%2 == 0);

    aos_sem_new(&g_sem_taskexit_sync, 0);
    aos_sem_new(&g_sem, 1);
    g_var = 0;

    for(i=0; i<task_count; i++) {
        snprintf(task_name, 10, "task%d", i+1);
        if(i < 2) {
            ret = aos_task_new(task_name, task6, NULL, stack_size);
            ASSERT_EQ(ret, 0);
        }
        else {
            ret = aos_task_new(task_name, task7, NULL, stack_size);
            ASSERT_EQ(ret, 0);
        }
        aos_msleep(10);
    }
    for(i=0; i<task_count; i++) {
        aos_sem_wait(&g_sem_taskexit_sync, -1);
    }
    printf("g_var = %d\r\n", g_var);
    ASSERT_EQ(g_var, 0);

    aos_sem_free(&g_sem_taskexit_sync);
    aos_sem_free(&g_sem);
}

CASE(test_task_comm, aos_1_017)
{
    int ret = -1;
    char msg_send[16] = {0};
    char msg_recv[16] = {0};
    unsigned int size_send = 16;
    unsigned int size_recv = 16;

    ret = aos_queue_new(&g_queue, queue_buf, TEST_CONFIG_QUEUE_BUF_SIZE, TEST_CONFIG_QUEUE_BUF_SIZE);
    ASSERT_EQ(ret, 0);

    ret = aos_queue_is_valid(&g_queue);
    ASSERT_EQ(ret, 1);

    ret = aos_queue_recv(&g_queue, 100, msg_recv, &size_recv);
    ASSERT_NE(ret, 0);

    snprintf(msg_send, size_send, "hello,queue!");
    size_send = strlen(msg_send);
    ret = aos_queue_send(&g_queue, msg_send, size_send);
    ASSERT_EQ(ret, 0);

    memset(msg_recv, 0, size_recv);
    ret = aos_queue_recv(&g_queue, 100, msg_recv, &size_recv);
    ASSERT_EQ(ret, 0);
    ASSERT_STR_EQ(msg_recv, msg_send);
    ASSERT_EQ(size_send, size_recv);

    aos_queue_free(&g_queue);
}

CASE(test_task_comm, aos_1_018)
{
    int          ret = -1;
    unsigned int stack_size = TEST_CONFIG_STACK_SIZE;
    unsigned int msg_send = 0;
    unsigned int msg_recv = 0;
    unsigned int size_send = sizeof(msg_send);
    unsigned int size_recv = 0;
    int i = 0;

    ret = aos_queue_new(&g_queue1, queue1_buf, TEST_CONFIG_QUEUE_BUF_SIZE, TEST_CONFIG_QUEUE_BUF_SIZE);
    ASSERT_EQ(ret, 0);
    ret = aos_queue_new(&g_queue2, queue2_buf, TEST_CONFIG_QUEUE_BUF_SIZE, TEST_CONFIG_QUEUE_BUF_SIZE);
    ASSERT_EQ(ret, 0);
    ret = aos_queue_new(&g_queue3, queue3_buf, TEST_CONFIG_QUEUE_BUF_SIZE, TEST_CONFIG_QUEUE_BUF_SIZE);
    ASSERT_EQ(ret, 0);

    ret = aos_task_new("task1", task8, NULL, stack_size);
    ASSERT_EQ(ret, 0);
    ret = aos_task_new("task2", task9, NULL, stack_size);
    ASSERT_EQ(ret, 0);

    for(i=1; i<=TEST_CONFIG_SYNC_TIMES; i++) {
        msg_send = i;
        ret = aos_queue_send(&g_queue1, &msg_send, size_send);
        ASSERT_EQ(ret, 0);

        ret = aos_queue_recv(&g_queue3, -1, &msg_recv, &size_recv);
        ASSERT_EQ(ret, 0);
        ASSERT_EQ(msg_send, msg_recv);
        ASSERT_EQ(size_send, size_recv);
        if(i%(TEST_CONFIG_SYNC_TIMES/10) == 0) {
            printf("%d/%d\r\n", i, TEST_CONFIG_SYNC_TIMES);
        }
    }
    ASSERT_EQ(msg_recv, TEST_CONFIG_SYNC_TIMES);
    aos_queue_free(&g_queue1);
    aos_queue_free(&g_queue2);
    aos_queue_free(&g_queue3);
}
#endif /* TEST_CONFIG_TASK_COMM_ENABLED */

/* task commication */
SUITE(test_task_comm) = {
#if (TEST_CONFIG_TASK_COMM_ENABLED > 0)
    ADD_CASE(test_task_comm, aos_1_013),
    ADD_CASE(test_task_comm, aos_1_014),
    ADD_CASE(test_task_comm, aos_1_015),
    ADD_CASE(test_task_comm, aos_1_016),
    ADD_CASE(test_task_comm, aos_1_017),
    ADD_CASE(test_task_comm, aos_1_018),
#endif
    ADD_CASE_NULL
};

#if (TEST_CONFIG_TIMER_ENABLED > 0)
static void timer_handler(void *arg1, void* arg2)
{
    printf("timer handler\r\n");
    if(++g_var == 10) {
        aos_sem_signal(&g_sem);
    }
}

CASE(test_timer, aos_1_019)
{
    int ret = -1;

    g_var = 0;
    ret = aos_sem_new(&g_sem, 0);
    ASSERT_EQ(ret, 0);

    ret = aos_timer_new(&g_timer, timer_handler, NULL, 200, 1);
    ASSERT_EQ(ret, 0);

    aos_sem_wait(&g_sem, -1);
    aos_sem_free(&g_sem);

    aos_timer_stop(&g_timer);
    aos_timer_free(&g_timer);
}

CASE(test_timer, aos_1_020)
{
    int ret = -1;

    g_var = 0;
    ret = aos_sem_new(&g_sem, 0);
    ASSERT_EQ(ret, 0);

    ret = aos_timer_new(&g_timer, timer_handler, NULL, 200, 1);
    ASSERT_EQ(ret, 0);
    aos_msleep(1000);

    aos_timer_stop(&g_timer);
    printf("timer stopped!\r\n");
    aos_msleep(1000);
    aos_timer_change(&g_timer, 1000);
    printf("timer changed!\r\n");
    aos_timer_start(&g_timer);
    printf("timer started!\r\n");

    aos_sem_wait(&g_sem, -1);
    aos_sem_free(&g_sem);

    aos_timer_stop(&g_timer);
    aos_timer_free(&g_timer);
}
#endif /* TEST_CONFIG_TIMER_ENABLED */

/* timer test suite */
SUITE(test_timer) = {
#if (TEST_CONFIG_TIMER_ENABLED > 0)
    ADD_CASE(test_timer, aos_1_019),
    ADD_CASE(test_timer, aos_1_020),
#endif
    ADD_CASE_NULL
};

#if (TEST_CONFIG_KV_ENABLED > 0)
#include <aos/kv.h>
CASE(test_kv, aos_2_001)
{
    char *key = "test_kv_key";
    char *set_value = "test_kv_value";
    int  set_len = strlen(set_value);
    char get_value[32] = {0};
    int  get_len = 32;
    int  ret = -1;

    ret = aos_kv_set(key, set_value, set_len, 1);
    ASSERT_EQ(ret, 0);

    ret = aos_kv_get(key, get_value, &get_len);
    ASSERT_EQ(ret, 0);
    ASSERT_EQ(get_len, set_len);
    ASSERT_STR_EQ(get_value, set_value);
}

CASE(test_kv, aos_2_002)
{
    char *key = "test_kv_key";
    char *set_value = "test_kv_value";
    int  set_len = strlen(set_value);
    char get_value[32] = {0};
    int  get_len = 32;
    int  ret = -1;

    aos_kv_del(key);

    ret = aos_kv_set(key, set_value, set_len, 1);
    ASSERT_EQ(ret, 0);

    ret = aos_kv_del(key);
    ASSERT_EQ(ret, 0);

    ret = aos_kv_get(key, get_value, &get_len);
    ASSERT_NE(ret, 0);
}

CASE(test_kv, aos_2_003)
{
    char key[64] = {0};
    char set_value[64] = {0};
    int  set_len = 0;
    char get_value[64] = {0};
    int  get_len = 0;
    int  ret = -1;
    int  i = 0;


    while(i++ < TEST_CONFIG_KV_TIMES) {
        memset(key, 0, sizeof(key));
        memset(set_value, 0, sizeof(set_value));
        memset(get_value, 0, sizeof(get_value));

        sprintf(key, "this_is_key_%d", i);
        sprintf(set_value, "this_is_value_%d", i);
        set_len = strlen(set_value);
        ret = aos_kv_set(key, set_value, set_len, 1);
        ASSERT_EQ(ret, 0);

        get_len = sizeof(get_value);
        ret = aos_kv_get(key, get_value, &get_len);
        ASSERT_EQ(ret, 0);
        ASSERT_EQ(get_len, set_len);
        ASSERT_TRUE(0 == memcmp(get_value, set_value, set_len));

        ret = aos_kv_del(key);
        ASSERT_EQ(ret, 0);

        if(i%100 == 0) {
            printf("kv test: %d/%d\r\n", i, TEST_CONFIG_KV_TIMES);
        }
        aos_msleep(1);
    }
}

#if (KV_CONFIG_SECURE_SUPPORT > 0) && (KV_CONFIG_SECURE_CRYPT_IMPL == 1)

#include "kv_adapt.h"

uint8_t aes_key[32] = {
        0x86, 0xf6, 0xd2, 0xbe, 0x45, 0xb5, 0xab, 0x9c,
        0xc7, 0xd5, 0x96, 0xf7, 0xaf, 0x45, 0xfa, 0xf7,
        0xbe, 0x6a, 0x5d, 0xb0, 0x04, 0xc4, 0xde, 0xb5,
        0xf5, 0x0c, 0x4f, 0xc3, 0x71, 0x19, 0x3e, 0xe8
    };

uint8_t aes_iv[16]  = {
        0xef, 0x80, 0x18, 0xdc, 0xa3, 0x72, 0x72, 0x31,
        0x99, 0x2e, 0x3a, 0xba, 0x60, 0xf5, 0x0b, 0xd4
    };

uint8_t* kv_secure_get_key(uint32_t len)
{
    if ((len <= 0) || (len > sizeof(aes_key))) {
        return NULL;
    }

    return aes_key;
}

uint8_t* kv_secure_get_iv(uint32_t len)
{
    if ((len <= 0) || (len > sizeof(aes_iv))) {
        return NULL;
    }

    return aes_iv;
}

#endif

#endif /* TEST_CONFIG_KV_ENABLED */

/* kv test suite */
SUITE(test_kv) = {
#if (TEST_CONFIG_KV_ENABLED > 0)
    ADD_CASE(test_kv, aos_2_001),
    ADD_CASE(test_kv, aos_2_002),
    ADD_CASE(test_kv, aos_2_003),
#endif
    ADD_CASE_NULL
};

void test_certificate(void)
{
    if (0 == dump_test_config()) {
        printf("test start!\r\n");
        ADD_SUITE(test_mm);
        ADD_SUITE(test_task);
        ADD_SUITE(test_task_comm);
        ADD_SUITE(test_timer);
        ADD_SUITE(test_kv);
        cut_main(0, NULL);
        printf("test finished!\r\n");
    }
    else {
        printf("test error!\r\n");
    }
}
AOS_TESTCASE(test_certificate);

