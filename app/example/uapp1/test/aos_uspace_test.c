/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include "u_api.h"
#include <aos/kernel.h>
#include "cut.h"

#ifndef SYSINFO_ARCH
#define SYSINFO_ARCH        "11"
#endif
#ifndef SYSINFO_MCU
#define SYSINFO_MCU         "11"
#endif
#ifndef SYSINFO_DEVICE_NAME
#define SYSINFO_DEVICE_NAME "11"
#endif
#ifndef SYSINFO_APP_VERSION
#define SYSINFO_APP_VERSION "11"
#endif
#define SYSINFO_KERNEL      "AOS"

/* dynamic memory alloc test */
#define TEST_CONFIG_MM_ENABLED                  (1)
#if (TEST_CONFIG_MM_ENABLED > 0)
#define TEST_CONFIG_MALLOC_MAX_SIZE             (1024)
#define TEST_CONFIG_MALLOC_FREE_TIMES           (100)
#endif

/* task test */
#define TEST_CONFIG_TASK_ENABLED                (1)
#if (TEST_CONFIG_TASK_ENABLED > 0)
#ifndef TEST_CONFIG_STACK_SIZE
#define TEST_CONFIG_STACK_SIZE                  (512)
#endif
#define TEST_CONFIG_MAX_TASK_COUNT              (10)
#define TEST_CONFIG_CREATE_TASK_TIMES           (10)
#endif

/* task communication test */
#define TEST_CONFIG_TASK_COMM_ENABLED           (1)
#if (TEST_CONFIG_TASK_COMM_ENABLED > 0)
#ifndef TEST_CONFIG_STACK_SIZE
#define TEST_CONFIG_STACK_SIZE                  (512)
#endif
#define TEST_CONFIG_SYNC_TIMES                  (1)
#define TEST_CONFIG_QUEUE_BUF_SIZE              (32)
#endif

/* timer test */
#if (RHINO_CONFIG_UTIMER_SUPPORT > 0)
#define TEST_CONFIG_UTIMER_ENABLED               (1)
#else
#define TEST_CONFIG_UTIMER_ENABLED               (0)
#endif

#define GLOBAL_VAL                              (0xA5A5)

static unsigned int g_var = 0;
static aos_sem_t    g_sem_taskexit_sync;
static aos_mutex_t  g_mutex;
static aos_sem_t    g_sem;
static aos_queue_t  g_queue;

#if (TEST_CONFIG_UTIMER_ENABLED > 0)
static aos_timer_t  g_timer;
#endif

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
        PRINT_CONFIG(SYSINFO_KERNEL_VERSION);
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

    PRINT_CONFIG(TEST_CONFIG_UTIMER_ENABLED);

    return 0;
}

#if (TEST_CONFIG_MM_ENABLED > 0)
CASE(test_mm, aos_1_001)
{
    unsigned int size = 512;
    unsigned char *ptr = NULL;

    ptr = malloc(size);
    ASSERT_NOT_NULL(ptr);

    free(ptr);
}

CASE(test_mm, aos_1_002)
{
    unsigned int size = 512;
    unsigned char *ptr = NULL;
    int i = 0;

    ptr = malloc(size);
    memset((void*)ptr, 0, size);
    ASSERT_NOT_NULL(ptr);

    for (; i<size; i++) {
        if (*(ptr+i) != 0) {
            free(ptr);
            ASSERT_FAIL();
        }
    }
    free(ptr);
}

CASE(test_mm, aos_1_003)
{
    unsigned int size1 = 512;
    unsigned int size2 = 1024;
    unsigned char *ptr = NULL;
    int i = 0;

    ptr = malloc(size1);
    ASSERT_NOT_NULL(ptr);
    memset(ptr, 0x5A, size1);

    ptr = realloc(ptr, size2);
    ASSERT_NOT_NULL(ptr);
    memset(ptr+size1, 0xA5, size2-size1);

    for(i=0; i<size1; i++) {
        if(*(ptr+i) != 0x5A) {
            free(ptr);
            ASSERT_FAIL();
        }
    }
    for(; i<size2; i++) {
        if(*(ptr+i) != 0xA5) {
            free(ptr);
            ASSERT_FAIL();
        }
    }
    free(ptr);
}

CASE(test_mm, aos_1_004)
{
    unsigned int size1 = 512;
    unsigned int size2 = 256;
    unsigned char *ptr = NULL;
    int i = 0;

    ptr = malloc(size1);
    ASSERT_NOT_NULL(ptr);
    memset(ptr, 0x5A, size1);

    ptr = realloc(ptr, size2);
    ASSERT_NOT_NULL(ptr);

    for(i=0; i<size2; i++) {
        if(*(ptr+i) != 0x5A) {
            free(ptr);
            ASSERT_FAIL();
        }
    }
    free(ptr);
}

