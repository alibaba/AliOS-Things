/*******************************************************************************
Copyright (c) 2006-2008 by Tensilica Inc.  ALL RIGHTS RESERVED.
These coded instructions, statements, and computer programs are the
copyrighted works and confidential proprietary information of Tensilica Inc.
They may not be modified, copied, reproduced, distributed, or disclosed to
third parties in any manner, medium, or form, in whole or in part, without
the prior written consent of Tensilica Inc.
--------------------------------------------------------------------------------

        XTENSA VECTORS AND LOW LEVEL HANDLERS FOR AN RTOS

Xtensa low level exception and interrupt vectors and handlers for an RTOS.

Interrupt handlers and user exception handlers support interaction with
the RTOS by calling XT_RTOS_INT_ENTER and XT_RTOS_INT_EXIT before and
after user's specific interrupt handlers. These macros are defined in
xtensa_<rtos>.h to call suitable functions in a specific RTOS. The user
may insert application-specific handlers at indicated places for each
hardware interrupt priority/level. These handlers may be coded in C and
called from the Xtensa low level handlers. Optional hooks are provided
to install a handler per level at run-time, made available by compiling
this source file with '-DXT_INTEXC_HOOKS' (useful for automated testing).

!!  This file is a template that usually needs to be modified to handle       !!
!!  application specific interrupts. Search USER_EDIT for helpful comments    !!
!!  on where to insert handlers and how to write them.                        !!

Because Xtensa is a configurable architecture, this port supports all user
generated configurations (except restrictions stated in the release notes).
This is accomplished by conditional compilation using macros and functions
defined in the Xtensa HAL (hardware adaptation layer) for your configuration.
Only the relevant parts of this file will be included in your RTOS build.
For example, this file provides interrupt vector templates for all types and
all priority levels, but only the ones in your configuration are built.

NOTES on the use of 'call0' for long jumps instead of 'j':
 1. This file should be assembled with the -mlongcalls option to xt-xcc.
 2. The -mlongcalls compiler option causes 'call0 dest' to be expanded to
    a sequence 'l32r a0, dest' 'callx0 a0' which works regardless of the
    distance from the call to the destination. The linker then relaxes
    it back to 'call0 dest' if it determines that dest is within range.
    This allows more flexibility in locating code without the performance
    overhead of the 'l32r' literal data load in cases where the destination
    is in range of 'call0'. There is an additional benefit in that 'call0'
    has a longer range than 'j' due to the target being word-aligned, so 
    the 'l32r' sequence is less likely needed.
 3. The use of 'call0' with -mlongcalls requires that register a0 not be 
    live at the time of the call, which is always the case for a function 
    call but needs to be ensured if 'call0' is used as a jump in lieu of 'j'.
 4. This use of 'call0' is independent of the C function call ABI.

*******************************************************************************/
    #include <xtensa/coreasm.h>
    #include <xtensa/config/system.h>
    #include <xtensa/config/specreg.h>
    //#include "xtos-internal.h"
    #include "frxt/xtensa_rtos.h"

#if 0

#else        /*********************Seperate from BAOFENG and NORMAL********************/

#define _INTERRUPT_LEVEL 3

#undef HAVE_XSR
#define HAVE_XSR 1

#undef STRUCT_FIELD
#define STRUCT_FIELD(ctype,size,pre,name)    pre##name:    .space    size

#undef STRUCT_AFIELD
#define STRUCT_AFIELD(ctype,size,pre,name,n)    pre##name:    .if n ; .space    (size)*(n) ; .endif

#define _JOIN2(a, b) a ## b
#define _JOIN3(a, b, c) a ## b ## c

#define JOIN2(a, b) _JOIN2(a, b)
#define JOIN3(a, b, c) _JOIN3(a, b, c)

#define LABEL(a, b) _JOIN3(a, 3, b)

#define EXCSAVE_LEVEL _JOIN2(EXCSAVE_, 3)

#undef XT_RTOS_INT_ENTER
#define XT_RTOS_INT_ENTER _xt_int_enter

#undef XT_RTOS_INT_EXIT
#define XT_RTOS_INT_EXIT _xt_int_exit

#undef XT_RTOS_TIMER_INT
#define XT_RTOS_TIMER_INT _xt_timer_int

STRUCT_BEGIN
STRUCT_FIELD (long,4,HESF_,SAR)
#ifdef __XTENSA_WINDOWED_ABI__
STRUCT_FIELD (long,4,HESF_,WINDOWSTART)
STRUCT_FIELD (long,4,HESF_,WINDOWBASE)
#endif
STRUCT_FIELD (long,4,HESF_,EPC1)
STRUCT_FIELD (long,4,HESF_,EXCCAUSE)
STRUCT_FIELD (long,4,HESF_,EXCVADDR)
STRUCT_FIELD (long,4,HESF_,EXCSAVE1)
STRUCT_FIELD (long,4,HESF_,EPC3)
STRUCT_FIELD (long,4,HESF_,EPS3)
#if XCHAL_HAVE_XEA1
STRUCT_FIELD (long,4,HESF_,VPRI)    /* (XEA1 only) */
#endif
#if XCHAL_HAVE_MAC16
STRUCT_FIELD (long,4,HESF_,ACCLO)
STRUCT_FIELD (long,4,HESF_,ACCHI)
/*STRUCT_AFIELD(long,4,HESF_,MR, 4)*/
#endif
#if XCHAL_HAVE_LOOPS
STRUCT_FIELD (long,4,HESF_,LCOUNT)
STRUCT_FIELD (long,4,HESF_,LBEG)
STRUCT_FIELD (long,4,HESF_,LEND)
#endif
#ifdef __XTENSA_WINDOWED_ABI__
STRUCT_AFIELD(long,4,HESF_,AREG, 64)    /* address registers ar0..ar63 */
#else
STRUCT_AFIELD(long,4,HESF_,AREG, 16)    /* address registers ar0..ar15 */
#endif
#define HESF_AR(n)  HESF_AREG+((n)*4)
STRUCT_END(HighPriFrame)
#define HESF_TOTALSIZE  HighPriFrameSize+32 /* 32 bytes for interrupted code's save areas under SP */


#if XCHAL_HAVE_XEA1 && HAVE_XSR     /* could be made true for T1040 and T1050 */
# error "high-priority interrupt stack frame needs adjustment if HAVE_XSR is allowed with XEA1"
#endif


#define PRI_N_STACK_SIZE     1024    /* default to 1 kB stack for each level-N handling */
#define PRI_N_STACK_SIZE2    256     /* default to 1 kB stack for each level-N handling */

#define USR_INT_STACK_SIZE   1024

    .global  usr_int_stack_top
    .section .bss, "aw"
    .balign 16
usr_int_stack:
    .space USR_INT_STACK_SIZE
usr_int_stack_top:
    .space 4

    //  Allocate save area and stack:
    //  (must use .bss, not .comm, because the subsequent .set does not work otherwise)
    .section .bss, "aw"
    .align  16
