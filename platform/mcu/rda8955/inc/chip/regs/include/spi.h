/* Copyright (C) 2016 RDA Technologies Limited and/or its affiliates("RDA").
* All rights reserved.
*
* This software is supplied "AS IS" without any warranties.
* RDA assumes no responsibility or liability for the use of the software,
* conveys no license or title under any patent, copyright, or mask work
* right to the product. RDA reserves the right to make changes in the
* software without notification.  RDA also make no representation or
* warranty that such application will be suitable for the specified use
* without further testing or modification.
*/


#ifndef _SPI_H_
#define _SPI_H_

#ifdef CT_ASM
#error "You are trying to use in an assembly code the normal H description of 'spi'."
#endif



// =============================================================================
//  MACROS
// =============================================================================
#define SPI_TX_FIFO_SIZE                        (16)
#define SPI_RX_FIFO_SIZE                        (16)

// =============================================================================
//  TYPES
// =============================================================================

// ============================================================================
// SPI_T
// -----------------------------------------------------------------------------
///
// =============================================================================
#define REG_SPI1_BASE              0x01A11000
#define REG_SPI2_BASE              0x01A12000
#define REG_SPI3_BASE              0x01A13000

typedef volatile struct
{
    REG32                          ctrl;                         //0x00000000
    REG32                          status;                       //0x00000004
    /// Spi1 fifo size (rxtx_buffer): 8bits.
    /// Spi2 fifo size (rxtx_buffer): 8bits.
    /// Spi3 fifo size (rxtx_buffer): 32bits.
    ///
    REG32                          rxtx_buffer;                  //0x00000008
    REG32                          cfg;                          //0x0000000C
    REG32                          pattern;                      //0x00000010
    REG32                          stream;                       //0x00000014
    REG32                          pin_control;                  //0x00000018
    REG32                          irq;                          //0x0000001C
} HWP_SPI_T;

#define hwp_spi1                   ((HWP_SPI_T*) KSEG1(REG_SPI1_BASE))
#define hwp_spi2                   ((HWP_SPI_T*) KSEG1(REG_SPI2_BASE))
#define hwp_spi3                   ((HWP_SPI_T*) KSEG1(REG_SPI3_BASE))


//ctrl
#define SPI_ENABLE                 (1<<0)
#define SPI_CS_SEL(n)              (((n)&3)<<1)
#define SPI_CS_SEL_MASK            (3<<1)
#define SPI_CS_SEL_SHIFT           (1)
#define SPI_CS_SEL_CS0             (0<<1)
#define SPI_CS_SEL_CS1             (1<<1)
#define SPI_CS_SEL_CS2             (2<<1)
#define SPI_CS_SEL_CS3             (3<<1)
#define SPI_INPUT_MODE             (1<<4)
#define SPI_CLOCK_POLARITY         (1<<5)
#define SPI_CLOCK_DELAY(n)         (((n)&3)<<6)
#define SPI_DO_DELAY(n)            (((n)&3)<<8)
#define SPI_DI_DELAY(n)            (((n)&3)<<10)
#define SPI_CS_DELAY(n)            (((n)&3)<<12)
#define SPI_CS_PULSE(n)            (((n)&3)<<14)
#define SPI_FRAME_SIZE(n)          (((n)&31)<<16)
#define SPI_OE_DELAY(n)            (((n)&31)<<24)
#define SPI_CTRL_DATA_MUX_SEL      (1<<29)
#define SPI_CTRL_DATA_MUX_SEL_MASK (1<<29)
#define SPI_CTRL_DATA_MUX_SEL_SHIFT (29)
#define SPI_CTRL_DATA_MUX_SEL_CTRL_REG_SEL (0<<29)
#define SPI_CTRL_DATA_MUX_SEL_DATA_REG_SEL (1<<29)
#define SPI_INPUT_SEL(n)           (((n)&3)<<30)

//status
#define SPI_ACTIVE_STATUS          (1<<0)
#define SPI_CAUSE_RX_OVF_IRQ       (1<<3)
#define SPI_CAUSE_TX_TH_IRQ        (1<<4)
#define SPI_CAUSE_TX_DMA_IRQ       (1<<5)
#define SPI_CAUSE_RX_TH_IRQ        (1<<6)
#define SPI_CAUSE_RX_DMA_IRQ       (1<<7)
#define SPI_TX_OVF                 (1<<9)
#define SPI_RX_UDF                 (1<<10)
#define SPI_RX_OVF                 (1<<11)
#define SPI_TX_TH                  (1<<12)
#define SPI_TX_DMA_DONE            (1<<13)
#define SPI_RX_TH                  (1<<14)
#define SPI_RX_DMA_DONE            (1<<15)
#define SPI_TX_SPACE(n)            (((n)&31)<<16)
#define SPI_TX_SPACE_MASK          (31<<16)
#define SPI_TX_SPACE_SHIFT         (16)
#define SPI_RX_LEVEL(n)            (((n)&31)<<24)
#define SPI_RX_LEVEL_MASK          (31<<24)
#define SPI_RX_LEVEL_SHIFT         (24)
#define SPI_FIFO_FLUSH             (1<<30)
#define SPI_IRQ_CAUSE(n)           (((n)&31)<<3)
#define SPI_IRQ_CAUSE_MASK         (31<<3)
#define SPI_IRQ_CAUSE_SHIFT        (3)

