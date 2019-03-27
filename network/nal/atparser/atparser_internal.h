/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef _ATPARSER_INTERNAL_H_
#define _ATPARSER_INTERNAL_H_

#include "atparser.h"

#include "atparser_opts.h"

#define OOB_MAX 5

typedef enum {
    AT_RSP_WAITPROMPT = 0,
    AT_RSP_PENDING,
    AT_RSP_PROCESSING,
    AT_RSP_PROCESSED,
    AT_RSP_INVALID,
} at_rsp_state_t;

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
    slist_t        next;
    void *         smpr;
    char *         command;
    char *         rsp;
    char *         rsp_prefix;
    char *         rsp_success_postfix;
    char *         rsp_fail_postfix;
    uint32_t       rsp_prefix_len;
    uint32_t       rsp_success_postfix_len;
    uint32_t       rsp_fail_postfix_len;
    uint32_t       rsp_offset;
    uint32_t       rsp_len;
    at_rsp_state_t rsp_state;
} at_task_t;

typedef struct {
    int   fd;
    char *buf;
    int   buf_size;
} at_task_para_t;

#define AT_TASK_NAME_MAX_LEN  15
#endif

/**
 * Parser structure for parsing AT commands
 */
typedef struct
{
    uint8_t         _inited;
    uint8_t         _port;
    at_dev_type_t   _type;
    void           *_dev;
    uint32_t        _timeout_ms;
    char           *_default_recv_prefix;
    char           *_default_recv_success_postfix;
    char           *_default_recv_fail_postfix;
    char           *_send_delimiter;
    uint8_t         _wait_prompt;
    int             _recv_prefix_len;
    int             _recv_success_postfix_len;
    int             _recv_fail_postfix_len;
    int             _send_delim_size;
    oob_t           _oobs[OOB_MAX];
    int             _oobs_num;
    void           *_recv_mutex;
    void           *_send_mutex;
#if !ATPSR_SINGLE_TASK
    int             _recv_task_prio;
    int             _recv_task_stacksize;
    void           *_task_mutex;
    char            _task_name[AT_TASK_NAME_MAX_LEN];
    at_task_para_t *_task_para;
    void           *_task_start_sem;
    void           *_task_exit_sem;
    slist_t         _task_l;
#endif
} at_dev_t;

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

typedef struct {
   at_dev_type_t type;
   int (*init)(void *dev);
   int (*recv)(void *dev, void *data, uint32_t expect_size,
               uint32_t *recv_size, uint32_t timeout);
   int (*send)(void *dev, void *data, uint32_t size,
               uint32_t timeout);
   int (*deinit)(void *dev);
} at_dev_ops_t;

#endif
