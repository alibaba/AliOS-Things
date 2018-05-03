/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <aos/kernel.h>

#include "cutest/cut.h"

#define QUEUE_BUF_SIZE (64)

#ifndef SYSINFO_ARCH
#define SYSINFO_ARCH	"Andes N1068A"
#endif

#ifndef SYSINFO_MCU
#define SYSINFO_MCU		"SV6266"
#endif

#ifndef SYSINFO_DEVICE_NAME
#define SYSINFO_DEVICE_NAME		"SV6266_EVB"
#endif

#define SYSINFO_KERNEL	"AOS"

static unsigned int g_var = 0;
static aos_sem_t    g_sem_taskexit_sync;
static aos_mutex_t  g_mutex;
static aos_sem_t    g_sem;
static aos_queue_t  g_queue;
static aos_timer_t  g_timer;

static char         queue_buf[QUEUE_BUF_SIZE];
static aos_queue_t  g_queue1;
static char         queue1_buf[QUEUE_BUF_SIZE];
static aos_queue_t  g_queue2;
static char         queue2_buf[QUEUE_BUF_SIZE];
static aos_queue_t  g_queue3;
static char         queue3_buf[QUEUE_BUF_SIZE];


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

    aos_realloc(ptr, size2);
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

    aos_realloc(ptr, size2);
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

/* memory manager test suite */
SUITE(test_mm) = {
    ADD_CASE(test_mm, aos_1_001),
    ADD_CASE(test_mm, aos_1_002),
    ADD_CASE(test_mm, aos_1_003),
    ADD_CASE(test_mm, aos_1_004),
    ADD_CASE(test_mm, aos_1_005),
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
    printf("task name: %s\n", aos_task_name());
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
        printf("%s task key: %d\n", aos_task_name(), task_key);
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
    printf("%s task key: %d\n", aos_task_name(), task_key);

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

    printf("task name %s: decrease\n", aos_task_name());
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

    printf("task name %s: increase\n", aos_task_name());
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

    printf("task name %s: decrease\n", aos_task_name());
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

    printf("task name %s: increase\n", aos_task_name());
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
    printf("%s exit!\n", aos_task_name());
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
    printf("%s exit!\n", aos_task_name());
    aos_task_exit(0);
}

CASE(test_task, aos_1_006)
{
    unsigned int stack_size = 1024;
    int ret = -1;

    aos_sem_new(&g_sem_taskexit_sync, 0);

    ret = aos_task_new("task1", task1, NULL, stack_size);
    ASSERT_EQ(ret, 0);

    aos_sem_wait(&g_sem_taskexit_sync, -1);
    printf("task1 exit!\n");

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
    printf("task1 exit!\n");

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
    printf("task2 exit!\n");

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
    printf("task2 exit!\n");

    aos_sem_free(&g_sem_taskexit_sync);
}

CASE(test_task, aos_1_010)
{
    unsigned int stack_size = 1024;
    int ret = -1;

    aos_sem_new(&g_sem_taskexit_sync, 0);

    ret = aos_task_new("task3", task3, NULL, stack_size);
    ASSERT_EQ(ret, 0);

    aos_sem_wait(&g_sem_taskexit_sync, -1);
    printf("task3 exit!\n");

    aos_sem_free(&g_sem_taskexit_sync);
}

CASE(test_task, aos_1_011)
{
    unsigned int stack_size = 512;
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
    printf("%d tasks exit!\n", TEST_CONFIG_MAX_TASK_COUNT);

    aos_sem_free(&g_sem_taskexit_sync);
}

CASE(test_task, aos_1_012)
{
    unsigned int stack_size = 512;
    char task_name[10] = {0};
    int ret = -1;
    int i = 0;


    for(i=0; i<TEST_CONFIG_CREATE_TASK_TIMES; i++) {
        memset(task_name, 0, sizeof(task_name));
        snprintf(task_name, 10, "task%02d", i);
        if(i % 500 == 0) {
            printf("create task: %d/%d\n", i, TEST_CONFIG_CREATE_TASK_TIMES);
        }

        aos_sem_new(&g_sem_taskexit_sync, 0);
        ret = aos_task_new(task_name, task0, NULL, stack_size);
        ASSERT_EQ(ret, 0);

        aos_sem_wait(&g_sem_taskexit_sync, -1);
        aos_sem_free(&g_sem_taskexit_sync);
        aos_msleep(1);
    }
}

