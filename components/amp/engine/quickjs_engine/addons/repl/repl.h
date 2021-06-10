/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef __AMP_REPL_H
#define __AMP_REPL_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define REPL_OK 0
#define REPL_ERR_NOMEM -10000
#define REPL_ERR_DENIED -10001
#define REPL_ERR_INVALID -10002
#define REPL_ERR_BADCMD -10003
#define REPL_ERR_SYNTAX -10004

/* repl task stack size */
#ifndef AMP_REPL_STACK_SIZE
#define REPL_STACK_SIZE 1024*4
#else
#define REPL_STACK_SIZE AMP_REPL_STACK_SIZE
#endif

#define REPL_INBUF_SIZE 256
#define REPL_OUTBUF_SIZE 1024

int32_t repl_printf(const char *buffer, ...);
int aos_repl_read(char *inbuf, uint32_t expected_length, uint32_t *recv_size);
int aos_repl_write(char *str);
void repl_read_task_start(void);
void aos_repl_init(void *arg);

#ifdef __cplusplus
}
#endif

#endif /* __AMP_REPL_H */
