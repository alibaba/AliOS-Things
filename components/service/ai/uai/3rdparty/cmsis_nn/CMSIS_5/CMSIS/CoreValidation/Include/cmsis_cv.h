/*-----------------------------------------------------------------------------
 *      Name:         cmsis_cv.h
 *      Purpose:      cmsis_cv header
 *----------------------------------------------------------------------------
 *      Copyright (c) 2017 - 2018 Arm Limited. All rights reserved.
 *----------------------------------------------------------------------------*/
#ifndef __CMSIS_CV_H
#define __CMSIS_CV_H

#include <stdint.h>
#include "CV_Config.h"

/* Expansion macro used to create CMSIS Driver references */
#define EXPAND_SYMBOL(name, port) name##port
#define CREATE_SYMBOL(name, port) EXPAND_SYMBOL(name, port)

// Simulator counter
#ifndef HW_PRESENT
extern uint32_t SIM_CYCCNT;
#endif

// SVC interrupt callback
extern void (*TST_IRQHandler)(void);

// Test main function
extern void cmsis_cv (void);
extern void cmsis_cv_abort (const char *fn, uint32_t ln, char *desc);

// Test cases
extern void TC_CoreInstr_NOP (void);
extern void TC_CoreInstr_SEV (void);
extern void TC_CoreInstr_BKPT (void);
extern void TC_CoreInstr_ISB (void);
extern void TC_CoreInstr_DSB (void);
extern void TC_CoreInstr_DMB (void);
extern void TC_CoreInstr_WFI (void);
extern void TC_CoreInstr_WFE (void);
extern void TC_CoreInstr_REV (void);
extern void TC_CoreInstr_REV16 (void);
extern void TC_CoreInstr_REVSH (void);
extern void TC_CoreInstr_ROR (void);
extern void TC_CoreInstr_RBIT (void);
extern void TC_CoreInstr_CLZ (void);
extern void TC_CoreInstr_SSAT (void);
extern void TC_CoreInstr_USAT (void);
extern void TC_CoreInstr_RRX (void);
extern void TC_CoreInstr_LoadStoreExclusive (void);
extern void TC_CoreInstr_LoadStoreUnpriv (void);
extern void TC_CoreInstr_LoadStoreAcquire (void);
extern void TC_CoreInstr_LoadStoreAcquireExclusive (void);
extern void TC_CoreInstr_UnalignedUint16 (void);
extern void TC_CoreInstr_UnalignedUint32 (void);

extern void TC_CoreSimd_SatAddSub (void);
extern void TC_CoreSimd_ParSat16 (void);
extern void TC_CoreSimd_PackUnpack (void);
extern void TC_CoreSimd_ParSel (void);
extern void TC_CoreSimd_ParAddSub8 (void);
extern void TC_CoreSimd_AbsDif8 (void);
extern void TC_CoreSimd_ParAddSub16 (void);
extern void TC_CoreSimd_ParMul16 (void);
extern void TC_CoreSimd_Pack16 (void);
extern void TC_CoreSimd_MulAcc32 (void);

#if defined(__CORTEX_M)
  extern void TC_CoreFunc_EnDisIRQ (void);
  extern void TC_CoreFunc_IRQPrio (void);
  extern void TC_CoreFunc_EncDecIRQPrio (void);
  extern void TC_CoreFunc_IRQVect (void);
  extern void TC_CoreFunc_Control (void);
  extern void TC_CoreFunc_IPSR (void);
  extern void TC_CoreFunc_APSR (void);
  extern void TC_CoreFunc_PSP (void);
  extern void TC_CoreFunc_MSP (void);
  extern void TC_CoreFunc_PSPLIM (void);
  extern void TC_CoreFunc_PSPLIM_NS (void);
  extern void TC_CoreFunc_MSPLIM (void);
  extern void TC_CoreFunc_MSPLIM_NS (void);
  extern void TC_CoreFunc_PRIMASK (void);
  extern void TC_CoreFunc_FAULTMASK (void);
  extern void TC_CoreFunc_BASEPRI (void);
  extern void TC_CoreFunc_FPUType (void);
  extern void TC_CoreFunc_FPSCR (void);
#elif defined(__CORTEX_A)
  extern void TC_CoreAFunc_IRQ (void);
  extern void TC_CoreAFunc_FPSCR (void);
  extern void TC_CoreAFunc_CPSR (void);
  extern void TC_CoreAFunc_Mode (void);
  extern void TC_CoreAFunc_SP (void);
  extern void TC_CoreAFunc_SP_usr (void);
  extern void TC_CoreAFunc_FPEXC (void);
  extern void TC_CoreAFunc_ACTLR (void);
  extern void TC_CoreAFunc_CPACR (void);
  extern void TC_CoreAFunc_DFSR (void);
  extern void TC_CoreAFunc_IFSR (void);
  extern void TC_CoreAFunc_ISR (void);
  extern void TC_CoreAFunc_CBAR (void);
  extern void TC_CoreAFunc_TTBR0 (void);
  extern void TC_CoreAFunc_DACR (void);
  extern void TC_CoreAFunc_SCTLR (void);
  extern void TC_CoreAFunc_ACTRL (void);
  extern void TC_CoreAFunc_MPIDR (void);
  extern void TC_CoreAFunc_VBAR (void);
  extern void TC_CoreAFunc_MVBAR (void);
  extern void TC_CoreAFunc_FPU_Enable (void);
#endif

extern void TC_MPU_SetClear (void);
extern void TC_MPU_Load (void);

#if defined(__CORTEX_A)
extern void TC_GenTimer_CNTFRQ (void);
extern void TC_GenTimer_CNTP_TVAL (void);
extern void TC_GenTimer_CNTP_CTL (void);
extern void TC_GenTimer_CNTPCT(void);
extern void TC_GenTimer_CNTP_CVAL(void);

extern void TC_L1Cache_EnDisable(void);
extern void TC_L1Cache_EnDisableBTAC(void);
extern void TC_L1Cache_log2_up(void);
extern void TC_L1Cache_InvalidateDCacheAll(void);
extern void TC_L1Cache_CleanDCacheAll(void);
extern void TC_L1Cache_CleanInvalidateDCacheAll(void);
#endif

#endif /* __CMSIS_CV_H */
