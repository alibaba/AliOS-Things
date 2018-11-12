/**
 *
 * \file
 *
 * \brief This module contains WILC1000 SDIO protocol bus APIs implementation.
 *
 * Copyright (c) 2016-2018 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */
#include "common/include/nm_common.h"
#ifdef CONF_WILC_USE_SDIO

#include "bus_wrapper/include/nm_bus_wrapper.h"
#include "nmsdio.h"

#define WILC_SDIO_BLOCK_SIZE 512

#define NMI_PERIPH_REG_BASE 	0x1000
#define NMI_INTR_REG_BASE (NMI_PERIPH_REG_BASE+0xa00)
#define NMI_PIN_MUX_0 			(NMI_PERIPH_REG_BASE + 0x408)
#define NMI_INTR_ENABLE 		(NMI_INTR_REG_BASE)

#define N_OK					 1
#define N_FAIL					 0
#define N_RESET					-1
#define N_RETRY					-2

static int sdio_set_func0_csa_address(uint32_t adr);
static int sdio_set_func0_block_size(uint32_t block_size);
static int sdio_set_func1_block_size(uint32_t block_size);
static int sdio_int_enable(void);

#define SDIO_ALGN_USE_STATIC_BUFFER

static int sdio_write_reg(uint32_t addr, uint32_t data)
{
#ifdef CONF_WILC_BIG_ENDIAN
	data = BYTE_SWAP(data);
#endif

	if((addr >= 0xf0) && (addr <= 0xff))
	{
		tstrNmSdioCmd52 cmd;
		cmd.read_write = 1;
		cmd.function = 0;
		cmd.raw = 0;
		cmd.address = addr;
		cmd.data = data;
		if (nm_bus_ioctl(NM_BUS_IOCTL_CMD_52, &cmd)) {
			M2M_ERR("[nmi sdio]: Failed cmd 52, read reg (%08lx) ...\n", addr);
			goto _fail_;
		}
	}
	else
	{
		tstrNmSdioCmd53 cmd;

		/**
			set the AHB address
		**/
		if (sdio_set_func0_csa_address(addr)) 
			goto _fail_;

		cmd.read_write = 1;
		cmd.function = 0;
		cmd.address = 0x10f;
		cmd.block_mode = 0;
		cmd.increment = 1;
		cmd.count = 4;
		cmd.buffer = (uint8_t *)&data;
		cmd.block_size = WILC_SDIO_BLOCK_SIZE;
	
		if (nm_bus_ioctl(NM_BUS_IOCTL_CMD_53, &cmd)) {
			M2M_ERR("[nmi sdio]: Failed cmd53, write reg (%08lx)...\n", addr);
			goto _fail_;
		}

#if 0
		if (!sdio_check_bs())
			goto _fail_;
#else
		//g_sdio.os_func.os_sleep(1);
#endif
	}

	return M2M_SUCCESS; 

_fail_:

	return M2M_ERR_BUS_FAIL;
}

static int sdio_write(uint32_t addr, uint8_t *buf, uint32_t size)
{
	tstrNmSdioCmd53 cmd;
	int nblk, nleft;
	cmd.read_write = 1;
	if (addr > 0) {
		/*
		 * has to be word aligned...
		 */
		if (size & 0x3) {
			size += 4;
			size &= ~0x3;
		}

		/*
		 * func 0 access
		 */
		cmd.function = 0;
		cmd.address = 0x10f;
	} else {
		/*
		 * has to be word aligned...
		 */
		if (size & 0x3) {
			size += 4;
			size &= ~0x3;
		}

		/*
		 * func 1 access
		 */
		cmd.function = 1;
		cmd.address = 0;
	}

	nblk = size / WILC_SDIO_BLOCK_SIZE;
	nleft = size % WILC_SDIO_BLOCK_SIZE;

	if (nblk > 0) {
		cmd.block_mode = 1;
		cmd.increment = 1;
		cmd.count = nblk;
		cmd.buffer = buf;
		cmd.block_size = WILC_SDIO_BLOCK_SIZE;
		if (addr > 0) {
			if (sdio_set_func0_csa_address(addr))
				goto _fail_;
		}
		if (nm_bus_ioctl(NM_BUS_IOCTL_CMD_53, &cmd)) {
			M2M_ERR("Failed cmd53 [%lx], block send\n", addr);
			goto _fail_;
		}
		if (addr > 0)
			addr += nblk * WILC_SDIO_BLOCK_SIZE;

		buf += nblk * WILC_SDIO_BLOCK_SIZE;
	}

	if (nleft > 0) {
		cmd.block_mode = 0;
		cmd.increment = 1;
		cmd.count = nleft;
		cmd.buffer = buf;

		cmd.block_size = WILC_SDIO_BLOCK_SIZE;

		if (addr > 0) {
			if (sdio_set_func0_csa_address(addr))
				goto _fail_;
		}
		if (nm_bus_ioctl(NM_BUS_IOCTL_CMD_53, &cmd)) {
			M2M_ERR("Failed cmd53 [%lx], bytes send\n", addr);
			goto _fail_;
		}
	}

	return M2M_SUCCESS;
_fail_:
	return M2M_ERR_BUS_FAIL;
}

