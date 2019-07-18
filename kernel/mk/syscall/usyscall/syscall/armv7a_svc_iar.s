;
; Copyright (C) 2015-2019 Alibaba Group Holding Limited
;

#include "svc.h"

    PUBLIC SYSCALL

    SECTION   .text:CODE(4)
    ARM
    REQUIRE8
    PRESERVE8

SYSCALL:
    PUSH {R2, R3, R8, LR}
    svc #SVC_syscall
    POP {R2, R3, R8, PC}

    END
