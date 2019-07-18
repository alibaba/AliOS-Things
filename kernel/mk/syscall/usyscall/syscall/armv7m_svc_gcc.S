/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "svc.h"

.syntax unified
.thumb

.text
.global SYSCALL
.type SYSCALL, function
SYSCALL:
    push {r4, r14}
    svc #SVC_syscall
    pop {r4, pc}

