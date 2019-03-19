#include <k_config.h>
#include <aux_config.h>

;/**********************************************
;*   EXTERN SYMBOLS
;**********************************************/
    import g_active_task
    import g_preferred_ready_task
    import krhino_stack_ovf_check
    import sys_stack_top
    import exceptionHandler
    import cpu_interrupt_handler
    import k_mm_hook
    import g_sys_stat
    import aos_cpu_except_handler
    import krhino_intrpt_enter
    import krhino_intrpt_exit
    import krhino_task_sched_stats_get

;/**********************************************
;*   EXPORT SYMBOLS
;**********************************************/
    export cpu_intrpt_save
    export cpu_intrpt_restore
    export cpu_task_switch
    export cpu_intrpt_switch
    export cpu_first_task_start

    export _context_restore
    export _interrupt_handler
    export _panic_handler
    export _interrupt_return_address
    export cpu_get_cpuid

;/**********************************************
;*   EQUATES
;**********************************************/
; Bits in CPSR (Current Program Status Register)
CPSR_Mode_USR     EQU    0x10
CPSR_Mode_FIQ     EQU    0x11
CPSR_Mode_IRQ     EQU    0x12
CPSR_Mode_SVC     EQU    0x13
CPSR_Mode_ABT     EQU    0x17
CPSR_Mode_UND     EQU    0x1B
CPSR_Mode_SYS     EQU    0x1F
CPSR_Mode_MASK    EQU    0x1F

; Disable FIQ.
CPSR_FIQ_DIS      EQU    0x40
; Disable IRQ.
CPSR_IRQ_DIS      EQU    0x80
CPSR_INT_DIS      EQU    0xC0
; Set Thumb mode.
CPSR_THUMB        EQU    0x20

;/**********************************************
; *  CODE GENERATION DIRECTIVES
;**********************************************/


    AREA ||.text||, CODE, READONLY
    CODE32
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


;/**********************************************
;* Functions:
;*     size_t cpu_intrpt_save(void);
;*     void cpu_intrpt_restore(size_t cpsr);
;***********************************************/
    AREA ||.text||, CODE, READONLY
    CODE32
    PRESERVE8
cpu_intrpt_save
    MRS     R0, CPSR
    CPSID   IF
    ; no need dsb here?
    DSB
    BX      LR

cpu_intrpt_restore
    ; no need dsb here?
    DSB
    MSR     CPSR_cxsf, R0
    BX      LR

;/**********************************************
;* Functions:
;*     void   cpu_first_task_start(void);
;**********************************************/
cpu_first_task_start
    ; change to SVC mode.
    MSR     CPSR_c, #(CPSR_INT_DIS :OR: CPSR_Mode_SVC)
    BL      _task_restore

