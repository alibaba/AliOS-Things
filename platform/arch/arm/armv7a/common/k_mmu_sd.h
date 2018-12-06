/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#ifndef K_MMU_SD_H
#define K_MMU_SD_H

#include "k_mmu.h"

#ifndef ALIGN_UP
#define ALIGN_UP(x, align)      (((x) + (align) - 1) & ~((align) - 1))
#endif

#ifndef ALIGN_DOWN
#define ALIGN_DOWN(x, align)    ((x) & ~((align) - 1))
#endif

/* first-level descriptor formats */
/*
Supersection    Bits[31:24] of the descriptor are bits[31:24] of the address of the Supersection.
Section         Bits[31:20] of the descriptor are bits[31:20] of the address of the Section.
Page table      Bits[31:10] of the descriptor are bits[31:10] of the address of a Page table.
*/
#define MMU_SUPERSECTION_BITS               24
#define MMU_SUPERSECTION_SIZE               (1<<MMU_SUPERSECTION_BITS)
#define MMU_SECTION_BITS                    20
#define MMU_SECTION_SIZE                    (1<<MMU_SECTION_BITS)
#define MMU_PAGETABLE_BITS                  10
#define MMU_PAGETABLE_SIZE                  (1<<MMU_PAGETABLE_BITS)
#define MMU_SUPERSECTION_REPEAT             (MMU_SUPERSECTION_SIZE / MMU_SECTION_SIZE)
#define MMU_SECTION_ENTRY_CNT               4096
/* first-level page table size
   number of section * entry size = 4096 * 4 = 16KB */
#define MMU_TALBE_SIZE_LVL1                 (MMU_SECTION_ENTRY_CNT*4)

#if (K_CONFIG_MMU_LVL2 == 1)
/* second-level descriptor formats */
/*
Large page  Bits[31:16] of the descriptor are bits[31:16] of the address of the Large page.
Small page  Bits[31:12] of the descriptor are bits[31:12] of the address of the Small page.
*/
#define MMU_LARGEPAGE_BITS                  16
#define MMU_LARGEPAGE_SIZE                  (1<<MMU_LARGEPAGE_BITS)
#define MMU_SMALLPAGE_BITS                  12
#define MMU_SMALLPAGE_SIZE                  (1<<MMU_SMALLPAGE_BITS)
#define MMU_LARGEPAGE_REPEAT                (MMU_LARGEPAGE_SIZE / MMU_SMALLPAGE_SIZE)
#define MMU_PAGE_ENTRY_CNT                  (MMU_SECTION_SIZE / MMU_SMALLPAGE_SIZE)
/* second-level page table size
   number of pages in one section * entry size = 256 * 4 = 1KB */
#define MMU_TALBE_SIZE_LVL2                 (MMU_PAGE_ENTRY_CNT*4)
#endif

/********* first-level descriptor formats for Supersection: *********
Bit0        PXN
Bit1        1
Bit2        B
Bit3        C
Bit4        XN, The Execute-never bit.
Bit5~8      Domain, Domain field. (DACR defines the permitted access for each domain)
Bit9        IMPLEMENTATION DEFINED
Bit10~11    AP[1:0], Access Permissions bits.
Bit12~14    TEX[2:0]
Bit15       AP[2], Access Permissions bits.
Bit16       S, The Shareable bit.
Bit17       nG, The not global bit.
Bit18       1, Descriptor is for a Supersection
Bit19       NS, Non-secure bit.
Bit20~31    PA, Section base address
*********************************************************************/
/* Secure + Global + Non-Shareable + AP(No access) + Domain 0 + Executable + strongly-ordered */
#define DES_SUPSEC_BASE                     0x40002
/* Supersection descriptor is same as Section descriptor, except followed bit */
#define DES_SUPSEC_FLAG                     0x40000

/************ first-level descriptor formats for Section: ***********
Bit0        PXN
Bit1        1
Bit2        B
Bit3        C
Bit4        XN, The Execute-never bit.
Bit5~8      Domain, Domain field. (DACR defines the permitted access for each domain)
Bit9        IMPLEMENTATION DEFINED
Bit10~11    AP[1:0], Access Permissions bits.
Bit12~14    TEX[2:0]
Bit15       AP[2], Access Permissions bits.
Bit16       S, The Shareable bit.
Bit17       nG, The not global bit.
Bit18       0, Descriptor is for a Section
Bit19       NS, Non-secure bit.
Bit20~31    PA, Section base address
*********************************************************************/
/* Secure + Global + Non-Shareable + AP(No access) + Domain 0 + Executable + strongly-ordered */
#define DES_SEC_BASE                        0x2

