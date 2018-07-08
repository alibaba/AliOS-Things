/*
*
* @file port_s.S
*
*/

@******************************************************************************
@                            EXTERN PARAMETERS
@******************************************************************************

.extern  g_active_task
.extern  g_preferred_ready_task
.extern  krhino_stack_ovf_check

@******************************************************************************
@                            EXPORT FUNCTIONS
@******************************************************************************

.global  fpu_reg_count
.global  cpu_intrpt_save
.global  cpu_intrpt_restore
.global  cpu_task_switch
.global  cpu_intrpt_switch
.global  cpu_first_task_start

.global  vector_undef
.global  vector_swi
.global  vector_pabt
.global  vector_dabt
.global  vector_resv
.global  vector_irq
.global  vector_fiq

.global  except_sp

@******************************************************************************
@                                 EQUATES
@******************************************************************************

.equ AMR_Mode_USR,               0x10
.equ AMR_Mode_FIQ,               0x11
.equ AMR_Mode_IRQ,               0x12
.equ AMR_Mode_SVC,               0x13
.equ AMR_Mode_ABT,               0x17
.equ AMR_Mode_UND,               0x1B
.equ AMR_Mode_SYS,               0x1F

.equ ARM_CONTROL_INT_DIS,        0xC0                     @ Disable both FIQ and IRQ.
.equ ARM_CONTROL_FIQ_DIS,        0x40                     @ Disable FIQ.
.equ ARM_CONTROL_IRQ_DIS,        0x80                     @ Disable IRQ.
.equ ARM_CONTROL_THUMB,          0x20                     @ Set THUMB mode.
.equ ARM_CONTROL_ARM,            0x00                     @ Set ARM mode.

.equ ARM_EXCEPT_RESET,           0x00
.equ ARM_EXCEPT_UNDEF_INSTR,     0x01
.equ ARM_EXCEPT_SWI,             0x02
.equ ARM_EXCEPT_PREFETCH_ABORT,  0x03
.equ ARM_EXCEPT_DATA_ABORT,      0x04
.equ ARM_EXCEPT_ADDR_ABORT,      0x05
.equ ARM_EXCEPT_IRQ,             0x06
.equ ARM_EXCEPT_FIQ,             0x07

@******************************************************************************
@                        DATA
@******************************************************************************

.section .data
.align  2
except_sp:
.rept   1
.long   0x0
.endr

@******************************************************************************
@                        CODE GENERATION DIRECTIVES
@******************************************************************************

.text
.align 2

@******************************************************************************
@                        MACRO DEFINED
@******************************************************************************

.macro POP_FP_REG reg
    POP     {\reg}
    VMSR    FPEXC, \reg                     /* Pop FPEXC */
    FLDMIAS SP!, {S0-S31}                  /* Pop floating point registers. */
    POP     {\reg}
    VMSR    FPSCR, \reg                     /* Pop  FPSCR. */
.endm

.macro PUSH_FP_REG reg
    VMRS    \reg, FPSCR                     /* Save FPSCR */
    PUSH    {\reg}                          /* Save floating-point registers. */
    FSTMDBS SP!, {S0-S31}
    VMRS    \reg, FPEXC                     /* Save FPEXC. */
    PUSH    {\reg}
.endm

@******************************************************************************
@ Functions:
@     size_t fpu_reg_count(void);
@******************************************************************************
fpu_reg_count:
    MOV     R0, #16
    BX      LR

@******************************************************************************
@ Functions:
@     size_t cpu_intrpt_save(void);
@     void cpu_intrpt_restore(size_t cpsr);
@******************************************************************************
cpu_intrpt_save:
    mrs r0, cpsr
    CPSID   IF
    dsb
    bx  lr

cpu_intrpt_restore:
    dsb
    msr cpsr, r0
    bx  lr

