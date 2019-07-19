;
; Copyright (C) 2015-2019 Alibaba Group Holding Limited
;

#include "svc.h"

    PUBLIC SYSCALL

    SECTION   .text:CODE(4)
    THUMB
    REQUIRE8
    PRESERVE8

SYSCALL:
    push {r4, r14}
    svc #SVC_syscall
    pop {r4, pc}

    END
