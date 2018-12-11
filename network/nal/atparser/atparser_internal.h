/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef _ATPARSER_export_H_
#define _ATPARSER_export_H_

typedef struct {
    char *reply_prefix,
    char *reply_success_postfix,
    char *reply_fail_postfix
} at_reply_format_t;

typedef void (*at_recv_cb)(void *arg, char *buf, int buflen);

/**
 * initialization
 *
 * @param replyformat AT reply format definition.
 * @param send_delimiter string of characters to use as line delimiters for
 * sending
 * @param send/recv timeout timeout of uart layer
 */
int at_init(const at_reply_format_t *replyformat, 
            const char *send_delimiter, int timeout);

/**
 * at send and wait reply
 *
 * @param cmd sending buf.
 * @param replybuf reply buffer.
 * @param bufsize reply buffer size
 * @param replyformat AT reply format definition
 */
int at_send_wait_reply(const char *cmd, char *replybuf, int bufsize,
                       const at_reply_format_t *replyformat);

/**
 * at send and does not wait reply
 *
 * @param data sending buffer.
 * @param datalen sending length.
 */
int at_send_no_reply(const char *data, int datalen);


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

#endif