/* XN, The Execute-never bit. */
#define DES_SEC_XN                          (1<<4)

/* Domain field, n = 0~15 */
#define DES_SEC_DOMAIN(n)                   ((n)<<5)

/* Memory Type, TEX[2:0] C B (TEX, C, and B encodings when SCTLR.TRE == 0):
MT_STRONGLY_ORDERED:    Strongly-ordered
MT_SHARED_DEVICE:       Shareable Device
MT_NONSHARED_DEVICE:    Non-shareable Device
MT_NORMAL_NONCACHE:     Normal + Non-cacheable
MT_NORMAL_CACHE_WB:     Normal + Write-Back, Write-Allocate
MT_NORMAL_CACHE_WT:     Normal + Write-Through, no Write-Allocate
*/
#define DES_SEC_MT_STRONGLY_ORDERED         ((0<<12) | (0<<3) | (0<<2))
#define DES_SEC_MT_SHARED_DEVICE            ((0<<12) | (0<<3) | (1<<2))
#define DES_SEC_MT_NONSHARED_DEVICE         ((2<<12) | (0<<3) | (0<<2))
#define DES_SEC_MT_NORMAL_NONCACHE          ((1<<12) | (0<<3) | (0<<2))
#define DES_SEC_MT_NORMAL_CACHE_WB          ((1<<12) | (1<<3) | (1<<2))
#define DES_SEC_MT_NORMAL_CACHE_WT          ((0<<12) | (1<<3) | (0<<2))

/* Access Permissions, AP[2:0], (When SCTLR.AFE == 0):
   000      PL1, No access      PL0, No access
   001      PL1, Full access    PL0, No access
   010      PL1, Full access    PL0, Read-Only
   011      PL1, Full access    PL0, Full access
   101      PL1, Read-only      PL0, No access

   KRM - kernel mode read write ( user mode no access )
   URW - user read write ( kernel mode read write )
*/
#define DES_SEC_AP_KRW                      ((0<<15) | (1<<10))
#define DES_SEC_AP_KRO                      ((1<<15) | (1<<10))
#define DES_SEC_AP_URW                      ((0<<15) | (3<<10))
#define DES_SEC_AP_URO                      ((0<<15) | (2<<10))

/* S, The Shareable bit */
#define DES_SEC_S                           (1<<16)

/* nG, The not global bit. */
#define DES_SEC_nG                          (1<<17)

/* NS, Non-secure bit. */
#define DES_SEC_NS                          (1<<19)

/********** mem_attr for Normal memory (e.g. ddr, sram ...) **********/
/* Secure + Global + Non-Shareable + Domain 0 + Executable + Cache write back */
#define MMU_ATTR_SEC_X_WB    \
        (DES_SEC_BASE | DES_SEC_MT_NORMAL_CACHE_WB)
/* Secure + Global + Non-Shareable + Domain 0 + Non-Executable + Cache write back */
#define MMU_ATTR_SEC_NX_WB   \
        (DES_SEC_BASE | DES_SEC_XN | DES_SEC_MT_NORMAL_CACHE_WB)
/* Secure + Global + Non-Shareable + Domain 0 + Executable + Cache Write through */
#define MMU_ATTR_SEC_X_WT    \
        (DES_SEC_BASE | DES_SEC_MT_NORMAL_CACHE_WT)
/* Secure + Global + Non-Shareable + Domain 0 + Non-Executable + Cache Write through */
#define MMU_ATTR_SEC_NX_WT    \
        (DES_SEC_BASE | DES_SEC_XN  | DES_SEC_MT_NORMAL_CACHE_WT)
/* Secure + Global + Non-Shareable + Domain 0 + Executable + Non-cacheable */
#define MMU_ATTR_SEC_X_NC    \
        (DES_SEC_BASE | DES_SEC_MT_NORMAL_NONCACHE)
