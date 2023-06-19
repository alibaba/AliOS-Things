/*
 * Copyright (C) 2015-2023 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>

#include <aos/kernel.h>
#include <assert.h>

#include "yunit/yunit.h"

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
    bool is_valid;

    /* aos_queue_new invalid param test */
#if 0
	// TODO: coredump
	ret = aos_queue_new(NULL, queue_buf, TEST_QUEUE_SIZE, TEST_QUEUE_MAX_MSG_SIZE);
	YUNIT_ASSERT_MSG(ret==RHINO_NULL_PTR, "ret=%d", ret);
#endif
    ret = aos_queue_new(&queue, NULL, TEST_QUEUE_SIZE, TEST_QUEUE_MAX_MSG_SIZE);
    YUNIT_ASSERT_MSG(ret == 0, "ret=%d", ret);

    is_valid = aos_queue_is_valid(&queue);
    YUNIT_ASSERT_MSG(is_valid==true, "is_valid=%d", is_valid);

    ret = aos_queue_new(&queue, queue_buf, 0, TEST_QUEUE_MAX_MSG_SIZE);
    YUNIT_ASSERT_MSG(ret == 0, "ret=%d", ret);

    ret = aos_queue_new(&queue, queue_buf, TEST_QUEUE_SIZE, 0);
    YUNIT_ASSERT_MSG(ret == 0, "ret=%d", ret);

    /* aos_queue_send invalid param test */
#if 0
	// TODO: coredump
	ret = aos_queue_send(NULL, &send_msg, sizeof(send_msg));
	YUNIT_ASSERT_MSG(ret==RHINO_NULL_PTR, "ret=%d", ret);
#endif

    // create fail
    ret = aos_queue_new(&queue, queue_buf, TEST_QUEUE_SIZE,
                        TEST_QUEUE_MAX_MSG_SIZE);
    YUNIT_ASSERT_MSG(ret == 0, "ret=%d", ret);

    ret = aos_queue_send(&queue, NULL, sizeof(send_msg));
    YUNIT_ASSERT_MSG(ret == 0, "ret=%d", ret);

    ret = aos_queue_send(&queue, &send_msg, 0);
    YUNIT_ASSERT_MSG(ret == 0, "ret=%d", ret);

    ret = aos_queue_send(&queue, &send_msg, TEST_QUEUE_MAX_MSG_SIZE);
    YUNIT_ASSERT_MSG(ret == 0, "ret=%d", ret);

    ret = aos_sem_new(&tmp_sem, 0);
    YUNIT_ASSERT_MSG(ret == 0, "ret=%d", ret);
    ret = aos_queue_send((aos_queue_t *)&tmp_sem, &send_msg, sizeof(send_msg));
    YUNIT_ASSERT_MSG(ret == 0, "ret=%d", ret);
    aos_sem_free(&tmp_sem);

    /* aos_queue_recv invalid param test */
#if 0
	// TODO: coredump
	ret = aos_queue_recv(NULL, 10, &recv_msg, &recv_size);
	YUNIT_ASSERT(ret == RHINO_NULL_PTR);
