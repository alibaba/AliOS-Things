/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <stdio.h>

#include <k_api.h>
#include <ulog/ulog.h>

/* This precompiled macro is for different configurations and is not required for practical use. */
#if (RHINO_CONFIG_QUEUE > 0)
/**
 * This case demonstrates the use of queues, creating two tasks where task1
 * sends messages every second and task2 receives them and prints them.
 * This case does the same thing as the buf queue case. Queue is more
 * efficient than buf queue, which sends a pointer but buf queue will
 * copy the contents.
 */

/* module name used by ulog */
#define MODULE_NAME "queue_app"

/* task1 parameters */
#define TASK1_NAME      "task1"
#define TASK1_PRI       50
#define TASK1_STACKSIZE 512

/* task2 parameters */
#define TASK2_NAME      "task2"
#define TASK2_PRI       50
#define TASK2_STACKSIZE 512

/* task tcb pointer */
static ktask_t *task1_tcb = NULL;
static ktask_t *task2_tcb = NULL;

/* queue length */
#define MESSAGE_LENGTH      10 /* message length */
#define TEST_QUEUE_MSG_SIZE 30 /* queue length */

/* Static memory for static creation */
kqueue_t test_queue;
void    *test_queue_msg[TEST_QUEUE_MSG_SIZE]; /* used as an internal buffer for queue */

/* task entry */
static void task1_entry(void *arg)
{
    uint32_t i;
    kstat_t  status;
    tick_t   sleep_time;
    char    *message_buf;

    uint8_t  message_id = 0;

    sleep_time = krhino_ms_to_ticks(1000); /* 1000ms to ticks */

    while (1) {
        message_buf = malloc(MESSAGE_LENGTH); /* malloc buffer used to send message */

        /**
         * generate message. The sequence of messages is as follows:
         * 0123456789
         * 1234567890
         * 2345678901
         * ......
         */
        for (i = 0; i < MESSAGE_LENGTH; i++) {
            message_buf[i] = (message_id + i) % 10;
        }
        message_id++;

        /* send message. the message is a pointer */
        status = krhino_queue_back_send(&test_queue, message_buf);
        if (status != RHINO_SUCCESS) {
            LOGE(MODULE_NAME, "send buf queue error");
        }

        krhino_task_sleep(sleep_time); /* sleep 1000ms */
    }
}

/* task entry */
static void task2_entry(void *arg)
{
    uint32_t i;
    kstat_t  status;
    char    *message_buf = NULL;

    while (1) {
        /**
         * receive message. The task will wait until it receives the message.
         */
        status = krhino_queue_recv(&test_queue, RHINO_WAIT_FOREVER, (void**)&message_buf);
        if (status == RHINO_SUCCESS) {
            /* show message data */
            LOGI(MODULE_NAME, "recv message : ");
            for (i = 0; i < MESSAGE_LENGTH; i++) {
                printf("%d", message_buf[i]);
            }
            printf("\r\n");
            if (message_buf != NULL) {
                /**
                 * free the buffer. It is best to free up memory where it is malloced.
                 * Be aware of memory leaks if you do not free memory at the malloc location.
                 */
                free(message_buf);
                message_buf = NULL;
            }
        } else {
            LOGE(MODULE_NAME, "recv buf queue error");
        }
    }
}

void queue_app(void)
{
    kstat_t status;

    /* create queues statically */
    status = krhino_queue_create(&test_queue, "test_queue",
                                 (void **)&test_queue_msg, TEST_QUEUE_MSG_SIZE);
    if (status != RHINO_SUCCESS) {
        LOGE(MODULE_NAME, "create queue error");
        return;
    }

    /**
     * Task1 is a producer that produces a set of data every second.
     */
    status = krhino_task_dyn_create(&task1_tcb, TASK1_NAME, (void *)0, TASK1_PRI, 50,
                                    TASK1_STACKSIZE, task1_entry, 1);
    if (status != RHINO_SUCCESS) {
        krhino_queue_del(&test_queue);
        LOGE(MODULE_NAME, "create task1 error");
        return;
    }

    /**
     * Task2 is the consumer that processes the data sent by task1.
     */
    status = krhino_task_dyn_create(&task2_tcb, TASK2_NAME, (void *)0, TASK2_PRI, 50,
                                    TASK2_STACKSIZE, task2_entry, 1);
    if (status != RHINO_SUCCESS) {
        krhino_queue_del(&test_queue);
        LOGE(MODULE_NAME, "create task2 error");
        return;
    }
}

#endif