static int sdio_read_reg(uint32_t addr, uint32_t *data)
{
	if((addr >= 0xf0) && (addr <= 0xff))
	{
		tstrNmSdioCmd52 cmd;
		cmd.read_write = 0;
		cmd.function = 0;
		cmd.raw = 0;
		cmd.address= addr;
		if (nm_bus_ioctl(NM_BUS_IOCTL_CMD_52, &cmd)) {
			M2M_ERR( "[nmi sdio]: Failed cmd 52, read reg (%08lx) ...\n", addr);
			goto _fail_;
		}
		*data = cmd.data;
	}
	else
	{
		tstrNmSdioCmd53 cmd;
		

		if (sdio_set_func0_csa_address(addr))
			goto _fail_;

		cmd.read_write = 0;
		cmd.function = 0;
		cmd.address = 0x10f;
		cmd.block_mode = 0;
		cmd.increment = 1;
		cmd.count = 4;
		cmd.buffer = (uint8_t *)data;

		cmd.block_size = WILC_SDIO_BLOCK_SIZE;

		if (nm_bus_ioctl(NM_BUS_IOCTL_CMD_53, &cmd)) {
			M2M_ERR( "[nmi sdio]: Failed cmd53, read reg (%08lx)...\n", addr);
			goto _fail_;
		}	

#if 0
		if (!sdio_check_bs())
			goto _fail_;
#else
		//g_sdio.os_func.os_sleep(1);
#endif
	}

#ifdef CONF_WILC_BIG_ENDIAN
	*data = BYTE_SWAP(*data);
#endif

	return M2M_SUCCESS; 

_fail_:
	return M2M_ERR_BUS_FAIL;
}

static int sdio_read(uint32_t addr, uint8_t *buf, uint32_t size)
{
	tstrNmSdioCmd53 cmd;
	int nblk, nleft;
		
	cmd.read_write = 0;
	if (addr > 0) {
		/*
		 * has to be word aligned...
		 */
		if (size & 0x3) {
			size += 4;
			size &= ~0x3;
		}

		/*
		 * func 0 access
		 */
		cmd.function = 0;
		cmd.address = 0x10f;
	} else {
		/*
		 * has to be word aligned...
		 */
		if (size & 0x3) {
			size += 4;
			size &= ~0x3;
		}

		/*
		 * func 1 access
		 */
		cmd.function = 1;
		cmd.address = 0;
	}

	nblk = size / WILC_SDIO_BLOCK_SIZE;
	nleft = size % WILC_SDIO_BLOCK_SIZE;

	if (nblk > 0) {
		cmd.block_mode = 1;
		cmd.increment = 1;
		cmd.count = nblk;
		cmd.buffer = buf;
		cmd.block_size = WILC_SDIO_BLOCK_SIZE;
		if (addr > 0) {
			if (sdio_set_func0_csa_address(addr))
				goto _fail_;
		}
		if (nm_bus_ioctl(NM_BUS_IOCTL_CMD_53, &cmd)) {
			M2M_ERR("Failed cmd53 [%lx], block read\n", addr);
			goto _fail_;
		}
		if (addr > 0)
			addr += nblk * WILC_SDIO_BLOCK_SIZE;

		buf += nblk * WILC_SDIO_BLOCK_SIZE;
	}

	if (nleft > 0) {
		cmd.block_mode = 0;
		cmd.increment = 1;
		cmd.count = nleft;
		cmd.buffer = buf;

		cmd.block_size = WILC_SDIO_BLOCK_SIZE;

		if (addr > 0) {
			if (sdio_set_func0_csa_address(addr))
				goto _fail_;
		}
		if (nm_bus_ioctl(NM_BUS_IOCTL_CMD_53, &cmd)) {
			M2M_ERR("Failed cmd53 [%lx], bytes read\n", addr);
			goto _fail_;
		}
	}

	return M2M_SUCCESS;
_fail_:
	return M2M_ERR_BUS_FAIL;
}

