/**
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _UART_H_
#define _UART_H_

#include "datatype.h"

#define SYS_UART0_ADDR (0x21401000)
#define SYS_UART1_ADDR (0x21402000)
#define SYS_UART2_ADDR (0x2140E000)

#define UART0 0
#define UART1 1
#define UART2 2

#define SYS_UART_CLK 19200000 // Ê±ÖÓÆµÂÊ


/* timer for timeout count, timeout 1000ms */
#define TIMEOUT_VALUE 1000      // ms
#define TIMEOUT_COUNT_CYCLE 100 // us
#define TIMEOUT_COUNT_VALUE (TIMEOUT_VALUE * 1000 / TIMEOUT_COUNT_CYCLE)

/* UART register bit definitions */

/*------ uart flag reg -----*/
#define UART_FR_RXBUSY (1 << 9)
#define UART_FR_TXBUSY (1 << 8)
#define UART_FR_TXFE (1 << 7)
#define UART_FR_RXFF (1 << 6)
#define UART_FR_TXFF (1 << 5)
#define UART_FR_RXFE (1 << 4)
#define UART_FR_DSR (1 << 3)
#define UART_FR_DCD (1 << 2)
#define UART_FR_CTS (1 << 1)
#define UART_FR_RI (1 << 0)

/*------ uart control reg -----*/
#define UART_CR_CTSEN (1 << 15) /* CTS hardware flow control */
#define UART_CR_RTSEN (1 << 14) /* RTS hardware flow control */
#define UART_CR_OUT2 (1 << 13)  /* OUT2 */
#define UART_CR_OUT1 (1 << 12)  /* OUT1 */
#define UART_CR_RTS (1 << 11)   /* RTS */
#define UART_CR_DTR (1 << 10)   /* DTR */
#define UART_CR_RXE (1 << 9)    /* receive enable */
#define UART_CR_TXE (1 << 8)    /* transmit enable */
#define UART_CR_LBE (1 << 7)    /* loopback enable */
#define UART_CR_SIRLP (1 << 2)  /* SIR low power mode */
#define UART_CR_SIREN (1 << 1)  /* SIR enable */
#define UART_CR_UARTEN (1 << 0) /* UART enable */

/*------ uart line control reg -----*/
#define UART_LCRH_SPS (1 << 7)
#define UART_LCRH_WLEN_8 (3 << 5)
#define UART_LCRH_WLEN_7 (2 << 5)
#define UART_LCRH_WLEN_6 (1 << 5)
#define UART_LCRH_WLEN_5 (0 << 5)
#define UART_LCRH_FEN (1 << 4)
#define UART_LCRH_STP2 (1 << 3)
#define UART_LCRH_EPS (1 << 2)
#define UART_LCRH_PEN (1 << 1)
#define UART_LCRH_BRK (1 << 0)

/*------ uart fifo level select reg -----*/
#define UART_IFLS_RX1_8 (0 << 3)
#define UART_IFLS_RX2_8 (1 << 3)
#define UART_IFLS_RX4_8 (2 << 3)
#define UART_IFLS_RX6_8 (3 << 3)
#define UART_IFLS_RX7_8 (4 << 3)
#define UART_IFLS_TX1_8 (0 << 0)
#define UART_IFLS_TX2_8 (1 << 0)
#define UART_IFLS_TX4_8 (2 << 0)
#define UART_IFLS_TX6_8 (3 << 0)
#define UART_IFLS_TX7_8 (4 << 0)

#define LCR_SET_DLAB 0x80        /* enable r/w DLR to set the baud rate */
#define LCR_PARITY_ENABLE 0x08   /* parity enabled */
#define LCR_PARITY_EVEN 0x10     /* even parity enabled */
#define LCR_PARITY_ODD 0xef      /* odd parity enabled */
#define LCR_WORD_SIZE_5 (0 << 5) /* the data length is 5 bits */
#define LCR_WORD_SIZE_6 (1 << 5) /* the data length is 6 bits */
#define LCR_WORD_SIZE_7 (2 << 5) /* the data length is 7 bits */
#define LCR_WORD_SIZE_8 (3 << 5) /* the data length is 8 bits */
#define LCR_WORD_SIZE_9 (1 << 8) /* the data length is 9 bits */
#define LCR_STOP_BIT1 (0 << 3)   /* 1 stop bit */
#define LCR_STOP_BIT2 (1 << 3)   /* 1.5 stop bit */

