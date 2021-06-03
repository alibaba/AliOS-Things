/*!
 * @file amp_at.h
 *
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#ifndef _AMP_AT_H_
#define _AMP_AT_H_

#include "at.h"

int amp_at_init(int at_uart_port, uint32_t baud_rate, char *send_delimiter);
int amp_at_register_callback(const char *prefix, at_recv_cb cb);
int amp_at_read(char *outbuf, int readsize);
int amp_at_send_wait_reply(const char *cmd, int cmdlen, bool delimiter,
                       const char *data, int datalen, const char *expectrsp);

#endif