static int sdio_set_func0_csa_address(uint32_t adr)
{
	tstrNmSdioCmd52 cmd;

	/**
		Review: BIG ENDIAN
	**/
	cmd.read_write = 1;
	cmd.function = 0;
	cmd.raw = 0;
	cmd.address = 0x10c;
	cmd.data = (uint8_t)adr;
	if (nm_bus_ioctl(NM_BUS_IOCTL_CMD_52, &cmd)) {
		M2M_ERR( "[nmi sdio]: Failed cmd52, set 0x10c data...\n");
		M2M_ERR( "Card detection Fail\n");
		while (1) {};	// stop running
		goto _fail_;
	}

	cmd.address = 0x10d;
	cmd.data = (uint8_t)(adr>>8);
	if (nm_bus_ioctl(NM_BUS_IOCTL_CMD_52, &cmd)) {
		M2M_ERR( "[nmi sdio]: Failed cmd52, set 0x10d data...\n");
		M2M_ERR( "Card detection Fail\n");
		while (1) {};	// stop running
		goto _fail_;
	}

	cmd.address = 0x10e;
	cmd.data = (uint8_t)(adr>>16);
	if (nm_bus_ioctl(NM_BUS_IOCTL_CMD_52, &cmd)) {
		M2M_ERR( "[nmi sdio]: Failed cmd52, set 0x10e data...\n");
		M2M_ERR( "Card detection Fail\n");
		while (1) {};	// stop running
		goto _fail_;
	}

	return M2M_SUCCESS;
_fail_:
	return M2M_ERR_BUS_FAIL;
}

static int sdio_set_func0_block_size(uint32_t block_size)
{
	tstrNmSdioCmd52 cmd;

	cmd.read_write = 1;
	cmd.function = 0;
	cmd.raw = 0;
	cmd.address = 0x10;
	cmd.data = (uint8_t)block_size;
	if (nm_bus_ioctl(NM_BUS_IOCTL_CMD_52, &cmd)) {
		M2M_ERR("Failed cmd52, set 0x10 data\n");
		goto _fail_;
	}

	cmd.address = 0x11;
	cmd.data = (uint8_t)(block_size >> 8);
	if (nm_bus_ioctl(NM_BUS_IOCTL_CMD_52, &cmd)) {
		M2M_ERR("Failed cmd52, set 0x11 data\n");
		goto _fail_;
	}

	return M2M_SUCCESS;
_fail_:
	return M2M_ERR_BUS_FAIL;
}

static int sdio_set_func1_block_size(uint32_t block_size)
{
	tstrNmSdioCmd52 cmd;

	cmd.read_write = 1;
	cmd.function = 0;
	cmd.raw = 0;
	cmd.address = 0x110;
	cmd.data = (uint8_t)block_size;
	if (nm_bus_ioctl(NM_BUS_IOCTL_CMD_52, &cmd)) {
		M2M_ERR("Failed cmd52, set 0x110 data\n");
		goto _fail_;
	}
	cmd.address = 0x111;
	cmd.data = (uint8_t)(block_size >> 8);
	if (nm_bus_ioctl(NM_BUS_IOCTL_CMD_52, &cmd)) {
		M2M_ERR("Failed cmd52, set 0x111 data\n");
		goto _fail_;
	}

	return M2M_SUCCESS;
_fail_:
	return M2M_ERR_BUS_FAIL;
}


