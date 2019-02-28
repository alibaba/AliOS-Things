/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef _ATPARSER_INTERNAL_H_
#define _ATPARSER_INTERNAL_H_


#include "aos/hal/uart.h"

#include "atparser_opts.h"

#define OOB_MAX 5

typedef struct oob_s
{
    char *     prefix;
    char *     postfix;
    char *     oobinputdata;
    uint32_t   reallen;
    uint32_t   maxlen;
    at_recv_cb cb;
    void *     arg;
} oob_t;

#if !ATPSR_SINGLE_TASK
#include "aos/list.h"
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
    void *    smpr;
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
#endif

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
    void       *at_uart_recv_mutex;
    void       *at_uart_send_mutex;
    void       *task_mutex;
#if !ATPSR_SINGLE_TASK
    slist_t task_l;
#endif
} at_parser_t;

#define TASK_DEFAULT_WAIT_TIME 5000

#ifndef AT_WORKER_STACK_SIZE
#define AT_WORKER_STACK_SIZE   1024
#endif

#ifndef AT_WORKER_PRIORITY
#define AT_WORKER_PRIORITY     AOS_DEFAULT_APP_PRI
#endif

#ifndef AT_UART_TIMEOUT_MS
#define AT_UART_TIMEOUT_MS     1000
#endif

#ifndef AT_SEND_DATA_DELAY_MS
#define AT_SEND_DATA_DELAY_MS  0
#endif

void *atpsr_malloc(uint32_t size);
void atpsr_free(void *ptr);
void *atpsr_mutex_new(void);
void atpsr_mutex_free(void *mutex);
void atpsr_mutex_lock(void *mutex);
void atpsr_mutex_unlock(void *mutex);
void *atpsr_sem_new(void);
void atpsr_sem_free(void *sem);
void atpsr_sem_signal(void *sem);
int atpsr_sem_wait(void *sem, uint32_t timeout_ms);
int atpsr_task_new_ext(void *task, char *name, void (*fn)(void *),
                       void *arg, int stack_size, int prio);
#endif
