#include <k_config.h>
#include <aux_config.h>

;******************************************************************************
;                            IMPORT SYMBOLS
;******************************************************************************
    IMPORT g_active_task
    IMPORT g_preferred_ready_task
    IMPORT sys_stack_top
    IMPORT exceptionHandler
    IMPORT cpu_interrupt_handler
    IMPORT k_proc_switch
    IMPORT g_sys_stat
    IMPORT krhino_task_sched_stats_get

    IF (RHINO_CONFIG_TASK_STACK_OVF_CHECK > 0)
        IMPORT krhino_stack_ovf_check
    ENDIF

;******************************************************************************
;                            EXPORT SYMBOLS
;******************************************************************************
    EXPORT cpu_intrpt_save
    EXPORT cpu_intrpt_restore
    EXPORT cpu_task_switch
    EXPORT cpu_intrpt_switch
    EXPORT cpu_first_task_start

    EXPORT _context_restore
    EXPORT _interrupt_handler
    EXPORT _panic_handler
    EXPORT _interrupt_return_address
    EXPORT cpu_get_cpuid

;******************************************************************************
;                                 EQUATES
;******************************************************************************
; Bits in CPSR (Current Program Status Register)
CPSR_Mode_USR    EQU    0x10
CPSR_Mode_FIQ    EQU    0x11
CPSR_Mode_IRQ    EQU    0x12
CPSR_Mode_SVC    EQU    0x13
CPSR_Mode_ABT    EQU    0x17
CPSR_Mode_UND    EQU    0x1B
CPSR_Mode_SYS    EQU    0x1F
CPSR_Mode_MASK   EQU    0x1F

CPSR_FIQ_DIS     EQU    0x40                    ; Disable FIQ.
CPSR_IRQ_DIS     EQU    0x80                    ; Disable IRQ.
CPSR_INT_DIS     EQU    CPSR_FIQ_DIS:OR:CPSR_IRQ_DIS
CPSR_THUMB       EQU    0x20                    ; Set Thumb mode.

;******************************************************************************
;                        CODE GENERATION DIRECTIVES
;******************************************************************************
    AREA ||.text||, CODE, READONLY
    ARM
    CODE32

;******************************************************************************
;                        MACRO DEFINED
;******************************************************************************
; Cortex-A7, ARMv7 VFPv4-D16
    MACRO
    POP_FP_REG $reg
        POP     {$reg}
        VMSR    FPEXC, $reg                                 ; Pop FPEXC.
        POP     {$reg}
        VMSR    FPSCR, $reg                                 ; Pop FPSCR.
        IF :DEF:TARGET_FEATURE_NEON
            VPOP    {Q0-Q7}
            VPOP    {Q8-Q15}
        ELSE
            VPOP    {D0-D15}
        ENDIF
    MEND

    MACRO
    PUSH_FP_REG $reg
        IF :DEF:TARGET_FEATURE_NEON
            VPUSH   {Q8-Q15}
            VPUSH   {Q0-Q7}
        ELSE
            VPUSH   {D0-D15}
        ENDIF
        VMRS    $reg, FPSCR                                 ; Save FPSCR.
        PUSH    {$reg}                                      ; Save floating-point registers.
        VMRS    $reg, FPEXC                                 ; Save FPEXC.
        PUSH    {$reg}
    MEND

    MACRO
    getcoreid $reg
        mrc   p15, 0, $reg, c0, c0, 5
        and   $reg, $reg, #3
    MEND

;******************************************************************************
; Functions:
;     size_t cpu_intrpt_save(void);
;     void cpu_intrpt_restore(size_t cpsr);
;******************************************************************************
    AREA ||.text||, CODE, READONLY
    CODE32
    PRESERVE8
cpu_intrpt_save
    MRS     R0, CPSR
    CPSID   IF
    DSB
    BX      LR

cpu_intrpt_restore
    DSB
    MSR     CPSR_c, R0
    BX      LR

;******************************************************************************
; Functions:
;     void   cpu_first_task_start(void);
;******************************************************************************
cpu_first_task_start
    MSR     CPSR_c, #(CPSR_INT_DIS:OR:CPSR_Mode_SVC)     ; change to SVC mode.
    BL      _task_restore

