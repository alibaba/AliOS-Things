#ifndef _FLASH_H
#define _FLASH_H

#define FLASH_SIZE 0x200000
#define FLASH_SECTOR_SIZE 0x1000
#define FLASH_PAGE_SIZE 0x100

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// IOPAD
//-------------------------------------------------------------------------------------------------
#define IO_REG_BASE 0xC0000500
#define IO_REG_MANUAL_IO (IO_REG_BASE+0x0)
#define IO_REG_MANUAL_PU (IO_REG_BASE+0x4)
#define IO_REG_MANUAL_PD (IO_REG_BASE+0x8)
#define IO_REG_MANUAL_DS (IO_REG_BASE+0xC)
#define IO_REG_IO_PUE (IO_REG_BASE+0x20)
#define IO_REG_IO_PDE (IO_REG_BASE+0x24)
#define IO_REG_IO_FUNC_SEL (IO_REG_BASE+0x2C)

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// SPI FLASH CONTROLLER
//-------------------------------------------------------------------------------------------------
#if defined(FLASH_CTL_v1)
#warning "FLASH_CTL_v1"
	#define	REG_SPI_CMD_SRAM	0xc0001028
	#define	REG_SPI_DATAIN_SRAM	0xc000100c
	#define	REG_SPI_CMD_LEN		0xc0001024
	#define REG_SPI_BUSY		0xc0001018
#elif defined(FLASH_CTL_v2)
	#define	REG_SPI_CMD_SRAM	0xc0001000
	#define	REG_SPI_DATAIN_SRAM	0xc0001004
	#define	REG_SPI_CMD_TLEN	0xc0001010
	#define	REG_SPI_CMD_RLEN	0xc0001014
	#define REG_SPI_BUSY		0xc000100c
#else
#error unsupported flash controller.
#endif

#define SRAM_ADDR_CMD_DATAOUT		0x00008000
#define SRAM_ADDR_CMD_DATAOUT_ADD4	0x00008004
#define SRAM_ADDR_DATAIN			0x00008400

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// SPI FLASH COMMAND
//-------------------------------------------------------------------------------------------------

#define	READ_STATUS1_CMD	0x05
#define	READ_STATUS1_TLEN	0x01
#define	READ_STATUS1_RLEN	0x01

#define	READ_STATUS2_CMD	0x35
#define	READ_STATUS2_TLEN	0x01
#define	READ_STATUS2_RLEN	0x01

#define	WRITE_STATUS2_CMD	0x31
#define	WRITE_STATUS2_TLEN	0x02
#define	WRITE_STATUS2_RLEN	0x00

#define	WRITE_STATUS1_CMD	0x01
#define	WRITE_STATUS1_TLEN	0x02
#define	WRITE_STATUS1_RLEN	0x00

#define	WRITE_STATUS1_AND_2_CMD		0x01
#define	WRITE_STATUS1_AND_2_TLEN	0x03
#define	WRITE_STATUS1_AND_2_RLEN	0x00

#define	WRITE_ENABLE_CMD	0x06
#define	WRITE_ENABLE_TLEN	0x01
#define	WRITE_ENABLE_RLEN	0x00

#define	WRITE_DISABLE_CMD	0x04
#define	WRITE_DISABLE_TLEN	0x01
#define	WRITE_DISABLE_RLEN	0x00

#define	ERASE_SECTOR_CMD	0x20
#define	ERASE_SECTOR_TLEN	0x04
#define	ERASE_SECTOR_RLEN	0x00

#define	PAGE_PROGRAM_CMD	0x02
#define	PAGE_PROGRAM_TLEN	0x00
#define	PAGE_PROGRAM_RLEN	0x00

#define JEDEC_ID_CMD_CMD	0x9F
#define JEDEC_ID_CMD_TLEN	0x01
#define JEDEC_ID_CMD_RLEN	0x03

#define MANUFATURE_DEVICEID_CMD		0x90
#define MANUFATURE_DEVICEID_TLEN	0x04
#define MANUFATURE_DEVICEID_RLEN	0x02

#define FAST_READ_CMD		0x0B
#define FAST_READ_TLEN		0x05
#define FAST_READ_RLEN		0x00

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// MACRO DEF
//-------------------------------------------------------------------------------------------------

#define ENDIAN_CHANGE(val)	( (((val) & 0xFF000000)>>24) | \
							  (((val) & 0x00FF0000)>>8) | \
							  (((val) & 0x0000FF00)<<8) | \
							  (((val) & 0x000000FF)<<24) )

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// SPI FLASH FUNCTION
//-------------------------------------------------------------------------------------------------
void flash_init(void);
unsigned int flash_read_spimc_data(void);
void flash_write_enable(void);
void flash_write_cmd(unsigned int cmd, unsigned int tlen, unsigned int rlen);
void flash_wait_spi_busy(void);
unsigned int flash_read_spi_status_register(unsigned int cmd, unsigned int tlen, unsigned int rlen);
void flash_wait_spi_status_register(void);
unsigned int flash_get_jedec(void);
void flash_set_status1(unsigned int status1);
void flash_set_status1_and_2(unsigned int status1, unsigned int status2);
void flash_set_qe_type0(void);
void flash_set_qe_type1(void);
void flash_set_quadbit(void);
void flash_sector_erase(unsigned int addr);
void flash_page_program(unsigned int flash_addr, unsigned int flash_len, unsigned char* src_arr);
void flash_fast_read(unsigned int flash_addr, unsigned int flash_len, unsigned char* dest);
#endif
