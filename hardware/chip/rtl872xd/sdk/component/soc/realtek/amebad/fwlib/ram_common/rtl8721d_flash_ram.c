/**
  ******************************************************************************
  * @file    rtl8721d_flash_ram.c
  * @author
  * @version V1.0.0
  * @date    2016-05-17
  * @brief   This file provides firmware functions to manage the flash RAM functions.
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
  ****************************************************************************** 
  */

#include "ameba_soc.h"

static u32 cpu_systick;

/**
  * @brief  This function is used to handle flash write ipc interrupt.
  * @param Data: The pointer will be pass the IRQ handler
  * @param IrqStatus: this specify the IPC_ISR. 
  * @param ChanNum: this specify the IPC channel number. 
  * @retval none
  */
IMAGE2_RAM_TEXT_SECTION
void FLASH_Write_IPC_Int(VOID *Data, u32 IrqStatus, u32 ChanNum)
{
	/* To avoid gcc warnings */
	( void ) Data;
	( void ) IrqStatus;
	( void ) ChanNum;

	/*disable all interrupts first if primask is clear, or the CPU can't sleep*/
	if(__get_PRIMASK() == 0) {	
		asm volatile ("cpsid i" : : : "memory");
		//asm volatile ("cpsid f" : : : "memory");

		asm volatile ("wfe");
		asm volatile ("wfe");
		
		asm volatile ("cpsie i" : : : "memory");
		//asm volatile ("cpsie f" : : : "memory");
	} else {		
		asm volatile ("wfe");
		asm volatile ("wfe");
	}
}

/**
  * @brief  This function is used to lock CPU when write or erase flash under XIP.
  * @note   
  *		- all interrupt include systick will be stopped.
  * @retval none
  */
IMAGE2_RAM_TEXT_SECTION
void FLASH_Write_Lock(void)
{
	u32 cpu_id = IPC_CPUID();
	u32 lp_sleep_state;

	asm volatile ("cpsid i" : : : "memory");
	cpu_systick = SysTick->CTRL;	//disable systick exception
	SysTick->CTRL = 0;

	//u32 hp_sleep_state;
	/*IPC request to let the other CPU sleep*/
	if (cpu_id == 1) {	
		ipc_send_message(IPC_INT_CHAN_FLASHPG_REQ, NULL);
		while(1) {
			lp_sleep_state = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_LP_KM0_CTRL); 	/*get KM0 sleep status*/
			if(lp_sleep_state & BIT_KM0_SLEEPSYS) {
				break;
			}
		}
	} else {
#if defined (ARM_CORE_CM0)
		u32 hp_sleep_state;
		if(km4_status_on()) {
			ipc_send_message(IPC_INT_CHAN_FLASHPG_REQ, NULL);
			while(1) {
				hp_sleep_state = HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HS_PLATFORM_PARA);	/*get KM4 sleep status*/
				if(hp_sleep_state & BIT_KM4_SLEEP_STATUS) {
					break;
				}
			}
		}
#endif
	}
}

/**
  * @brief  This function is used to unlock CPU after write or erase flash under XIP.
  * @note   
  *		- all interrupt will be restored.
  * @retval none
  */
IMAGE2_RAM_TEXT_SECTION
void FLASH_Write_Unlock(void)
{
	u32 cpu_id = IPC_CPUID();
	u32 lp_sleep_state;
	//u32 hp_sleep_state;

	/*send an event using "sev" instruction to let the other CPU wake up*/
	asm volatile ("sev");
	if (cpu_id == 1) {
		while(1) {
			lp_sleep_state = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_LP_KM0_CTRL);	/*get KM0 sleep status*/
			if(!(lp_sleep_state & BIT_KM0_SLEEPSYS)) {
				break;
			}
		}
	} else {
#if defined (ARM_CORE_CM0)
		u32 hp_sleep_state;
		if(km4_status_on()) {
			while(1) {
				hp_sleep_state = HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HS_PLATFORM_PARA);	/*get KM4 sleep status*/
				if(!(hp_sleep_state & BIT_KM4_SLEEP_STATUS)) {
					break;
				}
			}
		}
