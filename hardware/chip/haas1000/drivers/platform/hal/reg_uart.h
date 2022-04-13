/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __REG_UART_H__
#define __REG_UART_H__

#include "plat_types.h"

// UART Registers
struct UART_T {
    __IO uint32_t UARTDR;           // 0x000
    union {
        __I  uint32_t UARTRSR;      // 0x004
        __O  uint32_t UARTECR;      // 0x004
    };
    uint32_t RESERVED_008[4];       // 0x008
    __I  uint32_t UARTFR;           // 0x018
    uint32_t RESERVED_01C;          // 0x01C
    __IO uint32_t UARTILPR;         // 0x020
    __IO uint32_t UARTIBRD;         // 0x024
    __IO uint32_t UARTFBRD;         // 0x028
    __IO uint32_t UARTLCR_H;        // 0x02C
    __IO uint32_t UARTCR;           // 0x030
    __IO uint32_t UARTIFLS;         // 0x034
    __IO uint32_t UARTIMSC;         // 0x038
    __I  uint32_t UARTRIS;          // 0x03C
    __I  uint32_t UARTMIS;          // 0x040
    __O  uint32_t UARTICR;          // 0x044
    __IO uint32_t UARTDMACR;        // 0x048
    uint32_t RESERVED_04C[997];     // 0x04C
    __I  uint32_t UARTPID0;         // 0xFE0
    __I  uint32_t UARTPID1;         // 0xFE4
    __I  uint32_t UARTPID2;         // 0xFE8
    __I  uint32_t UARTPID3;         // 0xFEC
    __I  uint32_t UARTPCID0;        // 0xFF0
    __I  uint32_t UARTPCID1;        // 0xFF4
    __I  uint32_t UARTPCID2;        // 0xFF8
    __I  uint32_t UARTPCID3;        // 0xFFC
};

// Data status bits
#define UART_DATA_ERROR_MASK      0x0F00

// Status reg bits
#define UART_STATUS_ERROR_MASK    0x0F

// Flag reg bits
#define UARTFR_RI           (1 << 8)  // Ring indicator
#define UARTFR_TXFE         (1 << 7)  // Transmit FIFO empty
#define UARTFR_RXFF         (1 << 6)  // Receive  FIFO full
#define UARTFR_TXFF         (1 << 5)  // Transmit FIFO full
#define UARTFR_RXFE         (1 << 4)  // Receive  FIFO empty
#define UARTFR_BUSY         (1 << 3)  // UART busy
#define UARTFR_DCD          (1 << 2)  // Data carrier detect
#define UARTFR_DSR          (1 << 1)  // Data set ready
#define UARTFR_CTS          (1 << 0)  // Clear to send

// Flag reg bits - alternative names
#define UART_TX_EMPTY_FLAG_MASK   UARTFR_TXFE
#define UART_RX_FULL_FLAG_MASK    UARTFR_RXFF
#define UART_TX_FULL_FLAG_MASK    UARTFR_TXFF
#define UART_RX_EMPTY_FLAG_MASK   UARTFR_RXFE
#define UART_BUSY_FLAG_MASK       UARTFR_BUSY

// Control reg bits
#define UARTCR_CTSEN        (1 << 15) // CTS hardware flow control enable
#define UARTCR_RTSEN        (1 << 14) // RTS hardware flow control enable
#define UARTCR_RTS          (1 << 11) // Request to send
#define UARTCR_DTR          (1 << 10) // Data transmit ready.
#define UARTCR_RXE          (1 << 9)  // Receive enable
#define UARTCR_TXE          (1 << 8)  // Transmit enable
#define UARTCR_LBE          (1 << 7)  // Loopback enable
#define UARTCR_UARTEN       (1 << 0)  // UART Enable

// Line Control Register Bits
#define UARTLCR_H_DMA_RT_EN         (1 << 15)
#define UARTLCR_H_DMA_RT_CNT(n)     (((n) & 0x7F) << 8)
#define UARTLCR_H_DMA_RT_CNT_MASK   (0x7F << 8)
#define UARTLCR_H_DMA_RT_CNT_SHIFT  (8)
#define UARTLCR_H_SPS               (1 << 7)  // Stick parity select
#define UARTLCR_H_WLEN_8            (3 << 5)
#define UARTLCR_H_WLEN_7            (2 << 5)
#define UARTLCR_H_WLEN_6            (1 << 5)
#define UARTLCR_H_WLEN_5            (0 << 5)
#define UARTLCR_H_FEN               (1 << 4)  // FIFOs Enable
#define UARTLCR_H_STP2              (1 << 3)  // Two stop bits select
#define UARTLCR_H_EPS               (1 << 2)  // Even parity select
#define UARTLCR_H_PEN               (1 << 1)  // Parity Enable
#define UARTLCR_H_BRK               (1 << 0)  // Send break

// UARTIFLS reg bits
#define UARTIFLS_TXFIFO_LEVEL(n)      (((n) & 3) << 0)
#define UARTIFLS_TXFIFO_LEVEL_MASK    (3 << 0)
#define UARTIFLS_TXFIFO_LEVEL_SHIFT   (0)
#define UARTIFLS_RXFIFO_LEVEL(n)      (((n) & 3) << 3)
#define UARTIFLS_RXFIFO_LEVEL_MASK    (3 << 3)
#define UARTIFLS_RXFIFO_LEVEL_SHIFT   (3)

// DMACR reg bits
#define UARTDMACR_RXDMAE    (1 << 0)
#define UARTDMACR_TXDMAE    (1 << 1)
#define UARTDMACR_DMAONERR  (1 << 2)

#endif