/********************************************

	Bus interfaces

********************************************/
static int sdio_int_enable(void)
{
	uint32_t reg;
	int ret;
	/**
		interrupt pin mux select
	**/
	ret = sdio_read_reg(NMI_PIN_MUX_0, &reg);
	if (ret) {
		M2M_ERR("[nmi sdio]: Failed read reg (%08x)...\n", NMI_PIN_MUX_0);
		return M2M_ERR_INIT;
	}
	reg |= (1<<8);
	ret = sdio_write_reg(NMI_PIN_MUX_0, reg);
	if (ret) {
		M2M_ERR("[nmi sdio]: Failed write reg (%08x)...\n", NMI_PIN_MUX_0);
		return M2M_ERR_INIT;
	}

	/**
		interrupt enable
	**/
	ret = sdio_read_reg(NMI_INTR_ENABLE, &reg);
	if (ret) {
		M2M_ERR("[nmi sdio]: Failed read reg (%08x)...\n", NMI_INTR_ENABLE);
		return M2M_ERR_INIT;
	}	
	reg |= (1 << (27));
	#ifdef INT_BASED_TX
	reg |= (1 << (28));
	#endif	
	ret = sdio_write_reg(NMI_INTR_ENABLE, reg);
	if (ret) {
		M2M_ERR("[nmi sdio]: Failed write reg (%08x)...\n", NMI_INTR_ENABLE);
		return M2M_ERR_INIT;
	}	
	return M2M_SUCCESS;
}

/*
*	@fn		nm_sdio_init
*	@brief	Initialize the SDIO 
*	@return	M2M_SUCCESS in case of success and M2M_ERR_BUS_FAIL in case of failure
*	@author	A. Abozaeid
*	@date	13 September 2015
*	@version	1.0
*/ 
sint8 nm_sdio_init(void)
{
	tstrNmSdioCmd52 cmd;
	int loop;

	/*
	 * function 0 csa enable
	 */
	cmd.read_write = 1;
	cmd.function = 0;
	cmd.raw = 1;
	cmd.address = 0x100;
	cmd.data = 0x80;
	if (nm_bus_ioctl(NM_BUS_IOCTL_CMD_52, &cmd)) {
		M2M_ERR("Fail cmd 52, enable csa\n");
		return M2M_ERR_BUS_FAIL;
	}

	/*
	 * function 0 block size
	 */
	if (sdio_set_func0_block_size(WILC_SDIO_BLOCK_SIZE)) {
		M2M_ERR("Fail cmd 52, set func 0 block size\n");
		return M2M_ERR_BUS_FAIL;
	}

	/*
	 * enable func1 IO
	 */
	cmd.read_write = 1;
	cmd.function = 0;
	cmd.raw = 1;
	cmd.address = 0x2;
	cmd.data = 0x2;
	if (nm_bus_ioctl(NM_BUS_IOCTL_CMD_52, &cmd)) {
		M2M_ERR("Fail cmd 52, set IOE register\n");
		return M2M_ERR_BUS_FAIL;
	}

	/*
	 * make sure func 1 is up
	 */
	cmd.read_write = 0;
	cmd.function = 0;
	cmd.raw = 0;
	cmd.address = 0x3;
	loop = 3;
	do {
		cmd.data = 0;
		if (nm_bus_ioctl(NM_BUS_IOCTL_CMD_52, &cmd)) {
			M2M_ERR("Fail cmd 52, get IOR register\n");
			return M2M_ERR_INIT;
		}
		if (cmd.data == 0x2)
			break;
	} while (loop--);

	if (loop <= 0) {
		M2M_ERR("Fail func 1 is not ready\n");
		return M2M_ERR_INIT;
	}

	/*
	 * func 1 is ready, set func 1 block size
	 */
	if (sdio_set_func1_block_size(WILC_SDIO_BLOCK_SIZE)) {
		M2M_ERR("Fail set func 1 block size\n");
		return M2M_ERR_BUS_FAIL;
	}

	/*
	 * func 1 interrupt enable
	 */
	cmd.read_write = 1;
	cmd.function = 0;
	cmd.raw = 1;
	cmd.address = 0x4;
	cmd.data = 0x3;
	if (nm_bus_ioctl(NM_BUS_IOCTL_CMD_52, &cmd)) {
		M2M_ERR("Fail cmd 52, set IEN register\n");
		return M2M_ERR_BUS_FAIL;
	}

	sdio_int_enable();

	return M2M_SUCCESS;
}

