/**
 *
 * \file
 *
 * \brief This module contains M2M host interface APIs implementation.
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

#include "driver/source/nmbus.h"
#include "bsp/include/nm_bsp.h"
#include "m2m_hif.h"
#include "driver/include/m2m_types.h"
#include "driver/source/nmasic.h"
#include "driver/include/m2m_periph.h"


#if (defined NM_EDGE_INTERRUPT)&&(defined NM_LEVEL_INTERRUPT)
#error "only one type of interrupt NM_EDGE_INTERRUPT,NM_LEVEL_INTERRUPT"
#endif

#if !((defined NM_EDGE_INTERRUPT)||(defined NM_LEVEL_INTERRUPT))
#error "define interrupt type NM_EDGE_INTERRUPT,NM_LEVEL_INTERRUPT"
#endif

#ifndef CORTUS_APP
#define NMI_AHB_DATA_MEM_BASE  0x30000
#define NMI_AHB_SHARE_MEM_BASE 0xd0000

#define WIFI_HOST_RCV_CTRL_0	(0x1070)
#define WIFI_HOST_RCV_CTRL_1	(0x1084)
#define WIFI_HOST_RCV_CTRL_2    (0x1078)
#define WIFI_HOST_RCV_CTRL_3    (0x106c)
#define WIFI_HOST_RCV_CTRL_4	(0x150400)
#define WIFI_HOST_RCV_CTRL_5	(0x1088)

#ifdef CONF_WILC_USE_3000_REV_A
#define INTERRUPT_CORTUS_0_3000D0	(0x10a8)
#define INTERRUPT_CORTUS_1_3000D0	(0x10ac)
#define INTERRUPT_CORTUS_2_3000D0	(0x10b0)
#define INTERRUPT_CORTUS_3_3000D0	(0x10b4)
#endif

static volatile uint8 gu8ChipMode = 0;
static volatile uint8 gu8ChipSleep = 0;
static volatile uint8 gu8HifSizeDone = 0;
static volatile uint8 gu8Interrupt = 0;

static volatile tstrHifHdr gstrHif __M2M_DMA_BUF_ATT__;

tpfHifCallBack pfWifiCb = NULL;		/*!< pointer to Wi-Fi call back function */
tpfHifCallBack pfHifCb = NULL;

tstrHifinitParam strHifInitParam;

int enable_log;

static void isr(void)
{
	gu8Interrupt++;
#ifdef NM_LEVEL_INTERRUPT
	nm_bsp_interrupt_ctrl(0);
	DRIVER_SIGNAL_ISR();
#else

	DRIVER_SIGNAL_ISR();
#endif
}
static sint8 hif_set_rx_done(void)
{
#ifdef CONF_WILC_USE_1000_REV_B
	uint32 reg;
#endif
	sint8 ret = M2M_SUCCESS;
	gu8HifSizeDone  = 0;
#ifdef NM_EDGE_INTERRUPT
	nm_bsp_interrupt_ctrl(1);
#endif

#ifdef CONF_WILC_USE_1000_REV_B
	ret = nm_read_reg_with_ret(WIFI_HOST_RCV_CTRL_0,&reg);
	if(ret != M2M_SUCCESS)goto ERR1;
	//reg &= ~(1<<0);

	/* Set RX Done */
	reg |= (1<<1);		
	ret = nm_write_reg(WIFI_HOST_RCV_CTRL_0,reg);
	if(ret != M2M_SUCCESS)goto ERR1;
#elif defined CONF_WILC_USE_3000_REV_A
	ret = nm_write_reg(INTERRUPT_CORTUS_0_3000D0,1);
	if(ret != M2M_SUCCESS)goto ERR1;
#endif
#ifdef NM_LEVEL_INTERRUPT
	nm_bsp_interrupt_ctrl(1);
#endif
ERR1:
	return ret;
	
}
/**
*	@fn			static void m2m_hif_cb(uint8 u8OpCode, uint16 u16DataSize, uint32 u32Addr)
*	@brief		WiFi call back function
*	@param [in]	u8OpCode
*					HIF Opcode type.
*	@param [in]	u16DataSize
*					HIF data length.
*	@param [in]	pu8Buff
*					HIF data buffer.
*	@param [in]	grp
*					HIF group type.
*	@author
*	@date
*	@version	1.0
*/
static void m2m_hif_cb(uint8 u8OpCode, uint16 u16DataSize, uint8* pu8Buff)
{


}
/**
*	@fn		NMI_API sint8 hif_chip_wake(void);
*	@brief	To Wakeup the chip.
*    @return		The function shall return ZERO for successful operation and a negative value otherwise. 
*/

