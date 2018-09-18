/*
*
* @file port_s.S
*
*/

#include "typedef.h"

/******************************************************************************
@                            EXTERN PARAMETERS
@******************************************************************************/

.extern  g_active_task
.extern  g_preferred_ready_task
.extern  krhino_stack_ovf_check

/******************************************************************************
@                            EXPORT FUNCTIONS
@******************************************************************************/

.global  cpu_intrpt_save
.global  cpu_intrpt_restore
.global  cpu_task_switch
.global  cpu_intrpt_switch
.global  cpu_first_task_start
.global  restore_sp


/******************************************************************************
@                                 EQUATES
@******************************************************************************/

//.equ RISCV_MSTATUS_MIE,        (1<<3)       /*machine-level interrupt bit*/
//.equ RISCV_MSTATUS_MPIE,       (1<<7)       /*machine-level pre interrupt bit*/
//.equ RISCV_MSTATUS_MPP,        (0x3<<10)    /*machine-level MPP bit*/

//.equ LOG_REGBYTES,              2    
//.equ REGBYTES,                 (1<<LOG_REGBYTES)    /*machine-level MPP bit*/

/******************************************************************************
@                        CODE GENERATION DIRECTIVES
@******************************************************************************/

.text
.align 2

/******************************************************************************
@                        MACRO DEFINED
@******************************************************************************/

.macro POP_FP_REG reg


.endm

.macro PUSH_FP_REG reg


.endm


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
    la    t1,  g_active_task
    lw    t2, (t1)
    lw    sp, (t2)

    lw    t1, (sp)
    csrw  mstatus, t1
    lw    t1, 1*REGBYTES(sp)
    csrw  mepc, t1
    lw    x1, 2*REGBYTES(sp)    
    lw    x3, 3*REGBYTES(sp)
    lw    x4, 4*REGBYTES(sp)
    lw    x5, 5*REGBYTES(sp)
    lw    x6, 6*REGBYTES(sp)
    lw    x7, 7*REGBYTES(sp)
    lw    x8, 8*REGBYTES(sp)
    lw    x9, 9*REGBYTES(sp)
    lw    x10, 10*REGBYTES(sp)
    lw    x11, 11*REGBYTES(sp)
    lw    x12, 12*REGBYTES(sp)
    lw    x13, 13*REGBYTES(sp)
    lw    x14, 14*REGBYTES(sp)
    lw    x15, 15*REGBYTES(sp)
    lw    x16, 16*REGBYTES(sp)
    lw    x17, 17*REGBYTES(sp)
    lw    x18, 18*REGBYTES(sp)
    lw    x19, 19*REGBYTES(sp)
    lw    x20, 20*REGBYTES(sp)
    lw    x21, 21*REGBYTES(sp)
    lw    x22, 22*REGBYTES(sp)
    lw    x23, 23*REGBYTES(sp)
    lw    x24, 24*REGBYTES(sp)
    lw    x25, 25*REGBYTES(sp)
    lw    x26, 26*REGBYTES(sp)
    lw    x27, 27*REGBYTES(sp)
    lw    x28, 28*REGBYTES(sp)
    lw    x29, 29*REGBYTES(sp)
    lw    x30, 30*REGBYTES(sp)
    lw    x31, 31*REGBYTES(sp)

    addi  sp, sp, 32*REGBYTES
    mret


/******************************************************************************
@ Functions:
@     void cpu_task_switch(void);
@******************************************************************************/
cpu_task_switch:
    // save current task context:

    addi sp, sp, -32*REGBYTES

    sw   x1, 1*REGBYTES(sp)
    sw   x1, 2*REGBYTES(sp)
    sw   x3, 3*REGBYTES(sp)
    sw   x4, 4*REGBYTES(sp)
    sw   x5, 5*REGBYTES(sp)
    sw   x6, 6*REGBYTES(sp)
    sw   x7, 7*REGBYTES(sp)

    li   t1, RISCV_MSTATUS_MPIE
    csrr t2, mstatus
    /*MPIE is set to 1 after last mret*/
    andi t2, t2, RISCV_MSTATUS_MIE
    bnez t2, set_mpp
    li   t1, 0