#endif
	}

	SysTick->CTRL = cpu_systick;//restore systick exception
	asm volatile ("cpsie i" : : : "memory");	
}

  /**
  * @brief  This function is used to send Rx command to flash to get status register or flash id, and lock CPU when Rx
  * @param  cmd: command that need to be sent.
  * @param  read_len: the number of bytes that will be read by SPIC after sending cmd. 
  * @param  read_data: pointer to a byte array which is used to save received data.
  * @note This function is only used for rx status/flash id ,not used for read flash data.
  *		Only work in OneBitMode.
  * @retval none
  */
IMAGE2_RAM_TEXT_SECTION
void FLASH_RxCmdXIP(u8 cmd, u32 read_len, u8* read_data)
{
	FLASH_Write_Lock();

	FLASH_RxCmd(cmd, read_len, read_data);
	
	FLASH_Write_Unlock();
}
  
/**
  * @brief  FLASH_SetStatus used to set register status. FLASH_WriteEn & FLASH_WaitBusy, and lock CPU when set
  *  		are included in this function to avoid hardfault when TxCmd in XIP
  * @param    Cmd: command to be sent
  * @param    Len: the number of bytes to be sent after Cmd
  * @param    Status: pointer to byte array to be sent
  * @retval     none
  */
IMAGE2_RAM_TEXT_SECTION
void FLASH_SetStatusXIP(u8 Cmd, u32 Len, u8* Status)
{
	FLASH_Write_Lock();

	FLASH_SetStatus(Cmd, Len, Status);
	
	FLASH_Write_Unlock();
}

/**
  * @brief  FLASH_SetStatusBits set or clear status bits., used to set protect bit or quad enable bit, and lock CPU when set
  * @param    SetBits: 16bits valid, SetBits[7:0] is status1 & SetBits[15:8] is status2
  * @param    NewState: ENABLE/DISABLE
  * @retval none
  */
IMAGE2_RAM_TEXT_SECTION
void FLASH_SetStatusBitsXIP(u32 SetBits, u32 NewState)
{
	FLASH_Write_Lock();

	FLASH_SetStatusBits(SetBits, NewState);
	
	FLASH_Write_Unlock();
}
	
/**
  * @brief  This function is used to write data to flash in OneBitMode and User Mode, and lock CPU when write.
  * @param  StartAddr: Start address in flash from which SPIC writes.
  * @param  DataPhaseLen: the number of bytes that SPIC sends in Data Phase. 
  * @param  pData: pointer to a byte array that is to be sent.
  * @note   
  *		- page program(256B) time typical is 0.7ms: BaudRate=2.9Mbps, so one bit mode is enough.
  *		- page program(12B) time typical is 20+2.5*11= 47.5us BaudRate = 2.02M bps, so program 12B once is enough.
  *		- for compatibility with amebaz, which has 16-byte TX FIFO is 16 byte and max len is 16-cmdlen = 12 byte
  * @retval none
  */
IMAGE2_RAM_TEXT_SECTION
void FLASH_TxData12BXIP(u32 StartAddr, u8 DataPhaseLen, u8* pData)
{
	FLASH_Write_Lock();

	FLASH_TxData12B(StartAddr, DataPhaseLen, pData);
	Cache_Flush();
	
	FLASH_Write_Unlock();
}

/**
  * @brief  This function is used to erase flash, and lock CPU when erase. 
  * @param EraseType: can be one of the following  parameters:
  		@arg EraseChip: Erase the whole chip.
  		@arg EraseBlock: Erase specified block(64KB)
  		@arg EraseSector: Erase specified sector(4KB)
  * @param    Address should 4 byte align.The block/sector which
  * 		the address in will be erased.
  * @retval none
  */
IMAGE2_RAM_TEXT_SECTION
void FLASH_EraseXIP(u32 EraseType, u32 Address)
{
	FLASH_Write_Lock();

	FLASH_Erase(EraseType, Address);
	Cache_Flush();
	
	FLASH_Write_Unlock();
}