#endif

    ret = aos_queue_recv(&queue, 0, &recv_msg, &recv_size);
    YUNIT_ASSERT_MSG(ret == 0, "ret=%d", ret);

    ret = aos_queue_recv(&queue, AOS_WAIT_FOREVER, NULL, &recv_size);
    YUNIT_ASSERT_MSG(ret != 0, "ret=%d", ret);

    ret = aos_queue_recv(&queue, AOS_WAIT_FOREVER, &recv_msg, NULL);
    YUNIT_ASSERT_MSG(ret != 0, "ret=%d", ret);

    aos_sem_new(&tmp_sem, 0);
    ret = aos_queue_recv((aos_queue_t *)&tmp_sem, AOS_WAIT_FOREVER, &recv_msg,
                         &recv_size);
    YUNIT_ASSERT_MSG(ret != 0, "ret=%d", ret);
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
        YUNIT_ASSERT_MSG(ret == 0, "ret=%d", ret);
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
        YUNIT_ASSERT_MSG(ret == 0, "ret=%d", ret);
        aos_msleep(50);
    }
    aos_sem_signal(&sync_sem);
}
static void CASE_aosapi_kernel_queue_send_recv()
{
    int ret;

    ret = aos_sem_new(&sync_sem, 0);
    YUNIT_ASSERT_MSG(ret == 0, "ret=%d", ret);

    // TODO: nullptr coredump
    ret = aos_queue_new(&g_queue, queue_buf, TEST_QUEUE_SIZE,
                        TEST_QUEUE_MAX_MSG_SIZE);
    YUNIT_ASSERT_MSG(ret == 0, "ret=%d", ret);
    assert(ret == 0);

    ret =
      aos_task_new("TASK_aosapi_kernel_queue_send_testcase",
                   TASK_aosapi_kernel_queue_send, NULL, TEST_TASK_STACK_SIZE);
    YUNIT_ASSERT_MSG(ret == 0, "ret=%d", ret);
    ret =
      aos_task_new("TASK_aosapi_kernel_queue_recv_testcase",
                   TASK_aosapi_kernel_queue_recv, NULL, TEST_TASK_STACK_SIZE);
    YUNIT_ASSERT_MSG(ret == 0, "ret=%d", ret);
    assert(ret == 0);

    ret = aos_sem_wait(&sync_sem, AOS_WAIT_FOREVER);
    YUNIT_ASSERT_MSG(ret == 0, "ret=%d", ret);

    ret = aos_sem_wait(&sync_sem, AOS_WAIT_FOREVER);
    YUNIT_ASSERT_MSG(ret == 0, "ret=%d", ret);

    aos_sem_free(&sync_sem);
    aos_queue_free(&g_queue);
}

static void CASE_aosapi_kernel_queue_full()
{
    int ret, i;
    // create fail
    ret = aos_queue_new(&g_queue, queue_buf, TEST_QUEUE_SIZE,
                        TEST_QUEUE_MAX_MSG_SIZE);
    YUNIT_ASSERT_MSG(ret == 0, "ret=%d", ret);

    for (i = 0; i < TEST_QUEUE_MAX_MSG_COUNT - 1; i++) {
        ret = aos_queue_send(&g_queue, &send_msg, sizeof(send_msg));
        YUNIT_ASSERT_MSG(ret == 0, "ret=%d", ret);
    }
    ret = aos_queue_send(&g_queue, &send_msg, sizeof(send_msg));
    YUNIT_ASSERT_MSG(ret == 0, "ret=%d", ret);

    aos_queue_free(&g_queue);
}

/**
 * 该示例使用消息队列实现任务间数据同步，具体场景为创建任务A和认为B，以及一消息队列。\n\r
 * 任务A作为生产者循环向消息队列发送消息，任务B作为消费者循环从消息队列接收消息，
 * 一般情况下，消费者处理数据是要花费很长时间，所以会导致消息产生的速度大于消息处理的速度，使得消息队列溢出。
 * 所以可以通过调整任务B优先级大于任务A来避免这种情况，或者使用信号量来控制数据收发同步。
 * 示例说明如下：
 * 1. t0时刻，任务T调用aos_queue_new()创建一互斥量。任务T然后调用aos_task_create()创建任务A和任务B，任务A优先级设置为31，任务B优先级设置为30。任务B因消息队列无消息而阻塞，任务A得到运行向消息队列发送消息。
 * 2. t1时刻，任务B因从消息队列读取到消息而解除阻塞，任务B对消息进行处理后继续等待新的消息到来。
 * 3. t2时刻，任务A向消息队列发送消息。
 * 4. t3时刻，重复t1时刻的操作。
 */

/* module name used by log print */
#define MODULE_NAME "aos_queue_test"

/* taskA parameters */
#define TASKA_NAME      "taskA"
#define TASKA_PRIO       31
#define TASKA_STACKSIZE 4096

/* taskB parameters */
#define TASKB_NAME      "taskB"
#define TASKB_PRIO       30
#define TASKB_STACKSIZE 4096

/* queue resource */
#define MESSAGE_MAX_LENGTH 10 /* maximum message length */

/* Static memory for static creation */
static aos_queue_t queue_handle;                          /* queue handle */
static char        queue_buffer[MESSAGE_MAX_LENGTH * 10]; /* for the internal buffer of the queue */
static uint8_t  message_id = 0;

static int send_cnt = 0;
static int recv_cnt = 0;