/* task test suite */
SUITE(test_task) = {
    ADD_CASE(test_task, aos_1_006),
    ADD_CASE(test_task, aos_1_007),
    ADD_CASE(test_task, aos_1_008),
    ADD_CASE(test_task, aos_1_009),
    ADD_CASE(test_task, aos_1_010),
    ADD_CASE(test_task, aos_1_011),
    ADD_CASE(test_task, aos_1_012),
    ADD_CASE_NULL
};


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
    printf("g_var = %d\n", g_var);
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
    printf("g_var = %d\n", g_var);
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

    ret = aos_queue_new(&g_queue, queue_buf, QUEUE_BUF_SIZE, QUEUE_BUF_SIZE);
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

    ret = aos_queue_new(&g_queue1, queue1_buf, QUEUE_BUF_SIZE, QUEUE_BUF_SIZE);
    ASSERT_EQ(ret, 0);
    ret = aos_queue_new(&g_queue2, queue2_buf, QUEUE_BUF_SIZE, QUEUE_BUF_SIZE);
    ASSERT_EQ(ret, 0);
    ret = aos_queue_new(&g_queue3, queue3_buf, QUEUE_BUF_SIZE, QUEUE_BUF_SIZE);
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
            printf("%d/%d\n", i, TEST_CONFIG_SYNC_TIMES);
        }
    }
    ASSERT_EQ(msg_recv, TEST_CONFIG_SYNC_TIMES);
    aos_queue_free(&g_queue1);
    aos_queue_free(&g_queue2);
    aos_queue_free(&g_queue3);
}

/* task commication */
SUITE(test_task_comm) = {
    ADD_CASE(test_task_comm, aos_1_013),
    ADD_CASE(test_task_comm, aos_1_014),
    ADD_CASE(test_task_comm, aos_1_015),
    ADD_CASE(test_task_comm, aos_1_016),
    ADD_CASE(test_task_comm, aos_1_017),
    ADD_CASE(test_task_comm, aos_1_018),
    ADD_CASE_NULL
};

static void timer_handler(void *arg1, void* arg2)
{
    printf("timer handler\n");
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
    printf("timer stopped!\n");
    aos_msleep(1000);
    aos_timer_change(&g_timer, 1000);
    printf("timer changed!\n");
    aos_timer_start(&g_timer);
    printf("timer started!\n");

    aos_sem_wait(&g_sem, -1);
    aos_sem_free(&g_sem);

    aos_timer_stop(&g_timer);
    aos_timer_free(&g_timer);
}

/* timer test suite */
SUITE(test_timer) = {
    ADD_CASE(test_timer, aos_1_019),
    ADD_CASE(test_timer, aos_1_020),
    ADD_CASE_NULL
};

#ifdef TEST_CONFIG_KV_ENABLED
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
            printf("kv test: %d/%d\n", i, TEST_CONFIG_KV_TIMES);
        }
        aos_msleep(1);
    }
}

/* kv test suite */
SUITE(test_kv) = {
    ADD_CASE(test_kv, aos_2_001),
    ADD_CASE(test_kv, aos_2_002),
    ADD_CASE(test_kv, aos_2_003),
    ADD_CASE_NULL
};
#endif

#ifdef TEST_CONFIG_YLOOP_ENABLED
#include <aos/yloop.h>
#define EV_USER_TYPE1   (EV_USER+1)
#define EV_USER_TYPE2   (EV_USER+2)

static void ev_callback1(input_event_t *event, void *private_data)
{
    printf("ev_callback1 is called\n");
    g_var = 0x5A;
    aos_loop_exit();
}

static void delayed_action1(void *arg)
{
    g_var++;
    printf("delayed_action1 called, %d\n", g_var);
    aos_post_delayed_action(1000, delayed_action1, NULL);
    aos_cancel_delayed_action(1000, delayed_action1, NULL);
}

