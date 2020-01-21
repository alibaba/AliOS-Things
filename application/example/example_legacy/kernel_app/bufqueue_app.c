/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>

#include <k_api.h>
#include <ulog/ulog.h>

/**
 * This case demonstrates the use of message queues, creating two tasks where task1
 * sends messages every second and task2 receives them and prints them.
 */

/* module name used by ulog */
#define MODULE_NAME "bufqueue_app"

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

/* buf queue resource */
#define MESSAGE_MAX_LENGTH 10 /* maximum message length */

/* Static memory for static creation */
static kbuf_queue_t buf_queue_handle;                          /* buf queue struct */
static char         buf_queue_buffer[MESSAGE_MAX_LENGTH * 10]; /* for the internal buffer of the buf queue */

/* task entry */
static void task1_entry(void *arg)
{
    uint32_t i;
    kstat_t  status;
    tick_t   sleep_time;

    char     message_buf[MESSAGE_MAX_LENGTH]; /* buffer used to send message */
    uint8_t  message_id = 0;

    sleep_time = krhino_ms_to_ticks(1000); /* 1000ms to ticks */

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
        status = krhino_buf_queue_send(&buf_queue_handle, (void *)message_buf, sizeof(message_buf));
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
    /* The buffer must be greater than or equal to the maximum message length */
    char     message_buf[MESSAGE_MAX_LENGTH];
    uint32_t rev_size;

    while (1) {
        /**
         * receive message. The task will wait until it receives the message.
         * rev_size is set to the actual length of the received message.
         */
       status = krhino_buf_queue_recv(&buf_queue_handle, RHINO_WAIT_FOREVER, (void *)message_buf, &rev_size);
       if (status ==  RHINO_SUCCESS) {
           /* show message data */
           LOGI(MODULE_NAME, "recv message : ");
           for (i = 0; i < rev_size; i++) {
               printf("%d", message_buf[i]);
           }
           printf("\r\n");
       } else {
           LOGE(MODULE_NAME, "recv buf queue error");
       }
    }
}

void bufqueue_app(void)
{
    kstat_t status;

    /* create buf queues statically */
    status = krhino_buf_queue_create(&buf_queue_handle, "message queue", (void *)buf_queue_buffer,
                                     sizeof(buf_queue_buffer), MESSAGE_MAX_LENGTH);
    if (status != RHINO_SUCCESS) {
        LOGE(MODULE_NAME, "create buf queue error");
        return;
    }

    /**
     * Task1 is a producer that produces a set of data every second.
     */
    status = krhino_task_dyn_create(&task1_tcb, TASK1_NAME, (void *)0, TASK1_PRI, 50,
                                    TASK1_STACKSIZE, task1_entry, 1);
    if (status != RHINO_SUCCESS) {
        krhino_buf_queue_del(&buf_queue_handle);
        LOGE(MODULE_NAME, "create task1 error");
        return;
    }

    /**
     * Task2 is the consumer that processes the data sent by task1.
     */
    status = krhino_task_dyn_create(&task2_tcb, TASK2_NAME, (void *)0, TASK2_PRI, 50,
                                    TASK2_STACKSIZE, task2_entry, 1);
    if (status != RHINO_SUCCESS) {
        krhino_buf_queue_del(&buf_queue_handle);
        LOGE(MODULE_NAME, "create task2 error");
        return;
    }
}

