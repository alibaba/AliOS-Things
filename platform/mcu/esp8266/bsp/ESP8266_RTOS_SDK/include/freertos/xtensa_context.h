/*******************************************************************************
Copyright (c) 2006-2008 by Tensilica Inc.  ALL RIGHTS RESERVED.
These coded instructions, statements, and computer programs are the
copyrighted works and confidential proprietary information of Tensilica Inc.
They may not be modified, copied, reproduced, distributed, or disclosed to
third parties in any manner, medium, or form, in whole or in part, without
the prior written consent of Tensilica Inc.
--------------------------------------------------------------------------------

        XTENSA CONTEXT FRAMES AND MACROS FOR RTOS ASSEMBLER SOURCES

This header contains definitions and macros for use primarily by Xtensa
RTOS assembly coded source files. It includes and uses the Xtensa hardware
abstraction layer (HAL) to deal with config specifics. It may also be
included in C source files.

!! Supports only Xtensa Exception Architecture 2 (XEA2). XEA1 not supported. !!

NOTE: The Xtensa architecture requires stack pointer alignment to 16 bytes.

*******************************************************************************/

#ifndef XTENSA_CONTEXT_H
#define XTENSA_CONTEXT_H

#ifdef __ASSEMBLER__
#include    <xtensa/coreasm.h>
#endif

#include    <xtensa/config/tie.h>
#include    <xtensa/corebits.h>
#include    <xtensa/config/system.h>


/*
Align a value up to nearest n-byte boundary, where n is a power of 2.
*/
#define ALIGNUP(n, val) (((val) + (n)-1) & -(n))


/*******************************************************************************

INTERRUPT STACK FRAME FOR A THREAD OR NESTED INTERRUPT

A stack frame of this structure is allocated for any interrupt or exception.
It goes on the current stack. If the RTOS has a system stack for handling 
interrupts, every thread stack must allow space for just one interrupt stack 
frame, then nested interrupt stack frames go on the system stack.

The frame includes basic registers (explicit) and "extra" registers introduced 
by user TIE or the use of the MAC16 option in the user's Xtensa config.
The frame size is minimized by omitting regs not applicable to user's config.

For Windowed ABI, this stack frame includes the interruptee's base save area,
another base save area to manage gcc nested functions, and a little temporary 
space to help manage the spilling of the register windows.

*******************************************************************************/

#define XT_STK_EXIT             0x00    /* (offset 0) exit point for dispatch */
#define XT_STK_PC               0x04    /* return address */
#define XT_STK_PS               0x08    /* at level 1 PS.EXCM is set here */
#define XT_STK_A0               0x0C
#define XT_STK_A1               0x10    /* stack ptr before interrupt */
#define XT_STK_A2               0x14
#define XT_STK_A3               0x18
#define XT_STK_A4               0x1C
#define XT_STK_A5               0x20
#define XT_STK_A6               0x24
#define XT_STK_A7               0x28
#define XT_STK_A8               0x2C
#define XT_STK_A9               0x30
#define XT_STK_A10              0x34
#define XT_STK_A11              0x38
#define XT_STK_A12              0x3C    /* Call0 callee-save */
#define XT_STK_A13              0x40    /* Call0 callee-save */
#define XT_STK_A14              0x44    /* Call0 callee-save */
#define XT_STK_A15              0x48    /* Call0 callee-save */
#define XT_STK_SAR              0x4C

#if XCHAL_HAVE_LOOPS
#define XT_STK_LBEG             0x50
#define XT_STK_LEND             0x54
#define XT_STK_LCOUNT           0x58
#define XT_STK_NEXT1            0x5C    /* next unused offset */
#else
#define XT_STK_NEXT1            0x50    /* next unused offset */
#endif
                                /* there may be some unused space here */
#if XCHAL_EXTRA_SA_SIZE != 0
#define XT_STK_EXTRA            ALIGNUP(XCHAL_EXTRA_SA_ALIGN, XT_STK_NEXT1)

#define XT_STK_NEXT2            (XT_STK_EXTRA    + XCHAL_EXTRA_SA_SIZE)
#else
#define XT_STK_NEXT2            XT_STK_NEXT1   
#endif
                                        /* next unused offset */
                                /* there may be some unused space here */
