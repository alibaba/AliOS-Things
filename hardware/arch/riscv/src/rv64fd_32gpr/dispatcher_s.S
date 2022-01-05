#include <k_config.h>
#include <k_arch.h>

/******************************************************************************
@ Declares the function that this file will call
@******************************************************************************/
.extern g_active_task
.extern g_preferred_ready_task
.extern krhino_stack_ovf_check
.extern krhino_task_sched_stats_get
.extern exception_stack_top
.extern dispatcher_exception
.extern dispatcher_interrupt

.global task_restore
.global _interrupt_return_address

/******************************************************************************
@ Exception Vector. mtvec.mode must be set to 0, exceptions and interrupts use
@ the same vector entry
@******************************************************************************/
.section .vectors
    .align  6
    .globl  __Vectors
    .type   __Vectors, @object
__Vectors:
    j   ht_dispatcher

/******************************************************************************
@ Jump to this function when an exception or interrupt occurs. This function is
@ used to schedule the execution flow of exceptions, interrupts, and tasks.
@ System call 0 is used to trigger task scheduling.
@******************************************************************************/
.text
.align 8
.func
ht_dispatcher:
    /* first step: save context */
    addi    sp, sp, -(FLOAT_CONTEXT_LEN)

    fsd      f0, (F0_OFFSET)(sp)
    fsd      f1, (F1_OFFSET)(sp)
    fsd      f2, (F2_OFFSET)(sp)
    fsd      f3, (F3_OFFSET)(sp)
    fsd      f4, (F4_OFFSET)(sp)
    fsd      f5, (F5_OFFSET)(sp)
    fsd      f6, (F6_OFFSET)(sp)
    fsd      f7, (F7_OFFSET)(sp)
    fsd      f8, (F8_OFFSET)(sp)
    fsd      f9, (F9_OFFSET)(sp)
    fsd      f10,(F10_OFFSET)(sp)
    fsd      f11,(F11_OFFSET)(sp)
    fsd      f12,(F12_OFFSET)(sp)
    fsd      f13,(F13_OFFSET)(sp)
    fsd      f14,(F14_OFFSET)(sp)
    fsd      f15,(F15_OFFSET)(sp)
    fsd      f16,(F16_OFFSET)(sp)
    fsd      f17,(F17_OFFSET)(sp)
    fsd      f18,(F18_OFFSET)(sp)
    fsd      f19,(F19_OFFSET)(sp)
    fsd      f20,(F20_OFFSET)(sp)
    fsd      f21,(F21_OFFSET)(sp)
    fsd      f22,(F22_OFFSET)(sp)
    fsd      f23,(F23_OFFSET)(sp)
    fsd      f24,(F24_OFFSET)(sp)
    fsd      f25,(F25_OFFSET)(sp)
    fsd      f26,(F26_OFFSET)(sp)
    fsd      f27,(F27_OFFSET)(sp)
    fsd      f28,(F28_OFFSET)(sp)
    fsd      f29,(F29_OFFSET)(sp)
    fsd      f30,(F30_OFFSET)(sp)
    fsd      f31,(F31_OFFSET)(sp)

    addi    sp, sp, -(BASE_CONTEXT_LEN)

    sd     x1,  (X1_OFFSET)(sp)
    sd     x4,  (X4_OFFSET)(sp)
    sd     x5,  (X5_OFFSET)(sp)
    sd     x6,  (X6_OFFSET)(sp)
    sd     x7,  (X7_OFFSET)(sp)
    sd     x8,  (X8_OFFSET)(sp)
    sd     x9,  (X9_OFFSET)(sp)
    sd     x10, (X10_OFFSET)(sp)
    sd     x11, (X11_OFFSET)(sp)
    sd     x12, (X12_OFFSET)(sp)
    sd     x13, (X13_OFFSET)(sp)
    sd     x14, (X14_OFFSET)(sp)
    sd     x15, (X15_OFFSET)(sp)
    sd     x16, (X16_OFFSET)(sp)
    sd     x17, (X17_OFFSET)(sp)
    sd     x18, (X18_OFFSET)(sp)
    sd     x19, (X19_OFFSET)(sp)
    sd     x20, (X20_OFFSET)(sp)
    sd     x21, (X21_OFFSET)(sp)
    sd     x22, (X22_OFFSET)(sp)
    sd     x23, (X23_OFFSET)(sp)
    sd     x24, (X24_OFFSET)(sp)
    sd     x25, (X25_OFFSET)(sp)
    sd     x26, (X26_OFFSET)(sp)
    sd     x27, (X27_OFFSET)(sp)
    sd     x28, (X28_OFFSET)(sp)
    sd     x29, (X29_OFFSET)(sp)
    sd     x30, (X30_OFFSET)(sp)
    sd     x31, (X31_OFFSET)(sp)

    csrr    t0, mstatus
    sd      t0, (MSTATUS_OFFSET)(sp)

    /* g_active_task->task_stack = context region */
    la      a1, g_active_task
    ld      a1, (a1)
    sd      sp, (a1)

    /* exception or interrupt is determined by the mcause register */
    csrr    t0, mcause
    blt     t0, x0, interrupt

    /* second step: process exception. ecall 0 is used to trig task sched */
    /* Save the exception return address, the lenght of instruction may be 2 or 4 byte*/

    /* read the instruction which trigger the exception */
    csrr    t1, mepc
    lb      t2, 0(t1)
    li      t3, 0x3
    and     t2, t2, t3
    bne     t2, t3, 1f
    addi    t1, t1, 0x2
