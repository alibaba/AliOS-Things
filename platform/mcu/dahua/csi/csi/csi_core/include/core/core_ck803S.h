/*
 * Copyright (C) 2017 C-SKY Microsystems Co., Ltd. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/******************************************************************************
 * @file     core_ck803S.h
 * @brief    CSI CK803S Core Peripheral Access Layer Header File
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/

#ifndef __CORE_CK803S_H_GENERIC
#define __CORE_CK803S_H_GENERIC

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 *                 CSI definitions
 ******************************************************************************/
/**
  \ingroup Ck803S
  @{
 */

/*  CSI CK803S definitions */
#define __CK803S_CSI_VERSION_MAIN  (0x04U)                                      /*!< [31:16] CSI HAL main version */
#define __CK803S_CSI_VERSION_SUB   (0x1EU)                                      /*!< [15:0]  CSI HAL sub version */
#define __CK803S_CSI_VERSION       ((__CK803S_CSI_VERSION_MAIN << 16U) | \
                                   __CK803S_CSI_VERSION_SUB           )        /*!< CSI HAL version number */

#ifndef __CK80X
#define __CK80X                (0x03U)                                         /*!< CK80X Core */
#endif

/** __FPU_USED indicates whether an FPU is used or not.
    This core does not support an FPU at all
*/
#define __FPU_USED       0U

#if defined ( __GNUC__ )
#if defined (__VFP_FP__) && !defined(__SOFTFP__)
#error "Compiler generates FPU instructions for a device without an FPU (check __FPU_PRESENT)"
#endif
#endif

#include "csi_gcc/csi_gcc.h"

#ifdef __cplusplus
}
#endif

#endif /* __CORE_CK803S_H_GENERIC */

#ifndef __CSI_GENERIC

#ifndef __CORE_CK803S_H_DEPENDANT
#define __CORE_CK803S_H_DEPENDANT