set_mpp:
    li   t2, RISCV_MSTATUS_MPP
    or   t1, t1, t2
    sw   t1, (sp)     /*mstatus*/
    
    sw   x8, 8*REGBYTES(sp)
    sw   x9, 9*REGBYTES(sp)
    sw   x10, 10*REGBYTES(sp)
    sw   x11, 11*REGBYTES(sp)
    sw   x12, 12*REGBYTES(sp)
    sw   x13, 13*REGBYTES(sp)
    sw   x14, 14*REGBYTES(sp)
    sw   x15, 15*REGBYTES(sp)
    sw   x16, 16*REGBYTES(sp)
    sw   x17, 17*REGBYTES(sp)
    sw   x18, 18*REGBYTES(sp)
    sw   x19, 19*REGBYTES(sp)
    sw   x20, 20*REGBYTES(sp)
    sw   x21, 21*REGBYTES(sp)
    sw   x22, 22*REGBYTES(sp)
    sw   x23, 23*REGBYTES(sp)
    sw   x24, 24*REGBYTES(sp)
    sw   x25, 25*REGBYTES(sp)
    sw   x26, 26*REGBYTES(sp)
    sw   x27, 27*REGBYTES(sp)
    sw   x28, 28*REGBYTES(sp)
    sw   x29, 29*REGBYTES(sp)
    sw   x30, 30*REGBYTES(sp)
    sw   x31, 31*REGBYTES(sp)

    la    t1,  g_active_task
    lw    t1, (t1)
    sw    sp, (t1) 
        
    call    krhino_stack_ovf_check

    la    t0, g_active_task                         // g_active_task = g_preferred_ready_task;
    la    t1, g_preferred_ready_task
    lw    t2, (t1)
    sw    t2, (t0)
    
    lw    sp, (t2)
    
restore_sp:
    lw    t1, (sp)
    csrw  mstatus, t1
    lw    t1, 1*REGBYTES(sp)
    csrw  mepc, t1
    lw    ra, 2*REGBYTES(sp)
    lw    x3, 3*REGBYTES(sp)
    lw    x4, 4*REGBYTES(sp)
    lw    x5, 5*REGBYTES(sp)
    lw    x6, 6*REGBYTES(sp)
    lw    x7, 7*REGBYTES(sp)
    lw    x8, 8*REGBYTES(sp)
    lw    x9, 9*REGBYTES(sp)
    lw    x10, 10*REGBYTES(sp)
    lw    x11, 11*REGBYTES(sp)
    lw    x12, 12*REGBYTES(sp)
    lw    x13, 13*REGBYTES(sp)
    lw    x14, 14*REGBYTES(sp)
    lw    x15, 15*REGBYTES(sp)
    lw    x16, 16*REGBYTES(sp)
    lw    x17, 17*REGBYTES(sp)
    lw    x18, 18*REGBYTES(sp)
    lw    x19, 19*REGBYTES(sp)
    lw    x20, 20*REGBYTES(sp)
    lw    x21, 21*REGBYTES(sp)
    lw    x22, 22*REGBYTES(sp)
    lw    x23, 23*REGBYTES(sp)
    lw    x24, 24*REGBYTES(sp)
    lw    x25, 25*REGBYTES(sp)
    lw    x26, 26*REGBYTES(sp)
    lw    x27, 27*REGBYTES(sp)
    lw    x28, 28*REGBYTES(sp)
    lw    x29, 29*REGBYTES(sp)
    lw    x30, 30*REGBYTES(sp)
    lw    x31, 31*REGBYTES(sp)

    addi  sp, sp, 32*REGBYTES
    mret

/******************************************************************************
@ Functions:
@     void   cpu_intrpt_switch(void);
@******************************************************************************/
cpu_intrpt_switch:
    la     t1, g_active_task                                  // g_active_task = g_preferred_ready_task;
    la     t2, g_preferred_ready_task
    lw     t3, (t2)
    sw     t3, (t1)                                             // R0 = g_active_task->task_stack = context region

    lw     sp, (t3)

    j   restore_sp

