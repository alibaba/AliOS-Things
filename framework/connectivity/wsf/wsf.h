/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef WSF_H
#define WSF_H

#include "connectivity.h"
#include "os.h"

#define MODULE_NAME "wsf"
#define ALINK_BUF_SIZE      (CONFIG_MAX_MESSAGE_LENGTH)
#define ALINK_PARAMS_SIZE   (ALINK_BUF_SIZE - 512)//TODO: ??

void wsf_init_buff(uint8_t **, uint8_t **);
int wsf_connect(void);
int wsf_disconnect(void);
int wsf_send_async(void *pdata, int len, void *(*cb)(connectivity_rsp_t *rsp,
                                                     void *), void *arg);
connectivity_rsp_t *wsf_send(void *, int);
int wsf_add_listener(connectivity_cb func);
int wsf_del_listener(connectivity_cb func);
int wsf_set_state(int st);

#endif
