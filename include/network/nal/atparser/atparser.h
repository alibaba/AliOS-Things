/*!
 * @file atparser.h
 *
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _AT_PARSER_H_
#define _AT_PARSER_H_

#include <stdbool.h>
#include <stdint.h>

/** @addtogroup aos_atparser atparser
 *  AT parser, which is used to ease the operation of AT modules.
 *
 *  @{
 */

/**
 * AT dev type
 */
typedef enum {
    AT_DEV_UART = 0,
    /* add more types here when necessary */
    AT_DEV_TYPE_MAX
} at_dev_type_t;

/**
 * The reply format of the AT commands.
 */
typedef struct {
    char *reply_prefix;                 //!< reply prefix
    char *reply_success_postfix;        //!< reply postfix indicating success
    char *reply_fail_postfix;           //!< reply postfix indicating failure
} at_reply_config_t;

/**
 * The config for the AT device.
 */
typedef struct {
    uint8_t            port;                 //!< dev port. Compulsory 
    at_dev_type_t      type;                 //!< dev type. Compulsory 
    void               *dev_cfg;             //!< dev config. Compulsory. For UART, see uart_config_t in hal/uart.h 
    at_reply_config_t  reply_cfg;            //!< AT receive prefix and postfix. Compulsory. 
    char               *send_delimiter;      //!< AT sending delimiter between command and data. Optional, "\r" 
    uint32_t           timeout_ms;           //!< AT send or receive timeout in millisecond. Optional, 1000 ms by defaut 
    uint8_t            send_wait_prompt;     //!< whether AT send waits prompt before sending data. Optional, 0 by default 
    uint32_t           prompt_timeout_ms;    //!< AT send wait prompt timeout in millisecond. Optional, 200 ms by default 
    uint8_t            send_data_no_wait;    //!< whether AT send waits response after sending data. Optional, 0 by default 
    int                recv_task_priority;   //!< AT recv task priority. Optional, 32 by default
    int                recv_task_stacksize;  //!< AT recv task stacksize. Optional, 1K by default 
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

/**
 * The is the defition of the AT oob callback function.
 *
 * @param[in]  arg     The arguments for the callback function.
 * @param[in]  buf     The buffer to store the result when user uses
 *                     prefix and postfix to match a patterned AT event,
 *                     e.g. domain-to-ip event on some platforms.
 * @param[in]  buflen  The length of the buffer.
 *
 * @return none.
 */
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
 * @param[in]  config  AT config (e.g. port number, AT send delimiter). 
 *                     See definition for at_config_t. MUST not be NULL.
 *
 * @return success: fd for AT device; fail: -1
 */
int at_add_dev(at_config_t *config);

/**
 * Delete AT device by fd
 *
 * @param[in]  fd  fd for AT device
 *
 * @return success: 0; fail: -1
 */
int at_delete_dev(int fd);

/**
 * AT send (format: command + delimiter + data) and wait reply
 *
 * @param[in]      fd           fd for AT device
 * @param[in]      cmd          AT command sending buf. MUST not be NULL.
 * @param[in]      cmdlen       AT command length.
 * @param[in]      delimiter    whether sending delimiter, usually value is true
 * @param[in]      data data    sending buf. NULL if no data.
 * @param[in]      datalen      data length. Zero if no data.
 * @param[in,out]  replybuf     reply buffer. MUST not be NULL.
 * @param[in]      bufsize      reply buffer size
 * @param[in]      atcmdconfig  AT cmd reply format config. Use default if NULL 
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
 * @param[in]  fd         fd for AT device
 * @param[in]  data       sending buffer.
 * @param[in]  datalen    sending length.
 * @param[in]  delimiter  whether sending delimiter, usually value is false
 *
 * @return success: 0; fail: -1
 */
int at_send_no_reply(int fd, const char *data, int datalen, bool delimiter);


/**
 * AT read for certain bytes of data
 *
 * @param[in]      fd        fd for AT device
 * @param[in,out]  outbuf    output buffer.
 * @param[in]      readsize  read size.
 *  
 * @return success: read length; fail: -1
 */
int at_read(int fd, char *outbuf, int readsize);


/**
 * AT register callback for recv
 *
 * @param[in]      fd       fd for AT device
 * @param[in]      prefix   interested string.
 * @param[in]      postfix  intersted postfix.
 * @param[in,out]  recvbuf  recv buffer.
 * @param[in]      bufsize  recv buffer len.
 * @param[in]      cb       callback handle function.
 * @param[in]      arg      callback handle function args.
 * 
 * @return success: 0; fail: -1
 * 
 * @note
 * - The recvbuf and bufsize must be present if postfix is used,
 *   which are used to store the result and length if any data
 *   that match the prefix & postfix pattern.
 */
int at_register_callback(int fd, const char *prefix, const char *postfix, char *recvbuf,
                         int bufsize, at_recv_cb cb, void *arg);


/**
 * AT yield receive function. Only used in single task scenario
 *
 * @param[in]      fd           fd for AT device.
 * @param[in,out]  replybuf     reply buffer.
 * @param[in]      bufsize      reply buffer size.
 * @param[in]      atcmdconfig  AT cmd reply format config. Use default if NULL.
 * @param[in]      timeout_ms   receive timeout in millisecond.
 *
 * @return success: 0; fail: -1
 */
int at_yield(int fd, char *replybuf, int bufsize, const at_reply_config_t *atcmdconfig,
             int timeout_ms);

/** @} */ /* end of group aos_atparser */

#endif
