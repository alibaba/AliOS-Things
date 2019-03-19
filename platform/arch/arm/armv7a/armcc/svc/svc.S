;/*
; * Copyright (C) 2015-2017 Alibaba Group Holding Limited
; */

#include <k_config.h>
#include <k_default_config.h>
#include <aux_config.h>
#include <svc.h>
#include <syscall_no.h>
#include <svc_debug.h>


    import exceptionHandler
    import _context_restore
    import syscall_tbl
    import syscall_pre_debug
    import syscall_post_debug
    import bad_svc_no_exception
    import syscall_exception

    export SVC_Handler

;/**
; *   SVC exception description:
; *
; *   --------------------------------------------
; *   |  no  | description                       |
; *   --------------------------------------------
; *   | 0x02 | return to user task               |
; *   --------------------------------------------
; *   | 0x03 | syscall                           |
; *   --------------------------------------------
; *   |others| reserved                          |
; *   --------------------------------------------
; *
; *   svc no is defined in svc.h
; */

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


    AREA ||.text||, CODE, READONLY, ALIGN=5
    ALIGN 32
    PRESERVE8
;/**********************************************
;*                        MACRO DEFINED
;**********************************************/
; ortex-A9, ARMv7 VFPv4-D16
    MACRO
        POP_FP_REG $reg
        POP     {$reg}
        ; Pop FPEXC.
        VMSR    FPEXC, $reg
        VPOP    {D0-D15}
        POP     {$reg}
        ; Pop FPSCR.
        VMSR    FPSCR, $reg
    MEND

    MACRO
        PUSH_FP_REG $reg
        ; Save FPSCR.
        VMRS    $reg, FPSCR
        ; Save floating-point registers.
        PUSH    {$reg}
        VPUSH   {D0-D15}
        ; Save FPEXC.
        VMRS    $reg, FPEXC
        PUSH    {$reg}
    MEND


    MACRO
        getcoreid $reg
        mrc   p15, 0, $reg, c0, c0, 5
        and   $reg, $reg, #3
    MEND



    AREA ||.text||, CODE, READONLY, ALIGN=5
    ALIGN 32
    PRESERVE8


SVC_Handler
    ; which stack are we running on???
    ; we are using the task's kstack which was set
    ; when task are scheded in.

    ; {spsr, r0 ... r12, sp, lr, pc}
    sub     sp, sp, #(REG_SIZE - 4)
    stmia   sp, {r0-r12}
    mrs     r2, spsr
    push    {r2}
    ;       {spsr, r0 ... r12} is done

    mov     r4, sp
    add     r1, r4, #REG_SP
    and     r2, r2, #MODE_MASK
    cmp     r2, #USER_MODE
    bne     svc_enter_svc

    ; come from user mode
    ; save user mode sp and lr
    stmia   r1, {sp, lr}^
    add     r1, r1, #8
    b       store_pc

svc_enter_svc
    ; save svc mode sp and lr
    add     r2, r4, #REG_SIZE
    stmia   r1!, {r2, lr}

store_pc
    ;      stroe lr as pc
    stmia   r1!, {lr}

    IF  {FPU} != "SoftVFP"
    ; Push task fpu register.
    PUSH_FP_REG r1
    ;#endif
    ENDIF

    IF (CONFIG_SYSCALL_DEBUG > 0)
    mov     r0, sp
    bl      syscall_pre_debug
    ENDIF

    ; align stack to 8 bytes
    mov     r5, sp
    bic     sp, sp, #0x07

    ; get svc intruction
    ldr     r1, [r4, #REG_PC]
    ldr     r2, [r4, #REG_SPSR]
    tst     r2, #0x20
    subeq   r1, r1, #4
    subne   r1, r1, #2
    ldrheq  r1, [r1]
    ldrbne  r1, [r1]


    cmp     r1, #SVC_syscall
    beq     svc_syscall

    mov     r0, r5
    bl      bad_svc_no_exception
svc_exception
    ldr     r2, [r4, #REG_SP]
    mov     r0, #0x02 ; SWI exception
    push    {r0, r2}
    mov     r0, sp

    bl      exceptionHandler
    bl      _context_restore

svc_syscall
    ; enable interrput
    mrs     r1, cpsr
    bic     r1, #IRQ_DIS
    msr     cpsr_c, r1

    ldr     r0, [r4, #REG_R0]
    ldr     r1, [r4, #REG_R1]
    bl      do_syscall
    str     r0, [r4, #REG_R0]
    str     r1, [r4, #REG_R1]

    ; need disable interrupt???
    mrs     r1, cpsr
    orr     r1, r1, #IRQ_DIS
    msr     cpsr_c, r1

    ; restore sp
    mov     sp, r5

    IF (CONFIG_SYSCALL_DEBUG > 0)
    mov     r0, sp
    bl      syscall_post_debug
    ENDIF

    IF  {FPU} != "SoftVFP"
    ; Pop task fpu register.
    POP_FP_REG r1
    ;#endif
    ENDIF

    pop     {r0}
    msr     spsr_cxsf, r0

    and     r0, r0, #MODE_MASK
    cmp     r0, #USER_MODE
    bne     ret_to_svc

    ; return to user mode
    mov     lr, sp
    add     sp, sp, #(REG_SIZE - 4)
    ldmia   lr!, {r0-r12}
    ldmia   lr, {sp, lr}^
    add     lr, lr, #0x08
    ldmia   lr, {pc}^

ret_to_svc
    ; don't worry sp is not poped properly,
    ; new sp will be restored to svc mode
    ldmia   sp, {r0-r15}^

do_syscall
    push    {ip, lr}
    cmp     r0, #MAX_SYSCALL_NO
    mov     r3, #SYSCALL_EXCEPTION_NO
    bgt     bad_syscall
    ldr     ip, =syscall_tbl
    ldr     ip, [ip, r0, lsl #2]
    cmp     ip, #0x0
    mov     r3, #SYSCALL_EXCEPTION_FUNC_NULL
    beq     bad_syscall
    mov     r0, r1
    blx     ip
    pop     {ip, pc}

bad_syscall
    ; disable interrupt
    mrs     r1, cpsr
    orr     r1, r1, #IRQ_DIS
    msr     cpsr_c, r1

    mov     r1, r0
    mov     r0, r3
    bl      syscall_exception

    add     sp, sp, #0x08
    b       svc_exception

    END
