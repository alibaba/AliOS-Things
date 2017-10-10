/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef AT_PARSER_H
#define AT_PARSER_H

#include <stdarg.h>
#include <hal/soc/uart.h>
#include <aos/aos.h>

#ifndef bool
#define bool unsigned char
#endif

#ifndef true
#define true 1
#endif
#ifndef false
#define false 0
#endif

#define BUFFER_SIZE 256
#define OOB_MAX 5
#define PREFIX_MAX 32

#define RECV_STATUS_OK "OK\r\n"
#define RECV_STATUS_ERROR "ERROR\r\n"

typedef void (*oob_cb)(void *arg);

typedef struct oob_s {
    unsigned len;
    char prefix[PREFIX_MAX];
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
    char *rsp;
    int rsp_offset;
} at_task_t;

typedef enum {
    NORMAL = 0,
    ASYN
} at_mode_t;

/**
* Parser structure for parsing AT commands
*
* Here are some examples:
* @code
* extern at;
* at.init(uart, "\r\n", "\r\n", 1000);
* int value;
* char buffer[100];
*
* at.send("AT") && at.recv("OK");
* at.send("AT+CWMODE=%d", 3) && at.recv("OK");
* at.send("AT+CWMODE?") && at.recv("+CWMODE:%d\r\nOK", &value);
* at.read(buffer, value);
* at.recv("OK");
* @endcode
*/
typedef struct {
    /// used only internally
    uart_dev_t _uart;
    char _buffer[BUFFER_SIZE];
    int _timeout;
    char *_recv_delimiter;
    char *_send_delimiter;
    int _recv_delim_size;
    int _send_delim_size;
    oob_t _oobs[OOB_MAX];
    int _oobs_num;
    aos_mutex_t _mutex;
    at_mode_t _mode;

    // can be used externally
    slist_t task_l;

    /**
    * initialization
    *
    * @param serial serial interface to use for AT commands
    * @param buffer_size size of internal buffer for transaction
    * @param timeout timeout of the connection
    * @param send_delimiter string of characters to use as line delimiters for sending
    * @param recv_delimiter string of characters to use as line delimiters for receiving
    */
    void (*init)(uart_dev_t *u, const char *recv_delimiter,
                 const char *send_delimiter, int timeout);

    void (*set_mode)(at_mode_t m);

    void (*set_timeout)(int timeout);

    void (*set_delimiter)(const char *delimiter);

    void (*set_recv_delimiter)(const char *delimiter);

    void (*set_send_delimiter)(const char *delimiter);

    /**
    * Sends an AT command.
    */
    bool (*send)(const char *command, ...);
    bool (*vsend)(const char *command, va_list args);

    /*
    * This is a blocking API. It hanbles raw command sending, then is blocked
    * to wait for response.
    *
    * This API send raw command (prepared by caller) and wait for raw response.
    * The caller is responsible for preparing the ready-to-send command
    * as well as parsing the response result. The caller is alos responsible
    * for allocating/freeing rsp buffer.
    */
    int (*send_raw)(const char *command, char *rsp);

    /**
    * Recieve an AT response.
    *
    * Recieves a formatted response using scanf style formatting
    *
    * Responses are parsed line at a time using the specified delimiter.
    * Any recieved data that does not match the response is ignored until
    * a timeout occurs.
    */
    bool (*recv)(const char *response, ...);
    bool (*vrecv)(const char *response, va_list args);

    /**
    * Write a single byte to the buffer.
    */
    int (*putc)(char c);

    /**
    * Get a single byte from the buffer.
    */
    int (*getc)();

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
    void (*oob)(const char *prefix, oob_cb func, void *arg);
} at_parser_t;

extern at_parser_t at;
#endif