static void delayed_action2(void *arg)
{
    g_var++;
    printf("delayed_action2 called, %d\n", g_var);
    aos_loop_exit();
}

static void delayed_action3(void *arg)
{
    g_var++;
}

static void task_loop1(void *arg)
{
    int ret = -1;
    aos_loop_t loop = NULL;

    g_var = 0;
    printf("task name: %s\n", aos_task_name());

    loop = aos_loop_init();
    if (loop == NULL) {
        g_var = -1;
        aos_sem_signal(&g_sem_taskexit_sync);
        aos_task_exit(-1);
    }

    ret = aos_post_delayed_action(200, delayed_action2, NULL);
    if(ret != 0) {
        g_var = -2;
        aos_sem_signal(&g_sem_taskexit_sync);
        aos_task_exit(-2);
    }

    aos_loop_run();
    aos_loop_destroy();
    aos_sem_signal(&g_sem_taskexit_sync);
    aos_task_exit(0);
}

CASE(test_yloop, aos_2_004)
{
    int ret = -1;
    g_var = 0;

    ret = aos_register_event_filter(EV_USER_TYPE1, ev_callback1, NULL);
    ASSERT_EQ(ret, 0);

    ret = aos_post_event(EV_USER_TYPE1, 0, 0);
    // ASSERT_EQ(ret, 0);

    aos_loop_run();
    aos_unregister_event_filter(EV_USER_TYPE1, ev_callback1, NULL);
    ASSERT_EQ(g_var, 0x5A);
}

CASE(test_yloop, aos_2_005)
{
    int ret = -1;
    g_var = 0;

    ret = aos_post_delayed_action(500, delayed_action1, NULL);
    ASSERT_EQ(ret, 0);

    ret = aos_post_delayed_action(3000, delayed_action2, NULL);
    ASSERT_EQ(ret, 0);

    aos_loop_run();
    ASSERT_EQ(g_var, 2);
}

CASE(test_yloop, aos_2_006)
{
    int ret = -1;
    int stack_size = 1024;

    g_var = 0;
    ret = aos_sem_new(&g_sem_taskexit_sync, 0);
    ASSERT_EQ(ret, 0);

    ret = aos_task_new("task_loop1",  task_loop1, NULL, stack_size);
    ASSERT_EQ(ret, 0);

    aos_sem_wait(&g_sem_taskexit_sync, 1000);
    ASSERT_EQ(g_var, 1);

    aos_sem_free(&g_sem_taskexit_sync);
}

CASE(test_yloop, aos_2_007)
{
    int ret = -1;
    g_var = 0;

    ret = aos_post_delayed_action(200, delayed_action2, NULL);
    ASSERT_EQ(ret, 0);

    aos_loop_run();
    ASSERT_EQ(g_var, 1);
}

CASE(test_yloop, aos_2_008)
{
    int ret = -1;
    int i = 0;
    g_var = 0;

    for(i=0; i<TEST_CONFIG_YLOOP_EVENT_COUNT; i++) {
        ret = aos_post_delayed_action(i, delayed_action3, NULL);
        ASSERT_EQ(ret, 0);
        aos_msleep(1);
    }

    ret = aos_post_delayed_action(TEST_CONFIG_YLOOP_EVENT_COUNT+1000, delayed_action2, NULL);
    ASSERT_EQ(ret, 0);
    aos_loop_run();
    ASSERT_EQ(g_var, TEST_CONFIG_YLOOP_EVENT_COUNT+1);
}

CASE(test_yloop, aos_2_009)
{
    int i = 0;
    int ret = -1;
    int stack_size = 512;
    char task_name[10] = {0};

    g_var = 0;
    aos_sem_new(&g_sem_taskexit_sync, 0);

    for(i=0; i<TEST_CONFIG_MAX_TASK_COUNT; i++) {
        memset(task_name, 0, sizeof(task_name));
        snprintf(task_name, 10, "task%02d", i);
        ret = aos_task_new(task_name, task_loop1, NULL, stack_size);
        ASSERT_EQ(ret, 0);
        aos_msleep(1);
    }
    for(i=0; i<TEST_CONFIG_MAX_TASK_COUNT; i++) {
        aos_sem_wait(&g_sem_taskexit_sync, -1);
    }
    printf("%d tasks exit!\n", TEST_CONFIG_MAX_TASK_COUNT);
    ASSERT_EQ(g_var, TEST_CONFIG_MAX_TASK_COUNT);
    aos_sem_free(&g_sem_taskexit_sync);
}