sint8 hif_chip_wake(void)
{
	sint8 ret = M2M_SUCCESS;
	if(gu8HifSizeDone)
	{
		/*chip already wake for the rx not done no need to send wake request*/
		return ret;
	}
	if(gu8ChipSleep == 0)
	{
		if(gu8ChipMode != M2M_NO_PS)
		{
			ret = chip_wake();
			if(ret != M2M_SUCCESS)goto ERR1;
		}
		else
		{
		}
	}
	gu8ChipSleep++;
ERR1:
	return ret;
}
/*!
@fn	\
	NMI_API void hif_set_sleep_mode(uint8 u8Pstype);

@brief
	Set the sleep mode of the HIF layer.

@param [in]	u8Pstype
				Sleep mode.
				
@return		
	The function SHALL return 0 for success and a negative value otherwise.
*/

void hif_set_sleep_mode(uint8 u8Pstype)
{
	gu8ChipMode = u8Pstype;
}
/*!
@fn	\
	NMI_API uint8 hif_get_sleep_mode(void);

@brief
	Get the sleep mode of the HIF layer.
				
@return		
	The function SHALL return the sleep mode of the HIF layer.
*/

uint8 hif_get_sleep_mode(void)
{
	return gu8ChipMode;
}

/**
*	@fn		NMI_API sint8 hif_chip_sleep_sc(void);
*	@brief	To clear the chip sleep but keep the chip sleep
*    @return		The function shall return ZERO for successful operation and a negative value otherwise.
*/

sint8 hif_chip_sleep_sc(void)
{
	if(gu8ChipSleep >= 1)
	{
		gu8ChipSleep--;
	}
	return M2M_SUCCESS;
}
/**
*	@fn		NMI_API sint8 hif_chip_sleep(void);
*	@brief	To make the chip sleep.
*    @return		The function shall return ZERO for successful operation and a negative value otherwise. 
*/

sint8 hif_chip_sleep(void)
{
	sint8 ret = M2M_SUCCESS;

	if(gu8ChipSleep >= 1)
	{
		gu8ChipSleep--;
	}
	
	if(gu8ChipSleep == 0)
	{
		if(gu8ChipMode != M2M_NO_PS)
		{
			ret = chip_sleep();
			if(ret != M2M_SUCCESS)goto ERR1;

		}
		else
		{
		}
	}
ERR1:
	return ret;
}
/**
*   @fn		NMI_API sint8 hif_init(void * arg);
*   @brief	To initialize HIF layer.
*   @param [in]	arg
*				Pointer to the arguments.
*   @return		The function shall return ZERO for successful operation and a negative value otherwise. 
*/

