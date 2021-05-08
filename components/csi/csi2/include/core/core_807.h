/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */


/******************************************************************************
 * @file     core_ck807.h
 * @brief    CSI CK807 Core Peripheral Access Layer Header File
 * @version  V1.0
 * @date     26. Jan 2018
 ******************************************************************************/

#ifndef __CORE_CK807_H_GENERIC
#define __CORE_CK807_H_GENERIC

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 *                 CSI definitions
 ******************************************************************************/
/**
  \ingroup CK807
  @{
 */

/*  CSI CK807 definitions */
#define __CK807_CSI_VERSION_MAIN  (0x04U)                                      /*!< [31:16] CSI HAL main version */
#define __CK807_CSI_VERSION_SUB   (0x1EU)                                      /*!< [15:0]  CSI HAL sub version */
#define __CK807_CSI_VERSION       ((__CK807_CSI_VERSION_MAIN << 16U) | \
                                   __CK807_CSI_VERSION_SUB           )         /*!< CSI HAL version number */

#ifndef __CK807
#define __CK807                   (0x07U)                                      /*!< CK807 Core */
#endif

/** __FPU_USED indicates whether an FPU is used or not.
*/
#define __FPU_USED       1U

#if defined ( __GNUC__ )
#if defined (__VFP_FP__) && !defined(__SOFTFP__)
#error "Compiler generates FPU instructions for a device without an FPU (check __FPU_PRESENT)"
#endif
#endif

#ifdef __cplusplus
}
#endif

#endif /* __CORE_CK807_H_GENERIC */

#ifndef __CSI_GENERIC

#ifndef __CORE_CK807_H_DEPENDANT
#define __CORE_CK807_H_DEPENDANT