LABEL(_Pri_,_Stack):    .space  PRI_N_STACK_SIZE + HESF_TOTALSIZE + PRI_N_STACK_SIZE2 + HESF_TOTALSIZE

    .balign 16
LoadStoreErrorHandlerStack:
    .word   0       # a0
    .word   0       # (unused)
    .word   0       # a2
    .word   0       # a3
    .word   0       # a4
    .word   0       # a5
    .word   0       # a6
    .balign 16
LoadStoreErrorHandlerStack_reentry:
    .word   0       # a0
    .word   0       # (unused)
    .word   0       # a2
    .word   0       # a3
    .word   0       # a4
    .word   0       # a5
    .word   0       # a6

#if HAVE_XSR
    .data
    .global LABEL(_Pri_,_HandlerAddress)
LABEL(_Pri_,_HandlerAddress):   .space 4
    .global LABEL(_Pri_, _NMICount)
LABEL(_Pri_,_NMICount):   .space 4
#endif

/*************************** LoadStoreError Handler **************************/

        .section    .text

/* Xtensa "Load/Store Exception" handler:
 * Completes L8/L16 load instructions from Instruction address space, for which
 * the architecture only supports 32-bit reads.
 *
 * Called from UserExceptionVector if EXCCAUSE is LoadStoreErrorCause
 *
 * (Fast path (no branches) is for L8UI)
 */
        .literal_position

        .balign 4
LoadStoreErrorHandler:
//        .global     LoadStoreErrorHandler
        .type   LoadStoreErrorHandler, @function


        rsr     a0, excsave1    # restore a0 saved by UserExceptionVector
        wsr     a1, excsave1    # save a1 to excsave1, a1 can be used as varalbe 

        movi    a1, LABEL(_Pri_,_NMICount)
        l32i    a1, a1, 0
    
        bnez    a1, LoadStoreErrorHandler_reentry
        movi    sp, LoadStoreErrorHandlerStack
        j       LoadStoreErrorHandler_common
LoadStoreErrorHandler_reentry:
        movi    sp, LoadStoreErrorHandlerStack_reentry
LoadStoreErrorHandler_common:

        /* Registers are saved in the address corresponding to their register
         * number times 4.  This allows a quick and easy mapping later on when
         * needing to store the value to a particular register number. */

        s32i    a0, sp, 0
        s32i    a2, sp, 0x08
        s32i    a3, sp, 0x0c
        s32i    a4, sp, 0x10
        rsr     a0, sar         # Save SAR in a0 to restore later

        /* Examine the opcode which generated the exception */
        /* Note: Instructions are in this order to avoid pipeline stalls. */
        rsr     a2, epc1
        movi    a3, ~3
        ssa8l   a2              # sar is now correct shift for aligned read
        and     a2, a2, a3      # a2 now 4-byte aligned address of instruction
        l32i    a4, a2, 0
        l32i    a2, a2, 4
        movi    a3, 0x00700F    # opcode mask for l8ui/l16si/l16ui
        src     a2, a2, a4      # a2 now instruction that failed
        and     a3, a2, a3      # a3 is masked instruction

        # This is store instruction
        movi   a4, 0x004002
        beq    a3, a4, .LSE_check_s8i_store  # s8i

        movi   a4, 0x005002
        beq    a3, a4, .LSE_check_s16i_store   # s16i

        bnei    a3, 0x000002, .LSE_check_l16

        /* Note: At this point, opcode could technically be one of two things:
         *   xx0xx2 (L8UI)
         *   xx8xx2 (Reserved (invalid) opcode)
         * It is assumed that we'll never get to this point from an illegal
         * opcode, so we don't bother to check for that case and presume this
         * is always an L8UI. */

        movi    a4, ~3
        rsr     a3, excvaddr    # read faulting address
        and     a4, a3, a4      # a4 now word aligned read address

        l32i    a4, a4, 0       # perform the actual read
        ssa8l   a3              # sar is now shift to extract a3's byte
        srl     a3, a4          # shift right correct distance
        extui   a4, a3, 0, 8    # mask off bits we need for an l8

.LSE_post_fetch:
        /* We jump back here after either the L8UI or the L16*I routines do the
         * necessary work to read the value from memory.
         * At this point, a2 holds the faulting instruction and a4 holds the
         * correctly read value.

         * Restore original SAR value (saved in a0) and update EPC so we'll
         * return back to the instruction following the one we just emulated */

        /* Note: Instructions are in this order to avoid pipeline stalls */
        rsr     a3, epc1
        wsr     a0, sar
        addi    a3, a3, 0x3
        wsr     a3, epc1

        /* Stupid opcode tricks: The jumptable we use later on needs 16 bytes
         * per entry (so we can avoid a second jump by just doing a RFE inside
         * each entry).  Unfortunately, however, Xtensa doesn't have an addx16
         * operation to make that easy for us.  Luckily, all of the faulting
         * opcodes we're processing are guaranteed to have bit 3 be zero, which
         * means if we just shift the register bits of the opcode down by 3
         * instead of 4, we will get the register number multiplied by 2.  This
         * combined with an addx8 will give us an effective addx16 without
         * needing any extra shift operations. */
        extui   a2, a2, 3, 5    # a2 is now destination register 0-15 times 2

        bgei    a2, 10, .LSE_assign_reg     # a5..a15 use jumptable
        beqi    a2, 2, .LSE_assign_a1       # a1 uses a special routine

        /* We're storing into a0 or a2..a4, which are all saved in our "stack"
         * area.  Calculate the correct address and stick the value in there,
         * then just do our normal restore and RFE (no jumps required, which
         * actually makes a0..a4 substantially faster). */
        addx2   a2, a2, sp
        s32i    a4, a2, 0

        /* Restore all regs and return */
        l32i    a0, sp, 0
        l32i    a2, sp, 0x08
        l32i    a3, sp, 0x0c
        l32i    a4, sp, 0x10
        rsr     a1, excsave1    # restore a1 saved by UserExceptionVector
        rfe

.LSE_assign_reg:
        /* At this point, a2 contains the register number times 2, a4 is the
         * read value. */

        /* Calculate the jumptable address, and restore all regs except a2 and
         * a4 so we have less to do after jumping. */
        /* Note: Instructions are in this order to avoid pipeline stalls. */
        movi    a3, .LSE_jumptable_base
        l32i    a0, sp, 0
        addx8   a2, a2, a3      # a2 is now the address to jump to
        l32i    a3, sp, 0x0c

        jx      a2

        .balign 4
