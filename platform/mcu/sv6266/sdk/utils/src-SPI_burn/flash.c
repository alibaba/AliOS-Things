#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include "flash.h"

void flash_init(void)
{
	// set io pad
	/*
	outw(IO_REG_MANUAL_IO, 0x000fc000);
    outw(IO_REG_MANUAL_PU, 0x000fc000);
    outw(IO_REG_MANUAL_PD, 0x000fc000);
    outw(IO_REG_MANUAL_DS, 0x000fc000);
    outw(IO_REG_IO_FUNC_SEL, 0x00000400);
*/
    	outw(IO_REG_MANUAL_IO, 0x000fe000);
    outw(IO_REG_MANUAL_PU, 0x000fe000);
    outw(IO_REG_MANUAL_PD, 0x000fe000);
    outw(IO_REG_MANUAL_DS, 0x000fe000);
    outw(IO_REG_IO_FUNC_SEL, 0x00000400);
    
    outw(IO_REG_IO_PUE, 0x2000);  //PIN13 enable pull up
    outw(IO_REG_IO_PDE, 0x0);  //PIN13 disable pull down
        
	
	// set SPI FLASH CONTROLLER back to onebit mode
#if defined(FLASH_CTL_v1)
#warning "FLASH_CTL_v1"
#elif defined(FLASH_CTL_v2)
    outw(REG_SPI_BUSY, 0x00000000);
#else
#error unsupported flash controller.
#endif

    // set SPI FLASH CONTROLLER
	outw(REG_SPI_CMD_SRAM, SRAM_ADDR_CMD_DATAOUT);
	outw(REG_SPI_DATAIN_SRAM, SRAM_ADDR_DATAIN);
}

unsigned int flash_read_spimc_data(void)
{
	unsigned int reg_val;
	inw(SRAM_ADDR_DATAIN+4);
	reg_val = inw(SRAM_ADDR_DATAIN);
	return reg_val;
}

void flash_write_enable(void)
{ 
	flash_write_cmd(WRITE_ENABLE_CMD,WRITE_ENABLE_TLEN,WRITE_ENABLE_RLEN);
	flash_wait_spi_busy();
	flash_wait_spi_status_register();
}

void flash_write_cmd(unsigned int cmd, unsigned int tlen, unsigned int rlen)
{
#if defined(FLASH_CTL_v1)
#warning "FLASH_CTL_v1"
	unsigned trlen;	//tlen + rlen 

	trlen = (tlen + rlen);
	outw(SRAM_ADDR_CMD_DATAOUT,cmd);
	
	outw(REG_SPI_CMD_LEN,trlen);
#elif defined(FLASH_CTL_v2)
	outw(SRAM_ADDR_CMD_DATAOUT,cmd);
	
	outw(REG_SPI_CMD_RLEN,rlen);
	outw(REG_SPI_CMD_TLEN,tlen);
#else
#error unsupported flash controller.
#endif
}

void flash_wait_spi_busy(void)
{
	unsigned int reg_val;
	while(1)
	{
		reg_val = inw(REG_SPI_BUSY);
		
#if defined(FLASH_CTL_v1)
#warning "FLASH_CTL_v1"
		reg_val = ((reg_val & 0x00010000) >> 16);//shuttle
#elif defined(FLASH_CTL_v2)
		reg_val = ((reg_val & 0x00000008) >> 3);//MP
#else
#error unsupported flash controller.
#endif
	 
		if(reg_val != 0x1)
		{
			break;
		}
	}
}

unsigned int flash_read_spi_status_register(unsigned int cmd, unsigned int tlen, unsigned int rlen)
{
	unsigned int reg_val;
	flash_write_cmd(cmd,tlen,rlen);
	flash_wait_spi_busy();
	 
	reg_val = inw(SRAM_ADDR_DATAIN);

#if defined(FLASH_CTL_v1)
#warning "FLASH_CTL_v1"
	reg_val = (reg_val>>24) & 0xff;
#elif defined(FLASH_CTL_v2)
	reg_val = (reg_val>>0) & 0xff;
#else
#error unsupported flash controller.
#endif
	
	return reg_val;
}