#ifdef __XTENSA_CALL0_ABI__
/* Call0 - no more stack frame needed */
#define XT_STK_FRMSZ            ALIGNUP(0x10, XT_STK_NEXT2)
#else
/*
Windowed - 
    Need some temp space for saving stuff during window spill.
    Also add 16 bytes to skip over interruptee's base save area 
    and another 16 bytes in case of gcc nested functions: these 
    must be at physical top (logical base) of frame.
*/
#define XT_STK_N_TMP            3       /* # of 4-byte temp. slots */
#define XT_STK_TMP              XT_STK_NEXT2   
#define XT_STK_NEXT3            XT_STK_TMP    + (4 * XT_STK_N_TMP)
#define XT_STK_FRMSZ            (ALIGNUP(0x10, XT_STK_NEXT3) + 0x20)
#endif


/*******************************************************************************

SOLICTED STACK FRAME FOR A THREAD

A stack frame of this structure is allocated whenever a thread enters the 
RTOS kernel intentionally (and synchronously) to submit to thread scheduling.
It goes on the current thread's stack.

The solicted frame only includes registers that are required to be preserved
by the callee according to the compiler's ABI conventions, some space to save 
the return address for returning to the caller, and the caller's PS register.

For Windowed ABI, this stack frame includes the caller's base save area.

Note on XT_SOL_EXIT field:
      It is necessary to distinguish a solicited from an interrupt stack frame.
      This field corresponds to XT_STK_EXIT in the interrupt stack frame and is
      always at the same offset (0). It can be written with a code (usually 0) 
      to distinguish a solicted frame from an interrupt frame. An RTOS port may
      opt to ignore this field if it has another way of distinguishing frames.

*******************************************************************************/

#ifdef __XTENSA_CALL0_ABI__

/* Call0 ABI: room to save callee-save regs and return address. */
#define XT_SOL_EXIT             XT_STK_EXIT /* code indicates solicited frame */
#define XT_SOL_PC               0x04        /* return address */
#define XT_SOL_PS               0x08
#define XT_SOL_NEXT             0x0c        /* next unused offset */
                                /* there may be some unused space here */
#define XT_SOL_A12              ALIGNUP(0x10, XT_SOL_NEXT)
#define XT_SOL_A13              XT_SOL_A12    + 4
#define XT_SOL_A14              XT_SOL_A13    + 4
#define XT_SOL_A15              XT_SOL_A14    + 4
#define XT_SOL_FRMSZ            ALIGNUP(0x10, XT_SOL_A15)

#else

/* Windowed ABI: room to spill base-save area and save return address. */
#define XT_SOL_EXIT             XT_STK_EXIT /* code indicates solicited frame */
#define XT_SOL_PC               0x04    /* return address (b30-31=callinc) */
#define XT_SOL_PS               0x08
#define XT_SOL_NEXT             0x0c    /* next unused offset */
                                /* there may be some unused space here */
#define XT_SOL_A0               ALIGNUP(0x10, XT_SOL_NEXT)
#define XT_SOL_A1               XT_SOL_A0    + 4
#define XT_SOL_A2               XT_SOL_A1    + 4
#define XT_SOL_A3               XT_SOL_A2    + 4
#define XT_SOL_FRMSZ            ALIGNUP(0x10, XT_SOL_A3)

#endif


