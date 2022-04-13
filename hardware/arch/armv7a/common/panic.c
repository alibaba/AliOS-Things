/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

void exceptionHandler(void *context)
{
    while (1);
}

void panicGetCtx(void *context, char **pPC, char **pLR, int **pSP)
{

}

void panicShowRegs(void *context, int (*print_func)(const char *fmt, ...))
{

}

void backtrace_handle(char *PC, int *SP, char *LR, int (*print_func)(const char *fmt, ...))
{

}