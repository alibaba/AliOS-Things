;
; Copyright (C) 2015-2017 Alibaba Group Holding Limited
;

#include <k_config.h>
#include <k_default_config.h>
#include <aux_config.h>
#include <svc.h>
#include <syscall_no.h>

;
;  SVC exception description:
;
;  --------------------------------------------
;  |  no  | description                       |
;  --------------------------------------------
;  | 0x03 | syscall                           |
;  --------------------------------------------
;  |others| reserved                          |
;  --------------------------------------------
;
;  svc no is defined in svc.h
;

;**************************************************************
;                  EXTERN SYMBOLS
;**************************************************************

    IMPORT exceptionHandler
    IMPORT _context_restore
    IMPORT syscall_tbl
    IMPORT syscall_pre_debug
    IMPORT syscall_post_debug

;**************************************************************
;                  EXPORT SYMBOLS
;**************************************************************
    EXPORT SVC_Handler

;**************************************************************
;                  DEFINES
;**************************************************************
#define USER_MODE       0x10
#define FIQ_MODE        0x11
#define IRQ_MODE        0x12
#define SVC_MODE        0x13
#define ABT_MODE        0x17
#define UND_MODE        0x1b
#define SYS_MODE        0x1f
#define MODE_MASK       0x1f

#define FIQ_DIS         0x40
#define IRQ_DIS         0x80
#define INT_DIS         (FIQ_DIS :OR: IRQ_DIS)

#define REG_SPSR        0x00
#define REG_R0          0x04
#define REG_R1          0x08
#define REG_R2          0x0c
#define REG_R3          0x10
#define REG_R4          0x14
#define REG_R5          0x18
#define REG_R6          0x1c
#define REG_R7          0x20
#define REG_R8          0x24
#define REG_R9          0x28
#define REG_R10         0x2c
#define REG_R11         0x30
#define REG_R12         0x34
#define REG_SP          0x38
#define REG_LR          0x3c
#define REG_PC          0x40
#define REG_SIZE        0x44

;***********************************************************
;                  CODE GENERATION DIRECTIVES
;***********************************************************
    AREA ||.text||, CODE, READONLY, ALIGN=5
    ALIGN 32
    PRESERVE8

;***********************************************************
;                  MACRO DEFINED
;***********************************************************
; ortex-A9, ARMv7 VFPv4-D16
    MACRO
    POP_FP_REG $reg
        POP     {$reg}          ; Pop FPEXC.
        VMSR    FPEXC, $reg
        POP     {$reg}          ; Pop FPSCR.
        VMSR    FPSCR, $reg
    IF :DEF:TARGET_FEATURE_NEON
        VPOP    {Q0-Q7}
        VPOP    {Q8-Q15}
    ELSE
        VPOP    {D0-D15}
    ENDIF
    MEND

    MACRO
    PUSH_FP_REG $reg
    IF :DEF: TARGET_FEATURE_NEON
        VPUSH   {Q8-Q15}
        VPUSH   {Q0-Q7}
    ELSE
        VPUSH   {D0-D15}        ; Save floating-point registers.
    ENDIF
        VMRS    $reg, FPSCR     ; Save FPSCR.
        PUSH    {$reg}
        VMRS    $reg, FPEXC     ; Save FPEXC.
        PUSH    {$reg}
    MEND

;***********************************************************
; Functions:
;       void SVC_Handler(void);
;***********************************************************
SVC_Handler
    ; which stack are we running on???
    ; we are using the task's stack which was set
    ; when task are scheded in.

    ; save {SPSR, R0 ... R12, SP, LR, PC}
    SUB     SP, SP, #(REG_SIZE - 4)
    STMIA   SP, {R0-R12}
    MRS     R2, SPSR
    PUSH    {R2}
    ;       {SPSR, R0 ... R12} is done

    MOV     R4, SP
    ADD     R1, R4, #REG_SP
    AND     R2, R2, #MODE_MASK
    CMP     R2, #USER_MODE
    BNE     _svc_enter_svc

    ; come from user mode, save user mode SP and LR
    STMIA   R1, {SP, LR}^
    ADD     R1, R1, #8     ; none wirte back allowed on arm
    B       _store_pc