void flash_wait_spi_status_register(void)
{
	unsigned int reg_val;
	unsigned int StatusByte1;
	while(1)
	{
		reg_val = flash_read_spi_status_register( READ_STATUS1_CMD, READ_STATUS1_TLEN, READ_STATUS1_RLEN);
		StatusByte1 = reg_val & 0xff;
	 
		if((0x00==(StatusByte1 & 0x01)))
		{
			break;
		}
	}
}

unsigned int flash_get_jedec(void)
{
	unsigned int reg_val;
	unsigned int manufacture_id;
	
	flash_write_cmd(0x9f,0x1,0x3);
	flash_wait_spi_busy();
	reg_val = flash_read_spimc_data();
#if defined(FLASH_CTL_v1)
#warning "FLASH_CTL_v1"
	reg_val = (reg_val>>8) & 0x00FFFFFF;
#elif defined(FLASH_CTL_v2)
	reg_val = (reg_val>>0) & 0xFFFFFFFF;
#else
#error unsupported flash controller.
#endif
	manufacture_id = ((reg_val>>0) & 0xFF);
	
	printf("jedec: (Manufacturer,DeviceType,Capacity)=(0x%x,0x%x,0x%x,0x%x)\n",((reg_val>>0) & 0xFF),((reg_val>>8) & 0xFF),((reg_val>>16) & 0xFF),((reg_val>>24) & 0xFF));
	return manufacture_id;
}

unsigned int flash_get_jedec_ex(void)
{
	unsigned int reg_val;
	unsigned int manufacture_id;
	
	flash_write_cmd(0x9f,0x1,0x3);
	flash_wait_spi_busy();
	reg_val = flash_read_spimc_data();
#if defined(FLASH_CTL_v1)
#warning "FLASH_CTL_v1"
	reg_val = (reg_val>>8) & 0x00FFFFFF;
#elif defined(FLASH_CTL_v2)
	reg_val = (reg_val>>0) & 0xFFFFFFFF;
#else
#error unsupported flash controller.
#endif
	//manufacture_id = ((reg_val>>0) & 0xFF);
	
	printf("jedec: (Manufacturer,DeviceType,Capacity)=(0x%x,0x%x,0x%x,0x%x)\n",((reg_val>>0) & 0xFF),((reg_val>>8) & 0xFF),((reg_val>>16) & 0xFF),((reg_val>>24) & 0xFF));
	return reg_val;
}

void flash_set_status1(unsigned int status1)
{
	unsigned int cmd;
	flash_write_enable();
	cmd = ((status1 & 0xff) << 8);
	cmd = ( (WRITE_STATUS1_CMD | cmd));// cmd=0x2,24bit address: 0x001000
	flash_write_cmd(WRITE_STATUS1_CMD, WRITE_STATUS1_TLEN, WRITE_STATUS1_RLEN);
	
	flash_wait_spi_busy();
	flash_wait_spi_status_register();
}

void flash_set_status2(unsigned int status)
{
	unsigned int cmd;
	flash_write_enable();
	cmd = ((status & 0xff) << 8);
	cmd = ( (WRITE_STATUS2_CMD | cmd));// cmd=0x31,
	flash_write_cmd(cmd, WRITE_STATUS2_TLEN, WRITE_STATUS2_RLEN);
	
	flash_wait_spi_busy();
	flash_wait_spi_status_register();
}

void flash_set_status1_and_2(unsigned int status1, unsigned int status2)
{
	unsigned int cmd;
	flash_write_enable();
	
	cmd = ((status1 & 0xff) << 8) | ((status2 & 0xff) << 16);
	cmd = ( (WRITE_STATUS1_AND_2_CMD | cmd));// cmd=0x2,24bit address: 0x001000
	//len = 2;  //4 bytes for cmd
	flash_write_cmd(cmd,WRITE_STATUS1_AND_2_TLEN,WRITE_STATUS1_AND_2_RLEN);
	flash_wait_spi_busy();
	flash_wait_spi_status_register();
}

void flash_set_qe_type0(void)
{
	unsigned int status1;
	status1 = flash_read_spi_status_register(READ_STATUS1_CMD, READ_STATUS1_TLEN,READ_STATUS1_RLEN);
	status1 = status1 | 0x40;	 //qe is bit 6
	 
	flash_set_status1(status1);
}

