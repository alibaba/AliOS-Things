/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdint.h>

#include <aos/kernel.h>
#include <ulog/ulog.h>

/**
 * This case demonstrates the use of queue, creating two tasks where task1
 * sends messages every second and task2 receives them and prints them.
 */

/* module name used by ulog */
#define MODULE_NAME "queue_app"

/* task1 parameters */
#define TASK1_NAME      "task1"
#define TASK1_PRI       50
#define TASK1_STACKSIZE 1024

/* task2 parameters */
#define TASK2_NAME      "task2"
#define TASK2_PRI       50
#define TASK2_STACKSIZE 1024

/* queue resource */
#define MESSAGE_MAX_LENGTH 10 /* maximum message length */

/* Static memory for static creation */
static aos_queue_t queue_handle;                          /* queue handle */
static char        queue_buffer[MESSAGE_MAX_LENGTH * 10]; /* for the internal buffer of the queue */

/* task entry */
static void task1_entry(void *arg)
{
    uint32_t i;
    int      status;

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
            LOGE(MODULE_NAME, "send buf queue error");
        }

        aos_msleep(1000); /* sleep 1000ms */
    }
}

/* task entry */
static void task2_entry(void *arg)
{
    uint32_t i;
    int      status;
    /* The buffer must be greater than or equal to the maximum message length */
    char     message_buf[MESSAGE_MAX_LENGTH];
    uint32_t rev_size;

    while (1) {
        /**
         * receive message. The task will wait until it receives the message.
         * rev_size is set to the actual length of the received message.
         */
        status = aos_queue_recv(&queue_handle, AOS_WAIT_FOREVER, (void *)message_buf, &rev_size);
        if (status == 0) {
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

void queue_app(void)
{
    int status;

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
        LOGE(MODULE_NAME, "create queue error");
        return;
    }

    /* Task1 is a producer that produces a set of data every second. */
    status = aos_task_new(TASK1_NAME, task1_entry, NULL, TASK1_STACKSIZE);
    if (status != 0) {
        aos_queue_free(&queue_handle);
        LOGE(MODULE_NAME, "create task1 error");
        return;
    }

    /* Task2 is the consumer that processes the data sent by task1. */
    status = aos_task_new(TASK2_NAME, task2_entry, NULL, TASK2_STACKSIZE);
    if (status != 0) {
        aos_queue_free(&queue_handle);
        LOGE(MODULE_NAME, "create task2 error");
        return;
    }
}

