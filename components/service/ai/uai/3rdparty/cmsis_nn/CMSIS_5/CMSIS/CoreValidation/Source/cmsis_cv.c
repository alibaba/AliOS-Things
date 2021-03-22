/*-----------------------------------------------------------------------------
 *      Name:         cmsis_cv.c
 *      Purpose:      Driver validation test cases entry point
 *----------------------------------------------------------------------------
 *      Copyright (c) 2017 - 2018 Arm Limited. All rights reserved.
 *----------------------------------------------------------------------------*/
#include "cmsis_cv.h"
#include "RTE_Components.h"
#include "CV_Framework.h"
#include "CV_Config.h"

/*-----------------------------------------------------------------------------
 *      Prototypes
 *----------------------------------------------------------------------------*/

void Interrupt0_Handler(void);

/*-----------------------------------------------------------------------------
 *      Variables declarations
 *----------------------------------------------------------------------------*/

void (*TST_IRQHandler)(void);

void Interrupt0_Handler(void) {
  if (TST_IRQHandler != NULL) TST_IRQHandler();
}

/*-----------------------------------------------------------------------------
 *      Init test suite
 *----------------------------------------------------------------------------*/
static void TS_Init (void) {
  TST_IRQHandler = NULL;

#ifdef RTE_CV_MEASURETICKS
  StartCortexCycleCounter();
#endif
}

/*-----------------------------------------------------------------------------
 *      Test cases list
 *----------------------------------------------------------------------------*/