.LSE_check_l16:
        /* At this point, a2 contains the opcode, a3 is masked opcode */
        movi    a4, 0x001002    # l16si or l16ui opcode after masking
        bne     a3, a4, .LSE_wrong_opcode

        /* Note: At this point, the opcode could be one of two things:
         *   xx1xx2 (L16UI)
         *   xx9xx2 (L16SI)
         * Both of these we can handle. */

        movi    a4, ~3
        rsr     a3, excvaddr    # read faulting address
        and     a4, a3, a4      # a4 now word aligned read address

        l32i    a4, a4, 0       # perform the actual read
        ssa8l   a3              # sar is now shift to extract a3's bytes
        srl     a3, a4          # shift right correct distance
        extui   a4, a3, 0, 16   # mask off bits we need for an l16

        bbci    a2, 15, .LSE_post_fetch  # Not a signed op
        bbci    a4, 15, .LSE_post_fetch  # Value does not need sign-extension

        movi    a3, 0xFFFF0000
        or      a4, a3, a4      # set 32-bit sign bits
        j       .LSE_post_fetch

        .balign 4
.LSE_check_s8i_store:
        s32i    a5, sp, 0x14
        s32i    a6, sp, 0x18
        movi    a5,0xff
        j       .LSE_check_store

.LSE_check_s16i_store:
        s32i    a5, sp, 0x14
        s32i    a6, sp, 0x18
        movi    a5,0xffff
        j       .LSE_check_store

.LSE_check_store:
        movi    a4, ~3
        rsr     a3, excvaddr    # write faulting address
        and     a4, a3, a4      # a4 now word aligned write address
        ssa8b   a3
        l32i    a3, a4, 0       # perform the actual read

        mov      a4,a5
        sll      a4,a4
        movi     a6,-1
        xor      a4,a6,a4
        and      a3,a3,a4

        movi    a4, ~3
        rsr     a6, excvaddr    # write faulting address
        and     a4, a6, a4      # a4 now word aligned write address

        extui   a2, a2, 4, 4    # a2 is now destination register 0-15 times 2

        bgei    a2,7,.LSE_big_reg
        movi    a6,4
        mull    a6,a2,a6
        add     a2, a6, sp
        l32i    a2, a2, 0
        j       .Write_data

.LSE_big_reg:
        movi    a6,7
        sub     a2,a2,a6
        movi    a6,8
        mull   a2,a2,a6

        movi    a6,.LSE_big_reg_table
        add   a2,a2,a6
        jx     a2

.balign 4
.LSE_big_reg_table:
        .org    .LSE_big_reg_table + (0*(2*4))
        mov     a2,a7
        j       .Write_data

        .org    .LSE_big_reg_table + (1*(2*4))
        mov     a2,a8
        j       .Write_data

        .org    .LSE_big_reg_table + (2*(2*4))
        mov     a2,a9
        j       .Write_data

        .org    .LSE_big_reg_table + (3*(2*4))
        mov     a2,a10
        j       .Write_data

        .org    .LSE_big_reg_table + (4*(2*4))
        mov     a2,a11
        j       .Write_data

        .org    .LSE_big_reg_table + (5*(2*4))
        mov     a2,a12
        j       .Write_data

        .org    .LSE_big_reg_table + (6*(2*4))
        mov     a2,a13
        j       .Write_data

        .org    .LSE_big_reg_table + (7*(2*4))
        mov     a2,a14
        j       .Write_data

        .org    .LSE_big_reg_table + (8*(2*4))
        mov     a2,a15
        j       .Write_data

.Write_data:
        and      a2,a2,a5
        sll      a2,a2
        or       a3,a3,a2

        s32i    a3,a4,0

        rsr     a3, epc1
        wsr     a0, sar
        addi    a3, a3, 0x3
        wsr     a3, epc1

        /* Restore all regs and return */
        l32i    a0, sp, 0
        l32i    a2, sp, 0x08
        l32i    a3, sp, 0x0c
        l32i    a4, sp, 0x10
        l32i    a5, sp, 0x14
        l32i    a6, sp, 0x18
        rsr     a1, excsave1    # restore a1 saved by UserExceptionVector
        rfe

.LSE_wrong_opcode:
        /* If we got here it's not an opcode we can try to fix, so bomb out.
         * Restore registers so any dump the fatal exception routine produces
         * will have correct values */
        wsr     a0, sar
        l32i    a0, sp, 0
        l32i    a2, sp, 0x08
        l32i    a3, sp, 0x0c
        l32i    a4, sp, 0x10
        rsr     a1, excsave1
        call0   user_fatal_exception_handler

        .balign 4
.LSE_assign_a1:
        /* a1 is saved in excsave1, so just update that with the value, */
        s32i    a4, sp, 0x04
        /* Then restore all regs and return */
        l32i    a0, sp, 0
        l32i    a2, sp, 0x08
        l32i    a3, sp, 0x0c
        l32i    a4, sp, 0x10
        l32i    a1, sp, 0x04
        rfe

        .balign 4
.LSE_jumptable:
        /* The first 5 entries (80 bytes) of this table are unused (registers
         * a0..a4 are handled separately above).  Rather than have a whole bunch
         * of wasted space, we just pretend that the table starts 80 bytes
         * earlier in memory. */
        .set    .LSE_jumptable_base, .LSE_jumptable - (16 * 5)

        .org    .LSE_jumptable_base + (16 * 5)
        mov     a5, a4
        l32i    a2, sp, 0x08
        l32i    a4, sp, 0x10
        rsr     a1, excsave1
        rfe

        .org    .LSE_jumptable_base + (16 * 6)
        mov     a6, a4
        l32i    a2, sp, 0x08
        l32i    a4, sp, 0x10
        rsr     a1, excsave1
        rfe

        .org    .LSE_jumptable_base + (16 * 7)
        mov     a7, a4
        l32i    a2, sp, 0x08
        l32i    a4, sp, 0x10
        rsr     a1, excsave1
        rfe

        .org    .LSE_jumptable_base + (16 * 8)
        mov     a8, a4
        l32i    a2, sp, 0x08
        l32i    a4, sp, 0x10
        rsr     a1, excsave1
        rfe

        .org    .LSE_jumptable_base + (16 * 9)
        mov     a9, a4
        l32i    a2, sp, 0x08
        l32i    a4, sp, 0x10
        rsr     a1, excsave1
        rfe

        .org    .LSE_jumptable_base + (16 * 10)
        mov     a10, a4
        l32i    a2, sp, 0x08
        l32i    a4, sp, 0x10
        rsr     a1, excsave1
        rfe

        .org    .LSE_jumptable_base + (16 * 11)
        mov     a11, a4
        l32i    a2, sp, 0x08
        l32i    a4, sp, 0x10
        rsr     a1, excsave1
        rfe

        .org    .LSE_jumptable_base + (16 * 12)
        mov     a12, a4
        l32i    a2, sp, 0x08
        l32i    a4, sp, 0x10
        rsr     a1, excsave1
        rfe

        .org    .LSE_jumptable_base + (16 * 13)
        mov     a13, a4
        l32i    a2, sp, 0x08
        l32i    a4, sp, 0x10
        rsr     a1, excsave1
        rfe

        .org    .LSE_jumptable_base + (16 * 14)
        mov     a14, a4
        l32i    a2, sp, 0x08
        l32i    a4, sp, 0x10
        rsr     a1, excsave1
        rfe

        .org    .LSE_jumptable_base + (16 * 15)
        mov     a15, a4
        l32i    a2, sp, 0x08
        l32i    a4, sp, 0x10
        rsr     a1, excsave1
        rfe

    .section    .UserEnter.text, "ax"
    .global     call_user_start
    .type       call_user_start,@function
    .align      4
    .literal_position