/**
  * @brief  This function is used to erase some dwords, and keep other dwords unchanged in one sector.
  * @param  address: Start address in flash to be erased.
  * @param  dword_num: the number of dwords to be erased. 
  * @note   
  *		- this function is just used for change some dwords in one sector.
  *		- this function will erase whole sector and then write back other dwords.
  *		- please dont use this function if not needed !!!!!!!!!!!!!!
  *		- FLASH_EraseXIP is recommended if need
  * @retval none
  */
IMAGE2_RAM_TEXT_SECTION
void FLASH_EreaseDwordsXIP(u32 address, u32 dword_num)
{
	u32 data[2];
	u32 idx = 0;
	u32 opt_sector = address & ~(0xFFF);
	u32 erase_addr = address;
	u32 erase_num = dword_num;

	/* erase backup sector */
	FLASH_EraseXIP(EraseSector, FLASH_RESERVED_DATA_BASE);

	/* backup this sector */
	for (idx = 0; idx < 0x1000; idx += 4) {
		u32 read_addr = opt_sector + idx;
		
		_memcpy(data, (const void *)(SPI_FLASH_BASE + read_addr), 4);

		if (erase_num > 0) {
			if (erase_addr == read_addr) {
				data[0] = 0xFFFFFFFF;
				erase_addr += 4;
				erase_num--;
			}
		}
		
		FLASH_TxData12BXIP((FLASH_RESERVED_DATA_BASE + idx), 4, (u8*)data);
	}

	/* erase this sector */
	FLASH_EraseXIP(EraseSector, opt_sector);

	/* write this sector with target data erased */
	for (idx = 0; idx < 0x1000; idx += 8) {
		_memcpy(data, (const void *)(SPI_FLASH_BASE + FLASH_RESERVED_DATA_BASE + idx), 8);
		FLASH_TxData12BXIP((opt_sector + idx), 8, (u8*)data);
	}
}

/**
  * @brief  This function is used to write data to flash in OneBitMode and User Mode, and lock CPU when write.
  * @param  StartAddr: Start address in flash from which SPIC writes.
  * @param  DataPhaseLen: the number of bytes that SPIC sends in Data Phase. 
  * @param  pData: pointer to a byte array that is to be sent.
  * @note   
  *		- page program(256B) time typical is 0.7ms: BaudRate=2.9Mbps, so one bit mode is enough.
  *		- should use FLASH_SW_CS_Control to protect flash write
  * @retval none
  */
IMAGE2_RAM_TEXT_SECTION
void FLASH_TxData256BXIP(u32 StartAddr, u32 DataPhaseLen, u8* pData)
{
	FLASH_Write_Lock();

	FLASH_TxData256B(StartAddr, DataPhaseLen, pData);

	Cache_Flush();	
	FLASH_Write_Unlock();
}

/**
  * @brief  Read a stream of data from specified address
  * @param obj: Flash object define in application software.
  * @param  address: Specifies the starting address to read from.
  * @param  len: Specifies the length of the data to read.
  * @param  data: Specified the address to save the readback data.
  * @retval   status: Success:1 or Failure: Others.
  * @note auto mode is ok, because we have flash cache
  */
IMAGE2_RAM_TEXT_SECTION
int  FLASH_ReadStream(u32 address, u32 len, u8 * data)
{
	assert_param(data != NULL);

	u32 offset_to_align;
	u32 i;
	u32 read_word;
	u8 *ptr;
	u8 *pbuf;

	offset_to_align = address & 0x03;
	pbuf = data;
	if (offset_to_align != 0) {
		/* the start address is not 4-bytes aligned */
		read_word = HAL_READ32(SPI_FLASH_BASE, (address - offset_to_align));
		ptr = (u8*)&read_word + offset_to_align;
		offset_to_align = 4 - offset_to_align;
		for (i=0;i<offset_to_align;i++) {
			*pbuf = *(ptr+i);
			pbuf++;
			len--;
			if (len == 0) {
				break;
			}
		}
	}

	/* address = next 4-bytes aligned */
	address = (((address-1) >> 2) + 1) << 2;

	ptr = (u8*)&read_word;
	if ((u32)pbuf & 0x03) {
		while (len >= 4) {
			read_word = HAL_READ32(SPI_FLASH_BASE, address);
			for (i=0;i<4;i++) {
				*pbuf = *(ptr+i);
				pbuf++;
			}
			address += 4;
			len -= 4;
		}
	} else {
		while (len >= 4) {
			*((u32 *)pbuf) = HAL_READ32(SPI_FLASH_BASE, address);
			pbuf += 4;
			address += 4;
			len -= 4;
		}
	}

	if (len > 0) {
		read_word = HAL_READ32(SPI_FLASH_BASE, address);
		for (i=0;i<len;i++) {
			*pbuf = *(ptr+i);
			pbuf++;
		}        
	}

	return 1;
}

