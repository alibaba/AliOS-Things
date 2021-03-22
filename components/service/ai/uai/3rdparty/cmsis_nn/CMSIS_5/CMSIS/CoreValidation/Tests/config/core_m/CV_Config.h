/*-----------------------------------------------------------------------------
 *      Name:         CV_Config.h
 *      Purpose:      CV Config header
 *----------------------------------------------------------------------------
 *      Copyright (c) 2017 - 2018 Arm Limited. All rights reserved.
 *----------------------------------------------------------------------------*/
#ifndef __CV_CONFIG_H
#define __CV_CONFIG_H

#include "RTE_Components.h"
#include CMSIS_device_header

#define RTE_CV_COREINSTR 1
#define RTE_CV_COREFUNC  1
#define RTE_CV_CORESIMD  1
#define RTE_CV_MPUFUNC   __MPU_PRESENT

//-------- <<< Use Configuration Wizard in Context Menu >>> --------------------

// <h> Common Test Settings
// <o> Print Output Format <0=> Plain Text <1=> XML
// <i> Set the test results output format to plain text or XML
#ifndef PRINT_XML_REPORT
#define PRINT_XML_REPORT            1
#endif
// <o> Buffer size for assertions results
// <i> Set the buffer size for assertions results buffer
#define BUFFER_ASSERTIONS           128U
// </h>

// <h> Disable Test Cases
// <i> Uncheck to disable an individual test case
// <q0> TC_CoreInstr_NOP
#define TC_COREINSTR_NOP_EN                        1
// <q0> TC_CoreInstr_SEV
#define TC_COREINSTR_SEV_EN                        1
// <q0> TC_CoreInstr_BKPT
#define TC_COREINSTR_BKPT_EN                       1
// <q0> TC_CoreInstr_ISB
#define TC_COREINSTR_ISB_EN                        1
// <q0> TC_CoreInstr_DSB
#define TC_COREINSTR_DSB_EN                        1
// <q0> TC_CoreInstr_DMB
#define TC_COREINSTR_DMB_EN                        1
// <q0> TC_CoreInstr_WFI
#define TC_COREINSTR_WFI_EN                        0
// <q0> TC_CoreInstr_WFE
#define TC_COREINSTR_WFE_EN                        0

// <q0> TC_CoreInstr_REV
#define TC_COREINSTR_REV_EN                        1
// <q0> TC_CoreInstr_REV16
#define TC_COREINSTR_REV16_EN                      1
// <q0> TC_CoreInstr_REVSH
#define TC_COREINSTR_REVSH_EN                      1
// <q0> TC_CoreInstr_ROR
#define TC_COREINSTR_ROR_EN                        1
// <q0> TC_CoreInstr_RBIT
#define TC_COREINSTR_RBIT_EN                       1
// <q0> TC_CoreInstr_CLZ
#define TC_COREINSTR_CLZ_EN                        1
// <q0> TC_CoreInstr_SSAT
#define TC_COREINSTR_SSAT_EN                       1
// <q0> TC_CoreInstr_USAT
#define TC_COREINSTR_USAT_EN                       1
// <q0> TC_CoreInstr_RRX
#define TC_COREINSTR_RRX_EN                        1
// <q0> TC_CoreInstr_LoadStoreExlusive
#define TC_COREINSTR_LOADSTOREEXCLUSIVE_EN         1
// <q0> TC_CoreInstr_LoadStoreUnpriv
#define TC_COREINSTR_LOADSTOREUNPRIV_EN            1
// <q0> TC_CoreInstr_LoadStoreAcquire
#define TC_COREINSTR_LOADSTOREACQUIRE_EN           1
// <q0> TC_CoreInstr_LoadStoreAcquireExclusive
#define TC_COREINSTR_LOADSTOREACQUIREEXCLUSIVE_EN  1
// <q0> TC_CoreInstr_UnalignedUint16
#define TC_COREINSTR_UNALIGNEDUINT16_EN            1
// <q0> TC_CoreInstr_UnalignedUint32
#define TC_COREINSTR_UNALIGNEDUINT32_EN            1

// <q0> TC_CoreSimd_SatAddSub
#define TC_CORESIMD_SATADDSUB_EN                   1
// <q0> TC_CoreSimd_ParSat16
#define TC_CORESIMD_PARSAT16_EN                    1
// <q0> TC_CoreSimd_PackUnpack
#define TC_CORESIMD_PACKUNPACK_EN                  1
// <q0> TC_CoreSimd_ParSel
#define TC_CORESIMD_PARSEL_EN                      1
// <q0> TC_CoreSimd_ParAddSub8
#define TC_CORESIMD_PARADDSUB8_EN                  1
// <q0> TC_CoreSimd_AbsDif8
#define TC_CORESIMD_ABSDIF8_EN                     1
// <q0> TC_CoreSimd_ParAddSub16
#define TC_CORESIMD_PARADDSUB16_EN                 1
// <q0> TC_CoreSimd_ParMul16
#define TC_CORESIMD_PARMUL16_EN                    1
// <q0> TC_CoreSimd_Pack16
#define TC_CORESIMD_PACK16_EN                      1
// <q0> TC_CoreSimd_MulAcc32
#define TC_CORESIMD_MULACC32_EN                    1

// <q0> TC_CoreFunc_EnDisIRQ
#define TC_COREFUNC_ENDISIRQ_EN                    1
// <q0> TC_CoreFunc_IRQPrio
#define TC_COREFUNC_IRQPRIO_EN                     1
// <q0> TC_CoreFunc_EncDecIRQPrio
#define TC_COREFUNC_ENCDECIRQPRIO_EN               1
// <q0> TC_CoreFunc_IRQVect
#define TC_COREFUNC_IRQVECT_EN                     1
// <q0> TC_CoreFunc_Control
#define TC_COREFUNC_CONTROL_EN                     1
// <q0> TC_CoreFunc_IPSR
#define TC_COREFUNC_IPSR_EN                        1
// <q0> TC_CoreFunc_APSR
#define TC_COREFUNC_APSR_EN                        1
// <q0> TC_CoreFunc_PSP
#define TC_COREFUNC_PSP_EN                         1
// <q0> TC_CoreFunc_MSP
#define TC_COREFUNC_MSP_EN                         1

// <q0> TC_CoreFunc_PSPLIM
#define TC_COREFUNC_PSPLIM_EN                      1
// <q0> TC_CoreFunc_PSPLIM_NS
#define TC_COREFUNC_PSPLIM_NS_EN                   1
// <q0> TC_CoreFunc_MSPLIM
#define TC_COREFUNC_MSPLIM_EN                      1
// <q0> TC_CoreFunc_MSPLIM_NS
#define TC_COREFUNC_MSPLIM_NS_EN                   1
// <q0> TC_CoreFunc_PRIMASK
#define TC_COREFUNC_PRIMASK_EN                     1
// <q0> TC_CoreFunc_FAULTMASK
#define TC_COREFUNC_FAULTMASK_EN                   1
// <q0> TC_CoreFunc_BASEPRI
#define TC_COREFUNC_BASEPRI_EN                     1
// <q0> TC_CoreFunc_FPUType
#define TC_COREFUNC_FPUTYPE_EN                     1
// <q0> TC_CoreFunc_FPSCR
#define TC_COREFUNC_FPSCR_EN                       1


// <q0> TC_MPU_SetClear
#define TC_MPU_SETCLEAR_EN                         1
// <q0> TC_MPU_Load
#define TC_MPU_LOAD_EN                             1

// </h>

#endif /* __CV_CONFIG_H */