;******************************************************************************
; Functions:
;     void cpu_task_switch(void);
;******************************************************************************
cpu_task_switch
    ; save current task context:
    PUSH    {LR}                 ; Push PC.
    PUSH    {LR}                 ; Push LR

    SUB     SP, SP, #(14*4)      ; reserve space for {r0-r12, sp}
    STMIA   SP, {R0-R12}
    ADD     R0, SP, #(16*4)      ; SP  value shall be stored
    STR     R0, [SP, #(13*4)]    ; stroe SP

    ; Note: when @cpu_task_switch is called, the task
    ; is running at SVC mode, the next time the task
    ; is switched in, it should run in the same mode.
    ; so, we save CPSR, rather than SPSR.
    MRS     R0, CPSR             ; Push old task CPSR
    TST     LR, #1               ; test if called from Thumb mode,
    ORRNE   R0, R0, #CPSR_THUMB  ; if yes, set the T-bit.
    PUSH    {R0}

    IF {FPU} != "SoftVFP"
        PUSH_FP_REG R0
    ENDIF

    IF (RHINO_CONFIG_CPU_NUM > 1)
        getcoreid   R3
        LSL     R3, R3, #2
        LDR     R1, =g_active_task
        ADD     R1, R1, R3
    ELSE
        LDR     R1, =g_active_task
    ENDIF

    ; g_active_task->task_stack = SP;
    LDR     R1, [R1]
    STR     SP, [R1, #RHINO_CONFIG_TASK_KSTACK_OFFSET]

    IF (RHINO_CONFIG_TASK_STACK_OVF_CHECK > 0)
        BL      krhino_stack_ovf_check
    ENDIF

    IF (RHINO_CONFIG_SYS_STATS > 0)
        BL      krhino_task_sched_stats_get
    ENDIF

    IF (RHINO_CONFIG_CPU_NUM > 1)
        BL      os_unlock_sys_spin
    ENDIF

    BL      _task_restore

;******************************************************************************
; Functions:
;     void   cpu_intrpt_switch(void);
;******************************************************************************
cpu_intrpt_switch
    PUSH    {FP, LR}

    IF (RHINO_CONFIG_TASK_STACK_OVF_CHECK > 0)
        BL      krhino_stack_ovf_check
    ENDIF

    IF (RHINO_CONFIG_SYS_STATS > 0)
        BL      krhino_task_sched_stats_get
    ENDIF

    LDR     R0, =g_active_task     ; g_active_task = g_preferred_ready_task
    LDR     R1, =g_preferred_ready_task

    IF (RHINO_CONFIG_CPU_NUM > 1)
        getcoreid   R3
        LSL     R3, R3, #2

        ADD     R0, R0, R3
        ADD     R1, R1, R3
    ENDIF

    LDR     R2, [R1] ; new task
    LDR     R3, [R0] ; old task
    STR     R2, [R0]

    PUSH    {R0-R3,R12,LR}
    MOV     R0, R2
    MOV     R1, R3
    bl      k_proc_switch
    POP     {R0-r3, R12,LR}

    POP     {FP, PC}

;******************************************************************************
; _task_restore
; _context_restore
;******************************************************************************
_task_restore
    ; g_active_task[cpu] = g_preferred_ready_task[cpu]
    LDR     R0, =g_active_task
    LDR     R1, =g_preferred_ready_task

    IF (RHINO_CONFIG_CPU_NUM > 1)
        getcoreid   R3
        LSL     R3, R3, #2

        ADD     R0, R0, R3
        ADD     R1, R1, R3
    ENDIF

    LDR     R2, [R1]  ; new task
    LDR     R3, [R0]  ; old task
    STR     R2, [R0]

    PUSH    {R0-R3, R12, LR}
    MOV     R0, R2
    MOV     R1, R3
    bl      k_proc_switch
    POP     {R0-R3, R12, LR}

    LDR     SP, [R2, #RHINO_CONFIG_TASK_KSTACK_OFFSET]

_context_restore
    IF {FPU} != "SoftVFP"
        POP_FP_REG R0
    ENDIF

    POP     {R0}                ; Pop cpsr of task
    MSR     SPSR_cxsf, R0

    ; judge which mode should the task running at
    AND     R0, R0, #CPSR_Mode_MASK
    CMP     R0, #CPSR_Mode_USR
    BNE     svc_mode_return

    ; user mode
    MOV     LR, SP
    ; pop {r0-r15}
    ADD     SP, SP, #0x40
    LDMIA   LR!, {R0-R12}
    LDMIA   LR, {SP, LR}^
    ADD     LR, LR, #0x08
    LDMIA   LR, {PC}^

svc_mode_return
    ; svc mode
    MOV     R0, SP
    ADD     SP, SP, #0x40
    LDMFD   R0, {R0-R12, SP, LR, PC}^

;******************************************************************************
; _interrupt_handler
;******************************************************************************
; R0 exc_cause, R1 SPSR, R2 PC, R3 SP of old mode
_interrupt_handler
    ; change to SVC mode & disable interruptions.
    MSR     CPSR_c, #(CPSR_INT_DIS:OR:CPSR_Mode_SVC)
    PUSH    {R2}         ; Push old task PC,
    AND     R2, R1, #CPSR_Mode_MASK
    CMP     R2, #CPSR_Mode_USR
    BNE     svc_mode_interrupt

    ; user mode
    SUB     SP, SP, #0x08
    MOV     R2, SP
    STMIA   R2, {SP, LR}^
    LDR     R2, [SP]          ; take user mode SP
    B       usr_mode_interrupt

svc_mode_interrupt
    ; svc mode
    ADD     R2, SP, #0x04
    PUSH    {R2, LR}         ; Push SP,LR

usr_mode_interrupt
    PUSH    {R4-R12}     ; Push old task R12-R4,
    LDMFD   R3!, {R5-R8} ; Pop old task R3-R0 from mode stack.
    PUSH    {R5-R8}      ; Push old task R3-R0,
    PUSH    {R1}         ; Push task SPSR.

    IF {FPU} != "SoftVFP"
        PUSH_FP_REG R3
    ENDIF

    ; if (g_sys_stat == RHINO_RUNNING)
    LDR     R3, =g_sys_stat
    LDR     R4, [R3]
    CMP     R4, #3       ; RHINO_RUNNING = 3
    BNE     _interrupt_while_init

_interrupt_while_task
    ; g_active_task->task_stack = context region
    LDR     R3, =g_active_task

    IF (RHINO_CONFIG_CPU_NUM > 1)
        getcoreid   R5
        LSL     R5, R5, #2
        ADD     R3, R3, R5
    ELSE
        MOV     R5, #0
    ENDIF

    LDR     R4, [R3]

    AND     R1, R1, #CPSR_Mode_MASK
    CMP     R1, #CPSR_Mode_USR
    STREQ   R2, [R4, #RHINO_CONFIG_TASK_USTACK_OFFSET]

    STR     SP, [R4, #RHINO_CONFIG_TASK_KSTACK_OFFSET]

    ; Switch to system stack.
    LDR     R3, =sys_stack_top
    MOV     R4, #RHINO_CONFIG_SYSTEM_STACK_SIZE
    MUL     R4, R4, R5
    SUB     R3, R3, R4

    MOV     SP, R3

    ; cpu_interrupt_handler(except_type = R0)
    BL      cpu_interrupt_handler


_interrupt_return_address
    ; SP = g_active_task->task_stack;
    LDR     R3, =g_active_task
    IF (RHINO_CONFIG_CPU_NUM > 1)
        getcoreid   R4
        LSL     R4, R4, #2
        ADD     R3, R3, R4
    ENDIF

    LDR     R4, [R3]
    LDR     SP, [R4, #RHINO_CONFIG_TASK_KSTACK_OFFSET]

    BL      _context_restore

_interrupt_while_init
    ; align SP to 8 byte.
    MOV     R1, SP
    AND     R1, R1, #4
    SUB     SP, SP, R1
    PUSH    {R1, LR}

    ; cpu_interrupt_handler(except_type = R0)
    BL      cpu_interrupt_handler
    POP     {R1, LR}
    ADD     SP, SP, R1

    BL      _context_restore

_panic_handler
    ; change to SVC mode & disable interruptions.
    MSR     CPSR_c, #(CPSR_INT_DIS:OR:CPSR_Mode_SVC)

    PUSH    {R2}          ; Push old task PC,
    ADD     R2, SP, #4
    PUSH    {LR}          ; Push old task LR,
    PUSH    {R2}          ; Push old sp
    PUSH    {R4-R12}      ; Push old task R12-R4,
    LDMFD   R3!, {R5-R8}  ; Pop old task R3-R0 from mode stack.
    PUSH    {R5-R8}       ; Push old task R3-R0,
    PUSH    {R1}          ; Push task CPSR.

    IF {FPU} != "SoftVFP"
        PUSH_FP_REG R1        ; Push task fpu register.
    ENDIF

    PUSH    {R0, R2}      ; Push SP and exc_type

    ; align SP to 8 byte.
    MOV     R0, SP
    MOV     R1, SP
    AND     R1, R1, #4
    SUB     SP, SP, R1
    PUSH    {R1, LR}

    BL      exceptionHandler

    POP     {R1, LR}
    ADD     SP, SP, R1
    POP     {R0, R2}

    BL      _context_restore

;******************************************************************************
; int cpu_get_cpuid(void)
; get current CPU ID
;******************************************************************************
cpu_get_cpuid
    mrc   p15, 0, r0, c0, c0, 5
    and   r0, r0, #3
    BX    lr

    END

