/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _AT_PARSER_H_
#define _AT_PARSER_H_

#include <stdarg.h>
#include <hal/soc/soc.h>
#include <aos/aos.h>

#ifdef AOS_ATCMD
#include <hal/atcmd.h>
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

typedef struct oob_s {
    char *prefix;
    char *postfix;
    char *oobinputdata;
    uint32_t reallen;
    uint32_t maxlen;
    oob_cb cb;
    void *arg;
} oob_t;

/*
* --> | slist | --> | slist | --> NULL
*     ---------     ---------
*     | smhr  |     | smpr  |
*     ---------     ---------
*     | rsp   |     | rsp   |
*     ---------     ---------
*/
typedef struct at_task_s {
    slist_t next;
    aos_sem_t smpr;
    char *command;
    char *rsp;
    char *rsp_prefix;
    char *rsp_success_postfix;
    char *rsp_fail_postfix;
    uint32_t rsp_prefix_len;
    uint32_t rsp_success_postfix_len;
    uint32_t rsp_fail_postfix_len;
    uint32_t rsp_offset;
    uint32_t rsp_len;
} at_task_t;

typedef enum {
    NORMAL = 0,
    ASYN
} at_mode_t;

typedef enum {
    AT_SEND_RAW = 0,
    AT_SEND_PBUF
}at_send_t;

/**
* Parser structure for parsing AT commands
*/
typedef struct {
    /// used only internally
    uart_dev_t *_pstuart;
    int _timeout;
    char *_default_recv_prefix;
    char *_default_recv_success_postfix;
    char *_default_recv_fail_postfix;
    char *_send_delimiter;
    int _recv_prefix_len;
    int _recv_success_postfix_len;
    int _recv_fail_postfix_len;
    int _send_delim_size;
    oob_t _oobs[OOB_MAX];
    int _oobs_num;
    aos_mutex_t at_mutex;
    aos_mutex_t at_uart_send_mutex;
    aos_mutex_t task_mutex;
    
    at_mode_t _mode;

    // can be used externally
    slist_t task_l;

    /**
    * initialization
    *
    * @param u uart port used for AT communication.
    * @param send_delimiter string of characters to use as line delimiters for sending
    * @param recv_delimiter string of characters to use as line delimiters for receiving
    * @param timeout timeout of the connection
    */
    int (*init)(const char *recv_prefix, const char *recv_success_postfix,
                    const char *recv_fail_postfix, const char *send_delimiter, int timeout);

    void (*set_mode)(at_mode_t m);

    void (*set_timeout)(int timeout);

    void (*set_recv_delimiter)(const char *recv_prefix, const char *recv_success_postfix, const char *recv_fail_postfix);

    void (*set_send_delimiter)(const char *delimiter);

    int (*send_raw_self_define_respone_formate)(const char *command, char *rsp, uint32_t rsplen,
                                                   char *rsp_prefix, char *rsp_success_postfix, char *rsp_fail_postfix);
    /*
    * This is a blocking API. It hanbles raw command sending, then is blocked
    * to wait for response.
    *
    * This API sends raw command (prepared by caller) and wait for raw response.
    * The caller is responsible for preparing the ready-to-send command
    * as well as parsing the response result. The caller is also responsible
    * for allocating/freeing rsp buffer.
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
    int (*send_data_2stage)(const char *fst, const char *data, 
                            uint32_t len, char *rsp, uint32_t rsplen);
    /**
    * Write a single byte to the buffer.
    */
    int (*putch)(char c);

    /**
    * Get a single byte from the buffer.
    */
    int (*getch)(char *c);

    /**
    * Write an array of bytes to the underlying stream.
    */
    int (*write)(const char *data, int size);

    /**
    * Read an array of bytes from the underlying stream.
    */
    int (*read)(char *data, int size);

    /**
    * Attach a callback for out-of-band data.
    */
    void (*oob)(const char *prefix, const char *postfix, int maxlen, 
                    oob_cb cb, void *arg);
} at_parser_t;

extern at_parser_t at;
#endif
