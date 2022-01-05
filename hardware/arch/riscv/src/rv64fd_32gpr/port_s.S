#include <k_config.h>
#include <k_arch.h>

/******************************************************************************
@                            EXPORT FUNCTIONS
@******************************************************************************/
.global cpu_intrpt_save
.global cpu_intrpt_restore
.global cpu_task_switch
.global cpu_intrpt_switch
.global cpu_first_task_start

.extern task_restore
/******************************************************************************
@                                 EQUATES
@******************************************************************************/
.equ RISCV_MSTATUS_MIE,        (1<<3)       /*machine-level interrupt bit*/

/******************************************************************************
@                        CODE GENERATION DIRECTIVES
@*******************************************************************************/
.text
.align 3

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
@     void cpu_intrpt_switch(void);
@     void cpu_task_switch(void);
@******************************************************************************/
cpu_task_switch:
    mv      a0, x0
    ecall
    ret

cpu_intrpt_switch:
    ret

/******************************************************************************
@ Functions:
@     void cpu_first_task_start(void);
@******************************************************************************/
.align 8
.func
cpu_first_task_start:
    /* Enable mtime and external interrupts. */
    li      t0,  (IE_MTIE | IE_MEIE)
    csrw    mie, t0
    la      t0, task_restore
    jr      t0
    .endfunc