vector_entr:
    .word   0x40100000
call_user_start:
    l32r    a2, 0x40100000
    //movi    a2, 0x40100000    /* note: absolute symbol, not a ptr */
    wsr    a2, vecbase
    call0   user_start                    /* user exception handler */

#if 0
/*
Panic handler.
Should be reached by call0 (preferable) or jump only. If call0, a0 says where
from. If on simulator, display panic message and abort, else loop indefinitely.
*/
//    .section    .iram.text
    .section    .text
    .global     _xt_panic
    .type       _xt_panic,@function
    .align      4
_xt_panic:
    #ifdef XT_SIMULATOR
    addi    a4, a0, -3                      /* point to call0 */
    movi    a3, _xt_panic_message
    movi    a2, SYS_log_msg
    simcall
    movi    a2, SYS_gdb_abort
    simcall
    #else
    rsil    a2, XCHAL_EXCM_LEVEL            /* disable all low & med ints */
1:  j       1b                              /* loop infinitely */
    #endif

    .section    .rodata, "a"
    .align      4
_xt_panic_message:
    .string "\n*** _xt_panic() was called from 0x%08x or jumped to. ***\n"
#endif

#ifdef XT_INTEXC_HOOKS
    /*
    Hooks to dynamically install handlers for exceptions and interrupts.
    Allows automated regression frameworks to install handlers per test.
    Consists of an array of function pointers indexed by interrupt level,
    with index 0 containing the entry for user exceptions.
    Initialized with all 0s, meaning no handler is installed at each level.
    See comment in xtensa_rtos.h for more details.
    */
    .data
    .global     _xt_intexc_hooks
    .type       _xt_intexc_hooks,@object
    .align      4
_xt_intexc_hooks:
    .fill       XT_INTEXC_HOOK_NUM, 4, 0
#endif


/*******************************************************************************

EXCEPTION AND LEVEL 1 INTERRUPT VECTORS AND LOW LEVEL HANDLERS
(except window exception vectors).

Each vector goes at a predetermined location according to the Xtensa
hardware configuration, which is ensured by its placement in a special
section known to the Xtensa linker support package (LSP). It performs
the minimum necessary before jumping to the handler in the .text section.

The corresponding handler goes in the normal .text section. It sets up
the appropriate stack frame, saves a few vector-specific registers and
calls XT_RTOS_INT_ENTER to save the rest of the interrupted context
and enter the RTOS, then sets up a C environment. It then calls the
user's interrupt handler code (which may be coded in C) and finally 
calls XT_RTOS_INT_EXIT to transfer control to the RTOS for scheduling.

While XT_RTOS_INT_EXIT does not return directly to the interruptee,
eventually the RTOS scheduler will want to dispatch the interrupted
task or handler. The scheduler will return to the exit point that was
saved in the interrupt stack frame at XT_STK_EXIT.

*******************************************************************************/

/*
--------------------------------------------------------------------------------
Debug Exception.
--------------------------------------------------------------------------------
*/

#if XCHAL_HAVE_DEBUG

    .begin      literal_prefix .DebugExceptionVector
    .section    .DebugExceptionVector.text, "ax"
    .global     _DebugExceptionVector
    .align      4
    .literal_position
_DebugExceptionVector:

    #ifdef XT_SIMULATOR
    /*
    In the simulator, let the debugger (if any) handle the debug exception,
    or simply stop the simulation:
    */
    wsr     a2, EXCSAVE+XCHAL_DEBUGLEVEL    /* save a2 where sim expects it */
    movi    a2, SYS_gdb_enter_sktloop
    simcall                                 /* have ISS handle debug exc. */
    #elif 0 /* change condition to 1 to use the HAL minimal debug handler */
    wsr     a3, EXCSAVE+XCHAL_DEBUGLEVEL
    movi    a3, xthal_debugexc_defhndlr_nw  /* use default debug handler */
    jx      a3
    #else
    wsr     a0, EXCSAVE+XCHAL_DEBUGLEVEL    /* save original a0 somewhere */
    call0   user_fatal_exception_handler                       /* does not return */
    rfi     XCHAL_DEBUGLEVEL                /* make a0 point here not later */
    #endif

    .end        literal_prefix

#endif

/*
--------------------------------------------------------------------------------
Double Exception.
Double exceptions are not a normal occurrence. They indicate a bug of some kind.
--------------------------------------------------------------------------------
*/

#ifdef XCHAL_DOUBLEEXC_VECTOR_VADDR

    .begin      literal_prefix .DoubleExceptionVector
    .section    .DoubleExceptionVector.text, "ax"
    .global     _DoubleExceptionVector
    .align      4
    .literal_position
_DoubleExceptionVector:
    wsr     a0, EXCSAVE_1
    call0   _xt_ext_panic

    #if XCHAL_HAVE_DEBUG
    break   1, 4                            /* unhandled double exception */
    #endif
    call0   user_fatal_exception_handler                       /* does not return */
    rfde                                    /* make a0 point here not later */

    .end        literal_prefix

#endif /* XCHAL_DOUBLEEXC_VECTOR_VADDR */

/*
--------------------------------------------------------------------------------
Kernel Exception (including Level 1 Interrupt from kernel mode).
--------------------------------------------------------------------------------
*/

    .begin      literal_prefix .KernelExceptionVector
    .section    .KernelExceptionVector.text, "ax"
    .global     _KernelExceptionVector
    .align      4
    .literal_position
_KernelExceptionVector:

    wsr     a0, EXCSAVE_1                   /* preserve a0 */
    call0   _xt_ext_panic
    //call0   _xt_kernel_exc                  /* kernel exception handler */
    /* never returns here - call0 is used as a jump (see note at top) */

    .end        literal_prefix

//    .section    .iram.text
    .section    .text
    .align      4
    .literal_position
_xt_kernel_exc:
    #if XCHAL_HAVE_DEBUG
    break   1, 0                            /* unhandled kernel exception */
    #endif
    call0   user_fatal_exception_handler                       /* does not return */
    rfe                                     /* make a0 point here not there */


/*
--------------------------------------------------------------------------------
User Exception (including Level 1 Interrupt from user mode).
--------------------------------------------------------------------------------
*/

    .begin      literal_prefix .UserExceptionVector
    .section    .UserExceptionVector.text, "ax"
    .global     _UserExceptionVector
    .type       _UserExceptionVector,@function
    .align      4
    .literal_position
_UserExceptionVector:

    wsr     a0, EXCSAVE_1                   /* preserve a0 */
    call0   _xt_user_exc                    /* user exception handler */
    /* never returns here - call0 is used as a jump (see note at top) */

    .end        literal_prefix

