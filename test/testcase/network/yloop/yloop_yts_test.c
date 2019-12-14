/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>

#include "yunit/yunit.h"
#include "yunit/yts.h"

#include <aos/kernel.h>
#include <ulog/ulog.h>
#include "aos/yloop.h"
#include "network/network.h"

#define TAG "yloop_yts"

#define EV_YLOOP_TEST               0x1010
#define CODE_YLOOP_TEST             1
#define VALUE_YLOOP_TEST            2 
#define NON_MAIN_TEST_TIMEOUT_MS    60000
#define TEST_YLOOP_DEFAULT_DELAY_MS 1000
#define TEST_YLOOP_DEFAULT_PORT     9999
#define BUFFER_SIZE                 128
#define TEST_DEFUALT_MSG            "YLOOP TEST ECHO"

static aos_sem_t non_main_sem;
static int read_fd;
static char recv_buf[BUFFER_SIZE] = {0};

void aos_sem_signal(aos_sem_t *sem);

static void yloop_test_event(input_event_t *event, void *priv_data)
{
    if (event->type != EV_YLOOP_TEST) {
        return;
    }

    if (event->code != CODE_YLOOP_TEST) {
        return;
    }

    if (event->value != VALUE_YLOOP_TEST) {
        return;
    }

    LOGI(TAG, "yloop_test_event cb called!");
}

static void yloop_test_delayed_action(void *arg)
{
    LOGI(TAG, "yloop_test_delayed_action cb called! aos now ms %u", (unsigned int) aos_now_ms());
}

static void yloop_test_action(void *arg)
{
    LOGI(TAG, "yloop_test_action cb called!");
}

static void yloop_test_action_finish(void *arg)
{
    LOGI(TAG, "yloop_test_action finish cb called!");
}

static void test_main_loop(void)
{
    int ret_i;
    void *work = NULL;

    ret_i = aos_register_event_filter(EV_YLOOP_TEST, yloop_test_event, NULL);
    YUNIT_ASSERT( 0 == ret_i );

    ret_i = aos_post_event(EV_YLOOP_TEST, CODE_YLOOP_TEST, VALUE_YLOOP_TEST);
    YUNIT_ASSERT( ret_i >= 0 );

    LOGI(TAG, "yloop_test_delayed_action cb to be called! aos now ms %u", (unsigned int) aos_now_ms());
    ret_i = aos_post_delayed_action(0, yloop_test_delayed_action, NULL);
    YUNIT_ASSERT( 0 == ret_i );

    ret_i = aos_schedule_call(yloop_test_action, NULL);
    YUNIT_ASSERT( ret_i >= 0 );

    LOGI(TAG, "aos_loop_schedule_work cb to be called! aos now ms %u", (unsigned int) aos_now_ms());
    work = aos_loop_schedule_work(0, yloop_test_delayed_action, NULL,
                                  yloop_test_action_finish, NULL);
    YUNIT_ASSERT( NULL != work );
}

static void test_main_loop_cancel(void)
{
    void *work = NULL;
    int ret_i;

    ret_i = aos_unregister_event_filter(EV_YLOOP_TEST, yloop_test_event, NULL);
    YUNIT_ASSERT( 0 == ret_i );

    ret_i = aos_post_event(EV_YLOOP_TEST, CODE_YLOOP_TEST, VALUE_YLOOP_TEST);
    YUNIT_ASSERT( ret_i >= 0  );

    LOGI(TAG, "yloop_test_delayed_action cb to be called! aos now ms %u", (unsigned int) aos_now_ms());
    ret_i = aos_post_delayed_action(TEST_YLOOP_DEFAULT_DELAY_MS, yloop_test_delayed_action, NULL);
    YUNIT_ASSERT( 0 == ret_i );

    aos_cancel_delayed_action(TEST_YLOOP_DEFAULT_DELAY_MS, yloop_test_delayed_action, NULL);

    LOGI(TAG, "aos_loop_schedule_work cb to be called! aos now ms %u", (unsigned int) aos_now_ms());
    work = aos_loop_schedule_work(TEST_YLOOP_DEFAULT_DELAY_MS, yloop_test_delayed_action, NULL,
                                  yloop_test_action_finish, NULL);
    YUNIT_ASSERT( NULL != work );
    if ( !work ) return;

    aos_cancel_work(work, yloop_test_delayed_action, NULL);
}

