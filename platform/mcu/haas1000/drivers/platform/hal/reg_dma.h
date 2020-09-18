/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __REG_DMA_H__
#define __REG_DMA_H__

#include "plat_types.h"
#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

// Number of channels on GPDMA and AUDMA
#define DMA_NUMBER_CHANNELS 8

// DMA Channel register block structure
struct DMA_CH_T {
    __IO uint32_t  SRCADDR;             // 0x100+N*0x20 DMA Channel Source Address Register
    __IO uint32_t  DSTADDR;             // 0x104+N*0x20 DMA Channel Destination Address Register
    __IO uint32_t  LLI;                 // 0x108+N*0x20 DMA Channel Linked List Item Register
    __IO uint32_t  CONTROL;             // 0x10C+N*0x20 DMA Channel Control Register
    __IO uint32_t  CONFIG;              // 0x110+N*0x20 DMA Channel Configuration Register
         uint32_t  RESERVED1[3];        // 0x114+N*0x20
};

struct DMA_2D_CFG_T {
    __IO uint32_t  SRCX;                // 0x200+N*0x20 DMA 2D Source X Axis Register
    __IO uint32_t  SRCY;                // 0x204+N*0x20 DMA 2D Source Y Axis Register
    __IO uint32_t  DSTX;                // 0x208+N*0x20 DMA 2D Destination X Axis Register
    __IO uint32_t  DSTY;                // 0x20C+N*0x20 DMA 2D Destination Y Axis Register
    __IO uint32_t  CTRL;                // 0x210+N*0x20 DMA 2D Control Register
         uint32_t  RESERVED2[3];        // 0x214+N*0x20
};

// DMA register block
struct DMA_T {
    __I  uint32_t  INTSTAT;             // 0x000 DMA Interrupt Status Register
    __I  uint32_t  INTTCSTAT;           // 0x004 DMA Interrupt Terminal Count Request Status Register
    __O  uint32_t  INTTCCLR;            // 0x008 DMA Interrupt Terminal Count Request Clear Register
    __I  uint32_t  INTERRSTAT;          // 0x00C DMA Interrupt Error Status Register
    __O  uint32_t  INTERRCLR;           // 0x010 DMA Interrupt Error Clear Register
    __I  uint32_t  RAWINTTCSTAT;        // 0x014 DMA Raw Interrupt Terminal Count Status Register
    __I  uint32_t  RAWINTERRSTAT;       // 0x018 DMA Raw Error Interrupt Status Register
    __I  uint32_t  ENBLDCHNS;           // 0x01C DMA Enabled Channel Register
    __IO uint32_t  SOFTBREQ;            // 0x020 DMA Software Burst Request Register
    __IO uint32_t  SOFTSREQ;            // 0x024 DMA Software Single Request Register
    __IO uint32_t  SOFTLBREQ;           // 0x028 DMA Software Last Burst Request Register
    __IO uint32_t  SOFTLSREQ;           // 0x02C DMA Software Last Single Request Register
    __IO uint32_t  DMACONFIG;           // 0x030 DMA Configuration Register
    __IO uint32_t  SYNC;                // 0x034 DMA Synchronization Register
         uint32_t  RESERVED0[50];       // 0x038
    struct DMA_CH_T CH[DMA_NUMBER_CHANNELS]; // 0x100
    struct DMA_2D_CFG_T _2D[DMA_NUMBER_CHANNELS]; // 0x200
};

// Macro defines for DMA channel control registers

