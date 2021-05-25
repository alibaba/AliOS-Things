#include <k_config.h>

/******************************************************************************
@                            EXTERN PARAMETERS
@******************************************************************************/
.extern g_active_task
.extern g_preferred_ready_task
.extern krhino_stack_ovf_check
.extern krhino_task_sched_stats_get

/******************************************************************************
@                            EXPORT FUNCTIONS
@******************************************************************************/
.global cpu_intrpt_save
.global cpu_intrpt_restore
.global cpu_task_switch
.global cpu_intrpt_switch
.global cpu_first_task_start

.global Mtspend_Handler
.global _first_task_restore

/******************************************************************************
@                                 EQUATES
@******************************************************************************/
.equ MSTATUS_PRV1,        0x1880 /* Enable interrupts when returning from the handler.*/
.equ RISCV_MSTATUS_MIE,        (1<<3)       /*machine-level interrupt bit*/
.equ RISCV_MSTATUS_MPIE,       (1<<7)       /*machine-level pre interrupt bit*/
.equ RISCV_MSTATUS_MPP,        (0x3<<11)    /*machine-level MPP bit*/

#if __riscv_xlen == 64
#define portWORD_SIZE 8
#define store_x sd
#define load_x ld
#elif __riscv_xlen == 32
#define store_x sw
#define load_x lw
#define portWORD_SIZE 4
#else
	#error Assembler did not define __riscv_xlen
#endif

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
    lui     t0, 0x40040
    slli    t0, t0,0x8
    li      t2, 0x1
    sd      t2, 0(t0)
    ret


cpu_intrpt_switch:
    lui     t0, 0x40040
    slli    t0, t0,0x8
    li      t2, 0x1
    sd      t2, 0(t0)
    ret


/******************************************************************************
@ Functions:
@     void Mtspend_Handler(void);
@******************************************************************************/
.align 8
.func
Mtspend_Handler:
    csrc mstatus, 8

    addi     sp, sp, -(128+128)
    fsd      f31, (0  +0  )(sp)
    fsd      f30, (4  +4  )(sp)
    fsd      f29, (8  +8  )(sp)
    fsd      f28, (12 +12 )(sp)
    fsd      f27, (16 +16 )(sp)
    fsd      f26, (20 +20 )(sp)
    fsd      f25, (24 +24 )(sp)
    fsd      f24, (28 +28 )(sp)
    fsd      f23, (32 +32 )(sp)
    fsd      f22, (36 +36 )(sp)
    fsd      f21, (40 +40 )(sp)
    fsd      f20, (44 +44 )(sp)
    fsd      f19, (48 +48 )(sp)
    fsd      f18, (52 +52 )(sp)
    fsd      f17, (56 +56 )(sp)
    fsd      f16, (60 +60 )(sp)
    fsd      f15, (64 +64 )(sp)
    fsd      f14, (68 +68 )(sp)
    fsd      f13, (72 +72 )(sp)
    fsd      f12, (76 +76 )(sp)
    fsd      f11, (80 +80 )(sp)
    fsd      f10, (84 +84 )(sp)
    fsd      f9,  (88 +88 )(sp)
    fsd      f8,  (92 +92 )(sp)
    fsd      f7,  (96 +96 )(sp)
    fsd      f6,  (100+100)(sp)
    fsd      f5,  (104+104)(sp)
    fsd      f4,  (108+108)(sp)
    fsd      f3,  (112+112)(sp)
    fsd      f2,  (116+116)(sp)
    fsd      f1,  (120+120)(sp)
    fsd      f0,  (124+124)(sp)

    addi    sp, sp, -(124+124)

    sd      x1,  (0  +0  )(sp)
    sd      x3,  (4  +4  )(sp)
    sd      x4,  (8  +8  )(sp)
    sd      x5,  (12 +12 )(sp)
    sd      x6,  (16 +16 )(sp)
    sd      x7,  (20 +20 )(sp)
    sd      x8,  (24 +24 )(sp)
    sd      x9,  (28 +28 )(sp)
    sd      x10, (32 +32 )(sp)
    sd      x11, (36 +36 )(sp)
    sd      x12, (40 +40 )(sp)
    sd      x13, (44 +44 )(sp)
    sd      x14, (48 +48 )(sp)
    sd      x15, (52 +52 )(sp)
    sd      x16, (56 +56 )(sp)
    sd      x17, (60 +60 )(sp)
    sd      x18, (64 +64 )(sp)
    sd      x19, (68 +68 )(sp)
    sd      x20, (72 +72 )(sp)
    sd      x21, (76 +76 )(sp)
    sd      x22, (80 +80 )(sp)
    sd      x23, (84 +84 )(sp)
    sd      x24, (88 +88 )(sp)
    sd      x25, (92 +92 )(sp)
    sd      x26, (96 +96 )(sp)
    sd      x27, (100+100)(sp)
    sd      x28, (104+104)(sp)
    sd      x29, (108+108)(sp)
    sd      x30, (112+112)(sp)
    sd      x31, (116+116)(sp)
    csrr    t0, mepc
    sd      t0, (120+120)(sp)
    /* g_active_task->task_stack = context region */
    la      a1, g_active_task
    ld      a1, (a1)
    sd      sp, (a1)

