#ifndef _CK_SPU_SPI_H_
#define _CK_SPU_SPI_H_

//#define SPI_CS_TOGGLE
//======================================
// TX/RX fifo configuration
//======================================
#define SPI_TX_FIFO_DEPTH 7
#define SPI_TX_FIFO_TL    2  //almost-empty threshold
#define SPI_RX_FIFO_DEPTH 7
#define SPI_RX_FIFO_TL    6  //almost-full threshold

//======================================
//PROG_RAM offset
//======================================
#define SPI_OFFSET_TX_FIFO_HEAD 0x0
#define SPI_OFFSET_RX_FIFO_HEAD 0x4
#define SPI_OFFSET_FIFO_STATUS  0x8
#define SPI_OFFSET_INT          0xC
#define SPI_OFFSET_CTRL         0x10
#define SPI_OFFSET_LEN          0x14
#define SPI_OFFSET_TX_FIFO_DATA 0x18
#define SPI_OFFSET_RX_FIFO_DATA (SPI_OFFSET_TX_FIFO_DATA+(SPI_TX_FIFO_DEPTH+1)*4)

#define SPI_PROG_BYTES          (SPI_OFFSET_RX_FIFO_DATA+(SPI_RX_FIFO_DEPTH+1)*4)

//======================================
//FIFO head value
//======================================
#define SPI_TX_FIFO_HEAD ((SPI_OFFSET_TX_FIFO_DATA<<20)|(SPI_TX_FIFO_DEPTH<<17)|(SPI_TX_FIFO_TL<<12))
#define SPI_RX_FIFO_HEAD ((SPI_OFFSET_RX_FIFO_DATA<<20)|(SPI_RX_FIFO_DEPTH<<17)|(SPI_RX_FIFO_TL<<12))

//======================================
//Interrupt status value
//======================================
#define SPI_INT_TX_UNDERFLOW  0x1
#define SPI_INT_TX_EMPTY      0x2
#define SPI_INT_TX_ALEMPTY    0x4

#define SPI_INT_RX_OVERFLOW   0x100
#define SPI_INT_RX_FULL       0x200
#define SPI_INT_RX_ALFULL     0x400

#define SPI_INT_TRANS_DONE    0x1

//======================================
//"CTRL" value
//======================================
#define SPI_TMOD_TRANSMIT 0x1
#define SPI_TMOD_RECEIVE  0x2
#define SPI_TMOD_TRANSFER 0x3
#define SPI_SHIFT_LEFT    0x80
#ifdef SPI_CS_TOGGLE
#define SPI_CS_TYPE   0x600
#else
#define SPI_CS_TYPE   0x0
#endif

#endif /* _CK_SPU_IIS_H_ */
