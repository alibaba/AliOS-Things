/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef __AMP_REPL_H
#define __AMP_REPL_H

#include <stdint.h>
#include "duktape/duktape.h"

#ifdef __cplusplus
extern "C" {
#endif

#define REPL_OK 0
#define REPL_ERR_NOMEM -10000
#define REPL_ERR_DENIED -10001
#define REPL_ERR_INVALID -10002
#define REPL_ERR_BADCMD -10003
#define REPL_ERR_SYNTAX -10004

/* repl prompt tag */
#ifndef AMP_REPL_PROMPT
#define REPL_PROMPT "amp > "
#else
#define REPL_PROMPT AMP_REPL_PROMPT
#endif

/* repl port */
#ifndef AMP_REPL_STDIO
#define STDIO_UART 0
#else
#define STDIO_UART AMP_REPL_STDIO
#endif

/* repl port bandrate */
#ifndef AMP_REPL_STDIO_BANDRATE
#define STDIO_UART_BANDRATE 115200
#else
#define STDIO_UART_BANDRATE AMP_REPL_STDIO_BANDRATE
#endif

/* repl task stack size */
#ifndef AMP_REPL_STACK_SIZE
#define REPL_STACK_SIZE 1024*4
#else
#define REPL_STACK_SIZE AMP_REPL_STACK_SIZE
#endif

#define REPL_INBUF_SIZE 256
#define REPL_OUTBUF_SIZE 1024

int32_t repl_printf(const char *buffer, ...);

#ifdef __cplusplus
}
#endif

#endif /* __AMP_REPL_H */