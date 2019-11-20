/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "posix/mqueue.h"

#if (POSIX_CONFIG_MQUEUE_ENABLE > 0)

mqd_t mq_open(const char *name, int oflag, ...)
{
    kbuf_queue_t *mq;
    kstat_t       ret;

    ret = krhino_buf_queue_dyn_create(&mq, "buf_queue", DEFAULT_MQUEUE_SIZE, DEFAULT_MAX_MSG_SIZE);
    if (ret != RHINO_SUCCESS) {
        return 0;
    }

    return mq;
}

int mq_close(mqd_t mqdes)
{
    return krhino_buf_queue_dyn_del(mqdes);
}

ssize_t mq_receive(mqd_t mqdes, char *msg_ptr, size_t msg_len, unsigned *msg_prio)
{
    size_t  msg_size;
    kstat_t ret;

    *msg_prio = 0;
    ret = krhino_buf_queue_recv(mqdes, 0, msg_ptr, &msg_size);
    if (ret != RHINO_SUCCESS) {
        return 0;
    }

    return msg_size;
}

int mq_send(mqd_t mqdes, const char *msg_ptr, size_t msg_len, unsigned msg_prio)
{
    kstat_t ret;

    ret = krhino_buf_queue_send((kbuf_queue_t *)mqdes, (void *)msg_ptr, msg_len);
    if (ret != RHINO_SUCCESS) {
        return 0;
    }

    return msg_len;
}

int mq_setattr(mqd_t mqdes, const struct mq_attr *mqstat, struct mq_attr *omqstat)
{
    return 0;
}

int mq_getattr(mqd_t mqdes, struct mq_attr *mqstat)
{
    return 0;
}

ssize_t mq_timedreceive(mqd_t mqdes, char *msg_ptr, size_t msg_len,
                        unsigned *msg_prio, const struct timespec *abs_timeout)
{
    size_t  msg_size;
    kstat_t ret;
    tick_t  ticks;

    *msg_prio = 0;

    ticks = abs_timeout->tv_sec * RHINO_CONFIG_TICKS_PER_SECOND +
            (abs_timeout->tv_nsec * RHINO_CONFIG_TICKS_PER_SECOND) / NANOSECONDS_PER_SECOND;

    ret = krhino_buf_queue_recv(mqdes, ticks, msg_ptr, &msg_size);
    if (ret != RHINO_SUCCESS) {
        return 0;
    }

    return msg_size;
}

int mq_timedsend(mqd_t mqdes, const char *msg_ptr, size_t msg_len,
                 unsigned msg_prio, const struct timespec *abs_timeout)
{
    kstat_t ret;

    (void)msg_prio;

    ret = krhino_buf_queue_send((kbuf_queue_t *)mqdes, (void *)msg_ptr, msg_len);
    if (ret != RHINO_SUCCESS) {
        return 0;
    }

    return msg_len;
}

#endif