/*******************************************************************************

CO-PROCESSOR STATE SAVE AREA FOR A THREAD

The RTOS must provide an area per thread to save the state of co-processors
when that thread does not have control. Co-processors are context-switched
lazily (on demand) only when a new thread uses a co-processor instruction,
otherwise a thread retains ownership of the co-processor even when it loses
control of the processor. An Xtensa co-processor exception is triggered when
any co-processor instruction is executed by a thread that is not the owner,
and the context switch of that co-processor is then peformed by the handler.
Ownership represents which thread's state is currently in the co-processor.

Co-processors may not be used by interrupt or exception handlers. If an 
co-processor instruction is executed by an interrupt or exception handler,
the co-processor exception handler will trigger a kernel panic and freeze.
This restriction is introduced to reduce the overhead of saving and restoring
co-processor state (which can be quite large) and in particular remove that
overhead from interrupt handlers.

The co-processor state save area may be in any convenient per-thread location
such as in the thread control block or above the thread stack area. It need
not be in the interrupt stack frame since interrupts don't use co-processors.

Along with the save area for each co-processor, two bitmasks with flags per 
co-processor (laid out as in the CPENABLE reg) help manage context-switching
co-processors as efficiently as possible:

XT_CPENABLE
    The contents of a non-running thread's CPENABLE register.
    It represents the co-processors owned (and whose state is still needed)
    by the thread. When a thread is preempted, its CPENABLE is saved here.
    When a thread solicits a context-swtich, its CPENABLE is cleared - the
    compiler has saved the (caller-saved) co-proc state if it needs to.
    When a non-running thread loses ownership of a CP, its bit is cleared.
    When a thread runs, it's XT_CPENABLE is loaded into the CPENABLE reg.
    Avoids co-processor exceptions when no change of ownership is needed.

XT_CPSTORED
    A bitmask with the same layout as CPENABLE, a bit per co-processor.
    Indicates whether the state of each co-processor is saved in the state 
    save area. When a thread enters the kernel, only the state of co-procs
    still enabled in CPENABLE is saved. When the co-processor exception 
    handler assigns ownership of a co-processor to a thread, it restores 
    the saved state only if this bit is set, and clears this bit.

*******************************************************************************/

#if XCHAL_CP_NUM > 0
#define XT_CPENABLE 0
#define XT_CPSTORED (XT_CPENABLE + 1)
#define XT_CP0_SA   ALIGNUP(XCHAL_CP0_SA_ALIGN, XT_CPSTORED + 1)
#define XT_CP1_SA   ALIGNUP(XCHAL_CP1_SA_ALIGN, XT_CP0_SA + XCHAL_CP0_SA_SIZE)
#define XT_CP2_SA   ALIGNUP(XCHAL_CP2_SA_ALIGN, XT_CP1_SA + XCHAL_CP1_SA_SIZE)
#define XT_CP3_SA   ALIGNUP(XCHAL_CP3_SA_ALIGN, XT_CP2_SA + XCHAL_CP2_SA_SIZE)
#define XT_CP4_SA   ALIGNUP(XCHAL_CP4_SA_ALIGN, XT_CP3_SA + XCHAL_CP3_SA_SIZE)
#define XT_CP5_SA   ALIGNUP(XCHAL_CP5_SA_ALIGN, XT_CP4_SA + XCHAL_CP4_SA_SIZE)
#define XT_CP6_SA   ALIGNUP(XCHAL_CP6_SA_ALIGN, XT_CP5_SA + XCHAL_CP5_SA_SIZE)
#define XT_CP7_SA   ALIGNUP(XCHAL_CP7_SA_ALIGN, XT_CP6_SA + XCHAL_CP6_SA_SIZE)
#define XT_CP_SIZE  ALIGNUP(4                 , XT_CP7_SA + XCHAL_CP7_SA_SIZE)
#else
#define XT_CP_SIZE  0
#endif


/*******************************************************************************

MACROS TO HANDLE ABI SPECIFICS OF FUNCTION ENTRY AND RETURN

Convenient where the frame size requirements are the same for both ABIs.
    ENTRY(sz), RET(sz) are for framed functions (have locals or make calls).
    ENTRY0,    RET0    are for frameless functions (no locals, no calls).
where size = size of stack frame in bytes (must be >0 and aligned to 16).
For framed functions the frame is created and the return address saved at
base of frame (Call0 ABI) or as determined by hardware (Windowed ABI).
For frameless functions, there is no frame and return address remains in a0.
Note: Because CPP macros expand to a single line, macros requiring multi-line 
expansions are implemented as assembler macros.

*******************************************************************************/

#ifdef __ASSEMBLER__
#ifdef __XTENSA_CALL0_ABI__
  /* Call0 */
  #define ENTRY(sz)     entry1  sz
    .macro  entry1 size=0x10
    addi    sp, sp, -\size
    s32i    a0, sp, 0
    .endm
  #define ENTRY0      
  #define RET(sz)       ret1    sz
    .macro  ret1 size=0x10
    l32i    a0, sp, 0
    addi    sp, sp, \size
    ret
    .endm
  #define RET0          ret
#else
  /* Windowed */
  #define ENTRY(sz)     entry   sp, sz
  #define ENTRY0        entry   sp, 0x10
  #define RET(sz)       retw
  #define RET0          retw
#endif
#endif


#endif /* XTENSA_CONTEXT_H */
