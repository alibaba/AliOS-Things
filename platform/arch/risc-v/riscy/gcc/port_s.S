/*
 *
 * @file port_s.S
 *
 */
#include "soc_impl_remap.h"

/******************************************************************************
@                            EXTERN PARAMETERS
@******************************************************************************/

.extern  g_active_task
.extern  g_preferred_ready_task
.extern  g_intrpt_nested_level
.extern  krhino_stack_ovf_check
.extern  aos_level2_irq_handler

/******************************************************************************
@                            EXPORT FUNCTIONS
@******************************************************************************/

.global  cpu_intrpt_save
.global  cpu_intrpt_restore
.global  cpu_task_switch
.global  cpu_first_task_start
.global  aos_level1_irq_handler

/******************************************************************************
@                                 EQUATES
@******************************************************************************/

.equ RISCV_MSTATUS_MIE,        (1<<3)       /*machine-level interrupt bit*/
.equ RISCV_MSTATUS_MPIE,       (1<<7)       /*machine-level pre interrupt bit*/
.equ RISCV_MSTATUS_MPP,        (0x3<<11)    /*machine-level MPP bit*/

.equ LOG_REGBYTES,             (2)
.equ REGBYTES,                 (1<<LOG_REGBYTES)

/******************************************************************************
@                            DATA
@******************************************************************************/
.data
tsp:          .word 0 /* Task stack pointer */
task_running: .byte 0 /* Scheduler started and task is running. */

/******************************************************************************
@                        CODE GENERATION DIRECTIVES
@******************************************************************************/

.text
.align 2

/******************************************************************************
@                        MACRO DEFINED
@******************************************************************************/

.macro save_minimal_context
    addi sp, sp, -0x58
    sw  x1, 0x00(sp)  // ra
    sw  x3, 0x04(sp)  // gp
    sw  x4, 0x08(sp)  // tp
    sw  x5, 0x0c(sp)  // t0
    sw  x6, 0x10(sp)  // t1
    sw  x7, 0x14(sp)  // t2
    sw x10, 0x18(sp)  // a0
    sw x11, 0x1c(sp)  // a1
    sw x12, 0x20(sp)  // a2
    sw x13, 0x24(sp)  // a3
    sw x14, 0x28(sp)  // a4
    sw x15, 0x2c(sp)  // a5
    sw x16, 0x30(sp)  // a6
    sw x17, 0x34(sp)  // a7

    csrr a0, 0x7B0
    csrr a1, 0x7B1
    csrr a2, 0x7B2
    sw a0, 0x38(sp)  // lpstart[0]
    sw a1, 0x3c(sp)  // lpend[0]
    sw a2, 0x40(sp)  // lpcount[0]
    csrr a0, 0x7B4
    csrr a1, 0x7B5
    csrr a2, 0x7B6
    sw a0, 0x44(sp)  // lpstart[1]
    sw a1, 0x48(sp)  // lpend[1]
    sw a2, 0x4c(sp)  // lpcount[1]

    csrr a0, mepc
    sw a0, 0x50(sp)  // mepc
    csrr a0, mstatus
    sw a0, 0x54(sp)  // mstatus
.endm

.macro restore_minimal_context
    lw a0, 0x54(sp)  // mstatus
    csrrw x0, mstatus, a0
    lw a0, 0x50(sp)  // mepc
    csrrw x0, mepc, a0

    lw a0, 0x44(sp)  // lpstart[1]
    lw a1, 0x48(sp)  // lpend[1]
    lw a2, 0x4c(sp)  // lpcount[1]
    csrrw x0, 0x7B4, a0
    csrrw x0, 0x7B5, a1
    csrrw x0, 0x7B6, a2
    lw a0, 0x38(sp)  // lpstart[0]
    lw a1, 0x3c(sp)  // lpend[0]
    lw a2, 0x40(sp)  // lpcount[0]
    csrrw x0, 0x7B0, a0
    csrrw x0, 0x7B1, a1
    csrrw x0, 0x7B2, a2

    lw  x1, 0x00(sp)  // ra
    lw  x3, 0x04(sp)  // gp
    lw  x4, 0x08(sp)  // tp
    lw  x5, 0x0c(sp)  // t0
    lw  x6, 0x10(sp)  // t1
    lw  x7, 0x14(sp)  // t2
    lw x10, 0x18(sp)  // a0
    lw x11, 0x1c(sp)  // a1
    lw x12, 0x20(sp)  // a2
    lw x13, 0x24(sp)  // a3
    lw x14, 0x28(sp)  // a4
    lw x15, 0x2c(sp)  // a5
    lw x16, 0x30(sp)  // a6
    lw x17, 0x34(sp)  // a7
    addi sp, sp, 0x58
