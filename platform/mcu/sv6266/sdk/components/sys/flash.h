#ifndef _FLASH_H
#define _FLASH_H

#include "attrs.h"

// FLASH PARAM
#define FLASH_BLOCK_SIZE 65536
#define FLASH_SECTOR_SIZE 4096
#define FLASH_PAGE_SIZE 256
#define FLASH_MAX_CMD_LEN 4

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// IOPAD
//-------------------------------------------------------------------------------------------------


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

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// SPI FLASH COMMAND
//-------------------------------------------------------------------------------------------------

#define	READ_STATUS1_CMD	0x05
#define	READ_STATUS1_TLEN	0x01
#define	READ_STATUS1_RLEN	0x03

#define	READ_STATUS2_CMD	0x35
#define	READ_STATUS2_TLEN	0x01
#define	READ_STATUS2_RLEN	0x03

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

#define FAST_READ_CMD		0x0B
#define FAST_READ_TLEN		0x05
#define FAST_READ_RLEN		0x00

#define JEDEC_ID_CMD_CMD	0x9F
#define JEDEC_ID_CMD_TLEN	0x01
#define JEDEC_ID_CMD_RLEN	0x03

#define MANUFATURE_DEVICEID_CMD		0x90
#define MANUFATURE_DEVICEID_TLEN	0x04
#define MANUFATURE_DEVICEID_RLEN	0x02

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
unsigned int flash_init(void);
void flash_sector_erase(unsigned int addr);
void flash_page_program(unsigned int flash_addr, unsigned int flash_len, unsigned char* src_arr);

#if 0
void flash_dump(unsigned int dump_addr, unsigned int length);
#endif

#endif // _FLASH_H