@******************************************************************************
@ Functions:
@     void   cpu_first_task_start(void);
@******************************************************************************
cpu_first_task_start:
    MSR     CPSR_c, #(ARM_CONTROL_INT_DIS | AMR_Mode_SVC)                   @ change to SVC mode.

    @ switch to highest priority task:
    LDR     R0, =g_active_task                                              @ g_active_task = g_preferred_ready_task;
    LDR     R1, =g_preferred_ready_task
    LDR     R2, [R1]
    STR     R2, [R0]                                                        @ R0 = g_active_task->task_stack = context region

    LDR     SP, [R2]

    #if (defined(__VFP_FP__) && !defined(__SOFTFP__))
    POP_FP_REG R0                                                           @ Pop fpu register.
    #endif

    LDR     R0, [SP], #4                                                    @ R0 = SPSR; SP = SP + 4

    MSR     SPSR_cxsf, R0                                                   @ restore new task CPSR

    LDMFD   SP!, {R0-R12, LR, PC}^                                          @ restore new task context.

@******************************************************************************
@ Functions:
@     void cpu_task_switch(void);
@******************************************************************************
cpu_task_switch:
    @ save current task context:
    STMFD   SP!, {LR}                                           @ Push return address.
    STMFD   SP!, {LR}
    STMFD   SP!, {R0-R12}                                       @ Push R0-R12 registers
    MRS     R0, CPSR                                            @ Push old task CPSR
    TST     LR, #1                                              @ test if called from Thumb mode,
    ORRNE   R0, R0, #ARM_CONTROL_THUMB                   @ if yes, set the T-bit.
    STMFD   SP!, {R0}

    #if (defined(__VFP_FP__) && !defined(__SOFTFP__))
    PUSH_FP_REG R0                                              @ Push fpu register.
    #endif

    @g_active_task->task_stack = context region
    LDR     R1, =g_active_task                                  @ g_active_task->task_stack = SP;
    LDR     R1, [R1]
    STR     SP, [R1]

    bl      krhino_stack_ovf_check

    LDR     R0, =g_active_task                                  @ g_active_task = g_preferred_ready_task;
    LDR     R1, =g_preferred_ready_task
    LDR     R2, [R1]
    STR     R2, [R0]                                             @ R0 = g_active_task->task_stack = context region

    LDR     SP, [R2]

    #if (defined(__VFP_FP__) && !defined(__SOFTFP__))
    POP_FP_REG R0                                               @ Pop fpu register.
    #endif

    LDMFD   SP!, {R0}                                           @ restore CPSR
    MSR     SPSR_cxsf, R0

    LDMFD   SP!, {R0-R12, LR, PC}^                              @ restore new task context.

@******************************************************************************
@ Functions:
@     void   cpu_intrpt_switch(void);
@******************************************************************************
cpu_intrpt_switch:
    LDR     R0, =g_active_task                                  @ g_active_task = g_preferred_ready_task;
    LDR     R1, =g_preferred_ready_task
    LDR     R2, [R1]
    STR     R2, [R0]                                             @ R0 = g_active_task->task_stack = context region

    LDR     SP, [R2]

    #if (defined(__VFP_FP__) && !defined(__SOFTFP__))
    POP_FP_REG R0                                               @ Pop fpu register.
    #endif

    LDMFD   SP!, {R0}                                           @ Pop new task CPSR
    MSR     SPSR_cxsf, R0

    LDMFD   SP!, {R0-R12, LR, PC}^                              @ restore new task context.


.section .text.isr, "ax"
/* exception handlers: undef, swi, padt, dabt, resv, irq, fiq */
.align  5
vector_undef:
    STMFD   SP!, {R0-R3}                                        @ Push R0-R3 registers.
    MOV     R0, #ARM_EXCEPT_UNDEF_INSTR                         @ Set exception type to ARM_EXCEPT_UNDEF_INSTR.
    MRS     R1, SPSR                                            @ Save CPSR
    MOV     R2, LR                                              @ Save LR(PC) register.
    MOV     R3, SP                                              @ Save SP register.
    ADD     SP, SP, #(4 * 4)                                    @ set SP to undef stack top.
    B       common_except_handler                               @ bl to common_except_handler.