/**
  * @brief  Write a stream of data to specified address
  * @param  address: Specifies the starting address to write to.
  * @param  len: Specifies the length of the data to write.
  * @param  data: Pointer to a byte array that is to be written.
  * @retval   status: Success:1 or Failure: Others.
  */
IMAGE2_RAM_TEXT_SECTION
int  FLASH_WriteStream(u32 address, u32 len, u8 * data)
{
	// Check address: 4byte aligned & page(256bytes) aligned
	u32 page_begin = address &  (~0xff);                     
	u32 page_end = (address + len) & (~0xff);
	u32 page_cnt = ((page_end - page_begin) >> 8) + 1;

	u32 addr_begin = address;
	u32 addr_end = (page_cnt == 1) ? (address + len) : (page_begin + 0x100);
	u32 size = addr_end - addr_begin;
	u8 *buffer = data;
	u8 write_data[12];
	
	u32 offset_to_align;
	u32 read_word;
	u32 i;

	FLASH_Write_Lock();
	while(page_cnt){	
		offset_to_align = addr_begin & 0x3;
		
		if(offset_to_align != 0){
			read_word = HAL_READ32(SPI_FLASH_BASE, addr_begin - offset_to_align);
			for(i = offset_to_align;i < 4;i++){
				read_word = (read_word &  (~(0xff << (8*i)))) |( (*buffer) <<(8*i));
				size--;
				buffer++;
				if(size == 0)
					break;
			}
			FLASH_TxData12B(addr_begin - offset_to_align, 4, (u8*)&read_word);
		}

		addr_begin = (((addr_begin-1) >> 2) + 1) << 2;
		for(;size >= 12 ;size -= 12){
			_memcpy(write_data, buffer, 12);
			FLASH_TxData12B(addr_begin, 12, write_data);
			
			buffer += 12;
			addr_begin += 12;
		}

		for(;size >= 4; size -=4){
			_memcpy(write_data, buffer, 4);			
			FLASH_TxData12B(addr_begin, 4, write_data);
			
			buffer += 4;
			addr_begin += 4;
		}

		if(size > 0){
			read_word = HAL_READ32(SPI_FLASH_BASE, addr_begin);
			for( i = 0;i < size;i++){
				read_word = (read_word & (~(0xff << (8*i)))) | ((*buffer) <<(8*i));
				buffer++;
			}
			FLASH_TxData12B(addr_begin, 4, (u8*)&read_word); 
		}

		page_cnt--;
		addr_begin = addr_end;
		addr_end = (page_cnt == 1) ? (address + len) : (((addr_begin>>8) + 1)<<8);
		size = addr_end - addr_begin;		
	}

	Cache_Flush();
	FLASH_Write_Unlock();

	return 1;
}

/**
  * @brief    Configure SPIC IP Clock.
  * @param  Source:  This parameter can be one of the following values:
  *                            @arg BIT_SHIFT_FLASH_CLK_XTAL
  *                            @arg BIT_SHIFT_FLASH_CLK_PLL
  * @param  Protection:  if disable interrupt when switch clock:
  * @retval   None
  */
