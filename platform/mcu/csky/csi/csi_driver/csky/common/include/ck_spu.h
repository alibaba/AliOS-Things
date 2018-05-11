#ifndef _CK_SPU_H_
#define _CK_SPU_H_

#define SPU_CLK_FREQUENCY   20000000
#define SPU_INST_RAM_SIZE   5760
#define SPU_PROG_RAM_SIZE   1600

#define getData(value,base,offset) value=*(volatile uint32_t *)(base+offset)
#define putData(value,base,offset) *(volatile uint32_t *)(base+offset)=value

#define ERRNO_SPU_USART_OFFSET   0x1000
#define ERRNO_SPU_SPI_OFFSET     0x2000
#define ERRNO_SPU_IIC_OFFSET     0x3000
#define ERRNO_SPU_IIS_OFFSET     0x4000


#define FIFO_BASE   0x0
#define GLOBAL_BASE 0x0400
#define PGPIO_BASE  0x0800
#define PROG_BASE   0x1000
#define INST_BASE   0x4000

#define OFFSET_INT_SEL      0x0
#define OFFSET_INT_STATUS   0x4
#define OFFSET_FIFO_BADDR   0x8
#define OFFSET_BIG_ENDIAN   0xC

typedef enum {
    ENUM_DEV_NULL = 0,
    ENUM_SPU_USART,
    ENUM_SPU_SPI,
    ENUM_SPU_IIC,
    ENUM_SPU_IIS,
} spu_dev_type_e;

typedef enum {
    ENUM_SIGNAL_NULL = 0,
    ENUM_USART_TX,
    ENUM_USART_RX,
    ENUM_SPI_SCLK,
    ENUM_SPI_MOSI,
    ENUM_SPI_MISO,
    ENUM_SPI_SSEL,
    ENUM_IIC_SDA,
    ENUM_IIC_SCL,
    ENUM_IIS_SCLK,
    ENUM_IIS_WS,
    ENUM_IIS_SD,
    ENUM_IIS_ASSIS,
} spu_dev_signal_e;

typedef struct {
    spu_dev_type_e dev_type;
    int32_t dev_idx;
    spu_dev_signal_e dev_signal;
} spu_pgpio_info_t;

#endif /* _CK_SPU_H_ */
