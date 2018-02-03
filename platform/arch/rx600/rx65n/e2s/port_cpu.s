
    .GLB     _g_active_task
    .GLB     _g_preferred_ready_task
    .GLB     _interpt_bound

    .SECTION    P, CODE
    .GLB        _cpu_first_task_start

_cpu_first_task_start:
    SETPSW  U

    MOV.L   #_g_preferred_ready_task, R5
    MOV.L   [R5], R2
    MOV.L   [R2], SP                   

    POPM    R1-R2
    SHLL    #16, R2
    MVTACLO R2
    MVTACHI R1

.if __RX600
    POPC    FPSW 
.endif

    POPM    R1-R15
    RTE


    .SECTION    P, CODE
    .GLB        _cpu_switchint_handle

_cpu_switchint_handle:
    SETPSW  I
 
    PUSH.L  R15 
    MVFC    USP, R15
    SUB     #12, R15
    MVTC    R15, USP
 
    MOV.L   8[R0], 8[R15]
    MOV.L   4[R0], 4[R15]
    MOV.L   0[R0], 0[R15]
    ADD     #12, R0
    SETPSW  U
    PUSHM   R1-R14
.if __RX600
    PUSHC   FPSW
.endif
    MVFACHI R1
    MVFACMI R2
    PUSHM   R1-R2
 
    MOV.L   #_interpt_bound, R2
    MOV.L   [R2], R1
    MVFC    PSW,  R2
    AND     #-0F000001H, R2
    OR      R1,   R2
    MVTC    R2,   PSW

    MOV.L   #_g_active_task, R6
    MOV.L   [R6], R2
    MOV.L   SP, [R2]

    MOV.L   #_g_preferred_ready_task, R2
    MOV.L   [R2], [R6]
    MOV.L   [R6], R2
    MOV.L   [R2], SP

    MVTIPL  #1

    POPM    R1-R2
    SHLL    #16, R2
    MVTACHI R1
    MVTACLO R2
.if __RX600
    POPC    FPSW
.endif
    POPM    R1-R15
    RTE

    .END

