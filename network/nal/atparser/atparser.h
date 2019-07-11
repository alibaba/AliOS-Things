/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _AT_PARSER_H_
#define _AT_PARSER_H_

#include <stdbool.h>
#include <stdint.h>

/*
 * AT dev type
 */
typedef enum {
    AT_DEV_UART = 0,
     /* add more types here */
    AT_DEV_TYPE_MAX
} at_dev_type_t;

typedef struct {
    char *reply_prefix;                 /* reply prefix */
    char *reply_success_postfix;        /* reply postfix indicating success */
    char *reply_fail_postfix;           /* reply postfix indicating failure */
} at_reply_config_t;

typedef struct {
    uint8_t            port;                 /* dev port. Compulsory */
    at_dev_type_t      type;                 /* dev type. Compulsory */
    void               *dev_cfg;             /* dev config. Compulsory. For UART, see uart_config_t in hal/uart.h */
    at_reply_config_t  reply_cfg;            /* AT receive prefix and postfix. Compulsory. */
    char               *send_delimiter;      /* AT sending delimiter between command and data. Optional, "\r" */
    uint32_t           timeout_ms;           /* AT send or receive timeout in millisecond. Optional, 1000 ms by defaut */
    uint8_t            send_wait_prompt;     /* whether AT send waits prompt before sending data. Optional, 0 by default */
    uint32_t           prompt_timeout_ms;    /* AT send wait prompt timeout in millisecond. Optional, 200 ms by default */
    uint8_t            send_data_no_wait;    /* whether AT send waits response after sending data. Optional, 0 by default */
    int                recv_task_priority;   /* AT recv task priority. Optional, 32 by default*/
    int                recv_task_stacksize;  /* AT recv task stacksize. Optional, 1K by default */
} at_config_t;

#ifndef bool
#define bool unsigned char
#endif

#ifndef true
#define true 1
#endif
#ifndef false
#define false 0
#endif

typedef void (*at_recv_cb)(void *arg, char *buf, int buflen);

/**
 * Initialization for AT parser module
 *
 * @return success: 0; fail: -1
 */
int at_init(void);

/**
 * Deinitialization for AT parser module
 *
 * @return success: 0; fail: -1
 */
int at_deinit(void);

/**
 * Add a new AT device 
 *
 * @param config AT config (e.g. port number, AT send delimiter). 
 *               See definition for at_config_t. MUST not be NULL.
 *
 * @return success: fd for AT device; fail: -1
 */
int at_add_dev(at_config_t *config);

/**
 * Delete AT device by fd
 *
 * @param fd fd for AT device
 *
 * @return success: 0; fail: -1
 */
int at_delete_dev(int fd);

/**
 * AT send (format: command + delimiter + data) and wait reply
 *
 * @param fd fd for AT device
 * @param cmd AT command sending buf. MUST not be NULL.
 * @param cmdlen AT command length.
 * @param delimiter whether sending delimiter, usually value is true
 * @param data data sending buf. NULL if no data.
 * @param datalen data length. Zero if no data.
 * @param replybuf reply buffer. MUST not be NULL.
 * @param bufsize reply buffer size
 * @param atcmdconfig AT cmd reply format config. Use default if NULL 
 * 
 * @return success: 0; fail: -1
 */
int at_send_wait_reply(int fd, const char *cmd, int cmdlen, bool delimiter,
                       const char *data, int datalen,
                       char *replybuf, int bufsize,
                       const at_reply_config_t *atcmdconfig);

/**
 * AT send (format: data + delimiter) and does not wait reply
 *
 * @param fd fd for AT device
 * @param data sending buffer.
 * @param datalen sending length.
 * @param delimiter whether sending delimiter, usually value is false
 *
 * @return success: 0; fail: -1
 */
int at_send_no_reply(int fd, const char *data, int datalen, bool delimiter);


/**
 * AT read for certain bytes of data
 *
 * @param fd fd for AT device
 * @param outbuf output buffer.
 * @param readsize read size.
 *  
 * @return success: read length; fail: -1
 */
int at_read(int fd, char *outbuf, int readsize);


/**
 * AT register callback for recv
 *
 * @param fd fd for AT device
 * @param prefix interested string.
 * @param postfix intersted postfix.
 * @param bufsize recv buffer len.
 * @param bufsize recv buffer len.
 * @param cb callback handle function.
 * @param arg callback handle function args.
 * 
 * @return success: 0; fail: -1
 */
int at_register_callback(int fd, const char *prefix, const char *postfix, char *recvbuf,
                         int bufsize, at_recv_cb cb, void *arg);


/**
 * AT yield receive function. Only used in single task scenario
 *
 * @param fd fd for AT device.
 * @param replybuf reply buffer.
 * @param bufsize reply buffer size.
 * @param atcmdconfig AT cmd reply format config. Use default if NULL.
 * @param timeout_ms receive timeout in millisecond.
 *
 * @return success: 0; fail: -1
 */
int at_yield(int fd, char *replybuf, int bufsize, const at_reply_config_t *atcmdconfig,
             int timeout_ms);
#endif
