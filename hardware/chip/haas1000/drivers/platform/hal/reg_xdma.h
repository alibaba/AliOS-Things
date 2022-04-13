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

#define XDMA_NUMBER_CHANNELS 16

// DMA Channel register block structure
struct XDMA_CH_T {
    __IO uint32_t  DADR;                // 0x200+N*0x10 DMA Descriptor Address Registers 0-15
    __IO uint32_t  SADR;                // 0x204+N*0x10 DMA Source Address Registers 0-15
    __IO uint32_t  TADR;                // 0x208+N*0x10 DMA Target Address Registers 0-15
    __IO uint32_t  CMD;                 // 0x20C+N*0x10 DMA Command Registers 0-15
};

struct XDMA_CH_H_T {
    __IO uint32_t  DADRH;                 // 0x300+N*0x10 DMA Descriptor Address Higher Bits Registers 0-15
    __IO uint32_t  SADRH;                 // 0x304+N*0x10 DMA Source Address Higher Bits Registers 0-15
    __IO uint32_t  TADRH;                 // 0x308+N*0x10 DMA Target Address Higher Bits Registers 0-15
    __IO uint32_t  DBG;                 // 0x30C+N*0x10 DMA debug port
};

// DMA register block
struct XDMA_T {
    __IO  uint32_t  DCSR[XDMA_NUMBER_CHANNELS];    // Offset: 0x00-0x3C DMA Channel Control/Status Registers 0-15
          uint32_t  RESERVED0[24];                 // Offset: 0x40
    __IO  uint32_t  ALGN;                          // Offset: 0xA0 DMA Alignment Register
    __IO  uint32_t  PCSR;                          // Offset: 0xA4 DMA Programmed I/O Control Status Register
          uint32_t  RESERVED1[14];                 // Offset: 0xA8
    __IO  uint32_t  RQSR;                          // Offset: 0xE0 DREQ Status Register 0
          uint32_t  RESERVED2[3];                  // Offset: 0xE4
    __I   uint32_t  INTSTAT;                       // Offset: 0xF0 DMA Interrupt Register
          uint32_t  RESERVED3[3];                  // Offset: 0xF4
    __IO  uint32_t  RCMR[52];                      // Offset: 0x0100-0x01CC DMA Request to Channel Map Registers 0-51
          uint32_t  RESERVED4[12];                 // Offset: 0x01D0
    struct XDMA_CH_T CH[XDMA_NUMBER_CHANNELS];     // Offset: 0x200-0x2x0 DMA Descriptor Address Registers 0-15
    struct XDMA_CH_H_T CH_H[XDMA_NUMBER_CHANNELS]; // Offset: 0x300-0x3F0 DMA Descriptor Address Higher Bits Registers 0-15
};

//Offset: 0x00 - 0x3C
#define XDMA_CONFIG_RUN                               (((0x1) << 31))                      // software to start or stop the channel
#define XDMA_CONFIG_NODESCFETCH(n)                    ((((n) & 0x1) << 30))                // This bit controls whether or not a channel has a descriptor
#define XDMA_CONFIG_STOPIRQEN(n)                      ((((n) & 0x1) << 29))                // Stop interrupt enabled
#define XDMA_CONFIG_EORIRQEN(n)                       ((((n) & 0x1) << 28))                // Setting the End-of-Receive interrupt enable
#define XDMA_CONFIG_EORJMPEN(n)                       ((((n) & 0x1) << 27))                // Jump to the next descriptor on EOR
#define XDMA_CONFIG_EORSTOPEN(n)                      ((((n) & 0x1) << 26))                // Stop channel on EOR
#define XDMA_CONFIG_SETCMPST(n)                       ((((n) & 0x1) << 25))                // Set descriptor compare status
#define XDMA_CONFIG_CLRCMPST(n)                       ((((n) & 0x1) << 24))                // Clear descriptor compare status
#define XDMA_CONFIG_RASIRQEN(n)                       ((((n) & 0x1) << 23))                // Request after channel stopped interrupt enable
#define XDMA_CONFIG_MASKRUN(n)                        ((((n) & 0x1) << 22))                // Mask <Run> during a programmed I/O write to this register
#define XDMA_CONFIG_LPAE_EN                           ((0x1 << 21))                        // Long Physical Address Extension enable
#define XDMA_CONFIG_EORINT(n)                         ((((n) & 0x1) << 9))                 // End of Receive Interrupt
#define XDMA_CONFIG_RASINTR(n)                        ((((n) & 0x1) << 4))                 // Request after channel stopped
#define XDMA_STA_STOPINTR                             ((0x1 << 3))                         // Stop interrupt
#define XDMA_STA_ENDINTR                              ((0x1 << 2))                         // End interrupt
#define XDMA_STA_STARTINTR                            ((0x1 << 1))                         // Start interrupt
#define XDMA_STA_BUSERRINTR                           ((0x1 << 0))                         // Bus error interrupt

//Offset: 0xA0
#define XDMA_CONFIG_DALGN(n)                          ((0x1 << n))                         // Alignment control for channel x

//Offset: 0xE0
#define XDMA_CONFIG_CLR                               ((0x1 << 8))                         // Clearing pending request

//Offset: 0x0100-0x01CC
#define XDMA_CONFIG_MAPVLD(n)                         ((((n) & 0x1) << 7))                 // Map valid channel
#define XDMA_CONFIG_CHLNUM(n)                         ((((n) & 0x1F) << 0))                // Channel number

// Macro defines for DMA channel command registers

//Offset: 0x200-0x2x0
#define XDMA_CONFIG_DESCRIPTOR_ADDRESS(n)              ((((n) & 0xFFFFFFF) << 4))          // Contains address of next descriptor
#define XDMA_CONFIG_DESCRIPTOR_BREN(n)                 ((((n) & 0x1) << 1))                // Enable Descriptor Branch
#define XDMA_CONFIG_DESCRIPTOR_STOP(n)                 ((((n) & 0x1) << 0))                // Stop