#ifdef __cplusplus
extern "C" {
#endif

/* check device defines and use defaults */
//#if defined __CHECK_DEVICE_DEFINES
#ifndef __CK803S_REV
#define __CK803S_REV               0x0000U
//#warning "__CK803S_REV not defined in device header file; using default!"
#endif

#ifndef __VIC_PRIO_BITS
#define __VIC_PRIO_BITS          2U
//#warning "__VIC_PRIO_BITS not defined in device header file; using default!"
#endif

#ifndef __Vendor_SysTickConfig
#define __Vendor_SysTickConfig    0U
//#warning "__Vendor_SysTickConfig not defined in device header file; using default!"
#endif

#ifndef __GSR_GCR_PRESENT
#define __GSR_GCR_PRESENT         0U
//#warning "__GSR_GCR_PRESENT not defined in device header file; using default!"
#endif

#ifndef __MGU_PRESENT
#define __MGU_PRESENT             0U
//#warning "__MGU_PRESENT not defined in device header file; using default!"
#endif

#ifndef __ICACHE_PRESENT
#define __ICACHE_PRESENT          0U
//#warning "__ICACHE_PRESENT not defined in device header file; using default!"
#endif

#ifndef __DCACHE_PRESENT
#define __DCACHE_PRESENT          0U
//#warning "__DCACHE_PRESENT not defined in device header file; using default!"
#endif

//#endif

/* IO definitions (access restrictions to peripheral registers) */
/**
    \defgroup CSI_glob_defs CSI Global Defines

    <strong>IO Type Qualifiers</strong> are used
    \li to specify the access to peripheral variables.
    \li for automatic generation of peripheral register debug information.
*/
#ifdef __cplusplus
#define   __I     volatile             /*!< Defines 'read only' permissions */
#else
#define   __I     volatile const       /*!< Defines 'read only' permissions */
#endif
#define     __O     volatile             /*!< Defines 'write only' permissions */
#define     __IO    volatile             /*!< Defines 'read / write' permissions */

/* following defines should be used for structure members */
#define     __IM     volatile const      /*! Defines 'read only' structure member permissions */
#define     __OM     volatile            /*! Defines 'write only' structure member permissions */
#define     __IOM    volatile            /*! Defines 'read / write' structure member permissions */

/*@} end of group CK803S */

/*******************************************************************************
 *                 Register Abstraction
  Core Register contain:
  - Core Register
  - Core VIC Register
  - Core SCB Register
  - Core SysTick Register
 ******************************************************************************/
/**
  \defgroup CSI_core_register Defines and Type Definitions
  \brief Type definitions and defines for CK80X processor based devices.
*/

/**
  \ingroup    CSI_core_register
  \defgroup   CSI_CORE  Status and Control Registers
  \brief      Core Register type definitions.
  @{
 */

/**
  \brief  访问处理器状态寄存器(PSR)的联合体定义.
 */
typedef union {
    struct {
        uint32_t C: 1;                       /*!< bit:      0  条件码／进位位 */
        uint32_t _reserved0: 5;              /*!< bit:  2.. 5  保留 */
        uint32_t IE: 1;                      /*!< bit:      6  中断有效控制位 */
        uint32_t IC: 1;                      /*!< bit:      7  中断控制位 */
        uint32_t EE: 1;                      /*!< bit:      8  异常有效控制位 */
        uint32_t MM: 1;                      /*!< bit:      9  不对齐异常掩盖位 */
        uint32_t _reserved1: 6;              /*!< bit: 10..15  保留 */
        uint32_t VEC: 8;                     /*!< bit: 16..23  异常事件向量值 */
        uint32_t _reserved2: 7;              /*!< bit: 24..30  保留 */
        uint32_t S: 1;                       /*!< bit:     31  超级用户模式设置位 */
    } b;                                   /*!< Structure    用来按位访问 */
    uint32_t w;                            /*!< Type         整个寄存器访问 */
} PSR_Type;

/* PSR Register Definitions */
#define PSR_S_Pos                          31U                                            /*!< PSR: S Position */
#define PSR_S_Msk                          (1UL << PSR_S_Pos)                             /*!< PSR: S Mask */

#define PSR_VEC_Pos                        16U                                            /*!< PSR: VEC Position */
#define PSR_VEC_Msk                        (0x7FUL << PSR_VEC_Pos)                        /*!< PSR: VEC Mask */

#define PSR_MM_Pos                         9U                                             /*!< PSR: MM Position */
#define PSR_MM_Msk                         (1UL << PSR_MM_Pos)                            /*!< PSR: MM Mask */

#define PSR_EE_Pos                         8U                                             /*!< PSR: EE Position */
#define PSR_EE_Msk                         (1UL << PSR_EE_Pos)                            /*!< PSR: EE Mask */

#define PSR_IC_Pos                         7U                                             /*!< PSR: IC Position */
#define PSR_IC_Msk                         (1UL << PSR_IC_Pos)                            /*!< PSR: IC Mask */

#define PSR_IE_Pos                         6U                                             /*!< PSR: IE Position */
#define PSR_IE_Msk                         (1UL << PSR_IE_Pos)                            /*!< PSR: IE Mask */

#define PSR_C_Pos                          0U                                             /*!< PSR: C Position */
#define PSR_C_Msk                          (1UL << PSR_C_Pos)                             /*!< PSR: C Mask */

/**
  \brief  访问高速缓存配置寄存器(CCR, CR<18, 0>)的联合体定义.
 */
typedef union {
    struct {
        uint32_t MP: 1;                      /*!< bit:      0  内存保护设置位 */
        uint32_t _reserved0: 6;              /*!< bit:  1.. 6  保留 */
        uint32_t BE: 1;                      /*!< bit:      7  Endian模式 */
        uint32_t SCK: 3;                     /*!< bit:  8..10  系统和处理器的时钟比 */
        uint32_t _reserved1: 2;              /*!< bit: 11..12  保留 */
        uint32_t BE_V2: 1;                   /*!< bit:     13  V2版本大小端 */
        uint32_t _reserved2: 18;             /*!< bit: 14..31  保留 */
    } b;                                   /*!< Structure    用来按位访问 */
    uint32_t w;                            /*!< Type         整个寄存器访问 */
} CCR_Type;

/* CCR Register Definitions */
#define CCR_BE_V2_Pos                     13U                                            /*!< CCR: BE_V2 Position */
#define CCR_BE_V2_Msk                     (0x1UL << CCR_BE_V2_Pos)                         /*!< CCR: BE_V2 Mask */

#define CCR_SCK_Pos                       8U                                             /*!< CCR: SCK Position */
#define CCR_SCK_Msk                       (0x3UL << CCR_SCK_Pos)                         /*!< CCR: SCK Mask */

#define CCR_BE_Pos                        7U                                             /*!< CCR: BE Position */
#define CCR_BE_Msk                        (0x1UL << CCR_BE_Pos)                          /*!< CCR: BE Mask */

#define CCR_MP_Pos                        0U                                             /*!< CCR: MP Position */
#define CCR_MP_Msk                        (0x1UL << CCR_MP_Pos)                          /*!< CCR: MP Mask */

/**
  \brief  访问可高缓和访问权限配置寄存器(CAPR, CR<19,0>)的联合体定义..
 */
typedef union {
    struct {
        uint32_t X0: 1;                      /*!< bit:      0  不可执行属性设置位 */
        uint32_t X1: 1;                      /*!< bit:      1  不可执行属性设置位 */
        uint32_t X2: 1;                      /*!< bit:      2  不可执行属性设置位 */
        uint32_t X3: 1;                      /*!< bit:      3  不可执行属性设置位 */
        uint32_t X4: 1;                      /*!< bit:      4  不可执行属性设置位 */
        uint32_t X5: 1;                      /*!< bit:      5  不可执行属性设置位 */
        uint32_t X6: 1;                      /*!< bit:      6  不可执行属性设置位 */
        uint32_t X7: 1;                      /*!< bit:      7  不可执行属性设置位 */
        uint32_t AP0: 2;                     /*!< bit:  8.. 9  访问权限设置位 */
        uint32_t AP1: 2;                     /*!< bit: 10..11  访问权限设置位 */
        uint32_t AP2: 2;                     /*!< bit: 12..13  访问权限设置位 */
        uint32_t AP3: 2;                     /*!< bit: 14..15  访问权限设置位 */
        uint32_t AP4: 2;                     /*!< bit: 16..17  访问权限设置位 */
        uint32_t AP5: 2;                     /*!< bit: 18..19  访问权限设置位 */
        uint32_t AP6: 2;                     /*!< bit: 20..21  访问权限设置位 */
        uint32_t AP7: 2;                     /*!< bit: 22..23  访问权限设置位 */
        uint32_t S0: 1;                      /*!< bit:     24  安全属性设置位 */
        uint32_t S1: 1;                      /*!< bit:     25  安全属性设置位 */
        uint32_t S2: 1;                      /*!< bit:     26  安全属性设置位 */
        uint32_t S3: 1;                      /*!< bit:     27  安全属性设置位 */
        uint32_t S4: 1;                      /*!< bit:     28  安全属性设置位 */
        uint32_t S5: 1;                      /*!< bit:     29  安全属性设置位 */
        uint32_t S6: 1;                      /*!< bit:     30  安全属性设置位 */
        uint32_t S7: 1;                      /*!< bit:     31  安全属性设置位 */
    } b;                                   /*!< Structure    用来按位访问 */
    uint32_t w;                            /*!< Type         整个寄存器访问 */
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

#define CAPR_X7_Pos                        7U                                             /*!< CAPR: X7 Position */
#define CAPR_X7_Msk                        (0x1UL << CAPR_X7_Pos)                         /*!< CAPR: X7 Mask */

#define CAPR_X6_Pos                        6U                                             /*!< CAPR: X6 Position */
#define CAPR_X6_Msk                        (0x1UL << CAPR_X6_Pos)                         /*!< CAPR: X6 Mask */

#define CAPR_X5_Pos                        5U                                             /*!< CAPR: X5 Position */
#define CAPR_X5_Msk                        (0x1UL << CAPR_X5_Pos)                         /*!< CAPR: X5 Mask */

#define CAPR_X4_Pos                        4U                                             /*!< CAPR: X4 Position */
#define CAPR_X4_Msk                        (0x1UL << CAPR_X4_Pos)                         /*!< CAPR: X4 Mask */

#define CAPR_X3_Pos                        3U                                             /*!< CAPR: X3 Position */
#define CAPR_X3_Msk                        (0x1UL << CAPR_X3_Pos)                         /*!< CAPR: X3 Mask */

#define CAPR_X2_Pos                        2U                                             /*!< CAPR: X2 Position */
#define CAPR_X2_Msk                        (0x1UL << CAPR_X2_Pos)                         /*!< CAPR: X2 Mask */

#define CAPR_X1_Pos                        1U                                             /*!< CAPR: X1 Position */
#define CAPR_X1_Msk                        (0x1UL << CAPR_X1_Pos)                         /*!< CAPR: X1 Mask */

#define CAPR_X0_Pos                        0U                                             /*!< CAPR: X0 Position */
#define CAPR_X0_Msk                        (0x1UL << CAPR_X0_Pos)                         /*!< CAPR: X0 Mask */

/**
  \brief  访问保护区控制寄存器(PACR, CR<20,0>)的联合体定义.
 */
typedef union {
    struct {
        uint32_t E: 1;                       /*!< bit:      0  保护区有效设置 */
        uint32_t Size: 5;                    /*!< bit:  1.. 5  保护区大小 */
        uint32_t _reserved0: 4;              /*!< bit:  6.. 9  保留 */
        uint32_t base_addr: 22;              /*!< bit: 10..31  保护区地址的高位 */
    } b;                                     /*!< Structure    用来按位访问 */
    uint32_t w;                              /*!< Type         整个寄存器访问 */
} PACR_Type;

/* PACR Register Definitions */
#define PACR_BASE_ADDR_Pos                 10U                                            /*!< PACR: base_addr Position */
#define PACK_BASE_ADDR_Msk                 (0x3FFFFFUL << PACR_BASE_ADDR_Pos)             /*!< PACR: base_addr Mask */

#define PACR_SIZE_Pos                      1U                                             /*!< PACR: Size Position */
#define PACK_SIZE_Msk                      (0x1FUL << PACR_SIZE_Pos)                      /*!< PACR: Size Mask */

#define PACR_E_Pos                         0U                                             /*!< PACR: E Position */
#define PACK_E_Msk                         (0x1UL << PACR_E_Pos)                          /*!< PACR: E Mask */

/**
  \brief  访问保护区选择寄存器(PRSR,CR<21,0>)的联合体定义.
 */
typedef union {
    struct {
        uint32_t RID: 3;                     /*!< bit:  0.. 2  保护区索引值 */
        uint32_t _reserved0: 30;             /*!< bit:  3..31  保留 */
    } b;                                     /*!< Structure    用来按位访问 */
    uint32_t w;                              /*!< Type         整个寄存器访问 */
} PRSR_Type;

/* PRSR Register Definitions */
#define PRSR_RID_Pos                       0U                                            /*!< PRSR: RID Position */
#define PRSR_RID_Msk                       (0x7UL << PRSR_RID_Pos)                       /*!< PRSR: RID Mask */

/*@} end of group CSI_CORE */


/**
  \ingroup    CSI_core_register
  \defgroup   CSI_VIC Vectored Interrupt Controller (VIC)
  \brief      Type definitions for the VIC Registers
  @{
 */

/**
  \brief 访问矢量中断控制器的结构体.
 */
typedef struct {
    __IOM uint32_t ISER[1U];               /*!< Offset: 0x000 (R/W)  中断使能设置寄存器 */
    uint32_t RESERVED0[15U];
    __IOM uint32_t IWER[1U];               /*!< Offset: 0x040 (R/W)  中断低功耗唤醒设置寄存器 */
    uint32_t RESERVED1[15U];
    __IOM uint32_t ICER[1U];               /*!< Offset: 0x080 (R/W)  中断使能清除寄存器 */
    uint32_t RESERVED2[15U];
    __IOM uint32_t IWDR[1U];               /*!< Offset: 0x0c0 (R/W)  中断低功耗唤醒清除寄存器 */
    uint32_t RESERVED3[15U];
    __IOM uint32_t ISPR[1U];               /*!< Offset: 0x100 (R/W)  中断等待设置寄存器 */
    uint32_t RESERVED4[15U];
    __IOM uint32_t ISSR[1U];               /*!< Offset: 0x140 (R/W)  安全中断使能设置寄存器 */
    uint32_t RESERVED5[15U];
    __IOM uint32_t ICPR[1U];               /*!< Offset: 0x180 (R/W)  中断等待清除寄存器 */
    uint32_t RESERVED6[31U];
    __IOM uint32_t IABR[1U];               /*!< Offset: 0x200 (R/W)  中断响应状态寄存器 */
    uint32_t RESERVED7[63U];
    __IOM uint32_t IPR[8U];                /*!< Offset: 0x300 (R/W)  中断优先级设置寄存器 */
    uint32_t RESERVED8[504U];
    __IM  uint32_t ISR;                    /*!< Offset: 0xB00 (R/ )  中断状态寄存器 */
    __IOM uint32_t IPTR;                   /*!< Offset: 0xB04 (R/W)  中断优先级阈值寄存器 */
} VIC_Type;

/*@} end of group CSI_VIC */

/**
  \ingroup    CSI_core_register
  \defgroup   CSI_CACHE
  \brief      Type definitions for the cache Registers
  @{
 */

/**
  \brief 片上高速缓存的结构体.
 */
typedef struct
{
  __IOM uint32_t CER;                    /*!< Offset: 0x000 (R/W)  高速缓存使能寄存器 */
  __IOM uint32_t CIR;                    /*!< Offset: 0x004 (R/W)  高速缓存无效寄存器 */
  __IOM uint32_t CRCR[4U];               /*!< Offset: 0x008 (R/W)  可高缓区配置寄存器 */
        uint32_t RSERVED0[1015U];
  __IOM uint32_t CPFCR;                  /*!< Offset: 0xFF4 (R/W)  高速缓存性能分析控制寄存器 */
  __IOM uint32_t CPFATR;                 /*!< Offset: 0xFF8 (R/W)  高速缓存访问次数寄存器 */
  __IOM uint32_t CPFMTR;                 /*!< Offset: 0xFFC (R/W)  高速缓存缺失次数寄存器 */
} CACHE_Type;

/* CACHE Register Definitions */
#define CACHE_CER_EN_Pos                       0U                                            /*!< CACHE CER: EN Position */
#define CACHE_CER_EN_Msk                       (0x1UL << CACHE_CER_EN_Pos)                   /*!< CACHE CER: EN Mask */

#define CACHE_CER_CFIG_Pos                     1U                                            /*!< CACHE CER: CFIG Position */
#define CACHE_CER_CFIG_Msk                     (0x1UL << CACHE_CER_CFIG_Pos)                 /*!< CACHE CER: CFIG Mask */

#define CACHE_CER_WB_Pos                       2U                                            /*!< CACHE CER: WB Position */
#define CACHE_CER_WB_Msk                       (0x1UL << CACHE_CER_WB_Pos)                   /*!< CACHE CER: WB Mask */

#define CACHE_CER_WCFIG_Pos                    3U                                            /*!< CACHE CER: WCFIG Position */
#define CACHE_CER_WCFIG_Msk                    (0x1UL << CACHE_CER_WCFIG_Pos)                /*!< CACHE CER: WCFIG Mask */

#define CACHE_CER_DCW_Pos                      4U                                            /*!< CACHE CER: DCW Position */
#define CACHE_CER_DCW_Msk                      (0x1UL << CACHE_CER_DCW_Pos)                  /*!< CACHE CER: DCW Mask */

#define CACHE_CER_CS_Pos                       31U                                           /*!< CACHE CER: CS Position */
#define CACHE_CER_CS_Msk                       (0x1UL << CACHE_CER_CS_Pos)                   /*!< CACHE CER: CS Mask */

#define CACHE_CIR_INV_ALL_Pos                  0U                                            /*!< CACHE CIR: INV_ALL Position */
#define CACHE_CIR_INV_ALL_Msk                  (0x1UL << CACHE_CIR_INV_ALL_Pos)              /*!< CACHE CIR: INV_ALL Mask */

#define CACHE_CIR_INV_ONE_Pos                  1U                                            /*!< CACHE CIR: INV_ONE Position */
#define CACHE_CIR_INV_ONE_Msk                  (0x1UL << CACHE_CIR_INV_ONE_Pos)              /*!< CACHE CIR: INV_ONE Mask */

#define CACHE_CIR_CLR_ALL_Pos                  2U                                            /*!< CACHE CIR: CLR_ALL Position */
#define CACHE_CIR_CLR_ALL_Msk                  (0x1UL << CACHE_CIR_CLR_ALL_Pos)              /*!< CACHE CIR: CLR_ALL Mask */

#define CACHE_CIR_CLR_ONE_Pos                  3U                                            /*!< CACHE CIR: CLR_ONE Position */
#define CACHE_CIR_CLR_ONE_Msk                  (0x1UL << CACHE_CIR_CLR_ONE_Pos)              /*!< CACHE CIR: CLR_ONE Mask */

#define CACHE_CIR_INV_ADDR_Pos                 4U                                            /*!< CACHE CIR: INV_ADDR Position */
#define CACHE_CIR_INV_ADDR_Msk                 (0xFFFFFFFUL << CACHE_CIR_INV_ADDR_Pos)       /*!< CACHE CIR: INV_ADDR Mask */

#define CACHE_CRCR_EN_Pos                      0U                                            /*!< CACHE CRCR: EN Position */
#define CACHE_CRCR_EN_Msk                      (0x1UL << CACHE_CRCR_EN_Pos)                  /*!< CACHE CRCR: EN Mask */

#define CACHE_CRCR_SIZE_Pos                    1U                                            /*!< CACHE CRCR: Size Position */
#define CACHE_CRCR_SIZE_Msk                    (0x1FUL << CACHE_CRCR_SIZE_Pos)               /*!< CACHE CRCR: Size Mask */

#define CACHE_CRCR_BASE_ADDR_Pos               10U                                           /*!< CACHE CRCR: base addr Position */
#define CACHE_CRCR_BASE_ADDR_Msk               (0x3FFFFFUL << CACHE_CRCR_BASE_ADDR_Pos)      /*!< CACHE CRCR: base addr Mask */

#define CACHE_CPFCR_PFEN_Pos                   0U                                            /*!< CACHE CPFCR: PFEN Position */
#define CACHE_CPFCR_PFEN_Msk                   (0x1UL << CACHE_CPFCR_PFEN_Pos)               /*!< CACHE CPFCR: PFEN Mask */

#define CACHE_CPFCR_PFRST_Pos                  1U                                            /*!< CACHE CPFCR: PFRST Position */
#define CACHE_CPFCR_PFRST_Msk                  (0x1UL << CACHE_CPFCR_PFRST_Pos)              /*!< CACHE CPFCR: PFRST Mask */

#define CACHE_CRCR_4K                          0xB                                           /* 01011 */
#define CACHE_CRCR_8K                          0xC                                           /* 01100 */
#define CACHE_CRCR_16K                         0xD                                           /* 01101 */
#define CACHE_CRCR_32K                         0xE                                           /* 01110 */
#define CACHE_CRCR_64K                         0xF                                           /* 01111 */
#define CACHE_CRCR_128K                        0x10                                          /* 10000 */
#define CACHE_CRCR_256K                        0x11                                          /* 10001 */
#define CACHE_CRCR_512K                        0x12                                          /* 10010 */
#define CACHE_CRCR_1M                          0x13                                          /* 10011 */
#define CACHE_CRCR_2M                          0x14                                          /* 10100 */
#define CACHE_CRCR_4M                          0x15                                          /* 10101 */
#define CACHE_CRCR_8M                          0x16                                          /* 10110 */
#define CACHE_CRCR_16M                         0x17                                          /* 10111 */
#define CACHE_CRCR_32M                         0x18                                          /* 11000 */
#define CACHE_CRCR_64M                         0x19                                          /* 11001 */
#define CACHE_CRCR_128M                        0x1A                                          /* 11010 */
#define CACHE_CRCR_256M                        0x1B                                          /* 11011 */
#define CACHE_CRCR_512M                        0x1C                                          /* 11100 */
#define CACHE_CRCR_1G                          0x1D                                          /* 11101 */
#define CACHE_CRCR_2G                          0x1E                                          /* 11110 */
#define CACHE_CRCR_4G                          0x1F                                          /* 11111 */

/*@} end of group CSI_VIC */


/**
  \ingroup  CSI_core_register
  \defgroup CSI_SysTick     System Tick Timer (CORET)
  \brief    Type definitions for the System Timer Registers.
  @{
 */

/**
  \brief  访问系统计时器的数据结构.
 */
typedef struct {
    __IOM uint32_t CTRL;                   /*!< Offset: 0x000 (R/W)  控制状态寄存器 */
    __IOM uint32_t LOAD;                   /*!< Offset: 0x004 (R/W)  回填值寄存器 */
    __IOM uint32_t VAL;                    /*!< Offset: 0x008 (R/W)  当前值寄存器 */
    __IM  uint32_t CALIB;                  /*!< Offset: 0x00C (R/ )  校准寄存器 */
} CORET_Type;

/* CORET Control / Status Register Definitions */
#define CORET_CTRL_COUNTFLAG_Pos           16U                                            /*!< CORET CTRL: COUNTFLAG Position */
#define CORET_CTRL_COUNTFLAG_Msk           (1UL << CORET_CTRL_COUNTFLAG_Pos)              /*!< CORET CTRL: COUNTFLAG Mask */

#define CORET_CTRL_CLKSOURCE_Pos            2U                                            /*!< CORET CTRL: CLKSOURCE Position */
#define CORET_CTRL_CLKSOURCE_Msk           (1UL << CORET_CTRL_CLKSOURCE_Pos)              /*!< CORET CTRL: CLKSOURCE Mask */

#define CORET_CTRL_TICKINT_Pos              1U                                            /*!< CORET CTRL: TICKINT Position */
#define CORET_CTRL_TICKINT_Msk             (1UL << CORET_CTRL_TICKINT_Pos)                /*!< CORET CTRL: TICKINT Mask */

#define CORET_CTRL_ENABLE_Pos               0U                                            /*!< CORET CTRL: ENABLE Position */
#define CORET_CTRL_ENABLE_Msk              (1UL /*<< CORET_CTRL_ENABLE_Pos*/)             /*!< CORET CTRL: ENABLE Mask */

    /* CORET Reload Register Definitions */
#define CORET_LOAD_RELOAD_Pos               0U                                            /*!< CORET LOAD: RELOAD Position */
#define CORET_LOAD_RELOAD_Msk              (0xFFFFFFUL /*<< CORET_LOAD_RELOAD_Pos*/)      /*!< CORET LOAD: RELOAD Mask */

    /* CORET Current Register Definitions */
#define CORET_VAL_CURRENT_Pos               0U                                            /*!< CORET VAL: CURRENT Position */
#define CORET_VAL_CURRENT_Msk              (0xFFFFFFUL /*<< CORET_VAL_CURRENT_Pos*/)      /*!< CORET VAL: CURRENT Mask */

    /* CORET Calibration Register Definitions */
#define CORET_CALIB_NOREF_Pos               31U                                           /*!< CORET CALIB: NOREF Position */
#define CORET_CALIB_NOREF_Msk              (1UL << CORET_CALIB_NOREF_Pos)                 /*!< CORET CALIB: NOREF Mask */

#define CORET_CALIB_SKEW_Pos                30U                                           /*!< CORET CALIB: SKEW Position */
#define CORET_CALIB_SKEW_Msk               (1UL << CORET_CALIB_SKEW_Pos)                  /*!< CORET CALIB: SKEW Mask */

#define CORET_CALIB_TENMS_Pos               0U                                            /*!< CORET CALIB: TENMS Position */
#define CORET_CALIB_TENMS_Msk              (0xFFFFFFUL /*<< CORET_CALIB_TENMS_Pos*/)      /*!< CORET CALIB: TENMS Mask */

/*@} end of group CSI_SysTick */

/**
  \ingroup    CSI_core_register
  \defgroup   CSI_core_bitfield     Core register bit field macros
  \brief      Macros for use with bit field definitions (xxx_Pos, xxx_Msk).
  @{
 */

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
  \ingroup    CSI_core_register
  \defgroup   CSI_core_base     Core Definitions
  \brief      Definitions for base addresses, unions, and structures.
  @{
 */

/* Memory mapping of CK803S Hardware */
#define TCIP_BASE           (0xE000E000UL)                            /*!< Titly Coupled IP Base Address */
#define CORET_BASE          (TCIP_BASE +  0x0010UL)                   /*!< CORET Base Address */
#define VIC_BASE           (TCIP_BASE +  0x0100UL)                   /*!< VIC Base Address */
#define CACHE_BASE          (TCIP_BASE +  0x1000UL)                   /*!< CACHE Base Address */

#define CORET               ((CORET_Type   *)     CORET_BASE  )       /*!< SysTick configuration struct */
#define VIC                ((VIC_Type    *)     VIC_BASE   )       /*!< VIC configuration struct */
#define CACHE               ((CACHE_Type   *)     CACHE_BASE  )       /*!< cache configuration struct */

/*@} */

#ifdef __cplusplus
}
#endif

#endif /* __CORE_CK803S_H_DEPENDANT */

#endif /* __CSI_GENERIC */