/*
*	@fn		nm_sdio_deinit
*	@brief	De-initialize the SDIO 
*	@return	M2M_SUCCESS in case of success and M2M_ERR_BUS_FAIL in case of failure
*	@author	A. Abozaeid
*	@date	13 September 2015
*	@version	1.0
*/ 
sint8 nm_sdio_deinit(void)
{
	return M2M_SUCCESS;
}
	

/*
*	@fn		nm_sdio_read_reg
*	@brief	Read register
*	@param [in]	u32Addr
*				Register address
*	@return	Register value
*	@author	A. Abozaeid
*	@date	13 September 2015
*	@version	1.0
*/ 
uint32 nm_sdio_read_reg(uint32 u32Addr)
{
	uint32 u32Val;

	sdio_read_reg(u32Addr, &u32Val);

	return u32Val;
}

/*
*	@fn		nm_sdio_read_reg_with_ret
*	@brief	Read register with error code return
*	@param [in]	u32Addr
*				Register address
*	@param [out]	pu32RetVal
*				Pointer to u32 variable used to return the read value
*	@return	M2M_SUCCESS in case of success and M2M_ERR_BUS_FAIL in case of failure
*	@author	A. Abozaeid
*	@date	13 September 2015
*	@version	1.0
*/ 
sint8 nm_sdio_read_reg_with_ret(uint32 u32Addr, uint32* pu32RetVal)
{
	sint8 s8Ret;
	s8Ret = sdio_read_reg(u32Addr,pu32RetVal);

	return s8Ret;
}

/*
*	@fn		nm_sdio_write_reg
*	@brief	write register
*	@param [in]	u32Addr
*				Register address
*	@param [in]	u32Val
*				Value to be written to the register
*	@return	M2M_SUCCESS in case of success and M2M_ERR_BUS_FAIL in case of failure
*	@author	A. Abozaeid
*	@date	13 September 2015
*	@version	1.0
*/ 
sint8 nm_sdio_write_reg(uint32 u32Addr, uint32 u32Val)
{
	sint8 s8Ret;

	s8Ret = sdio_write_reg(u32Addr, u32Val);

	return s8Ret;
}

/*
*	@fn		nm_sdio_read_block
*	@brief	Read block of data
*	@param [in]	u32Addr
*				Start address
*	@param [out]	puBuf
*				Pointer to a buffer used to return the read data
*	@param [in]	u16Sz
*				Number of bytes to read. The buffer size must be >= u16Sz
*	@return	M2M_SUCCESS in case of success and M2M_ERR_BUS_FAIL in case of failure
*	@author	A. Abozaeid
*	@date	13 September 2015
*	@version	1.0
*/ 
sint8 nm_sdio_read_block(uint32 u32Addr, uint8 *puBuf, uint16 u16Sz)
{
	return sdio_read(u32Addr, puBuf, u16Sz);
}

/*
*	@fn		nm_sdio_write_block
*	@brief	Write block of data
*	@param [in]	u32Addr
*				Start address
*	@param [in]	puBuf
*				Pointer to the buffer holding the data to be written
*	@param [in]	u16Sz
*				Number of bytes to write. The buffer size must be >= u16Sz
*	@return	M2M_SUCCESS in case of success and M2M_ERR_BUS_FAIL in case of failure
*	@author	A. Abozaeid
*	@date	13 September 2015
*	@version	1.0
*/ 
sint8 nm_sdio_write_block(uint32 u32Addr, uint8 *puBuf, uint16 u16Sz)
{
#ifdef CHECK_SDIO_BUFFER_ALIGNMENT
    if (((uint32) puBuf) % 4 != 0) {
        return -1;
    }
#endif
	return sdio_write(u32Addr, puBuf, u16Sz);
}

#endif