#ifdef __cplusplus
extern "C" {
#endif

/* check device defines and use defaults */
#ifndef __CK807_REV
#define __CK807_REV               0x0000U
#endif

#ifndef __GSR_GCR_PRESENT
#define __GSR_GCR_PRESENT         0U
#endif

#ifndef __ICACHE_PRESENT
#define __ICACHE_PRESENT          1U
#endif

#ifndef __DCACHE_PRESENT
#define __DCACHE_PRESENT          1U
#endif

#include <core/csi_gcc.h>

/* IO definitions (access restrictions to peripheral registers) */
/**
    \defgroup CSI_glob_defs CSI Global Defines

    <strong>IO Type Qualifiers</strong> are used
    \li to specify the access to peripheral variables.
    \li for automatic generation of peripheral register debug information.
*/
#ifdef __cplusplus
#define     __I      volatile             /*!< Defines 'read only' permissions */
#else
#define     __I      volatile const       /*!< Defines 'read only' permissions */
#endif
#define     __O      volatile             /*!< Defines 'write only' permissions */
#define     __IO     volatile             /*!< Defines 'read / write' permissions */

/* following defines should be used for structure members */
#define     __IM     volatile const       /*! Defines 'read only' structure member permissions */
#define     __OM     volatile             /*! Defines 'write only' structure member permissions */
#define     __IOM    volatile             /*! Defines 'read / write' structure member permissions */

/*@} end of group CK807 */

/*******************************************************************************
 *                 Register Abstraction
  Core Register contain:
  - Core Register
 ******************************************************************************/
/**
  \defgroup CSI_core_register Defines and Type Definitions
  \brief Type definitions and defines for CK807 processor based devices.
*/

/**
  \ingroup    CSI_core_register
  \defgroup   CSI_CORE  Status and Control Registers
  \brief      Core Register type definitions.
  @{
 */

/**
  \brief  Access Processor Status Register(PSR)struct definition.
 */
typedef union {
    struct {
        uint32_t C: 1;                       /*!< bit:      0  Conditional code/Carry flag */
        uint32_t AF: 1;                      /*!< bit:      1  Alternate register valid control bit */
        uint32_t _reserved0: 2;              /*!< bit:  2.. 3  Reserved */
        uint32_t FE: 1;                      /*!< bit:      4  Fast interrupt enable control bit */
        uint32_t _reserved1: 1;              /*!< bit:      5  Reserved */
        uint32_t IE: 1;                      /*!< bit:      6  Interrupt effective control bit */
        uint32_t IC: 1;                      /*!< bit:      7  Interrupt control bit */
        uint32_t EE: 1;                      /*!< bit:      8  Abnormally effective control bit */
        uint32_t MM: 1;                      /*!< bit:      9  Unsymmetrical masking bit */
        uint32_t _reserved2: 2;              /*!< bit: 10..11  Reserved */
        uint32_t TE: 1;                      /*!< bit:     12  Trace transmission control bit */
        uint32_t TP: 1;                      /*!< bit:     13  Pending trace exception set bit */
        uint32_t TM: 2;                      /*!< bit: 14..15  Tracing mode bit */
        uint32_t VEC: 8;                     /*!< bit: 16..23  Abnormal event vector value */
        uint32_t _reserved3: 7;              /*!< bit: 24..30  Reserved */
        uint32_t S: 1;                       /*!< bit:     31  Superuser mode set bit */
    } b;                                     /*!< Structure    Access by bit */
    uint32_t w;                              /*!< Type         Access by whole register */
} PSR_Type;

/* PSR Register Definitions */
#define PSR_S_Pos                          31U                                            /*!< PSR: S Position */
#define PSR_S_Msk                          (0x1UL << PSR_S_Pos)                           /*!< PSR: S Mask */

#define PSR_VEC_Pos                        16U                                            /*!< PSR: VEC Position */
#define PSR_VEC_Msk                        (0xFFUL << PSR_VEC_Pos)                        /*!< PSR: VEC Mask */

#define PSR_TM_Pos                         14U                                            /*!< PSR: TM Position */
#define PSR_TM_Msk                         (0x3UL << PSR_TM_Pos)                          /*!< PSR: TM Mask */

#define PSR_TP_Pos                         13U                                            /*!< PSR: TP Position */
#define PSR_TP_Msk                         (0x1UL << PSR_TM_Pos)                          /*!< PSR: TP Mask */

#define PSR_TE_Pos                         12U                                            /*!< PSR: TE Position */
#define PSR_TE_Msk                         (0x1UL << PSR_TE_Pos)                          /*!< PSR: TE Mask */

#define PSR_MM_Pos                         9U                                             /*!< PSR: MM Position */
#define PSR_MM_Msk                         (0x1UL << PSR_MM_Pos)                          /*!< PSR: MM Mask */

#define PSR_EE_Pos                         8U                                             /*!< PSR: EE Position */
#define PSR_EE_Msk                         (0x1UL << PSR_EE_Pos)                          /*!< PSR: EE Mask */

#define PSR_IC_Pos                         7U                                             /*!< PSR: IC Position */
#define PSR_IC_Msk                         (0x1UL << PSR_IC_Pos)                          /*!< PSR: IC Mask */

#define PSR_IE_Pos                         6U                                             /*!< PSR: IE Position */
#define PSR_IE_Msk                         (0x1UL << PSR_IE_Pos)                          /*!< PSR: IE Mask */

#define PSR_FE_Pos                         4U                                             /*!< PSR: FE Position */
#define PSR_FE_Msk                         (0x1UL << PSR_FE_Pos)                          /*!< PSR: FE Mask */

#define PSR_AF_Pos                         1U                                             /*!< PSR: AF Position */
#define PSR_AF_Msk                         (0x1UL << PSR_AF_Pos)                          /*!< PSR: AF Mask */

#define PSR_C_Pos                          0U                                             /*!< PSR: C Position */
#define PSR_C_Msk                          (0x1UL << PSR_C_Pos)                           /*!< PSR: C Mask */

/**
  \brief Consortium definition for accessing Cache Configuration Registers(CCR, CR<18, 0>).
 */
typedef union {
    struct {
        uint32_t MP: 2;                      /*!< bit:  0.. 1  memory protection settings */
        uint32_t IE: 1;                      /*!< bit:      2  Instruction cache enable */
        uint32_t DE: 1;                      /*!< bit:      3  Data cache enable */
        uint32_t WB: 1;                      /*!< bit:      4  Cache write back */
        uint32_t RS: 1;                      /*!< bit:      5  Address return stack settings */
        uint32_t Z: 1;                       /*!< bit:      6  Allow predictive jump bit */
        uint32_t BE: 1;                      /*!< bit:      7  Endian mode */
        uint32_t SCK: 3;                     /*!< bit:  8..10  the clock ratio of the system and the processor */
        uint32_t _reserved0: 1;              /*!< bit:     11  Reserved */
        uint32_t WA: 1;                      /*!< bit:     12  Write allocate enable */
        uint32_t E_V2: 1;                    /*!< bit:     13  V2 Endian mode */
        uint32_t BSTE: 1;                    /*!< bit:     14  Burst transmit enable */
        uint32_t IPE: 1;                     /*!< bit:     15  Indirect predict enable */
        uint32_t _reserved1: 16;             /*!< bit: 16..31  Reserved */
    } b;                                   /*!< Structure    Access by bit */
    uint32_t w;                            /*!< Type         Access by whole register */
} CCR_Type;

/* CCR Register Definitions */
#define CCR_IPE_Pos                       15u                                            /*!< CCR: IPE Position */
#define CCR_IPE_Msk                       (0x1UL << CCR_IPE_Pos)                         /*!< CCR: IPE Mask */

#define CCR_BSTE_Pos                      14u                                            /*!< CCR: BSTE Position */
#define CCR_BSTE_Msk                      (0x1UL << CCR_BSTE_Pos)                        /*!< CCR: BSTE Mask */

#define CCR_E_V2_Pos                      13U                                            /*!< CCR: E_V2 Position */
#define CCR_E_V2_Msk                      (0x1UL << CCR_E_V2_Pos)                        /*!< CCR: E_V2 Mask */

#define CCR_WA_Pos                        12u                                            /*!< CCR: WA Position */
#define CCR_WA_Msk                        (0x1UL << CCR_WA_Pos)                          /*!< CCR: WA Mask */

#define CCR_SCK_Pos                       8U                                             /*!< CCR: SCK Position */
#define CCR_SCK_Msk                       (0x3UL << CCR_SCK_Pos)                         /*!< CCR: SCK Mask */

#define CCR_BE_Pos                        7U                                             /*!< CCR: BE Position */
#define CCR_BE_Msk                        (0x1UL << CCR_BE_Pos)                          /*!< CCR: BE Mask */

#define CCR_Z_Pos                         6U                                             /*!< CCR: Z Position */
#define CCR_Z_Msk                         (0x1UL << CCR_BE_Pos)                          /*!< CCR: Z Mask */

#define CCR_RS_Pos                        5U                                             /*!< CCR: RS Position */
#define CCR_RS_Msk                        (0x1UL << CCR_BE_Pos)                          /*!< CCR: RS Mask */

#define CCR_WB_Pos                        4U                                             /*!< CCR: WB Position */
#define CCR_WB_Msk                        (0x1UL << CCR_BE_Pos)                          /*!< CCR: WB Mask */

#define CCR_DE_Pos                        3U                                             /*!< CCR: DE Position */
#define CCR_DE_Msk                        (0x1UL << CCR_BE_Pos)                          /*!< CCR: DE Mask */

#define CCR_IE_Pos                        2U                                             /*!< CCR: IE Position */
#define CCR_IE_Msk                        (0x1UL << CCR_BE_Pos)                          /*!< CCR: IE Mask */

#define CCR_MP_Pos                        0U                                             /*!< CCR: MP Position */
#define CCR_MP_Msk                        (0x3UL << CCR_MP_Pos)                          /*!< CCR: MP Mask */

/**
  \brief  Consortium definition for accessing high ease access permission configutation registers(CAPR, CR<19,0>)
 */
typedef union {
    struct {
        uint32_t NX0: 1;                     /*!< bit:      0  Non executable attribute setting */
        uint32_t NX1: 1;                     /*!< bit:      1  Non executable attribute setting */
        uint32_t NX2: 1;                     /*!< bit:      2  Non executable attribute setting */
        uint32_t NX3: 1;                     /*!< bit:      3  Non executable attribute setting */
        uint32_t NX4: 1;                     /*!< bit:      4  Non executable attribute setting */
        uint32_t NX5: 1;                     /*!< bit:      5  Non executable attribute setting */
        uint32_t NX6: 1;                     /*!< bit:      6  Non executable attribute setting */
        uint32_t NX7: 1;                     /*!< bit:      7  Non executable attribute setting */
        uint32_t AP0: 2;                     /*!< bit:  8.. 9  access permissions settings bit */
        uint32_t AP1: 2;                     /*!< bit: 10..11  access permissions settings bit */
        uint32_t AP2: 2;                     /*!< bit: 12..13  access permissions settings bit */
        uint32_t AP3: 2;                     /*!< bit: 14..15  access permissions settings bit */
        uint32_t AP4: 2;                     /*!< bit: 16..17  access permissions settings bit */
        uint32_t AP5: 2;                     /*!< bit: 18..19  access permissions settings bit */
        uint32_t AP6: 2;                     /*!< bit: 20..21  access permissions settings bit */
        uint32_t AP7: 2;                     /*!< bit: 22..23  access permissions settings bit */
        uint32_t S0: 1;                      /*!< bit:     24  Security property settings */
        uint32_t S1: 1;                      /*!< bit:     25  Security property settings */
        uint32_t S2: 1;                      /*!< bit:     26  Security property settings */
        uint32_t S3: 1;                      /*!< bit:     27  Security property settings */
        uint32_t S4: 1;                      /*!< bit:     28  Security property settings */
        uint32_t S5: 1;                      /*!< bit:     29  Security property settings */
        uint32_t S6: 1;                      /*!< bit:     30  Security property settings */
        uint32_t S7: 1;                      /*!< bit:     31  Security property settings */
    } b;                                   /*!< Structure    Access by bit */
    uint32_t w;                            /*!< Type         Access by whole register */
} CAPR_Type;

/* CAPR Register Definitions */
#define CAPR_S7_Pos                        31U                                            /*!< CAPR: S7 Position */
#define CAPR_S7_Msk                        (1UL << CAPR_S7_Pos)                           /*!< CAPR: S7 Mask */

#define CAPR_S6_Pos                        30U                                            /*!< CAPR: S6 Position */
#define CAPR_S6_Msk                        (1UL << CAPR_S6_Pos)                           /*!< CAPR: S6 Mask */

#define CAPR_S5_Pos                        29U                                            /*!< CAPR: S5 Position */
#define CAPR_S5_Msk                        (1UL << CAPR_S5_Pos)                           /*!< CAPR: S5 Mask */

#define CAPR_S4_Pos                        28U                                            /*!< CAPR: S4 Position */
#define CAPR_S4_Msk                        (1UL << CAPR_S4_Pos)                           /*!< CAPR: S4 Mask */

#define CAPR_S3_Pos                        27U                                            /*!< CAPR: S3 Position */
#define CAPR_S3_Msk                        (1UL << CAPR_S3_Pos)                           /*!< CAPR: S3 Mask */

#define CAPR_S2_Pos                        26U                                            /*!< CAPR: S2 Position */
#define CAPR_S2_Msk                        (1UL << CAPR_S2_Pos)                           /*!< CAPR: S2 Mask */

#define CAPR_S1_Pos                        25U                                            /*!< CAPR: S1 Position */
#define CAPR_S1_Msk                        (1UL << CAPR_S1_Pos)                           /*!< CAPR: S1 Mask */

#define CAPR_S0_Pos                        24U                                            /*!< CAPR: S0 Position */
#define CAPR_S0_Msk                        (1UL << CAPR_S0_Pos)                           /*!< CAPR: S0 Mask */

#define CAPR_AP7_Pos                       22U                                            /*!< CAPR: AP7 Position */
#define CAPR_AP7_Msk                       (0x3UL << CAPR_AP7_Pos)                        /*!< CAPR: AP7 Mask */

#define CAPR_AP6_Pos                       20U                                            /*!< CAPR: AP6 Position */
#define CAPR_AP6_Msk                       (0x3UL << CAPR_AP6_Pos)                        /*!< CAPR: AP6 Mask */

#define CAPR_AP5_Pos                       18U                                            /*!< CAPR: AP5 Position */
#define CAPR_AP5_Msk                       (0x3UL << CAPR_AP5_Pos)                        /*!< CAPR: AP5 Mask */

#define CAPR_AP4_Pos                       16U                                            /*!< CAPR: AP4 Position */
#define CAPR_AP4_Msk                       (0x3UL << CAPR_AP4_Pos)                        /*!< CAPR: AP4 Mask */

#define CAPR_AP3_Pos                       14U                                            /*!< CAPR: AP3 Position */
#define CAPR_AP3_Msk                       (0x3UL << CAPR_AP3_Pos)                        /*!< CAPR: AP3 Mask */

#define CAPR_AP2_Pos                       12U                                            /*!< CAPR: AP2 Position */
#define CAPR_AP2_Msk                       (0x3UL << CAPR_AP2_Pos)                        /*!< CAPR: AP2 Mask */

#define CAPR_AP1_Pos                       10U                                            /*!< CAPR: AP1 Position */
#define CAPR_AP1_Msk                       (0x3UL << CAPR_AP1_Pos)                        /*!< CAPR: AP1 Mask */

#define CAPR_AP0_Pos                       8U                                             /*!< CAPR: AP0 Position */
#define CAPR_AP0_Msk                       (0x3UL << CAPR_AP0_Pos)                        /*!< CAPR: AP0 Mask */

#define CAPR_NX7_Pos                       7U                                             /*!< CAPR: NX7 Position */
#define CAPR_NX7_Msk                       (0x1UL << CAPR_NX7_Pos)                        /*!< CAPR: NX7 Mask */

#define CAPR_NX6_Pos                       6U                                             /*!< CAPR: NX6 Position */
#define CAPR_NX6_Msk                       (0x1UL << CAPR_NX6_Pos)                        /*!< CAPR: NX6 Mask */

#define CAPR_NX5_Pos                       5U                                             /*!< CAPR: NX5 Position */
#define CAPR_NX5_Msk                       (0x1UL << CAPR_NX5_Pos)                        /*!< CAPR: NX5 Mask */

#define CAPR_NX4_Pos                       4U                                             /*!< CAPR: NX4 Position */
#define CAPR_NX4_Msk                       (0x1UL << CAPR_NX4_Pos)                        /*!< CAPR: NX4 Mask */

#define CAPR_NX3_Pos                       3U                                             /*!< CAPR: NX3 Position */
#define CAPR_NX3_Msk                       (0x1UL << CAPR_NX3_Pos)                        /*!< CAPR: NX3 Mask */

#define CAPR_NX2_Pos                       2U                                             /*!< CAPR: NX2 Position */
#define CAPR_NX2_Msk                       (0x1UL << CAPR_NX2_Pos)                        /*!< CAPR: NX2 Mask */

#define CAPR_NX1_Pos                       1U                                             /*!< CAPR: NX1 Position */
#define CAPR_NX1_Msk                       (0x1UL << CAPR_NX1_Pos)                        /*!< CAPR: NX1 Mask */

#define CAPR_NX0_Pos                       0U                                             /*!< CAPR: NX0 Position */
#define CAPR_NX0_Msk                       (0x1UL << CAPR_NX0_Pos)                        /*!< CAPR: NX0 Mask */

/**
  \brief  Consortium definition for accessing high ease access permission configutation registers(CAPR1, CR<16,0>)
 */
typedef union {
    struct {
        uint32_t NX8: 1;                      /*!< bit:      0  Non executable attribute setting */
        uint32_t NX9: 1;                      /*!< bit:      1  Non executable attribute setting */
        uint32_t NX10: 1;                     /*!< bit:      2  Non executable attribute setting */
        uint32_t NX11: 1;                     /*!< bit:      3  Non executable attribute setting */
        uint32_t NX12: 1;                     /*!< bit:      4  Non executable attribute setting */
        uint32_t NX13: 1;                     /*!< bit:      5  Non executable attribute setting */
        uint32_t NX14: 1;                     /*!< bit:      6  Non executable attribute setting */
        uint32_t NX15: 1;                     /*!< bit:      7  Non executable attribute setting */
        uint32_t AP8: 2;                      /*!< bit:  8.. 9  access permissions settings bit */
        uint32_t AP9: 2;                      /*!< bit: 10..11  access permissions settings bit */
        uint32_t AP10: 2;                     /*!< bit: 12..13  access permissions settings bit */
        uint32_t AP11: 2;                     /*!< bit: 14..15  access permissions settings bit */
        uint32_t AP12: 2;                     /*!< bit: 16..17  access permissions settings bit */
        uint32_t AP13: 2;                     /*!< bit: 18..19  access permissions settings bit */
        uint32_t AP14: 2;                     /*!< bit: 20..21  access permissions settings bit */
        uint32_t AP15: 2;                     /*!< bit: 22..23  access permissions settings bit */
        uint32_t S8: 1;                       /*!< bit:     24  Security property settings */
        uint32_t S9: 1;                       /*!< bit:     25  Security property settings */
        uint32_t S10: 1;                      /*!< bit:     26  Security property settings */
        uint32_t S11: 1;                      /*!< bit:     27  Security property settings */
        uint32_t S12: 1;                      /*!< bit:     28  Security property settings */
        uint32_t S13: 1;                      /*!< bit:     29  Security property settings */
        uint32_t S14: 1;                      /*!< bit:     30  Security property settings */
        uint32_t S15: 1;                      /*!< bit:     31  Security property settings */
    } b;                                   /*!< Structure    Access by bit */
    uint32_t w;                            /*!< Type         Access by whole register */
} CAPR1_Type;

/* CAPR1 Register Definitions */
#define CAPR1_S15_Pos                        31U                                            /*!< CAPR1: S15 Position */
#define CAPR1_S15_Msk                        (1UL << CAPR1_S15_Pos)                         /*!< CAPR1: S15 Mask */

#define CAPR1_S14_Pos                        30U                                            /*!< CAPR1: S14 Position */
#define CAPR1_S14_Msk                        (1UL << CAPR1_S14_Pos)                         /*!< CAPR1: S14 Mask */

#define CAPR1_S13_Pos                        29U                                            /*!< CAPR1: S13 Position */
#define CAPR1_S13_Msk                        (1UL << CAPR1_S13_Pos)                         /*!< CAPR1: S13 Mask */

#define CAPR1_S12_Pos                        28U                                            /*!< CAPR1: S12 Position */
#define CAPR1_S12_Msk                        (1UL << CAPR1_S12_Pos)                         /*!< CAPR1: S12 Mask */

#define CAPR1_S11_Pos                        27U                                            /*!< CAPR1: S11 Position */
#define CAPR1_S11_Msk                        (1UL << CAPR1_S11_Pos)                         /*!< CAPR1: S11 Mask */

#define CAPR1_S10_Pos                        26U                                            /*!< CAPR1: S10 Position */
#define CAPR1_S10_Msk                        (1UL << CAPR1_S10_Pos)                         /*!< CAPR1: S10 Mask */

#define CAPR1_S9_Pos                         25U                                            /*!< CAPR1: S9 Position */
#define CAPR1_S9_Msk                         (1UL << CAPR1_S9_Pos)                          /*!< CAPR1: S9 Mask */

#define CAPR1_S8_Pos                         24U                                            /*!< CAPR1: S8 Position */
#define CAPR1_S8_Msk                         (1UL << CAPR1_S8_Pos)                          /*!< CAPR1: S8 Mask */

#define CAPR1_AP15_Pos                       22U                                            /*!< CAPR1: AP15 Position */
#define CAPR1_AP15_Msk                       (0x3UL << CAPR1_AP15_Pos)                      /*!< CAPR1: AP15 Mask */

#define CAPR1_AP14_Pos                       20U                                            /*!< CAPR1: AP14 Position */
#define CAPR1_AP14_Msk                       (0x3UL << CAPR1_AP14_Pos)                      /*!< CAPR1: AP14 Mask */

#define CAPR1_AP13_Pos                       18U                                            /*!< CAPR1: AP13 Position */
#define CAPR1_AP13_Msk                       (0x3UL << CAPR1_AP13_Pos)                      /*!< CAPR1: AP13 Mask */

#define CAPR1_AP12_Pos                       16U                                            /*!< CAPR1: AP12 Position */
#define CAPR1_AP12_Msk                       (0x3UL << CAPR1_AP12_Pos)                      /*!< CAPR1: AP12 Mask */

#define CAPR1_AP11_Pos                       14U                                            /*!< CAPR1: AP11 Position */
#define CAPR1_AP11_Msk                       (0x3UL << CAPR1_AP11_Pos)                      /*!< CAPR1: AP11 Mask */

#define CAPR1_AP10_Pos                       12U                                            /*!< CAPR1: AP10 Position */
#define CAPR1_AP10_Msk                       (0x3UL << CAPR1_AP10_Pos)                      /*!< CAPR1: AP10 Mask */

#define CAPR1_AP9_Pos                        10U                                            /*!< CAPR1: AP9 Position */
#define CAPR1_AP9_Msk                        (0x3UL << CAPR1_AP9_Pos)                       /*!< CAPR1: AP9 Mask */

#define CAPR1_AP8_Pos                        8U                                             /*!< CAPR1: AP8 Position */
#define CAPR1_AP8_Msk                        (0x3UL << CAPR1_AP8_Pos)                       /*!< CAPR1: AP8 Mask */

#define CAPR1_NX15_Pos                       7U                                             /*!< CAPR1: NX15 Position */
#define CAPR1_NX15_Msk                       (0x1UL << CAPR1_NX15_Pos)                      /*!< CAPR1: NX15 Mask */

#define CAPR1_NX14_Pos                       6U                                             /*!< CAPR1: NX14 Position */
#define CAPR1_NX14_Msk                       (0x1UL << CAPR1_NX14_Pos)                      /*!< CAPR1: NX14 Mask */

#define CAPR1_NX13_Pos                       5U                                             /*!< CAPR1: NX13 Position */
#define CAPR1_NX13_Msk                       (0x1UL << CAPR1_NX13_Pos)                      /*!< CAPR1: NX13 Mask */

#define CAPR1_NX12_Pos                       4U                                             /*!< CAPR1: NX12 Position */
#define CAPR1_NX12_Msk                       (0x1UL << CAPR1_NX12_Pos)                      /*!< CAPR1: NX12 Mask */

#define CAPR1_NX11_Pos                       3U                                             /*!< CAPR1: NX11 Position */
#define CAPR1_NX11_Msk                       (0x1UL << CAPR1_NX11_Pos)                      /*!< CAPR1: NX11 Mask */

#define CAPR1_NX10_Pos                       2U                                             /*!< CAPR1: NX10 Position */
#define CAPR1_NX10_Msk                       (0x1UL << CAPR1_NX10_Pos)                      /*!< CAPR1: NX10 Mask */

#define CAPR1_NX9_Pos                        1U                                             /*!< CAPR1: NX9 Position */
#define CAPR1_NX9_Msk                        (0x1UL << CAPR1_NX9_Pos)                       /*!< CAPR1: NX9 Mask */

#define CAPR1_NX8_Pos                        0U                                             /*!< CAPR1: NX8 Position */
#define CAPR1_NX8_Msk                        (0x1UL << CAPR1_NX8_Pos)                       /*!< CAPR1: NX8 Mask */

/**
  \brief  Consortium definition for accessing control register(PACR, CR<20,0>).
 */
typedef union {
    struct {
        uint32_t E: 1;                       /*!< bit:      0  Effective setting of protected area */
        uint32_t size: 5;                    /*!< bit:  1.. 5  Size of protected area */
        uint32_t base_addr: 26;              /*!< bit:   6..31 The high position of the address of a protected area */
    } b;                                     /*!< Structure    Access by bit */
    uint32_t w;                              /*!< Type         Access by whole register */
} PACR_Type;

    /* PACR Register Definitions */
#define PACR_BASE_ADDR_Pos                 6U                                            /*!< PACR: base_addr Position */
#define PACR_BASE_ADDR_Msk                 (0x3FFFFFFUL << PACR_BASE_ADDR_Pos)              /*!< PACR: base_addr Mask */

#define PACR_SIZE_Pos                      1U                                             /*!< PACR: Size Position */
#define PACR_SIZE_Msk                      (0x1FUL << PACR_SIZE_Pos)                      /*!< PACR: Size Mask */

#define PACR_E_Pos                         0U                                             /*!< PACR: E Position */
#define PACR_E_Msk                         (0x1UL << PACR_E_Pos)                          /*!< PACR: E Mask */

/**
  \brief  Consortium definition for accessing protection area selection register(PRSR,CR<21,0>).
 */
typedef union {
    struct {
        uint32_t RID: 4;                     /*!< bit:  0.. 3  Protected area index value */
        uint32_t _reserved0: 28;             /*!< bit:  2..28  Reserved */
    } b;                                     /*!< Structure    Access by bit */
    uint32_t w;                              /*!< Type         Access by whole register */
} PRSR_Type;

/* PRSR Register Definitions */
#define PRSR_RID_Pos                       0U                                            /*!< PRSR: RID Position */
#define PRSR_RID_Msk                       (0x3UL << PRSR_RID_Pos)                       /*!< PRSR: RID Mask */

/**
  \brief  Consortium definition for accessing protection area selection register(ATTR0,CR<26,0>).
 */
typedef union {
    struct {
        uint32_t Attr0_reserved: 1;                      /*!< bit:      0  Reserved */
        uint32_t Attr0_B: 1;                             /*!< bit:      1  Bufferable attribute setting */
        uint32_t Attr0_SO: 1;                            /*!< bit:      2  Strong-Order attribute setting */
        uint32_t Attr0_C: 1;                             /*!< bit:      3  Cacheable attribute setting */
        uint32_t Attr1_reserved: 1;                      /*!< bit:      4  Reserved */
        uint32_t Attr1_B: 1;                             /*!< bit:      5  Bufferable attribute setting */
        uint32_t Attr1_SO: 1;                            /*!< bit:      6  Strong-Order attribute setting */
        uint32_t Attr1_C: 1;                             /*!< bit:      7  Cacheable attribute setting */
        uint32_t Attr2_reserved: 1;                      /*!< bit:      8  Reserved */
        uint32_t Attr2_B: 1;                             /*!< bit:      9  Bufferable attribute setting */
        uint32_t Attr2_SO: 1;                            /*!< bit:      10  Strong-Order attribute setting */
        uint32_t Attr2_C: 1;                             /*!< bit:      11  Bufferable attribute setting */
        uint32_t Attr3_reserved: 1;                      /*!< bit:      12  Reserved */
        uint32_t Attr3_B: 1;                             /*!< bit:      13  Bufferable attribute setting */
        uint32_t Attr3_SO: 1;                            /*!< bit:      14  Strong-Order attribute setting */
        uint32_t Attr3_C: 1;                             /*!< bit:      15  Bufferable attribute setting */
        uint32_t Attr4_reserved: 1;                      /*!< bit:      16  Reserved */
        uint32_t Attr4_B: 1;                             /*!< bit:      17  Bufferable attribute setting */
        uint32_t Attr4_SO: 1;                            /*!< bit:      18  Strong-Order attribute setting */
        uint32_t Attr4_C: 1;                             /*!< bit:      19  Bufferable attribute setting */
        uint32_t Attr5_reserved: 1;                      /*!< bit:      20  Reserved */
        uint32_t Attr5_B: 1;                             /*!< bit:      21  Bufferable attribute setting */
        uint32_t Attr5_SO: 1;                            /*!< bit:      22  Strong-Order attribute setting */
        uint32_t Attr5_C: 1;                             /*!< bit:      23  Bufferable attribute setting */
        uint32_t Attr6_reserved: 1;                      /*!< bit:      24  Reserved */
        uint32_t Attr6_B: 1;                             /*!< bit:      25  Bufferable attribute setting */
        uint32_t Attr6_SO: 1;                            /*!< bit:      26  Strong-Order attribute setting */
        uint32_t Attr6_C: 1;                             /*!< bit:      27  Bufferable attribute setting */
        uint32_t Attr7_reserved: 1;                      /*!< bit:      28  Reserved */
        uint32_t Attr7_B: 1;                             /*!< bit:      29  Bufferable attribute setting */
        uint32_t Attr7_SO: 1;                            /*!< bit:      30  Strong-Order attribute setting */
        uint32_t Attr7_C: 1;                             /*!< bit:      31  Bufferable attribute setting */
    } b;                                     /*!< Structure    Access by bit */
    uint32_t w;                              /*!< Type         Access by whole register */
} ATTR0_Type;

/* ATTR0 Register Definitions */
#define ATTR0_RESERVED_Pos                       0U                                                  /*!< ATTR0: RESERVED Position */
#define ATTR0_RESERVED_Msk                       (0x1UL << ATTR0_RESERVED_Pos)                       /*!< ATTR0: RESERVED Mask */

#define ATTR0_B_Pos                              1U                                                  /*!< ATTR0: BUFFERABLE Position */
#define ATTR0_B_Msk                              (0x1UL << ATTR0_B_Pos)                              /*!< ATTR0: BUFFERABLE Mask */

#define ATTR0_SO_Pos                             2U                                                  /*!< ATTR0: STRONG-ORDER Position */
#define ATTR0_SO_Msk                             (0x1UL << ATTR0_SO_Pos)                             /*!< ATTR0: STRONG-ORDER Mask */

#define ATTR0_C_Pos                              3U                                                  /*!< ATTR0: CACHEABLE Position */
#define ATTR0_C_Msk                              (0x1UL << ATTR0_C_Pos)                              /*!< ATTR0: CACHEABLE Mask */

#define ATTR1_RESERVED_Pos                       4U                                                  /*!< ATTR1: RESERVED Position */
#define ATTR1_RESERVED_Msk                       (0x1UL << ATTR1_RESERVED_Pos)                       /*!< ATTR1: RESERVED Mask */

#define ATTR1_B_Pos                              5U                                                  /*!< ATTR1: BUFFERABLE Position */
#define ATTR1_B_Msk                              (0x1UL << ATTR1_B_Pos)                              /*!< ATTR1: BUFFERABLE Mask */

#define ATTR1_SO_Pos                             6U                                                  /*!< ATTR1: STRONG-ORDER Position */
#define ATTR1_SO_Msk                             (0x1UL << ATTR1_SO_Pos)                             /*!< ATTR1: STRONG-ORDER Mask */

#define ATTR1_C_Pos                              7U                                                  /*!< ATTR1: CACHEABLE Position */
#define ATTR1_C_Msk                              (0x1UL << ATTR1_C_Pos)                              /*!< ATTR1: CACHEABLE Mask */

#define ATTR2_RESERVED_Pos                       8U                                                  /*!< ATTR2: RESERVED Position */
#define ATTR2_RESERVED_Msk                       (0x1UL << ATTR2_RESERVED_Pos)                       /*!< ATTR2: RESERVED Mask */

#define ATTR2_B_Pos                              9U                                                  /*!< ATTR2: BUFFERABLE Position */
#define ATTR2_B_Msk                              (0x1UL << ATTR2_B_Pos)                              /*!< ATTR2: BUFFERABLE Mask */

#define ATTR2_SO_Pos                             10U                                                 /*!< ATTR2: STRONG-ORDER Position */
#define ATTR2_SO_Msk                             (0x1UL << ATTR2_SO_Pos)                             /*!< ATTR2: STRONG-ORDER Mask */

#define ATTR2_C_Pos                              11U                                                 /*!< ATTR2: CACHEABLE Position */
#define ATTR2_C_Msk                              (0x1UL << ATTR2_C_Pos)                              /*!< ATTR2: CACHEABLE Mask */

#define ATTR3_RESERVED_Pos                       12U                                                 /*!< ATTR3: RESERVED Position */
#define ATTR3_RESERVED_Msk                       (0x1UL << ATTR3_RESERVED_Pos)                       /*!< ATTR3: RESERVED Mask */

#define ATTR3_B_Pos                              13U                                                 /*!< ATTR3: BUFFERABLE Position */
#define ATTR3_B_Msk                              (0x1UL << ATTR3_B_Pos)                              /*!< ATTR3: BUFFERABLE Mask */

#define ATTR3_SO_Pos                             14U                                                 /*!< ATTR3: STRONG-ORDER Position */
#define ATTR3_SO_Msk                             (0x1UL << ATTR3_SO_Pos)                             /*!< ATTR3: STRONG-ORDER Mask */

#define ATTR3_C_Pos                              15U                                                 /*!< ATTR3: CACHEABLE Position */
#define ATTR3_C_Msk                              (0x1UL << ATTR3_C_Pos)                              /*!< ATTR3: CACHEABLE Mask */

#define ATTR4_RESERVED_Pos                       16U                                                 /*!< ATTR4: RESERVED Position */
#define ATTR4_RESERVED_Msk                       (0x1UL << ATTR4_RESERVED_Pos)                       /*!< ATTR4: RESERVED Mask */

#define ATTR4_B_Pos                              17U                                                 /*!< ATTR4: BUFFERABLE Position */
#define ATTR4_B_Msk                              (0x1UL << ATTR4_B_Pos)                              /*!< ATTR4: BUFFERABLE Mask */

#define ATTR4_SO_Pos                             18U                                                 /*!< ATTR4: STRONG-ORDER Position */
#define ATTR4_SO_Msk                             (0x1UL << ATTR4_SO_Pos                              /*!< ATTR4: STRONG-ORDER Mask */

#define ATTR4_C_Pos                              19U                                                 /*!< ATTR4: CACHEABLE Position */
#define ATTR4_C_Msk                              (0x1UL << ATTR4_C_Pos)                              /*!< ATTR4: CACHEABLE Mask */

#define ATTR5_RESERVED_Pos                       20U                                                 /*!< ATTR5: RESERVED Position */
#define ATTR5_RESERVED_Msk                       (0x1UL << ATTR5_RESERVED_Pos)                       /*!< ATTR5: RESERVED Mask */

#define ATTR5_B_Pos                              21U                                                 /*!< ATTR5: BUFFERABLE Position */
#define ATTR5_B_Msk                              (0x1UL << ATTR5_B_Pos)                              /*!< ATTR5: BUFFERABLE Mask */

#define ATTR5_SO_Pos                             22U                                                 /*!< ATTR5: STRONG-ORDER Position */
#define ATTR5_SO_Msk                             (0x1UL << ATTR5_SO_Pos)                             /*!< ATTR5: STRONG-ORDER Mask */

#define ATTR5_C_Pos                              23U                                                 /*!< ATTR5: CACHEABLE Position */
#define ATTR5_C_Msk                              (0x1UL << ATTR5_C_Pos)                              /*!< ATTR5: CACHEABLE Mask */

#define ATTR6_RESERVED_Pos                       24U                                                 /*!< ATTR6: RESERVED Position */
#define ATTR6_RESERVED_Msk                       (0x1UL << ATTR6_RESERVED_Pos)                       /*!< ATTR6: RESERVED Mask */

#define ATTR6_B_Pos                              25U                                                 /*!< ATTR6: BUFFERABLE Position */
#define ATTR6_B_Msk                              (0x1UL << ATTR6_B_Pos)                              /*!< ATTR6: BUFFERABLE Mask */

#define ATTR6_SO_Pos                             26U                                                 /*!< ATTR6: STRONG-ORDER Position */
#define ATTR6_SO_Msk                             (0x1UL << ATTR6_SO_Pos)                             /*!< ATTR6: STRONG-ORDER Mask */

#define ATTR6_C_Pos                              27U                                                 /*!< ATTR6: CACHEABLE Position */
#define ATTR6_C_Msk                              (0x1UL << ATTR6_C_Pos)                              /*!< ATTR6: CACHEABLE Mask */

#define ATTR7_RESERVED_Pos                       28U                                                 /*!< ATTR7: RESERVED Position */
#define ATTR7_RESERVED_Msk                       (0x1UL << ATTR7_RESERVED_Pos)                       /*!< ATTR7: RESERVED Mask */

#define ATTR7_B_Pos                              29U                                                 /*!< ATTR7: BUFFERABLE Position */
#define ATTR7_B_Msk                              (0x1UL << ATTR7_B_Pos)                              /*!< ATTR7: BUFFERABLE Mask */

#define ATTR7_SO_Pos                             30U                                                 /*!< ATTR7: STRONG-ORDER Position */
#define ATTR7_SO_Msk                             (0x1UL << ATTR7_SO_Pos)                             /*!< ATTR7: STRONG-ORDER Mask */

#define ATTR7_C_Pos                              31U                                                 /*!< ATTR7: CACHEABLE Position */
#define ATTR7_C_Msk                              (0x1UL << ATTR7_C_Pos)                              /*!< ATTR7: CACHEABLE Mask */

/**
  \brief  Consortium definition for accessing protection area selection register(ATTR1,CR<27,0>).
 */
typedef union {
    struct {
        uint32_t Attr8_reserved: 1;                      /*!< bit:      0  Reserved */
        uint32_t Attr8_B: 1;                             /*!< bit:      1  Bufferable attribute setting */
        uint32_t Attr8_SO: 1;                            /*!< bit:      2  Strong-Order attribute setting */
        uint32_t Attr8_C: 1;                             /*!< bit:      3  Cacheable attribute setting */
        uint32_t Attr9_reserved: 1;                      /*!< bit:      4  Reserved */
        uint32_t Attr9_B: 1;                             /*!< bit:      5  Bufferable attribute setting */
        uint32_t Attr9_SO: 1;                            /*!< bit:      6  Strong-Order attribute setting */
        uint32_t Attr9_C: 1;                             /*!< bit:      7  Cacheable attribute setting */
        uint32_t Attr10_reserved: 1;                     /*!< bit:      8  Reserved */
        uint32_t Attr10_B: 1;                            /*!< bit:      9  Bufferable attribute setting */
        uint32_t Attr10_SO: 1;                           /*!< bit:      10  Strong-Order attribute setting */
        uint32_t Attr10_C: 1;                            /*!< bit:      11  Bufferable attribute setting */
        uint32_t Attr11_reserved: 1;                     /*!< bit:      12  Reserved */
        uint32_t Attr11_B: 1;                            /*!< bit:      13  Bufferable attribute setting */
        uint32_t Attr11_SO: 1;                           /*!< bit:      14  Strong-Order attribute setting */
        uint32_t Attr11_C: 1;                            /*!< bit:      15  Bufferable attribute setting */
        uint32_t Attr12_reserved: 1;                     /*!< bit:      16  Reserved */
        uint32_t Attr12_B: 1;                            /*!< bit:      17  Bufferable attribute setting */
        uint32_t Attr12_SO: 1;                           /*!< bit:      18  Strong-Order attribute setting */
        uint32_t Attr12_C: 1;                            /*!< bit:      19  Bufferable attribute setting */
        uint32_t Attr13_reserved: 1;                     /*!< bit:      20  Reserved */
        uint32_t Attr13_B: 1;                            /*!< bit:      21  Bufferable attribute setting */
        uint32_t Attr13_SO: 1;                           /*!< bit:      22  Strong-Order attribute setting */
        uint32_t Attr13_C: 1;                            /*!< bit:      23  Bufferable attribute setting */
        uint32_t Attr14_reserved: 1;                     /*!< bit:      24  Reserved */
        uint32_t Attr14_B: 1;                            /*!< bit:      25  Bufferable attribute setting */
        uint32_t Attr14_SO: 1;                           /*!< bit:      26  Strong-Order attribute setting */
        uint32_t Attr14_C: 1;                            /*!< bit:      27  Bufferable attribute setting */
        uint32_t Attr15_reserved: 1;                     /*!< bit:      28  Reserved */
        uint32_t Attr15_B: 1;                            /*!< bit:      29  Bufferable attribute setting */
        uint32_t Attr15_SO: 1;                           /*!< bit:      30  Strong-Order attribute setting */
        uint32_t Attr15_C: 1;                            /*!< bit:      31  Bufferable attribute setting */
    } b;                                     /*!< Structure    Access by bit */
    uint32_t w;                              /*!< Type         Access by whole register */
} ATTR1_Type;

/* ATTR0 Register Definitions */
#define ATTR8_RESERVED_Pos                       0U                                                  /*!< ATTR8: RESERVED Position */
#define ATTR8_RESERVED_Msk                       (0x1UL << ATTR8_RESERVED_Pos)                       /*!< ATTR8: RESERVED Mask */

#define ATTR8_B_Pos                              1U                                                  /*!< ATTR8: BUFFERABLE Position */
#define ATTR8_B_Msk                              (0x1UL << ATTR8_B_Pos)                              /*!< ATTR8: BUFFERABLE Mask */

#define ATTR8_SO_Pos                             2U                                                  /*!< ATTR8: STRONG-ORDER Position */
#define ATTR8_SO_Msk                             (0x1UL << ATTR8_SO_Pos)                             /*!< ATTR8: STRONG-ORDER Mask */

#define ATTR8_C_Pos                              3U                                                  /*!< ATTR8: CACHEABLE Position */
#define ATTR8_C_Msk                              (0x1UL << ATTR8_C_Pos)                              /*!< ATTR8: CACHEABLE Mask */

#define ATTR9_RESERVED_Pos                       4U                                                  /*!< ATTR9: RESERVED Position */
#define ATTR9_RESERVED_Msk                       (0x1UL << ATTR9_RESERVED_Pos)                       /*!< ATTR9: RESERVED Mask */

#define ATTR9_B_Pos                              5U                                                  /*!< ATTR9: BUFFERABLE Position */
#define ATTR9_B_Msk                              (0x1UL << ATTR9_B_Pos)                              /*!< ATTR9: BUFFERABLE Mask */

#define ATTR9_SO_Pos                             6U                                                  /*!< ATTR9: STRONG-ORDER Position */
#define ATTR9_SO_Msk                             (0x1UL << ATTR9_SO_Pos)                             /*!< ATTR9: STRONG-ORDER Mask */

#define ATTR9_C_Pos                              7U                                                  /*!< ATTR9: CACHEABLE Position */
#define ATTR9_C_Msk                              (0x1UL << ATTR9_C_Pos)                              /*!< ATTR9: CACHEABLE Mask */


#define ATTR10_RESERVED_Pos                      8U                                                  /*!< ATTR10: RESERVED Position */
#define ATTR10_RESERVED_Msk                      (0x1UL << ATTR10_RESERVED_Pos)                      /*!< ATTR10: RESERVED Mask */

#define ATTR10_B_Pos                             9U                                                  /*!< ATTR10: BUFFERABLE Position */
#define ATTR10_B_Msk                             (0x1UL << ATTR10_B_Pos)                             /*!< ATTR10: BUFFERABLE Mask */

#define ATTR10_SO_Pos                            10U                                                 /*!< ATTR10: STRONG-ORDER Position */
#define ATTR10_SO_Msk                            (0x1UL << ATTR10_SO_Pos)                            /*!< ATTR10: STRONG-ORDER Mask */

#define ATTR10_C_Pos                             11U                                                 /*!< ATTR10: CACHEABLE Position */
#define ATTR10_C_Msk                             (0x1UL << ATTR10_C_Pos)                             /*!< ATTR10: CACHEABLE Mask */

#define ATTR11_RESERVED_Pos                      12U                                                 /*!< ATTR11: RESERVED Position */
#define ATTR11_RESERVED_Msk                      (0x1UL << ATTR11_RESERVED_Pos)                      /*!< ATTR11: RESERVED Mask */

#define ATTR11_B_Pos                             13U                                                 /*!< ATTR11: BUFFERABLE Position */
#define ATTR11_B_Msk                             (0x1UL << ATTR11_B_Pos)                             /*!< ATTR11: BUFFERABLE Mask */

#define ATTR11_SO_Pos                            14U                                                 /*!< ATTR11: STRONG-ORDER Position */
#define ATTR11_SO_Msk                            (0x1UL << ATTR11_SO_Pos)                            /*!< ATTR11: STRONG-ORDER Mask */

#define ATTR11_C_Pos                             15U                                                 /*!< ATTR11: CACHEABLE Position */
#define ATTR11_C_Msk                             (0x1UL << ATTR11_C_Pos)                             /*!< ATTR11: CACHEABLE Mask */


#define ATTR12_RESERVED_Pos                      16U                                                 /*!< ATTR12: RESERVED Position */
#define ATTR12_RESERVED_Msk                      (0x1UL << ATTR12_RESERVED_Pos)                      /*!< ATTR12: RESERVED Mask */

#define ATTR12_B_Pos                             17U                                                 /*!< ATTR12: BUFFERABLE Position */
#define ATTR12_B_Msk                             (0x1UL << ATTR12_B_Pos)                             /*!< ATTR12: BUFFERABLE Mask */

#define ATTR12_SO_Pos                            18U                                                 /*!< ATTR12: STRONG-ORDER Position */
#define ATTR12_SO_Msk                            (0x1UL << ATTR12_SO_Pos                             /*!< ATTR12: STRONG-ORDER Mask */

#define ATTR12_C_Pos                             19U                                                 /*!< ATTR12: CACHEABLE Position */
#define ATTR12_C_Msk                             (0x1UL << ATTR12_C_Pos)                             /*!< ATTR12: CACHEABLE Mask */

#define ATTR13_RESERVED_Pos                      20U                                                 /*!< ATTR13: RESERVED Position */
#define ATTR13_RESERVED_Msk                      (0x1UL << ATTR13_RESERVED_Pos)                      /*!< ATTR13: RESERVED Mask */

#define ATTR13_B_Pos                             21U                                                 /*!< ATTR13: BUFFERABLE Position */
#define ATTR13_B_Msk                             (0x1UL << ATTR13_B_Pos)                             /*!< ATTR13: BUFFERABLE Mask */

#define ATTR13_SO_Pos                            22U                                                 /*!< ATTR13: STRONG-ORDER Position */
#define ATTR13_SO_Msk                            (0x1UL << ATTR13_SO_Pos)                            /*!< ATTR13: STRONG-ORDER Mask */

#define ATTR13_C_Pos                             23U                                                 /*!< ATTR13: CACHEABLE Position */
#define ATTR13_C_Msk                             (0x1UL << ATTR13_C_Pos)                             /*!< ATTR13: CACHEABLE Mask */

#define ATTR14_RESERVED_Pos                      24U                                                 /*!< ATTR14: RESERVED Position */
#define ATTR14_RESERVED_Msk                      (0x1UL << ATTR14_RESERVED_Pos)                      /*!< ATTR14: RESERVED Mask */

#define ATTR14_B_Pos                             25U                                                 /*!< ATTR14: BUFFERABLE Position */
#define ATTR14_B_Msk                             (0x1UL << ATTR14_B_Pos)                             /*!< ATTR14: BUFFERABLE Mask */

#define ATTR14_SO_Pos                            26U                                                 /*!< ATTR14: STRONG-ORDER Position */
#define ATTR14_SO_Msk                            (0x1UL << ATTR14_SO_Pos)                            /*!< ATTR14: STRONG-ORDER Mask */

#define ATTR14_C_Pos                             27U                                                 /*!< ATTR14: CACHEABLE Position */
#define ATTR14_C_Msk                             (0x1UL << ATTR14_C_Pos)                             /*!< ATTR14: CACHEABLE Mask */

#define ATTR15_RESERVED_Pos                      28U                                                 /*!< ATTR15: RESERVED Position */
#define ATTR15_RESERVED_Msk                      (0x1UL << ATTR15_RESERVED_Pos)                      /*!< ATTR15: RESERVED Mask */

#define ATTR15_B_Pos                             29U                                                 /*!< ATTR15: BUFFERABLE Position */
#define ATTR15_B_Msk                             (0x1UL << ATTR15_B_Pos)                             /*!< ATTR15: BUFFERABLE Mask */

#define ATTR15_SO_Pos                            30U                                                 /*!< ATTR15: STRONG-ORDER Position */
#define ATTR15_SO_Msk                            (0x1UL << ATTR15_SO_Pos)                            /*!< ATTR15: STRONG-ORDER Mask */

#define ATTR15_C_Pos                             31U                                                 /*!< ATTR15: CACHEABLE Position */
#define ATTR15_C_Msk                             (0x1UL << ATTR15_C_Pos)                             /*!< ATTR15: CACHEABLE Mask */

/* CCR2 Register Definitions */
#define CCR2_CRECC_Pos                           7U                                                  /*!< CCR2: CRECC Position */
#define CCR2_CRECC_Msk                           (0x1UL << CCR2_CRECC_Pos)                           /*!< CCR2: CRECC Mask */

#define CCR2_ECC_Pos                             6U                                                  /*!< CCR2: ECC Position */
#define CCR2_ECC_Msk                             (0x1UL << CCR2_ECC_Pos)                             /*!< CCR2: ECC Mask */


/**
  \brief  Consortium definition for accessing mmu index register(MIR,CR<0,15>).
 */
typedef union {
    struct {
        uint32_t Index: 10;                  /*!< bit:   0.. 9   TLB index */
        uint32_t _reserved: 20;              /*!< bit:  10.. 29  Reserved */
        uint32_t TF: 1;                      /*!< bit:       30  TLB fatal error */
        uint32_t P: 1;                       /*!< bit:       31  TLBP instruction */
    } b;
    uint32_t w;
} MIR_Type;

/* MIR Register Definitions */
#define MIR_P_Pos                          31                                            /*!< PRSR: P(TLBP instruction) Position */
#define MIR_P_Msk                          (0x1UL << MIR_P_Pos)                          /*!< PRSR: P(TLBP instruction) Mask */

#define MIR_TF_Pos                         30                                            /*!< PRSR: Tfatal Position */
#define MIR_TF_Msk                         (0x1UL << MIR_TF_Pos)                         /*!< PRSR: Tfatal Mask */

#define MIR_Index_Pos                      0                                             /*!< PRSR: Index Position */
#define MIR_Index_Msk                      (0x3ffUL << MIR_Index_Pos)                    /*!< PRSR: Index Mask */


/**
  \brief  Consortium definition for accessing mmu entry of high physical address register(MEL, CR<2,15> and CR<3,15>).
 */
typedef union {
    struct {
        uint32_t G: 1;                        /*!< bit:       0   Global enbale bit */
        uint32_t V: 1;                        /*!< bit:       1   TLB mapping valid bit */
        uint32_t D: 1;                        /*!< bit:       2   TLB Page dirty bit */
        uint32_t C: 1;                        /*!< bit:       3   TLB Page cacheable bit */
        uint32_t SEC: 1;                      /*!< bit:       4   TLB Page security bit */
        uint32_t SO: 1;                       /*!< bit:       2   Strong order enable bit */
        uint32_t B: 1;                        /*!< bit:       2   TLB Page bufferable bit */
        uint32_t _reserved: 5;                /*!< bit:   7.. 11  Reserved */
        uint32_t PFN: 20;                     /*!< bit:  12.. 31  Physical frame number */
    } b;
    uint32_t w;
} MEL_Type;

/* MEL Register Definitions */
#define MEL_PFN_Pos                        12                                            /*!< MEL: PFN Position */
#define MEL_PFN_Msk                        (0xFFFFFUL << MEL_PFN_Pos)                    /*!< MEL: PFN Mask */

#define MEL_B_Pos                          6                                             /*!< MEL: B Position */
#define MEL_B_Msk                          (0x1UL << MEL_B_Pos)                          /*!< MEL: B Mask */

#define MEL_SO_Pos                         5                                             /*!< MEL: SO Position */
#define MEL_SO_Msk                         (0x1UL << MEL_SO_Pos)                         /*!< MEL: SO Mask */

#define MEL_SEC_Pos                        4                                             /*!< MEL: SEC Position */
#define MEL_SEC_Msk                        (0x1UL << MEL_SEC_Pos)                        /*!< MEL: SEC Mask */

#define MEL_C_Pos                          3                                             /*!< MEL: C Position */
#define MEL_C_Msk                          (0x1UL << MEL_C_Pos)                          /*!< MEL: C Mask */

#define MEL_D_Pos                          2                                             /*!< MEL: D Position */
#define MEL_D_Msk                          (0x1UL << MIR_D_Pos)                          /*!< MEL: D Mask */

#define MEL_V_Pos                          1                                             /*!< MEL: V Position */
#define MEL_V_Msk                          (0x1UL << MIR_V_Pos)                          /*!< MEL: V Mask */

#define MEL_G_Pos                          0                                             /*!< MEL: G Position */
#define MEL_G_Msk                          (0x1UL << MIR_G_Pos)                          /*!< MEL: G Mask */


/**
  \brief  Consortium definition for accessing mmu entry of high physical address register(MEH, CR<4,15>).
 */
typedef union {
    struct {
        uint32_t ASID :8;                     /*!< bit:   0.. 7   ASID */
        uint32_t _reserved :4;                /*!< bit:   7.. 10  Reserved */
        uint32_t VPN :20;                     /*!< bit:  11.. 31  Virtual page number */
    } b;
    uint32_t w;
} MEH_Type;

/* MEH Register Definitions */
#define MEH_VPN_Pos                        12                                             /*!< MEH: VPN Position */
#define MEH_VPN_Msk                        (0xFFFFFUL << MEH_VPN_Pos)                     /*!< MEH: VPN Mask */

#define MEH_ASID_Pos                       0                                              /*!< MEH: ASID Position */
#define MEH_ASID_Msk                       (0xFFUL << MEH_ASID_Pos)                       /*!< MEH: ASID Mask */


/**
  \brief  Consortium definition for accessing mmu entry of high physical address register(MPR, CR<6,15>). 
*/

typedef union {
    struct {
        uint32_t _reserved0: 13;              /*!< bit:   0.. 12   Reserved */
        uint32_t page_mask: 12;               /*!< bit:  13.. 24   Page mask */
        uint32_t _reserved1: 7;               /*!< bit:  25.. 31   Reserved */
    } b;
    uint32_t w;
} MPR_Type;

/* MPR Register Definitions */
#define MPR_PAGE_MASK_Pos                        13                                             /*!< MPR: PAGE_MASK Position */
#define MPR_PAGE_MASK_Msk                        (0xFFFUL << MPR_PAGE_MASK_Pos)                 /*!< MPR: PAGE_MASK Mask */


/**
  \brief  Consortium definition for accessing mmu entry of high physical address register(CR<8,15>).
 */
typedef union {
    struct {
        uint32_t ASID: 8;                     /*!< bit:   0.. 7   ASID */
        uint32_t _reserved: 17;               /*!< bit:   8.. 24  Reserved */
        uint32_t TLBINV_INDEX: 1;             /*!< bit:       25  TLBINV_INDEX */
        uint32_t TLBINV_ALL: 1;               /*!< bit:       26  TLBINV_ALL */
        uint32_t TLBINV: 1;                   /*!< bit:       27  TLBINV */
        uint32_t TLBWR: 1;                    /*!< bit:       28  TLBWR */
        uint32_t TLBWI: 1;                    /*!< bit:       29  TLBWI */
        uint32_t TLBR: 1;                     /*!< bit:       30  TLBR */
        uint32_t TLBP: 1;                     /*!< bit:       31  TLBP */
    } b;
    uint32_t w;
} MCIR_Type;

/* MCIR Register Definitions */
#define MCIR_TLBP_Pos                        31                                               /*!< MCIR: TLBP Position */
#define MCIR_TLBP_Msk                        (0x1UL << MCIR_TLBP_Pos)                         /*!< MCIR: TLBP Mask */

#define MCIR_TLBR_Pos                        30                                               /*!< MCIR: TLBR Position */
#define MCIR_TLBR_Msk                        (0x1UL << MCIR_TLBR_Pos)                         /*!< MCIR: TLBR Mask */

#define MCIR_TLBWI_Pos                       29                                               /*!< MCIR: TLBWI Position */
#define MCIR_TLBWI_Msk                       (0x1UL << MCIR_TLBWI_Pos)                        /*!< MCIR: TLBWI Mask */

#define MCIR_TLBWR_Pos                       28                                               /*!< MCIR: TLBWR Position */
#define MCIR_TLBWR_Msk                       (0x1UL << MCIR_TLBWR_Pos)                        /*!< MCIR: TLBWR Mask */

#define MCIR_TLBINV_Pos                      27                                               /*!< MCIR: TLBINV Position */
#define MCIR_TLBINV_Msk                      (0x1UL << MCIR_TLBINV_Pos)                       /*!< MCIR: TLBINV Mask */

#define MCIR_TLBINV_ALL_Pos                  26                                               /*!< MCIR: TLBINV_ALL Position */
#define MCIR_TLBINV_ALL_Msk                  (0x1UL << MCIR_TLBINV_ALL_Pos)                   /*!< MCIR: TLBINV_ALL Mask */

#define MCIR_TLBINV_INDEX_Pos                25                                               /*!< MCIR: TLBINV_INDEX Position */
#define MCIR_TLBINV_INDEX_Msk                (0x1UL << MCIR_TLBINV_INDEX_Pos)                 /*!< MCIR: TLBINV_INDEX Mask */

#define MCIR_ASID_Pos                        0                                                /*!< MCIR: ASID Position */
#define MCIR_ASID_Msk                        (0xFFUL << MCIR_ASID_Pos)                        /*!< MCIR: ASID Mask */


/*@} end of group CSI_CORE */


/**
  \ingroup    CSI_core_register
  \defgroup   CSI_CACHE
  \brief      Type definitions for the cache Registers
  @{
 */

/**
  \brief  Consortium definition for accessing protection area selection register(CFR,CR<17,0>).
 */
typedef union {
    struct {
        uint32_t CACHE_SEL: 2;                      /*!< bit:  0..1  Instruction and data cache selection */
        uint32_t _reserved0: 2;                     /*!< bit:  2..3  Reserved */
        uint32_t INV: 1;                            /*!< bit:     4  Invalid data in cache */
        uint32_t CLR: 1;                            /*!< bit:     5  Clear the dirty tlb table */
        uint32_t OMS: 1;                            /*!< bit:     6  Cache invalid and clear operation mode (one line or all line)*/
        uint32_t ITS: 1;                            /*!< bit:     7  Cache invalid and clear operation mode (CIR used as virtual index or SET/WAY/LEVE index)*/
        uint32_t _reserved1: 8;                     /*!< bit: 8..15  Reserved */
        uint32_t BHT_INV: 1;                        /*!< bit:    16  Invalid data in branch history table */
        uint32_t _reserved2: 14;                    /*!< bit: 17..30 Reserved */
        uint32_t LICF: 1;                           /*!< bit:     31 Failure of clearing or invalid cache line */
    } b;                                            /*!< Structure    Access by bit */
    uint32_t w;                                     /*!< Type         Access by whole register */
} CFR_Type;

#define CFR_LICF_Pos                     31U                                             /*!< CFR: LICF Position */
#define CFR_LICF_Msk                     (0x1UL << CFR_LICF_Pos)                         /*!< CFR: LICF Mask */

#define CFR_BHT_INV_Pos                  16U                                             /*!< CFR: BHT Position */
#define CFR_BHT_INV_Msk                  (0x1UL << CFR_BHT_INV_Pos)                      /*!< CFR: BHT Mask */

#define CFR_ITS_Pos                      7U                                              /*!< CFR: ITS Position */
#define CFR_ITS_Msk                      (0x1UL << CFR_ITS_Pos)                          /*!< CFR: ITS Mask */

#define CFR_OMS_Pos                      6U                                              /*!< CFR: OMS Position */
#define CFR_OMS_Msk                      (0x1UL << CFR_OMS_Pos)                          /*!< CFR: OMS Mask */

#define CFR_CLR_Pos                      5U                                              /*!< CFR: CLR Position */
#define CFR_CLR_Msk                      (0x1UL << CFR_CLR_Pos)                          /*!< CFR: CLR Mask */

#define CFR_INV_Pos                      4U                                              /*!< CFR: INV Position */
#define CFR_INV_Msk                      (0x1UL << CFR_INV_Pos)                          /*!< CFR: INV Mask */

#define CFR_CACHE_SEL_Pos                0                                               /*!< CFR: CACHE_SEL Position */    
#define CFR_CACHE_SEL_Msk                (0x3UL << CFR_CACHE_SEL_Pos)                    /*!< CFR: CACHE_SEL Masok */

/* CFR Register Definitions */
/*@} end of group CSI_CACHE */


/**
  \ingroup    CSI_core_register
  \defgroup   CSI_CACHE
  \brief      Type definitions for the cache Registers
  @{
 */
#define SSEG0_BASE_ADDR         0x80000000
#define CACHE_RANGE_MAX_SIZE    0x80000

#define INS_CACHE               (1 << 0)
#define DATA_CACHE              (1 << 1)
#define CACHE_INV               (1 << 4)
#define CACHE_CLR               (1 << 5)
#define CACHE_OMS               (1 << 6)
#define CACHE_ITS               (1 << 7)
#define CACHE_LICF              (1 << 31)

#define    L1_CACHE_SHIFT       4      /* 16 Bytes */
#define    L1_CACHE_BYTES       (1 << L1_CACHE_SHIFT)

/**
  \brief   Mask and shift a bit field value for use in a register bit range.
  \param[in] field  Name of the register bit field.
  \param[in] value  Value of the bit field.
  \return           Masked and shifted value.
*/
#define _VAL2FLD(field, value)    ((value << field ## _Pos) & field ## _Msk)

/**
  \brief     Mask and shift a register value to extract a bit filed value.
  \param[in] field  Name of the register bit field.
  \param[in] value  Value of register.
  \return           Masked and shifted bit field value.
*/
#define _FLD2VAL(field, value)    ((value & field ## _Msk) >> field ## _Pos)

/*@} end of group CSI_core_bitfield */


/**
  \ingroup    CSI_tcm_register
  \defgroup   CSI_TCM
  \brief      Type definitions for the tcm Registers
  @{
 */

/**
  \brief  Consortium definition for accessing protection area selection register(ITCMCR,CR<22,1>).
 */
typedef union {
    struct {
        uint32_t EN: 1;                             /*!< bit:     0  Instruction Tightly-Coupled Memory enable */
        uint32_t _reserved0: 1;                     /*!< bit:     1  Reserved */
        uint32_t SIF: 1;                            /*!< bit:     2  Slave interface access ITCM */
        uint32_t _reserved1: 1;                     /*!< bit:     3  Reserved */
        uint32_t Size: 4;                           /*!< bit:  4..7  Size of ITCM */
        uint32_t _reserved2: 1;                     /*!< bit:     8  Reserved */
        uint32_t Delay: 1;                          /*!< bit:     9  The time from access ITCM to data come back. */
        uint32_t _reserved3: 2;                     /*!< bit: 10..11 Reserved */
        uint32_t Base_Address: 20;                  /*!< bit: 12..31 Base address of DTCM */
    } b;                                            /*!< Structure    Access by bit */
    uint32_t w;                                     /*!< Type         Access by whole register */
} ITCMCR_Type;

#define ITCMCR_Base_Address_Pos             12U                                              /*!< ITCMCR: Base_Address Position */
#define ITCMCR_Base_Address_Msk             (0xfffffUL << ITCMCR_Base_Address_Pos)           /*!< ITCMCR: Base_Address Mask */

#define ITCMCR_Delay_Pos                    9U                                               /*!< ITCMCR: Delay Position */
#define ITCMCR_Delay_Msk                    (0x1UL << ITCMCR_Delay_Pos)                      /*!< ITCMCR: Delay Mask */

#define ITCMCR_Size_Pos                     4U                                               /*!< ITCMCR: Size Position */
#define ITCMCR_Size_Msk                     (0xfUL << ITCMCR_Size_Pos)                       /*!< ITCMCR: Size Mask */

#define ITCMCR_SIF_Pos                      2U                                               /*!< ITCMCR: SIF Position */
#define ITCMCR_SIF_Msk                      (0x1UL << ITCMCR_SIF_Pos)                        /*!< ITCMCR: SIF Mask */

#define ITCMCR_EN_Pos                       0U                                               /*!< ITCMCR: EN Position */
#define ITCMCR_EN_Msk                       (0x1UL << ITCMCR_EN_Pos)                         /*!< ITCMCR: EN Mask */

/**
  \brief  Consortium definition for accessing protection area selection register(DTCMCR,CR<23,1>).
 */
typedef union {
    struct {
        uint32_t EN: 1;                             /*!< bit:     0  Data Tightly-Coupled Memory enable */
        uint32_t _reserved0: 1;                     /*!< bit:     1  Reserved */
        uint32_t SIF: 1;                            /*!< bit:     2  Slave interface access DTCM */
        uint32_t _reserved1: 1;                     /*!< bit:     3  Reserved */
        uint32_t Size: 4;                           /*!< bit:  4..7  Size of DTCM */
        uint32_t _reserved2: 1;                     /*!< bit:     8  Reserved */
        uint32_t Delay: 1;                          /*!< bit:     9  The time from access DTCM to data come back.*/
        uint32_t _reserved3: 2;                     /*!< bit: 10..11 Reserved */
        uint32_t Base_Address: 20;                  /*!< bit: 12..31 Base address of DTCM */
    } b;                                            /*!< Structure    Access by bit */
    uint32_t w;                                     /*!< Type         Access by whole register */
} DTCMCR_Type;

#define DTCMCR_Base_Address_Pos             12U                                              /*!< DTCMCR: Base_Address Position */
#define DTCMCR_Base_Address_Msk             (0xfffffUL << DTCMCR_Base_Address_Pos)           /*!< DTCMCR: Base_Address Mask */

#define DTCMCR_Delay_Pos                    9U                                               /*!< DTCMCR: Delay Position */
#define DTCMCR_Delay_Msk                    (0x1UL << DTCMCR_Delay_Pos)                      /*!< DTCMCR: Delay Mask */

#define DTCMCR_Size_Pos                     4U                                               /*!< DTCMCR: Size Position */
#define DTCMCR_Size_Msk                     (0xfUL << DTCMCR_Size_Pos)                       /*!< DTCMCR: Size Mask */

#define DTCMCR_SIF_Pos                      2U                                               /*!< DTCMCR: SIF Position */
#define DTCMCR_SIF_Msk                      (0x1UL << DTCMCR_SIF_Pos)                        /*!< DTCMCR: SIF Mask */

#define DTCMCR_EN_Pos                       0U                                               /*!< DTCMCR: EN Position */
#define DTCMCR_EN_Msk                       (0x1UL << DTCMCR_EN_Pos)                         /*!< DTCMCR: EN Mask */

/*@} end of group CSI_TCM_bitfield */

/**
  \ingroup    CSI_ECC_register
  \defgroup   CSI_ECC
  \brief      Type definitions for the ECC Registers
  @{
 */

typedef union {
    struct {
        uint32_t Index: 17;                         /*!< bit:  0..16 RAM index */
        uint32_t _reserved0: 1;                     /*!< bit:  2..23 Reserved */
        uint32_t Way: 2;                            /*!< bit: 18..19 ICACHE/DCACHE WAY */
        uint32_t _reserved1: 2;                     /*!< bit: 20..21 Reserved */
        uint32_t RAMID: 8;                          /*!< bit: 22..29 ECC RAM ID */
        uint32_t ERR_TYPE: 2;                       /*!< bit: 30..31 ECC ERR TYPE */
    } b;
    uint32_t w;
} ERRLC_Type;

#define ERRLC_ERR_TYPE_Pos                 30U                                               /*!< ERRLC: ERR_TYPE Position */
#define ERRLC_ERR_TYPE_Msk                 (0x3UL << ERRLC_ERR_TYPE_Pos)                     /*!< ERRLC: ERR_TYPE Mask */

#define ERRLC_RAMID_Pos                    22U                                               /*!< ERRLC: RAMID Position */
#define ERRLC_RAMID_Msk                    (0xFFUL << ERRLC_RAMID_Pos)                       /*!< ERRLC: RAMID Mask */

#define ERRLC_Way_Pos                      18U                                               /*!< ERRLC: Way Position */
#define ERRLC_Way_Msk                      (0x3UL << ERRLC_Way_Pos)                          /*!< ERRLC: Way Mask */

#define ERRLC_Index_Pos                    0U                                                /*!< ERRLC: Index Position */
#define ERRLC_Index_Msk                    (0x1FFFFUL << ERRLC_Index_Pos)                    /*!< ERRLC: Index Mask */

typedef union {
    struct {
        uint32_t INJ_EN: 1;                         /*!< bit:     0  ECC inject enable */
        uint32_t ERR_TYPE: 1;                       /*!< bit:     1  ECC error type */
        uint32_t _reserved0: 22;                    /*!< bit:  2..23 Reserved */
        uint32_t RAMID: 8;                          /*!< bit: 24..31 ECC RAMID */
    } b;                                            /*!< Structure    Access by bit */
    uint32_t w;                                     /*!< Type         Access by whole register */
} ERRINJCR_Type;

#define ERRINJCR_RAMID_Pos                 24U                                               /*!< ERRINJCR: RAMID Position */
#define ERRINJCR_RAMID_Msk                 (0xFFUL << ERRINJCR_RAMID_Pos)                    /*!< ERRINJCR: RAMID Mask */

#define ERRINJCR_ERR_TYPE_Pos              1U                                                /*!< ERRINJCR: ERR_TYPE Position */
#define ERRINJCR_ERR_TYPE_Msk              (0x1UL << ERRINJCR_ERR_TYPE_Pos)                  /*!< ERRINJCR: ERR_TYPE Mask */

#define ERRINJCR_INJ_EN_Pos                0U                                                /*!< ERRINJCR: INJ_EN Position */
#define ERRINJCR_INJ_EN_Msk                (0x1UL << ERRINJCR_INJ_EN_Pos)                    /*!< ERRINJCR: INJ_EN Mask */

/*@} end of group CSI_ECC_bitfield */


/*******************************************************************************
 *                Hardware Abstraction Layer
  Core Function Interface contains:
  - Core VIC Functions
  - Core CORET Functions
  - Core Register Access Functions
 ******************************************************************************/

/* The following MACROS handle generation of the register offset and byte masks */
#define _BIT_SHIFT(IRQn)         (  ((((uint32_t)(int32_t)(IRQn))         )      &  0x03UL) * 8UL)
#define _IP_IDX(IRQn)            (   (((uint32_t)(int32_t)(IRQn))                >>    2UL)      )


/* ##########################  Cache functions  #################################### */
/**
  \ingroup  CSI_Core_FunctionInterface
  \defgroup CSI_Core_CacheFunctions Cache Functions
  \brief    Functions that configure Instruction and Data cache.
  @{
 */

/**
  \brief   Enable I-Cache
  \details Turns on I-Cache
  */
__STATIC_INLINE void csi_icache_enable (void)
{
   __set_CCR(__get_CCR() | 0x00000004);
}


/**
  \brief   Disable I-Cache
  \details Turns off I-Cache
  */
__STATIC_INLINE void csi_icache_disable (void)
{
   __set_CCR(__get_CCR() & 0xFFFFFFFB);
}


/**
  \brief   Invalidate I-Cache
  \details Invalidates I-Cache
  */
__STATIC_INLINE void csi_icache_invalid (void)
{
    __set_CFR(0x11);
    __set_CFR(INS_CACHE | CACHE_INV);
}


/**
  \brief   Enable D-Cache
  \details Turns on D-Cache
  \note    I-Cache also turns on.
  */
__STATIC_INLINE void csi_dcache_enable (void)
{
   __set_CCR(__get_CCR() | 0x00000008);
}


/**
  \brief   Disable D-Cache
  \details Turns off D-Cache
  \note    I-Cache also turns off.
  */
__STATIC_INLINE void csi_dcache_disable (void)
{
   __set_CCR(__get_CCR() & 0xFFFFFFF7);
}


/**
  \brief   Invalidate D-Cache
  \details Invalidates D-Cache
  \note    I-Cache also invalid
  */
__STATIC_INLINE void csi_dcache_invalid (void)
{
    __set_CFR(DATA_CACHE | CACHE_INV);
}


/**
  \brief   Clean D-Cache
  \details Cleans D-Cache
  \note    I-Cache also cleans
  */
__STATIC_INLINE void csi_dcache_clean (void)
{
    __set_CFR(DATA_CACHE | CACHE_CLR);
}

/**
  \brief   Clean & Invalidate D-Cache
  \details Cleans and Invalidates D-Cache
  \note    I-Cache also flush.
  */
__STATIC_INLINE void csi_dcache_clean_invalid (void)
{
    __set_CFR(DATA_CACHE | CACHE_CLR | CACHE_INV);
}

__STATIC_INLINE  void set_cache_range (uint32_t start, uint32_t end, uint32_t value)
{
    if (!(start & SSEG0_BASE_ADDR) || (end - start) &~(CACHE_RANGE_MAX_SIZE - 1)) {
        __set_CFR(value);
    }

    if (value & INS_CACHE) {
        csi_icache_disable();
    }
    uint32_t i;
    for (i = start; i < end; i += L1_CACHE_BYTES) {
        __set_CIR(i);
        __set_CFR(CACHE_OMS | value);
    }

    if (end & (L1_CACHE_BYTES-1)) {
        __set_CIR(end);
        __set_CFR(CACHE_OMS | value);
    }

    if (value & INS_CACHE) {
        csi_icache_enable();
    }

}

/**
  \brief   D-Cache Invalidate by address
  \details Invalidates D-Cache for the given address
  \param[in]   addr    address (aligned to 16-byte boundary)
  \param[in]   dsize   size of memory block (aligned to 16-byte boundary)
*/
__STATIC_INLINE void csi_dcache_invalid_range (uint32_t *addr, int32_t dsize)
{
    set_cache_range((uint32_t)addr, (uint32_t)addr + dsize, (DATA_CACHE | CACHE_INV));
}


/**
  \brief   D-Cache Clean by address
  \details Cleans D-Cache for the given address
  \param[in]   addr    address (aligned to 16-byte boundary)
  \param[in]   dsize   size of memory block (aligned to 16-byte boundary)
*/
__STATIC_INLINE void csi_dcache_clean_range (uint32_t *addr, int32_t dsize)
{
    set_cache_range((uint32_t)addr, (uint32_t)addr + dsize, (DATA_CACHE | CACHE_CLR));
}


/**
  \brief   D-Cache Clean and Invalidate by address
  \details Cleans and invalidates D_Cache for the given address
  \param[in]   addr    address (aligned to 16-byte boundary)
  \param[in]   dsize   size of memory block (aligned to 16-byte boundary)
*/
__STATIC_INLINE void csi_dcache_clean_invalid_range (uint32_t *addr, int32_t dsize)
{
    set_cache_range((uint32_t)addr, (uint32_t)addr + dsize, (DATA_CACHE | CACHE_CLR | CACHE_INV));
}


/*@} end of CSI_Core_CacheFunctions */


/* ##########################  MMU functions  #################################### */
/**
  \ingroup  CSI_Core_FunctionInterface
  \defgroup CSI_Core_MMUFunctions MMU Functions
  \brief    Functions that configure MMU.
  @{
 */

typedef struct {
    uint32_t global: 1;         /* tlb page global access. */
    uint32_t valid: 1;          /* tlb page valid */
    uint32_t writeable: 1;      /* tlb page writeable */
    uint32_t cacheable: 1;      /* tlb page cacheable*/
    uint32_t is_secure: 1;      /* tlb page security access */
    uint32_t strong_order: 1;   /* the sequence of accessing data on tlb page is corresponding to the program flow? */
    uint32_t bufferable: 1;     /* tlb page bufferable */
} page_attr_t;

typedef enum {
    PAGE_SIZE_4KB   = 0x000,
    PAGE_SIZE_16KB  = 0x003,
    PAGE_SIZE_64KB  = 0x00F,
    PAGE_SIZE_256KB = 0x03F,
    PAGE_SIZE_1MB   = 0x0FF,
    PAGE_SIZE_4MB   = 0x3FF,
    PAGE_SIZE_16MB  = 0xFFF
} page_size_e;


/**
  \brief  enable mmu
  \details
  */
__STATIC_INLINE void csi_mmu_enable(void)
{
    __set_CCR(__get_CCR() | (1u << CCR_MP_Pos));
}

/**
  \brief  disable mmu
  \details
  */
__STATIC_INLINE void csi_mmu_disable(void)
{
    __set_CCR(__get_CCR() & (~(1u << CCR_MP_Pos)));
}

/**
  \brief  create page with feature.
  \details
  \param [in]  vaddr     virtual address.
  \param [in]  paddr     physical address.
  \param [in]  asid      address sapce id (default: 0).
  \param [in]  attr      \ref page_attr_t. tlb page attribute.
  */
__STATIC_INLINE void csi_mmu_set_tlb(uint32_t vaddr, uint32_t paddr, uint32_t asid, page_attr_t attr)
{
     MPR_Type pgmask;
     MEH_Type meh;
     MEL_Type mel;
     uint32_t vaddr_bit;
     uint32_t page_feature = 0;

     page_feature |= attr.global << MEL_G_Pos | attr.valid << MEL_V_Pos |
                     attr.writeable << MEL_D_Pos | attr.cacheable << MEL_C_Pos |
                     attr.is_secure << MEL_SEC_Pos | attr.strong_order << MEL_SO_Pos |
                     attr.bufferable << MEL_B_Pos;

     pgmask.w = __get_MPR();
     vaddr_bit = 44 - __FF0(~((uint32_t)pgmask.b.page_mask));

     meh.b.ASID = (uint8_t)asid;
     meh.b.VPN  = (vaddr & ((~pgmask.w | 0xFE000000) & 0xFFFFE000)) >> MEH_VPN_Pos;
     __set_MEH(meh.w);

     __set_MCIR(1u << MCIR_TLBP_Pos);

     mel.w = ((paddr & ~(pgmask.b.page_mask << 12)) | page_feature);
     if (vaddr & (1 << vaddr_bit)) {
         __set_MEL1(mel.w);
     }
     else {
         __set_MEL0(mel.w);
     }

     if (__get_MIR() & (1 << MIR_P_Pos)) {
        __set_MCIR(1u << MCIR_TLBWR_Pos);
     } else {
        __set_MCIR(1u << MCIR_TLBWI_Pos);
     }
}


/**
  \brief  enble mmu
  \details
  \param [in]  size  tlb page size.
  */
__STATIC_INLINE void csi_mmu_set_pagesize(page_size_e size)
{
    MPR_Type pgmask;
    pgmask.b.page_mask = size;
    __set_MPR(pgmask.w);
}


/**
  \brief  read MEH, MEL0, MEL1 by tlb index.
  \details
  \param [in]    index  tlb index(0, 1, 2, ...)
  \param [out]   meh    pointer to variable for retrieving MEH.
  \param [out]   mel0   pointer to variable for retrieving MEL0.
  \param [out]   mel1   pointer to variable for retrieving MEL1.
  */
__STATIC_INLINE void csi_mmu_read_by_index(uint32_t index, uint32_t *meh, uint32_t *mel0, uint32_t *mel1)
{
    MIR_Type mir;

    if (meh == NULL || mel0 == NULL || mel1 == NULL) {
        return;
    }

    mir.b.Index = index;
    __set_MIR(mir.w);
    __set_MCIR(1u << MCIR_TLBR_Pos);

    *meh = __get_MEH();
    *mel0 = __get_MEL0();
    *mel1 = __get_MEL1();
}


/**
  \brief  flush all mmu tlb.
  \details
  */
__STATIC_INLINE void csi_mmu_invalid_tlb_all(void)
{
    __set_MCIR(1u << MCIR_TLBINV_ALL_Pos);
}

/**
  \brief  flush mmu tlb by index.
  \details
  */
__STATIC_INLINE void csi_mmu_invalid_tlb_by_index(uint32_t index)
{
    MIR_Type mir;

    mir.b.Index = index;
    __set_MIR(mir.w);
    __set_MCIR(1u << MCIR_TLBINV_INDEX_Pos);
}


/**
  \brief  flush mmu tlb by virtual address.
  \details
  */
__STATIC_INLINE void csi_mmu_invalid_tlb_by_vaddr(uint32_t vaddr, uint32_t asid)
{
    __set_MEH(vaddr | (asid & MEH_ASID_Msk));
    __set_MCIR(1u << MCIR_TLBP_Pos);

    if (__get_MIR() & (1 << MIR_P_Pos)) {
        return;
    } else {
        __set_MCIR(1u << MCIR_TLBINV_INDEX_Pos);
    }
}

/*@} end of CSI_Core_MMUFunctions */

/* ##########################  MPU functions  #################################### */
/**
  \ingroup  CSI_Core_FunctionInterface
  \defgroup CSI_Core_MPUFunctions MPU Functions
  \brief    Functions that configure MPU.
  @{
 */

typedef enum {
    REGION_SIZE_128B     = 0x6,
    REGION_SIZE_256B     = 0x7,
    REGION_SIZE_512B     = 0x8,
    REGION_SIZE_1KB      = 0x9,
    REGION_SIZE_2KB      = 0xA,
    REGION_SIZE_4KB      = 0xB,
    REGION_SIZE_8KB      = 0xC,
    REGION_SIZE_16KB     = 0xD,
    REGION_SIZE_32KB     = 0xE,
    REGION_SIZE_64KB     = 0xF,
    REGION_SIZE_128KB    = 0x10,
    REGION_SIZE_256KB    = 0x11,
    REGION_SIZE_512KB    = 0x12,
    REGION_SIZE_1MB      = 0x13,
    REGION_SIZE_2MB      = 0x14,
    REGION_SIZE_4MB      = 0x15,
    REGION_SIZE_8MB      = 0x16,
    REGION_SIZE_16MB     = 0x17,
    REGION_SIZE_32MB     = 0x18,
    REGION_SIZE_64MB     = 0x19,
    REGION_SIZE_128MB    = 0x1A,
    REGION_SIZE_256MB    = 0x1B,
    REGION_SIZE_512MB    = 0x1C,
    REGION_SIZE_1GB      = 0x1D,
    REGION_SIZE_2GB      = 0x1E,
    REGION_SIZE_4GB      = 0x1F
} region_size_e;

typedef enum {
    AP_BOTH_INACCESSIBLE = 0,
    AP_SUPER_RW_USER_INACCESSIBLE,
    AP_SUPER_RW_USER_RDONLY,
    AP_BOTH_RW
} access_permission_e;

typedef struct {
    uint32_t nx: 1;
    access_permission_e ap: 2;    /* super user and normal user access.*/
    uint32_t s: 1;
    uint32_t c: 1;                /* cacheable */
    uint32_t so: 1;
    uint32_t b: 1;
} mpu_region_attr_t;

/**
  \brief  enable mpu
  \details
  */
__STATIC_INLINE void csi_mpu_enable(void)
{
    __set_CCR(__get_CCR() | CCR_MP_Msk);
}

/**
  \brief  disable mpu
  \details
  */
__STATIC_INLINE void csi_mpu_disable(void)
{
    __set_CCR(__get_CCR() & (~CCR_MP_Msk));
}

/**
  \brief  configure memory protected region.
  \details
  \param [in]  idx        memory protected region (0, 1, 2, 3.).
  \param [in]  base_addr  base address must be aligned with page size.
  \param [in]  size       \ref region_size_e. memory protected region size.
  \param [in]  attr       \ref region_size_t. memory protected region attribute.
  \param [in]  enable     enable or disable memory protected region.
  */
__STATIC_INLINE void csi_mpu_config_region(uint32_t idx, uint32_t base_addr, region_size_e size,
                                           mpu_region_attr_t attr, uint32_t enable)
{
    uint32_t op_idx;

    if (idx > 15) {
        return;
    }

    CAPR_Type capr;
    PACR_Type pacr;
    PRSR_Type prsr;
    ATTR0_Type reg_attr;

    if (idx <= 7) {
        capr.w = __get_CAPR();
        reg_attr.w = __get_ATTR0();
        op_idx = idx;
    } else {
        capr.w = __get_CAPR1();
        reg_attr.w = __get_ATTR1();
        op_idx = idx - 8;
    }

    pacr.w = __get_PACR();
    prsr.w = __get_PRSR();

    pacr.w = (base_addr & PACR_BASE_ADDR_Msk) | (size << PACR_SIZE_Pos);
    pacr.w &= ~(((1u << (size - 6)) - 1) << 7);
    pacr.b.E = enable;

    prsr.b.RID = idx;
    __set_PRSR(prsr.w);

    capr.w &= ~((0x1 << op_idx) | (0x3 << (op_idx * 2 + 8)) | (0x1 << (op_idx + 24)));
    capr.w = (capr.w | (attr.nx << op_idx) | (attr.ap << (op_idx * 2 + 8)) | (attr.s << (op_idx + 24)));

    reg_attr.w &= ~(0x7 << (op_idx << 2));
    reg_attr.w |= ((attr.c | (attr.so << 1) | (attr.b << 2)) << (op_idx << 2));

    if (idx <= 7) {
        __set_CAPR(capr.w);
        __set_ATTR0(reg_attr.w);
    } else {
        __set_CAPR1(capr.w);
        __set_ATTR1(reg_attr.w);
    }

    __set_PACR(pacr.w);
}

/**
  \brief  enable mpu region by idx.
  \details
  \param [in]  idx        memory protected region (0, 1, 2, 3.).
  */
__STATIC_INLINE void csi_mpu_enable_region(uint32_t idx)
{
    if (idx > 15) {
        return;
    }

    __set_PRSR((__get_PRSR() & (~PRSR_RID_Msk)) | idx);
    __set_PACR(__get_PACR() | PACR_E_Msk);
}

/**
  \brief  disable mpu region by idx.
  \details
  \param [in]  idx        memory protected region (0, 1, 2, 3.).
  */
__STATIC_INLINE void csi_mpu_disable_region(uint32_t idx)
{
    if (idx > 15) {
        return;
    }

    __set_PRSR((__get_PRSR() & (~PRSR_RID_Msk)) | idx);
    __set_PACR(__get_PACR() & (~PACR_E_Msk));
}

/*@} end of CSI_Core_MPUFunctions */

/* ##########################  TCM functions  #################################### */
/**
  \ingroup  CSI_Core_FunctionInterface
  \defgroup CSI_Core_TCMFunctions TCM Functions
  \brief    Functions that configure TCM.
  @{
 */

/**
  \brief   Enable ITCM
  \details Turns on ITCM
  */
__STATIC_INLINE void csi_itcm_enable (void)
{
    __set_ITCMCR(__get_ITCMCR() | ITCMCR_EN_Msk);
}

/**
  \brief   Enable DTCM
  \details Turns on DTCM
  */
__STATIC_INLINE void csi_dtcm_enable (void)
{
    __set_DTCMCR(__get_DTCMCR() | DTCMCR_EN_Msk);
}

/**
  \brief   Enable ITCM
  \details Turns on ITCM
  */
__STATIC_INLINE void csi_itcm_disable (void)
{
    __set_ITCMCR(__get_ITCMCR() & (~ITCMCR_EN_Msk));
}

/**
  \brief   Enable DTCM
  \details Turns on DTCM
  */
__STATIC_INLINE void csi_dtcm_disable (void)
{
    __set_DTCMCR(__get_DTCMCR() & (~DTCMCR_EN_Msk));
}

/**
  \brief   Enable ITCM slave interface access
  \details Enable ITCM slave interface access
  */
__STATIC_INLINE void csi_itcm_enable_slave_access(void)
{
    __set_ITCMCR(__get_ITCMCR() | ITCMCR_SIF_Msk);
}

/**
  \brief   Disable ITCM slave interface access
  \details Disable ITCM slave interface access
  */
__STATIC_INLINE void csi_itcm_disable_slave_access(void)
{
    __set_ITCMCR(__get_ITCMCR() & (~ITCMCR_SIF_Msk));
}

/**
  \brief   Enable DTCM slave interface access
  \details Enable DTCM slave interface access
  */
__STATIC_INLINE void csi_dtcm_enable_slave_access(void)
{
    __set_DTCMCR(__get_DTCMCR() | DTCMCR_SIF_Msk);
}

/**
  \brief   Disable DTCM slave interface access
  \details Disable DTCM slave interface access
  */
__STATIC_INLINE void csi_dtcm_disable_slave_access(void)
{
    __set_DTCMCR(__get_DTCMCR() & (~DTCMCR_SIF_Msk));
}

/**
  \brief   Get ITCM Size
  \details Get ITCM Size
  \return         ITCM size (bytes).
  */
__STATIC_INLINE uint32_t csi_itcm_get_size(void)
{
    ITCMCR_Type sizemask;
    uint32_t ret;

    sizemask.w = __get_ITCMCR();
    ret = sizemask.b.Size;

    return (1 << ret) << 10;
}

/**
  \brief   Get DTCM Size
  \details Get DTCM Size
  \return         DTCM size (bytes).
  */
__STATIC_INLINE uint32_t csi_dtcm_get_size(void)
{
    DTCMCR_Type sizemask;
    uint32_t ret;

    sizemask.w = __get_DTCMCR();
    ret = sizemask.b.Size;

    return (1 << ret) << 10;
}

/**
  \brief   Get ITCM Delay
  \details Get ITCM Delay
  \return         delay time.
  */
__STATIC_INLINE uint32_t csi_itcm_get_delay(void)
{
    ITCMCR_Type delaymask;
    uint32_t ret;

    delaymask.w = __get_ITCMCR();
    ret = delaymask.b.Delay;

    return ret;
}

/**
  \brief   Get DTCM Delay
  \details Get DTCM Delay
  \return         delay time.
  */
__STATIC_INLINE uint32_t csi_dtcm_get_delay(void)
{
    DTCMCR_Type delaymask;
    uint32_t ret;

    delaymask.w = __get_DTCMCR();
    ret = delaymask.b.Delay;

    return ret;
}

/**
  \brief   Set ITCM Base Address
  \details Set ITCM Base Address
  \param [in]  base_addr  itcm base address.
  */
__STATIC_INLINE void csi_itcm_set_base_addr(uint32_t base_addr)
{
    __set_ITCMCR((__get_ITCMCR() & (~ITCMCR_Base_Address_Msk)) | (base_addr & ITCMCR_Base_Address_Msk));
}

/**
  \brief   Set DTCM Base Address
  \details Set DTCM Base Address
  \param [in]  base_addr  dtcm base address.
  */
__STATIC_INLINE void csi_dtcm_set_base_addr(uint32_t base_addr)
{
    __set_DTCMCR((__get_DTCMCR() & (~DTCMCR_Base_Address_Msk)) | (base_addr & DTCMCR_Base_Address_Msk));
}


/*@} end of CSI_Core_TCMFunctions */

/* ##########################  ECC functions  #################################### */
/**
  \ingroup  CSI_Core_FunctionInterface
  \defgroup CSI_Core_ECCFunctions ECC Functions
  \brief    Functions that configure ECC.
  @{
 */

typedef enum {
    ECC_ERROR_CORRECTABLE = 0,
    ECC_ERROR_FATAL = 1
} ecc_error_type_e;

typedef enum {
    ECC_DTCM_RAM = 0,
    ECC_ITCM_RAM = 1,
    ECC_DCACHE_DATA_RAM = 4,
    ECC_DCACHE_TAG_RAM = 5,
    ECC_ICACHE_DATA_RAM = 6,
    ECC_ICACHE_TAG_RAM = 7
} ecc_ramid_e;

typedef struct {
    uint32_t erraddr;
    uint32_t index;
    uint8_t way;
    ecc_ramid_e ramid: 8;
    ecc_error_type_e err_type: 8;
} ecc_error_info_t;

/**
  \brief   Enable ECC
  \details Turns on ECC
  */
__STATIC_INLINE void csi_ecc_enable (void)
{
    __set_CCR2(__get_CCR2() | CCR2_ECC_Msk);
}

/**
  \brief   Disable ECC
  \details Turns off ECC
  */
__STATIC_INLINE void csi_ecc_disable (void)
{
    __set_CCR2(__get_CCR2() & ~CCR2_ECC_Msk);
}

/**
  \brief   Enable ECC error fix function
  \details Turns on ECC error fix function
  */
__STATIC_INLINE void csi_ecc_enable_error_fix (void)
{
    __set_CCR2(__get_CCR2() | CCR2_CRECC_Msk);
}

/**
  \brief   Disable ECC error fix function
  \details Turns off ECC error fix function
  */
__STATIC_INLINE void csi_ecc_disable_error_fix (void)
{
    __set_CCR2(__get_CCR2() & ~CCR2_CRECC_Msk);
}

/**
  \brief   Inject ECC error
  \details Inject ECC error
  \param [in]      type ECC error type.
  \param [in]      ramid ECC ram id.
*/
__STATIC_INLINE void csi_ecc_inject_error(ecc_error_type_e type, ecc_ramid_e ramid)
{
    ERRINJCR_Type errinjcr;

    errinjcr.b.ERR_TYPE = type;
    errinjcr.b.RAMID = (1U << ramid);
    errinjcr.b.INJ_EN = 1U;
    errinjcr.b._reserved0 = 0;

    __set_ERRINJCR(errinjcr.w);
}

/**
  \brief   Get ECC error info
  \details Inject ECC error info
  \param [out]      info ECC error info
*/
__STATIC_INLINE void csi_ecc_get_error_info(ecc_error_info_t *info)
{
    ERRLC_Type errlc;

    if (info != NULL) {
        errlc.w = __get_ERRLC();
        info->erraddr = __get_ERRADDR();
        info->index = errlc.b.Index;
        info->way = errlc.b.Way;
        info->ramid = 31 - __FF1(errlc.b.RAMID);
        info->err_type = errlc.b.ERR_TYPE;
    }
}

/*@} end of CSI_Core_ECCFunctions */

/* ##################################    IRQ Functions  ############################################ */

/**
  \brief   Save the Irq context
  \details save the psr result before disable irq.
  \param [in]      irq_num  External interrupt number. Value cannot be negative.
 */
__STATIC_INLINE uint32_t csi_irq_save(void)
{
    uint32_t result;
    result = __get_PSR();
    __disable_irq();
    return(result);
}

/**
  \brief   Restore the Irq context
  \details restore saved primask state.
  \param [in]      irq_state  psr irq state.
 */
__STATIC_INLINE void csi_irq_restore(uint32_t irq_state)
{
    __set_PSR(irq_state);
}

/*@} end of IRQ Functions */

#ifdef __cplusplus
}
#endif

#endif /* __CORE_CK807_H_DEPENDANT */

#endif /* __CSI_GENERIC */