static void yloop_non_main_task_action(void *arg)
{
    LOGI(TAG, "yloop_non_main_task_action cb called!");
}

static void test_read_fd_loop(void);

static void test_pertask_loop(void *arg)
{
    int ret_i;
    aos_loop_t *loop;

    loop = aos_loop_init();
    YUNIT_ASSERT( NULL != loop );
    if ( !loop ) return;

    ret_i = aos_schedule_call(yloop_non_main_task_action, NULL);
    YUNIT_ASSERT( ret_i >= 0 );

    test_read_fd_loop();

    aos_loop_run();

    aos_loop_destroy();

    aos_sem_signal(&non_main_sem);
}

static void test_non_main_loop(void)
{
    int ret_i = 0;

    ret_i = aos_task_new("non_main_loop", test_pertask_loop, NULL, 4 * 1024);
    YUNIT_ASSERT( 0 == ret_i );
    if ( 0 != ret_i ) return;

    ret_i = aos_sem_wait(&non_main_sem, NON_MAIN_TEST_TIMEOUT_MS);
    YUNIT_ASSERT( 0 == ret_i );
}

static void yloop_read_fd_action(int fd, void *arg)
{
    int ret;

    LOGI(TAG, "yloop_read_fd_action cb called! Going to exit current loop");
    memset(recv_buf, 0, BUFFER_SIZE);
    ret = recv(fd, recv_buf, BUFFER_SIZE, 0);
    YUNIT_ASSERT(0 == strncmp(recv_buf, TEST_DEFUALT_MSG, strlen(TEST_DEFUALT_MSG)));
    aos_loop_exit();
}

static void yloop_send_loopback(void)
{
    int ret, len;
    struct sockaddr_in dest_addr;

    len = sizeof(dest_addr);
    bzero(&dest_addr,sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    inet_aton("127.0.0.1", &dest_addr.sin_addr);
    dest_addr.sin_port = htons(TEST_YLOOP_DEFAULT_PORT);

    ret = sendto(read_fd, TEST_DEFUALT_MSG, strlen(TEST_DEFUALT_MSG), 0, 
                 (struct sockaddr*)&dest_addr, len);
    YUNIT_ASSERT( ret > 0);
}

static void test_read_fd_loop(void)
{
    int ret_i = 0;
    ret_i = aos_poll_read_fd(read_fd, yloop_read_fd_action, NULL);
    YUNIT_ASSERT( 0 == ret_i );

    yloop_send_loopback();
} 

static int init(void)
{
    int ret;
    struct sockaddr_in serv_addr;

    aos_set_log_level(AOS_LL_INFO);
    aos_sem_new(&non_main_sem, 0);

    read_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (read_fd < 0) {
        LOGI(TAG, "create socket failed\n");
        return -1;
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(TEST_YLOOP_DEFAULT_PORT);

    ret = bind(read_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if (ret < 0) {
        LOGI(TAG, "socket bind fail!\n");
        close(read_fd);
        return -1;
    }

    return 0;
}

static int cleanup(void)
{
    aos_sem_free(&non_main_sem);
    close(read_fd);
    return 0;
}

static void setup(void)
{

}

static void teardown(void)
{

}

static yunit_test_case_t aos_yloop_testcases[] = {
    { "main_loop",        test_main_loop },
    { "main_loop_cancel", test_main_loop_cancel},
    { "non_main_loop",    test_non_main_loop},
    YUNIT_TEST_CASE_NULL
};

static yunit_test_suite_t suites[] = {
    { "yloop", init, cleanup, setup, teardown, aos_yloop_testcases },
    YUNIT_TEST_SUITE_NULL
};

void test_yloop(void)
{
    yunit_add_test_suites(suites);
}
AOS_TESTCASE(test_yloop);
