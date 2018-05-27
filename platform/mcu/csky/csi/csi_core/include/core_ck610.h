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
 * @file     core_ck610.h
 * @brief    CSI CK610 Core Peripheral Access Layer Header File
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/

#ifndef __CORE_CK610_H_GENERIC
#define __CORE_CK610_H_GENERIC

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 *                 CSI definitions
 ******************************************************************************/
/**
  \ingroup Ck610
  @{
 */

/*  CSI CK610 definitions */
#define __CK610_CSI_VERSION_MAIN  (0x01U)                                      /*!< [31:16] CSI HAL main version */
#define __CK610_CSI_VERSION_SUB   (0x1EU)                                      /*!< [15:0]  CSI HAL sub version */
#define __CK610_CSI_VERSION       ((__CK610_CSI_VERSION_MAIN << 16U) | \
                                   __CK610_CSI_VERSION_SUB           )        /*!< CSI HAL version number */

#define __CK610                   (0x01U)                                         /*!< CK610 Core */

/** __FPU_USED indicates whether an FPU is used or not.
    This core does not support an FPU at all
*/
#define __FPU_USED       0U

#if defined ( __GNUC__ )
#if defined (__VFP_FP__) && !defined(__SOFTFP__)
#error "Compiler generates FPU instructions for a device without an FPU (check __FPU_PRESENT)"
#endif
#endif

#ifdef __cplusplus
}
#endif

#endif /* __CORE_CK610_H_GENERIC */

#ifndef __CSI_GENERIC

#ifndef __CORE_CK610_H_DEPENDANT
#define __CORE_CK610_H_DEPENDANT