/* Secure + Global + Non-Shareable + Domain 0 + Non-Executable + Non-cacheable */
#define MMU_ATTR_SEC_NX_NC    \
        (DES_SEC_BASE | DES_SEC_XN | DES_SEC_MT_NORMAL_NONCACHE)
/*******************************************************/

/********** mem_attr for MMIO (e.g. regs, dma ...) **********/
/* Secure + Global + Domain 0 + Non-Executable + Strongly-ordered */
#define MMU_ATTR_SEC_NX_SO   \
        (DES_SEC_BASE | DES_SEC_XN | DES_SEC_MT_STRONGLY_ORDERED)
/* Secure + Global + Domain 0 + Non-Executable + Shareable Device */
#define MMU_ATTR_SEC_NX_DEV   \
        (DES_SEC_BASE | DES_SEC_XN | DES_SEC_MT_SHARED_DEVICE)
/* Secure + Global + Domain 0 + Non-Executable + Non-shareable Device */
#define MMU_ATTR_SEC_NX_DEV_NSH   \
        (DES_SEC_BASE | DES_SEC_XN | DES_SEC_MT_NONSHARED_DEVICE)
/*******************************************************/

#if (K_CONFIG_MMU_LVL2 == 1)

/********** First-level descriptor formats for Page table: **********
Bit0        0
Bit1        1
Bit2        PXN
Bit3        NS, Non-secure bit.
Bit4        SBZ
Bit5~8      Domain, Domain field. (DACR defines the permitted access for each domain)
Bit9        IMPLEMENTATION DEFINED
Bit10~31    PA, Page table base address
*********************************************************************/
/* Secure + Domain 0 */
#define DES_PGTBL_BASE                      0x1

/**********Second-level descriptor formats for Large page: **********
Bit0        1
Bit1        0
Bit2        B
Bit3        C
Bit4~5      AP[1:0], Access Permissions bits.
Bit6~8      SBZ
Bit9        AP[2], Access Permissions bits.
Bit10       S, The Shareable bit.
Bit11       nG, The not global bit.
Bit12~14    TEX[2:0]
Bit15       XN, The Execute-never bit.
Bit16~31    PA, Large page base address
*********************************************************************/
/* Secure + Global + Non-Shareable + AP(No access) + Domain 0 + Executable + strongly-ordered */
#define DES_LPG_BASE                       0x1

/* XN, The Execute-never bit. */
#define DES_LPG_XN                         (1<<15)

/* Memory Type, TEX[2:0] C B (TEX, C, and B encodings when SCTLR.TRE == 0):
MT_STRONGLY_ORDERED:    Strongly-ordered
MT_SHARED_DEVICE:       Shareable Device
MT_NONSHARED_DEVICE:    Non-shareable Device
MT_NORMAL_NONCACHE:     Normal + Non-cacheable
MT_NORMAL_CACHE_WB:     Normal + Write-Back, Write-Allocate
MT_NORMAL_CACHE_WT:     Normal + Write-Through, no Write-Allocate
*/
#define DES_LPG_MT_STRONGLY_ORDERED        ((0<<12) | (0<<3) | (0<<2))
#define DES_LPG_MT_SHARED_DEVICE           ((0<<12) | (0<<3) | (1<<2))
#define DES_LPG_MT_NONSHARED_DEVICE        ((2<<12) | (0<<3) | (0<<2))
#define DES_LPG_MT_NORMAL_NONCACHE         ((1<<12) | (0<<3) | (0<<2))
#define DES_LPG_MT_NORMAL_CACHE_WB         ((1<<12) | (1<<3) | (1<<2))
#define DES_LPG_MT_NORMAL_CACHE_WT         ((0<<12) | (1<<3) | (0<<2))

/* Access Permissions, AP[2:0], (When SCTLR.AFE == 0):
   000      PL1, No access      PL0, No access
   001      PL1, Full access    PL0, No access
   010      PL1, Full access    PL0, Read-Only
   011      PL1, Full access    PL0, Full access
   101      PL1, Read-only      PL0, No access

   KRM - kernel mode read write ( user mode no access )
   URW - user read write ( kernel mode read write )
*/
#define DES_LPG_AP_KRW                     ((0<<9) | (1<<4))
#define DES_LPG_AP_KRO                     ((1<<9) | (1<<4))
#define DES_LPG_AP_URW                     ((0<<9) | (3<<4))
#define DES_LPG_AP_URO                     ((0<<9) | (2<<4))

