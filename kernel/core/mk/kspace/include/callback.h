/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef U_CALLBACK_H
#define U_CALLBACK_H

#include "task_group.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_ARG_CNT  (8)

typedef struct cb_call_msg {
    void     *func_ptr;
    uint16_t  arg_cnt;
    uint16_t  has_ret;
    void     *arg_value[MAX_ARG_CNT];
} cb_call_msg_t;

typedef struct cb_ret_msg {
    void *ret_val;
} cb_ret_msg_t;

int cb_call_buf_queue_push(void *buf_queue, cb_call_msg_t *msg);
int cb_ret_buf_queue_pop(void *buf_queue, cb_ret_msg_t *msg, size_t *size);

int  cb_init(task_group_t *group);
int cb_exit(task_group_t *group);

#ifdef __cplusplus
}
#endif

#endif /* U_CALLBACK_H */

