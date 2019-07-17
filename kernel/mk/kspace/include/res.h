/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef RES_H
#define RES_H

#include <stddef.h>
#include "task_group.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RES_QUEUE_MSG_NUM   2
#define RES_MSG_ARG_MAX_CNT 2

#define REQUEST_MALLOC 0x01
#define REQUEST_FREE   0x02
#define REQUEST_EXIT   0x03

typedef struct {
    uint8_t  type;
    uint8_t  arg_cnt;
    uint8_t  has_ret;
    uint8_t  resv;
    void    *arg[RES_MSG_ARG_MAX_CNT];
} res_request_msg_t;

typedef struct {
    void *ret_val;
} res_reply_msg_t;

void *res_malloc(int pid, size_t len);
void  res_free(int pid, void *ptr);
int   res_init(task_group_t *group);
void  res_destory(task_group_t *group);

#ifdef __cplusplus
}
#endif

#endif /* RES_H */