.align  5
vector_swi:
    STMFD   SP!, {R0-R3}                                        @ Push R0-R3 registers.
    MOV     R0, #ARM_EXCEPT_SWI                                 @ Set exception type to ARM_EXCEPT_SWI.
    MRS     R1, SPSR                                            @ Save CPSR
    MOV     R2, LR                                              @ Save LR(PC) register.
    MOV     R3, SP                                              @ Save SP register.
    ADD     SP, SP, #(4 * 4)                                    @ set SP to swi stack top.
    B       common_except_handler                               @ bl to common_except_handler.

.align  5
vector_pabt:
    STMFD   SP!, {R0-R3}                                        @ Push R0-R3 registers.
    MOV     R0, #ARM_EXCEPT_PREFETCH_ABORT                      @ Set exception type to ARM_EXCEPT_PREFETCH_ABORT.
    MRS     R1, SPSR                                            @ Save CPSR.
    SUB     R2, LR, #4                                          @ Save LR(PC) register: -4.
    MOV     R3, SP                                              @ Save SP register.
    ADD     SP, SP, #(4 * 4)                                    @ set SP to padt stack top.
    B       common_except_handler                               @ bl to common_except_handler.

.align  5
vector_dabt:
    STMFD   SP!, {R0-R3}                                        @ Push R0-R3 registers.
    MOV     R0, #ARM_EXCEPT_DATA_ABORT                          @ Set exception type to ARM_EXCEPT_DATA_ABORT.
    MRS     R1, SPSR                                            @ Save CPSR.
    SUB     R2, LR, #8                                          @ Save LR(PC) register: -8.
    MOV     R3, SP                                              @ Save SP register.
    ADD     SP, SP, #(4 * 4)                                    @ set SP to dabt stack top.
    B       common_except_handler                               @ bl to common_except_handler.

.align  5
vector_resv:
    STMFD   SP!, {R0-R3}                                        @ Push R0-R3 registers.
    MOV     R0, #ARM_EXCEPT_ADDR_ABORT                          @ Set exception type to ARM_EXCEPT_ADDR_ABORT.
    MRS     R1, SPSR                                            @ Save CPSR.
    SUB     R2, LR, #8                                          @ Save LR(PC) register: -8.
    MOV     R3, SP                                              @ Save SP register.
    ADD     SP, SP, #(4 * 4)                                    @ set SP to resv stack top.
    B       common_except_handler                               @ bl to common_except_handler.

.align  5
vector_irq:
    STMFD   SP!, {R0-R3}                                        @ Push R0-R3 registers.
    MOV     R0, #ARM_EXCEPT_IRQ                                 @ Set exception type to ARM_EXCEPT_IRQ.
    MRS     R1, SPSR                                            @ Save CPSR.
    SUB     R2, LR, #4                                          @ Save LR(PC) register: -4.
    MOV     R3, SP                                              @ Save SP register.
    ADD     SP, SP, #(4 * 4)                                    @ set SP to irq stack top.
    B       common_except_handler                               @ bl to common_except_handler.

.align  5
vector_fiq:
    STMFD   SP!, {R0-R3}                                        @ Push R0-R3 registers.
    MOV     R0, #ARM_EXCEPT_FIQ                                 @ Set exception type to ARM_EXCEPT_FIQ.
    MRS     R1, SPSR                                            @ Save CPSR.
    SUB     R2, LR, #4                                          @ Save LR(PC) register: -4.
    MOV     R3, SP                                              @ Save SP register.
    ADD     SP, SP, #(4 * 4)                                    @ set SP to fiq stack top.
    B       common_except_handler                               @ bl to common_except_handler.

common_except_handler:
        @ change to SVC mode & disable interruptions.
        MSR     CPSR_c, #(ARM_CONTROL_INT_DIS | AMR_Mode_SVC)

        STMFD   SP!, {R2}                                        @   Push old task PC,
        STMFD   SP!, {LR}                                        @   Push old task LR,
        STMFD   SP!, {R4-R12}                                    @   Push old task R12-R4,
        LDMFD   R3!, {R5-R8}                                     @   Pop old task R3-R0 from irq stack.
        STMFD   SP!, {R5-R8}                                     @   Push old task R3-R0,
        STMFD   SP!, {R1}                                        @   Push task CPSR.

        #if (defined(__VFP_FP__) && !defined(__SOFTFP__))
        PUSH_FP_REG R1                                            @  Push task fpu register.
        #endif
                                                                  @ if (g_sys_stat == RHINO_RUNNING)
        LDR     R3, =g_sys_stat
        LDR     R4, [R3]
        CMP     R4, #3                                            @ RHINO_RUNNING = 3
        BNE     except_before_task_running

        PUSH    {R0}
        BL      krhino_intrpt_enter                               @ g_intrpt_nested_level++;
        POP     {R0}

        LDR     R3,=g_intrpt_nested_level
        LDRB    R4, [R3]
        CMP     R4, #1                                            @ if (g_intrpt_nested_level == 1)
        BNE     except_from_intrpt