CASE(test_mm, aos_1_005)
{
    unsigned int size = 1024;
    unsigned char *ptr = NULL;
    int i = TEST_CONFIG_MALLOC_FREE_TIMES;

    while(i--) {
        ptr = malloc(size);
        ASSERT_NOT_NULL(ptr);
        memset(ptr, 0x5A, size);
        free(ptr);
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
        aos_task_key_delete(task_key);
    }
    aos_sem_signal(&g_sem_taskexit_sync);
    aos_task_exit(0);
}

/* task: decrease g_var with mutex*/
static void task4(void *arg)
{
    int i = 0;

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

    printf("task8 run\n");
    while(1) {
        aos_queue_recv(&g_queue1, -1, &msg, &size);
        aos_queue_send(&g_queue2, &msg, size);
        if(msg == TEST_CONFIG_SYNC_TIMES) {
            break;
        }
    }
    printf("%d\n", __LINE__);
    aos_task_exit(0);
}

/* task: g_queue2 -> g_queue3 */
static void task9(void *arg)
{
    int msg = -1;
    unsigned int size = 0;

    printf("task9 run\n");
    while(1) {
        aos_queue_recv(&g_queue2, -1, &msg, &size);
        aos_queue_send(&g_queue3, &msg, size);
        if(msg == TEST_CONFIG_SYNC_TIMES) {
            break;
        }
    }
    printf("%d\n", __LINE__);
    aos_task_exit(0);
}

#if (TEST_CONFIG_TASK_ENABLED > 0)
#include "k_api.h"
CASE(test_task, aos_1_006)
{
    unsigned int stack_size = 1024;
    int ret = -1;

    ret = aos_sem_new(&g_sem_taskexit_sync, 0);
    ASSERT_EQ(ret, 0);

    ret = aos_task_new("task1", task1, NULL, stack_size);
    ASSERT_EQ(ret, 0);

    ret = aos_sem_wait(&g_sem_taskexit_sync, AOS_WAIT_FOREVER);

    aos_sem_free(&g_sem_taskexit_sync);
}

CASE(test_task, aos_1_007)
{
    unsigned int stack_size = 1024;
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
    unsigned int stack_size = 1024;
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
    unsigned int stack_size = 1024;
    int ret = -1;

    aos_sem_new(&g_sem_taskexit_sync, 0);

    ret = aos_task_new("task2", task2, NULL, stack_size);
    ASSERT_EQ(ret, 0);

    aos_sem_wait(&g_sem_taskexit_sync, -1);
    printf("task2 exit!\r\n");

    aos_sem_free(&g_sem_taskexit_sync);
}

