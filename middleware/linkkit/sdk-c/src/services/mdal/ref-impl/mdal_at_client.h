/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef _MDAL_AT_CLIENT_H_
#define _MDAL_AT_CLIENT_H_

#ifndef AT_RECV_PREFIX
#define AT_RECV_PREFIX "\r\n"
#endif

#ifndef AT_RECV_SUCCESS_POSTFIX
#define AT_RECV_SUCCESS_POSTFIX "OK\r\n"
#endif

#ifndef AT_RECV_FAIL_POSTFIX
#define AT_RECV_FAIL_POSTFIX "ERROR\r\n"
#endif

#ifndef AT_SEND_DELIMITER
#define AT_SEND_DELIMITER "\r"
#endif

typedef void (*oob_cb)(void *arg, char *buf, int buflen);

int  mdal_at_client_init();
int  mdal_at_client_oob(const char *prefix, const char *postfix, int maxlen,
                  oob_cb cb, void *arg);
int  mdal_at_client_write(const char* at_cmd);
#endif