//    .section    .iram.text
    .section    .text
    /*
    Insert some waypoints for jumping beyond the signed 8-bit range
    of conditional branch instructions, so the conditional branchces
    to specific-cause exception handlers are not taken in the mainline.
    Saves some cycles in the mainline.
    */

    #if XCHAL_HAVE_WINDOWED
    .align      4
_xt_to_alloca_exc:
    call0   _xt_alloca_exc                  /* in window vectors section */
    /* never returns here - call0 is used as a jump (see note at top) */
    #endif

    .align      4
    .literal_position
_xt_to_syscall_exc:
    call0   _xt_syscall_exc
    /* never returns here - call0 is used as a jump (see note at top) */

    /* User exception handler begins here. */
    .type       _xt_user_exc,@function
    .align      4
_xt_user_exc:
    /*
    Handle alloca and syscall exceptions before allocating stack frame and
    interacting with RTOS.
    */
    rsr     a0, EXCCAUSE
    #if XCHAL_HAVE_WINDOWED
    beqi    a0, EXCCAUSE_ALLOCA,  _xt_to_alloca_exc
    #endif
    beqi    a0, EXCCAUSE_SYSCALL, _xt_to_syscall_exc
    beqi    a0, EXCCAUSE_LOAD_STORE_ERROR, LoadStoreErrorHandler
    beqi    a0, EXCCAUSE_LEVEL1INTERRUPT, _xt_user_entry1

    call0   _xt_ext_panic

_xt_user_entry1:
    /* Allocate interrupt stack frame and save minimal context. */
    mov     a0, sp                          /* sp == a1 */
    addi    sp, sp, -XT_STK_FRMSZ           /* allocate interrupt stack frame */
    s32i    a0, sp, XT_STK_A1               /* save pre-interrupt SP */
    rsr     a0, PS                          /* save interruptee's PS */
    s32i    a0, sp, XT_STK_PS
    rsr     a0, EPC_1                       /* save interruptee's PC */
    s32i    a0, sp, XT_STK_PC
    rsr     a0, EXCSAVE_1                   /* save interruptee's a0 */
    s32i    a0, sp, XT_STK_A0
    movi    a0, _xt_user_exit               /* save exit point for dispatch */
    s32i    a0, sp, XT_STK_EXIT

    /*
    Handle co-processor exceptions after allocating stack frame and before
    interacting with RTOS.
    */
    #if XCHAL_CP_NUM > 0
    rsr     a0, EXCCAUSE
    bgeui   a0, EXCCAUSE_CP0_DISABLED, _xt_to_coproc_exc
.L_xt_user_exc_not_coproc:
    #endif

    /* Save rest of interrupt context and enter RTOS. */
    call0   XT_RTOS_INT_ENTER               /* common RTOS interrupt entry */

    /* !! We are now on the RTOS system stack !! */

    /* Set up PS for C, reenable hi-pri interrupts, and clear EXCM. */
    #ifdef __XTENSA_CALL0_ABI__
    movi    a0, PS_INTLEVEL(XCHAL_EXCM_LEVEL) | PS_UM
    #else
    movi    a0, PS_INTLEVEL(XCHAL_EXCM_LEVEL) | PS_UM | PS_WOE
    #endif
    wsr     a0, PS
    rsync

    /* !! It is OK to call C handlers after this point. !! */

    /* Handle exceptions per EXCCAUSE. */
    rsr     a2, EXCCAUSE                    /* a2 = exception cause */
    beqi    a2, EXCCAUSE_LEVEL1INTERRUPT, .L_xt_user_int    /* level 3 int */

    #ifdef XT_INTEXC_HOOKS
    /*
    Call exception hook to pre-handle exceptions (if installed).
    Pass EXCCAUSE in a2, and check result in a2 (if -1, skip default handling).
    */
    movi    a0, _xt_intexc_hooks
    l32i    a0, a0, 0                       /* user exception hook index 0 */
    beqz    a0, 1f
.Ln_xt_user_exc_call_hook:
    #ifdef __XTENSA_CALL0_ABI__
    callx0  a0
    beqi    a2, -1, .L_xt_user_done
    #else
    mov     a6, a2
    callx4  a0
    beqi    a6, -1, .L_xt_user_done
    mov     a2, a6
    #endif
1:
    #endif

    /* USER_EDIT:
    ADD ANY CUSTOM EXCEPTION HANDLER CODE HERE, OR CALL C HANDLER.
    The exeption cause is in A2. After handling, jump to .L_xt_user_int .
    Note on Call0 ABI: Callee-saved regs (a12-15) have not yet been saved,
    so should not be corrupted here. A C handler will not corrupt them.
    */

    /* If we get here, the exception has not been handled. */
.Ln_xt_user_unhandled:
    #if XCHAL_HAVE_DEBUG
    break   1, 1                            /* unhandled user exception */
    #endif
    call0   user_fatal_exception_handler

    /* Handle level 1 interrupts. OK to enable med-pri interrupts now. */
.L_xt_user_int:
    rsil    a0, 1                           /* reenable ints above level 1 */

    /*
    Get mask of pending, enabled interrupts at this level into a2.
    Comment this out if there is only one interrupt at this level.
    */
    rsr     a2, INTENABLE
    rsr     a3, INTERRUPT
    movi    a4, XCHAL_INTLEVEL1_MASK
    and     a2, a2, a3
    and     a2, a2, a4

    #ifdef XT_INTEXC_HOOKS
    /* Call interrupt hook if present to (pre)handle interrupts. */
    movi    a0, _xt_intexc_hooks
    l32i    a0, a0, 1<<2
    beqz    a0, 2f
.Ln_xt_user_int_call_hook:
    #ifdef __XTENSA_CALL0_ABI__
    callx0  a0
    beqz    a2, .L_xt_user_done
    #else
    mov     a6, a2
    callx4  a0
    beqz    a6, .L_xt_user_done
    mov     a2, a6
    #endif
2:
    #endif

    /* USER_EDIT:
    ADD LOW PRIORITY LEVEL 1 INTERRUPT HANDLER CODE HERE, OR CALL C HANDLER.
    At this point, a2 contains a mask of pending, enabled ints at this level.
    Note on Call0 ABI: Callee-saved regs (a12-15) have not yet been saved,
    so should not be corrupted here. A C handler will not corrupt them.
    HANDLER MUST CAUSE LEVEL TRIGGERED INTERRUPT REQUESTS TO BE DEASSERTED.
    When done, ensure a2 contains a mask of unhandled (still pending)
    enabled ints at this level, and fall through.
    */

    #if XT_TIMER_INTPRI == 1
