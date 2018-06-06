/* Copyright (C) 2016 RDA Technologies Limited and/or its affiliates("RDA").
* All rights reserved.
*
* This software is supplied "AS IS" without any warranties.
* RDA assumes no responsibility or liability for the use of the software,
* conveys no license or title under any patent, copyright, or mask work
* right to the product. RDA reserves the right to make changes in the
* software without notification.  RDA also make no representation or
* warranty that such application will be suitable for the specified use
* without further testing or modification.
*/




#ifndef  _HALP_IRQ_HANDLER_H_
#define  _HALP_IRQ_HANDLER_H_

#define MANUAL_EXIT_SC 0 // not fully implemented anymore
#define USE_GDB_PAGE_IRQ


/// @todo move the IRQ_LOCATION into a configurable
/// piece of software
#define IRQ_LOCATION 0  // 0 = ram; Status_BEV = rom

#define IRQ_IN_SRAM

//  --------------------------------------------------
// Special registers usage
// k0 lost register in context restoration
// k1 keeps track of the level of IRQ
// gp used to save stack addr when jumping to irq,
// it can be changed when calling task swap from inside irq
//  --------------------------------------------------
//-----------------------
// small context map
//-----------------------
#ifdef __REDUCED_REGS__
#define TASK_SMALL_CTX_SIZE (20)
#define SC_S0        0
#define SC_S1        4
#define SC_A0        8
#define SC_PC        12
#define    SC_CTX_TYPE    16
#else
#define TASK_SMALL_CTX_SIZE    (48)
#define SC_S0        0
#define SC_S1        4
#define SC_S2        8
#define SC_S3        12
#define SC_S4        16
#define SC_S5        20
#define SC_S6        24
#define SC_S7        28
#define SC_FP        32
#define SC_A0        36
#define SC_PC        40
#define SC_CTX_TYPE    44
#endif //  __REDUCED_REGS__

// use sign bit to be able to use bltz/bgez
// also use 16b value to allow sign extended immediate loading with '| 1' for SC flag, as done in IRQ handler
#define CTX_TYPE_LONG 0xffff8000

//-----------------------
// extra context map
//-----------------------
#ifdef __REDUCED_REGS__
#define TASK_EXTRA_CTX_SIZE (56)
#define FC_LO    0
#define FC_HI    4
#define FC_V0    8
#define FC_V1    12
#define FC_A1    16
#define FC_A2    20
#define FC_A3    24
#define FC_T0    28
#define FC_T1    32
#define FC_T2    36
#define FC_T8    40
#define FC_T9    44
#define FC_RA    48
#define FC_AT    52
#else
#define TASK_EXTRA_CTX_SIZE (80)
#define FC_LO    0
#define FC_HI    4
#define FC_V0    8
#define FC_V1    12
#define FC_A1    16
#define FC_A2    20
#define FC_A3    24
#define FC_T0    28
#define FC_T1    32
#define FC_T2    36
#define FC_T3    40
#define FC_T4    44
#define FC_T5    48
#define FC_T6    52
#define FC_T7    56
#define FC_T8    60
#define FC_T9    64
#define FC_RA    68
#define FC_AT    72
#endif //  __REDUCED_REGS__
// not saved
// R0
// k0
// k1
// GP
// FP

// GP used inside irq, saved in irq stack

// K1 is used to record the level of IRQ we are in
//  0: means no IRQ : we are on a TASK
//  1 ... n : nomber of entered interrupt
//  the following bit is set when we are in a job's task
#define JOB_STACK_FLAG  0x01000000
//  the following bit is set when we are in the deep low power function
//  (only called from idle task)
#define LPS_STACK_FLAG  0x00001000


//-----------------------
// job context map
//-----------------------
#ifdef __REDUCED_REGS__
#define JOB_CTX_SIZE    (24)
#define JC_S0            0
#define JC_S1            4
#define JC_A0            8
#define JC_A1            12
#define JC_PC            16
#define JC_IS           20
#else
#define JOB_CTX_SIZE    (52)
#define JC_S0           0
#define JC_S1           4
#define JC_S2           8
#define JC_S3           12
#define JC_S4           16
#define JC_S5           20
#define JC_S6           24
#define JC_S7           28
#define JC_FP           32
#define JC_A0           36
#define JC_A1           40
#define JC_PC           44
#define JC_IS           48
#endif

#define CTX_TYPE_FROM_JOB   (0x10)


//-----------------------
// IRQ LINES
//-----------------------
#define HIP_FIRST_LINE  (0x400)
#define XCPU_HIP_MAIN_IRQ   (HIP_FIRST_LINE << XCPU_MAIN_IRQ_LINE)
#define XCPU_HIP_WDT        (HIP_FIRST_LINE << XCPU_WDT_IRQ_LINE)
#define XCPU_HIP_PAGE_SPY   (HIP_FIRST_LINE << XCPU_PAGE_SPY_IRQ_LINE)
#define XCPU_HIP_DEBUG      (HIP_FIRST_LINE << XCPU_DEBUG_IRQ_LINE)
#define XCPU_HIP_HOST       (HIP_FIRST_LINE << XCPU_HOST_IRQ_LINE)


#endif // _HALP_IRQ_HANDLER_H_