CASE(test_task, aos_1_011)
{
    unsigned int stack_size = 1024;
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
    unsigned int stack_size = 1024;
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
    unsigned int stack_size = 1024;
    char task_name[10] = {0};
    unsigned int task_count = 4;
    int ret = -1;
    int i = 0;

    ASSERT_TRUE(task_count%2 == 0);

    aos_sem_new(&g_sem_taskexit_sync, 0);
    aos_mutex_new(&g_mutex);
    g_var = 0;

    for(i=0; i<task_count; i++) {
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
    for(i=0; i<task_count; i++) {
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
    unsigned int stack_size = 1024;
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
    unsigned int stack_size = 512;
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

    ret = aos_task_new("task8", task8, NULL, stack_size);
    ASSERT_EQ(ret, 0);
    ret = aos_task_new("task9", task9, NULL, stack_size);
    ASSERT_EQ(ret, 0);

    for(i=1; i<=TEST_CONFIG_SYNC_TIMES; i++) {
        msg_send = i;
        ret = aos_queue_send(&g_queue1, &msg_send, size_send);
        ASSERT_EQ(ret, 0);

        ret = aos_queue_recv(&g_queue3, -1, &msg_recv, &size_recv);
        printf("q3 recv:0x%x, size:%d\n", msg_recv, size_recv);

        ASSERT_EQ(ret, 0);
        ASSERT_EQ(msg_send, msg_recv);
        ASSERT_EQ(size_send, size_recv);
        /*
        if(i%(TEST_CONFIG_SYNC_TIMES/10) == 0) {
            printf("%d/%d\r\n", i, TEST_CONFIG_SYNC_TIMES);
        }*/
    }
    //ASSERT_EQ(msg_recv, TEST_CONFIG_SYNC_TIMES);
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
    ADD_CASE(test_task_comm, aos_1_017), /*queue*/
    ADD_CASE(test_task_comm, aos_1_018),
#endif
    ADD_CASE_NULL
};

#if (TEST_CONFIG_UTIMER_ENABLED > 0)
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
#endif /* TEST_CONFIG_UTIMER_ENABLED */

/* timer test suite */
SUITE(test_timer) = {
#if (TEST_CONFIG_UTIMER_ENABLED > 0)
    ADD_CASE(test_timer, aos_1_019),
    ADD_CASE(test_timer, aos_1_020),
#endif
    ADD_CASE_NULL
};


/* task11: decrease g_var with mutex timeout*/
static void task11(void *arg)
{
    int ret = -1;

    ret = aos_mutex_lock(&g_mutex, 100);

    if (RHINO_BLK_TIMEOUT == ret) {
        g_var--;
    } else {
        aos_mutex_unlock(&g_mutex);
    }

    aos_task_exit(0);
}

/* task12: decrease g_var with sem timeout*/
static void task12(void *arg)
{
    int ret = -1;

    ret = aos_sem_wait(&g_sem, 100);

    if (RHINO_BLK_TIMEOUT == ret) {
        g_var++;
    } else {
        aos_sem_signal(&g_sem);
    }

    aos_task_exit(0);
}

/* task13: decrease g_var with queue timeout*/
static void task13(void *arg)
{
    int ret = -1;
    int msg = -1;
    unsigned int size = 0;

    ret = aos_queue_recv(&g_queue1, 100, &msg, &size);

    if (RHINO_BLK_TIMEOUT == ret) {
        g_var--;
    }

    aos_task_exit(0);
}


CASE(test_timeout, aos_1_021)
{
    unsigned int stack_size = 1024;
    int ret = -1;

    ret = aos_mutex_new(&g_mutex);
    ASSERT_EQ(ret, 0);

    g_var = GLOBAL_VAL;

    ret = aos_mutex_lock(&g_mutex, -1);
    ASSERT_EQ(ret, 0);

    ret = aos_task_new("task11", task11, NULL, stack_size);
    ASSERT_EQ(ret, 0);

    aos_msleep(200);

    printf("g_var = 0x%x\r\n", g_var);
    ASSERT_EQ(g_var, GLOBAL_VAL-1);

    aos_mutex_free(&g_mutex);
}

CASE(test_timeout, aos_1_022)
{
    unsigned int stack_size = 1024;
    int ret = -1;

    ret = aos_sem_new(&g_sem, 0);
    ASSERT_EQ(ret, 0);

    g_var = GLOBAL_VAL;

    ret = aos_task_new("task12", task12, NULL, stack_size);
    ASSERT_EQ(ret, 0);

    aos_msleep(200);

    printf("g_var = 0x%x\r\n", g_var);
    ASSERT_EQ(g_var, GLOBAL_VAL+1);

    aos_sem_free(&g_sem);
}

CASE(test_timeout, aos_1_023)
{
    int          ret = -1;
    unsigned int stack_size = 1024;

    g_var = GLOBAL_VAL;

    ret = aos_queue_new(&g_queue1, queue1_buf, TEST_CONFIG_QUEUE_BUF_SIZE, TEST_CONFIG_QUEUE_BUF_SIZE);
    ASSERT_EQ(ret, 0);

    ret = aos_task_new("task13", task13, NULL, stack_size);
    ASSERT_EQ(ret, 0);

    aos_msleep(200);

    printf("g_var = 0x%x\r\n", g_var);
    ASSERT_EQ(g_var, GLOBAL_VAL-1);

    aos_queue_free(&g_queue1);
}


/* timeout test suite */
SUITE(test_timeout) = {
    ADD_CASE(test_timeout, aos_1_021),
    ADD_CASE(test_timeout, aos_1_022),
    ADD_CASE(test_timeout, aos_1_023),
    ADD_CASE_NULL
};

static void task24(void *arg)
{
    g_var++;
    printf("g_var : %d\n", g_var);

    while (1) {
        aos_msleep(1);
    }
}

/*test process creat and exit*/
CASE(test_process, aos_1_024)
{
    unsigned int stack_size = 512;
    char task_name[10] = {0};
    int ret = -1;
    int i = 0;

    g_var = 0;

    memset(task_name, 0, sizeof(task_name));
    snprintf(task_name, 10, "task%02d", i);
    ret = aos_task_new("task00", task24, NULL, stack_size);
    ret = aos_task_new("task01", task24, NULL, stack_size);
    ASSERT_EQ(ret, 0);

    aos_msleep(1);
}

/* process test suite */
SUITE(test_process) = {
    ADD_CASE(test_process, aos_1_024),
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
        ADD_SUITE(test_timeout);
        //ADD_SUITE(test_process);/*default close*/
        cut_main(0, NULL);
        printf("test finished!\r\n");
    }
    else {
        printf("test error!\r\n");
    }
}
AOS_TESTCASE(test_certificate);

