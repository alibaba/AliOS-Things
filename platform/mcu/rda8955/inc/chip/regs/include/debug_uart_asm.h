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


#ifndef _DEBUG_UART_ASM_H_
#define _DEBUG_UART_ASM_H_

//THIS FILE HAS BEEN GENERATED WITH COOLWATCHER. PLEASE EDIT WITH CARE !

#ifndef CT_ASM
#error "You are trying to use in a normal C code the assembly H description of 'debug_uart'."
#endif


#define DEBUG_UART_RX_FIFO_SIZE                 (16)
#define DEBUG_UART_TX_FIFO_SIZE                 (16)
#define DEBUG_UART_NB_RX_FIFO_BITS              (4)
#define DEBUG_UART_NB_TX_FIFO_BITS              (4)
#define ESC_DAT                                 (92)

//==============================================================================
// debug_uart
//------------------------------------------------------------------------------
///
//==============================================================================
#define REG_DEBUG_UART_BASE        0x01A14000

#define REG_DEBUG_UART_BASE_HI     BASE_HI(REG_DEBUG_UART_BASE)
#define REG_DEBUG_UART_BASE_LO     BASE_LO(REG_DEBUG_UART_BASE)

#define REG_DEBUG_UART_CTRL        REG_DEBUG_UART_BASE_LO + 0x00000000
#define REG_DEBUG_UART_STATUS      REG_DEBUG_UART_BASE_LO + 0x00000004
#define REG_DEBUG_UART_RXTX_BUFFER REG_DEBUG_UART_BASE_LO + 0x00000008
#define REG_DEBUG_UART_IRQ_MASK    REG_DEBUG_UART_BASE_LO + 0x0000000C
#define REG_DEBUG_UART_IRQ_CAUSE   REG_DEBUG_UART_BASE_LO + 0x00000010
#define REG_DEBUG_UART_TRIGGERS    REG_DEBUG_UART_BASE_LO + 0x00000014
#define REG_DEBUG_UART_XCHAR       REG_DEBUG_UART_BASE_LO + 0x00000018

//ctrl
#define DEBUG_UART_ENABLE          (1<<0)
#define DEBUG_UART_ENABLE_DISABLE  (0<<0)
#define DEBUG_UART_ENABLE_ENABLE   (1<<0)
#define DEBUG_UART_DATA_BITS       (1<<1)
#define DEBUG_UART_DATA_BITS_7_BITS (0<<1)
#define DEBUG_UART_DATA_BITS_8_BITS (1<<1)
#define DEBUG_UART_TX_STOP_BITS    (1<<2)
#define DEBUG_UART_TX_STOP_BITS_1_BIT (0<<2)
#define DEBUG_UART_TX_STOP_BITS_2_BITS (1<<2)
#define DEBUG_UART_PARITY_ENABLE   (1<<3)
#define DEBUG_UART_PARITY_ENABLE_NO (0<<3)
#define DEBUG_UART_PARITY_ENABLE_YES (1<<3)
#define DEBUG_UART_PARITY_SELECT(n) (((n)&3)<<4)
#define DEBUG_UART_PARITY_SELECT_ODD (0<<4)
#define DEBUG_UART_PARITY_SELECT_EVEN (1<<4)
#define DEBUG_UART_PARITY_SELECT_SPACE (2<<4)
#define DEBUG_UART_PARITY_SELECT_MARK (3<<4)
#define DEBUG_UART_TX_BREAK_CONTROL (1<<6)
#define DEBUG_UART_TX_BREAK_CONTROL_OFF (0<<6)
#define DEBUG_UART_TX_BREAK_CONTROL_ON (1<<6)
#define DEBUG_UART_RX_FIFO_RESET   (1<<7)
#define DEBUG_UART_TX_FIFO_RESET   (1<<8)
#define DEBUG_UART_DMA_MODE        (1<<9)
#define DEBUG_UART_DMA_MODE_DISABLE (0<<9)
#define DEBUG_UART_DMA_MODE_ENABLE (1<<9)
#define DEBUG_UART_SWRX_FLOW_CTRL(n) (((n)&3)<<12)
#define DEBUG_UART_SWRX_FLOW_CTRL_MASK (3<<12)
#define DEBUG_UART_SWRX_FLOW_CTRL_SHIFT (12)
#define DEBUG_UART_SWTX_FLOW_CTRL(n) (((n)&3)<<14)
#define DEBUG_UART_SWTX_FLOW_CTRL_MASK (3<<14)
#define DEBUG_UART_SWTX_FLOW_CTRL_SHIFT (14)
#define DEBUG_UART_BACKSLASH_EN    (1<<16)
#define DEBUG_UART_TX_FINISH_N_WAIT (1<<19)
#define DEBUG_UART_DIVISOR_MODE    (1<<20)
#define DEBUG_UART_IRDA_ENABLE     (1<<21)
#define DEBUG_UART_RX_RTS          (1<<22)
#define DEBUG_UART_RX_RTS_INACTIVE (0<<22)
#define DEBUG_UART_RX_RTS_ACTIVE   (1<<22)
#define DEBUG_UART_AUTO_FLOW_CONTROL (1<<23)
#define DEBUG_UART_AUTO_FLOW_CONTROL_ENABLE (1<<23)
#define DEBUG_UART_AUTO_FLOW_CONTROL_DISABLE (0<<23)
#define DEBUG_UART_LOOP_BACK_MODE  (1<<24)
#define DEBUG_UART_RX_LOCK_ERR     (1<<25)
#define DEBUG_UART_RX_LOCK_ERR_DISABLE (0<<25)
#define DEBUG_UART_RX_LOCK_ERR_ENABLE (1<<25)
#define DEBUG_UART_HST_TXD_OEN     (1<<26)
#define DEBUG_UART_HST_TXD_OEN_DISABLE (1<<26)
#define DEBUG_UART_HST_TXD_OEN_ENABLE (0<<26)
#define DEBUG_UART_RX_BREAK_LENGTH(n) (((n)&15)<<28)

