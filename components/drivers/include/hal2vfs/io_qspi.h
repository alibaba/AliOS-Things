/*
 * Copyright ( C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef _IO_QSPI_H_
#define _IO_QSPI_H_

#include <aos/hal/spi.h>

#define IOC_QSPI_BASE 'Q'
#define IOC_QSPI_SET_CFLAG IOC_QSPI_BASE + 1
#define IOC_QSPI_SET_FREQ  IOC_QSPI_BASE + 2
#define IOC_QSPI_SEND_CMD  IOC_QSPI_BASE + 3
#define IOC_QSPI_SEND_DATA  IOC_QSPI_BASE + 4
#define IOC_QSPI_RECV_DATA  IOC_QSPI_BASE + 5
#define IOC_QSPI_SEND_RECV_DATA  IOC_QSPI_BASE + 6

#define QSPI_CPHA        (1<<0)          /**< Clock Polarity setting */
#define QSPI_CPOL        (1<<1)          /**< Clock Phase setting */

#define QSPI_NO_CS       0x40            /**< NO CS select */
#define QSPI_CS_HIGH     0x80            /**< CS high */

#define QSPI_TRANSFER_MODE_MASK  0x600
#define QSPI_TRANSFER_DMA        0x200   /**< DMA mode */
#define QSPI_TRANSFER_PIO        0x400   /**< PIO mode */

#define QSPI_DATA_SIZE_MASK 0x0007000
#define QSPI_DATA_8BIT 0x0001000
#define QSPI_DATA_16BIT 0x0002000
#define QSPI_DATA_24BIT 0x0003000
#define QSPI_DATA_32BIT 0x0004000

#define QSPI_BUS_LINE_MASK 0x0070000
#define QSPI_BUS_1_LINE 0x0010000        /**< 1 line mode */
#define QSPI_BUS_2_LINE 0x0020000        /**< 2 line mode */
#define QSPI_BUS_4_LINE 0x0040000        /**< 4 line mode */

#define QSPI_DDR_ENABLE 0x0080000


#define QSPI_MODE_0      (0|0)
#define QSPI_MODE_1      (0|QSPI_CPHA)
#define QSPI_MODE_2      (QSPI_CPOL|0)
#define QSPI_MODE_3      (QSPI_CPOL|QSPI_CPHA)

typedef struct  {
	const char *tx_buf;
	char *rx_buf;
	uint16_t tx_size;
	uint16_t rx_size;
	uint32_t timeout;
} ioc_qspi_data_t;

typedef struct {
	struct {
		uint32_t skip:1;              /**< Instruction phase skipped if disabled is set to true */
		uint32_t lines:3;             /**< Bus width for the instruction */
		uint32_t value:8;             /**< Instruction value */
	} instruction;
	struct {
		uint32_t skip:1;              /**< Address phase skipped if disabled is set to true */
		uint32_t lines:3;             /**< Bus width for the address */
		uint32_t size:3;              /**< Address size */
		uint32_t value;               /**< Address value */
	}  address;
	struct {
		uint32_t skip:1;              /**< Alternative phase skipped if disabled is set to true */
		uint32_t lines:3;             /**< Bus width for alternative */
		uint32_t size:3;              /**< Alternative size */
		uint32_t value;               /**< Alternative value */
	} alt;
	uint8_t dummy_count;              /**< Dummy cycles count */
	struct {
		uint32_t lines;                /**< Bus width for data */
	} data;
} ioc_qspi_cmd_t;

typedef struct  {
	ioc_qspi_cmd_t *cmd;
	ioc_qspi_data_t *data;
} ioc_qspi_transfer_t;

int vfs_qspi_drv_init (void);

#endif //_IO_QSPI_H_
