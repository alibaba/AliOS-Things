/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef U_RES_H
#define U_RES_H

#include <k_api.h>

#define RES_MSG_MAGIC        0x68686868
#define RES_MSG_ARG_MAX_CNT  2

#define REQUEST_MALLOC       0x01
#define REQUEST_FREE         0x02
#define REQUEST_EXIT         0x03

#define RES_QUEUE_MSG_NUM    2

typedef struct {
    unsigned int magic;
    unsigned char type;
    unsigned char arg_cnt;
    unsigned char has_ret;
    unsigned char resv;
    unsigned int arg[RES_MSG_ARG_MAX_CNT];
} res_request_msg_t;

typedef struct {
    unsigned int magic;
    unsigned int ret_val;
} res_reply_msg_t;

void* u_res_malloc(int pid, size_t len);

void u_res_free(int pid, void *ptr);

void u_res_kill(int pid);

#endif /* U_RES_H */