//status
#define DEBUG_UART_RX_FIFO_LEVEL(n) (((n)&31)<<0)
#define DEBUG_UART_RX_FIFO_LEVEL_MASK (31<<0)
#define DEBUG_UART_RX_FIFO_LEVEL_SHIFT (0)
#define DEBUG_UART_TX_FIFO_LEVEL(n) (((n)&31)<<8)
#define DEBUG_UART_TX_FIFO_LEVEL_MASK (31<<8)
#define DEBUG_UART_TX_FIFO_LEVEL_SHIFT (8)
#define DEBUG_UART_TX_ACTIVE       (1<<13)
#define DEBUG_UART_RX_ACTIVE       (1<<14)
#define DEBUG_UART_RX_OVERFLOW_ERR (1<<16)
#define DEBUG_UART_TX_OVERFLOW_ERR (1<<17)
#define DEBUG_UART_RX_PARITY_ERR   (1<<18)
#define DEBUG_UART_RX_FRAMING_ERR  (1<<19)
#define DEBUG_UART_RX_BREAK_INT    (1<<20)
#define DEBUG_UART_TX_DCTS         (1<<24)
#define DEBUG_UART_TX_CTS          (1<<25)
#define DEBUG_UART_TX_FIFO_RSTED_L (1<<28)
#define DEBUG_UART_RX_FIFO_RSTED_L (1<<29)
#define DEBUG_UART_ENABLE_N_FINISHED (1<<30)
#define DEBUG_UART_CLK_ENABLED     (1<<31)

//rxtx_buffer
#define DEBUG_UART_RX_DATA(n)      (((n)&0xFF)<<0)
#define DEBUG_UART_TX_DATA(n)      (((n)&0xFF)<<0)

//irq_mask
#define DEBUG_UART_TX_MODEM_STATUS (1<<0)
#define DEBUG_UART_RX_DATA_AVAILABLE (1<<1)
#define DEBUG_UART_TX_DATA_NEEDED  (1<<2)
#define DEBUG_UART_RX_TIMEOUT      (1<<3)
#define DEBUG_UART_RX_LINE_ERR     (1<<4)
#define DEBUG_UART_TX_DMA_DONE     (1<<5)
#define DEBUG_UART_RX_DMA_DONE     (1<<6)
#define DEBUG_UART_RX_DMA_TIMEOUT  (1<<7)
#define DEBUG_UART_XOFF_DETECTED   (1<<8)

//irq_cause
//#define DEBUG_UART_TX_MODEM_STATUS (1<<0)
//#define DEBUG_UART_RX_DATA_AVAILABLE (1<<1)
//#define DEBUG_UART_TX_DATA_NEEDED (1<<2)
//#define DEBUG_UART_RX_TIMEOUT    (1<<3)
//#define DEBUG_UART_RX_LINE_ERR   (1<<4)
//#define DEBUG_UART_TX_DMA_DONE   (1<<5)
//#define DEBUG_UART_RX_DMA_DONE   (1<<6)
//#define DEBUG_UART_RX_DMA_TIMEOUT (1<<7)
#define DEBUG_UART_TX_MODEM_STATUS_U (1<<16)
#define DEBUG_UART_RX_DATA_AVAILABLE_U (1<<17)
#define DEBUG_UART_TX_DATA_NEEDED_U (1<<18)
#define DEBUG_UART_RX_TIMEOUT_U    (1<<19)
#define DEBUG_UART_RX_LINE_ERR_U   (1<<20)
#define DEBUG_UART_TX_DMA_DONE_U   (1<<21)
#define DEBUG_UART_RX_DMA_DONE_U   (1<<22)
#define DEBUG_UART_RX_DMA_TIMEOUT_U (1<<23)

//triggers
#define DEBUG_UART_RX_TRIGGER(n)   (((n)&15)<<0)
#define DEBUG_UART_TX_TRIGGER(n)   (((n)&15)<<4)
#define DEBUG_UART_AFC_LEVEL(n)    (((n)&15)<<8)

//XChar
#define DEBUG_UART_XON1(n)         (((n)&0xFF)<<0)
#define DEBUG_UART_XOFF1(n)        (((n)&0xFF)<<8)
#define DEBUG_UART_XON2(n)         (((n)&0xFF)<<16)
#define DEBUG_UART_XOFF2(n)        (((n)&0xFF)<<24)




#endif
