#ifndef _CK_SPU_USART_H_
#define _CK_SPU_USART_H_

//======================================
//TX/RX fifo configuration
//======================================
#define USART_TX_FIFO_DEPTH      7 //tx fifo depth
#define USART_TX_FIFO_TL         1 //almost-empty threshold
#define USART_RX_FIFO_DEPTH      7 //rx fifo depth
#define USART_RX_FIFO_TL         6 //almost-full threshold
#define USART_RX_FIFO_TL_TRIGGER 1 //almost-full threshold(receive trigger)

//======================================
//PROG_RAM offset
//======================================
#define USART_OFFSET_TX_FIFO_HEAD   0x0
#define USART_OFFSET_TX_FIFO_STATUS 0x8
#define USART_OFFSET_TX_INT         0xC
#define USART_OFFSET_TX_CTRL        0x10
#define USART_OFFSET_TX_LEN         0x14
#define USART_OFFSET_TX_FIFO_DATA   0x18

#define USART_OFFSET_RX_FIFO_HEAD   0x4
#define USART_OFFSET_RX_FIFO_STATUS 0x8
#define USART_OFFSET_RX_INT         0xC
#define USART_OFFSET_RX_CTRL        0x10
#define USART_OFFSET_RX_LEN         0x14
#define USART_OFFSET_RX_FIFO_DATA   0x18

#define USART_TX_PROG_BYTES         (USART_OFFSET_TX_FIFO_DATA+(USART_TX_FIFO_DEPTH+1)*4)
#define USART_RX_PROG_BYTES         (USART_OFFSET_RX_FIFO_DATA+(USART_RX_FIFO_DEPTH+1)*4)
#define USART_PROG_BYTES            (USART_TX_PROG_BYTES+USART_RX_PROG_BYTES)

//======================================
//Interrupt status value
//======================================
#define USART_TX_FIFO_HEAD          ((USART_OFFSET_TX_FIFO_DATA<<20)|(USART_TX_FIFO_DEPTH<<17)|(USART_TX_FIFO_TL<<12))
#define USART_RX_FIFO_HEAD          ((USART_OFFSET_RX_FIFO_DATA<<20)|(USART_RX_FIFO_DEPTH<<17)|(USART_RX_FIFO_TL<<12))
#define USART_RX_FIFO_HEAD_TRIGGER  ((USART_OFFSET_RX_FIFO_DATA<<20)|(USART_RX_FIFO_DEPTH<<17)|(USART_RX_FIFO_TL_TRIGGER<<12))

#define USART_INT_TX_UNDERFLOW  0x1
#define USART_INT_TX_EMPTY      0x2
#define USART_INT_TX_ALEMPTY    0x4

#define USART_INT_RX_OVERFLOW   0x100
#define USART_INT_RX_FULL       0x200
#define USART_INT_RX_ALFULL     0x400

#define USART_INT_TX_DONE       0x1
#define USART_INT_RX_DONE       0x2
#define USART_INT_PARITY_ERR    0x4
#define USART_INT_STOP_ERR      0x8

#endif /* _CK_SPU_USART_H_ */
