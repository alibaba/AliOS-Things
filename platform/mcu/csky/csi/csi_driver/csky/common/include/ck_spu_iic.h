#ifndef _CK_SPU_IIC_H_
#define _CK_SPU_IIC_H_

//======================================
//baud
//======================================
#define IIC_FREQUENCY_STANDARD  100000
#define IIC_FREQUENCY_FAST      400000

//======================================
//fifo configuration
//======================================
#define IIC_FIFO_DEPTH  7
#define IIC_TX_FIFO_TL  1  //almost-empty threshold
#define IIC_RX_FIFO_TL  6  //almost-full threshold

//======================================
//PROG_RAM offset
//======================================
#define IIC_OFFSET_TX_FIFO_HEAD 0x0
#define IIC_OFFSET_RX_FIFO_HEAD 0x4
#define IIC_OFFSET_FIFO_STATUS  0x8
#define IIC_OFFSET_INT          0xC
#define IIC_OFFSET_CTRL         0x10
#define IIC_OFFSET_TX_LEN       0x14  //master
#define IIC_OFFSET_RX_LEN       0x18  //master
#define IIC_OFFSET_LEN          0x14  //slave
#define IIC_OFFSET_SLAVE_ADDR   0x18  //slave
#define IIC_OFFSET_FIFO_DATA    0x1C

#define IIC_PROG_BYTES          (IIC_OFFSET_FIFO_DATA+(IIC_FIFO_DEPTH+1)*4)

//======================================
//PROG RAM value
//======================================
#define IIC_TX_FIFO_HEAD ((IIC_OFFSET_FIFO_DATA<<20)|(IIC_FIFO_DEPTH<<17)|(IIC_TX_FIFO_TL<<12))
#define IIC_RX_FIFO_HEAD ((IIC_OFFSET_FIFO_DATA<<20)|(IIC_FIFO_DEPTH<<17)|(IIC_RX_FIFO_TL<<12))

#define IIC_INT_TX_UNDERFLOW    0x1
#define IIC_INT_TX_EMPTY        0x2
#define IIC_INT_TX_ALEMPTY      0x4

#define IIC_INT_RX_OVERFLOW     0x100
#define IIC_INT_RX_FULL         0x200
#define IIC_INT_RX_ALFULL       0x400

#define IIC_INT_DONE            0x1
#define IIC_INT_PENDING         0x2
#define IIC_INT_NACK            0x4
#define IIC_INT_ARB_LOST        0x8
#define IIC_INT_SLAVE_TX        0x10
#define IIC_INT_SLAVE_RX        0x20
#define IIC_INT_GENE_CALL       0x60

#define IIC_CTRL_CONTINUE       0x1
#define IIC_CTRL_CONT_RX        0x2
#define IIC_CTRL_READ           0x4
#define IIC_CTRL_PEND           0x8
#define IIC_CTRL_SLAVE_TX       0x10
#define IIC_CTRL_SLAVE_RX       0x0

#endif /* _CK_SPU_IIC_H_ */
