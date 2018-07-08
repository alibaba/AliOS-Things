/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>
#include "cutest/cut.h"

#ifndef SYSINFO_ARCH
#define SYSINFO_ARCH        ""
#endif
#ifndef SYSINFO_MCU
#define SYSINFO_MCU         ""
#endif
#ifndef SYSINFO_DEVICE_NAME
#define SYSINFO_DEVICE_NAME ""
#endif
#ifndef SYSINFO_APP_VERSION
#define SYSINFO_APP_VERSION ""
#endif
#define SYSINFO_KERNEL      "RHINO"

/* dynamic memory alloc test */
#if (RHINO_CONFIG_MM_TLF > 0)
#define TEST_CONFIG_MM_ENABLED                  (1)
#if (TEST_CONFIG_MM_ENABLED > 0)
#define TEST_CONFIG_MALLOC_MAX_SIZE             (1024)
#define TEST_CONFIG_MALLOC_FREE_TIMES           (100000)
#endif
#endif

/* task test */
#define TEST_CONFIG_TASK_ENABLED                (1)
#if (TEST_CONFIG_TASK_ENABLED > 0)
#ifndef TEST_CONFIG_STACK_SIZE
#define TEST_CONFIG_STACK_SIZE                  (256)
#endif
#define TEST_CONFIG_MAX_TASK_COUNT              (10)
#define TEST_CONFIG_CREATE_TASK_TIMES           (10000)
#endif

/* task communication test */
#define TEST_CONFIG_TASK_COMM_ENABLED           (1)
#if (TEST_CONFIG_TASK_COMM_ENABLED > 0)
#ifndef TEST_CONFIG_STACK_SIZE
#define TEST_CONFIG_STACK_SIZE                  (256)
#endif
#define TEST_CONFIG_SYNC_TIMES                  (100000)
#define TEST_CONFIG_QUEUE_BUF_SIZE              (32)
#endif

/* timer test */
#define TEST_CONFIG_TIMER_ENABLED               (1)

/* kv test */
#define TEST_CONFIG_KV_ENABLED                  (0)
#if (TEST_CONFIG_KV_ENABLED > 0)
#define TEST_CONFIG_KV_TIMES                    (10000)
#endif

/* yloop test */
#define TEST_CONFIG_YLOOP_ENABLED               (0)
#if (TEST_CONFIG_YLOOP_ENABLED)
#define TEST_CONFIG_YLOOP_EVENT_COUNT           (1000)
#define TEST_CONFIG_YLOOP_LOOP_COUNT            (5)
#endif

static int g_var0 = 0;
static int g_var1 = 0;
static cpu_stack_t stack_buf[TEST_CONFIG_MAX_TASK_COUNT][TEST_CONFIG_STACK_SIZE];
static ktask_t g_task[TEST_CONFIG_MAX_TASK_COUNT];

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

    PRINT_CONFIG(TEST_CONFIG_TIMER_ENABLED);

    PRINT_CONFIG(TEST_CONFIG_KV_ENABLED);
#if (TEST_CONFIG_KV_ENABLED > 0)
    PRINT_CONFIG(TEST_CONFIG_KV_TIMES);
#endif

    PRINT_CONFIG(TEST_CONFIG_YLOOP_ENABLED);
#if (TEST_CONFIG_YLOOP_ENABLED)
    PRINT_CONFIG(TEST_CONFIG_YLOOP_EVENT_COUNT);
    PRINT_CONFIG(TEST_CONFIG_YLOOP_LOOP_COUNT);
#endif
    return 0;
}

#if (TEST_CONFIG_MM_ENABLED > 0)
#if (RHINO_CONFIG_MM_TLF > 0)
CASE(test_mm, aos_1_001)
{
    unsigned int size = 512;
    unsigned char *ptr = NULL;

    ptr = krhino_mm_alloc(size);
    ASSERT_NOT_NULL(ptr);

    krhino_mm_free(ptr);
}

CASE(test_mm, aos_1_002)
{
    /* case is not support by rhino */
}

