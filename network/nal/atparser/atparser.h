/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _AT_PARSER_H_
#define _AT_PARSER_H_

#include <stdbool.h>

#ifdef AOS_ATCMD
#include <atcmd_config_platform.h>
#include <atcmd_config_module.h>
#endif

#ifndef bool
#define bool unsigned char
#endif

#ifndef true
#define true 1
#endif
#ifndef false
#define false 0
#endif

typedef struct {
    char *reply_prefix;
    char *reply_success_postfix;
    char *reply_fail_postfix;
} atcmd_config_t;

typedef void (*at_recv_cb)(void *arg, char *buf, int buflen);

/**
 * initialization
 * 
 */
int at_init();

/**
 * at send and wait reply
 *
 * @param data sending buf.
 * @param datalen sending length.
 * @param delimiter whether sending delimiter, usually value is true
 * @param replybuf reply buffer.
 * @param bufsize reply buffer size
 * @param atcmdconfig AT cmd reply format config. Use default if NULL 
 */
int at_send_wait_reply(const char *data, int datalen, bool delimiter,
                       char *replybuf, int bufsize,
                       const atcmd_config_t *atcmdconfig);

/**
 * at send and does not wait reply
 *
 * @param data sending buffer.
 * @param datalen sending length.
 * @param delimiter whether sending delimiter, usually value is false
 */
int at_send_no_reply(const char *data, int datalen, bool delimiter);


/**
 * at read for certain bytes of data
 *
 * @param outbuf output buffer.
 * @param readsize read size.
 */
int at_read(char *outbuf, int readsize);


/**
 * at register callback for recv
 *
 * @param prefix interested string.
 * @param postfix intersted postfix.
 * @param maxlen max recv data len
 * @param cb callback handle function
 * @param arg callback handle function args
 */
int at_register_callback(const char *prefix, const char *postfix,
                         int maxlen, at_recv_cb cb, void *arg);


/**
 * at yield receive function. Only used in single task scenario
 *
 * @param replybuf reply buffer.
 * @param bufsize reply buffer size.
 * @param atcmdconfig AT cmd reply format config. Use default if NULL
 * @param timeout_ms receive timeout in millisecond
 */
int at_yield(char *replybuf, int bufsize, const atcmd_config_t *atcmdconfig,
             int timeout_ms);
#endif
