/*-----------------------------------------------------------------------------
 *      Name:         CV_Config.h
 *      Purpose:      CV Config header
 *----------------------------------------------------------------------------
 *      Copyright (c) 2017 ARM Limited. All rights reserved.
 *----------------------------------------------------------------------------*/
#ifndef __CV_CONFIG_H
#define __CV_CONFIG_H

#include "RTE_Components.h"
#include CMSIS_device_header

//-------- <<< Use Configuration Wizard in Context Menu >>> --------------------

// <h> Common Test Settings
// <o> Print Output Format <0=> Plain Text <1=> XML
// <i> Set the test results output format to plain text or XML
#ifndef PRINT_XML_REPORT
#define PRINT_XML_REPORT            0
#endif
// <o> Buffer size for assertions results
// <i> Set the buffer size for assertions results buffer
#define BUFFER_ASSERTIONS           128U
// </h>

// <h> Disable Test Cases
// <i> Uncheck to disable an individual test case
// <q00> TC_CoreInstr_NOP
// <q01> TC_CoreInstr_REV
// <q02> TC_CoreInstr_REV16
// <q03> TC_CoreInstr_REVSH
// <q04> TC_CoreInstr_ROR
// <q05> TC_CoreInstr_RBIT
// <q06> TC_CoreInstr_CLZ
// <q07> TC_CoreInstr_SSAT
// <q08> TC_CoreInstr_USAT

#define TC_COREINSTR_NOP_EN                 1
#define TC_COREINSTR_REV_EN                 1
#define TC_COREINSTR_REV16_EN               1
#define TC_COREINSTR_REVSH_EN               1
#define TC_COREINSTR_ROR_EN                 1
#define TC_COREINSTR_RBIT_EN                1
#define TC_COREINSTR_CLZ_EN                 1
#define TC_COREINSTR_SSAT_EN                1
#define TC_COREINSTR_USAT_EN                1

// <q09> TC_CoreAFunc_FPSCR
// <q10> TC_CoreAFunc_CPSR
// <q11> TC_CoreAFunc_Mode
// <q12> TC_CoreAFunc_SP
// <q13> TC_CoreAFunc_SP_usr
// <q14> TC_CoreAFunc_FPEXC
// <q15> TC_CoreAFunc_ACTLR
// <q16> TC_CoreAFunc_CPACR
// <q17> TC_CoreAFunc_DFSR
// <q18> TC_CoreAFunc_IFSR
// <q19> TC_CoreAFunc_ISR
// <q20> TC_CoreAFunc_CBAR
// <q21> TC_CoreAFunc_TTBR0
// <q22> TC_CoreAFunc_DACR
// <q23> TC_CoreAFunc_SCTLR
// <q24> TC_CoreAFunc_ACTRL
// <q25> TC_CoreAFunc_MPIDR
// <q26> TC_CoreAFunc_VBAR
// <q27> TC_CoreAFunc_MVBAR
// <q28> TC_CoreAFunc_FPU_Enable

#define TC_COREAFUNC_IRQ                    1
#define TC_COREAFUNC_FPSCR                  1
#define TC_COREAFUNC_CPSR                   1
#define TC_COREAFUNC_MODE                   1
#define TC_COREAFUNC_SP                     1
#define TC_COREAFUNC_SP_USR                 1
#define TC_COREAFUNC_FPEXC                  1
#define TC_COREAFUNC_ACTLR                  1
#define TC_COREAFUNC_CPACR                  1
#define TC_COREAFUNC_DFSR                   1
#define TC_COREAFUNC_IFSR                   1
#define TC_COREAFUNC_ISR                    1
#define TC_COREAFUNC_CBAR                   1
#define TC_COREAFUNC_TTBR0                  1
#define TC_COREAFUNC_DACR                   1
#define TC_COREAFUNC_SCTLR                  1
#define TC_COREAFUNC_ACTRL                  1
#define TC_COREAFUNC_MPIDR                  1
#define TC_COREAFUNC_VBAR                   1
#define TC_COREAFUNC_MVBAR                  1
#define TC_COREAFUNC_FPU_ENABLE             1

// <q29> TC_GenTimer_CNTFRQ
// <q30> TC_GenTimer_CNTP_TVAL
// <q31> TC_GenTimer_CNTP_CTL
// <q32> TC_GenTimer_CNTPCT
// <q33> TC_GenTimer_CNTP_CVAL

#define TC_GENTIMER_CNTFRQ                  1
#define TC_GENTIMER_CNTP_TVAL               1
#define TC_GENTIMER_CNTP_CTL                1
#define TC_GENTIMER_CNTPCT                  1
#define TC_GENTIMER_CNTP_CVAL               1

// <q34> TC_L1Cache_EnDisable
// <q35> TC_L1Cache_EnDisableBTAC
// <q36> TC_L1Cache_log2_up
// <q37> TC_L1Cache_InvalidateDCacheAll
// <q38> TC_L1Cache_CleanDCacheAll
// <q39> TC_L1Cache_CleanInvalidateDCacheAll

#define TC_L1CACHE_ENDISABLE                1
#define TC_L1CACHE_ENDISABLEBTAC            1
#define TC_L1CACHE_LOG2_UP                  1
#define TC_L1CACHE_INVALIDATEDCACHEALL      1
#define TC_L1CACHE_CLEANDCACHEALL           1
#define TC_L1CACHE_CLEANINVALIDATEDCACHEALL 1
// </h>

#endif /* __CV_CONFIG_H */