1:
    addi    t1, t1, 0x2
2:
    sd      t1, (PC_OFFSET)(sp)

    /* use exception statck to process exception */
    mv      a2, sp
    la      sp, exception_stack_top
    mv      a1, t0
    la      t0, dispatcher_exception
    jalr    t0
    beq     x0,x0, task_sched

    /* third step: process interrupt */
interrupt:
    csrr    t1, mepc
    sd      t1, (PC_OFFSET)(sp)

    /* use sys statck to process interrupt */
    la      sp, sys_stack_top
    andi    a0, t0, 0x7FF
    la      t0, dispatcher_interrupt
    jalr    t0
_interrupt_return_address:

    /* forth step: task sched */
task_sched:
#if (RHINO_CONFIG_TASK_STACK_OVF_CHECK > 0)
    call      krhino_stack_ovf_check
#endif
#if (RHINO_CONFIG_SYS_STATS > 0)
    call      krhino_task_sched_stats_get
#endif

    /* save context: R0 = g_active_task->task_stack = context region */
    la      a0, g_active_task
    la      a1, g_preferred_ready_task
    ld      a2, (a1)
    sd      a2, (a0)

task_restore:
    la      a1, g_active_task
    ld      a1, (a1)
    ld      sp, (a1)

    ld      t0, (MSTATUS_OFFSET)(sp)
    csrw    mstatus, t0

    ld      t0, (PC_OFFSET)(sp)
    csrw    mepc, t0

    ld     x1,  (X1_OFFSET)(sp)
    ld     x4,  (X4_OFFSET)(sp)
    ld     x5,  (X5_OFFSET)(sp)
    ld     x6,  (X6_OFFSET)(sp)
    ld     x7,  (X7_OFFSET)(sp)
    ld     x8,  (X8_OFFSET)(sp)
    ld     x9,  (X9_OFFSET)(sp)
    ld     x10, (X10_OFFSET)(sp)
    ld     x11, (X11_OFFSET)(sp)
    ld     x12, (X12_OFFSET)(sp)
    ld     x13, (X13_OFFSET)(sp)
    ld     x14, (X14_OFFSET)(sp)
    ld     x15, (X15_OFFSET)(sp)
    ld     x16, (X16_OFFSET)(sp)
    ld     x17, (X17_OFFSET)(sp)
    ld     x18, (X18_OFFSET)(sp)
    ld     x19, (X19_OFFSET)(sp)
    ld     x20, (X20_OFFSET)(sp)
    ld     x21, (X21_OFFSET)(sp)
    ld     x22, (X22_OFFSET)(sp)
    ld     x23, (X23_OFFSET)(sp)
    ld     x24, (X24_OFFSET)(sp)
    ld     x25, (X25_OFFSET)(sp)
    ld     x26, (X26_OFFSET)(sp)
    ld     x27, (X27_OFFSET)(sp)
    ld     x28, (X28_OFFSET)(sp)
    ld     x29, (X29_OFFSET)(sp)
    ld     x30, (X30_OFFSET)(sp)
    ld     x31, (X31_OFFSET)(sp)

    addi    sp, sp, (BASE_CONTEXT_LEN)

    fld      f0, (F0_OFFSET)(sp)
    fld      f1, (F1_OFFSET)(sp)
    fld      f2, (F2_OFFSET)(sp)
    fld      f3, (F3_OFFSET)(sp)
    fld      f4, (F4_OFFSET)(sp)
    fld      f5, (F5_OFFSET)(sp)
    fld      f6, (F6_OFFSET)(sp)
    fld      f7, (F7_OFFSET)(sp)
    fld      f8, (F8_OFFSET)(sp)
    fld      f9, (F9_OFFSET)(sp)
    fld      f10,(F10_OFFSET)(sp)
    fld      f11,(F11_OFFSET)(sp)
    fld      f12,(F12_OFFSET)(sp)
    fld      f13,(F13_OFFSET)(sp)
    fld      f14,(F14_OFFSET)(sp)
    fld      f15,(F15_OFFSET)(sp)
    fld      f16,(F16_OFFSET)(sp)
    fld      f17,(F17_OFFSET)(sp)
    fld      f18,(F18_OFFSET)(sp)
    fld      f19,(F19_OFFSET)(sp)
    fld      f20,(F20_OFFSET)(sp)
    fld      f21,(F21_OFFSET)(sp)
    fld      f22,(F22_OFFSET)(sp)
    fld      f23,(F23_OFFSET)(sp)
    fld      f24,(F24_OFFSET)(sp)
    fld      f25,(F25_OFFSET)(sp)
    fld      f26,(F26_OFFSET)(sp)
    fld      f27,(F27_OFFSET)(sp)
    fld      f28,(F28_OFFSET)(sp)
    fld      f29,(F29_OFFSET)(sp)
    fld      f30,(F30_OFFSET)(sp)
    fld      f31,(F31_OFFSET)(sp)

    addi    sp, sp, (FLOAT_CONTEXT_LEN)

    mret
    .endfunc