//Offset: 0x204-0x2x4
#define XDMA_CONFIG_SRCADDR(n)                         ((((n) & 0x1FFFFFFF) << 3))         // Source address
#define XDMA_CONFIG_SRCADDR2(n)                        ((((n) & 0x1) << 2))                // 
#define XDMA_CONFIG_SRCADDR0(n)                        ((((n) & 0x3) << 0))                // 

//Offset: 0x208-0x2x8
#define XDMA_CONFIG_TRGADDR(n)                         ((((n) & 0x1FFFFFFF) << 3))         // Target address
#define XDMA_CONFIG_TRGADDR2(n)                        ((((n) & 0x1) << 2))                // 
#define XDMA_CONFIG_TRGADDR0(n)                        ((((n) & 0x3) << 0))                // 

//Offset: 0x20C-0x2xC
#define XDMA_CONTROL_SI                   ((1UL << 31))              // Source increment
#define XDMA_CONTROL_DI                   ((1UL << 30))              // Destination increment
#define XDMA_CONTROL_SFLOW                ((1UL << 29))              // Source flow control
#define XDMA_CONTROL_DFLOW                ((1UL << 28))              // Destination flow control
#define XDMA_CONTROL_DCMP_ENABLE          ((1UL << 25))              // Descriptor compare enable
#define XDMA_CONTROL_ADDRMODE(n)          ((((n) & 0x01) << 23))     // transfer width
#define XDMA_CONTROL_STARTIRQEN           ((1UL << 22))              // Start interrupt enable
#define XDMA_CONTROL_ENDIRQEN             ((1UL << 21))              // End interrupt enable
#define XDMA_CONTROL_MAXBSIZE(n)          ((((n) & 0x07) << 16))     // MAX burst size
#define XDMA_CONTROL_WIDTH(n)             ((((n) & 0x03) << 14))     // transfer width
#define XDMA_CONTROL_WIDTH_MASK           (0x03 << 14)
#define XDMA_CONTROL_WIDTH_SHIFT          (14)
#define XDMA_CONTROL_TRANSFERSIZE(n)      ((((n) & 0x1FFF) << 0))  // Transfer size
#define XDMA_CONTROL_TRANSFERSIZE_MASK    (0x1FFF << 0)
#define XDMA_CONTROL_TRANSFERSIZE_SHIFT   (0)

#define XDMA_CONTROL_DESCRIPTOR_ADDRESS_H(n)         ((((n) & 0xFF) << 0))     // contrain the higher 8 bits of memory address of the next descriptor
#define XDMA_CONTROL_SOURCE_ADDRESS_H(n)             ((((n) & 0xFF) << 0))     // contrain the source address higher bits [39:32] of the current descriptor
#define XDMA_CONTROL_TARGET_ADDRESS_H(n)             ((((n) & 0xFF) << 0))     // contrain the target address higher bits [39:32] of the current descriptor

/*
#define XDMA_CONTROL_SRCAHB1           0
#define XDMA_CONTROL_DSTAHB1           0
#define XDMA_CONTROL_PROT1             ((1UL << 28))              // Indicates that the access is in user mode or privileged mode
#define XDMA_CONTROL_PROT2             ((1UL << 29))              // Indicates that the access is bufferable or not bufferable
#define XDMA_CONTROL_PROT3             ((1UL << 30))              // Indicates that the access is cacheable or not cacheable
#define XDMA_CONTROL_TC_IRQ            ((1UL << 31))              // Terminal count interrupt enable bit

// Macro defines for DMA Channel Configuration registers

#define XDMA_CONFIG_EN                 ((1UL << 0))          // DMA control enable
#define XDMA_CONFIG_SRCPERIPH(n)       ((((n) & 0x1F) << 1)) // Source peripheral
#define XDMA_CONFIG_DSTPERIPH(n)       ((((n) & 0x1F) << 6)) // Destination peripheral
#define XDMA_CONFIG_TRANSFERTYPE(n)    ((((n) & 0x7) << 11)) // This value indicates the type of transfer
#define XDMA_CONFIG_ERR_IRQMASK        ((1UL << 14))         // Interrupt error mask
#define XDMA_CONFIG_TC_IRQMASK         ((1UL << 15))         // Terminal count interrupt mask
#define XDMA_CONFIG_LOCK               ((1UL << 16))         // Lock
#define XDMA_CONFIG_ACTIVE             ((1UL << 17))         // Active
#define XDMA_CONFIG_HALT               ((1UL << 18))         // Halt
#define XDMA_CONFIG_TRY_BURST          ((1UL << 19))         // Try burst
*/

#define XDMA_INTSTA_CHAN                 (0xF)
#define XDMA_ERRINT_CHAN                 (0x1)
#define XDMA_STAT_CHAN_ALL               (0xFF)

// Macro defines for DMA Configuration register

#define XDMA_DMACONFIG_EN              (1 << 0)    // DMA Controller enable
#define XDMA_DMACONFIG_AHB1_BIGENDIAN  (1 << 1)    // AHB Master endianness configuration
#define XDMA_DMACONFIG_AHB2_BIGENDIAN  (1 << 2)    // AHB Master endianness configuration

#define XDMA_DMACONFIG_TC_IRQ_EN(n)    (((n) & 0xFF) << 4)
#define XDMA_DMACONFIG_TC_IRQ_EN_MASK  (0xFF << 4)
#define XDMA_DMACONFIG_TC_IRQ_EN_SHIFT (4)

#ifdef __cplusplus
}
#endif

#endif

