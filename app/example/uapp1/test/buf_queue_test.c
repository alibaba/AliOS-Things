/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <u_api.h>

#define STACK_SIZE 0x400

static ktask_t     *bufq_obj;
static cpu_stack_t *stack;

#define MSG_SIZE  10
#define MSG_NUM   50

static kbuf_queue_t static_bufq;
static char         bufq_buf[MSG_SIZE*MSG_NUM];

static kbuf_queue_t *dynamic_bufq;

#define LOOP_ROUND 10

static ksem_t *bufq_sem;
static bool    bufq_ready;
static bool    bufq_recv_done;
static bool    bufq_send_exit;
static int     send_crc;
static int     recv_crc;

static void static_bufq_run(void *arg)
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
        krhino_sem_take(bufq_sem, RHINO_WAIT_FOREVER);

        if (bufq_send_exit)
            break;

        recv_buf = malloc(MSG_SIZE + 1);
        if (NULL == recv_buf) {
            printf("malloc recv_buf failr\r\n");
            break;
        }
        memset(recv_buf, 0, MSG_SIZE+1);

        krhino_buf_queue_recv(&static_bufq,
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

    krhino_utask_del(krhino_cur_task_get());
}

static void dynamic_bufq_run(void *arg)
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
        krhino_sem_take(bufq_sem, RHINO_WAIT_FOREVER);

        if (bufq_send_exit)
            break;

        recv_buf = malloc(MSG_SIZE + 1);
        if (NULL == recv_buf) {
            printf("malloc recv_buf failr\r\n");
            break;
        }
        memset(recv_buf, 0, MSG_SIZE+1);

        krhino_buf_queue_recv(dynamic_bufq, RHINO_WAIT_FOREVER, recv_buf, &num);
        if (num > MSG_SIZE) {
            printf("%s: reev buf overflow, reev 0x%x bytes\r\n", __func__, num);
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

    krhino_utask_del(krhino_cur_task_get());
}