except_from_task:
        @ g_active_task->task_stack = context region
        LDR     R3, =g_active_task                                @ g_active_task->task_stack = SP;
        LDR     R4, [R3]
        STR     SP, [R4]

        LDR     R3, =except_stack_top                             @ Switch to except stack.
        MOV     SP, R3

        BL      aos_cpu_except_handler                             @ aos_cpu_except_handler(except_type = R0)

        @ change to SVC mode & disable interruptions.
        MSR     CPSR_c, #(ARM_CONTROL_INT_DIS | AMR_Mode_SVC)

        @ call krhino_intrpt_exit() to return if a ready task with higher priority.
        BL      krhino_intrpt_exit

        LDR     R3, =g_active_task                                 @ SP = g_active_task->task_stack;
        LDR     R4, [R3]
        LDR     SP, [R4]

        #if (defined(__VFP_FP__) && !defined(__SOFTFP__))
        POP_FP_REG R0                                              @  Pop task fpu register.
        #endif

        LDMFD   SP!, {R0}                                          @ Pop new task CPSR,
        MSR     SPSR_cxsf, R0

        LDMFD   SP!, {R0-R12, LR, PC}^                             @ restore new task context.

except_from_intrpt:
        LDR     R1, =except_sp
        STR     SP, [R1]

        @ align SP to 8 byte
        MOV     R1, SP
        AND     R1, R1, #4
        SUB     SP, SP, R1
        STMFD   SP!, {R1, LR}

        BL      aos_cpu_except_handler                               @ aos_cpu_except_handler(except_type = R0)

        LDMIA   SP!, {R1, LR}
        ADD     SP, SP, R1

        @ change to SVC mode & disable interruptions.
        MSR     CPSR_c, #(ARM_CONTROL_INT_DIS | AMR_Mode_SVC)

        LDR     R3,=g_intrpt_nested_level                           @ g_intrpt_nested_level--;
        LDRB     R4, [R3]
        SUB      R4, R4, #1
        STRB     R4, [R3]

        #if (defined(__VFP_FP__) && !defined(__SOFTFP__))
        POP_FP_REG R0                                               @  Pop task fpu register.
        #endif

        LDMFD   SP!, {R0}                                           @ Pop old task CPSR,
        MSR     SPSR_cxsf, R0

        LDMFD   SP!, {R0-R12, LR, PC}^                              @ restore working registers and return from exception.


except_before_task_running:
        LDR     R1, =except_sp
        STR     SP, [R1]

        @ align SP to 8 byte.
        MOV     R1, SP
        AND     R1, R1, #4
        SUB     SP, SP, R1
        STMFD   SP!, {R1, LR}

        LDR     R3,=aos_cpu_except_handler                           @ aos_cpu_except_handler(except_type = R0)
        MOV     LR, PC
        BX      R3

        LDMIA   SP!, {R1, LR}
        ADD     SP, SP, R1

        @ change to SVC mode & disable interruptions.
        MSR     CPSR_c, #(ARM_CONTROL_INT_DIS | AMR_Mode_SVC)

        #if (defined(__VFP_FP__) && !defined(__SOFTFP__))
        POP_FP_REG R0                                               @  Pop task fpu register.
        #endif

        LDMFD   SP!, {R0}                                           @ Pop old CPSR,
        MSR     SPSR_cxsf, R0

        LDMFD   SP!, {R0-R12, LR, PC}^                              @ restore working registers and return from exception.


