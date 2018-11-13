/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <u_api.h>

#define BUFQ_STACK_SIZE 0x400

static ktask_t     bufq_obj;
static cpu_stack_t bufq_stack[BUFQ_STACK_SIZE];

#define MSG_SIZE  10
#define MSG_NUM   50

static char         bufq_buf[MSG_SIZE*MSG_NUM];
static kbuf_queue_t bufq;

#define LOOP_ROUND 10

static ksem_t bufq_sem;
static bool   bufq_ready;
static bool   bufq_recv_done;
static bool   bufq_send_exit;
static int    send_crc;
static int    recv_crc;

static void bufq_run(void *arg)
{
    unsigned int  num;
    char         *recv_buf;

    arg            = arg;
    bufq_recv_done = 0;
    recv_crc       = 0;
    num            = 0;
    recv_buf       = NULL;

    while (bufq_ready == 0) {
        krhino_task_sleep(1);
    }

    for (int i = 0; i < LOOP_ROUND; i++) {
        krhino_sem_take(&bufq_sem, RHINO_WAIT_FOREVER);

        if (bufq_send_exit)
            break;

        recv_buf = malloc(MSG_SIZE + 1);
        if (NULL == recv_buf) {
            printf("malloc recv_buf failr\r\n");
            break;
        }
        memset(recv_buf, 0, MSG_SIZE+1);

        krhino_buf_queue_recv(&bufq,
                              RHINO_WAIT_FOREVER,
                              recv_buf, &num);
        if (num > MSG_SIZE) {
            printf("reev buf overflow, reev 0x%x bytes\r\n", num);
            break;
        }

        for (int j = 0; j < num; j++) {
            recv_crc += recv_buf[j];
        }
        recv_buf[num] = 0;
        printf("\t\t\trecv: %s\r\n", recv_buf);
        free(recv_buf);
        recv_buf = NULL;
    }

    bufq_recv_done = 1;

    while (1) {
        krhino_task_sleep(1000);
    }
}

int buf_queue_test(void)
{
    kstat_t  stat;
    int      ret      = 0;
    char    *send_buf = NULL;

    bufq_ready     = 0;
    bufq_send_exit = 0;

    stat = krhino_sem_create(&bufq_sem, "bufq_sem", 0);
    if (stat != RHINO_SUCCESS) {
        ret = -1;
        goto out;
    }

    stat = krhino_utask_create(&bufq_obj, "bufq_test", 0,
                               AOS_DEFAULT_APP_PRI - 1, (tick_t)0,
                               bufq_stack, BUFQ_STACK_SIZE, BUFQ_STACK_SIZE,
                               (task_entry_t)bufq_run, 1);

    if (stat != RHINO_SUCCESS) {
        printf("create buf queue task failed, ret 0x%x\r\n",
                stat);
        return -2;
    }

    stat = krhino_fix_buf_queue_create(&bufq, "bufq_test", bufq_buf,
                                       MSG_SIZE, MSG_NUM);
    if (stat != RHINO_SUCCESS) {
        ret = -3;
        goto out;
    }

    bufq_ready = 1;
    krhino_task_sleep(1);

    char ch  = 'a';
    send_crc = 0;
    for (int i = 0; i < LOOP_ROUND; i++) {
        send_buf = malloc(MSG_SIZE + 1);
        if (NULL == send_buf) {
            printf("malloc send_buf fail\r\n");
            krhino_sem_give(&bufq_sem);
            bufq_send_exit=1;
            ret = -4;
            break;
        }
        memset(send_buf, 0, MSG_SIZE+1);

        for (int j = 0; j < MSG_SIZE; j++, ch++) {
            if (ch > 'z')
                ch = 'a';

            send_buf[j] = ch;
            send_crc += ch;
        }
        send_buf[MSG_SIZE] = 0;
        printf(" send: %s\r\n", send_buf);
        krhino_buf_queue_send(&bufq, send_buf, MSG_SIZE);
        free(send_buf);
        send_buf = NULL;
        krhino_sem_give(&bufq_sem);

        if (bufq_recv_done == 1)
            break;
    }

    while (bufq_recv_done == 0) {
        krhino_task_sleep(1);
    }

    stat = krhino_buf_queue_del(&bufq);
    if (RHINO_SUCCESS != stat) {
        printf("buf queue del failed, err code %d\r\n", stat);
        ret = -5;
        goto out;
    }

    stat = krhino_sem_del(&bufq_sem);
    if (RHINO_SUCCESS != stat) {
        printf("sem del failed, err code %d\r\n", stat);
        ret = -6;
        goto out;
    }

    printf("buf queue: send_crc 0x%x recv_crc 0x%x\r\n",
           send_crc, recv_crc);

    if (send_crc == recv_crc) {
        ret =0;
    } else {
        ret = -10;
    }

out:
    return ret;
}