sint8 hif_init(tstrHifinitParam * pstrInitParam)
{
	pfWifiCb = NULL;
	
	gu8ChipSleep = 0;
	gu8ChipMode = M2M_NO_PS;

	gu8Interrupt = 0;
	nm_bsp_register_isr(isr);

	hif_register_cb(M2M_REQ_GRP_HIF,m2m_hif_cb);

	m2m_memcpy((uint8*)(&strHifInitParam), (uint8*)pstrInitParam, sizeof(tstrHifinitParam));
	
	return M2M_SUCCESS;
}
/**
*	@fn		NMI_API sint8 hif_deinit(void * arg);
*	@brief	To Deinitialize HIF layer.
*    @param [in]	arg
*				Pointer to the arguments.
*    @return		The function shall return ZERO for successful operation and a negative value otherwise. 
*/
sint8 hif_deinit(void * arg)
{
	sint8 ret = M2M_SUCCESS;
#if 0
	uint32 reg = 0, cnt=0;
	while (reg != M2M_DISABLE_PS)
	{
		nm_bsp_sleep(1);
		reg = nm_read_reg(STATE_REG);
		if(++cnt > 1000)
		{
			M2M_DBG("failed to stop power save\n");
			break;
		}
	}
#endif
	ret = hif_chip_wake();
	
	return ret;
}
/**
*	@fn		NMI_API sint8 hif_send(uint8 u8Gid,uint8 u8Opcode,uint8 *pu8CtrlBuf,uint16 u16CtrlBufSize,
					   uint8 *pu8DataBuf,uint16 u16DataSize, uint16 u16DataOffset)
*	@brief	Send packet using host interface.

*	@param [in]	u8Gid
*				Group ID.
*	@param [in]	u8Opcode
*				Operation ID.
*	@param [in]	pu8CtrlBuf
*				Pointer to the Control buffer.
*	@param [in]	u16CtrlBufSize
				Control buffer size.
*	@param [in]	u16DataOffset
				Packet Data offset.
*	@param [in]	pu8DataBuf
*				Packet buffer Allocated by the caller.
*	@param [in]	u16DataSize
				Packet buffer size (including the HIF header).
*    @return		The function shall return ZERO for successful operation and a negative value otherwise. 
*/

sint8 hif_send(uint8 u8Gid,uint8 u8Opcode,uint8 *pu8CtrlBuf,uint16 u16CtrlBufSize,
			   uint8 *pu8DataBuf,uint16 u16DataSize, uint16 u16DataOffset)
{
	sint8		ret = M2M_ERR_SEND;
#ifndef INT_BASED_TX
	int count = 0;
#endif
	gstrHif.u8Opcode		= u8Opcode&(~NBIT7);
	gstrHif.u8Gid		= u8Gid;
	gstrHif.u16Length	= M2M_HIF_HDR_OFFSET;
	if(pu8DataBuf != NULL)
	{
		gstrHif.u16Length += u16DataOffset + u16DataSize;
	}
	else
	{
		gstrHif.u16Length += u16CtrlBufSize;
	}
	ret = hif_chip_wake();
	if(ret == M2M_SUCCESS)
	{
		volatile uint32 reg, dma_addr = 0;
#ifndef INT_BASED_TX
		for (count = 0; count < 1000 && dma_addr == 0; count++)
#endif
		{
			ret = nm_read_reg_with_ret(0x150400,(uint32 *)&dma_addr);			
		}		
		if (dma_addr != 0) 
		{
			volatile uint32	u32CurrAddr;
			nm_write_reg(0x150400,0);
			u32CurrAddr = dma_addr;
			gstrHif.u16Length=NM_BSP_B_L_16(gstrHif.u16Length);
			ret = nm_write_block(u32CurrAddr, (uint8*)&gstrHif, M2M_HIF_HDR_OFFSET);
			if(M2M_SUCCESS != ret) goto ERR1;
			u32CurrAddr += M2M_HIF_HDR_OFFSET;
			if(pu8CtrlBuf != NULL)
			{
				ret = nm_write_block(u32CurrAddr, pu8CtrlBuf, u16CtrlBufSize);
				if(M2M_SUCCESS != ret) goto ERR1;
				u32CurrAddr += u16CtrlBufSize;
			}
			if(pu8DataBuf != NULL)
			{
				u32CurrAddr += (u16DataOffset - u16CtrlBufSize);
				ret = nm_write_block(u32CurrAddr, pu8DataBuf, u16DataSize);
				if(M2M_SUCCESS != ret) goto ERR1;
				u32CurrAddr += u16DataSize;
			}
			reg = dma_addr << 2;
			reg |= (1 << 1);
			ret = nm_write_reg(WIFI_HOST_RCV_CTRL_3, reg);
			if(M2M_SUCCESS != ret) goto ERR1;
#if defined CONF_WILC_USE_3000_REV_A
			ret = nm_write_reg(INTERRUPT_CORTUS_2_3000D0, 1);
			if(M2M_SUCCESS != ret) goto ERR1;
#endif
		}
		else
		{
			ret = hif_chip_sleep();
			M2M_DBG("Failed to alloc rx size %d\r\n",ret);
			ret = M2M_ERR_MEM_ALLOC;
			goto ERR2;
		}

	}
	else
	{
		M2M_ERR("(HIF)Fail to wakup the chip\n");
		goto ERR2;
	}
	/*actual sleep ret = M2M_SUCCESS*/
 	ret = hif_chip_sleep();
	return ret;
ERR1:
	/*reset the count but no actual sleep as it already bus error*/
	hif_chip_sleep_sc();
ERR2:
	/*logical error*/
	return ret;
}