CASE(test_mm, aos_1_003)
{
    unsigned int size1 = 512;
    unsigned int size2 = 1024;
    unsigned char *ptr = NULL;
    int i = 0;

    ptr = krhino_mm_alloc(size1);
    ASSERT_NOT_NULL(ptr);
    memset(ptr, 0x5A, size1);

    krhino_mm_realloc(ptr, size2);
    ASSERT_NOT_NULL(ptr);
    memset(ptr+size1, 0xA5, size2-size1);

    for(i=0; i<size1; i++) {
        if(*(ptr+i) != 0x5A) {
            krhino_mm_free(ptr);
            ASSERT_FAIL();
        }
    }
    for(; i<size2; i++) {
        if(*(ptr+i) != 0xA5) {
            krhino_mm_free(ptr);
            ASSERT_FAIL();
        }
    }
    krhino_mm_free(ptr);
}

CASE(test_mm, aos_1_004)
{
    unsigned int size1 = 512;
    unsigned int size2 = 256;
    unsigned char *ptr = NULL;
    int i = 0;

    ptr = krhino_mm_alloc(size1);
    ASSERT_NOT_NULL(ptr);
    memset(ptr, 0x5A, size1);

    krhino_mm_realloc(ptr, size2);
    ASSERT_NOT_NULL(ptr);

    for(i=0; i<size2; i++) {
        if(*(ptr+i) != 0x5A) {
            krhino_mm_free(ptr);
            ASSERT_FAIL();
        }
    }
    krhino_mm_free(ptr);
}

CASE(test_mm, aos_1_005)
{
    char *ptr = NULL;
    int i = 0;
    int j = 0;

    for (i = 1; i <= TEST_CONFIG_MALLOC_FREE_TIMES; i++) {
        ptr = krhino_mm_alloc(TEST_CONFIG_MALLOC_MAX_SIZE);
        ASSERT_NOT_NULL(ptr);

        for (j = 0; j < TEST_CONFIG_MALLOC_MAX_SIZE; j++) {
            *(ptr + j) = j;
        }
        krhino_mm_free(ptr);
        if(i % (TEST_CONFIG_MALLOC_FREE_TIMES/10) == 0) {
            printf("alloc-free: %d/%d\r\n", i, TEST_CONFIG_MALLOC_FREE_TIMES);
        }
    }
}
#endif /* RHINO_CONFIG_MM_TLF > 0 */
#endif /* TEST_CONFIG_MM_ENABLED > 0 */

SUITE(test_mm) = {
#if (TEST_CONFIG_MM_ENABLED > 0)
#if (RHINO_CONFIG_MM_TLF > 0)
    ADD_CASE(test_mm, aos_1_001),
    ADD_CASE(test_mm, aos_1_002),
    ADD_CASE(test_mm, aos_1_003),
    ADD_CASE(test_mm, aos_1_004),
    ADD_CASE(test_mm, aos_1_005),
#endif
#endif
    ADD_CASE_NULL
};

#if (TEST_CONFIG_TASK_ENABLED > 0)
static void task0(void *arg)
{
    krhino_task_sleep(10);
    g_var0++;
}

CASE(test_task, aos_1_006)
{
    kstat_t ret;

    g_var0 = 0;
    printf("create task start\r\n");
    ret = krhino_task_create(&g_task[0], "task0", NULL, 10, 50,
                             stack_buf[0], TEST_CONFIG_STACK_SIZE, task0, 1);
    ASSERT_EQ(ret, RHINO_SUCCESS);
    printf("create task success\r\n");

    krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND*2);
    krhino_task_del(&g_task[0]);

    ASSERT_EQ(g_var0, 1);
    g_var0 = 0;
}

CASE(test_task, aos_1_007)
{
    /* case is not support by rhino */
}

CASE(test_task, aos_1_008)
{
    /* case is not support by rhino */
}

CASE(test_task, aos_1_009)
{
    /* case is not support by rhino */
}

CASE(test_task, aos_1_010)
{
    /* case is not support by rhino */
}

