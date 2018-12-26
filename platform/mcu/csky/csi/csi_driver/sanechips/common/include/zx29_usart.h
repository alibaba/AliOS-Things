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
 * @file     zx29_usart.h
 * @brief    header file for usart driver
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/
#ifndef __ZX29_USART_H
#define __ZX29_USART_H

#include <stdio.h>
#include "errno.h"
#include "soc.h"

#define ZX29_UART0_NUM    0     /*lp uart*/
#define ZX29_UART1_NUM    1     /*normal uart*/
#define ZX29_UART2_NUM    2     /*normal uart*/
#define ZX29_UART3_NUM    3     /*lp uart*/

/*offset address*/
#define zx29_UART_DR        0x04    /* Data read or written from the interface. */
#define zx29_UART_SC        0x08    /* Set special character to generate a interrupt*/
#define zx29_UART_RSR       0x10    /* Receive status register (Read). */
#define zx29_UART_ECR       0x10    /* Error clear register (Write). */
#define zx29_UART_FR        0x14    /* Flag register (Read only). */

#define zx29_UART_ILPR      0x20    /* IrDA low power counter register. */
#define zx29_UART_IBRD      0x24    /* Integer baud rate divisor register. */
#define zx29_UART_FBRD      0x28    /* Fractional baud rate divisor register. */
#define zx29_UART_LCRH      0x30    /* Line control register. */
#define zx29_UART_CR        0x34    /* Control register. */
#define zx29_UART_IFLS      0x38    /* Interrupt fifo level select. */
#define zx29_UART_IMSC      0x40    /* Interrupt mask. */
#define zx29_UART_RIS       0x44    /* Raw interrupt status. */
#define zx29_UART_MIS       0x48    /* Masked interrupt status. */
#define zx29_UART_ICR       0x4c    /* Interrupt clear register. */
#define zx29_UART_DMACR     0x50    /* DMA control register. */


/*------ uart flag reg -----*/
#define UART_FR_RXBUSY        (1<<9)
#define UART_FR_TXBUSY        (1<<8)
#define UART_FR_TXFE        (1<<7)
#define UART_FR_RXFF        (1<<6)
#define UART_FR_TXFF        (1<<5)
#define UART_FR_RXFE        (1<<4)
#define UART_FR_DSR         (1<<3)
#define UART_FR_DCD         (1<<2)
#define UART_FR_CTS         (1<<1)
#define UART_FR_RI            (1<<0)

/*------ uart control reg -----*/
#define UART_CR_CTSEN        (1<<15)    /* CTS hardware flow control */
#define UART_CR_RTSEN        (1<<14)    /* RTS hardware flow control */
#define UART_CR_OUT2        (1<<13)    /* OUT2 */
#define UART_CR_OUT1        (1<<12)    /* OUT1 */
#define UART_CR_RTS            (1<<11)    /* RTS */
#define UART_CR_DTR            (1<<10)    /* DTR */
#define UART_CR_RXE            (1<<9)    /* receive enable */
#define UART_CR_TXE            (1<<8)    /* transmit enable */
#define UART_CR_LBE            (1<<7)    /* loopback enable */
#define UART_CR_SIRLP        (1<<2)    /* SIR low power mode */
#define UART_CR_SIREN        (1<<1)    /* SIR enable */
#define UART_CR_UARTEN        (1<<0)    /* UART enable */

/*------ uart line control reg -----*/
#define UART_LCRH_SPS        (1<<7)
#define UART_LCRH_WLEN_8    (3<<5)
#define UART_LCRH_WLEN_7    (2<<5)
#define UART_LCRH_WLEN_6    (1<<5)
#define UART_LCRH_WLEN_5    (0<<5)
#define UART_LCRH_FEN        (1<<4)
#define UART_LCRH_STP2        (1<<3)
#define UART_LCRH_EPS        (1<<2)
#define UART_LCRH_PEN        (1<<1)
#define UART_LCRH_BRK        (1<<0)

/*------ uart fifo level select reg -----*/
#define UART_IFLS_RX1_8    (0 << 3)
#define UART_IFLS_RX2_8    (1 << 3)
#define UART_IFLS_RX4_8    (2 << 3)
#define UART_IFLS_RX6_8    (3 << 3)
#define UART_IFLS_RX7_8    (4 << 3)
#define UART_IFLS_TX1_8    (0 << 0)
#define UART_IFLS_TX2_8    (1 << 0)
#define UART_IFLS_TX4_8    (2 << 0)
#define UART_IFLS_TX6_8    (3 << 0)
#define UART_IFLS_TX7_8    (4 << 0)