/**
 *	@fn sint8 hif_send_optimized(uint8 u8Gid,uint8 u8Opcode,uint8 *buffer,uint16 u16CtrlBufSize,
 *  uint16 u16DataSize, uint16 u16DataOffset)
 *	@brief	Send packet using host interface using one SDIO transaction.
 *	This API expects a big enough buffer with offsetted data and control fields so that
 *	the buffer is sent in one SDIO operation without copying.
 *
 *	@param [in]	u8Gid
 *				Group ID.
 *	@param [in]	u8Opcode
 *				Operation ID.
 *	@param [in]	buffer
 *				Pointer to the buffer.
 *	@param [in]	u16DataSize
 *              Length of the buffer that will be sent to FW including the 
 *				HIF header, Eth Offset and PAD.
 *  @return		The function shall return ZERO for successful operation and a negative value otherwise.
 */

sint8 hif_send_optimized(uint8 u8Gid, uint8 u8Opcode, uint8* buffer, uint16 u16DataSize) {
	sint8 ret = M2M_ERR_SEND;
	volatile tstrHifHdr* strHif = (tstrHifHdr*) (buffer);
#ifndef INT_BASED_TX
	int count = 0;
#endif
	strHif->u8Opcode = u8Opcode & (~NBIT7);
	strHif->u8Gid = u8Gid;
	strHif->u16Length = u16DataSize;
	ret = hif_chip_wake();
	if (ret == M2M_SUCCESS) {
		volatile uint32 reg, dma_addr = 0;
#ifndef INT_BASED_TX
		for (count = 0; count < 1000 && dma_addr == 0; count++)
#endif
				{
			ret = nm_read_reg_with_ret(0x150400, (uint32 *) &dma_addr);
		}
		if (dma_addr != 0) {
			volatile uint32 u32CurrAddr;
			nm_write_reg(0x150400, 0);
			u32CurrAddr = dma_addr;
			strHif->u16Length = NM_BSP_B_L_16(strHif->u16Length);
			
			ret = nm_write_block(u32CurrAddr, buffer, u16DataSize);
				
			if (M2M_SUCCESS != ret)
				return ret;
			u32CurrAddr += u16DataSize;
			reg = dma_addr << 2;
			reg |= (1 << 1);
			ret = nm_write_reg(WIFI_HOST_RCV_CTRL_3, reg);
			if (M2M_SUCCESS != ret)
				return ret;
#if defined CONF_WILC_USE_3000_REV_A
			ret = nm_write_reg(INTERRUPT_CORTUS_2_3000D0, 1);
			if(M2M_SUCCESS != ret) return ret;
#endif
		} else {
			M2M_DBG("Failed to alloc rx size\r\n");
			ret = M2M_ERR_MEM_ALLOC;
			return ret;
		}
	} else {
		M2M_ERR("(HIF)Fail to wakup the chip\n");
		return ret;
	}
	ret = hif_chip_sleep();
	return ret;
}

