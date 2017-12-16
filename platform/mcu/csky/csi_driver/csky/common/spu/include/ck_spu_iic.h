//======================================
//baud rate & filter
//======================================
#define I2C_FREQUENCY_STANDARD 10000
//#define I2C_FREQUENCY_FAST 400000
#define I2C_FREQUENCY_FAST 100000
#define I2C_FREQUENCY_FAST_PLUS 1000000
//#define I2C_FREQUENCY_HIGH 1000000
#define I2C_FREQUENCY_HIGH 200000
#define I2C_FILTER_COE 2

//======================================
//PROG_RAM offset
//======================================
#define I2C_OFFSET_REFER_CLK 0x0
#define I2C_OFFSET_CTRL 0x4
#define I2C_OFFSET_TRANS_LEN 0x8
#define I2C_OFFSET_INT 0xC

//======================================
//"CTRL" value
//======================================
//master mode
#define I2C_TX_CONTINUE 0x1
#define I2C_RX_CONTINUE 0x2
//#define I2C_TX_RESTART 0X4
//#define I2C_RX_RESTART 0X8
#define I2C_HS_FLAG 0x4
#define I2C_READ_FLAG 0x8
#define I2C_PEND_EN 0x10
#define I2C_SR_FLAG 0x20

//slave mode
#define I2C_SLV_BUSY 0x1
#define I2C_SLV_RX 0x2

//======================================
//"INTERRUPT" value
//======================================
#define I2C_INT_TX_EMPTY 0x1
#define I2C_INT_TX_NACK 0x2
#define I2C_INT_RX_FULL 0x4
#define I2C_INT_DONE 0x8
#define I2C_INT_PENDING 0x10
#define I2C_INT_ARB_LOST 0x20  //master mode
#define I2C_INT_GENE_CALL 0x20  //slave mode
#define I2C_INT_SLV_TX 0x40
#define I2C_INT_SLV_RX 0x80

//======================================
//configuration of TX/RX fifo
//======================================
//master mode
#define I2C_M_OFFSET_TX_HEAD_W0 0x10
#define I2C_M_OFFSET_TX_HEAD_W1 0x14
#define I2C_M_OFFSET_RX_HEAD_W0 0x18
#define I2C_M_OFFSET_RX_HEAD_W1 0x1C
#define I2C_M_OFFSET_FIFO_MEM_LITTLE (I2C_M_OFFSET_RX_HEAD_W1+0x4)
#define I2C_M_OFFSET_FIFO_MEM_BIG (I2C_M_OFFSET_FIFO_MEM_LITTLE+(I2C_M_FIFO_DEPTH_LITTLE+1)*4)

#define I2C_M_FIFO_DEPTH_LITTLE 3
#define I2C_M_FIFO_DEPTH_BIG 7
#define I2C_M_TX_EMPTY_TL 0
#define I2C_M_RX_FULL_TL 0

#define I2C_M_PROG_BYTES (I2C_M_OFFSET_FIFO_MEM_LITTLE+(I2C_M_FIFO_DEPTH_LITTLE+I2C_M_FIFO_DEPTH_BIG+2)*4)

//slave mode
#define I2C_S_OFFSET_FIFO_HEAD_W0 0x10
#define I2C_S_OFFSET_FIFO_HEAD_W1 0x14
#define I2C_S_OFFSET_FIFO_MEM (I2C_S_OFFSET_FIFO_HEAD_W1+0x4)

#define I2C_S_FIFO_DEPTH 7
#define I2C_S_TX_EMPTY_TL 0
#define I2C_S_RX_FULL_TL 0

#define I2C_S_PROG_BYTES (I2C_S_OFFSET_FIFO_MEM+(I2C_S_FIFO_DEPTH+1)*4)

//======================================
//RESTART flag in data
//======================================
#define I2C_SR_IN_DATA 0x80000000