CASE(test_task, aos_1_011)
{
    kstat_t ret;
    char task_name[10];
    int i = 0;

    g_var0 = 0;
    for (i=0; i<TEST_CONFIG_MAX_TASK_COUNT; i++) {
        sprintf(task_name, "task%d", i);
        ret = krhino_task_create(&g_task[i], task_name, NULL, 10, 50,
                                 stack_buf[i], TEST_CONFIG_STACK_SIZE, task0, 1);
        ASSERT_EQ(ret, RHINO_SUCCESS);
        printf("create %s OK\r\n", task_name);
		krhino_task_sleep(20);
    }
    krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND*5);
    for (i=0; i<TEST_CONFIG_MAX_TASK_COUNT; i++) {
        krhino_task_del(&g_task[i]);
    }

    ASSERT_EQ(g_var0, TEST_CONFIG_MAX_TASK_COUNT);
    g_var0 = 0;
}

CASE(test_task, aos_1_012)
{
    kstat_t ret;
    char task_name[10];
    int i = 0;

    for(i=1; i<=TEST_CONFIG_CREATE_TASK_TIMES; i++) {
        g_var0 = 0;
        sprintf(task_name, "task%d", i);
        ret = krhino_task_create(&g_task[0], task_name, NULL, 10, 50,
                                 stack_buf[0], TEST_CONFIG_STACK_SIZE, task0, 1);
        ASSERT_EQ(ret, RHINO_SUCCESS);
        krhino_task_sleep(10);
        ASSERT_EQ(g_var0, 1);
        if (i % 50 == 0) {
            printf("create task: %d/%d\r\n", i, TEST_CONFIG_CREATE_TASK_TIMES);
        }
				krhino_task_del(&g_task[0]);
				krhino_task_sleep(10);
    }
}
#endif /* TEST_CONFIG_TASK_ENABLED > 0 */

SUITE(test_task) =
{
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
static kmutex_t g_mutex;
/* task: decrease g_var0 with mutex*/
static void task1(void *arg)
{
    int i = 0;

    for(i=0; i<TEST_CONFIG_SYNC_TIMES; i++) {
        krhino_mutex_lock(&g_mutex, RHINO_WAIT_FOREVER);
        g_var0--;
        krhino_mutex_unlock(&g_mutex);
    }
    g_var1++;
}

/* task: increase g_var0 with mutex*/
static void task2(void *arg)
{
    int i = 0;

    for(i=0; i<TEST_CONFIG_SYNC_TIMES; i++) {
        krhino_mutex_lock(&g_mutex, RHINO_WAIT_FOREVER);
        g_var0++;
        krhino_mutex_unlock(&g_mutex);
    }
    g_var1++;
}

CASE(test_task_comm, aos_1_013)
{
    kstat_t ret;

    ret = krhino_mutex_create(&g_mutex, "g_mutext");
    ASSERT_EQ(ret, RHINO_SUCCESS);

    ret = krhino_mutex_lock(&g_mutex, RHINO_CONFIG_TICKS_PER_SECOND);
    ASSERT_EQ(ret, RHINO_SUCCESS);

    ret = krhino_mutex_lock(&g_mutex, RHINO_CONFIG_TICKS_PER_SECOND);
    ASSERT_EQ(ret, RHINO_MUTEX_OWNER_NESTED);

    ret = krhino_mutex_unlock(&g_mutex);
    ASSERT_EQ(ret, RHINO_MUTEX_OWNER_NESTED);

    ret = krhino_mutex_del(&g_mutex);
    ASSERT_EQ(ret, RHINO_SUCCESS);
}

CASE(test_task_comm, aos_1_014)
{
    kstat_t ret = RHINO_SUCCESS;
    char task_name[10];
    int task_count = 4;
    int i = 0;

    ASSERT_EQ(task_count % 2, 0);

    g_var0 = 0;
    g_var1 = 0;
    ret = krhino_mutex_create(&g_mutex, "g_mutext");
    ASSERT_EQ(ret, RHINO_SUCCESS);
    for (i=0; i<task_count; i++) {
        if(i < (task_count>>1)) {
            ret = krhino_task_create(&g_task[i], task_name, NULL, 10, 50,
                                     stack_buf[i], TEST_CONFIG_STACK_SIZE, task1, 1);
            ASSERT_EQ(ret, RHINO_SUCCESS);
        }
        else {
            ret = krhino_task_create(&g_task[i], task_name, NULL, 10, 50,
                                     stack_buf[i], TEST_CONFIG_STACK_SIZE, task2, 1);
            ASSERT_EQ(ret, RHINO_SUCCESS);
        }
        krhino_task_sleep(1);
    }

    while(g_var1 < task_count) {
        krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND);
    }
    for(i=0; i<task_count; i++) {
        krhino_task_del(&g_task[i]);
    }
    krhino_mutex_del(&g_mutex);
    ASSERT_EQ(g_var0, 0);
}

