;
; Copyright (C) 2015-2019 Alibaba Group Holding Limited
;

#include "svc.h"

    export SYSCALL

    AREA ||.text||, CODE, READONLY, ALIGN=5
    ALIGN 32
    PRESERVE8

SYSCALL
    PUSH {R2, R3, R8, LR}
    SVC  #SVC_syscall
    POP  {R2, R3, R8, PC}

    END

