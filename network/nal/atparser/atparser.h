/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _AT_PARSER_H_
#define _AT_PARSER_H_

#include <aos/aos.h>

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

#define OOB_MAX 5

typedef void (*oob_cb)(void *arg, char *buf, int buflen);

typedef struct oob_s
{
    char *   prefix;
    char *   postfix;
    char *   oobinputdata;
    uint32_t reallen;
    uint32_t maxlen;
    oob_cb   cb;
    void *   arg;
} oob_t;

/*
 * --> | slist | --> | slist | --> NULL
 *     ---------     ---------
 *     | smhr  |     | smpr  |
 *     ---------     ---------
 *     | rsp   |     | rsp   |
 *     ---------     ---------
 */
typedef struct at_task_s
{
    slist_t   next;
    aos_sem_t smpr;
    char *    command;
    char *    rsp;
    char *    rsp_prefix;
    char *    rsp_success_postfix;
    char *    rsp_fail_postfix;
    uint32_t  rsp_prefix_len;
    uint32_t  rsp_success_postfix_len;
    uint32_t  rsp_fail_postfix_len;
    uint32_t  rsp_offset;
    uint32_t  rsp_len;
} at_task_t;

/**
 * Parser structure for parsing AT commands
 */
typedef struct
{
    /// used only internally
    uart_dev_t *_pstuart;
    int         _timeout;
    char *      _default_recv_prefix;
    char *      _default_recv_success_postfix;
    char *      _default_recv_fail_postfix;
    char *      _send_delimiter;
    int         _recv_prefix_len;
    int         _recv_success_postfix_len;
    int         _recv_fail_postfix_len;
    int         _send_delim_size;
    oob_t       _oobs[OOB_MAX];
    int         _oobs_num;
    aos_mutex_t at_uart_recv_mutex;
    aos_mutex_t at_uart_send_mutex;
    aos_mutex_t task_mutex;

    // can be used externally
    slist_t task_l;

    int (*send_raw_self_define_respone_formate)(const char *command, char *rsp,
                                                uint32_t rsplen,
                                                char *   rsp_prefix,
                                                char *   rsp_success_postfix,
                                                char *   rsp_fail_postfix);
    /*
     * This is a blocking API. It hanbles raw command sending, then is blocked
     * to wait for response.
     *
     * This API sends raw command (prepared by caller) and wait for raw
     * response. The caller is responsible for preparing the ready-to-send
     * command as well as parsing the response result. The caller is also
     * responsible for allocating/freeing rsp buffer.
     */
    int (*send_raw)(const char *command, char *rsp, uint32_t rsplen);

    /*
     * This is a blocking API. It hanbles data sending, it inside follows
     * below steps:
     *    1. Send first line (with send_delimeter);
     *    2. Waiting for prompt symbol, usually '>' character;
     *    3. Send data with 'len' length, and without send_delimeter.
     *
     * This API sends prefix command and data, and wait for raw response.
     * The caller is responsible for preparing the ready-to-send first line
     * as well as parsing the response result. The caller is also responsible
     * for allocating/freeing rsp buffer.
     */
    int (*send_data_2stage)(const char *fst, const char *data, uint32_t len,
                            char *rsp, uint32_t rsplen);

    /*
     * This API can be used to send packet, without response required.
     *
     * AT stream format as below:
     *     [<header>,]data[,<tailer>]
     *
     * In which, header and tailer is optional.
     */
    int (*send_data_3stage_no_rsp)(const char *header, const uint8_t *data,
                                   uint32_t len, const char *tailer);

    /*
     * This API is used, usually by athost, to send stream content without
     * response required. The content is usually status event, such as
     * YEVENT:MONITOR_UP/MONITOR_DOWN, etc.
     */
    int (*send_raw_no_rsp)(const char *content);

    /**
     * Write an array of bytes to the underlying stream.
     */
    int (*write)(const char *data, int size);

    /**
     * Read an array of bytes from the underlying stream.
     */
    int (*read)(char *data, int size);

    /**
     * Alien name for read to avoid conflict with socket read macro
     */
    int (*parse)(char *data, int size);

    /**
     * Attach a callback for out-of-band data.
     */
    void (*oob)(const char *prefix, const char *postfix, int maxlen, oob_cb cb,
                void *arg);
} at_parser_t;

extern at_parser_t at;


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
 * @param cmd sending buf.
 * @param replybuf reply buffer.
 * @param bufsize reply buffer size
 * @param atcmdconfig AT cmd reply format config. Use default if NULL 
 */
int _at_send_wait_reply(const char *cmd, char *replybuf, int bufsize,
                        const atcmd_config_t *atcmdconfig);

/**
 * at send and does not wait reply
 *
 * @param data sending buffer.
 * @param datalen sending length.
 */
int _at_send_no_reply(const char *data, int datalen);


/**
 * at read for certain bytes of data
 *
 * @param outbuf output buffer.
 * @param readsize read size.
 */
int _at_read(char *outbuf, int readsize);


/**
 * at register callback for recv
 *
 * @param prefix interested string.
 * @param postfix intersted postfix.
 * @param maxlen max recv data len
 * @param cb callback handle function
 * @param arg callback handle function args
 */
int _at_register_callback(const char *prefix, const char *postfix,
                         int maxlen, at_recv_cb cb, void *arg);

#endif
