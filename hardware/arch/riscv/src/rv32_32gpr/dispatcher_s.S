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
.global ht_dispatcher
.global sys_stack_base
.global sys_stack_top
.global exception_stack_base
.global exception_stack_top

/******************************************************************************
@ Exception Vector. mtvec.mode must be set to 0, exceptions and interrupts use
@ the same vector entry
@******************************************************************************/

#define CONFIG_ARCH_INTERRUPTSTACK 4096
.section .bss
.align 3

sys_stack_base:
.space CONFIG_ARCH_INTERRUPTSTACK
 sys_stack_top:

exception_stack_base:
.space CONFIG_ARCH_INTERRUPTSTACK
exception_stack_top:

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
#ifdef RISCV_SUPPORT_FPU
    addi    sp, sp, -(FLOAT_CONTEXT_LEN)

    fsw      f0, (F0_OFFSET)(sp)
    fsw      f1, (F1_OFFSET)(sp)
    fsw      f2, (F2_OFFSET)(sp)
    fsw      f3, (F3_OFFSET)(sp)
    fsw      f4, (F4_OFFSET)(sp)
    fsw      f5, (F5_OFFSET)(sp)
    fsw      f6, (F6_OFFSET)(sp)
    fsw      f7, (F7_OFFSET)(sp)
    fsw      f8, (F8_OFFSET)(sp)
    fsw      f9, (F9_OFFSET)(sp)
    fsw      f10,(F10_OFFSET)(sp)
    fsw      f11,(F11_OFFSET)(sp)
    fsw      f12,(F12_OFFSET)(sp)
    fsw      f13,(F13_OFFSET)(sp)
    fsw      f14,(F14_OFFSET)(sp)
    fsw      f15,(F15_OFFSET)(sp)
    fsw      f16,(F16_OFFSET)(sp)
    fsw      f17,(F17_OFFSET)(sp)
    fsw      f18,(F18_OFFSET)(sp)
    fsw      f19,(F19_OFFSET)(sp)
    fsw      f20,(F20_OFFSET)(sp)
    fsw      f21,(F21_OFFSET)(sp)
    fsw      f22,(F22_OFFSET)(sp)
    fsw      f23,(F23_OFFSET)(sp)
    fsw      f24,(F24_OFFSET)(sp)
    fsw      f25,(F25_OFFSET)(sp)
    fsw      f26,(F26_OFFSET)(sp)
    fsw      f27,(F27_OFFSET)(sp)
    fsw      f28,(F28_OFFSET)(sp)
    fsw      f29,(F29_OFFSET)(sp)
    fsw      f30,(F30_OFFSET)(sp)
    fsw      f31,(F31_OFFSET)(sp)
#endif
    addi    sp, sp, -(BASE_CONTEXT_LEN)

    sw     x1,  (X1_OFFSET)(sp)
    sw     x4,  (X4_OFFSET)(sp)
    sw     x5,  (X5_OFFSET)(sp)
    sw     x6,  (X6_OFFSET)(sp)
    sw     x7,  (X7_OFFSET)(sp)
    sw     x8,  (X8_OFFSET)(sp)
    sw     x9,  (X9_OFFSET)(sp)
    sw     x10, (X10_OFFSET)(sp)
    sw     x11, (X11_OFFSET)(sp)
    sw     x12, (X12_OFFSET)(sp)
    sw     x13, (X13_OFFSET)(sp)
    sw     x14, (X14_OFFSET)(sp)
    sw     x15, (X15_OFFSET)(sp)
    sw     x16, (X16_OFFSET)(sp)
    sw     x17, (X17_OFFSET)(sp)
    sw     x18, (X18_OFFSET)(sp)
    sw     x19, (X19_OFFSET)(sp)
    sw     x20, (X20_OFFSET)(sp)
    sw     x21, (X21_OFFSET)(sp)
    sw     x22, (X22_OFFSET)(sp)
    sw     x23, (X23_OFFSET)(sp)
    sw     x24, (X24_OFFSET)(sp)
    sw     x25, (X25_OFFSET)(sp)
    sw     x26, (X26_OFFSET)(sp)
    sw     x27, (X27_OFFSET)(sp)
    sw     x28, (X28_OFFSET)(sp)
    sw     x29, (X29_OFFSET)(sp)
    sw     x30, (X30_OFFSET)(sp)
    sw     x31, (X31_OFFSET)(sp)

    csrr    t0, mstatus
    sw      t0, (MSTATUS_OFFSET)(sp)

    /* g_active_task->task_stack = context region */
    la      a1, g_active_task
    lw      a1, (a1)
    sw      sp, (a1)

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
    sw      t1, (PC_OFFSET)(sp)

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
    sw      t1, (PC_OFFSET)(sp)


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
    lw      a2, (a1)
    sw      a2, (a0)

