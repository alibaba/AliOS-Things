/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef _SAL_UTIL_H_
#define _SAL_UTIL_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    void    *buffer;
    void    *buf_sem;
    uint32_t length;
    uint32_t head;
    uint32_t tail;
} sal_ringbuf_t;

void *HAL_QueueCreate(void *buf, unsigned int size, int max_msg);

void HAL_QueueDestroy(void *queue);

int HAL_QueueSend(void *queue, void *msg, unsigned int size);

int HAL_QueueRecv(void *queue, unsigned int ms, void *msg,
                  unsigned int *size);

void *HAL_QueueBufPtr(void *queue);

#ifdef __cplusplus
}
#endif

#endif  /* _SAL_UTIL_H_ */