.Ln_xt_user_int_timer:
    movi    a3,  0xFFBF
    and        a3,  a2, a3
    bnez    a3,  3f    
    /* Interrupt handler for the RTOS tick timer if at this level. */
    movi    a3,  XT_TIMER_INTEN             /* timer interrupt bit */
    /*bnone   a2,  a3,  3f*/
    #ifdef __XTENSA_CALL0_ABI__
    sub     a12, a2,  a3                    /* clear timer int and save mask */
    call0   XT_RTOS_TIMER_INT
    mov     a2,  a12                        /* recover mask of remaining ints */
    beqz    a2,  4f
    #else
    call4   XT_RTOS_TIMER_INT               /* a2 automatically preserved */
    sub     a2,  a2,  a3                    /* clear timer int from mask */
    #endif
3:
    movi    a3, usr_int_stack_top
    s32i    a1, a3, 0
    mov     a1, a3

    //call0   _xt_isr_handler
    movi a0,   _xt_isr_handler
    callx0 a0

    movi    a3, usr_int_stack_top
    l32i    a1, a3, 0
   
    bnez    a2, .Ln_xt_user_int_timer
    #endif
4:
    /* All interrupts at this level should have been handled now. */
    beqz    a2, .L_xt_user_done

    /* If we get here, we have an unhandled interrupt. */
    #if XCHAL_HAVE_DEBUG
    break   1, 1                            /* unhandled user exception    */
                                            /* EXCCAUSE == 4 (level 1 int) */
    #endif
    call0   user_fatal_exception_handler

    /* Done handling after XT_RTOS_INT_ENTER. Give control to RTOS. */
.L_xt_user_done:
    call0   XT_RTOS_INT_EXIT                /* does not return directly here */
    
    /*
    Exit point for dispatch. Saved in interrupt stack frame at XT_STK_EXIT
    on entry and used to return to a thread or interrupted interrupt handler.
    */
    .global     _xt_user_exit
    .type       _xt_user_exit,@function
    .align      4
_xt_user_exit:
    l32i    a0, sp, XT_STK_PS               /* retrieve interruptee's PS */
    wsr     a0, PS
    l32i    a0, sp, XT_STK_PC               /* retrieve interruptee's PC */
    wsr     a0, EPC_1
    l32i    a0, sp, XT_STK_A0               /* retrieve interruptee's A0 */
    l32i    sp, sp, XT_STK_A1               /* remove interrupt stack frame */
    rsync                                   /* ensure PS and EPC written */
    rfe                                     /* PS.EXCM is cleared */

/*
--------------------------------------------------------------------------------
Syscall Exception Handler (jumped to from User Exception Handler).
Syscall 0 is required to spill the register windows (no-op in Call 0 ABI).
Only syscall 0 is handled here. Other syscalls return -1 to caller in a2.
--------------------------------------------------------------------------------
*/

//    .section    .iram.text
    .section    .text
    .type       _xt_syscall_exc,@function
    .align      4
_xt_syscall_exc:

    #ifdef __XTENSA_CALL0_ABI__
    /*
    Save minimal regs for scratch. Syscall 0 does nothing in Call0 ABI.
    Use a minimal stack frame (16B) to save A2 & A3 for scratch.
    PS.EXCM could be cleared here, but unlikely to improve worst-case latency.
    rsr     a0, PS
    addi    a0, a0, -PS_EXCM_MASK
    wsr     a0, PS
    */
    addi    sp, sp, -16
    s32i    a2, sp, 8
    s32i    a3, sp, 12
    #else   /* Windowed ABI */
    /*
    Save necessary context and spill the register windows.
    PS.EXCM is still set and must remain set until after the spill.
    Reuse context save function though it saves more than necessary.
    For this reason, a full interrupt stack frame is allocated.
    */
    addi    sp, sp, -XT_STK_FRMSZ           /* allocate interrupt stack frame */
    s32i    a12, sp, XT_STK_A12             /* _xt_context_save requires A12- */
    s32i    a13, sp, XT_STK_A13             /* A13 to have already been saved */
    call0   _xt_context_save
    #endif

    /*
    Grab the interruptee's PC and skip over the 'syscall' instruction.
    If it's at the end of a zero-overhead loop and it's not on the last
    iteration, decrement loop counter and skip to beginning of loop.
    */
    rsr     a2, EPC_1                       /* a2 = PC of 'syscall' */
    addi    a3, a2, 3                       /* ++PC                 */
    #if XCHAL_HAVE_LOOPS
    rsr     a0, LEND                        /* if (PC == LEND       */
    bne     a3, a0, 1f
    rsr     a0, LCOUNT                      /*     && LCOUNT != 0)  */
    beqz    a0, 1f                          /* {                    */
    addi    a0, a0, -1                      /*   --LCOUNT           */
    rsr     a3, LBEG                        /*   PC = LBEG          */
    wsr     a0, LCOUNT                      /* }                    */
    #endif
1:  wsr     a3, EPC_1                       /* update PC            */

    /* Restore interruptee's context and return from exception. */
    #ifdef __XTENSA_CALL0_ABI__
    l32i    a2, sp, 8
    l32i    a3, sp, 12
    addi    sp, sp, 16
    #else
    call0   _xt_context_restore
    addi    sp, sp, XT_STK_FRMSZ
    #endif
    movi    a0, -1
    movnez  a2, a0, a2                      /* return -1 if not syscall 0 */
    rsr     a0, EXCSAVE_1
    rfe


/*
Currently only shells for high priority interrupt handlers are provided
here. However a template and example can be found in the Tensilica tools
documentation: "Microprocessor Programmer's Guide".
*/

#if XCHAL_HAVE_NMI

    .begin      literal_prefix .NMIExceptionVector
    .section    .NMIExceptionVector.text, "ax"
    .global     _NMIExceptionVector
    .type       _NMIExceptionVector,@function
    .align      4
    .literal_position
_NMIExceptionVector:
    wsr     a0, EXCSAVE + XCHAL_NMILEVEL  _ /* preserve a0 */
    call0   _xt_nmi                         /* load interrupt handler */
    /* never returns here - call0 is used as a jump (see note at top) */

    .end        literal_prefix

//    .section    .iram.text
    .section    .text
    .type       _xt_nmi,@function
    .align      4
_xt_nmi:

    #ifdef XT_INTEXC_HOOKS
    #error
    /* Call interrupt hook if present to (pre)handle interrupts. */
    movi    a0, _xt_intexc_hooks
    l32i    a0, a0, XCHAL_NMILEVEL<<2
    beqz    a0, 1f
.Ln_xt_nmi_call_hook:
    callx0  a0                              /* must NOT disturb stack! */
1:
    #endif

    /* USER_EDIT:
    ADD HIGH PRIORITY NON-MASKABLE INTERRUPT (NMI) HANDLER CODE HERE.
    */

    movi    a0, LABEL(_Pri_,_NMICount)
    l32i    a0, a0, 0

    bnez    a0, nmi_reentry
    movi    a0, LABEL(_Pri_,_Stack) + PRI_N_STACK_SIZE  // get ptr to save area
    j       nmi_common
nmi_reentry:
    movi    a0, LABEL(_Pri_,_Stack) + PRI_N_STACK_SIZE + HESF_TOTALSIZE + PRI_N_STACK_SIZE2// get ptr to save area
