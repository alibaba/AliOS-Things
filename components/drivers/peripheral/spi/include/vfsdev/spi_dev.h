/*
 * Copyright ( C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef _IO_SPI_H_
#define _IO_SPI_H_

#define IOC_SPI_BASE 'S'
#define IOC_SPI_SET_CFLAG IOC_SPI_BASE + 1
#define IOC_SPI_SET_FREQ  IOC_SPI_BASE + 2
#define IOC_SPI_SEND_RECV  IOC_SPI_BASE + 3
#define IOC_SPI_SEND_AND_RECV  IOC_SPI_BASE + 4
#define IOC_SPI_SEND_AND_SEND  IOC_SPI_BASE + 5
#define IOC_SPI_SET_SERIAL_LEN  IOC_SPI_BASE + 6  //default DEFAULT_SPI_SERAIL_LEN

#define SPI_CPHA        (1<<0)          /**< spi communication is master mode */
#define SPI_CPOL        (1<<1)          /**< spi communication is slave mode */

#define SPI_MSB         (0<<2)          /**< spi communication is master mode */
#define SPI_LSB         (1<<2)          /**< spi communication is slave mode */

#define SPI_MASTER      (0<<3)          /**< spi communication is master mode */
#define SPI_SLAVE       (1<<3)          /**< spi communication is slave mode */

#define SPI_3WIRE       0x10            /**< spi si\so shared */
#define SPI_LOOP        0x20            /**< loop mode */
#define SPI_NO_CS       0x40            /**< NO CS select */
#define SPI_READY       0x80
#define SPI_CS_HIGH     0x100           /**< CS high */

#define SPI_TRANSFER_MODE_MASK  0x600
#define SPI_TRANSFER_DMA_MODE        0x200
#define SPI_TRANSFER_NORMAL_MODE     0x400

#define SPI_DATA_SIZE_MASK 0x000f000
#define SPI_DATA_4BIT 0x0001000
#define SPI_DATA_5BIT 0x0002000
#define SPI_DATA_6BIT 0x0003000
#define SPI_DATA_7BIT 0x0004000
#define SPI_DATA_8BIT 0x0005000
#define SPI_DATA_9BIT 0x0006000
#define SPI_DATA_10BIT 0x0007000
#define SPI_DATA_11BIT 0x0008000
#define SPI_DATA_12BIT 0x0009000
#define SPI_DATA_13BIT 0x000a000
#define SPI_DATA_14BIT 0x000b000
#define SPI_DATA_15BIT 0x000c000
#define SPI_DATA_16BIT 0x000d000

#define SPI_MASK        SPI_CPHA | SPI_CPOL | SPI_MSB | SPI_3WIRE | SPI_LOOP | SPI_NO_CS | SPI_READY | SPI_CS_HIGH | SPI_TRANSFER_MODE_MASK | SPI_DATA_SIZE_MASK

#define SPI_MODE_0      (0|0)
#define SPI_MODE_1      (0|SPI_CPHA)
#define SPI_MODE_2      (SPI_CPOL|0)
#define SPI_MODE_3      (SPI_CPOL|SPI_CPHA)

typedef struct  {
	uint8_t *	tx_buf;
	uint8_t *	rx_buf;
	uint16_t tx_size;
    uint16_t rx_size;
} ioc_spi_transfer_t;

typedef int (*spi_rx_cb) (void *spi); // spi's actual type is spi_dev_t *

int vfs_spi_drv_init (void);

#endif //_IO_SPI_H_
