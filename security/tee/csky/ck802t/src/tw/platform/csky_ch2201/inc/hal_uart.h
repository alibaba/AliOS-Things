/**
 * Copyright (C) 2015 The YunOS Project. All rights reserved.
 */

#ifndef _HAL_UART_H_
#define _HAL_UART_H_

#include "hal_memmap.h"
#include "tee_types.h"

#define CK_REG volatile uint32_t
#define CK_UINT32 uint32_t
typedef unsigned char BOOL;

typedef struct CKS_GPIO
{
    CK_REG SWPORT_DR;
    CK_REG SWPORT_DDR;
    CK_REG PORT_CTL;
} CKStruct_GPIO, *PCKStruct_GPIO;

typedef enum
{
    CK_GPIO_BEHARDWARE,
    CK_GPIO_BESOFTWARE
} CSKYEnum_GPIO_CTL;

/* UART registers addr definition */
#define CK_UART_RBR 0x00 /* Receive Buffer Register (32 bits, R) */
#define CK_UART_THR 0x00 /* Transmit Holding Register (32 bits, W) */
#define CK_UART_DLL 0x00 /* Divisor Latch(Low)  (32 bits, R/W) */
#define CK_UART_IER 0x01 /* Interrupt Enable Register (32 bits, R/W) */
#define CK_UART_DLH 0x01 /* Divisor Latch(High) (32 bits, R/W) */
#define CK_UART_IIR 0x02 /* Interrupt Identity Register (32 bits, R) */
#define CK_UART_FCR 0x02 /* fifo Countrol Register (32 bits, W) */
#define CK_UART_LCR 0x03 /* Line Control Register (32 bits, R/W) */
#define CK_UART_MCR 0x04 /* Modem Control Register (32 bits, W) */
#define CK_UART_LSR 0x05 /* Line Status Register (32 bits, R) */
#define CK_UART_MSR 0x06 /* Modem Status Register (32 bits, R/W) */
#define CK_UART_USR 0x1f /* UART Status Register (32 bits, R/W) */


#define UART_BUSY_TIMEOUT 1000000
#define UART_RECEIVE_TIMEOUT 1000
#define UART_TRANSMIT_TIMEOUT 1000


/* UART register bit definitions */

#define USR_UART_BUSY 0x01
#define LSR_DATA_READY 0x01
#define LSR_THR_EMPTY 0x20
#define IER_RDA_INT_ENABLE 0x01
#define IER_THRE_INT_ENABLE 0x02
#define IIR_NO_ISQ_PEND 0x01

#define LCR_SET_DLAB 0x80      /* enable r/w DLR to set the baud rate */
#define LCR_PARITY_ENABLE 0x08 /* parity enabled */
#define LCR_PARITY_EVEN 0x10   /* Even parity enabled */
#define LCR_PARITY_ODD 0xef    /* Odd parity enabled */
#define LCR_WORD_SIZE_5 0xfc   /* the data length is 5 bits */
#define LCR_WORD_SIZE_6 0x01   /* the data length is 6 bits */
#define LCR_WORD_SIZE_7 0x02   /* the data length is 7 bits */
#define LCR_WORD_SIZE_8 0x03   /* the data length is 8 bits */
#define LCR_STOP_BIT1 0xfb     /* 1 stop bit */
#define LCR_STOP_BIT2 0x04     /* 1.5 stop bit */

#define CK_LSR_PFE 0x80
#define CK_LSR_TEMT 0x40
#define CK_LSR_THRE 0x40
#define CK_LSR_BI 0x10
#define CK_LSR_FE 0x08
#define CK_LSR_PE 0x04
#define CK_LSR_OE 0x02
#define CK_LSR_DR 0x01
#define CK_LSR_TRANS_EMPTY 0x20


/*config the uart */

typedef struct CK_UART_Info_t
{
    volatile CK_UINT32 *addr;
} CKStruct_UartInfo, *PCKStruct_UartInfo;

/* Struct of Uart REG*/
typedef struct CK_UART_REG
{
    union
    {
        CK_REG uart_RBR;
        CK_REG uart_THR;
        CK_REG uart_DLL;
    };
    union
    {
        CK_REG uart_IER;
        CK_REG uart_DLH;
    };
    union
    {
        CK_REG uart_IIR;
        CK_REG uart_FCR;
    };
    CK_REG uart_LCR;
    CK_REG uart_MCR;
    CK_REG uart_LSR;
    CK_REG uart_MSR;
    CK_REG uart_RSV[24];
    CK_REG uart_USR;
    CK_REG uart_TFL;
    CK_REG uart_RFL;
} CK_UART_REG;


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
} CK_Uart_Baudrate;

#define BAUDRATE 19200


typedef enum
{
    UART0 = 0,
    UART1 = 1,
    UART2 = 2,
    UART3 = 3
} CK_Uart_Device;

typedef enum
{
    WORD_SIZE_5 = 0,
    WORD_SIZE_6 = 1,
    WORD_SIZE_7 = 2,
    WORD_SIZE_8 = 3
} CK_Uart_WordSize;

typedef enum
{
    ODD  = 0,
    EVEN = 1,
    NONE = 2
} CK_Uart_Parity;

typedef enum
{
    LCR_STOP_BIT_1 = 0,
    LCR_STOP_BIT_2 = 1
} CK_Uart_StopBit;


typedef enum
{
    CK_Uart_CTRL_C      = 0,
    CK_Uart_FrameError  = 1,
    CK_Uart_ParityError = 2
} CKEnum_Uart_Error;


#endif /* _HAL_UART_H_ */