IMAGE2_RAM_TEXT_SECTION
void FLASH_ClockSwitch(u32 Source, u32 Protection)
{
	/* To avoid gcc warnings */
	( void ) Source;
	( void ) Protection;
#if defined (ARM_CORE_CM0)
	//SPIC_TypeDef *spi_flash = SPIC;
	u32 Temp = 0;
	u32 timeout = 20;

	if (Protection) {
		asm volatile ("cpsid i" : : : "memory");
		//asm volatile ("cpsid f" : : : "memory");
		SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
	}

	/* sequence should be followed strickly */
	if (Source == BIT_SHIFT_FLASH_CLK_XTAL) {
		/* 1. clock source switch to XTAL */
		Temp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_LP_CLK_CTRL0);
		Temp &= ~(BIT_MASK_FLASH_CLK_SEL << BIT_SHIFT_FLASH_CLK_SEL);
		Temp |= Source;
		HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_LP_CLK_CTRL0, Temp);
		DelayUs(10);

		/* 2. close 400M & 400MPS */
		Temp = HAL_READ32(PERI_ON_BASE, REG_LP_FLASH_CTRL);
		Temp &= ~(BIT_FLASH_CK_PS_DIV_EN | BIT_FLASH_CK_DIV_EN); /* disable clock ps div & disable clock div*/
		HAL_WRITE32(PERI_ON_BASE, REG_LP_FLASH_CTRL, Temp);
		FLASH_CalibrationNewCmd(DISABLE);

		Temp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_PLL_SYS_PS_REG);
		Temp &= ~BIT_EN_CK_400M;
		HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_PLL_SYS_PS_REG, Temp);

		Temp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_PLL_SYS_PS_REG);
		Temp &= ~BIT_POW_CKGEN_400M;
		HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_PLL_SYS_PS_REG, Temp);
		
		/* 3. SPIC Dummy to low speed dummy */
		flash_init_para.FLASH_rd_sample_phase = SPIC_LOWSPEED_SAMPLE_PHASE;
		FLASH_SetSpiMode(&flash_init_para, flash_init_para.FLASH_cur_bitmode);
	} else if (Source == BIT_SHIFT_FLASH_CLK_PLL) {
		/* 1. enable 400M & 400MPS */
		Temp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_PLL_SYS_PS_REG);
		Temp |= BIT_POW_CKGEN_400M;
		HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_PLL_SYS_PS_REG, Temp);

		Temp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_PLL_SYS_PS_REG);
		Temp |= BIT_EN_CK_400M;
		HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_PLL_SYS_PS_REG, Temp);
		
		FLASH_CalibrationNewCmd(ENABLE);
		Temp = HAL_READ32(PERI_ON_BASE, REG_LP_FLASH_CTRL);
		Temp |= (BIT_FLASH_CK_PS_DIV_EN | BIT_FLASH_CK_DIV_EN); /* enable clock ps div & enable clock div*/
		HAL_WRITE32(PERI_ON_BASE, REG_LP_FLASH_CTRL, Temp);

		/* wait clock ready about 40us */
		while (timeout > 0) {
			timeout--;
			Temp = HAL_READ32(PERI_ON_BASE, REG_LP_FLASH_CTRL);
			if ((Temp & BIT_FLASH_PS_DIV_RDY) && (Temp & BIT_FLASH_DIV_RDY))
				break;
		}
			
		/* 2. clock source switch */
		Temp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_LP_CLK_CTRL0);
		Temp &= ~(BIT_MASK_FLASH_CLK_SEL << BIT_SHIFT_FLASH_CLK_SEL);
		Temp |= Source;
		HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_LP_CLK_CTRL0, Temp);
		DelayUs(10);
		
		/* 3. SPIC Dummy to high speed dummy */
		flash_init_para.FLASH_rd_sample_phase = flash_init_para.FLASH_rd_sample_phase_cal;
		FLASH_SetSpiMode(&flash_init_para, flash_init_para.FLASH_cur_bitmode);	
	}

	if (Protection) {
		SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
		asm volatile ("cpsie i" : : : "memory");
		//asm volatile ("cpsie f" : : : "memory");
	}
#endif
}

IMAGE2_RAM_TEXT_SECTION
void FLASH_Invalidate_Auto_Write(void)
{
	/* Auto write related bits in valid command register are all set to 0,
		just need to invalidate write single and write enable cmd in auto mode. */
	SPIC_TypeDef *spi_flash = SPIC;

	/* Disable SPI_FLASH User Mode */
	spi_flash->ssienr = 0;
	
	/* Invalidate write single and write enable cmd in auto mode */
	spi_flash->wr_single = 0x0;
	spi_flash->wr_enable = 0x0;
}

/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