.endm

.macro save_extra_context
    lw t0, tsp
    addi t0,   t0, -0x40
    sw   x8,   0x00(t0)
    sw   x9,   0x04(t0)
    sw   x18,  0x08(t0)
    sw   x19,  0x0c(t0)
    sw   x20,  0x10(t0)
    sw   x21,  0x14(t0)
    sw   x22,  0x18(t0)
    sw   x23,  0x1c(t0)
    sw   x24,  0x20(t0)
    sw   x25,  0x24(t0)
    sw   x26,  0x28(t0)
    sw   x27,  0x2c(t0)
    sw   x28,  0x30(t0)
    sw   x29,  0x34(t0)
    sw   x30,  0x38(t0)
    sw   x31,  0x3c(t0)
    lw   t1,   g_active_task
    sw   t0,   (t1)
.endm

.macro restore_extra_context
    /* Before calling this, the sp should be set the g_active_task stack pointer. */
    lw   x8,   0x00(sp)
    lw   x9,   0x04(sp)
    lw   x18,  0x08(sp)
    lw   x19,  0x0c(sp)
    lw   x20,  0x10(sp)
    lw   x21,  0x14(sp)
    lw   x22,  0x18(sp)
    lw   x23,  0x1c(sp)
    lw   x24,  0x20(sp)
    lw   x25,  0x24(sp)
    lw   x26,  0x28(sp)
    lw   x27,  0x2c(sp)
    lw   x28,  0x30(sp)
    lw   x29,  0x34(sp)
    lw   x30,  0x38(sp)
    lw   x31,  0x3c(sp)
    addi sp,   sp, 0x40
.endm

.type aos_level1_irq_handler, %function
aos_level1_irq_handler:

    save_minimal_context

    la a4, g_intrpt_nested_level
    lb a5, (a4)

    /*
     * If enter ISR from task and task schedual started, then switch to
     * use system stack.
     *
     * if ((task_running != 0) && (g_intrpt_nested_level == 0))
     * {
     *     sp = __StackTop;
     * }
     */
    bnez a5, intrpt_enter
    sw sp, tsp, a5
    lb a4, task_running
    beqz a4, intrpt_enter
    la sp, __StackTop

intrpt_enter:
    call krhino_intrpt_enter

    /* Call the interrupt handler. */
    csrr a0, mcause
    jal aos_level2_irq_handler

    call krhino_intrpt_exit

    /* If interrupt nesting, return directly. */
    la a4, g_intrpt_nested_level
    lb a5, (a4)
    bnez a5, end_handler

    /* Did the interrupt request a context switch? */
    lb a4, g_task_switch_required
    beqz a4, exit_without_switch

switch_before_exit:
    /*
     * A context swtich is to be performed. Clear the context switch
     * pending flag.
     */
    sb zero, g_task_switch_required, a5

    save_extra_context
    la    t0, g_active_task                         // g_active_task = g_preferred_ready_task;
    la    t1, g_preferred_ready_task
    lw    t2, (t1)
    sw    t2, (t0)

    lw    sp,   (t2)

    restore_extra_context /* sp is set to task stack pointer here. */
    j end_handler

exit_without_switch:
    lw sp, tsp

end_handler:
    restore_minimal_context
    mret

/******************************************************************************
@ Functions:
@     size_t cpu_intrpt_save(void);
@     void cpu_intrpt_restore(size_t cpsr);
@******************************************************************************/
cpu_intrpt_save:
    csrrci a0, mstatus, RISCV_MSTATUS_MIE
    ret

