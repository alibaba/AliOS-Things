//======================================
//baud rate & filter
//======================================
#define SPI_FILTER_COE 2

//======================================
//PROG_RAM offset
//======================================
#define SPI_OFFSET_REFER_CLK 0x0
#define SPI_OFFSET_CTRL 0x4
#define SPI_OFFSET_DEFAULT_VALUE 0x8
#define SPI_OFFSET_TRANS_LEN 0xC
#define SPI_OFFSET_INT 0x10
#define SPI_OFFSET_TX_HEAD_W0 0x14
#define SPI_OFFSET_TX_HEAD_W1 0x18
#define SPI_OFFSET_RX_HEAD_W0 0x1C
#define SPI_OFFSET_RX_HEAD_W1 0x20

//======================================
//"CTRL" value
//======================================
#define SPI_TMOD_TRANSMIT 0x1
#define SPI_TMOD_RECEIVE 0x2
#define SPI_TMOD_TRANSFER 0x3
#define SPI_BACK_TO_BACK 0x4
#define SPI_CS_PEND 0x8

//======================================
//"INTERRUPT" value
//======================================
#define SPI_INT_RX_DONE 0x1
#define SPI_INT_RX_FULL 0x2
#define SPI_INT_TX_DONE 0x4
#define SPI_INT_TX_EMPTY 0x8

//======================================
//configuration of TX/RX fifo
//======================================
#define SPI_TX_FIFO_DEPTH 7
#define SPI_TX_FIFO_EMPTY_TL 1
#define SPI_RX_FIFO_DEPTH 7
#define SPI_RX_FIFO_FULL_TL 6

#define SPI_OFFSET_TX_MEM (SPI_OFFSET_RX_HEAD_W1+4)
#define SPI_OFFSET_RX_MEM (SPI_OFFSET_TX_MEM+(SPI_TX_FIFO_DEPTH+1)*4)

#define SPI_PROG_BYTES (SPI_OFFSET_RX_MEM+(SPI_RX_FIFO_DEPTH+1)*4)