/*------ uart interrupt mask reg -----*/
#define UART_OEIM (1 << 10)  /* overrun error interrupt mask */
#define UART_BEIM (1 << 9)   /* break error interrupt mask */
#define UART_PEIM (1 << 8)   /* parity error interrupt mask */
#define UART_FEIM (1 << 7)   /* framing error interrupt mask */
#define UART_RTIM (1 << 6)   /* receive timeout interrupt mask */
#define UART_TXIM (1 << 5)   /* transmit interrupt mask */
#define UART_RXIM (1 << 4)   /* receive interrupt mask */
#define UART_DSRMIM (1 << 3) /* DSR interrupt mask */
#define UART_DCDMIM (1 << 2) /* DCD interrupt mask */
#define UART_CTSMIM (1 << 1) /* CTS interrupt mask */
#define UART_RIMIM (1 << 0)  /* RI interrupt mask */


typedef enum
{
    B1200   = 1200,
    B2400   = 2400,
    B4800   = 4800,
    B9600   = 9600,
    B14400  = 14400,
    B19200  = 19200,
    B56000  = 56000,
    B38400  = 38400,
    B57600  = 57600,
    B115200 = 115200
} uart_baudrate_t;

typedef enum
{
    NONE = 0,
    ODD  = 8,
    EVEN = 16
} uart_parity_t;

typedef enum
{
    WORD_SIZE_5 = 5,
    WORD_SIZE_6,
    WORD_SIZE_7,
    WORD_SIZE_8,
    WORD_SIZE_9,
} uart_wordsize_t;

typedef enum
{
    LCR_STOP_BIT_1 = 1,
    LCR_STOP_BIT_2
} uart_stopbit_t;

typedef volatile struct _hal_uart_reg_t
{
    uint32_t vs;        // 0x00 °æ±¾¼Ä´æÆ÷UARTVS
    uint32_t dr;        // 0x04 Êý¾Ý¼Ä´æÆ÷UARTDR
    uint32_t sc;        // 0x08 ÌØÊâ×Ö·û¼Ä´æÆ÷UARTSC
    uint32_t reserved0; // 0x0c ±£Áô
    uint32_t rsr; // 0x10 ½ÓÊÕ×´Ì¬/´íÎóÇå³ý¼Ä´æÆ÷UARTRSR/UARTECR
    uint32_t fr;           // 0x14 ±êÖ¾¼Ä´æÆ÷ UARTFR
    uint32_t reserved1[2]; // 0x18-0x1c ±£Áô
    uint32_t ilpr; // 0x20 ºìÍâµÍ¹¦ºÄ¼ÆÊý¼Ä´æÆ÷UARTILPR
    uint32_t ibrd; // 0x24 ²¨ÌØÂÊ·ÖÆµÏµÊýÕûÊý¼Ä´æÆ÷UARTIBRD
    uint32_t fbrd; // 0x28 ²¨ÌØÂÊ·ÖÆµÏµÊýµÄÐ¡Êý¼Ä´æÆ÷UARTFBRD
    uint32_t reserved2; // 0x2C ±£Áô
    uint32_t lcr_h;     // 0x30 line ¿ØÖÆ¼Ä´æÆ÷UARTLCR_H
    uint32_t cr;        // 0x34 ¿ØÖÆ¼Ä´æÆ÷UARTCR
    uint32_t ifls;      // 0x38 FIFOãÐÖµ¼Ä´æÆ÷UARTIFLS
    uint32_t reserved3; // 0x3c ±£Áô
    uint32_t imsc;      // 0x40 ÖÐ¶ÏÆÁ±Î¼Ä´æÆ÷
    uint32_t ris;       // 0x44 ÖÐ¶ÏÔ­Ê¼×´Ì¬¼Ä´æÆ÷
    uint32_t mis;       // 0x48 ÖÐ¶Ï×´Ì¬¼Ä´æÆ÷
    uint32_t icr;       // 0x4c ÖÐ¶ÏÇå³ý¼Ä´æÆ÷
    uint32_t dmacr;     // 0x50 DMA ¿ØÖÆ¼Ä´æÆ÷
} hal_uart_reg_t;

/*config the uart */
typedef struct _hal_uart_info_t
{
    uint32_t                 id;
    volatile hal_uart_reg_t *addr;
    uint32_t                 baudrate;
    uint32_t                 parity; /* ODD=8, EVEN=16, or NONE=0. */
    uint32_t                 word_size;
    uint32_t                 stop_bits;
    bool                     rx_bquery;
    bool                     tx_bquery;
} hal_uart_info_t;

int32_t _hal_uart_init(uint32_t id, uint32_t baudrate, uint32_t parity,
                       int32_t wordsize, int32_t stopbit);

#endif /* _HAL_UART_H_ */
