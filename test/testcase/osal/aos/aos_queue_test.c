/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>

#include <k_api.h>
#include <aos/kernel.h>
#include <assert.h>

#include <yunit.h>
#include <errno.h>

#define TEST_TASK_STACK_SIZE (8192)

typedef struct
{
    int  id;
    int  len;
    char msg[32];
} Message;
#define TEST_QUEUE_MAX_MSG_SIZE (sizeof(Message))
#define TEST_QUEUE_MAX_MSG_COUNT (8)
#define TEST_QUEUE_SIZE (TEST_QUEUE_MAX_MSG_SIZE * TEST_QUEUE_MAX_MSG_COUNT)

static char         queue_buf[TEST_QUEUE_SIZE] = { 0 };
static aos_queue_t  g_queue;
static aos_sem_t    sync_sem;
static Message      send_msg;
static Message      recv_msg;
static unsigned int recv_size = 0;
static aos_sem_t    tmp_sem;
static aos_queue_t  queue;

static void CASE_aosapi_kernel_queue_param()
{
    int ret;

    /* aos_queue_new invalid param test */
#if 0
	// TODO: coredump
	ret = aos_queue_new(NULL, queue_buf, TEST_QUEUE_SIZE, TEST_QUEUE_MAX_MSG_SIZE);
	YUNIT_ASSERT_MSG(ret==RHINO_NULL_PTR, "ret=%d", ret);
#endif
    ret = aos_queue_new(&queue, NULL, TEST_QUEUE_SIZE, TEST_QUEUE_MAX_MSG_SIZE);
    YUNIT_ASSERT_MSG(ret != RHINO_SUCCESS, "ret=%d", ret);

    ret = aos_queue_new(&queue, queue_buf, 0, TEST_QUEUE_MAX_MSG_SIZE);
    YUNIT_ASSERT_MSG(ret != RHINO_SUCCESS, "ret=%d", ret);

    ret = aos_queue_new(&queue, queue_buf, TEST_QUEUE_SIZE, 0);
    YUNIT_ASSERT_MSG(ret != RHINO_SUCCESS, "ret=%d", ret);

    /* aos_queue_send invalid param test */
#if 0
	// TODO: coredump
	ret = aos_queue_send(NULL, &send_msg, sizeof(send_msg));
	YUNIT_ASSERT_MSG(ret==RHINO_NULL_PTR, "ret=%d", ret);
#endif

    // create fail
    ret = aos_queue_new(&queue, queue_buf, TEST_QUEUE_SIZE,
                        TEST_QUEUE_MAX_MSG_SIZE);
    YUNIT_ASSERT_MSG(ret == RHINO_SUCCESS, "ret=%d", ret);

    ret = aos_queue_send(&queue, NULL, sizeof(send_msg));
    YUNIT_ASSERT_MSG(ret != RHINO_SUCCESS, "ret=%d", ret);

    ret = aos_queue_send(&queue, &send_msg, 0);
    YUNIT_ASSERT_MSG(ret != RHINO_SUCCESS, "ret=%d", ret);

    ret = aos_queue_send(&queue, &send_msg, TEST_QUEUE_MAX_MSG_SIZE);
    YUNIT_ASSERT_MSG(ret == RHINO_SUCCESS, "ret=%d", ret);

    ret = aos_sem_new(&tmp_sem, 0);
    YUNIT_ASSERT_MSG(ret == RHINO_SUCCESS, "ret=%d", ret);
    ret = aos_queue_send((aos_queue_t *)&tmp_sem, &send_msg, sizeof(send_msg));
    YUNIT_ASSERT_MSG(ret != RHINO_SUCCESS, "ret=%d", ret);
    aos_sem_free(&tmp_sem);

    /* aos_queue_recv invalid param test */
#if 0
	// TODO: coredump
	ret = aos_queue_recv(NULL, 10, &recv_msg, &recv_size);
	YUNIT_ASSERT(ret == RHINO_NULL_PTR);
#endif

    ret = aos_queue_recv(&queue, 0, &recv_msg, &recv_size);
    YUNIT_ASSERT_MSG(ret == RHINO_SUCCESS, "ret=%d", ret);

    ret = aos_queue_recv(&queue, AOS_WAIT_FOREVER, NULL, &recv_size);
    YUNIT_ASSERT_MSG(ret != RHINO_SUCCESS, "ret=%d", ret);

    ret = aos_queue_recv(&queue, AOS_WAIT_FOREVER, &recv_msg, NULL);
    YUNIT_ASSERT_MSG(ret != RHINO_SUCCESS, "ret=%d", ret);

    aos_sem_new(&tmp_sem, 0);
    ret = aos_queue_recv((aos_queue_t *)&tmp_sem, AOS_WAIT_FOREVER, &recv_msg,
                         &recv_size);
    YUNIT_ASSERT_MSG(ret != RHINO_SUCCESS, "ret=%d", ret);
    aos_sem_free(&tmp_sem);
    aos_queue_free(&queue);

    /* aos_queue_free invalid param test */
#if 0
	aos_queue_free(NULL);
#endif
#if 0
	aos_sem_new(&tmp_sem, 0);
	aos_queue_free((aos_queue_t*)&tmp_sem);
	YUNIT_ASSERT_MSG(ret==RHINO_KOBJ_TYPE_ERR, "ret=%d", ret);
//	aos_sem_free(&tmp_sem); // already free
#endif
}


