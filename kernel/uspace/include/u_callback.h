/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef U_CALLBACK_H
#define U_CALLBACK_H

#include <k_api.h>
#include <u_task.h>

#define MAX_ARG_CNT  (6)

#define MSG_CALL_MAGIC    (0xf38ca84c)
#define MSG_RET_MAGIC     (0xc48ac83f)

#define MAX_ARG_CNT (6)

typedef struct cb_call_msg {
    uint32_t  magic;
    void     *func_ptr;
    uint16_t  arg_cnt;
    uint16_t  has_ret;
    uint32_t arg_value[MAX_ARG_CNT];
} cb_call_msg_t;

typedef struct cb_ret_msg {
    uint32_t magic;
    uint32_t ret_val;
} cb_ret_msg_t;

int32_t get_cb_queue_id(void);
kstat_t cb_call_buf_queue_push(kbuf_queue_t *buf_queue, cb_call_msg_t *msg);
kstat_t cb_ret_buf_queue_pop(kbuf_queue_t *buf_queue, cb_ret_msg_t *msg, size_t *size);
kstat_t create_cb_buf_queues(ktask_t *task);
kstat_t delete_cb_buf_queues(task_group_t *group);

#endif /* U_CALLBACK_H */