#if (RHINO_CONFIG_SEM > 0)
static ksem_t g_sem;
/* task: decrease g_var with sem */
static void task3(void *arg)
{
    int i = 0;

    for(i=0; i<TEST_CONFIG_SYNC_TIMES; i++) {
        krhino_sem_take(&g_sem, RHINO_WAIT_FOREVER);
        g_var0--;
        krhino_sem_give(&g_sem);
    }
    g_var1++;
}

/* task: decrease g_var with sem */
static void task4(void *arg)
{
    int i = 0;

    for(i=0; i<TEST_CONFIG_SYNC_TIMES; i++) {
        krhino_sem_take(&g_sem, RHINO_WAIT_FOREVER);
        g_var0++;
        krhino_sem_give(&g_sem);
    }
    g_var1++;
}
CASE(test_task_comm, aos_1_015)
{
    kstat_t ret = RHINO_SUCCESS;

    ret = krhino_sem_create(&g_sem, "g_sem", 0);
    ASSERT_EQ(ret, RHINO_SUCCESS);

    ret = krhino_sem_take(&g_sem, RHINO_CONFIG_TICKS_PER_SECOND);
    ASSERT_EQ(ret, RHINO_BLK_TIMEOUT);

    ret = krhino_sem_give(&g_sem);
    ASSERT_EQ(ret, RHINO_SUCCESS);

    ret = krhino_sem_take(&g_sem, RHINO_CONFIG_NEXT_INTRPT_TICKS);
    ASSERT_EQ(ret, RHINO_SUCCESS);

    ret = krhino_sem_del(&g_sem);
    ASSERT_EQ(ret, RHINO_SUCCESS);
}

CASE(test_task_comm, aos_1_016)
{
    kstat_t ret = RHINO_SUCCESS;
    char task_name[10] = {0};
    unsigned int task_count = 4;
    int i = 0;

    ASSERT_TRUE(task_count%2 == 0);

    ret = krhino_sem_create(&g_sem, "g_sem", 1);
    ASSERT_EQ(ret, RHINO_SUCCESS);

    g_var0 = 0;
    g_var1 = 0;
    for(i=0; i<task_count; i++) {
        sprintf(task_name, "task%d", i+1);
        if(i < (task_count>>1)) {
            ret = krhino_task_create(&g_task[i], task_name, NULL, 10, 50,
                                     stack_buf[i], TEST_CONFIG_STACK_SIZE, task3, 1);
            ASSERT_EQ(ret, RHINO_SUCCESS);
        }
        else {
            ret = krhino_task_create(&g_task[i], task_name, NULL, 10, 50,
                                     stack_buf[i], TEST_CONFIG_STACK_SIZE, task4, 1);
            ASSERT_EQ(ret, RHINO_SUCCESS);
        }
        krhino_task_sleep(1);
    }
    while(g_var1 < task_count) {
        krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND);
    }
    for(i=0; i<task_count; i++) {
        krhino_task_del(&g_task[i]);
    }
    krhino_sem_del(&g_sem);
    ASSERT_EQ(g_var0, 0);
}
#endif /* RHINO_CONFIG_SEM > 0 */

#if (RHINO_CONFIG_QUEUE > 0)
static char *queue_buf[3][TEST_CONFIG_QUEUE_BUF_SIZE];
static kqueue_t g_queue[3];