#define DMA_CONTROL_TRANSFERSIZE(n)   ((((n) & 0xFFF) << 0)) // Transfer size
#define DMA_CONTROL_TRANSFERSIZE_MASK (0xFFF << 0)
#define DMA_CONTROL_TRANSFERSIZE_SHIFT (0)
#define DMA_CONTROL_SBSIZE(n)         ((((n) & 0x07) << 12)) // Source burst size
#define DMA_CONTROL_DBSIZE(n)         ((((n) & 0x07) << 15)) // Destination burst size
#define DMA_CONTROL_SWIDTH(n)         ((((n) & 0x07) << 18)) // Source transfer width
#define DMA_CONTROL_SWIDTH_MASK       (0x07 << 18)
#define DMA_CONTROL_SWIDTH_SHIFT      (18)
#define DMA_CONTROL_DWIDTH(n)         ((((n) & 0x07) << 21)) // Destination transfer width
#define DMA_CONTROL_SI                ((1UL << 26))         // Source increment
#define DMA_CONTROL_DI                ((1UL << 27))         // Destination increment
#define DMA_CONTROL_SRCAHB1           0
#define DMA_CONTROL_DSTAHB1           0
#define DMA_CONTROL_PROT1             ((1UL << 28))         // Indicates that the access is in user mode or privileged mode
#define DMA_CONTROL_PROT2             ((1UL << 29))         // Indicates that the access is bufferable or not bufferable
#define DMA_CONTROL_PROT3             ((1UL << 30))         // Indicates that the access is cacheable or not cacheable
#define DMA_CONTROL_TC_IRQ            ((1UL << 31))         // Terminal count interrupt enable bit

// Macro defines for DMA Channel Configuration registers

#define DMA_CONFIG_EN                 ((1UL << 0))          // DMA control enable
#define DMA_CONFIG_SRCPERIPH(n)       ((((n) & 0x1F) << 1)) // Source peripheral
#define DMA_CONFIG_DSTPERIPH(n)       ((((n) & 0x1F) << 6)) // Destination peripheral
#define DMA_CONFIG_TRANSFERTYPE(n)    ((((n) & 0x7) << 11)) // This value indicates the type of transfer
#define DMA_CONFIG_ERR_IRQMASK        ((1UL << 14))         // Interrupt error mask
#define DMA_CONFIG_TC_IRQMASK         ((1UL << 15))         // Terminal count interrupt mask
#define DMA_CONFIG_LOCK               ((1UL << 16))         // Lock
#define DMA_CONFIG_ACTIVE             ((1UL << 17))         // Active
#define DMA_CONFIG_HALT               ((1UL << 18))         // Halt
#define DMA_CONFIG_TRY_BURST          ((1UL << 19))         // Try burst

#define DMA_STAT_CHAN(n)              ((1 << (n)) & 0xFF)
#define DMA_STAT_CHAN_ALL             (0xFF)

// Macro defines for DMA Configuration register

#define DMA_DMACONFIG_EN              (1 << 0)    // DMA Controller enable
#define DMA_DMACONFIG_AHB1_BIGENDIAN  (1 << 1)    // AHB Master endianness configuration
#define DMA_DMACONFIG_AHB2_BIGENDIAN  (1 << 2)    // AHB Master endianness configuration

#define DMA_DMACONFIG_TC_IRQ_EN(n)    (((n) & 0xFF) << 4)
#define DMA_DMACONFIG_TC_IRQ_EN_MASK  (0xFF << 4)
#define DMA_DMACONFIG_TC_IRQ_EN_SHIFT (4)

// Macro defines for DMA 2D Configuration registers

#define DMA_2D_MODIFY_SHIFT         11
#define DMA_2D_MODIFY_MASK          (0x1FFFFF << DMA_2D_MODIFY_SHIFT)
#define DMA_2D_MODIFY(n)            BITFIELD_VAL(DMA_2D_MODIFY, n)
#define DMA_2D_COUNT_SHIFT          0
#define DMA_2D_COUNT_MASK           (0x7FF << DMA_2D_COUNT_SHIFT)
#define DMA_2D_COUNT(n)             BITFIELD_VAL(DMA_2D_COUNT, n)

#define DMA_2D_CTRL_DST_EN          (1 << 1)
#define DMA_2D_CTRL_SRC_EN          (1 << 0)

#ifdef __cplusplus
}
#endif

#endif

