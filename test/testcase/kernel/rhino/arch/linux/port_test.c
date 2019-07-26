/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <pthread.h>
#include <semaphore.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <k_api.h>
#include <cpu_event.h>
#include <signal.h>

#include "yts/yunit.h"

#define TID ((long int)syscall(SYS_gettid))

static cpu_event_t test_cpu_event;
static const char *test_msg = "test linuxhost port";

static ksem_t    sem;
static pthread_t test_thread;
static sem_t     pthread_sem;
static long int  main_tid;
static int       test_done;

static void test_event_handler(const void *arg)
{
    YUNIT_ASSERT(arg == test_msg);

    krhino_sem_give(&sem);

    sigset_t sigblocked;
    sigprocmask(SIG_BLOCK, NULL, &sigblocked);
    YUNIT_ASSERT(0 == sigismember(&sigblocked, SIGUSR2));
    YUNIT_ASSERT(0 == sigismember(&sigblocked, SIGALRM));
}

static void test_cpu_event_case(void)
{
    int ret;

    test_cpu_event.handler = test_event_handler;
    test_cpu_event.arg     = test_msg;

    ret = cpu_notify_event(&test_cpu_event);

    YUNIT_ASSERT(ret == 0);

    krhino_sem_take(&sem, RHINO_WAIT_FOREVER);
}

static void test_event_pthread_handler(const void *arg)
{
    int ret;

    YUNIT_ASSERT(arg == &main_tid);

    ret = sem_post(&pthread_sem);

    YUNIT_ASSERT(ret == 0);

    sigset_t sigblocked;
    sigprocmask(SIG_BLOCK, NULL, &sigblocked);
    YUNIT_ASSERT(0 == sigismember(&sigblocked, SIGUSR2));
    YUNIT_ASSERT(0 == sigismember(&sigblocked, SIGALRM));
}

static void *test_cpu_event_pthread_proc(void *arg)
{
    int ret;

    test_cpu_event.handler = test_event_pthread_handler;
    test_cpu_event.arg     = arg;

    ret = cpu_notify_event(&test_cpu_event);

    YUNIT_ASSERT(ret == 0);

    ret = sem_wait(&pthread_sem);
    YUNIT_ASSERT(ret == 0);

    test_done = 1;

    return NULL;
}

static void test_cpu_event_pthread_case(void)
{
    int ret;

    main_tid = TID;

    ret = pthread_create(&test_thread, NULL, test_cpu_event_pthread_proc, &main_tid);

    YUNIT_ASSERT(ret == 0);

    while (!test_done) {
        krhino_task_sleep(10);
    }
}

static int init(void)
{
    int ret;

    ret = krhino_sem_create(&sem, "rhino test port", 0);
    if (ret != 0) {
        return ret;
    }

    ret = sem_init(&pthread_sem, 0, 0);

    return ret;
}

static int cleanup(void)
{
    int ret;

    ret = krhino_sem_del(&sem);
    if (ret != 0) {
        return ret;
    }

    ret = sem_destroy(&pthread_sem);

    return ret;
}

static void setup(void)
{
    return;
}

static void teardown(void)
{
    return;
}

static yunit_test_case_t rhino_port_testcases[] = {
    { "Testing cpu_event_case:", test_cpu_event_case },
    { "Testing cpu_event_pthread_case:", test_cpu_event_pthread_case },
    YUNIT_TEST_CASE_NULL
};

static yunit_test_suite_t suites[] = {
    { "rhino_port", init, cleanup, setup, teardown, rhino_port_testcases },
    YUNIT_TEST_SUITE_NULL
};

void test_rhino_port(void)
{
    yunit_add_test_suites(suites);
}