/* S, The Shareable bit */
#define DES_LPG_S                          (1<<10)

/* nG, The not global bit. */
#define DES_LPG_nG                         (1<<11)

/********** mem_attr for Normal memory (e.g. ddr, sram ...) **********/
/* Secure + Global + Non-Shareable + Read-only + Domain 0 + Executable + Cache write back */
#define MMU_ATTR_LPG_X_WB    \
        (DES_LPG_BASE | DES_LPG_MT_NORMAL_CACHE_WB)
/* Secure + Global + Non-Shareable + Read-only + Domain 0 + Non-Executable + Cache write back */
#define MMU_ATTR_LPG_NX_WB   \
        (DES_LPG_BASE | DES_LPG_XN | DES_LPG_MT_NORMAL_CACHE_WB)
/* Secure + Global + Non-Shareable + Domain 0 + Executable + Cache Write through */
#define MMU_ATTR_LPG_X_WT    \
        (DES_LPG_BASE | DES_LPG_MT_NORMAL_CACHE_WT)
/* Secure + Global + Non-Shareable + Domain 0 + Non-Executable + Cache Write through */
#define MMU_ATTR_LPG_NX_WT    \
        (DES_LPG_BASE | DES_LPG_XN  | DES_LPG_MT_NORMAL_CACHE_WT)
/* Secure + Global + Non-Shareable + Domain 0 + Executable + Non-cacheable */
#define MMU_ATTR_LPG_X_NC    \
        (DES_LPG_BASE | DES_LPG_MT_NORMAL_NONCACHE)
/* Secure + Global + Non-Shareable + Domain 0 + Non-Executable + Non-cacheable */
#define MMU_ATTR_LPG_NX_NC    \
        (DES_LPG_BASE | DES_LPG_XN | DES_LPG_MT_NORMAL_NONCACHE)
/*******************************************************/

/********** mem_attr for MMIO (e.g. regs, dma ...) **********/
/* Secure + Global + Domain 0 + Non-Executable + Strongly-ordered */
#define MMU_ATTR_LPG_NX_SO   \
        (DES_LPG_BASE | DES_LPG_XN | DES_LPG_MT_STRONGLY_ORDERED)
/* Secure + Global + Domain 0 + Non-Executable + Shareable Device */
#define MMU_ATTR_LPG_NX_DEV   \
        (DES_LPG_BASE | DES_LPG_XN | DES_LPG_MT_SHARED_DEVICE)
/* Secure + Global + Domain 0 + Non-Executable + Non-shareable Device */
#define MMU_ATTR_LPG_NX_DEV_NSH   \
        (DES_LPG_BASE | DES_LPG_XN | DES_LPG_MT_NONSHARED_DEVICE)
/*******************************************************/

/**********Second-level descriptor formats for Small page: **********
Bit0        XN, The Execute-never bit.
Bit1        1
Bit2        B
Bit3        C
Bit4~5      AP[1:0], Access Permissions bits.
Bit6~8      TEX[2:0]
Bit9        AP[2], Access Permissions bits.
Bit10       S, The Shareable bit.
Bit11       nG, The not global bit.
Bit12~31    PA, Small page base address
*********************************************************************/
/* Secure + Global + Non-Shareable + AP(No access) + Domain 0 + Executable + strongly-ordered */
#define DES_SPG_BASE                       0x2

/* XN, The Execute-never bit. */
#define DES_SPG_XN                         (1<<0)