#if (RHINO_CONFIG_TASK_STACK_OVF_CHECK > 0)
    call      krhino_stack_ovf_check
#endif
#if (RHINO_CONFIG_SYS_STATS > 0)
    call      krhino_task_sched_stats_get
#endif

_Mtspend_Handler_nosave:
    /* R0 = g_active_task->task_stack = context region */
    la      a0, g_active_task
    la      a1, g_preferred_ready_task
    ld      a2, (a1)
    sd      a2, (a0)

    la      a1, g_active_task
    ld      a1, (a1)
    ld      sp, (a1)

    /* clear Mtspend irq */
    lui     t0, 0x40040
    slli    t0, t0,0x8
    li      t2, 0x0
    sd      t2, 0(t0)

    /* Run in machine mode */
    li      t0, MSTATUS_PRV1
    csrs    mstatus, t0

    ld      t0, (120+120)(sp)
    csrw    mepc, t0

    ld     x1,  (0  +0  )(sp)
    ld     x3,  (4  +4  )(sp)
    ld     x4,  (8  +8  )(sp)
    ld     x5,  (12 +12 )(sp)
    ld     x6,  (16 +16 )(sp)
    ld     x7,  (20 +20 )(sp)
    ld     x8,  (24 +24 )(sp)
    ld     x9,  (28 +28 )(sp)
    ld     x10, (32 +32 )(sp)
    ld     x11, (36 +36 )(sp)
    ld     x12, (40 +40 )(sp)
    ld     x13, (44 +44 )(sp)
    ld     x14, (48 +48 )(sp)
    ld     x15, (52 +52 )(sp)
    ld     x16, (56 +56 )(sp)
    ld     x17, (60 +60 )(sp)
    ld     x18, (64 +64 )(sp)
    ld     x19, (68 +68 )(sp)
    ld     x20, (72 +72 )(sp)
    ld     x21, (76 +76 )(sp)
    ld     x22, (80 +80 )(sp)
    ld     x23, (84 +84 )(sp)
    ld     x24, (88 +88 )(sp)
    ld     x25, (92 +92 )(sp)
    ld     x26, (96 +96 )(sp)
    ld     x27, (100+100)(sp)
    ld     x28, (104+104)(sp)
    ld     x29, (108+108)(sp)
    ld     x30, (112+112)(sp)
    ld     x31, (116+116)(sp)

    addi    sp, sp, (124+124)

    fld      f31,( 0 + 0 )(sp)
    fld      f30,( 4 + 4 )(sp)
    fld      f29,( 8 + 8 )(sp)
    fld      f28,( 12+ 12)(sp)
    fld      f27,( 16+ 16)(sp)
    fld      f26,( 20+ 20)(sp)
    fld      f25,( 24+ 24)(sp)
    fld      f24,( 28+ 28)(sp)
    fld      f23,( 32+ 32)(sp)
    fld      f22,( 36+ 36)(sp)
    fld      f21,( 40+ 40)(sp)
    fld      f20,( 44+ 44)(sp)
    fld      f19,( 48+ 48)(sp)
    fld      f18,( 52+ 52)(sp)
    fld      f17,( 56+ 56)(sp)
    fld      f16,( 60+ 60)(sp)
    fld      f15,( 64+ 64)(sp)
    fld      f14,( 68+ 68)(sp)
    fld      f13,( 72+ 72)(sp)
    fld      f12,( 76+ 76)(sp)
    fld      f11,( 80+ 80)(sp)
    fld      f10,( 84+ 84)(sp)
    fld      f9, ( 88+ 88)(sp)
    fld      f8, ( 92+ 92)(sp)
    fld      f7, ( 96+ 96)(sp)
    fld      f6, (100+100)(sp)
    fld      f5, (104+104)(sp)
    fld      f4, (108+108)(sp)
    fld      f3, (112+112)(sp)
    fld      f2, (116+116)(sp)
    fld      f1, (120+120)(sp)
    fld      f0, (124+124)(sp)

    addi    sp, sp, (128+128)
