/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef K_CONTEXT_H
#define K_CONTEXT_H

/* get usr context from task kstack through syscall*/
typedef struct {
    long R0;
    long R1;
    long R2;
    long R3;
    long R12;
    long LR;
    long PC;
    long xPSR;
} debug_syscall_context_t;

#endif /* K_CONTEXT_H */

