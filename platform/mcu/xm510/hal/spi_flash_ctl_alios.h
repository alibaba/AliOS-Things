#ifndef __SPI_FLASH_CTL_
#define __SPI_FLASH_CTL_

struct spi_flash {
	const char *name;
	uint8_t dual_flash;
	uint8_t shift;

	uint32_t size;
	uint32_t page_size;
	uint32_t sector_size;
	uint32_t erase_size;
	uint8_t poll_cmd;
	uint8_t erase_cmd;
	uint8_t read_cmd;
	uint8_t write_cmd;
	uint8_t dummy_byte;
    kmutex_t sfclock;
	void *memory_map;
};

struct spi_flash_params {
	const char *name;
	uint32_t jedec;
	uint16_t ext_jedec;
	uint32_t sector_size;
	uint32_t nr_sectors;
	uint8_t e_rd_cmd;
	uint16_t flags;
};

/* Dual SPI flash memories */
enum spi_dual_flash {
	SF_SINGLE_FLASH = 0,
	SF_DUAL_STACKED_FLASH = 1 << 0,
	SF_DUAL_PARALLEL_FLASH = 1 << 1,
};

/* sf param flags */
#define SECT_4K		1 << 1
#define SECT_32K	1 << 2
#define E_FSR		1 << 3
#define WR_QPP		1 << 4

#define FL_CMD_ERASE_4K     0x20
#define FL_CMD_ERASE_32K    0x52
#define FL_CMD_ERASE_CHIP   0xc7
#define FL_CMD_ERASE_64K    0xd8
#define FL_CMD_WRITE_ST     0x01
#define FL_CMD_WRITE_DATA   0x02
#define FL_CMD_READ_DATA    0x03
#define FL_CMD_READ_ST      0x05
#define FL_CMD_READ_ID      0x9F            /*读ID*/

#define FL_OP1  			0x01
#define FL_OP2  			0x02
#define FL_OP3  			0x03
#define FL_OP4  			0x04
#define FL_OP5  			0x05
#define FL_OP6  			0x06
#define FL_OP1_OK  			0x01
#define FL_OP2_OK  			0x02
#define FL_OP3_OK  			0x04
#define FL_OP4_OK  			0x08
#define FL_OP5_OK  			0x10
#define FL_OP6_OK  			0x20


/* Enum list - Full read commands */
enum spi_read_cmds {
	ARRAY_SLOW = 1 << 0,
	DUAL_OUTPUT_FAST = 1 << 1,
	DUAL_IO_FAST = 1 << 2,
	QUAD_OUTPUT_FAST = 1 << 3,
	QUAD_IO_FAST = 1 << 4,
};

#define RD_EXTN		ARRAY_SLOW | DUAL_OUTPUT_FAST | DUAL_IO_FAST
#define RD_FULL		RD_EXTN | QUAD_OUTPUT_FAST | QUAD_IO_FAST

const struct spi_flash_params spi_flash_params_default = {
	"DEFAULT",	   0x000000, 0x0,	64 * 1024,    128,	0,		           0};

/* SPI/QSPI flash device params structure */
const struct spi_flash_params spi_flash_params_table[] = {
  {"W25Q64CV",     0xef4017, 0x0,  64 * 1024,   128,   0,           0},
  {"W25Q128BV",     0xef4018, 0x0,  64 * 1024,   256,   0,           0},
  {"W25Q64DW",     0xef6017, 0x0,  64 * 1024,   128,   0,           0},
  {"W25Q128FW",     0xef6018, 0x0,  64 * 1024,   256,   0,           0},
  {"MX25L6405D",     0xc22017, 0x0,  64 * 1024,   128,  0,         0},
};

#endif