static TEST_CASE TC_LIST[] = {
#if defined(RTE_CV_COREINSTR) && RTE_CV_COREINSTR
  #if defined(__CORTEX_M)
    TCD ( TC_CoreInstr_NOP,                        TC_COREINSTR_NOP_EN                       ),
    TCD ( TC_CoreInstr_WFI,                        TC_COREINSTR_WFI_EN                       ),
    TCD ( TC_CoreInstr_WFE,                        TC_COREINSTR_WFE_EN                       ),
    TCD ( TC_CoreInstr_SEV,                        TC_COREINSTR_SEV_EN                       ),
    TCD ( TC_CoreInstr_BKPT,                       TC_COREINSTR_BKPT_EN                      ),
    TCD ( TC_CoreInstr_ISB,                        TC_COREINSTR_ISB_EN                       ),
    TCD ( TC_CoreInstr_DSB,                        TC_COREINSTR_DSB_EN                       ),
    TCD ( TC_CoreInstr_DMB,                        TC_COREINSTR_DMB_EN                       ),
    TCD ( TC_CoreInstr_REV,                        TC_COREINSTR_REV_EN                       ),
    TCD ( TC_CoreInstr_REV16,                      TC_COREINSTR_REV16_EN                     ),
    TCD ( TC_CoreInstr_REVSH,                      TC_COREINSTR_REVSH_EN                     ),
    TCD ( TC_CoreInstr_ROR,                        TC_COREINSTR_ROR_EN                       ),
    TCD ( TC_CoreInstr_RBIT,                       TC_COREINSTR_RBIT_EN                      ),
    TCD ( TC_CoreInstr_CLZ,                        TC_COREINSTR_CLZ_EN                       ),
    TCD ( TC_CoreInstr_SSAT,                       TC_COREINSTR_SSAT_EN                      ),
    TCD ( TC_CoreInstr_USAT,                       TC_COREINSTR_USAT_EN                      ),
    TCD ( TC_CoreInstr_RRX,                        TC_COREINSTR_RRX_EN                       ),
    TCD ( TC_CoreInstr_LoadStoreExclusive,         TC_COREINSTR_LOADSTOREEXCLUSIVE_EN        ),
    TCD ( TC_CoreInstr_LoadStoreUnpriv,            TC_COREINSTR_LOADSTOREUNPRIV_EN           ),
    TCD ( TC_CoreInstr_LoadStoreAcquire,           TC_COREINSTR_LOADSTOREACQUIRE_EN          ),
    TCD ( TC_CoreInstr_LoadStoreAcquireExclusive,  TC_COREINSTR_LOADSTOREACQUIREEXCLUSIVE_EN ),
    TCD ( TC_CoreInstr_UnalignedUint16,            TC_COREINSTR_UNALIGNEDUINT16_EN           ),
    TCD ( TC_CoreInstr_UnalignedUint32,            TC_COREINSTR_UNALIGNEDUINT32_EN           ),

  #elif defined(__CORTEX_A)
    TCD (TC_CoreInstr_NOP,                         TC_COREINSTR_NOP_EN                 ),
    TCD (TC_CoreInstr_REV,                         TC_COREINSTR_REV_EN                 ),
    TCD (TC_CoreInstr_REV16,                       TC_COREINSTR_REV16_EN               ),
    TCD (TC_CoreInstr_REVSH,                       TC_COREINSTR_REVSH_EN               ),
    TCD (TC_CoreInstr_ROR,                         TC_COREINSTR_ROR_EN                 ),
    TCD (TC_CoreInstr_RBIT,                        TC_COREINSTR_RBIT_EN                ),
    TCD (TC_CoreInstr_CLZ,                         TC_COREINSTR_CLZ_EN                 ),
    TCD (TC_CoreInstr_SSAT,                        TC_COREINSTR_SSAT_EN                ),
    TCD (TC_CoreInstr_USAT,                        TC_COREINSTR_USAT_EN                ),
    TCD (TC_CoreInstr_LoadStoreExclusive,          TC_COREINSTR_EXCLUSIVES_EN          ),
  #endif
#endif /* RTE_CV_COREINSTR */

#if defined (RTE_CV_CORESIMD) && RTE_CV_CORESIMD
    TCD ( TC_CoreSimd_SatAddSub,                   TC_CORESIMD_SATADDSUB_EN                  ),
    TCD ( TC_CoreSimd_ParSat16,                    TC_CORESIMD_PARSAT16_EN                   ),
    TCD ( TC_CoreSimd_PackUnpack,                  TC_CORESIMD_PACKUNPACK_EN                 ),
    TCD ( TC_CoreSimd_ParSel,                      TC_CORESIMD_PARSEL_EN                     ),
    TCD ( TC_CoreSimd_ParAddSub8,                  TC_CORESIMD_PARADDSUB8_EN                 ),
    TCD ( TC_CoreSimd_AbsDif8,                     TC_CORESIMD_ABSDIF8_EN                    ),
    TCD ( TC_CoreSimd_ParAddSub16,                 TC_CORESIMD_PARADDSUB16_EN                ),
    TCD ( TC_CoreSimd_ParMul16,                    TC_CORESIMD_PARMUL16_EN                   ),
    TCD ( TC_CoreSimd_Pack16,                      TC_CORESIMD_PACK16_EN                     ),
    TCD ( TC_CoreSimd_MulAcc32,                    TC_CORESIMD_MULACC32_EN                   ),
#endif /* RTE_CV_CORESIMD */

#if defined(RTE_CV_COREFUNC) && RTE_CV_COREFUNC
  #if defined(__CORTEX_M)
    TCD ( TC_CoreFunc_EnDisIRQ,                    TC_COREFUNC_ENDISIRQ_EN                   ),
    TCD ( TC_CoreFunc_IRQPrio,                     TC_COREFUNC_IRQPRIO_EN                    ),
    TCD ( TC_CoreFunc_EncDecIRQPrio,               TC_COREFUNC_ENCDECIRQPRIO_EN              ),
    TCD ( TC_CoreFunc_IRQVect,                     TC_COREFUNC_IRQVECT_EN                    ),
    TCD ( TC_CoreFunc_Control,                     TC_COREFUNC_CONTROL_EN                    ),
    TCD ( TC_CoreFunc_IPSR,                        TC_COREFUNC_IPSR_EN                       ),
    TCD ( TC_CoreFunc_APSR,                        TC_COREFUNC_APSR_EN                       ),
    TCD ( TC_CoreFunc_PSP,                         TC_COREFUNC_PSP_EN                        ),
    TCD ( TC_CoreFunc_MSP,                         TC_COREFUNC_MSP_EN                        ),
    TCD ( TC_CoreFunc_PSPLIM,                      TC_COREFUNC_PSPLIM_EN                     ),
    TCD ( TC_CoreFunc_PSPLIM_NS,                   TC_COREFUNC_PSPLIM_NS_EN                  ),
    TCD ( TC_CoreFunc_MSPLIM,                      TC_COREFUNC_MSPLIM_EN                     ),
    TCD ( TC_CoreFunc_MSPLIM_NS,                   TC_COREFUNC_MSPLIM_NS_EN                  ),
    TCD ( TC_CoreFunc_PRIMASK,                     TC_COREFUNC_PRIMASK_EN                    ),
    TCD ( TC_CoreFunc_FAULTMASK,                   TC_COREFUNC_FAULTMASK_EN                  ),
    TCD ( TC_CoreFunc_BASEPRI,                     TC_COREFUNC_BASEPRI_EN                    ),
    TCD ( TC_CoreFunc_FPUType,                     TC_COREFUNC_FPUTYPE_EN                    ),
    TCD ( TC_CoreFunc_FPSCR,                       TC_COREFUNC_FPSCR_EN                      ),

  #elif defined(__CORTEX_A)
    TCD ( TC_CoreAFunc_IRQ,                        TC_COREAFUNC_IRQ                          ),
    TCD ( TC_CoreAFunc_FPSCR,                      TC_COREAFUNC_FPSCR                        ),
    TCD ( TC_CoreAFunc_CPSR,                       TC_COREAFUNC_CPSR                         ),
    TCD ( TC_CoreAFunc_Mode,                       TC_COREAFUNC_MODE                         ),
    TCD ( TC_CoreAFunc_SP,                         TC_COREAFUNC_SP                           ),
    TCD ( TC_CoreAFunc_SP_usr,                     TC_COREAFUNC_SP_USR                       ),
    TCD ( TC_CoreAFunc_FPEXC,                      TC_COREAFUNC_FPEXC                        ),
    TCD ( TC_CoreAFunc_ACTLR,                      TC_COREAFUNC_ACTLR                        ),
    TCD ( TC_CoreAFunc_CPACR,                      TC_COREAFUNC_CPACR                        ),
    TCD ( TC_CoreAFunc_DFSR,                       TC_COREAFUNC_DFSR                         ),
    TCD ( TC_CoreAFunc_IFSR,                       TC_COREAFUNC_IFSR                         ),
    TCD ( TC_CoreAFunc_ISR,                        TC_COREAFUNC_ISR                          ),
    TCD ( TC_CoreAFunc_CBAR,                       TC_COREAFUNC_CBAR                         ),
    TCD ( TC_CoreAFunc_TTBR0,                      TC_COREAFUNC_TTBR0                        ),
    TCD ( TC_CoreAFunc_DACR,                       TC_COREAFUNC_DACR                         ),
    TCD ( TC_CoreAFunc_SCTLR,                      TC_COREAFUNC_SCTLR                        ),
    TCD ( TC_CoreAFunc_ACTRL,                      TC_COREAFUNC_ACTRL                        ),
    TCD ( TC_CoreAFunc_MPIDR,                      TC_COREAFUNC_MPIDR                        ),
    TCD ( TC_CoreAFunc_VBAR,                       TC_COREAFUNC_VBAR                         ),
    TCD ( TC_CoreAFunc_MVBAR,                      TC_COREAFUNC_MVBAR                        ),
    TCD ( TC_CoreAFunc_FPU_Enable,                 TC_COREAFUNC_FPU_ENABLE                   ),
  #endif
#endif /* RTE_CV_COREFUNC */

#if defined(RTE_CV_MPUFUNC) && RTE_CV_MPUFUNC
    TCD ( TC_MPU_SetClear,                         TC_MPU_SETCLEAR_EN                        ),
    TCD ( TC_MPU_Load,                             TC_MPU_LOAD_EN                            ),
#endif /* RTE_CV_MPUFUNC */

#if defined(RTE_CV_GENTIMER) && RTE_CV_GENTIMER
    TCD ( TC_GenTimer_CNTFRQ,                      TC_GENTIMER_CNTFRQ                        ),
    TCD ( TC_GenTimer_CNTP_TVAL,                   TC_GENTIMER_CNTP_TVAL                     ),
    TCD ( TC_GenTimer_CNTP_CTL,                    TC_GENTIMER_CNTP_CTL                      ),
    TCD ( TC_GenTimer_CNTPCT,                      TC_GENTIMER_CNTPCT                        ),
    TCD ( TC_GenTimer_CNTP_CVAL,                   TC_GENTIMER_CNTP_CVAL                     ),
#endif /* RTE_CV_GENTIMER */

#if defined(RTE_CV_L1CACHE) && RTE_CV_L1CACHE
    TCD ( TC_L1Cache_EnDisable,                    TC_L1CACHE_ENDISABLE                      ),
    TCD ( TC_L1Cache_EnDisableBTAC,                TC_L1CACHE_ENDISABLEBTAC                  ),
    TCD ( TC_L1Cache_log2_up,                      TC_L1CACHE_LOG2_UP                        ),
    TCD ( TC_L1Cache_InvalidateDCacheAll,          TC_L1CACHE_INVALIDATEDCACHEALL            ),
    TCD ( TC_L1Cache_CleanDCacheAll,               TC_L1CACHE_CLEANDCACHEALL                 ),
    TCD ( TC_L1Cache_CleanInvalidateDCacheAll,     TC_L1CACHE_CLEANINVALIDATEDCACHEALL       ),
#endif /* RTE_CV_L1CACHE */
};

#if defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdate-time"
#endif
/*-----------------------------------------------------------------------------
 *      Test suite description
 *----------------------------------------------------------------------------*/
TEST_SUITE ts = {
  __FILE__, __DATE__, __TIME__,
  "CMSIS-CORE Test Suite",
  TS_Init,
  1,
  TC_LIST,
  ARRAY_SIZE (TC_LIST),
};
#if defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
#pragma clang diagnostic pop
#endif
