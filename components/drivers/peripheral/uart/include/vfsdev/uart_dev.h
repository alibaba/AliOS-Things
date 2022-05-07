/*
 * Copyright ( C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef _IO_UART_H_
#define _IO_UART_H_

#define IOC_UART_BASE 'U'
#define IOC_UART_SET_CFLAG IOC_UART_BASE + 1
#define IOC_UART_GET_CFLAG IOC_UART_BASE + 2
#define IOC_UART_DMA_CTL IOC_UART_BASE + 3
#define IOC_UART_TRX_CTL IOC_UART_BASE + 4 // disable Tx or Rx
#define IOC_UART_POLL_TEST IOC_UART_BASE + 5 // for test purpose

/* argument definition */

/* C_ Cflag bit meaning */
#define CBAUD  0x0010017
#define B0    0x0000000       /* ignore */
#define B50   0x0000001
#define B75   0x0000002
#define B110  0x0000003
#define B134  0x0000004
#define B150  0x0000005
#define B200  0x0000006
#define B300  0x0000007
#define B600  0x0000010
#define B1200 0x0000011
#define B1800 0x0000012
#define B2400 0x0000013
#define B4800 0x0000014
#define B9600 0x0000015
#define B19200    0x0000016
#define B38400    0x0000017
#define EXTA 0xB19200
#define EXTB 0xB38400

#define CBAUDEX 0x0010000
#define B57600 0x0010001
#define B115200 0x0010002
#define B230400 0x0010003
#define B460800 0x0010004
#define B500000 0x0010005
#define B576000 0x0010006
#define B921600 0x0010007
#define B1000000 0x0010010
#define B1152000 0x0010011
#define B1500000 0x0010012
#define B2000000 0x0010013
#define B2500000 0x0010014
#define B3000000 0x0010015
#define B3500000 0x0010016
#define B4000000 0x0010017

#define CSIZE  0x0000060
#define CS5  0x0000000
#define CS6  0x0000020
#define CS7  0x0000040
#define CS8  0x0000060

#define CSTOPB 0x0000100
#define CREAD  0x0000200
#define PARENB 0x0000400
#define PARODD 0x0001000

#define CRTS_CTS 0x30000000 /* flow Control */
#define CRTS 0x10000000 /* flow Control */
#define CCTS 0x20000000 /* flow Control */

int vfs_uart_drv_init (void);

#endif //_IO_UART_H_