/* task entry for taskA*/
static void taskA_entry(void *arg)
{
    uint32_t      i;
    aos_status_t  status;

    char     message_buf[MESSAGE_MAX_LENGTH]; /* buffer used to send message */

    int test_count = 10;

    while (test_count--) {
        /**
         * generate message. The sequence of messages is as follows:
         * 0123456789
         * 1234567890
         * 2345678901
         * ......
         */
        for (i = 0; i < sizeof(message_buf); i++) {
            message_buf[i] = (message_id + i) % 10;
        }
        message_id++;

        /* send message. The message length must not exceed the maximum message length */
        status = aos_queue_send(&queue_handle, (void *)message_buf, sizeof(message_buf));
        if (status != 0) {
            printf("[%s]send buf queue error\n", MODULE_NAME);
            continue;
        }
        send_cnt++;
        aos_msleep(10); /* sleep 1000ms */
    }
}

/* task entry for taskB*/
static void taskB_entry(void *arg)
{
    uint32_t      i;
    aos_status_t  status;
    /* The buffer must be greater than or equal to the maximum message length */
    char          message_buf[MESSAGE_MAX_LENGTH];
    size_t        rev_size = 0;

    int test_count = 10;

    while (test_count--) {
        /**
         * receive message. The task will wait until it receives the message.
         * rev_size is set to the actual length of the received message.
         */
        status = aos_queue_recv(&queue_handle, AOS_WAIT_FOREVER, (void *)message_buf, &rev_size);
        if (status == 0) {
            /* show message data */
            printf("[%s]%d recv message : ", MODULE_NAME, rev_size);
            if (rev_size != MESSAGE_MAX_LENGTH) {
                continue;
            }
            for (i = 0; i < rev_size; i++) {
                printf("%d", message_buf[i]);
                if (message_buf[i] != (((message_id - 1) + i) % 10)) {
                    printf("\r\n");
                    continue;
                }

            }
            printf("\r\n");
            recv_cnt++;
        } else {
            printf("[%s]recv buf queue error\n", MODULE_NAME);
        }

    }
}

static int aos_queue_test(void)
{
    aos_status_t  status;
    aos_task_t    taskA_handle;
    aos_task_t    taskB_handle;

    recv_cnt = 0;
    send_cnt = 0;
    /**
     * create a queue.
     * queue:   queue_handle(aos_queue_t struct variable)
     * buf:     queue_buffer(for the internal buffer of the queue)
     * size:    sizeof(queue_buffer) is the length of buf
     * max_msg: MESSAGE_MAX_LENGTH(maximum message length, here is 10 byte)
     */
    status = aos_queue_new(&queue_handle, (void *)queue_buffer, sizeof(queue_buffer),
                           MESSAGE_MAX_LENGTH);
    if (status != 0) {
        printf("[%s]create queue error\n", MODULE_NAME);
        return -1;
    }

    /* TaskA is a producer that produces a set of data every second. */
    status = aos_task_create(&taskA_handle, TASKA_NAME, taskA_entry, NULL, NULL, TASKA_STACKSIZE, TASKA_PRIO, AOS_TASK_AUTORUN);
    if (status != 0) {
        aos_queue_free(&queue_handle);
        printf("[%s]create %s error\n", MODULE_NAME, TASKA_NAME);
        return -1;
    }

    /* TaskB is the consumer that processes the data sent by taskA. */
    status = aos_task_create(&taskB_handle, TASKB_NAME, taskB_entry, NULL, NULL, TASKB_STACKSIZE, TASKB_PRIO, AOS_TASK_AUTORUN);
    if (status != 0) {
        aos_queue_free(&queue_handle);
        printf("[%s]create %s error\n", MODULE_NAME, TASKB_NAME);
        return -1;
    }

    aos_msleep(500); /* sleep 500ms */
    if ((recv_cnt == 10) && (send_cnt == 10)) {
        return 0;
    }

    return -1;
}

void kernel_test_queue(void)
{
    YUNIT_ASSERT(aos_queue_test() == 0);
}

void aosapi_kernel_queue_test_entry(yunit_test_suite_t *suite)
{
    yunit_add_test_case(suite, "kernel.queue.new", kernel_test_queue);
    yunit_add_test_case(suite, "kernel.queue.param",
                        CASE_aosapi_kernel_queue_param);
    //	yunit_add_test_case(suite, "kernel.queue.sendrecv",
    //CASE_aosapi_kernel_queue_send_recv);
    yunit_add_test_case(suite, "kernel.queue.full",
                        CASE_aosapi_kernel_queue_full);

    (void)CASE_aosapi_kernel_queue_send_recv;
}