/**
*	@fn		hif_isr
*	@brief	Host interface interrupt service routine
*	@author	M. Abdelmawla
*	@date	15 July 2012
*	@return	1 in case of interrupt received else 0 will be returned
*	@version	1.0
*/ 
static sint8 hif_isr(void)
{
	
	
	sint8 ret = M2M_ERR_BUS_FAIL;
	volatile uint32 reg,int_stat;
	volatile tstrHifHdr* pstrHif;
	
	/*read int status*/
	ret = nm_read_reg_with_ret(WILC_INT_STATUS_REG, (uint32*)&int_stat);
	int_stat >>= IRG_FLAGS_OFFSET;
	/*clear the read status*/		
	ret |= nm_write_reg(WILC_INT_CLEAR_REG,int_stat);
	if(M2M_SUCCESS == ret)
	{
#ifdef INT_BASED_TX
		if(int_stat & 0x2)	/* New interrupt has been received */
		{
			if( (int_stat&1) == 0)
				nm_bsp_interrupt_ctrl(1);
				
			if(pfWifiCb)
				pfWifiCb(M2M_WIFI_RESP_PACKET_SENT,0,0);				
		}
#endif
		if(int_stat & 0x1)	/* New interrupt has been received */
		{
			uint16 size;
			/*read packet size*/
			ret = nm_read_reg_with_ret(WIFI_HOST_RCV_CTRL_0, (uint32*)&reg);
			nm_bsp_interrupt_ctrl(0);				
			gu8HifSizeDone = 1;
			size = (uint16)((reg >> 2) & 0xfff);	
			if (size > 0) {
				uint32 address = 0;
				/**
				start bus transfer
				**/
				ret = nm_read_reg_with_ret(WIFI_HOST_RCV_CTRL_1, &address);
				if((M2M_SUCCESS != ret) || (size > strHifInitParam.u32RcvBuffSize))
				{
					M2M_ERR("(hif) WIFI_HOST_RCV_CTRL_1 bus fail or buffer is too small. packet Discarded\n");
					M2M_ERR("ret: %d, size: %d, buffersize: %lu\n", ret, size, strHifInitParam.u32RcvBuffSize);
					hif_set_rx_done();
					nm_bsp_interrupt_ctrl(1);
					goto ERR1;
				}
				ret = nm_read_block(address, strHifInitParam.pu8RcvBuff, size);
				
				//if (size==72)
				//{
					//enable_log = 1;
					//printf("rev size = %d\r\n", size);
				//}

				//for (int i=0; i< size; i++)
				//	printf("0x%x ", strHifInitParam.pu8RcvBuff[i]);
					
				if(M2M_SUCCESS != ret)
				{
					M2M_ERR("(hif) address bus fail\n");
					hif_set_rx_done();
					nm_bsp_interrupt_ctrl(1);
					goto ERR1;
				}
				pstrHif = (tstrHifHdr*)strHifInitParam.pu8RcvBuff;
				pstrHif->u16Length = NM_BSP_B_L_16(pstrHif->u16Length);
				if(pstrHif->u16Length != size)
				{
					if((size - pstrHif->u16Length) > 4)
					{
						M2M_ERR("(hif) Corrupted packet Address: %08lx, xSize = %u <L = %u, G = %u, OP = %02X>\n",
							address, size, pstrHif->u16Length, pstrHif->u8Gid, pstrHif->u8Opcode);
						hif_set_rx_done();
						nm_bsp_interrupt_ctrl(1);
						ret = M2M_ERR_BUS_FAIL;
						goto ERR1;
					}
				}
				if(M2M_REQ_GRP_WIFI == pstrHif->u8Gid)
				{
					if(pfWifiCb)
						pfWifiCb(pstrHif->u8Opcode,pstrHif->u16Length - M2M_HIF_HDR_OFFSET, strHifInitParam.pu8RcvBuff + M2M_HIF_HDR_OFFSET);
					
				} 
				else
				{
					M2M_ERR("(hif) invalid group ID\n");
					hif_set_rx_done();
					nm_bsp_interrupt_ctrl(1);
					ret = M2M_ERR_BUS_FAIL;
					goto ERR1;
				}
				ret = hif_set_rx_done();
			}
			else
			{
				M2M_ERR("(hif) Wrong Size\n");
				ret = M2M_ERR_RCV;
				goto ERR1;
			}
		}
		if (int_stat > 3 || int_stat == 0)
		{
#ifndef WIN32
			M2M_ERR("(hif) False interrupt %lx",int_stat);
#ifdef NM_LEVEL_INTERRUPT
			nm_bsp_interrupt_ctrl(1);

#endif
#endif
		}
	}
	else
	{
		M2M_ERR("(hif) Fail to Read interrupt reg\n");
		goto ERR1;
	}
ERR1:
	return ret;
}