//rxtx_buffer
#define SPI_DATA_IN(n)             (((n)&0x1FFFFFFF)<<0)
#define SPI_DATA_OUT(n)            (((n)&0x1FFFFFFF)<<0)
#define SPI_CS(n)                  (((n)&3)<<29)
#define SPI_CS_MASK                (3<<29)
#define SPI_CS_SHIFT               (29)
#define SPI_READ_ENA               (1<<31)
#define SPI_READ_ENA_MASK          (1<<31)
#define SPI_READ_ENA_SHIFT         (31)

//cfg
#define SPI_CS_POLARITY_0          (1<<0)
#define SPI_CS_POLARITY_0_MASK     (1<<0)
#define SPI_CS_POLARITY_0_SHIFT    (0)
#define SPI_CS_POLARITY_0_ACTIVE_HIGH (0<<0)
#define SPI_CS_POLARITY_0_ACTIVE_LOW (1<<0)
#define SPI_CS_POLARITY_1          (1<<1)
#define SPI_CS_POLARITY_1_MASK     (1<<1)
#define SPI_CS_POLARITY_1_SHIFT    (1)
#define SPI_CS_POLARITY_1_ACTIVE_HIGH (0<<1)
#define SPI_CS_POLARITY_1_ACTIVE_LOW (1<<1)
#define SPI_CS_POLARITY_2          (1<<2)
#define SPI_CS_POLARITY_2_MASK     (1<<2)
#define SPI_CS_POLARITY_2_SHIFT    (2)
#define SPI_CS_POLARITY_2_ACTIVE_HIGH (0<<2)
#define SPI_CS_POLARITY_2_ACTIVE_LOW (1<<2)
#define SPI_CLOCK_DIVIDER(n)       (((n)&0x3FF)<<16)
#define SPI_CLOCK_DIVIDER_MASK     (0x3FF<<16)
#define SPI_CLOCK_DIVIDER_SHIFT    (16)
#define SPI_CLOCK_LIMITER          (1<<28)
#define SPI_CLOCK_LIMITER_MASK     (1<<28)
#define SPI_CLOCK_LIMITER_SHIFT    (28)
#define SPI_CS_POLARITY(n)         (((n)&7)<<0)
#define SPI_CS_POLARITY_MASK       (7<<0)
#define SPI_CS_POLARITY_SHIFT      (0)

//pattern
#define SPI_PATTERN(n)             (((n)&0xFF)<<0)
#define SPI_PATTERN_MODE           (1<<8)
#define SPI_PATTERN_MODE_MASK      (1<<8)
#define SPI_PATTERN_MODE_SHIFT     (8)
#define SPI_PATTERN_MODE_DISABLED  (0<<8)
#define SPI_PATTERN_MODE_ENABLED   (1<<8)
#define SPI_PATTERN_SELECTOR       (1<<9)
#define SPI_PATTERN_SELECTOR_MASK  (1<<9)
#define SPI_PATTERN_SELECTOR_SHIFT (9)
#define SPI_PATTERN_SELECTOR_UNTIL (0<<9)
#define SPI_PATTERN_SELECTOR_WHILE (1<<9)

//stream
#define SPI_TX_STREAM_BIT          (1<<0)
#define SPI_TX_STREAM_BIT_MASK     (1<<0)
#define SPI_TX_STREAM_BIT_SHIFT    (0)
#define SPI_TX_STREAM_BIT_ZERO     (0<<0)
#define SPI_TX_STREAM_BIT_ONE      (1<<0)
#define SPI_TX_STREAM_MODE         (1<<8)
#define SPI_TX_STREAM_MODE_MASK    (1<<8)
#define SPI_TX_STREAM_MODE_SHIFT   (8)
#define SPI_TX_STREAM_MODE_DISABLED (0<<8)
#define SPI_TX_STREAM_MODE_ENABLED (1<<8)
#define SPI_TX_STREAM_STOP_WITH_RX_DMA_DONE (1<<16)
#define SPI_TX_STREAM_STOP_WITH_RX_DMA_DONE_MASK (1<<16)
#define SPI_TX_STREAM_STOP_WITH_RX_DMA_DONE_SHIFT (16)
#define SPI_TX_STREAM_STOP_WITH_RX_DMA_DONE_DISABLED (0<<16)
#define SPI_TX_STREAM_STOP_WITH_RX_DMA_DONE_ENABLED (1<<16)