void flash_set_qe_type1(void)
{
	unsigned int status1;
	unsigned int status2;
	
	status1 = flash_read_spi_status_register(READ_STATUS1_CMD, READ_STATUS1_TLEN,READ_STATUS1_RLEN);
	status2 = flash_read_spi_status_register(READ_STATUS2_CMD, READ_STATUS2_TLEN,READ_STATUS2_RLEN);	//qe is bit 9 -> status1 bit1
	
	status2 = 0x02; //force to set CMP=0
	flash_set_status1_and_2(status1,status2);
}

void flash_set_qe_type2(void)
{
	unsigned int status;
	status = flash_read_spi_status_register(READ_STATUS2_CMD, READ_STATUS2_TLEN,READ_STATUS2_RLEN);
	status = 0x02;	 //qe is S9 -> status2 bit1 
	
	flash_set_status2(status);
	
	//double check QE status
	status = flash_read_spi_status_register(READ_STATUS2_CMD, READ_STATUS2_TLEN,READ_STATUS2_RLEN);	
	printf("after flash_set_status2, value = 0x%x\n", status);
}

void flash_set_quadbit(void)
{
	unsigned int manufacture_id;
	unsigned int device_id;
	unsigned int capacity;
			 
	//manufacture_id = flash_get_jedec();
	int jedec_value = flash_get_jedec_ex();
	printf("jedec value = 0x%x\n", jedec_value);
	
	manufacture_id = (jedec_value>>0) & 0xFF;
	device_id = (jedec_value>>8) & 0xFF;
	capacity =  (jedec_value>>16) & 0xFF;
	printf("(manufacture, device, capacity) = (0x%x, 0x%x, 0x%x)\n", manufacture_id, device_id, capacity);
	
	if(manufacture_id == 0xC8 && device_id == 0x40 && capacity == 0x17) 
	{
		flash_set_qe_type2();		
	}
	else if(manufacture_id==0xC2)//MXIC
	{
		flash_set_qe_type0();
	}
	else
	{
		flash_set_qe_type1();
	}
}

void flash_sector_erase(unsigned int addr)
{
	unsigned int s32cmd;
	flash_write_enable();
	
	addr = ENDIAN_CHANGE(addr);
	s32cmd = (ERASE_SECTOR_CMD | addr);
	 
	flash_write_cmd(s32cmd,ERASE_SECTOR_TLEN,ERASE_SECTOR_RLEN);
	 
	flash_wait_spi_busy();
	flash_wait_spi_status_register();
}

void flash_page_program(unsigned int flash_addr, unsigned int flash_len, unsigned char* src_arr)
{
	unsigned int cmd;
	unsigned int len;
	
	if(flash_len>FLASH_PAGE_SIZE) flash_len = FLASH_PAGE_SIZE;
	
	fastout(SRAM_ADDR_CMD_DATAOUT_ADD4, flash_len, src_arr);
	flash_write_enable();
	
	cmd = ENDIAN_CHANGE(flash_addr);
	cmd = ( (PAGE_PROGRAM_CMD | cmd));	 // cmd=0x2,24bit address: 0x001000
	len = flash_len + 4;  //4 bytes for cmd
	flash_write_cmd(cmd,len,0); //rlen=0
	 
	flash_wait_spi_busy();
	flash_wait_spi_status_register();
}

void flash_fast_read(unsigned int flash_addr, unsigned int flash_len, unsigned char* dest)
{
	unsigned int cmd;
	unsigned int len;
	unsigned int *pdata;
	
	cmd = ENDIAN_CHANGE(flash_addr);
	cmd = ( (FAST_READ_CMD | cmd));
	
	outw(SRAM_ADDR_CMD_DATAOUT,cmd);
	outw(SRAM_ADDR_CMD_DATAOUT+4,0x0);
	
	outw(REG_SPI_CMD_RLEN,flash_len);
	outw(REG_SPI_CMD_TLEN,FAST_READ_TLEN);
	
	flash_wait_spi_busy();
	
	for(len=0; len<flash_len; len+=4)
	{
		pdata = &dest[len];
		*pdata = inw(SRAM_ADDR_DATAIN+len);
	}
}