#ifdef __cplusplus
extern "C" {
#endif

/* check device defines and use defaults */
//#if defined __CHECK_DEVICE_DEFINES
#ifndef __CK610_REV
#define __CK610_REV               0x0000U
//#warning "__CK610_REV not defined in device header file; using default!"
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

#include <csi_gcc.h>

//#endif

/* IO definitions (access restrictions to peripheral registers) */
/**
    \defgroup CSI_glob_defs CSI Global Defines

    <strong>IO Type Qualifiers</strong> are used
    \li to specify the access to peripheral variables.
    \li for automatic generation of peripheral register debug information.
*/
#ifdef __cplusplus
#define     __I     volatile             /*!< Defines 'read only' permissions */
#else
#define     __I     volatile const       /*!< Defines 'read only' permissions */
#endif
#define     __O     volatile             /*!< Defines 'write only' permissions */
#define     __IO    volatile             /*!< Defines 'read / write' permissions */

/* following defines should be used for structure members */
#define     __IM     volatile const      /*! Defines 'read only' structure member permissions */
#define     __OM     volatile            /*! Defines 'write only' structure member permissions */
#define     __IOM    volatile            /*! Defines 'read / write' structure member permissions */

/*@} end of group CK610 */

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
  \brief Type definitions and defines for CK610 processor based devices.
*/

/**
  \brief Consortium definition for accessing Cache Configuration Registers(CCR, CR<18, 0>).
 */
typedef union {
    struct {
        uint32_t MP: 1;                      /*!< bit:      0  memory protection settings */
        uint32_t _reserved0: 6;              /*!< bit:  1.. 6  Reserved */
        uint32_t BE: 1;                      /*!< bit:      7  Endian mode */
        uint32_t SCK: 3;                     /*!< bit:  8..10  the clock ratio of the system and the processor */
        uint32_t _reserved1: 2;              /*!< bit: 11..12  Reserved */
        uint32_t BE_V2: 1;                   /*!< bit:     13  V2 Endian mode */
        uint32_t _reserved2: 18;             /*!< bit: 14..31  Reserved */
    } b;                                   /*!< Structure    Access by bit */
    uint32_t w;                            /*!< Type         Access by whole register */
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
  \brief  Consortium definition for accessing high ease access permission configutation registers(CAPR, CR<19,0>).
 */
typedef union {
    struct {
//TODO:
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
      \brief  Consortium definition for accessing control register(PACR, CR<20,0>).
     */
    typedef union {
        struct {
            uint32_t E: 1;                       /*!< bit:      0  Effective setting of protected area */
            uint32_t Size: 5;                    /*!< bit:  1.. 5  Size of protected area */
            uint32_t _reserved0: 4;              /*!< bit:  6.. 9  Reserved */
            uint32_t base_addr: 22;              /*!< bit: 10..31  The high position of the address of a protected area */
        } b;                                   /*!< Structure    Access by bit */
        uint32_t w;                            /*!< Type         Access by whole register */
    } PACR_Type;

    /* PACR Register Definitions */
#define PACR_BASE_ADDR_Pos                 10U                                            /*!< PACR: base_addr Position */
#define PACK_BASE_ADDR_Msk                 (0x3FFFFFUL << PACR_BASE_ADDR_Pos)             /*!< PACR: base_addr Mask */

#define PACR_SIZE_Pos                      1U                                             /*!< PACR: Size Position */
#define PACK_SIZE_Msk                      (0x1FUL << PACR_SIZE_Pos)                      /*!< PACR: Size Mask */

#define PACR_E_Pos                         0U                                             /*!< PACR: E Position */
#define PACK_E_Msk                         (0x1UL << PACR_E_Pos)                          /*!< PACR: E Mask */

    /**
      \brief  Consortium definition for accessing protection area selection register(PRSR,CR<21,0>).
     */
    typedef union {
        struct {
            uint32_t RID: 3;                     /*!< bit:  0.. 2  Protected area index value */
            uint32_t _reserved0: 29;             /*!< bit:  3..31  Reserved */
        } b;                                   /*!< Structure    Access by bit */
        uint32_t w;                            /*!< Type         Access by whole register */
    } PRSR_Type;

    /* PRSR Register Definitions */
#define PRSR_RID_Pos                       0U                                            /*!< PRSR: RID Position */
#define PRSR_RID_Msk                       (0x7UL << PRSR_RID_Pos)                       /*!< PRSR: RID Mask */

/*@} end of group CSI_CORE */

/**
  \ingroup    CSI_core_register
  \defgroup   CSI_CACHE
  \brief      Type definitions for the cache Registers
  @{
 */
#define set_cr17(value) \
    __asm__ __volatile__( \
        "mtcr   %0, cr17\n\t" \
        ::"r"(value))

#define set_cr22(value)                 \
__asm__ __volatile__("mtcr  %0, cr22\n\t"       \
            ::"r"(value)) 

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


/*******************************************************************************
 *                Hardware Abstraction Layer
  Core Function Interface contains:
  - Core VIC Functions
  - Core CORET Functions
  - Core Register Access Functions
 ******************************************************************************/
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
__ALWAYS_INLINE void csi_icache_enable (void)
{
    uint32_t tmp=0;
    __ASM volatile("mfcr %0, cr18\n\t"
                   "bseti %0, 2\n\t"
                   "mtcr %0, cr18\n\t"
                ::"r"(tmp));
}


/**
  \brief   Disable I-Cache
  \details Turns off I-Cache
  */
__ALWAYS_INLINE void csi_icache_disable (void)
{
    uint32_t tmp=0;
    __ASM volatile("mfcr %0, cr18\n\t"
                   "bclri %0, 2\n\t"
                   "mtcr %0, cr18\n\t"
                ::"r"(tmp));
}


/**
  \brief   Invalidate I-Cache
  \details Invalidates I-Cache
  */
__ALWAYS_INLINE void csi_icache_invalid (void)
{
    set_cr17(0x11);
    set_cr17(INS_CACHE | CACHE_INV);
}


/**
  \brief   Enable D-Cache
  \details Turns on D-Cache
  \note    I-Cache also turns on.
  */
__ALWAYS_INLINE void csi_dcache_enable (void)
{
    uint32_t tmp=0;
    __ASM volatile("mfcr %0, cr18\n\t"
                   "bseti %0, 3\n\t"
                   "mtcr %0, cr18\n\t"
                ::"r"(tmp));
}


/**
  \brief   Disable D-Cache
  \details Turns off D-Cache
  \note    I-Cache also turns off.
  */
__ALWAYS_INLINE void csi_dcache_disable (void)
{
    uint32_t tmp=0;
    __ASM volatile("mfcr %0, cr18\n\t"
                   "bclri %0, 3\n\t"
                   "mtcr %0, cr18\n\t"
                ::"r"(tmp));
}


/**
  \brief   Invalidate D-Cache
  \details Invalidates D-Cache
  \note    I-Cache also invalid
  */
__ALWAYS_INLINE void csi_dcache_invalid (void)
{
    set_cr17(DATA_CACHE | CACHE_INV);
}


/**
  \brief   Clean D-Cache
  \details Cleans D-Cache
  \note    I-Cache also cleans
  */
__ALWAYS_INLINE void csi_dcache_clean (void)
{
    set_cr17(DATA_CACHE | CACHE_CLR);
}


/**
  \brief   Clean & Invalidate D-Cache
  \details Cleans and Invalidates D-Cache
  \note    I-Cache also flush.
  */
__ALWAYS_INLINE void csi_dcache_clean_invalid (void)
{
    set_cr17(DATA_CACHE | CACHE_CLR | CACHE_INV);
}

__ALWAYS_INLINE  void set_cache_range (uint32_t start, uint32_t end, uint32_t value)
{
    if (!(start & SSEG0_BASE_ADDR) || (end - start) &~(CACHE_RANGE_MAX_SIZE - 1)) {
        set_cr17(value);
    }

    if (value & INS_CACHE) {
        csi_icache_disable();
    }
    uint32_t i;
    for (i = start; i < end; i += L1_CACHE_BYTES) {
        set_cr22(i);
        set_cr17(CACHE_OMS | value);
    }

    if (end & (L1_CACHE_BYTES-1)) {
        set_cr22(end);
        set_cr17(CACHE_OMS | value);
    }

    if (value & INS_CACHE) {
        csi_icache_enable();
    }

}

/**
  \brief   D-Cache Invalidate by address
  \details Invalidates D-Cache for the given address
  \param[in]   addr    address (aligned to 16-byte boundary)
  \param[in]   dsize   size of memory block (in number of bytes)
*/
__ALWAYS_INLINE csi_dcache_invalid_range (uint32_t *addr, int32_t dsize)
{
    set_cache_range((uint32_t)addr, (uint32_t)addr + dsize, (DATA_CACHE | CACHE_INV));
}


/**
  \brief   D-Cache Clean by address
  \details Cleans D-Cache for the given address
  \param[in]   addr    address (aligned to 16-byte boundary)
  \param[in]   dsize   size of memory block (in number of bytes)
*/
__ALWAYS_INLINE void csi_dcache_clean_range (uint32_t *addr, int32_t dsize)
{
    set_cache_range((uint32_t)addr, (uint32_t)addr + dsize, (DATA_CACHE | CACHE_CLR));
}


/**
  \brief   D-Cache Clean and Invalidate by address
  \details Cleans and invalidates D_Cache for the given address
  \param[in]   addr    address (aligned to 16-byte boundary)
  \param[in]   dsize   size of memory block (in number of bytes)
*/
__ALWAYS_INLINE void csi_dcache_clean_invalid_range (uint32_t *addr, int32_t dsize)
{
    set_cache_range((uint32_t)addr, (uint32_t)addr + dsize, (DATA_CACHE | CACHE_CLR | CACHE_INV));
}

/*@} end of CSI_Core_CacheFunctions */

/*@} */

#ifdef __cplusplus
}
#endif

#endif /* __CORE_CK610_H_DEPENDANT */

#endif /* __CSI_GENERIC */