/* task: g_queue[0] -> g_queue[1] */
static void task5(void *arg)
{
    int *recv = NULL;

    while(1) {
        krhino_queue_recv(&g_queue[0], RHINO_WAIT_FOREVER, (void**)&recv);
        krhino_queue_back_send(&g_queue[1], recv);
        if(*recv == TEST_CONFIG_SYNC_TIMES) {
            break;
        }
    }
    printf("task5 exit!\r\n");
}

/* task: g_queue[1] -> g_queue[2] */
static void task6(void *arg)
{
    int *recv = NULL;

    while(1) {
        krhino_queue_recv(&g_queue[1], RHINO_WAIT_FOREVER, (void**)&recv);
        krhino_queue_back_send(&g_queue[2], recv);
        if(*recv == TEST_CONFIG_SYNC_TIMES) {
            break;
        }
    }
    printf("task6 exit!\r\n");
}

CASE(test_task_comm, aos_1_017)
{
    kstat_t ret = RHINO_SUCCESS;
    char *send = "hello world!";
    char *recv = NULL;

    ret = krhino_queue_create(&g_queue[0], "g_queue", (void**)&queue_buf[0], TEST_CONFIG_QUEUE_BUF_SIZE);
    ASSERT_EQ(ret, RHINO_SUCCESS);

    ret = krhino_queue_recv(&g_queue[0], 100, (void**)&recv);
    ASSERT_EQ(ret, RHINO_BLK_TIMEOUT);

    krhino_queue_del(&g_queue[0]);
}

CASE(test_task_comm, aos_1_018)
{
    kstat_t ret = RHINO_SUCCESS;
    int i = 0;
    int send = 0;
    int *recv = NULL;

    ret = krhino_queue_create(&g_queue[0], "g_queue[0]", (void**)&queue_buf[0], TEST_CONFIG_QUEUE_BUF_SIZE);
    ASSERT_EQ(ret, RHINO_SUCCESS);

    ret = krhino_queue_create(&g_queue[1], "g_queue[1]", (void**)&queue_buf[1], TEST_CONFIG_QUEUE_BUF_SIZE);
    ASSERT_EQ(ret, RHINO_SUCCESS);

    ret = krhino_queue_create(&g_queue[2], "g_queue[2]", (void**)&queue_buf[2], TEST_CONFIG_QUEUE_BUF_SIZE);
    ASSERT_EQ(ret, RHINO_SUCCESS);

    ret = krhino_task_create(&g_task[0], "task5", NULL, 10, 50,
                             stack_buf[0], TEST_CONFIG_STACK_SIZE, task5, 1);
    ASSERT_EQ(ret, RHINO_SUCCESS);

    ret = krhino_task_create(&g_task[1], "task6", NULL, 10, 50,
                             stack_buf[1], TEST_CONFIG_STACK_SIZE, task6, 1);
    ASSERT_EQ(ret, RHINO_SUCCESS);

    for(i=1; i<=TEST_CONFIG_SYNC_TIMES; i++) {
        send = i;
        ret = krhino_queue_back_send(&g_queue[0], &send);
        ASSERT_EQ(ret, RHINO_SUCCESS);

        ret = krhino_queue_recv(&g_queue[2], RHINO_WAIT_FOREVER, (void **)&recv);
        ASSERT_EQ(ret, RHINO_SUCCESS);
        ASSERT_EQ(send, *recv);
        if (i % (TEST_CONFIG_SYNC_TIMES/10) == 0) {
            printf("queue sync: %d/%d\r\n", i, TEST_CONFIG_SYNC_TIMES);
        }
    }
    krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND);

    krhino_queue_del(&g_queue[0]);
    krhino_queue_del(&g_queue[1]);
    krhino_queue_del(&g_queue[2]);
    krhino_task_del(&g_task[0]);
    krhino_task_del(&g_task[1]);
}
#endif /* RHINO_CONFIG_QUEUE > 0 */
#endif /* TEST_CONFIG_TASK_COMM_ENABLED > 0 */