nmi_common:

    // interlock

    //  Save a few registers so we can do some work:
    s32i    a2,  a0, HESF_AR(2)
#if HAVE_XSR
    //movi  a0, LABEL(_Level,FromVector)        // this dispatcher's address
    movi    a2, LABEL(_Pri_,_HandlerAddress)    // dispatcher address var.
    s32i    a1,  a0, HESF_AR(1)
    l32i    a2, a2, 0               // get dispatcher address
    s32i    a3,  a0, HESF_AR(3)
    xsr a2, EXCSAVE_LEVEL   // get saved a0, restore dispatcher address
#else
    #error
    rsr a2, EXCSAVE_LEVEL   // get saved a0
    s32i    a1,  a0, HESF_AR(1)
    s32i    a3,  a0, HESF_AR(3)
#endif
    s32i    a4,  a0, HESF_AR(4)
    s32i    a2,  a0, HESF_AR(0)

    //  Save/restore all exception state
    //  (IMPORTANT:  this code assumes no general exceptions occur
    //   during the execution of this dispatcher until this state
    //   is completely saved and from the point it is restored.)
    //
    //  Exceptions that may normally occur within the C handler
    //  include window exceptions (affecting EPC1), alloca exceptions
    //  (affecting EPC1/EXCCAUSE and its handling uses EXCSAVE1),
    //  and possibly others depending on the particular C handler
    //  (possibly needing save/restore of EXCVADDR; and EXCVADDR
    //   is also possibly corrupted by any access thru an auto-refill
    //   way on a processor with a full MMU).
    //
    rsr a3, EPC1
    rsr a4, EXCCAUSE
    s32i    a3, a0, HESF_EPC1
    s32i    a4, a0, HESF_EXCCAUSE
#if !XCHAL_HAVE_XEA1
    rsr a3, EXCVADDR
    s32i    a3, a0, HESF_EXCVADDR
#endif
    rsr a4, EXCSAVE1
    s32i    a4, a0, HESF_EXCSAVE1

    // hardware bug: "rfi 3" instruction can be interrupt by NMI,
    // so here handle this issue
    rsr     a3, EPC3
    movi    a4, nmi_rfi
    beq     a3, a4, nmi_reentried
    s32i    a3, a0, HESF_EPC3
    rsr a4, EPS3
    s32i    a4, a0, HESF_EPS3
    
nmi_reentried:

#ifdef __XTENSA_WINDOWED_ABI__
    //  Save remainder of entire address register file (!):
    movi    a2, XCHAL_NUM_AREGS - 8     // how many saved so far
#endif

    s32i    a5,  a0, HESF_AR(5)
    s32i    a6,  a0, HESF_AR(6)
    s32i    a7,  a0, HESF_AR(7)

1:  s32i    a8,  a0, HESF_AR(8)
    s32i    a9,  a0, HESF_AR(9)
    s32i    a10, a0, HESF_AR(10)
    s32i    a11, a0, HESF_AR(11)
    s32i    a12, a0, HESF_AR(12)
    s32i    a13, a0, HESF_AR(13)
    s32i    a14, a0, HESF_AR(14)
    s32i    a15, a0, HESF_AR(15)

#ifdef __XTENSA_WINDOWED_ABI__
    addi    a8, a2, -8
    addi    a10, a0, 8*4
    rotw    2
    bnez    a2, 1b          // loop until done

    rotw    2
    // back to original a2 ...

    //  Save a few other registers required for C:
    rsr a3, WINDOWSTART
    rsr a4, WINDOWBASE
    s32i    a3, a0, HESF_WINDOWSTART
    s32i    a4, a0, HESF_WINDOWBASE

    //  Setup window registers for first caller:
    movi    a3, 1
    movi    a4, 0
    wsr a3, WINDOWSTART
    wsr a4, WINDOWBASE
    rsync

    //  Note:  register window has rotated, ie. a0..a15 clobbered.

#endif /* __XTENSA_WINDOWED_ABI__ */

    //movi    a1, LABEL(_Pri_,_Stack) + PRI_N_STACK_SIZE  // get ptr to save area (is also initial stack ptr)
    mov     a1, a0
    movi    a0, 0       // mark start of call frames in stack

    //  Critical state saved, a bit more to do to allow window exceptions...

    //  We now have a C-coherent stack and window state.
    //  Still have to fix PS while making sure interrupts stay disabled
    //  at the appropriate level (ie. level 2 and below are disabled in this case).

#if XCHAL_HAVE_XEA1
    #error
    movi    a7, _xtos_intstruct     // address of interrupt management globals
    rsilft  a3, _INTERRUPT_LEVEL, XTOS_LOCKLEVEL    // lockout
    movi    a4, ~INTLEVEL_N_BELOW_MASK  // mask out all interrupts at this level or lower
    l32i    a3, a7, XTOS_VPRI_ENABLED_OFS   // read previous _xtos_vpri_enabled
    l32i    a5, a7, XTOS_ENABLED_OFS    // read _xtos_enabled
    s32i    a4, a7, XTOS_VPRI_ENABLED_OFS   // set new _xtos_vpri_enabled (mask interrupts as if at _INTERRUPT_LEVEL)
    s32i    a3, a1, HESF_VPRI       // save previous vpri
    movi    a2, 0x50020         // WOE=1, UM=1, INTLEVEL=0
    and a3, a5, a4          // mask out selected interrupts
    wsr a3, INTENABLE           // disable all low-priority interrupts
#else
    //  Load PS for C code, clear EXCM (NOTE: this step is different for XEA1):
# ifdef __XTENSA_CALL0_ABI__
    movi    a2, 0x00020 + _INTERRUPT_LEVEL  // WOE=0, CALLINC=0, UM=1, INTLEVEL=N, EXCM=0, RING=0
# else
    movi    a2, 0x50020 + _INTERRUPT_LEVEL  // WOE=1, CALLINC=1, UM=1, INTLEVEL=N, EXCM=0, RING=0
# endif

#endif
    wsr a2, PS              // update PS to enable window exceptions, etc as per above
    rsync

    //  Okay, window exceptions can now happen (although we have to call
    //  deep before any will happen because we've reset WINDOWSTART).

    //  Save other state that might get clobbered by C code:

//////////////////  COMMON DISPATCH CODE BEGIN

    rsr a14, SAR
    s32i    a14, a1, HESF_SAR
#if XCHAL_HAVE_LOOPS
    #error
    rsr a14, LCOUNT
    s32i    a14, a1, HESF_LCOUNT
    rsr a14, LBEG
    s32i    a14, a1, HESF_LBEG
    rsr a14, LEND
    s32i    a14, a1, HESF_LEND
#endif
#if XCHAL_HAVE_MAC16
    #error
    rsr a14, ACCLO
    s32i    a14, a1, HESF_ACCLO
    rsr a14, ACCHI
    s32i    a14, a1, HESF_ACCHI