static void TASK_aosapi_kernel_queue_recv(void *arg)
{
    int ret;
    int i = 1;
    for (; i < 10; i++) {
        memset(&recv_msg, 0, sizeof(recv_msg));
        ret = aos_queue_recv(&g_queue, 500, &recv_msg, &recv_size);
        YUNIT_ASSERT_MSG(ret == RHINO_SUCCESS, "ret=%d", ret);
        YUNIT_ASSERT_MSG(recv_size == TEST_QUEUE_MAX_MSG_SIZE, "recv_size=%d",
                         recv_size);
        YUNIT_ASSERT_MSG(recv_msg.id == i, "recv_msg.id=%d", i);
        YUNIT_ASSERT_MSG(recv_msg.len == 5, "recv_msg.leb=5");
        YUNIT_ASSERT_MSG(strcmp(recv_msg.msg, "hello") == 0, "recv_msg.msg=%s",
                         "hello");
    }
    aos_sem_signal(&sync_sem);
}
static void TASK_aosapi_kernel_queue_send(void *arg)
{
    int ret;
    int i = 1;
    for (; i < 10; i++) {
        memset(&send_msg, 0, sizeof(send_msg));
        send_msg.id  = i;
        send_msg.len = 5;
        strcpy(send_msg.msg, "hello");
        ret = aos_queue_send(&g_queue, &send_msg, sizeof(send_msg));
        YUNIT_ASSERT_MSG(ret == RHINO_SUCCESS, "ret=%d", ret);
        aos_msleep(50);
    }
    aos_sem_signal(&sync_sem);
}
static void CASE_aosapi_kernel_queue_send_recv()
{
    int ret;

    ret = aos_sem_new(&sync_sem, 0);
    YUNIT_ASSERT_MSG(ret == RHINO_SUCCESS, "ret=%d", ret);

    // TODO: nullptr coredump
    ret = aos_queue_new(&g_queue, queue_buf, TEST_QUEUE_SIZE,
                        TEST_QUEUE_MAX_MSG_SIZE);
    YUNIT_ASSERT_MSG(ret == RHINO_SUCCESS, "ret=%d", ret);
    assert(ret == RHINO_SUCCESS);

    ret =
      aos_task_new("TASK_aosapi_kernel_queue_send_testcase",
                   TASK_aosapi_kernel_queue_send, NULL, TEST_TASK_STACK_SIZE);
    YUNIT_ASSERT_MSG(ret == RHINO_SUCCESS, "ret=%d", ret);
    ret =
      aos_task_new("TASK_aosapi_kernel_queue_recv_testcase",
                   TASK_aosapi_kernel_queue_recv, NULL, TEST_TASK_STACK_SIZE);
    YUNIT_ASSERT_MSG(ret == RHINO_SUCCESS, "ret=%d", ret);
    assert(ret == RHINO_SUCCESS);

    ret = aos_sem_wait(&sync_sem, AOS_WAIT_FOREVER);
    YUNIT_ASSERT_MSG(ret == RHINO_SUCCESS, "ret=%d", ret);

    ret = aos_sem_wait(&sync_sem, AOS_WAIT_FOREVER);
    YUNIT_ASSERT_MSG(ret == RHINO_SUCCESS, "ret=%d", ret);

    aos_sem_free(&sync_sem);
    aos_queue_free(&g_queue);
}

static void CASE_aosapi_kernel_queue_full()
{
    int ret, i;
    // create fail
    ret = aos_queue_new(&g_queue, queue_buf, TEST_QUEUE_SIZE,
                        TEST_QUEUE_MAX_MSG_SIZE);
    YUNIT_ASSERT_MSG(ret == RHINO_SUCCESS, "ret=%d", ret);

    for (i = 0; i < TEST_QUEUE_MAX_MSG_COUNT - 1; i++) {
        ret = aos_queue_send(&g_queue, &send_msg, sizeof(send_msg));
        YUNIT_ASSERT_MSG(ret == RHINO_SUCCESS, "ret=%d", ret);
    }
    ret = aos_queue_send(&g_queue, &send_msg, sizeof(send_msg));
    YUNIT_ASSERT_MSG(ret != RHINO_SUCCESS, "ret=%d", ret);

    aos_queue_free(&g_queue);
}


void aosapi_kernel_queue_test_entry(yunit_test_suite_t *suite)
{
    yunit_add_test_case(suite, "kernel.queue.param",
                        CASE_aosapi_kernel_queue_param);
    //	yunit_add_test_case(suite, "kernel.queue.sendrecv",
    //CASE_aosapi_kernel_queue_send_recv);
    yunit_add_test_case(suite, "kernel.queue.full",
                        CASE_aosapi_kernel_queue_full);

    (void)CASE_aosapi_kernel_queue_send_recv;
}