//pin_control
#define SPI_CLK_CTRL(n)            (((n)&3)<<0)
#define SPI_CLK_CTRL_MASK          (3<<0)
#define SPI_CLK_CTRL_SHIFT         (0)
#define SPI_CLK_CTRL_SPI_CTRL      (0<<0)
#define SPI_CLK_CTRL_INPUT_CTRL    (1<<0)
#define SPI_CLK_CTRL_FORCE_0_CTRL  (2<<0)
#define SPI_CLK_CTRL_FORCE_1_CTRL  (3<<0)
#define SPI_DO_CTRL(n)             (((n)&3)<<2)
#define SPI_DO_CTRL_MASK           (3<<2)
#define SPI_DO_CTRL_SHIFT          (2)
#define SPI_DO_CTRL_SPI_CTRL       (0<<2)
#define SPI_DO_CTRL_INPUT_CTRL     (1<<2)
#define SPI_DO_CTRL_FORCE_0_CTRL   (2<<2)
#define SPI_DO_CTRL_FORCE_1_CTRL   (3<<2)
#define SPI_CS0_CTRL(n)            (((n)&3)<<4)
#define SPI_CS0_CTRL_MASK          (3<<4)
#define SPI_CS0_CTRL_SHIFT         (4)
#define SPI_CS0_CTRL_SPI_CTRL      (0<<4)
#define SPI_CS0_CTRL_INPUT_CTRL    (1<<4)
#define SPI_CS0_CTRL_FORCE_0_CTRL  (2<<4)
#define SPI_CS0_CTRL_FORCE_1_CTRL  (3<<4)
#define SPI_CS1_CTRL(n)            (((n)&3)<<6)
#define SPI_CS1_CTRL_MASK          (3<<6)
#define SPI_CS1_CTRL_SHIFT         (6)
#define SPI_CS1_CTRL_SPI_CTRL      (0<<6)
#define SPI_CS1_CTRL_INPUT_CTRL    (1<<6)
#define SPI_CS1_CTRL_FORCE_0_CTRL  (2<<6)
#define SPI_CS1_CTRL_FORCE_1_CTRL  (3<<6)
#define SPI_CS2_CTRL(n)            (((n)&3)<<8)
#define SPI_CS2_CTRL_MASK          (3<<8)
#define SPI_CS2_CTRL_SHIFT         (8)
#define SPI_CS2_CTRL_SPI_CTRL      (0<<8)
#define SPI_CS2_CTRL_INPUT_CTRL    (1<<8)
#define SPI_CS2_CTRL_FORCE_0_CTRL  (2<<8)
#define SPI_CS2_CTRL_FORCE_1_CTRL  (3<<8)

//irq
#define SPI_MASK_RX_OVF_IRQ        (1<<0)
#define SPI_MASK_TX_TH_IRQ         (1<<1)
#define SPI_MASK_TX_DMA_IRQ        (1<<2)
#define SPI_MASK_RX_TH_IRQ         (1<<3)
#define SPI_MASK_RX_DMA_IRQ        (1<<4)
#define SPI_TX_THRESHOLD(n)        (((n)&3)<<5)
#define SPI_TX_THRESHOLD_MASK      (3<<5)
#define SPI_TX_THRESHOLD_SHIFT     (5)
#define SPI_TX_THRESHOLD_1_EMPTY_SLOT (0<<5)
#define SPI_TX_THRESHOLD_4_EMPTY_SLOTS (1<<5)
#define SPI_TX_THRESHOLD_8_EMPTY_SLOTS (2<<5)
#define SPI_TX_THRESHOLD_12_EMPTY_SLOTS (3<<5)
#define SPI_RX_THRESHOLD(n)        (((n)&3)<<7)
#define SPI_RX_THRESHOLD_MASK      (3<<7)
#define SPI_RX_THRESHOLD_SHIFT     (7)
#define SPI_RX_THRESHOLD_1_VALID_DATA (0<<7)
#define SPI_RX_THRESHOLD_4_VALID_DATA (1<<7)
#define SPI_RX_THRESHOLD_8_VALID_DATA (2<<7)
#define SPI_RX_THRESHOLD_12_VALID_DATA (3<<7)
#define SPI_IRQ_MASK(n)            (((n)&31)<<0)
#define SPI_IRQ_MASK_MASK          (31<<0)
#define SPI_IRQ_MASK_SHIFT         (0)





#endif