/**
*	@fn		hif_handle_isr(void)
*	@brief	Handle interrupt received from WILC1000 firmware.
*   @return     The function SHALL return 0 for success and a negative value otherwise.
*/

sint8 hif_handle_isr(void)
{
	sint8 ret = M2M_SUCCESS;
	
	while (gu8Interrupt) {
		/*must be at that place because of the race of interrupt increment and that decrement*/
		/*when the interrupt enabled*/
		gu8Interrupt--;
		while(1)
		{
			ret = hif_isr();
			if(ret == M2M_SUCCESS) {
				/*we will try forever untill we get that interrupt*/
				/*Fail return errors here due to bus errors (reading expected values)*/
				break;
			} else {
				M2M_ERR("(HIF) Fail to handle interrupt %d try Again..\n",ret);
			}
		}
	}

	return ret;
}
/*
*	@fn		hif_receive
*	@brief	Host interface interrupt serviece routine
*	@param [in]	u32Addr
*				Receive start address
*	@param [out]	pu8Buf
*				Pointer to receive buffer. Allocated by the caller
*	@param [in]	u16Sz
*				Receive buffer size
*	@param [in]	isDone
*				If you don't need any more packets send True otherwise send false
*    @return		The function shall return ZERO for successful operation and a negative value otherwise.
*/
sint8 hif_receive(uint32 u32Addr, uint8 *pu8Buf, uint16 u16Sz, uint8 isDone)
{
	uint32 address, reg;
	uint16 size;
	sint8 ret = M2M_SUCCESS;

	ret = nm_read_reg_with_ret(WIFI_HOST_RCV_CTRL_0,&reg);
	if(ret != M2M_SUCCESS)goto ERR1;	


	size = (uint16)((reg >> 2) & 0xfff);	
	ret = nm_read_reg_with_ret(WIFI_HOST_RCV_CTRL_1,&address);
	if(ret != M2M_SUCCESS)goto ERR1;	

	/* Receive the payload */
	ret = nm_read_block(u32Addr, pu8Buf, u16Sz);
	if(ret != M2M_SUCCESS)goto ERR1;

	if(u16Sz > size)
	{
		ret = M2M_ERR_FAIL;
		M2M_ERR("APP Requested Size is larger than the recived buffer size <%d><%d>\n",u16Sz, size);
		goto ERR1;
	}
	if((u32Addr < address)||((u32Addr + u16Sz)>(address+size)))
	{
		ret = M2M_ERR_FAIL;
		M2M_ERR("APP Requested Address beyond the recived buffer address and length\n");
		goto ERR1;
	}

	/* check if this is the last packet */
	if((((address+size) - (u32Addr+u16Sz)) < 4) || isDone)
	{
		
		/* set RX done */
		ret = hif_set_rx_done();
	}
	


ERR1:
	return ret;
}

/**
*	@fn		hif_register_cb
*	@brief	To set Callback function for every compantent Component
*	@param [in]	u8Grp
*				Group to which the Callback function should be set.
*	@param [in]	fn
*				function to be set
*    @return		The function shall return ZERO for successful operation and a negative value otherwise. 
*/

sint8 hif_register_cb(uint8 u8Grp,tpfHifCallBack fn)
{
	sint8 ret = M2M_SUCCESS;
	switch(u8Grp)
	{
		case M2M_REQ_GRP_WIFI:
			pfWifiCb = fn;
			break;
		case M2M_REQ_GRP_HIF:
			pfHifCb = fn;
			break;
		default:
			M2M_ERR("GRp ? %d\n",u8Grp);
			ret = M2M_ERR_FAIL;
			break;
	}
	return ret;
}

sint8 hif_set_receive_buffer(void* pvBuffer,uint16 u16BufferLen)
{
	if ((((uint32)pvBuffer)%4) != 0) {
		M2M_ERR("Error: unaligned buffer!\r\n");
		return -1;
	}
	strHifInitParam.pu8RcvBuff = pvBuffer;
	strHifInitParam.u32RcvBuffSize = u16BufferLen;
	return 0;
}
#endif