;/**********************************************
;* Functions:
;*     void cpu_task_switch(void);
;**********************************************/
cpu_task_switch
    ; save current task context:
    PUSH    {LR}                 ; Push PC.
    PUSH    {LR}                 ; Push LR

    SUB     SP, SP, #(14*4)      ; reserve space for {r0-r12, sp}
    STMIA   SP, {R0-R12}
    ADD     R0, SP, #(16*4)      ; SP  value shall be stored
    STR     R0, [SP, #(13*4)]    ; stroe SP

;    ADD     LR, SP, #0x08
;    PUSH    {LR}                 ; Push old SP
;    PUSH    {R0-R12}         ; Push R0-R12 LR

    ; Note: when @cpu_task_switch is called, the task
    ; is running at SVC mode, the next time the task
    ; is switched in, it should run in the same mode.
    ; so, we save CPSR, rather than SPSR.
    MRS     R0, CPSR             ; Push old task CPSR
    TST     LR, #1               ; test if called from Thumb mode,
    ORRNE   R0, R0, #CPSR_THUMB  ; if yes, set the T-bit.
    PUSH    {R0}

        IF   {FPU} != "SoftVFP"
        ; Push fpu register.
        PUSH_FP_REG R0
    ;    #endif
    ENDIF

    getcoreid   R3
    LSL     R3,R3,#2

    LDR     R1, =g_active_task
    ADD     R1, R1, R3

    ; g_active_task->task_stack = SP;
    LDR     R1, [R1]
    STR     SP, [R1, #RHINO_CONFIG_TASK_KSTACK_OFFSET]

    IF (RHINO_CONFIG_TASK_STACK_OVF_CHECK > 0)
    ;    #if (RHINO_CONFIG_TASK_STACK_OVF_CHECK > 0)
        BL      krhino_stack_ovf_check
    ;    #endif
    ENDIF

    IF (RHINO_CONFIG_TASK_SCHED_STATS > 0)
    ;    #if (RHINO_CONFIG_TASK_SCHED_STATS > 0)
        BL      krhino_task_sched_stats_get
    ;    #endif
    ENDIF

    IF (RHINO_CONFIG_CPU_NUM > 1)
        ;#if (RHINO_CONFIG_CPU_NUM > 1)
        BL      os_unlock_sys_spin
        ;#endif
    ENDIF

    BL      _task_restore

;/**********************************************
;* Functions:
;*     void   cpu_intrpt_switch(void);
;**********************************************/
cpu_intrpt_switch
    PUSH    {FP, LR}

    IF (RHINO_CONFIG_TASK_STACK_OVF_CHECK > 0)
       ;#if (RHINO_CONFIG_TASK_STACK_OVF_CHECK > 0)
        BL      krhino_stack_ovf_check
       ;#endif
    ENDIF

    IF (RHINO_CONFIG_TASK_SCHED_STATS > 0)
    ;    #if (RHINO_CONFIG_TASK_SCHED_STATS > 0)
        BL      krhino_task_sched_stats_get
    ;    #endif
    ENDIF

    getcoreid   R3
    LSL     R3, R3, #2

    ; g_active_task[cpu] = g_preferred_ready_task[cpu]
    LDR     R0, =g_active_task
    LDR     R1, =g_preferred_ready_task

    ADD     R0, R0, R3
    ADD     R1, R1, R3

    LDR     R2, [R1] ; new task
    LDR     R3, [R0] ; old task
    STR     R2, [R0]

    PUSH    {R0-r3,r12,lr}
    MOV     R0, R3
    MOV     R1, R2
    bl      k_mm_hook
    POP     {R0-r3,r12,lr}

    POP     {FP, PC}


;/**********************************************
;* _task_restore
;* _context_restore
;**********************************************/
_task_restore
    getcoreid   R3
    LSL     R3, R3, #2

    ; g_active_task[cpu] = g_preferred_ready_task[cpu]
    LDR     R0, =g_active_task
    LDR     R1, =g_preferred_ready_task

    ADD     R0, R0, R3
    ADD     R1, R1, R3

    LDR     R2, [R1]
    LDR     R3, [R0]
    STR     R2, [R0]

    PUSH    {R0-r3,r12,lr}
    MOV     R0, R3
    MOV     R1, R2
    bl      k_mm_hook
    POP     {R0-r3,r12,lr}

    LDR     SP, [R2, #RHINO_CONFIG_TASK_KSTACK_OFFSET]

_context_restore
        IF   {FPU} != "SoftVFP"
    ; Pop fpu register.
    POP_FP_REG R0
;    #endif
    ENDIF

    ; Pop cpsr of task
    POP     {R0}
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

;/**********************************************
;* _interrupt_handler
;**********************************************/
; R0 exc_cause, R1 SPSR, R2 PC, R3 SP of old mode
_interrupt_handler
    ; change to SVC mode & disable interruptions.
    MSR     CPSR_c, #(CPSR_INT_DIS :OR: CPSR_Mode_SVC)
    PUSH    {R2}         ; Push old task PC,
    AND     R2, R1, #CPSR_Mode_MASK
    CMP     R2, #CPSR_Mode_USR
    BNE     svc_mode_interrupt

    ; user mode
    SUB     SP, SP, #0x08
    MOV     R2, SP
    STMIA   R2, {SP, LR}^
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

    IF   {FPU} != "SoftVFP"
    ; Push task fpu register.
    PUSH_FP_REG R1
;    #endif
    ENDIF

    ; if (g_sys_stat == RHINO_RUNNING)
    LDR     R3, =g_sys_stat
    LDR     R4, [R3]
    CMP     R4, #3  ; RHINO_RUNNING = 3
    BNE     _interrupt_while_init

_interrupt_while_task
    ; g_active_task->task_stack = context region
    LDR     R3, =g_active_task

    getcoreid   R5
    LSL     R5,R5,#2
    ADD     R3, R3, R5

    LDR     R4, [R3]
    STR     SP, [R4, #RHINO_CONFIG_TASK_KSTACK_OFFSET]

    ; Switch to system stack.
    LDR     R3, =sys_stack_top
    MOV     R4, #RHINO_EACHCORE_SYSTEM_STACK_SIZE
    MUL     R4, R4, R5
    SUB     R3, R3, R4

    MOV     SP, R3

    ; cpu_interrupt_handler(except_type = R0)
    ;BL      cpu_interrupt_handler
    PUSH    {R0}
    BL      krhino_intrpt_enter
    POP     {R0}
    BL      aos_cpu_except_handler
    BL      krhino_intrpt_exit 

_interrupt_return_address

    ; SP = g_active_task->task_stack;
    LDR     R3, =g_active_task
    getcoreid   R4
    LSL     R4,R4,#2
    ADD     R3, R3, R4

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
    BL      aos_cpu_except_handler
    ;BL      dpu_interrupt_handler
    POP     {R1, LR}
    ADD     SP, SP, R1

    BL      _context_restore

_panic_handler
    ; change to SVC mode & disable interruptions.
    MSR     CPSR_c, #(CPSR_INT_DIS :OR: CPSR_Mode_SVC)

    PUSH    {R2}          ; Push old task PC,
    ADD     R2, SP, #0x04
    PUSH    {LR}          ; Push old task LR,
    PUSH    {R2}          ; Push old sp
    PUSH    {R4-R12}      ; Push old task R12-R4,
    LDMFD   R3!, {R5-R8}  ; Pop old task R3-R0 from mode stack.
    PUSH    {R5-R8}       ; Push old task R3-R0,
    PUSH    {R1}          ; Push task CPSR.

    IF   {FPU} != "SoftVFP"
    PUSH_FP_REG R1        ; Push task fpu register.
    ;#endif
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



  ; int cpu_get_cpuid(void)@
  ; get current CPU ID
cpu_get_cpuid
    mrc   p15, 0, r0, c0, c0, 5
    and   r0, r0, #3
    BX    lr

    END
