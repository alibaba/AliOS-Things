/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdint.h>

#include <aos/kernel.h>

#if AOS_COMP_CLI
#include "aos/cli.h"
#endif

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
#define MODULE_NAME "aos_queue_example"

/* taskA parameters */
#define TASKA_NAME      "taskA"
#define TASKA_PRIO       31
#define TASKA_STACKSIZE 1024

/* taskB parameters */
#define TASKB_NAME      "taskB"
#define TASKB_PRIO       30
#define TASKB_STACKSIZE 1024

/* queue resource */
#define MESSAGE_MAX_LENGTH 10 /* maximum message length */

/* Static memory for static creation */
static aos_queue_t queue_handle;                          /* queue handle */
static char        queue_buffer[MESSAGE_MAX_LENGTH * 10]; /* for the internal buffer of the queue */

/* task entry for taskA*/
static void taskA_entry(void *arg)
{
    uint32_t      i;
    aos_status_t  status;

    char     message_buf[MESSAGE_MAX_LENGTH]; /* buffer used to send message */
    uint8_t  message_id = 0;

    while (1) {
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
        }

        aos_msleep(1000); /* sleep 1000ms */
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

    while (1) {
        /**
         * receive message. The task will wait until it receives the message.
         * rev_size is set to the actual length of the received message.
         */
        status = aos_queue_recv(&queue_handle, AOS_WAIT_FOREVER, (void *)message_buf, &rev_size);
        if (status == 0) {
            /* show message data */
            printf("[%s]%d recv message : ", MODULE_NAME, rev_size);
            for (i = 0; i < rev_size; i++) {
                printf("%d", message_buf[i]);
            }
            printf("\r\n");
        } else {
            printf("[%s]recv buf queue error\n", MODULE_NAME);
        }
    }
}

static void aos_queue_example(int argc, char **argv)
{
    aos_status_t  status;
    aos_task_t    taskA_handle;
    aos_task_t    taskB_handle;

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
        return;
    }

    /* TaskA is a producer that produces a set of data every second. */
    status = aos_task_create(&taskA_handle, TASKA_NAME, taskA_entry, NULL, NULL, TASKA_STACKSIZE, TASKA_PRIO, AOS_TASK_AUTORUN);
    if (status != 0) {
        aos_queue_free(&queue_handle);
        printf("[%s]create %s error\n", MODULE_NAME, TASKA_NAME);
        return;
    }

    /* TaskB is the consumer that processes the data sent by taskA. */
    status = aos_task_create(&taskB_handle, TASKB_NAME, taskB_entry, NULL, NULL, TASKB_STACKSIZE, TASKB_PRIO, AOS_TASK_AUTORUN);
    if (status != 0) {
        aos_queue_free(&queue_handle);
        printf("[%s]create %s error\n", MODULE_NAME, TASKB_NAME);
        return;
    }
}

#if AOS_COMP_CLI
/* reg args: fun, cmd, description*/
ALIOS_CLI_CMD_REGISTER(aos_queue_example, queue_example, aos queue example)
#endif