//    csrrw    sp, mscratch, sp

    mret
    .endfunc


/******************************************************************************
@ Functions:
@     void cpu_first_task_start(void);
@******************************************************************************/
.align 8
.func
cpu_first_task_start:
	/* Enable mtime and external interrupts.  1<<7 for timer interrupt, 1<<11
		for external interrupt. */
    li      t0,  0x888
    csrw    mie, t0

    la      a0, g_active_task
    ld      a0, (a0)
    ld      sp, (a0)

    /* Run in machine mode */
    li      t0, MSTATUS_PRV1
    csrs    mstatus, t0

    ld      t0, (120+120)(sp)
    csrw    mepc, t0

    ld      x1,  (0  +0  )(sp)
    ld      x3,  (4  +4  )(sp)
    ld      x4,  (8  +8  )(sp)
    ld      x5,  (12 +12 )(sp)
    ld      x6,  (16 +16 )(sp)
    ld      x7,  (20 +20 )(sp)
    ld      x8,  (24 +24 )(sp)
    ld      x9,  (28 +28 )(sp)
    ld      x10, (32 +32 )(sp)
    ld      x11, (36 +36 )(sp)
    ld      x12, (40 +40 )(sp)
    ld      x13, (44 +44 )(sp)
    ld      x14, (48 +48 )(sp)
    ld      x15, (52 +52 )(sp)
    ld      x16, (56 +56 )(sp)
    ld      x17, (60 +60 )(sp)
    ld      x18, (64 +64 )(sp)
    ld      x19, (68 +68 )(sp)
    ld      x20, (72 +72 )(sp)
    ld      x21, (76 +76 )(sp)
    ld      x22, (80 +80 )(sp)
    ld      x23, (84 +84 )(sp)
    ld      x24, (88 +88 )(sp)
    ld      x25, (92 +92 )(sp)
    ld      x26, (96 +96 )(sp)
    ld      x27, (100+100)(sp)
    ld      x28, (104+104)(sp)
    ld      x29, (108+108)(sp)
    ld      x30, (112+112)(sp)
    ld      x31, (116+116)(sp)

    addi    sp, sp, (124+124)

    fld      f31, (0  +0  )(sp)
    fld      f30, (4  +4  )(sp)
    fld      f29, (8  +8  )(sp)
    fld      f28, (12 +12 )(sp)
    fld      f27, (16 +16 )(sp)
    fld      f26, (20 +20 )(sp)
    fld      f25, (24 +24 )(sp)
    fld      f24, (28 +28 )(sp)
    fld      f23, (32 +32 )(sp)
    fld      f22, (36 +36 )(sp)
    fld      f21, (40 +40 )(sp)
    fld      f20, (44 +44 )(sp)
    fld      f19, (48 +48 )(sp)
    fld      f18, (52 +52 )(sp)
    fld      f17, (56 +56 )(sp)
    fld      f16, (60 +60 )(sp)
    fld      f15, (64 +64 )(sp)
    fld      f14, (68 +68 )(sp)
    fld      f13, (72 +72 )(sp)
    fld      f12, (76 +76 )(sp)
    fld      f11, (80 +80 )(sp)
    fld      f10, (84 +84 )(sp)
    fld      f9,  (88 +88 )(sp)
    fld      f8,  (92 +92 )(sp)
    fld      f7,  (96 +96 )(sp)
    fld      f6,  (100+100)(sp)
    fld      f5,  (104+104)(sp)
    fld      f4,  (108+108)(sp)
    fld      f3,  (112+112)(sp)
    fld      f2,  (116+116)(sp)
    fld      f1,  (120+120)(sp)
    fld      f0,  (124+124)(sp)

    addi    sp, sp, (128+128)

    mret
    .endfunc