/* yloop test suite */
SUITE(test_yloop) = {
    ADD_CASE(test_yloop, aos_2_004),
    ADD_CASE(test_yloop, aos_2_005),
    ADD_CASE(test_yloop, aos_2_006),
    ADD_CASE(test_yloop, aos_2_007),
    ADD_CASE(test_yloop, aos_2_008),
    ADD_CASE(test_yloop, aos_2_009),
    ADD_CASE_NULL
};
#endif

static void dump_test_config(void)
{
#define _PARSE(x) #x
#define PARSE(x) _PARSE(x)
#define PRINT_CONFIG(x) printf("\33[0;35m%s=%s\33[0m\n", #x, PARSE(x))
    
    PRINT_CONFIG(SYSINFO_ARCH);
	PRINT_CONFIG(SYSINFO_MCU);
	PRINT_CONFIG(SYSINFO_DEVICE_NAME);
	PRINT_CONFIG(SYSINFO_KERNEL);
	PRINT_CONFIG(SYSINFO_KERNEL_VERSION);

#undef PRINT_CONFIG
#define PRINT_CONFIG(str, config) \
    printf("\33[0;35m%s=%d\33[0m\n", str, config)
    
#ifdef TEST_CONFIG_MALLOC_FREE_TIMES
    PRINT_CONFIG("TEST_CONFIG_MALLOC_FREE_TIMES", TEST_CONFIG_MALLOC_FREE_TIMES);
#endif
#ifdef TEST_CONFIG_MAX_TASK_COUNT
    PRINT_CONFIG("TEST_CONFIG_MAX_TASK_COUNT", TEST_CONFIG_MAX_TASK_COUNT);
#endif
#ifdef TEST_CONFIG_CREATE_TASK_TIMES
    PRINT_CONFIG("TEST_CONFIG_CREATE_TASK_TIMES", TEST_CONFIG_CREATE_TASK_TIMES);
#endif
#ifdef TEST_CONFIG_SYNC_TIMES
    PRINT_CONFIG("TEST_CONFIG_SYNC_TIMES", TEST_CONFIG_SYNC_TIMES);
#endif
#ifdef TEST_CONFIG_KV_ENABLED
    PRINT_CONFIG("TEST_CONFIG_KV_ENABLED", TEST_CONFIG_KV_ENABLED);
#endif
#ifdef TEST_CONFIG_KV_TIMES
    PRINT_CONFIG("TEST_CONFIG_KV_TIMES", TEST_CONFIG_KV_TIMES);
#endif
#ifdef test_config_yloop_enabled
    PRINT_CONFIG("TEST_CONFIG_KV_TIMES", TEST_CONFIG_KV_TIMES);
#endif
#ifdef TEST_CONFIG_YLOOP_EVENT_COUNT
    PRINT_CONFIG("TEST_CONFIG_YLOOP_EVENT_COUNT", TEST_CONFIG_YLOOP_EVENT_COUNT);
#endif
#ifdef TEST_CONFIG_YLOOP_LOOP_COUNT
    PRINT_CONFIG("TEST_CONFIG_YLOOP_LOOP_COUNT", TEST_CONFIG_YLOOP_LOOP_COUNT);
#endif
}


void test_certificate(void)
{
    dump_test_config();

    ADD_SUITE(test_mm);
    ADD_SUITE(test_task);
    ADD_SUITE(test_task_comm);
    ADD_SUITE(test_timer);

#ifdef TEST_CONFIG_KV_ENABLED
    ADD_SUITE(test_kv);
#endif

#ifdef TEST_CONFIG_YLOOP_ENABLED
    ADD_SUITE(test_yloop);
#endif

    cut_main(0, NULL);
}
AOS_TESTCASE(test_certificate);