SUITE(test_task_comm) =
{
#if (TEST_CONFIG_TASK_COMM_ENABLED > 0)
    ADD_CASE(test_task_comm, aos_1_013),
    ADD_CASE(test_task_comm, aos_1_014),
#if (RHINO_CONFIG_SEM > 0)
    ADD_CASE(test_task_comm, aos_1_015),
    ADD_CASE(test_task_comm, aos_1_016),
#endif
#if (RHINO_CONFIG_QUEUE > 0)
    ADD_CASE(test_task_comm, aos_1_017),
    ADD_CASE(test_task_comm, aos_1_018),
#endif
#endif
    ADD_CASE_NULL
};

#if (TEST_CONFIG_TIMER_ENABLED > 0)
#if (RHINO_CONFIG_TIMER > 0)
static ktimer_t g_timer;
static void timer_handler(void* timer, void* args)
{
    printf("timer handler: %d\r\n", ++g_var0);
}

CASE(test_timer, aos_1_019)
{
    kstat_t ret = RHINO_SUCCESS;

    g_var0 = 0;
    ret = krhino_timer_create(&g_timer, "g_timer", timer_handler, 1, 10, NULL, 0);
    ASSERT_EQ(ret, RHINO_SUCCESS);

    ret = krhino_timer_start(&g_timer);
    ASSERT_EQ(ret, RHINO_SUCCESS);

    while(g_var0 != 10) {
        krhino_task_sleep(1);
    }
    krhino_timer_stop(&g_timer);
    ret = krhino_timer_del(&g_timer);
    ASSERT_EQ(ret, RHINO_SUCCESS);
}

CASE(test_timer, aos_1_020)
{
    kstat_t ret = RHINO_SUCCESS;

    g_var0 = 0;
    ret = krhino_timer_create(&g_timer, "g_timer", timer_handler, 1, RHINO_CONFIG_TICKS_PER_SECOND, NULL, 0);
    ASSERT_EQ(ret, RHINO_SUCCESS);

    ret = krhino_timer_start(&g_timer);
    ASSERT_EQ(ret, RHINO_SUCCESS);

    while(g_var0 != 3) {
        krhino_task_sleep(1);
    }
    krhino_timer_stop(&g_timer);
    krhino_timer_change(&g_timer, 1, RHINO_CONFIG_TICKS_PER_SECOND*2);
    krhino_timer_start(&g_timer);
    while(g_var0 != 6) {
        krhino_task_sleep(1);
    }
    krhino_timer_stop(&g_timer);
    ret = krhino_timer_del(&g_timer);
    ASSERT_EQ(ret, RHINO_SUCCESS);
}
#endif /* RHINO_CONFIG_TIMER > 0 */
#endif /* TEST_CONFIG_TIMER_ENABLED > 0 */

SUITE(test_timer) =
{
#if (TEST_CONFIG_TIMER_ENABLED > 0)
#if (RHINO_CONFIG_TIMER > 0)
    ADD_CASE(test_timer, aos_1_019),
    ADD_CASE(test_timer, aos_1_020),
#endif
#endif
    ADD_CASE_NULL
};

#define TEST_TASK_STACKSIZE    (512)
static ktask_t test_task_obj;
static cpu_stack_t test_task_buf[TEST_TASK_STACKSIZE];
void test_task(void *arg)
{
    if (0 == dump_test_config()) {
        printf("test start!\r\n");
        ADD_SUITE(test_mm);
        ADD_SUITE(test_task);
        ADD_SUITE(test_task_comm);
        ADD_SUITE(test_timer);
        cut_main(0, NULL);
        printf("test finished!\r\n");
    }
    else {
        printf("test error!\r\n");
    }

    krhino_task_del(&test_task_obj);
}

/*extern void sdk_init(void);*/
void test_certificate(void)
{
    krhino_init();
    krhino_task_create(&test_task_obj, "test_task", 0, 20, 50,
                       test_task_buf, TEST_TASK_STACKSIZE, test_task, 1);

    /* mcu-specific initialization start */
    /*sdk_init();*/
    /* mcu-specific initialization finish */

    krhino_start();
}
