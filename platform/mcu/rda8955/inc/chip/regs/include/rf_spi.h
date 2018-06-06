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


#ifndef _RF_SPI_H_
#define _RF_SPI_H_

#ifdef CT_ASM
#error "You are trying to use in an assembly code the normal H description of 'rf_spi'."
#endif



// =============================================================================
//  MACROS
// =============================================================================
#define CMD_FIFO_LEN_BITS                       (5)
#define CMD_FIFO_LEN                            (20)
#define CMD_SIZE_BITS                           (8)
#define CMD_DATA_FIFO_LEN_BITS                  (4)
#define CMD_DATA_FIFO_LEN                       (EXP2(CMD_DATA_FIFO_LEN_BITS))
#define GAIN_TABLE_LEN_BITS                     (4)
#define GAIN_TABLE_LEN                          (15)
#define GAIN_SIZE_BITS                          (4)
#define RX_DATA_FIFO_LEN_BITS                   (2)
#define RX_DATA_FIFO_LEN                        (EXP2(RX_DATA_FIFO_LEN_BITS))

// =============================================================================
//  TYPES
// =============================================================================

// ============================================================================
// RF_SPI_T
// -----------------------------------------------------------------------------
///
// =============================================================================
#define REG_RF_SPI_BASE            0x01A0E000

typedef volatile struct
{
    REG32                          Ctrl;                         //0x00000000
    REG32                          Status;                       //0x00000004
    REG32                          Rx_Data;                      //0x00000008
    REG32                          Command;                      //0x0000000C
    REG32                          Cmd_Size;                     //0x00000010
    REG32                          Cmd_Data;                     //0x00000014
    REG32                          Gain_Size;                    //0x00000018
    REG32                          Gain_Data;                    //0x0000001C
    REG32                          IRQ;                          //0x00000020
    REG32                          IRQ_Mask;                     //0x00000024
    REG32                          IRQ_Threshold;                //0x00000028
    REG32                          Divider;                      //0x0000002C
} HWP_RF_SPI_T;

#define hwp_rfSpi                  ((HWP_RF_SPI_T*) KSEG1(REG_RF_SPI_BASE))


//Ctrl
#define RF_SPI_ENABLE              (1<<0)
#define RF_SPI_CS_POLARITY         (1<<1)
#define RF_SPI_DIGRF_READ          (1<<2)
#define RF_SPI_CLOCKED_BACK2BACK   (1<<3)
#define RF_SPI_INPUT_MODE          (1<<4)
#define RF_SPI_CLOCK_POLARITY      (1<<5)
#define RF_SPI_CLOCK_DELAY(n)      (((n)&3)<<6)
#define RF_SPI_DO_DELAY(n)         (((n)&3)<<8)
#define RF_SPI_DI_DELAY(n)         (((n)&3)<<10)
#define RF_SPI_CS_DELAY(n)         (((n)&3)<<12)
#define RF_SPI_CS_END_HOLD(n)      (((n)&3)<<14)
#define RF_SPI_FRAME_SIZE(n)       (((n)&31)<<16)
#define RF_SPI_FRAME_SIZE_MASK     (31<<16)
#define RF_SPI_FRAME_SIZE_SHIFT    (16)
#define RF_SPI_CS_END_PULSE(n)     (((n)&3)<<22)
#define RF_SPI_INPUT_FRAME_SIZE(n) (((n)&31)<<24)
#define RF_SPI_INPUT_FRAME_SIZE_MASK (31<<24)
#define RF_SPI_INPUT_FRAME_SIZE_SHIFT (24)
#define RF_SPI_TURNAROUND_TIME(n)  (((n)&3)<<30)

//Status
#define RF_SPI_ACTIVE_STATUS       (1<<0)
#define RF_SPI_ERROR_CMD           (1<<1)
#define RF_SPI_TABLE_OVF           (1<<6)
#define RF_SPI_TABLE_UDF           (1<<7)
#define RF_SPI_CMD_LEVEL(n)        (((n)&31)<<8)
#define RF_SPI_CMD_LEVEL_MASK      (31<<8)
#define RF_SPI_CMD_LEVEL_SHIFT     (8)
#define RF_SPI_CMD_OVF             (1<<14)
#define RF_SPI_CMD_UDF             (1<<15)
#define RF_SPI_CMD_DATA_LEVEL(n)   (((n)&31)<<16)
#define RF_SPI_CMD_DATA_LEVEL_MASK (31<<16)
#define RF_SPI_CMD_DATA_LEVEL_SHIFT (16)
#define RF_SPI_CMD_DATA_OVF        (1<<22)
#define RF_SPI_CMD_DATA_UDF        (1<<23)
#define RF_SPI_RX_LEVEL(n)         (((n)&7)<<24)
#define RF_SPI_RX_LEVEL_MASK       (7<<24)
#define RF_SPI_RX_LEVEL_SHIFT      (24)
#define RF_SPI_RX_OVF              (1<<30)
#define RF_SPI_RX_UDF              (1<<31)

