/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>

#include <k_api.h>
#include <ulog/ulog.h>

/* This precompiled macro is for different configurations and is not required for practical use. */
#if ((RHINO_CONFIG_TIMER > 0) && (RHINO_CONFIG_BUF_QUEUE > 0))
/**
 * This case demonstrates the use of timer.
 * This case does the same thing as the buf queue case. The difference is that
 * the case creates a timer and a task, where timer1 is the producer and task1 is the consumer.
 */

/* module name used by ulog */
#define MODULE_NAME "timer_app"

/* task1 parameters */
#define TASK1_NAME      "task1"
#define TASK1_PRI       50
#define TASK1_STACKSIZE 512

/* task tcb pointer */
static ktask_t *task1_tcb = NULL;

/* buf queue resource */
#define MESSAGE_MAX_LENGTH 10 /* maximum message length */

/* Static memory for buf queue */
static kbuf_queue_t buf_queue_handle;                          /* buf queue struct */
static char         buf_queue_buffer[MESSAGE_MAX_LENGTH * 10]; /* for the internal buffer of the buf queue */

/* Static memory for timer */
static ktimer_t  timer1;

/* timer callback function.
 * Warning: An interface that causes a block is not allowed to be called within this function.
 * Blocking within this function will cause other timers to run incorrectly.
 */
static void timer1_func(void *timer, void *arg)
{
    uint32_t i;
    kstat_t  status;

    char            message_buf[MESSAGE_MAX_LENGTH]; /* buffer used to send message */
    /*  */
    static uint8_t  message_id = 0;

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
}

/* task entry */
static void task1_entry(void *arg)
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
        if (status == RHINO_SUCCESS) {
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

void timer_app(void)
{
    kstat_t status;
    tick_t   timer_period;


    /* create buf queues statically */
    status = krhino_buf_queue_create(&buf_queue_handle, "message queue", (void *)buf_queue_buffer,
                                     sizeof(buf_queue_buffer), MESSAGE_MAX_LENGTH);
    if (status != RHINO_SUCCESS) {
        LOGE(MODULE_NAME, "create buf queue error");
        return;
    }

    timer_period = krhino_ms_to_ticks(1000); /* 1000ms to ticks */

    /**
     * create timer statically. some of the parameters are as follows:
     * first:   timer_period (the time when the timer first fires)
     * round:   timer_period (the cycle of the timer)
     * autorun: 0 (not activated when the timer is created, call krhino_timer_start to start the timer)
     */
    status = krhino_timer_create(&timer1, "timer1", timer1_func, timer_period, timer_period, NULL, 0);
    if (status != RHINO_SUCCESS) {
        LOGE(MODULE_NAME, "create timer error");
        return;
    }

    /* start the timer */
    status = krhino_timer_start(&timer1);
    if (status != RHINO_SUCCESS) {
        LOGE(MODULE_NAME, "start timer error");
        return;
    }

    /**
     * Task1 is the consumer that processes the data sent by timer.
     */
    status = krhino_task_dyn_create(&task1_tcb, TASK1_NAME, (void *)0, TASK1_PRI, 50,
                                    TASK1_STACKSIZE, task1_entry, 1);
    if (status != RHINO_SUCCESS) {
        /* stop the timer before deleting it */
        krhino_timer_stop(&timer1);
        krhino_timer_del(&timer1);

        krhino_buf_queue_del(&buf_queue_handle);
        LOGE(MODULE_NAME, "create task1 error");
        return;
    }
}

#endif

