/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "svc.h"

.syntax unified
.arm

.text
.global SYSCALL
.type SYSCALL, function
SYSCALL:
    PUSH {R2, R3, R8, LR}
    svc #SVC_syscall
    POP {R2, R3, R8, PC}