int static_bufq_test(void)
{
    kstat_t  stat;
    int      ret      = 0;
    char    *send_buf = NULL;

    bufq_ready     = 0;
    bufq_send_exit = 0;

    stat = krhino_sem_dyn_create(&bufq_sem, "bufq_sem", 0);
    if (stat != RHINO_SUCCESS) {
        printf("%s: allocate sem failed\r\n", __func__);
        return -1;
    }

    stat = krhino_fix_buf_queue_create(&static_bufq, "bufq_test", bufq_buf,
                                       MSG_SIZE, MSG_NUM);
    if (stat != RHINO_SUCCESS) {
        free(stack);
        ret = -2;
        goto out1;
    }

    stack = malloc(STACK_SIZE*sizeof(cpu_stack_t));
    if (stack == NULL) {
        printf("%s: alloc stack failed\r\n", __func__);
        ret = -3;
        goto out2;
    }

    stat = krhino_utask_create(&bufq_obj, "bufq_test", 0,
                               AOS_DEFAULT_APP_PRI - 1, (tick_t)0,
                               stack, STACK_SIZE, STACK_SIZE,
                               (task_entry_t)static_bufq_run, 1);

    if (stat != RHINO_SUCCESS) {
        printf("create buf queue task failed, ret 0x%x\r\n",
                stat);
        free(stack);
        ret = -4;
        goto out1;
    }

    printf("----- static buf queue test -----\r\n");

    bufq_ready = 1;
    krhino_task_sleep(1);

    char ch  = 'a';
    send_crc = 0;
    for (int i = 0; i < LOOP_ROUND; i++) {
        send_buf = malloc(MSG_SIZE + 1);
        if (NULL == send_buf) {
            printf("malloc send_buf fail\r\n");
            krhino_sem_give(bufq_sem);
            bufq_send_exit=1;
            ret = -5;
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
        krhino_buf_queue_send(&static_bufq, send_buf, MSG_SIZE);
        free(send_buf);
        send_buf = NULL;
        krhino_sem_give(bufq_sem);

        if (bufq_recv_done == 1)
            break;
    }

    while (bufq_recv_done == 0) {
        krhino_task_sleep(1);
    }

    if (send_crc == recv_crc) {
        ret =0;
    } else {
        ret = -6;
    }

    printf("send_crc 0x%x recv_crc 0x%x\r\n", send_crc, recv_crc);

out2:
    stat = krhino_buf_queue_del(&static_bufq);
    if (RHINO_SUCCESS != stat) {
        printf("%s: buf queue del failed, err code %d\r\n",
               __func__, stat);
        ret = -7;
    }

out1:
    stat = krhino_sem_dyn_del(bufq_sem);
    if (RHINO_SUCCESS != stat) {
        printf("%s: sem del failed, err code %d\r\n",
               __func__, stat);
        ret = -8;
    }

    return ret;
}

int dynamic_bufq_test(void)
{
    kstat_t  stat;
    int      ret      = 0;
    char    *send_buf = NULL;

    bufq_ready     = 0;
    bufq_send_exit = 0;

    stat = krhino_sem_dyn_create(&bufq_sem, "bufq_sem", 0);
    if (stat != RHINO_SUCCESS) {
        printf("%s: allocate sem failed\r\n", __func__);
        return -9;
    }

    stat = krhino_buf_queue_dyn_create(&dynamic_bufq, "bufq_test",
                                       MSG_SIZE * MSG_NUM, 0xffff);
    if (stat != RHINO_SUCCESS) {
        printf("%s: dynamic buf queue create failed\r\n", __func__);
        free(stack);
        ret = -10;
        goto out1;
    }

    stack = malloc(STACK_SIZE*sizeof(cpu_stack_t));
    if (stack == NULL) {
        printf("%s: alloc stack failed\r\n", __func__);
        ret = -11;
        goto out2;
    }

    stat = krhino_utask_create(&bufq_obj, "bufq_test", 0,
                               AOS_DEFAULT_APP_PRI - 1, (tick_t)0,
                               stack, STACK_SIZE, STACK_SIZE,
                               (task_entry_t)dynamic_bufq_run, 1);

    if (stat != RHINO_SUCCESS) {
        printf("create buf queue task failed, ret 0x%x\r\n", stat);
        free(stack);
        ret = -12;
        goto out1;
    }

    printf("----- dynamic buf queue test -----\r\n");

    bufq_ready = 1;
    krhino_task_sleep(1);

    char ch  = 'a';
    send_crc = 0;
    for (int i = 0; i < LOOP_ROUND; i++) {
        send_buf = malloc(MSG_SIZE + 1);
        if (NULL == send_buf) {
            printf("malloc send_buf fail\r\n");
            krhino_sem_give(bufq_sem);
            bufq_send_exit=1;
            ret = -13;
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
        krhino_buf_queue_send(dynamic_bufq, send_buf, MSG_SIZE);
        free(send_buf);
        send_buf = NULL;
        krhino_sem_give(bufq_sem);

        if (bufq_recv_done == 1)
            break;
    }

    while (bufq_recv_done == 0) {
        krhino_task_sleep(1);
    }

    if (send_crc == recv_crc) {
        ret =0;
    } else {
        ret = -14;
    }

    printf("send_crc 0x%x recv_crc 0x%x\r\n", send_crc, recv_crc);

out2:
    stat = krhino_buf_queue_dyn_del(dynamic_bufq);
    if (RHINO_SUCCESS != stat) {
        printf("%s: dynamic buf queue del failed, err code %d\r\n",
               __func__, stat);
        ret = -15;
    }

out1:
    stat = krhino_sem_dyn_del(bufq_sem);
    if (RHINO_SUCCESS != stat) {
        printf("%s: sem del failed, err code %d\r\n", __func__, stat);
        ret = -16;
    }

    return ret;
}

int buf_queue_test(void)
{
    int ret;

    ret = static_bufq_test();
    if (ret)
        return ret;

    return dynamic_bufq_test();

}
