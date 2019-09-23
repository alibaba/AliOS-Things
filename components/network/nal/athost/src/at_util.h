/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef _AT_UTIL_H_
#define _AT_UTIL_H_

#include <stdint.h>

#define MAX_ATCMD_PREFIX_LEN 40
#define MAX_ATCMD_DOMAIN_LEN 260
#define MAX_ATCMD_PORT_LEN   10

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

/* Structure for registering at CLI commands */
typedef struct atcmd_handler
{
    const char *name;
    const char *help;

    int (*function)(void);
} atcmd_hdl_t, *atcmd_hdl_ptr_t;

typedef struct atcmd_op_s
{
    const char *name;
    const char *prefix;

    int (*init)(void);
    void (*deinit)(void);

    atcmd_hdl_ptr_t (*get_atcmd_handler)(void);
} atcmd_op_t, *atcmd_op_ptr_t;

enum
{
    ATCMD_FAIL    = 0,
    ATCMD_SUCCESS = 1,
};

enum
{
    CME_ERROR_OP_NOT_ALLOWED       = 3,
    CME_ERROR_OP_NOT_SUPPORT       = 4,
    CME_ERROR_TXT_STR_TOO_LONG     = 24,
    CME_ERROR_TXT_STR_INVALID_CHAR = 25,
    CME_ERROR_NET_TIMEOUT          = 31,
    CME_ERROR_INCORRECT_PARA       = 50,
    CME_ERROR_UNKNOWN              = 100,
};

/*
 * substitute for itoa for some cases where itoa is missing
 */
void itoa_decimal(int n, char s[]);

/*
 * utility functions for uart at read
 */
typedef int (*at_data_check_cb_t)(char data);
int atcmd_socket_data_len_check(char data);
int atcmd_socket_ip_info_check(char data);
int atcmd_socket_conntype_check(char data);
int atcmd_socket_data_info_get(char *buf, uint32_t buflen,
                               at_data_check_cb_t valuecheck);
int atcmd_socket_text_info_get(char *buf, uint32_t buflen,
                               at_data_check_cb_t valuecheck);

/*
 * utility functions for uart at send
 */
int  uart_send_task_init();
void uart_send_task_deinit();
int  insert_uart_send_msg(char *cmdptr, uint8_t *dataptr, uint16_t cmdlen,
                          uint16_t datalen);
int atcmd_write(char *cmdptr, uint8_t *dataptr, uint16_t cmdlen,
                uint16_t datalen);
int atcmd_read(char *outbuf, uint32_t len);

/*
 * utility functions for common atcmd result notify
 */
int notify_atcmd_recv_status(int status);

/*
 * utility functions for common cme notify
 */
int notify_atcmd_cme_error(int error_no);

/*
 * register functions for atcmd handlers
 */
int atcmd_register_handlers(atcmd_op_ptr_t handler);

/*
 * entry of atcmd handle
 */
void atcmd_handle_entry();

/*
 * Initialization of atcmd handle
 */
int atcmd_handle_init();
#endif