#endif

    //save NMI Count
    movi    a2, LABEL(_Pri_,_NMICount)
    l32i    a3, a2, 0
    addi    a3, a3, 1
    s32i    a3, a2, 0
    
#ifdef __XTENSA_CALL0_ABI__
//    movi a13, pwm_tim1_intr_handler
    movi a13, wDev_ProcessFiq
    callx0  a13         // call interrupt's C handler
#else
    movi a13, NMI_Handler
    callx4  a13         // call interrupt's C handler
#endif

    //Restore NMI level
    movi    a2, LABEL(_Pri_,_NMICount)
    l32i    a3, a2, 0
    addi    a3, a3, -1
    s32i    a3, a2, 0
    
    beqi    a3, 1, nmi_reentry2
    movi    a1, LABEL(_Pri_,_Stack) + PRI_N_STACK_SIZE  // get ptr to save area
    j       nmi_common2
nmi_reentry2:
    movi    a1, LABEL(_Pri_,_Stack) + PRI_N_STACK_SIZE + HESF_TOTALSIZE + PRI_N_STACK_SIZE2// get ptr to save area
nmi_common2:


    //  Restore everything, and return.

    //  Three temp registers are required for this code to be optimal (no interlocks) in
    //  T2xxx microarchitectures with 7-stage pipe; otherwise only two
    //  registers would be needed.
    //
#if XCHAL_HAVE_LOOPS
    l32i    a13, a1, HESF_LCOUNT
    l32i    a14, a1, HESF_LBEG
    l32i    a15, a1, HESF_LEND
    wsr a13, LCOUNT
    wsr a14, LBEG
    wsr a15, LEND
#endif

#if XCHAL_HAVE_MAC16
    l32i    a13, a1, HESF_ACCLO
    l32i    a14, a1, HESF_ACCHI
    wsr a13, ACCLO
    wsr a14, ACCHI
#endif
    l32i    a15, a1, HESF_SAR
    wsr a15, SAR

//////////////////  COMMON DISPATCH CODE END

#if XCHAL_HAVE_XEA1
    //  Here, a7 = address of interrupt management globals
    l32i    a4, a1, HESF_VPRI       // restore previous vpri
    rsil    a3, XTOS_LOCKLEVEL      // lockout
    l32i    a5, a7, XTOS_ENABLED_OFS    // read _xtos_enabled
    s32i    a4, a7, XTOS_VPRI_ENABLED_OFS   // set new _xtos_vpri_enabled
    movi    a2, 0x00020 + _INTERRUPT_LEVEL  // WOE=0, UM=1, INTLEVEL=N
    /* movi    a2, 0x00020 + 0  // WOE=0, UM=1, INTLEVEL=N */
    and a3, a5, a4          // mask out selected interrupts
    wsr a3, INTENABLE           // disable all low-priority interrupts
#else
    //  Load PS for interrupt exit, set EXCM:
    movi    a2, 0x00030 + _INTERRUPT_LEVEL  // WOE=0, CALLINC=0, UM=1, INTLEVEL=N, EXCM=1, RING=0
    /* movi    a2, 0x00030 + 0  // WOE=0, CALLINC=0, UM=1, INTLEVEL=N, EXCM=1, RING=0 */
#endif
    wsr a2, PS              // update PS to disable window exceptions, etc as per above
    rsync

    //  NOTE:  here for XEA1, restore INTENABLE etc...

#ifdef __XTENSA_WINDOWED_ABI__
    //  Restore window registers:
    l32i    a2, a1, HESF_WINDOWSTART
    l32i    a3, a1, HESF_WINDOWBASE
    wsr a2, WINDOWSTART
    wsr a3, WINDOWBASE
    rsync
    //  Note:  register window has rotated, ie. a0..a15 clobbered.

    //  Reload initial stack pointer:
    movi    a1, LABEL(_Pri_,_Stack) + PRI_N_STACK_SIZE  // - 16
    movi    a6, XCHAL_NUM_AREGS - 8     // how many saved so far
    addi    a7, a1, -8*4

    //  Restore entire register file (!):

1:
    addi    a14, a6, -8
    addi    a15, a7, 8*4
    l32i    a4, a15, HESF_AR(4)
    l32i    a5, a15, HESF_AR(5)
    l32i    a6, a15, HESF_AR(6)
    l32i    a7, a15, HESF_AR(7)
    l32i    a8, a15, HESF_AR(8)
    l32i    a9, a15, HESF_AR(9)
    l32i    a10,a15, HESF_AR(10)
    l32i    a11,a15, HESF_AR(11)
    rotw    2
    bnez    a6, 1b          // loop until done

    l32i    a4, a7, HESF_AR(12)
    l32i    a5, a7, HESF_AR(13)
    l32i    a6, a7, HESF_AR(14)
    l32i    a7, a7, HESF_AR(15)
    rotw    2

    // back to original a1 ...

#else  /* Call0 ABI: */

    l32i    a4, a1, HESF_AR(4)  // restore general registers
    l32i    a5, a1, HESF_AR(5)
    l32i    a6, a1, HESF_AR(6)
    l32i    a7, a1, HESF_AR(7)
    l32i    a8, a1, HESF_AR(8)
    l32i    a9, a1, HESF_AR(9)
    l32i    a10, a1, HESF_AR(10)
    l32i    a11, a1, HESF_AR(11)
    l32i    a12, a1, HESF_AR(12)
    l32i    a13, a1, HESF_AR(13)
    l32i    a14, a1, HESF_AR(14)
    l32i    a15, a1, HESF_AR(15)

#endif  /* __XTENSA_WINDOWED_ABI__ */

    //  Restore exception state:
    l32i    a2, a1, HESF_EPC1
    l32i    a3, a1, HESF_EXCCAUSE
    wsr a2, EPC1
    wsr a3, EXCCAUSE
#if !XCHAL_HAVE_XEA1
    l32i    a2, a1, HESF_EXCVADDR
    wsr a2, EXCVADDR
#endif
    l32i    a3, a1, HESF_EXCSAVE1
    wsr a3, EXCSAVE1
    l32i    a2, a1, HESF_EPC3
    wsr a2, EPC3
    l32i    a3, a1, HESF_EPS3
    wsr a3, EPS3

    l32i    a0,  a1, HESF_AR(0)

    /* Re-Open NMI */

    rsr a3, SAR
    movi a2, 0x3ff
    slli a2,a2,20
    wsr a3, SAR
    rsync
    movi a3, 1
    s32i a3,a2,0 

    l32i    a2,  a1, HESF_AR(2)
    l32i    a3,  a1, HESF_AR(3)
    l32i    a1,  a1, HESF_AR(1)

nmi_rfi:
    rfi XCHAL_NMILEVEL

#if 0
    .align  4
.L_xt_nmi_exit:
    rsr     a0, EXCSAVE + XCHAL_NMILEVEL    /* restore a0 */
    rfi     XCHAL_NMILEVEL
#endif
#endif  /* NMI */



#endif