/*------ uart interrupt mask reg -----*/
#define UART_OEIM        (1 << 10)    /* overrun error interrupt mask */
#define UART_BEIM        (1 << 9)    /* break error interrupt mask */
#define UART_PEIM        (1 << 8)    /* parity error interrupt mask */
#define UART_FEIM        (1 << 7)    /* framing error interrupt mask */
#define UART_RTIM        (1 << 6)    /* receive timeout interrupt mask */
#define UART_TXIM        (1 << 5)    /* transmit interrupt mask */
#define UART_RXIM        (1 << 4)    /* receive interrupt mask */
#define UART_DSRMIM        (1 << 3)    /* DSR interrupt mask */
#define UART_DCDMIM        (1 << 2)    /* DCD interrupt mask */
#define UART_CTSMIM        (1 << 1)    /* CTS interrupt mask */
#define UART_RIMIM        (1 << 0)    /* RI interrupt mask */

/*------ uart raw interrupt status reg -----*/
#define UART_OEIS        (1 << 10)    /* overrun error interrupt status */
#define UART_BEIS        (1 << 9)    /* break error interrupt status */
#define UART_PEIS        (1 << 8)    /* parity error interrupt status */
#define UART_FEIS        (1 << 7)    /* framing error interrupt status */
#define UART_RTIS        (1 << 6)    /* receive timeout interrupt status */
#define UART_TXIS        (1 << 5)    /* transmit interrupt status */
#define UART_RXIS        (1 << 4)    /* receive interrupt status */
#define UART_DSRMIS        (1 << 3)    /* DSR interrupt status */
#define UART_DCDMIS        (1 << 2)    /* DCD interrupt status */
#define UART_CTSMIS        (1 << 1)    /* CTS interrupt status */
#define UART_RIMIS        (1 << 0)    /* RI interrupt status */

/*------ uart interrupt clear reg -----*/
#define UART_SCIC        (1 << 11)    /* special character interrupt clear */
#define UART_OEIC        (1 << 10)    /* overrun error interrupt clear */
#define UART_BEIC        (1 << 9)    /* break error interrupt clear */
#define UART_PEIC        (1 << 8)    /* parity error interrupt clear */
#define UART_FEIC        (1 << 7)    /* framing error interrupt clear */
#define UART_RTIC        (1 << 6)    /* receive timeout interrupt clear */
#define UART_TXIC        (1 << 5)    /* transmit interrupt clear */
#define UART_RXIC        (1 << 4)    /* receive interrupt clear */
#define UART_DSRMIC        (1 << 3)    /* DSR interrupt clear */
#define UART_DCDMIC        (1 << 2)    /* DCD interrupt clear */
#define UART_CTSMIC        (1 << 1)    /* CTS interrupt clear */
#define UART_RIMIC        (1 << 0)    /* RI interrupt clear */

/*------ uart DMA control reg -----*/
#define UART_DMAONERR    (1 << 2)    /* disable dma on error */
#define UART_TXDMAE        (1 << 1)    /* enable transmit dma */
#define UART_RXDMAE        (1 << 0)    /* enable receive dma */

/*------ uart receive data  error indicator in data reg -----*/
#define UART_DR_OE        (1 << 11)
#define UART_DR_BE        (1 << 10)
#define UART_DR_PE        (1 << 9)
#define UART_DR_FE        (1 << 8)

/*------ uart RSR reg -----*/
#define UART_RSR_OE         0x08
#define UART_RSR_BE         0x04
#define UART_RSR_PE         0x02
#define UART_RSR_FE         0x01

#define UART_DR_ERROR        (UART_DR_OE|UART_DR_BE|UART_DR_PE|UART_DR_FE)
#define UART_DUMMY_DR_RX    (1 << 16)

#define UART_RSR_ANY        (UART_RSR_OE|UART_RSR_BE|UART_RSR_PE|UART_RSR_FE)
#define UART_FR_MODEM_ANY    (UART_FR_DCD|UART_FR_DSR|UART_FR_CTS)

#define UART_TXFIFO_SIZE 16

//#define UART_PIN_NUM_BASE 48  /*the first uart pin number*/

#define UART0_PIN_NUM_BASE  ZX29_GPIO_37 /*the uart0 pin number*/
#define UART2_PIN_NUM_BASE  ZX29_GPIO_39 /*the uart2 pin number*/
#define UART3_PIN_NUM_BASE  ZX29_GPIO_43 /*the uart3 pin number*/
#define UART0_PIN_CTS         ZX29_GPIO_39 /*the uart0 cts pin number*/
#define UART0_PIN_RTS         ZX29_GPIO_40 /*the uart0 rts pin number*/
#define UART3_PIN_CTS         ZX29_GPIO_41 /*the uart3 cts pin number*/
#define UART3_PIN_RTS         ZX29_GPIO_42 /*the uart3 rts pin number*/