cpu_intrpt_restore:
    csrw mstatus, a0
    ret


/******************************************************************************
@ Functions:
@     void   cpu_first_task_start(void);
@******************************************************************************/
cpu_first_task_start:
    csrci mstatus, RISCV_MSTATUS_MIE

    /* task_running = 1 */
    li t0, 1
    sb t0, task_running, t1

    lw t2, g_active_task
    lw sp, (t2)

    restore_extra_context
    restore_minimal_context

    /*
     * Don't need to enable interrupt, because RISCV_MSTATUS_MPIE is set,
     * interrupt is enabled after mret.
     */
    mret

/******************************************************************************
@ Functions:
@     void cpu_task_switch(void);
@******************************************************************************/
cpu_task_switch:

    addi sp, sp, -0x98

    sw  x1, 0x90(sp)  // mepc
    sw  x1, 0x40(sp)  // ra
    sw  x3, 0x44(sp)  // gp
    sw  x4, 0x48(sp)  // tp
    sw  x5, 0x4c(sp)  // t0
    sw  x6, 0x50(sp)  // t1
    sw  x7, 0x54(sp)  // t2

    /*
     * If interrupt is enabled previously, set MPIE then the interrupt is enabled after mret.
     *
     * if ((mstatus & RISCV_MSTATUS_MIE) != 0)
     * {
     *     mstatus = RISCV_MSTATUS_MPP | RISCV_MSTATUS_MPIE;
     * }
     * else
     * {
     *     mstatus = RISCV_MSTATUS_MPP;
     * }
     */
    csrrci t0, mstatus, RISCV_MSTATUS_MIE
    li   t2, RISCV_MSTATUS_MPP
    andi t0, t0, RISCV_MSTATUS_MIE
    beqz t0, save_mstatus
    ori  t2, t2, RISCV_MSTATUS_MPIE

save_mstatus:
    sw   t2, 0x94(sp)     /*mstatus*/

    sw x10, 0x58(sp)  // a0
    sw x11, 0x5c(sp)  // a1
    sw x12, 0x60(sp)  // a2
    sw x13, 0x64(sp)  // a3
    sw x14, 0x68(sp)  // a4
    sw x15, 0x6c(sp)  // a5
    sw x16, 0x70(sp)  // a6
    sw x17, 0x74(sp)  // a7

    csrr a0, 0x7B0
    csrr a1, 0x7B1
    csrr a2, 0x7B2
    sw a0, 0x78(sp)  // lpstart[0]
    sw a1, 0x7c(sp)  // lpend[0]
    sw a2, 0x80(sp)  // lpcount[0]
    csrr a0, 0x7B4
    csrr a1, 0x7B5
    csrr a2, 0x7B6
    sw a0, 0x84(sp)  // lpstart[1]
    sw a1, 0x88(sp)  // lpend[1]
    sw a2, 0x8c(sp)  // lpcount[1]

    sw   x8,   0x00(sp)
    sw   x9,   0x04(sp)
    sw   x18,  0x08(sp)
    sw   x19,  0x0c(sp)
    sw   x20,  0x10(sp)
    sw   x21,  0x14(sp)
    sw   x22,  0x18(sp)
    sw   x23,  0x1c(sp)
    sw   x24,  0x20(sp)
    sw   x25,  0x24(sp)
    sw   x26,  0x28(sp)
    sw   x27,  0x2c(sp)
    sw   x28,  0x30(sp)
    sw   x29,  0x34(sp)
    sw   x30,  0x38(sp)
    sw   x31,  0x3c(sp)
    lw   t1,   g_active_task
    sw   sp,   (t1)

    call    krhino_stack_ovf_check

    la    t0, g_active_task                         // g_active_task = g_preferred_ready_task;
    la    t1, g_preferred_ready_task
    lw    t2, (t1)
    sw    t2, (t0)

    lw    sp,   (t2)

    restore_extra_context
    restore_minimal_context
    mret