//Rx_Data
#define RF_SPI_RX_DATA(n)          (((n)&0xFF)<<0)

//Command
#define RF_SPI_SEND_CMD            (1<<0)
#define RF_SPI_FLUSH_CMD_FIFO      (1<<8)
#define RF_SPI_FLUSH_RX_FIFO       (1<<16)
#define RF_SPI_RESTART_GAIN        (1<<24)
#define RF_SPI_RELOAD_GAIN         (1<<28)
#define RF_SPI_DRIVE_ZERO          (1<<31)

//Cmd_Size
#define RF_SPI_CMD_SIZE(n)         (((n)&0xFF)<<0)
#define RF_SPI_CMD_MARK            (1<<31)

//Cmd_Data
#define RF_SPI_CMD_DATA(n)         (((n)&0xFF)<<0)

//Gain_Size
#define RF_SPI_GAIN_SIZE(n)        (((n)&15)<<0)

//Gain_Data
#define RF_SPI_GAIN_DATA(n)        (((n)&0xFF)<<0)

//IRQ
#define RF_SPI_CMD_DATA_DMA_DONE_CAUSE (1<<0)
#define RF_SPI_CMD_FIFO_EMPTY_CAUSE (1<<2)
#define RF_SPI_CMD_THRESHOLD_CAUSE (1<<3)
#define RF_SPI_RX_FIFO_FULL_CAUSE  (1<<4)
#define RF_SPI_RX_THRESHOLD_CAUSE  (1<<5)
#define RF_SPI_ERROR_CAUSE         (1<<6)
#define RF_SPI_CMD_DATA_DMA_DONE_STATUS (1<<16)
#define RF_SPI_CMD_FIFO_EMPTY_STATUS (1<<18)
#define RF_SPI_CMD_THRESHOLD_STATUS (1<<19)
#define RF_SPI_RX_FIFO_FULL_STATUS (1<<20)
#define RF_SPI_RX_THRESHOLD_STATUS (1<<21)
#define RF_SPI_ERROR_STATUS        (1<<22)
#define RF_SPI_ALL_CAUSE(n)        (((n)&0x7D)<<0)
#define RF_SPI_ALL_CAUSE_MASK      (0x7D<<0)
#define RF_SPI_ALL_CAUSE_SHIFT     (0)
#define RF_SPI_ALL_STATUS(n)       (((n)&0x7D)<<16)
#define RF_SPI_ALL_STATUS_MASK     (0x7D<<16)
#define RF_SPI_ALL_STATUS_SHIFT    (16)

//IRQ_Mask
#define RF_SPI_CMD_DATA_DMA_DONE_MASK (1<<0)
#define RF_SPI_CMD_FIFO_EMPTY_MASK (1<<2)
#define RF_SPI_CMD_THRESHOLD_MASK  (1<<3)
#define RF_SPI_RX_FIFO_FULL_MASK   (1<<4)
#define RF_SPI_RX_THRESHOLD_MASK   (1<<5)
#define RF_SPI_ERROR_MASK          (1<<6)
#define RF_SPI_ALL_MASK(n)         (((n)&0x7D)<<0)
#define RF_SPI_ALL_MASK_MASK       (0x7D<<0)
#define RF_SPI_ALL_MASK_SHIFT      (0)

//IRQ_Threshold
#define RF_SPI_CMD_THRESHOLD(n)    (((n)&31)<<8)
#define RF_SPI_RX_THRESHOLD(n)     (((n)&3)<<24)

//Divider
#define RF_SPI_DIVIDER(n)          (((n)&0x3F)<<1)
#define RF_SPI_DIVIDER_MASK        (0x3F<<1)
#define RF_SPI_DIVIDER_SHIFT       (1)
#define RF_SPI_CLOCK_LIMITER       (1<<28)
#define RF_SPI_CLOCK_LIMITER_MASK  (1<<28)
#define RF_SPI_CLOCK_LIMITER_SHIFT (28)





#endif