/* UART registers addr definition */
#define CK_UART_RBR 0x00         /* receive buffer register (32 bits, R) */
#define CK_UART_THR 0x00         /* transmit holding register (32 bits, W) */
#define CK_UART_DLL 0x00         /* divisor latch(low)  (32 bits, R/W) */
#define CK_UART_IER 0x04         /* interrupt enable register (32 bits, R/W) */
#define CK_UART_DLH 0x04         /* divisor latch(high) (32 bits, R/W) */
#define CK_UART_IIR 0x08         /* interrupt identity register (32 bits, R) */
#define CK_UART_FCR 0x08         /* fifo countrol register (32 bits, W) */
#define CK_UART_LCR 0x0C         /* line control register (32 bits, R/W) */
#define CK_UART_MCR 0x10         /* modem control register (32 bits, W) */
#define CK_UART_LSR 0x14         /* line status register (32 bits, R) */
#define CK_UART_MSR 0x18         /* modem status register (32 bits, R/W) */
#define CK_UART_USR 0x7C         /* UART status register (32 bits, R/W) */

#define UART_BUSY_TIMEOUT     1000000
#define UART_RECEIVE_TIMEOUT  1000
#define UART_TRANSMIT_TIMEOUT 1000

/* UART register bit definitions */

#define USR_UART_BUSY       0x01
#define LSR_DATA_READY      0x01
#define LSR_THR_EMPTY       0x20
#define LSR_TEMT_EMPTY      0x40
#define IER_RDA_INT_ENABLE  0x01
#define IER_THRE_INT_ENABLE 0x02
#define IIR_NO_ISQ_PEND     0x01
#define FCR_FIFO_ENAB       0x01

#define LCR_SET_DLAB        0x80 /* enable r/w DLR to set the baud rate */
#define LCR_PARITY_ENABLE   0x08 /* parity enabled */
#define LCR_PARITY_EVEN     0x10 /* even parity enabled */
#define LCR_PARITY_ODD      0xef /* odd parity enabled */
#define LCR_WORD_SIZE_5     (0<<5) /* the data length is 5 bits */
#define LCR_WORD_SIZE_6     (1<<5) /* the data length is 6 bits */
#define LCR_WORD_SIZE_7     (2<<5) /* the data length is 7 bits */
#define LCR_WORD_SIZE_8     (3<<5) /* the data length is 8 bits */
#define LCR_STOP_BIT1       (0<<3)/* 1 stop bit */
#define LCR_STOP_BIT2       (1<<3) /* 1.5 stop bit */

#define CK_LSR_PFE          0x80
#define CK_LSR_TEMT         0x40
#define CK_LSR_THRE         0x40
#define CK_LSR_BI           0x10
#define CK_LSR_FE           0x08
#define CK_LSR_PE           0x04
#define CK_LSR_OE           0x02
#define CK_LSR_DR           0x01
#define CK_LSR_TRANS_EMPTY  0x20

#define CK_UART_TXBUFFERSIZE 128
#define CK_UART_RXBUFFERSIZE 128


#define zx29_UART_T0        0x80    /* time out register. */
#define zx29_UART_SILENT    0x84    /* silent set register. */
#define UART_CR_LPMODE        (1<<3)    /* LP  mode */

#define LCR_WORD_SIZE_9     (1<<8) /* the data length is 9 bits */

/*------ uart fifo level select reg -----*/
#define UART_IFLS_RX2_32    (0 << 3)
#define UART_IFLS_RX4_32    (1 << 3)
#define UART_IFLS_RX8_32    (2 << 3)
#define UART_IFLS_RX12_32    (3 << 3)
#define UART_IFLS_RX14_32    (4 << 3)

#define UART_IFLS_TX18_32    (0 << 0)
#define UART_IFLS_TX20_32    (1 << 0)
#define UART_IFLS_TX24_32    (2 << 0)
#define UART_IFLS_TX28_32    (3 << 0)
#define UART_IFLS_TX30_32    (4 << 0)
#define UART_IFLS_TX16_32    (5 << 0)


typedef enum {
    LSR_IIR_MOD_STS  = 0,
    LSR_IIR_INT_PND  = 1,
    LSR_IIR_THR_EMP  = 2,
    LSR_IIR_RCV_AVAL = 4,
    LSR_IIR_RCV_STS  = 6,
    LSR_IIR_BS_DET   = 7,
    LSR_IIR_CHAR_TO  = 12
} LSR_IIR_IID;

typedef enum {
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
} UART_BAUDRATE;

typedef enum {
    NONE,
    ODD,
    EVEN
} UART_PARITY;

typedef enum {
    WORD_SIZE_5 = 5,
    WORD_SIZE_6,
    WORD_SIZE_7,
    WORD_SIZE_8,
    WORD_SIZE_9,
} UART_WORDSIZE;

typedef enum {
    LCR_STOP_BIT_1,
    LCR_STOP_BIT_2
} UART_STOPBIT;


#endif /* __ZX29_USART_H */