task_restore:
    la      a1, g_active_task
    lw      a1, (a1)
    lw      sp, (a1)

    lw      t0, (MSTATUS_OFFSET)(sp)
    csrw    mstatus, t0

    lw      t0, (PC_OFFSET)(sp)
    csrw    mepc, t0

    lw     x1,  (X1_OFFSET)(sp)
    lw     x4,  (X4_OFFSET)(sp)
    lw     x5,  (X5_OFFSET)(sp)
    lw     x6,  (X6_OFFSET)(sp)
    lw     x7,  (X7_OFFSET)(sp)
    lw     x8,  (X8_OFFSET)(sp)
    lw     x9,  (X9_OFFSET)(sp)
    lw     x10, (X10_OFFSET)(sp)
    lw     x11, (X11_OFFSET)(sp)
    lw     x12, (X12_OFFSET)(sp)
    lw     x13, (X13_OFFSET)(sp)
    lw     x14, (X14_OFFSET)(sp)
    lw     x15, (X15_OFFSET)(sp)
    lw     x16, (X16_OFFSET)(sp)
    lw     x17, (X17_OFFSET)(sp)
    lw     x18, (X18_OFFSET)(sp)
    lw     x19, (X19_OFFSET)(sp)
    lw     x20, (X20_OFFSET)(sp)
    lw     x21, (X21_OFFSET)(sp)
    lw     x22, (X22_OFFSET)(sp)
    lw     x23, (X23_OFFSET)(sp)
    lw     x24, (X24_OFFSET)(sp)
    lw     x25, (X25_OFFSET)(sp)
    lw     x26, (X26_OFFSET)(sp)
    lw     x27, (X27_OFFSET)(sp)
    lw     x28, (X28_OFFSET)(sp)
    lw     x29, (X29_OFFSET)(sp)
    lw     x30, (X30_OFFSET)(sp)
    lw     x31, (X31_OFFSET)(sp)

    addi    sp, sp, (BASE_CONTEXT_LEN)

#ifdef RISCV_SUPPORT_FPU
    flw      f0, (F0_OFFSET)(sp)
    flw      f1, (F1_OFFSET)(sp)
    flw      f2, (F2_OFFSET)(sp)
    flw      f3, (F3_OFFSET)(sp)
    flw      f4, (F4_OFFSET)(sp)
    flw      f5, (F5_OFFSET)(sp)
    flw      f6, (F6_OFFSET)(sp)
    flw      f7, (F7_OFFSET)(sp)
    flw      f8, (F8_OFFSET)(sp)
    flw      f9, (F9_OFFSET)(sp)
    flw      f10,(F10_OFFSET)(sp)
    flw      f11,(F11_OFFSET)(sp)
    flw      f12,(F12_OFFSET)(sp)
    flw      f13,(F13_OFFSET)(sp)
    flw      f14,(F14_OFFSET)(sp)
    flw      f15,(F15_OFFSET)(sp)
    flw      f16,(F16_OFFSET)(sp)
    flw      f17,(F17_OFFSET)(sp)
    flw      f18,(F18_OFFSET)(sp)
    flw      f19,(F19_OFFSET)(sp)
    flw      f20,(F20_OFFSET)(sp)
    flw      f21,(F21_OFFSET)(sp)
    flw      f22,(F22_OFFSET)(sp)
    flw      f23,(F23_OFFSET)(sp)
    flw      f24,(F24_OFFSET)(sp)
    flw      f25,(F25_OFFSET)(sp)
    flw      f26,(F26_OFFSET)(sp)
    flw      f27,(F27_OFFSET)(sp)
    flw      f28,(F28_OFFSET)(sp)
    flw      f29,(F29_OFFSET)(sp)
    flw      f30,(F30_OFFSET)(sp)
    flw      f31,(F31_OFFSET)(sp)

    addi    sp, sp, (FLOAT_CONTEXT_LEN)
#endif
    mret
    .endfunc