_svc_enter_svc
    ; save svc mode SP and LR
    ADD     R2, R4, #REG_SIZE
    STMIA   R1!, {R2, LR}

_store_pc
    ;      stroe LR as PC
    STMIA   R1!, {LR}

    IF  {FPU} != "SoftVFP"
        ; Push task fpu register.
        PUSH_FP_REG R1
        ;#endif
    ENDIF

    ; align stack to 8 bytes
    MOV     R5, SP
    BIC     SP, SP, #0x07

    IF (CONFIG_SYSCALL_DEBUG > 0)
        ; syscall pre debug
        MOV     R0, R5
        BL      syscall_pre_debug
    ENDIF

    ; get svc intruction
    LDR     R1, [R4, #REG_PC]
    ; judge arm or thumb2 instruction
    LDR     R2, [R4, #REG_SPSR]
    TST     R2, #0x20
    SUBEQ   R1, R1, #4
    SUBNE   R1, R1, #2
    LDRHEQ  R1, [R1]
    LDRBNE  R1, [R1]
    CMP     R1, #SVC_syscall
    BNE     _svc_exception

_svc_syscall
    ; enable interrput
    MRS     R1, CPSR
    BIC     R1, #IRQ_DIS
    MSR     CPSR_c, R1

    ; prepare args for _do_syscall
    LDR     R0, [R4, #REG_R0]
    LDR     R1, [R4, #REG_R1]
    BL      _do_syscall
    ; save syscall return data
    STR     R0, [R4, #REG_R0]
    STR     R1, [R4, #REG_R1]

    ; disable interrupt
    MRS     R1, CPSR
    ORR     R1, R1, #IRQ_DIS
    MSR     CPSR_c, R1

    ; restore SP
    MOV     SP, R5

    IF (CONFIG_SYSCALL_DEBUG > 0)
        ; syscall post debug
        MOV     R0, SP
        BL      syscall_post_debug
    ENDIF

    IF  {FPU} != "SoftVFP"
        ; Pop task fpu register.
        POP_FP_REG R1
    ENDIF

    POP     {R0}
    MSR     SPSR, R0

    AND     R0, R0, #MODE_MASK
    CMP     R0, #USER_MODE
    BNE     _ret_to_svc

    ; return to user mode
    MOV     LR, SP
    ADD     SP, SP, #(REG_SIZE - 4)
    LDMIA   LR!, {R0-R12}
    LDMIA   LR, {SP, LR}^
    ADD     LR, LR, #0x08
    LDMIA   LR, {PC}^

_ret_to_svc
    ; don't worry SP is not poped properly,
    ; new SP will be restored to svc mode
    LDMIA   SP, {R0-R15}^

;***********************************************************
; Functions:
;     static unsigned long long _do_syscall(int syscall_no, void *arg);
;***********************************************************
_do_syscall
    PUSH    {IP, LR}
    CMP     R0, #MAX_SYSCALL_NO
    BGT     _bad_syscall
    LDR     IP, =syscall_tbl
    LDR     IP, [IP, R0, LSL #2]
    MOV     R0, R1
    CMP     IP, #0x0
    BEQ     _bad_syscall
    BLX     IP
    POP     {IP, PC}

_bad_syscall
    ; disable interrupt
    MRS     R1, CPSR
    ORR     R1, R1, #IRQ_DIS
    MSR     CPSR_c, R1
    ADD     SP, #0x08

_svc_exception
    LDR     R2, [R4, #REG_SP]
    ; set SWI exception
    MOV     R0, #0x02
    PUSH    {R0, R2}
    MOV     R0, SP

    BL      exceptionHandler
    MOV     SP, R5
    BL      _context_restore

    END