/* Memory Type, TEX[2:0] C B (TEX, C, and B encodings when SCTLR.TRE == 0):
MT_STRONGLY_ORDERED:    Strongly-ordered
MT_SHARED_DEVICE:       Shareable Device
MT_NONSHARED_DEVICE:    Non-shareable Device
MT_NORMAL_NONCACHE:     Normal + Non-cacheable
MT_NORMAL_CACHE_WB:     Normal + Write-Back, Write-Allocate
MT_NORMAL_CACHE_WT:     Normal + Write-Through, no Write-Allocate
*/
#define DES_SPG_MT_STRONGLY_ORDERED        ((0<<6) | (0<<3) | (0<<2))
#define DES_SPG_MT_SHARED_DEVICE           ((0<<6) | (0<<3) | (1<<2))
#define DES_SPG_MT_NONSHARED_DEVICE        ((2<<6) | (0<<3) | (0<<2))
#define DES_SPG_MT_NORMAL_NONCACHE         ((1<<6) | (0<<3) | (0<<2))
#define DES_SPG_MT_NORMAL_CACHE_WB         ((1<<6) | (1<<3) | (1<<2))
#define DES_SPG_MT_NORMAL_CACHE_WT         ((0<<6) | (1<<3) | (0<<2))

/* Access Permissions, AP[2:0], (When SCTLR.AFE == 0):
   000      PL1, No access      PL0, No access
   001      PL1, Full access    PL0, No access
   010      PL1, Full access    PL0, Read-Only
   011      PL1, Full access    PL0, Full access
   101      PL1, Read-only      PL0, No access

   KRM - kernel mode read write ( user mode no access )
   URW - user read write ( kernel mode read write )
*/
#define DES_SPG_AP_KRW                     ((0<<9) | (1<<4))
#define DES_SPG_AP_KRO                     ((1<<9) | (1<<4))
#define DES_SPG_AP_URW                     ((0<<9) | (3<<4))
#define DES_SPG_AP_URO                     ((0<<9) | (2<<4))

/* S, The Shareable bit */
#define DES_SPG_S                          (1<<10)

/* nG, The not global bit. */
#define DES_SPG_nG                         (1<<11)

/********** mem_attr for Normal memory (e.g. ddr, sram ...) **********/
/* Secure + Global + Non-Shareable + Read-only + Domain 0 + Executable + Cache write back */
#define MMU_ATTR_SPG_X_WB    \
        (DES_SPG_BASE | DES_SPG_MT_NORMAL_CACHE_WB)
/* Secure + Global + Non-Shareable + Read-only + Domain 0 + Non-Executable + Cache write back */
#define MMU_ATTR_SPG_NX_WB   \
        (DES_SPG_BASE | DES_SPG_XN | DES_SPG_MT_NORMAL_CACHE_WB)
/* Secure + Global + Non-Shareable + Domain 0 + Executable + Cache Write through */
#define MMU_ATTR_SPG_X_WT    \
        (DES_SPG_BASE | DES_SPG_MT_NORMAL_CACHE_WT)
/* Secure + Global + Non-Shareable + Domain 0 + Non-Executable + Cache Write through */
#define MMU_ATTR_SPG_NX_WT    \
        (DES_SPG_BASE | DES_SPG_XN  | DES_SPG_MT_NORMAL_CACHE_WT)
/* Secure + Global + Non-Shareable + Domain 0 + Executable + Non-cacheable */
#define MMU_ATTR_SPG_X_NC    \
        (DES_SPG_BASE | DES_SPG_MT_NORMAL_NONCACHE)
/* Secure + Global + Non-Shareable + Domain 0 + Non-Executable + Non-cacheable */
#define MMU_ATTR_SPG_NX_NC    \
        (DES_SPG_BASE | DES_SPG_XN | DES_SPG_MT_NORMAL_NONCACHE)
/*******************************************************/

/********** mem_attr for MMIO (e.g. regs, dma ...) **********/
/* Secure + Global + Domain 0 + Non-Executable + Strongly-ordered */
#define MMU_ATTR_SPG_NX_SO   \
        (DES_SPG_BASE | DES_SPG_XN | DES_SPG_MT_STRONGLY_ORDERED)
/* Secure + Global + Domain 0 + Non-Executable + Shareable Device */
#define MMU_ATTR_SPG_NX_DEV   \
        (DES_SPG_BASE | DES_SPG_XN | DES_SPG_MT_SHARED_DEVICE)
/* Secure + Global + Domain 0 + Non-Executable + Non-shareable Device */
#define MMU_ATTR_SPG_NX_DEV_NSH   \
        (DES_SPG_BASE | DES_SPG_XN | DES_SPG_MT_NONSHARED_DEVICE)
/*******************************************************/
#endif

#endif /* K_MMU_SD_H */
