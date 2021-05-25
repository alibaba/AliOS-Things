/**
  ******************************************************************************
  * @file    rtl8721d_ota.c
  * @author
  * @version V1.0.0
  * @date    2016-12-19
  * @brief   This file contains the code which is used to update firmware over the air(OTA) in local area network
  *
  *  @verbatim
  *      
  *          ===================================================================
  *                                 How to use the local OTA upgrade code
  *          ===================================================================
  *          1. Firstly, read the OTA related documents to know about how the code to realize updating firmware
  *              over the air(OTA), and some protocol included in it.
  *
  *          2. Read the source code and APIs in this file.
  *
  *          3. Porting this code in this file to the specified cloud platform according to the upgrade flow and parameters of
  *              the specified cloud service providers
  *
  *          4. Test the code after porting on the specified cloud platform.
  *
  *          5. Generate the release verision that will run. 
  *
  *          ===================================================================
  *                                 the basic flow of the local ota upgrade code
  *          ===================================================================
  *          (1) Connects to server
  *
  *          (2) Receive newer firmware file header from server
  *
  *          (3) Parse firmware file header and get the target OTA image header
  *
  *          (4) Erase flash space for new firmware
  *
  *          (5) Download new firmware from server and write it to flash
  *
  *          (6) Verify checksum and update signature
  *
  *	      (7) OTA upgrade successfully, restart device
  *  @endverbatim
  *
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
  ****************************************************************************** 
  */
#include <stdlib.h>
#include <string.h>
#include <FreeRTOS.h>
//#include <task.h>
#include <lwip/sockets.h>
//#include <sys.h>

#include <device_lock.h>
#include "ameba_soc.h"
#include "lwip/netdb.h"

#include "osdep_service.h"
//sys_thread_t TaskOTA = NULL;

#if (SERVER_TYPE == SERVER_LOCAL)

typedef struct
{
	uint32_t	ip_addr;
	uint16_t	port;
}update_cfg_local_t;

const u32 IMG_ADDR[MAX_IMG_NUM][2] = {
	{LS_IMG2_OTA1_ADDR, LS_IMG2_OTA2_ADDR},
};

/**
  * @brief  Allocate memory from heap.
  * @param  size: the number of bytes to be allocated.
  * @retval The pointer to the allocated memory.
  */   
void* ota_update_malloc(unsigned int size)
{
	return pvPortMalloc(size);
}

/**
  * @brief  Deallocate memory from heap.
  * @param  buf: the pointer to the memory to be deallocated.
  * @retval none
  */   
void ota_update_free(void *buf)
{
	vPortFree(buf);
}

/**
  * @brief  Reset CPU
  * @param  none
  * @retval none
  */ 
void ota_platform_reset(void)
{
	WDG_InitTypeDef WDG_InitStruct;
	u32 CountProcess;
	u32 DivFacProcess;
	
	vTaskDelay(100);

	/* CPU reset: Cortex-M3 SCB->AIRCR*/
	//NVIC_SystemReset();	
	
#if defined(CONFIG_MBED_API_EN) && CONFIG_MBED_API_EN
	rtc_backup_timeinfo();
#endif

	WDG_Scalar(50, &CountProcess, &DivFacProcess);
	WDG_InitStruct.CountProcess = CountProcess;
	WDG_InitStruct.DivFacProcess = DivFacProcess;
	WDG_Init(&WDG_InitStruct);

	WDG_Cmd(ENABLE);
}

/**
  * @brief  Read a stream of data from specified address in user mode
  * @param obj: Flash object define in application software.
  * @param  address: Specifies the starting address to read from.
  * @param  len: Specifies the length of the data to read.
  * @param  data: Specified the address to save the readback data.
  * @retval   status: Success:1 or Failure: Others.
  * @note SPIC user mode is used because this mode can bypass RSIP(include OTF and MMU).
  *           User mode can read original data from physical address without decrypting, which is
  *           useful when calculate checksum.
  */
IMAGE2_RAM_TEXT_SECTION
int  ota_readstream_user(u32 address, u32 len, u8 * data)
{
	assert_param(data != NULL);

	u32 offset_to_align;
	u32 i;
	u32 read_word;
	u8 *ptr;
	u8 *pbuf;

	FLASH_Write_Lock();

	offset_to_align = address & 0x03;
	pbuf = data;
	if (offset_to_align != 0) {
		/* the start address is not 4-bytes aligned */
		FLASH_RxData(0, address - offset_to_align, 4, (u8*)&read_word);
		
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
			FLASH_RxData(0, address, 4, (u8*)&read_word);
			
			for (i=0;i<4;i++) {
				*pbuf = *(ptr+i);
				pbuf++;
			}
			address += 4;
			len -= 4;
		}
	} else {
		while (len >= 4) {
			FLASH_RxData(0, address, 4, pbuf);
			
			pbuf += 4;
			address += 4;
			len -= 4;
		}
	}

	if (len > 0) {
		FLASH_RxData(0, address, 4, (u8*)&read_word);
		
		for (i=0;i<len;i++) {
			*pbuf = *(ptr+i);
			pbuf++;
		}        
	}

	FLASH_Write_Unlock();

	return 1;
}

/**
  * @brief  Write a stream of data to specified address in user mode
  * @param  address: Specifies the starting address to write to.
  * @param  len: Specifies the length of the data to write.
  * @param  data: Pointer to a byte array that is to be written.
  * @retval   status: Success:1 or Failure: Others.
  * @note   SPIC user mode is used because this mode can bypass RSIP(include OTF and MMU).
  *           User mode can read original data from physical address without decrypting, which is
  *           useful when address or len is not 4 byte aligned.
  */
IMAGE2_RAM_TEXT_SECTION
int  ota_writestream_user(u32 address, u32 len, u8 * data)
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
			FLASH_RxData(0, addr_begin - offset_to_align, 4, (u8*)&read_word);
			
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
			FLASH_RxData(0, addr_begin, 4, (u8*)&read_word);
			
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

	DCache_Invalidate(address, len);
	FLASH_Write_Unlock();

	return 1;
}

/**
  * @brief  get current image2 location
  * @param  none
  * @retval  The retval can be one of the followings:
  *              OTA_INDEX_1: current images located in OTA1 address space
  *              OTA_INDEX_2: current images located in OTA2 address space
  */   
u32 ota_get_cur_index(void)
{
	u32 AddrStart, Offset, IsMinus, PhyAddr;;

	RSIP_REG_TypeDef* RSIP = ((RSIP_REG_TypeDef *) RSIP_REG_BASE);
	u32 CtrlTemp = RSIP->FLASH_MMU[0].MMU_ENTRYx_CTRL;

	if (CtrlTemp & MMU_BIT_ENTRY_VALID) {
		AddrStart = RSIP->FLASH_MMU[0].MMU_ENTRYx_STRADDR;
		Offset = RSIP->FLASH_MMU[0].MMU_ENTRYx_OFFSET;
		IsMinus = CtrlTemp & MMU_BIT_ENTRY_OFFSET_MINUS;

		if(IsMinus)
			PhyAddr = AddrStart - Offset;
		else
			PhyAddr = AddrStart + Offset;

		if(PhyAddr == LS_IMG2_OTA1_ADDR)
			return OTA_INDEX_1;
		else if(PhyAddr == LS_IMG2_OTA2_ADDR)
			return OTA_INDEX_2;
	}

	return OTA_INDEX_1;
}

/**
  * @brief  disable flash run time decrypt in some special FLASH area
  * @param Addr: FLASH area address (should 4k alignment)
  * @param Len: number of bytes
  * @param NewStatus This parameter can be one of the following values
  *		 @arg DISABLE close this area run time decypt mask
  *		 @arg ENABLE enable this area run time decypt mask (this area will not be decrypt when read)
  */ 
void ota_rsip_mask(u32 addr, u32 len, u8 status)
{
	u32 NewImg2BlkSize = ((len - 1)/4096) + 1;

	RSIP_OTF_Mask(1, addr, NewImg2BlkSize, status);
	DCache_Invalidate(addr, len);
}

/**
  * @brief  receive file_info from server. This operation is patched for the compatibility with ameba.
  * @param  Recvbuf: point for receiving buffer
  * @param  len: length of file info
  * @param  socket: socket handle
  * @retval 0: receive fail, 1: receive ok
  */   
u32 recv_file_info_from_server(u8 * Recvbuf, u32 len, int socket)
{
	int read_bytes = 0;
	u32 TempLen;
	u8 * buf;
	
	/*read 4 Dwords from server, get image header number and header length*/
	buf = Recvbuf;
	TempLen = len;
	while(TempLen > 0) {
		read_bytes = read(socket, buf, TempLen);
		if(read_bytes < 0){
			printf("\n\r[%s] read socket failed\n", __FUNCTION__);
			goto error;
		}
		if(read_bytes == 0) {
			break;
		}
		TempLen -= read_bytes;
		buf += read_bytes;
	}

	return 1;
error:
	return 0;
}

/**
  * @brief	  receive OTA firmware file header from server.
  * @param  Recvbuf: pointer to buffer for receiving OTA header of firmware file
  * @param  len: data length to be received from server
  * @param  pOtaTgtHdr: point to target image OTA  header
  * @param  socket: socket handle
  * @retval 0: receive fail, 1: receive ok
  */  
u32 recv_ota_file_hdr(u8 * Recvbuf, u32 * len, update_ota_target_hdr * pOtaTgtHdr, int socket)
{
	int read_bytes = 0;
	u32 TempLen;
	u8 * buf;
	update_file_hdr * pOtaFileHdr;
	update_file_img_hdr * pOtaFileImgHdr;
	
	/*read 4 Dwords from server, get image header number and header length*/
	buf = Recvbuf;
	TempLen = 16;
	while(TempLen > 0) {
		read_bytes = read(socket, buf, TempLen);
		if(read_bytes < 0){
			printf("\n\r[%s] read socket failed\n", __FUNCTION__);
			goto error;
		}
		if(read_bytes == 0) {
			break;
		}
		TempLen -= read_bytes;
		buf += read_bytes;
	}

	pOtaFileHdr = (update_file_hdr *)Recvbuf;
	pOtaFileImgHdr = (update_file_img_hdr *)(Recvbuf + 8);

	pOtaTgtHdr->FileHdr.FwVer = pOtaFileHdr->FwVer;
	pOtaTgtHdr->FileHdr.HdrNum = pOtaFileHdr->HdrNum;

	/*read the remaining Header info*/
	buf = Recvbuf + 16;
	TempLen =  (pOtaFileHdr->HdrNum * pOtaFileImgHdr->ImgHdrLen) - 8;
	while(TempLen > 0) {
		read_bytes = read(socket, buf, TempLen);
		if(read_bytes < 0){
			printf("\n\r[%s] read socket failed\n", __FUNCTION__);
			goto error;
		}
		if(read_bytes == 0) {
			break;
		}
		TempLen -= read_bytes;
		buf += read_bytes;
	}

	*len = (pOtaFileHdr->HdrNum * pOtaFileImgHdr->ImgHdrLen) + 8;

	return 1;
error:
	return 0;
}

/**
  * @brief	  parse firmware file header and get the desired OTA header
  * @param   buf: point to buffer for receiving OTA header of new firmware
  * @param   len: data length to be received from server
  * @param   pOtaTgtHdr:point to target image OTA  header
  * @param   ImgId: point to image identification strings
  * @retval 0: receive fail, 1: receive ok
  */ 
u32 get_ota_tartget_header(u8* buf, u32 len, update_ota_target_hdr * pOtaTgtHdr, u8 target_idx)
{
	update_file_img_hdr * ImgHdr;
	update_file_hdr * FileHdr;
	u8 * pTempAddr;
	u32 i = 0, j = 0;
	int index = -1;

	/*check if buf and len is valid or not*/
	if((len < (sizeof(update_file_img_hdr) + 8)) || (!buf)) {
		goto error;
	}

	FileHdr = (update_file_hdr *)buf;
	ImgHdr = (update_file_img_hdr *)(buf + 8);
	pTempAddr = buf + 8;

	if(len < (FileHdr->HdrNum * ImgHdr->ImgHdrLen + 8)) {
		goto error;
	}

	/*get the target OTA header from the new firmware file header*/
	for(i = 0; i < FileHdr->HdrNum; i++) {
		index = -1;
		pTempAddr = buf + 8 + ImgHdr->ImgHdrLen * i;

		if(strncmp("OTA", (const char *)pTempAddr, 3) == 0)
			index = 0;
		else
			goto error;

		if(index >= 0) {
			_memcpy((u8*)(&pOtaTgtHdr->FileImgHdr[j]), pTempAddr, sizeof(update_file_img_hdr));
			pOtaTgtHdr->FileImgHdr[j].FlashAddr = IMG_ADDR[index][target_idx];	
			j++;
		}
	}

	pOtaTgtHdr->ValidImgCnt = j;

	if(j == 0) {
		printf("\n\r[%s] no valid image\n", __FUNCTION__);
		goto error;
	}

	return 1;
error:
	return 0;
}

/**
  * @brief	  erase the flash space for new firmware.
  * @param   addr: new image address
  * @param   len: new image length
  * @retval  none
  */ 
void erase_ota_target_flash( u32 addr, u32 len)
{
	u32 sector_cnt;
	u32 i;

	sector_cnt = ((len - 1)/4096) + 1;

	device_mutex_lock(RT_DEV_LOCK_FLASH);
	for( i = 0; i < sector_cnt; i++)
		FLASH_EraseXIP(EraseSector, addr -SPI_FLASH_BASE + i * 4096);
	
	device_mutex_unlock(RT_DEV_LOCK_FLASH);
}

/**
  * @brief	   download new firmware from server and write it to flash.
  * @param     addr: new image address
  * @param     socket: socket handle
  * @param     pOtaTgtHdr: point to target image OTA  header
  * @param     signature: point to signature strings
  * @retval   download size of OTA image
  */ 
u32 download_new_fw_from_server(int socket, update_ota_target_hdr * pOtaTgtHdr, u8 targetIdx)
{
	/* To avoid gcc warnings */
	( void ) targetIdx;
	
	u8 * alloc;
	u8 * buf;
	s32 size = 0;
	int read_bytes;
	int read_bytes_buf;
	u32 TempLen;
	u32 ImageCnt;
	update_dw_info DownloadInfo[MAX_IMG_NUM];
	
	/*initialize the variables used in downloading procedure*/
	u32 OtaFg = 0;
	u32 IncFg = 0;
	s32 RemainBytes;
	u32 SigCnt = 0;
	u32 TempCnt = 0;	
	u32 i;
	u8 res = _TRUE;
	u8 * signature;

	/*acllocate buffer for downloading image from server*/
	alloc = ota_update_malloc(BUF_SIZE);

	buf = alloc;

	/*init download information buffer*/
	memset((u8 *)DownloadInfo, 0, MAX_IMG_NUM*sizeof(update_dw_info));

	ImageCnt = pOtaTgtHdr->ValidImgCnt;
	for(i = 0; i < ImageCnt; i++) {
		/* get OTA image and Write New Image to flash, skip the signature, 
			not write signature first for power down protection*/
		DownloadInfo[i].ImgId = OTA_IMAG;
		DownloadInfo[i].FlashAddr = pOtaTgtHdr->FileImgHdr[i].FlashAddr - SPI_FLASH_BASE + 8;
		DownloadInfo[i].ImageLen = pOtaTgtHdr->FileImgHdr[i].ImgLen - 8; /*skip the signature*/
		DownloadInfo[i].ImgOffset = pOtaTgtHdr->FileImgHdr[i].Offset;
	}

	/*initialize the reveiving counter*/
	TempLen = (pOtaTgtHdr->FileHdr.HdrNum * pOtaTgtHdr->FileImgHdr[0].ImgHdrLen) + sizeof(update_file_hdr);
	
	/*downloading parse the OTA and RDP image from the data stream sent by server*/
	for(i = 0; i < ImageCnt; i++) {

		/*the next image length*/
		RemainBytes = DownloadInfo[i].ImageLen;
		signature = &pOtaTgtHdr->Sign[i][0];

		/*download the new firmware from server*/
		while(RemainBytes > 0){
			buf = alloc;
			if(IncFg == 1) {
				IncFg = 0;
				read_bytes = read_bytes_buf;
			} else {
				memset(buf, 0, BUF_SIZE);
				read_bytes = read(socket, buf, BUF_SIZE);
				if(read_bytes == 0){
					break; // Read end			
				}
				if(read_bytes < 0){
					//OtaImgSize = -1;
					printf("\n\r[%s] Read socket failed", __FUNCTION__);
					res = _FALSE;
					goto exit;
				}
				read_bytes_buf = read_bytes;
				TempLen += read_bytes;
			}
			
			if(TempLen > DownloadInfo[i].ImgOffset) {
				if(!OtaFg) {
					/*reach the desired image, the first packet process*/
					OtaFg = 1;
					TempCnt = TempLen -DownloadInfo[i].ImgOffset;
					if(TempCnt < 8) {
						SigCnt = TempCnt;
					} else {
						SigCnt = 8;
					}

					_memcpy(signature, buf + read_bytes -TempCnt, SigCnt);

					if((SigCnt < 8) || (TempCnt -8 == 0)) {
						continue;
					}

					buf = buf + (read_bytes -TempCnt + 8);
					read_bytes = TempCnt -8;
				} else {
					/*normal packet process*/
					if(SigCnt < 8) {
						if(read_bytes < (int)(8 -SigCnt)) {
							_memcpy(signature + SigCnt, buf, read_bytes);
							SigCnt += read_bytes;
							continue;
						} else {
							_memcpy(signature + SigCnt, buf, (8 -SigCnt));
							buf = buf + (8 - SigCnt);
							read_bytes -= (8 - SigCnt) ;
							SigCnt = 8;
							if(!read_bytes) {
								continue;
							}
						}
					}
				}
				
				RemainBytes -= read_bytes;
				if(RemainBytes < 0) {
					read_bytes = read_bytes -(-RemainBytes);
				}

				device_mutex_lock(RT_DEV_LOCK_FLASH);
				if(ota_writestream_user(DownloadInfo[i].FlashAddr + size, read_bytes, buf) < 0){
					printf("\n\r[%s] Write sector failed", __FUNCTION__);
					device_mutex_unlock(RT_DEV_LOCK_FLASH);
					res = _FALSE;
					goto exit;
				}
				device_mutex_unlock(RT_DEV_LOCK_FLASH);			
				size += read_bytes;
			}
		}

		printf("\n\rUpdate file size: %d bytes, start addr:%08x", size + 8, pOtaTgtHdr->FileImgHdr[i].FlashAddr);
		if((u32)size != (pOtaTgtHdr->FileImgHdr[i].ImgLen - 8)) {
			printf("\n\rdownload new firmware failed\n");
			goto exit;
		}
		
		/*update flag status*/
		size = 0;
		OtaFg = 0;
		IncFg = 1;
	}

exit:
	ota_update_free(alloc);

	return res;
}

/**
  * @brief	  verify new firmware checksum.
  * @param  addr: new image address
  * @param  len: new image length
  * @param  signature: point to signature strings
  * @param  pOtaTgtHdr: point to target image OTA  header
  * @retval 0: verify fail, 1: verify ok
  */ 
u32 verify_ota_checksum(update_ota_target_hdr * pOtaTgtHdr)
{
	u32 i, index;
	u32 flash_checksum=0;
	u32 addr, len;
	u8 * signature;
	u8 * pTempbuf;
	int k;
	int rlen;
	u8 res = _TRUE;

	pTempbuf = ota_update_malloc(BUF_SIZE);

	for(index = 0; index < pOtaTgtHdr->ValidImgCnt; index++) {
		flash_checksum = 0;
		
		addr = pOtaTgtHdr->FileImgHdr[index].FlashAddr;
		len = pOtaTgtHdr->FileImgHdr[index].ImgLen - 8;
		signature = &pOtaTgtHdr->Sign[index][0];
	
		/* read flash data back and calculate checksum */
		for(i=0;i<len;i+=BUF_SIZE){
			rlen = (len-i)>BUF_SIZE?BUF_SIZE:(len-i);
			ota_readstream_user(addr - SPI_FLASH_BASE+i+8, rlen, pTempbuf);

			for(k=0;k<rlen;k++)
				flash_checksum+=pTempbuf[k];
		}
		/*add signature's checksum*/
		for(i = 0; i < 8; i++) {
			flash_checksum += signature[i];
		}

		if(flash_checksum != pOtaTgtHdr->FileImgHdr[index].Checksum) {
			printf("\n\rOTA image(%08x) checksum error!!!\nCalculated checksum 0x%8x, host checksum 0x%8x\n", addr, \
			flash_checksum, pOtaTgtHdr->FileImgHdr[index].Checksum);
			res = _FALSE;

			goto EXIT;
		} else {
			printf("\n\rOTA image(%08x) checksum ok!!!\n", addr);
		}
	}

EXIT:
	ota_update_free(pTempbuf);
	
	return res;
}

/**
  * @brief	  update signature.
  * @param  addr: new image address
  * @retval 0: change signature fail, 1: change signature ok
  */ 
u32 change_ota_signature(update_ota_target_hdr * pOtaTgtHdr, u32 ota_target_index)
{
	u32 addr;
	u8 * signature;
	u8 index;
	u8 ota_old_index = ota_target_index ^ 1;
	u8 empty_sig = 0x0;

	device_mutex_lock(RT_DEV_LOCK_FLASH);

	for(index = 0; index < pOtaTgtHdr->ValidImgCnt; index++) {
		addr = pOtaTgtHdr->FileImgHdr[index].FlashAddr;
		signature = &pOtaTgtHdr->Sign[index][0];
	
		/*write the signature finally*/
		if(FLASH_WriteStream(addr - SPI_FLASH_BASE, 8, signature) < 0){
			printf("\n\r[%s] Write sector failed", __FUNCTION__);
			device_mutex_unlock(RT_DEV_LOCK_FLASH);
			goto error;
		}
	}
	
	for(index = 0; index < pOtaTgtHdr->ValidImgCnt; index++) {
		if(strncmp("OTA", (const char *)pOtaTgtHdr->FileImgHdr[index].ImgId, 3) == 0)
			addr = IMG_ADDR[0][ota_old_index];
	
		/*clear the old FW signature finally*/
		if(FLASH_WriteStream(addr - SPI_FLASH_BASE, 4, &empty_sig) < 0){
			printf("\n\r[%s] Write sector failed", __FUNCTION__);
			device_mutex_unlock(RT_DEV_LOCK_FLASH);
			goto error;
		}
	}
	
	device_mutex_unlock(RT_DEV_LOCK_FLASH);
	printf("\n\r[%s] Update OTA success!", __FUNCTION__);

	return 1;
error:
	return 0;
}

#if 0
/**
  * @brief	  OTA upgrade task for single image method.
  * @param  param: pointer to configuration of server
  * @retval none
  */ 
static void ota_update_local_task(void *param)
{
	int server_socket;
	struct sockaddr_in server_addr;
	unsigned char *alloc;
	update_cfg_local_t *cfg = (update_cfg_local_t *)param;
	uint32_t file_info[3];
	int ret = -1 ;
	uint32_t ota_target_index = OTA_INDEX_2;
	update_ota_target_hdr OtaTargetHdr;
	u32 RevHdrLen;
	u8 i = 0;

	memset((u8 *)&OtaTargetHdr, 0, sizeof(update_ota_target_hdr));
	printf("\n\r[%s] Update task start\n", __FUNCTION__);

	alloc = ota_update_malloc(BUF_SIZE);
	if(!alloc){
		printf("\n\r[%s] Alloc buffer failed", __FUNCTION__);
		goto update_ota_exit;
	}

	/*-------------------step1: connect to server-------------------*/
	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if(server_socket < 0){
		printf("\n\r[%s] Create socket failed", __FUNCTION__);
		goto update_ota_exit;
	}
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = cfg->ip_addr;
	server_addr.sin_port = cfg->port;

	if(connect(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1){
		printf("\n\r[%s] socket connect failed", __FUNCTION__);
		goto update_ota_exit;
	}
	
	DBG_INFO_MSG_OFF(MODULE_FLASH);

	/* check OTA index we should update */
	if (ota_get_cur_index() == OTA_INDEX_1) {
		ota_target_index = OTA_INDEX_2;
		printf("\n\r[%s] OTA2 address space will be upgraded", __FUNCTION__);
		
	} else {
		ota_target_index = OTA_INDEX_1;
		printf("\n\r[%s] OTA1 address space will be upgraded", __FUNCTION__);
	}

	/* Receive file_info[] from server. Add this for compatibility. This file_info includes the 
	file_size and checksum information of the total firmware file.	Even though the file_info 
	is received from server , it won't be used.*/
	memset(file_info, 0, sizeof(file_info));
	if(!recv_file_info_from_server((u8 *)file_info, sizeof(file_info), server_socket)) {
		printf("\n\r[%s] receive file_info failed", __FUNCTION__);
		goto update_ota_exit;
	}

	/*----------------step2: receive firmware file header---------------------*/
	if(!recv_ota_file_hdr(alloc, &RevHdrLen, &OtaTargetHdr, server_socket)) {
		printf("\n\r[%s] rev firmware header failed", __FUNCTION__);
		goto update_ota_exit;
	}

	/* -----step3: parse firmware file header and get the target OTA image header-----*/
	if(!get_ota_tartget_header(alloc, RevHdrLen, &OtaTargetHdr, ota_target_index)) {
		printf("\n\rget OTA header failed\n");
		goto update_ota_exit;
	}

	/* the upgrade space should be masked */
	//ota_rsip_mask(NewImg2Addr, OtaTargetHdr.FileImgHdr.ImgLen, ENABLE);

	/*-------------------step4: erase flash space for new firmware--------------*/
	/*erase flash space new OTA image */
	printf("\n\rErase is ongoing...");

	for(i = 0; i < OtaTargetHdr.ValidImgCnt; i++) {
		erase_ota_target_flash(OtaTargetHdr.FileImgHdr[i].FlashAddr, OtaTargetHdr.FileImgHdr[i].ImgLen);
	}

	/*---------step5: download new firmware from server and write it to flash--------*/
	//size = download_new_fw_from_server(NewImg2Addr, server_socket, &OtaTargetHdr, signature);
	if(download_new_fw_from_server(server_socket, &OtaTargetHdr, ota_target_index) == _FALSE){
		goto update_ota_exit;
	}

	 /*-------------step6: verify checksum and update signature-----------------*/
	if(verify_ota_checksum(&OtaTargetHdr)){
		if(!change_ota_signature(&OtaTargetHdr, ota_target_index)) {
			printf("\n\rChange signature failed\n");
			goto update_ota_exit;
		}
		ret = 0;
	} 
	
	/* unmask the upgrade space */
	//ota_rsip_mask(NewImg2Addr, OtaTargetHdr.FileImgHdr.ImgLen, DISABLE);

update_ota_exit:
	if(alloc)
		ota_update_free(alloc);
	if(server_socket >= 0)
		close(server_socket);
	if(param)
		ota_update_free(param);
	
	TaskOTA = NULL;	
	printf("\n\r[%s] Update task exit", __FUNCTION__);
	
	/*-------------step7: OTA upgrade successfully, restart device------------*/
	if(!ret){
		//printf("\n\r[%s] Ready to reboot", __FUNCTION__);	
		//ota_platform_reset();
		printf("\n\rOTA is finished. Please reset device.", __FUNCTION__);	
	}
	vTaskDelete(NULL);	
	return;

}

int update_ota_local(char *ip, int port)
{
	update_cfg_local_t *pUpdateCfg;
	
	if(TaskOTA){
		printf("\n\r[%s] Update task has created.", __FUNCTION__);
		return 0;
	}
	pUpdateCfg = ota_update_malloc(sizeof(update_cfg_local_t));
	if(pUpdateCfg == NULL){
		printf("\n\r[%s] Alloc update cfg failed", __FUNCTION__);
		return -1;
	}
	pUpdateCfg->ip_addr = inet_addr(ip);
	pUpdateCfg->port = ntohs(port);

	if(xTaskCreate(ota_update_local_task, "OTA_server", 1024, pUpdateCfg, tskIDLE_PRIORITY + 1, &TaskOTA) != pdPASS){
	  	ota_update_free(pUpdateCfg);
		printf("\n\r[%s] Create update task failed", __FUNCTION__);
	}
	
	return 0;
}

#endif // #if (SERVER_TYPE == SERVER_LOCAL)
#endif
#if 0
/*  choose to boot from ota2 image or not */
void cmd_ota_image(bool cmd)
{
	/* To avoid gcc warnings */
	( void ) cmd;
#if 0
	if (cmd == 1)
		OTA_Change(OTA_INDEX_2);
	else
		OTA_Change(OTA_INDEX_1);
#endif
}

void cmd_update(int argc, char **argv)
{
#if (SERVER_TYPE == SERVER_LOCAL)

	int port;
	if(argc != 3){
		printf("\n\r[%s] Usage: update IP PORT", __FUNCTION__);
		return;
	}
	port = atoi(argv[2]);
	update_ota_local(argv[1], port);

#elif (SERVER_TYPE == SERVER_CLOUD)

	printf("OTA demo doesn't support update from cloud server. "
	"Please customized OTA process according to the standard of different cloud providers\n");

#endif
}
#endif

#if 0
#if (defined HTTP_OTA_UPDATE) || (defined HTTPS_OTA_UPDATE)
static char *redirect = NULL;
static int redirect_len;
static u16 redirect_server_port;
static char *redirect_server_host = NULL;
static char *redirect_resource = NULL;

int  parser_url( char *url, char *host, u16 *port, char *resource)
{

	if(url){
		char *http = NULL, *pos = NULL;

		http = strstr(url, "http://");
		if(http) // remove http
			url += strlen("http://");
		memset(host, 0, redirect_len);

		pos = strstr(url, ":");	// get port
		if(pos){
			memcpy(host, url, (pos-url));
			pos += 1;
			*port = atoi(pos);
		}else{
			pos = strstr(url, "/");
			if(pos){
				memcpy(host, url, (pos-url));
				url = pos;
			}
			*port = 80;
		}
		printf("server: %s\n\r", host);
		printf("port: %d\n\r", *port);
		
		memset(resource, 0, redirect_len);
		pos = strstr(url, "/");
		if(pos){
			memcpy(resource, pos + 1, strlen(pos + 1));
		}
		printf("resource: %s\n\r", resource);
		
		return 0;
	}
	return -1;
}


/**
  * @brief  parse http response.
  * @param  response: the http response got from server
  * @param  response_len: The length of http response
  * @param  result: The struct that store the useful info from the http response
  * @retval  1:only got status code;3:got status code and content length,but not get the full header;4: got all info;-1:failed
  */  
int parse_http_response(unsigned char *response, unsigned int response_len, http_response_result_t *result) {
	uint32_t i, p, q, m;
	uint32_t header_end = 0;

	//Get status code
	if(0 == result->parse_status){//didn't get the http response
		uint8_t status[4] = {0};
		i = p = q = m = 0;
		for (; i < response_len; ++i) {
			if (' ' == response[i]) {
				++m;
				if (1 == m) {//after HTTP/1.1
					p = i;
				} 
				else if (2 == m) {//after status code
					q = i;
					break;
				}
			}
		}
		if (!p || !q || q-p != 4) {//Didn't get the status code
			return -1;
		}
		memcpy(status, response+p+1, 3);//get the status code
		result->status_code = atoi((char const *)status);
		if(result->status_code == 200)
			result->parse_status = 1;
		else if(result->status_code == 302)
		{
			char *tmp=NULL; 
			const char *location1 = "LOCATION";
			const char *location2 = "Location";
			printf("response 302:%s \r\n", response);
		
			if((tmp =strstr((char *)response, location1)) ||(tmp=strstr((char *)response, location2)))
			{
				redirect_len = strlen(tmp+10);
				printf("Location len = %d\r\n", redirect_len);
				if(redirect ==NULL)
				{
					redirect = ota_update_malloc(redirect_len);
					if(redirect == NULL)
					{
						return -1;
					}
				}
				memset(redirect, 0, redirect_len);
				memcpy(redirect, tmp+10, strlen(tmp+10));
			}

			if(redirect_server_host ==NULL)
			{
				redirect_server_host = ota_update_malloc(redirect_len);
				if(redirect_server_host == NULL)
				{
					return -1;
				}
			}

			if(redirect_resource ==NULL)
			{
				redirect_resource = ota_update_malloc(redirect_len);
				if(redirect_resource == NULL)
				{
					return -1;
				}
			}

			memset(redirect_server_host, 0, redirect_len);
			memset(redirect_resource, 0, redirect_len);
			if(parser_url(redirect, redirect_server_host, &redirect_server_port , redirect_resource)<0)
			{
				return -1;
			}
			return -1;
		}
		else{
			printf("\n\r[%s] The http response status code is %d", __FUNCTION__, result->status_code);
			return -1;
		}
	}

	//if didn't receive the full http header
	if(3 == result->parse_status){//didn't get the http response
		p = q = 0;
		for (i = 0; i < response_len; ++i) {
			if (response[i] == '\r' && response[i+1] == '\n' &&
				response[i+2] == '\r' && response[i+3] == '\n') {//the end of header
				header_end = i+4;
				result->parse_status = 4;
				result->header_len = header_end;
				result->body = response + header_end;
				break;
			}
		}
		if (3 == result->parse_status) {//Still didn't receive the full header	
			result->header_bak = ota_update_malloc(HEADER_BAK_LEN + 1);
			memset(result->header_bak, 0, strlen((const char*)result->header_bak));
			memcpy(result->header_bak, response + response_len - HEADER_BAK_LEN, HEADER_BAK_LEN);
		}
	}

	//Get Content-Length
	if(1 == result->parse_status){//didn't get the content length
		const char *content_length_buf1 = "CONTENT-LENGTH";
		const char *content_length_buf2 = "Content-Length";
		const uint32_t content_length_buf_len = strlen((const char*)content_length_buf1);
		p = q = 0;
		
		for (i = 0; i < response_len; ++i) {
			if (response[i] == '\r' && response[i+1] == '\n') {
				q = i;//the end of the line
				if (!memcmp(response+p, content_length_buf1, content_length_buf_len) ||
						!memcmp(response+p, content_length_buf2, content_length_buf_len)) {//get the content length
					unsigned int j1 = p+content_length_buf_len, j2 = q-1;
					while ( j1 < q && (*(response+j1) == ':' || *(response+j1) == ' ') ) ++j1;
					while ( j2 > j1 && *(response+j2) == ' ') --j2;
					uint8_t len_buf[12] = {0};
					memcpy(len_buf, response+j1, j2-j1+1);
					result->body_len = atoi((char const *)len_buf);
					result->parse_status = 2;
				}
				p = i+2;
			}
			if (response[i] == '\r' && response[i+1] == '\n' &&
					response[i+2] == '\r' && response[i+3] == '\n') {//Get the end of header
				header_end = i+4;//p is the start of the body
				if(result->parse_status == 2){//get the full header and the content length
					result->parse_status = 4;
					result->header_len = header_end;
					result->body = response + header_end;
				}
				else {//there are no content length in header	
					printf("\n\r[%s] No Content-Length in header", __FUNCTION__);
					return -1;
				}
				break;
			}	
		}
		
		if (1 == result->parse_status) {//didn't get the content length and the full header
			result->header_bak = ota_update_malloc(HEADER_BAK_LEN + 1);
			memset(result->header_bak, 0, strlen((char *)result->header_bak));
			memcpy(result->header_bak, response + response_len - HEADER_BAK_LEN, HEADER_BAK_LEN);
		}
		else if (2 == result->parse_status) {//didn't get the full header but get the content length
			result->parse_status = 3;
			result->header_bak = ota_update_malloc(HEADER_BAK_LEN + 1);
			memset(result->header_bak, 0, strlen((char *)result->header_bak));
			memcpy(result->header_bak, response + response_len - HEADER_BAK_LEN, HEADER_BAK_LEN);
		}
	}

	return result->parse_status;
}


#ifdef HTTP_OTA_UPDATE

/**
  * @brief  connect to the OTA http server.
  * @param  server_socket: the socket used
  * @param  host: host address of the OTA server
  * @param  port: port of the OTA server
  * @retval  -1 when connect fail, socket value when connect success
  */   
int update_ota_http_connect_server(int server_socket, char *host, int port){
	struct sockaddr_in server_addr;
	struct hostent *server;
	
	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if(server_socket < 0){
		printf("\n\r[%s] Create socket failed", __FUNCTION__);
		return -1;
	}
	printf("[%s] Create socket: %d success!\n", __FUNCTION__, server_socket);
	server = gethostbyname(host);
	if(server == NULL){ 
		printf("[ERROR] Get host ip failed\n");
		return -1;
	}

	memset(&server_addr,0,sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	memcpy((void *)&server_addr.sin_addr,(void *)server->h_addr,4);

	if (connect(server_socket,(struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
		printf("\n\r[%s] Socket connect failed", __FUNCTION__);
		return -1;
	 }

	return server_socket;
}

  /**
  * @brief  receive OTA firmware file header from server
  * @param  Recvbuf: pointer to buffer for receiving OTA header of firmware file
  * @param  writelen:the length already read from server
  * @param  len: data length to be received from server
  * @param  pOtaTgtHdr: point to target image OTA  header
  * @param  socket: socket handler 
  * @retval  0:failed;1:success
  */   
u32 recv_ota_file_hdr_http(u8 * Recvbuf, u32 writelen, u32 * len, update_ota_target_hdr * pOtaTgtHdr, int socket)
{
	int read_bytes = 0;
	u32 TempLen;
	u8 * buf;
	update_file_hdr * pOtaFileHdr;
	update_file_img_hdr * pOtaFileImgHdr;

	buf = Recvbuf + writelen;
	/*receive the remaining OTA firmware file header info if needed*/
	if (writelen < 32) {
		TempLen = 32 - writelen;
		while(TempLen) {
			read_bytes = recv(socket, buf, TempLen, 0);
			if(read_bytes < 0){
				printf("[%s] read socket failed\n", __FUNCTION__);
				goto error;
			}
			if(read_bytes == 0) {
				break;
			}
			TempLen -= read_bytes;
			buf += read_bytes;
		}
	}

	pOtaFileHdr = (update_file_hdr *)(Recvbuf);
	pOtaFileImgHdr = (update_file_img_hdr *)(Recvbuf + 8);
	
	pOtaTgtHdr->FileHdr.FwVer = pOtaFileHdr->FwVer;
	pOtaTgtHdr->FileHdr.HdrNum = pOtaFileHdr->HdrNum;

	*len = (pOtaFileHdr->HdrNum * pOtaFileImgHdr->ImgHdrLen) + 8;

	return 1;
error:
	return 0;
}

  /**
  * @brief  http read socket
  * @param  Recvbuf: pointer to buffer for receiving
  * @param  socket: socket handler 
  * @param  buf_len: read data length
  * @retval  >0:success;<0:error
  */   
int http_read_socket( int socket, u8 *recevie_buf, int buf_len )
{
	int bytes_rcvd = -1; 
	if( socket < 0 ) {
		printf("[%s], socket is invalid\n", __FUNCTION__);
		return bytes_rcvd;
	}
	memset(recevie_buf, 0, buf_len);  
	//DBG_8195A("R\n");
	bytes_rcvd = recv(socket, recevie_buf, buf_len, 0 );

	if(bytes_rcvd <= 0) {
		printf("[%s], Close HTTP Socket[%d].\n", socket, __FUNCTION__);
		return -2;
	}
	return bytes_rcvd;
}

/**
  * @brief	  download new firmware from http server and write it to flash.
  * @param     first_buf: point data already from http server
  * @param     firstbuf_len: the length of already read data
  * @param     socket: socket handle
  * @param     pOtaTgtHdr: point to target image OTA  header
  * @param     targetIdx: target OTA index
  * @retval   	download size of OTA image
  */ 
u32 download_new_fw_from_server_http(u8* first_buf, unsigned int firstbuf_len, int socket, update_ota_target_hdr * pOtaTgtHdr, u8 targetIdx)
{
	/* To avoid gcc warnings */
	( void ) targetIdx;

	u8 * alloc;
	u8 * buf;
	s32 size = 0;
	int read_bytes;
	int read_bytes_buf;
	u32 TempLen;
	u32 ImageCnt;
	update_dw_info DownloadInfo[MAX_IMG_NUM];
	/*initialize the variables used in downloading procedure*/
	u32 OtaFg = 0;
	u32 IncFg = 0;
	u32 firstbufFg = 0;
	s32 RemainBytes;
	u32 SigCnt = 0;
	u32 TempCnt = 0;	
	u32 i;
	u8 res = _TRUE;
	u8 * signature;
	u32 write_sector = 0;
	u32 next_erase_sector = 0;

	/*acllocate buffer for downloading image from server*/
	alloc = ota_update_malloc(BUF_SIZE);

	buf = alloc;

	/*init download information buffer*/
	memset((u8 *)DownloadInfo, 0, MAX_IMG_NUM*sizeof(update_dw_info));

	ImageCnt = pOtaTgtHdr->ValidImgCnt;
	for(i = 0; i < ImageCnt; i++) {
		/* get OTA image and Write New Image to flash, skip the signature, 
			not write signature first for power down protection*/
		DownloadInfo[i].ImgId = OTA_IMAG;
		DownloadInfo[i].FlashAddr = pOtaTgtHdr->FileImgHdr[i].FlashAddr - SPI_FLASH_BASE + 8;
		DownloadInfo[i].ImageLen = pOtaTgtHdr->FileImgHdr[i].ImgLen - 8; /*skip the signature*/
		DownloadInfo[i].ImgOffset = pOtaTgtHdr->FileImgHdr[i].Offset;
	}

	/*initialize the reveiving counter*/
	TempLen = (pOtaTgtHdr->FileHdr.HdrNum * pOtaTgtHdr->FileImgHdr[0].ImgHdrLen) + sizeof(update_file_hdr);

	/*downloading parse the OTA and RDP image from the data stream sent by server*/
	for(i = 0; i < ImageCnt; i++) {

		/*the next image length*/
		RemainBytes = DownloadInfo[i].ImageLen;
		signature = &pOtaTgtHdr->Sign[i][0];

		if (i == 0) {
			if (firstbuf_len > DownloadInfo[i].ImgOffset) {
				firstbufFg = 1;
				TempLen += firstbuf_len -DownloadInfo[i].ImgOffset;
			}
		}

		/*download the new firmware from server*/
		while(RemainBytes > 0){
			buf = alloc;
			if(IncFg == 1) {
				IncFg = 0;
				read_bytes = read_bytes_buf;
			} else if(firstbufFg != 1) {
				read_bytes = http_read_socket(socket, buf, BUF_SIZE);
				if(read_bytes == 0){
					break; // Read end			
				}
				if(read_bytes < 0){
					//OtaImgSize = -1;
					printf("\n\r[%s] Read socket failed", __FUNCTION__);
					res = _FALSE;
					goto exit;
				}
				read_bytes_buf = read_bytes;
				TempLen += read_bytes;
			}
			
			if(TempLen > DownloadInfo[i].ImgOffset) {
				if(!OtaFg) {
					/*reach the desired image, the first packet process*/
					OtaFg = 1;
					TempCnt = TempLen -DownloadInfo[i].ImgOffset;
					if(TempCnt < 8) {
						SigCnt = TempCnt;
					} else {
						SigCnt = 8;
					}

					if (firstbufFg == 1)
						_memcpy(signature, first_buf + DownloadInfo[i].ImgOffset, SigCnt);
					else
						_memcpy(signature, buf + read_bytes -TempCnt, SigCnt);
						
					if((SigCnt < 8) || (TempCnt -8 == 0)) {
						if (firstbufFg == 1) {							
							firstbufFg = 0;
						}	
						continue;
					}
					if (firstbufFg == 1) {
						buf = first_buf + DownloadInfo[i].ImgOffset + 8;
						firstbufFg = 0;
					} else
						buf = buf + (read_bytes -TempCnt + 8);
					read_bytes = TempCnt -8;
				} else {
					/*normal packet process*/
					if(SigCnt < 8) {
						if(read_bytes < (int)(8 -SigCnt)) {
							_memcpy(signature + SigCnt, buf, read_bytes);
							SigCnt += read_bytes;
							continue;
						} else {
							_memcpy(signature + SigCnt, buf, (8 -SigCnt));
							buf = buf + (8 - SigCnt);
							read_bytes -= (8 - SigCnt) ;
							SigCnt = 8;
							if(!read_bytes) {
								continue;
							}
						}
					}
				}
				
				RemainBytes -= read_bytes;
				if(RemainBytes < 0) {
					read_bytes = read_bytes -(-RemainBytes);
				}
#if 1
				/* erase flash */
				write_sector = (DownloadInfo[i].ImageLen - RemainBytes-1+8)/4096;
				if (write_sector >= next_erase_sector){
					//DBG_8195A("E1\n");
					device_mutex_lock(RT_DEV_LOCK_FLASH);
					//DelayMs(30);
					FLASH_EraseXIP(EraseSector, pOtaTgtHdr->FileImgHdr[i].FlashAddr -SPI_FLASH_BASE + write_sector * 4096);
					device_mutex_unlock(RT_DEV_LOCK_FLASH);
					next_erase_sector++;
					//DBG_8195A("E2\n");
				}
				//DBG_8195A("E3\n");

				device_mutex_lock(RT_DEV_LOCK_FLASH);
				if(ota_writestream_user(DownloadInfo[i].FlashAddr + size, read_bytes, buf) < 0){
					printf("\n\r[%s] Write sector failed", __FUNCTION__);
					device_mutex_unlock(RT_DEV_LOCK_FLASH);
					res = _FALSE;
					goto exit;
				}
				//DelayMs(2);
				device_mutex_unlock(RT_DEV_LOCK_FLASH);

#endif						
				//DBG_8195A("E4\n");
				size += read_bytes;
			}
		}

		printf("\n\rUpdate file size: %d bytes, start addr:%08x", size + 8, pOtaTgtHdr->FileImgHdr[i].FlashAddr);
		if((u32)size != (pOtaTgtHdr->FileImgHdr[i].ImgLen - 8)) {
			printf("\n\rdownload new firmware failed\n");
			goto exit;
		}
		
		/*update flag status*/
		size = 0;
		OtaFg = 0;
		IncFg = 1;
		next_erase_sector = 0;
	}

exit:
	ota_update_free(alloc);

	return res;
}

/**
  * @brief	 OTA update through http
  * @param     host: host addr of http server
  * @param     port: http server port
  * @param     resource: resource path
  * @retval      -1:fail;0:success
  */ 

int http_update_ota(char *host, int port, char *resource)
{
	int server_socket = -1;
	unsigned char *alloc=NULL, *request=NULL;
	int alloc_buf_size = BUF_SIZE;
	int read_bytes = 0;
	int ret = -1;
	int writelen = 0;
	u32 RevHdrLen = 0;
	http_response_result_t rsp_result = {0};
	uint32_t ota_target_index = OTA_INDEX_2;
	update_ota_target_hdr OtaTargetHdr;
	
restart_http_ota:
	redirect_server_port = 0;
	
	alloc = (unsigned char *)ota_update_malloc(alloc_buf_size);
	if(!alloc){
		printf("[%s] Alloc buffer failed\n", __FUNCTION__);
		goto update_ota_exit;
	}

	/*Connect server */
	server_socket = update_ota_http_connect_server(server_socket, host, port);
	if(server_socket == -1){
		goto update_ota_exit;
	}
		uint32_t idx = 0;
		printf("\n\r");
		
		/*send http request*/
		request = (unsigned char *) ota_update_malloc(strlen("GET /") + strlen(resource) + strlen(" HTTP/1.1\r\nHost: ") 
			+ strlen(host) + strlen("\r\n\r\n") + 1);
		sprintf((char*)request, "GET /%s HTTP/1.1\r\nHost: %s\r\n\r\n", resource, host);

		ret = write(server_socket, request, strlen((char*)request));
		if(ret < 0){
			printf("[%s] Send HTTP request failed\n", __FUNCTION__);
			goto update_ota_exit;
		}
		
		/* parse http response*/
		while (3 >= rsp_result.parse_status){//still read header
			if(0 == rsp_result.parse_status){//didn't get the http response
				memset(alloc, 0, alloc_buf_size);
				read_bytes = read(server_socket, alloc, alloc_buf_size);
				if(read_bytes <= 0){
					printf("[%s] Read socket failed\n", __FUNCTION__);
					goto update_ota_exit;
				}
				idx = read_bytes;
				memset(&rsp_result, 0, sizeof(rsp_result));
				if(parse_http_response(alloc, idx, &rsp_result) == -1){
					goto update_ota_exit;
				}
			} else if ((1 == rsp_result.parse_status) || (3 == rsp_result.parse_status)){//just get the status code
				memset(alloc, 0, alloc_buf_size);
				memcpy(alloc, rsp_result.header_bak, HEADER_BAK_LEN);
				ota_update_free(rsp_result.header_bak);
				rsp_result.header_bak = NULL;
				read_bytes = read(server_socket, alloc + HEADER_BAK_LEN, (alloc_buf_size - HEADER_BAK_LEN));
				if(read_bytes <= 0){
					printf("[%s] Read socket failed\n", __FUNCTION__);
					goto update_ota_exit;
				}
				idx = read_bytes + HEADER_BAK_LEN;
				if (parse_http_response(alloc, read_bytes + HEADER_BAK_LEN, &rsp_result) == -1){
					goto update_ota_exit;
				}
			}
		}
		
	if (0 == rsp_result.body_len) {
		printf("[%s] New firmware size = 0 !\n", __FUNCTION__);
		goto update_ota_exit;
	} else {
		printf("[%s] Download new firmware begin, total size : %d\n", __FUNCTION__, rsp_result.body_len);
	}	

	writelen = idx - rsp_result.header_len;
	/* remove http header_len from alloc*/
	memset(alloc, 0, rsp_result.header_len);
	_memcpy(alloc, alloc+rsp_result.header_len, writelen);
	memset(alloc+writelen, 0, rsp_result.header_len);

	/* check OTA index we should update */
	if (ota_get_cur_index() == OTA_INDEX_1) {
		ota_target_index = OTA_INDEX_2;
		printf("\n\r[%s] OTA2 address space will be upgraded\n", __FUNCTION__);
		
	} else {
		ota_target_index = OTA_INDEX_1;
		printf("\n\r[%s] OTA1 address space will be upgraded\n", __FUNCTION__);
	}
	
	/*----------------step2: receive firmware file header---------------------*/
	if(!recv_ota_file_hdr_http(alloc, writelen, &RevHdrLen, &OtaTargetHdr, server_socket)) {
		printf("\n\r[%s] rev firmware header failed", __FUNCTION__);
		goto update_ota_exit;
	}	
	
	/* -----step3: parse firmware file header and get the target OTA image header-----*/
	if(!get_ota_tartget_header(alloc, RevHdrLen, &OtaTargetHdr, ota_target_index)) {
		printf("\n\rget OTA header failed\n");
		goto update_ota_exit;
	}

	/* the upgrade space should be masked */
	//ota_rsip_mask(NewImg2Addr, OtaTargetHdr.FileImgHdr.ImgLen, ENABLE);

	/*-------------------step4: erase flash space for new firmware--------------*/
	/*erase flash space new OTA image */
	//printf("\n\rErase is ongoing...");
	//for(i = 0; i < OtaTargetHdr.ValidImgCnt; i++) {
	//	erase_ota_target_flash(OtaTargetHdr.FileImgHdr[i].FlashAddr, OtaTargetHdr.FileImgHdr[i].ImgLen);
	//}

	/*---------step5: download new firmware from server and write it to flash--------*/
	if(download_new_fw_from_server_http(alloc, writelen, server_socket, &OtaTargetHdr, ota_target_index) == _FALSE){
		goto update_ota_exit;
	}

	 /*-------------step6: verify checksum and update signature-----------------*/
	if(verify_ota_checksum(&OtaTargetHdr)){
		//if(!change_ota_signature(&OtaTargetHdr, ota_target_index)) {
			//printf("\n\rChange signature failed\n");
			//goto update_ota_exit;
		//}
		//ret = 0;
	} 

update_ota_exit:
	if(alloc)
		ota_update_free(alloc);
	if(request)
		ota_update_free(request);
	if(server_socket >= 0)
		close(server_socket);
	
	/* redirect_server_port != 0 means there is redirect URL can be downloaded*/
	if(redirect_server_port != 0){
		host = redirect_server_host;
		resource = redirect_resource;
		port = redirect_server_port;
		printf("OTA redirect host: %s, port: %s, resource: %s\n\r", host, port, resource);
		goto restart_http_ota;
	}
	
	ota_update_free(redirect);
	ota_update_free(redirect_server_host);
	ota_update_free(redirect_resource);

	return ret;
}
#endif

#ifdef HTTPS_OTA_UPDATE

static int my_random(void *p_rng, unsigned char *output, size_t output_len)
{
	( void ) p_rng;
	rtw_get_random_bytes(output, output_len);
	return 0;
}

static void* my_calloc(size_t nelements, size_t elementSize)
{
	size_t size;
	void *ptr = NULL;

	size = nelements * elementSize;
	ptr = pvPortMalloc(size);

	if(ptr)
		memset(ptr, 0, size);

	return ptr;
}


static char *https_itoa(int value){
	char *val_str;
	int tmp = value, len = 1;

	while((tmp /= 10) > 0)
		len ++;

	val_str = (char *) pvPortMalloc(len + 1);
	sprintf(val_str, "%d", value);

	return val_str;
}


  /**
  * @brief  receive OTA firmware file header from server
  * @param  Recvbuf: pointer to buffer for receiving OTA header of firmware file
  * @param  writelen:the length already read from server
  * @param  len: data length to be received from server
  * @param  pOtaTgtHdr: point to target image OTA  header
  * @param  ssl: context for mbedtls 
  * @retval  0:failed;1:success
  */   
u32 recv_ota_file_hdr_https(u8 * Recvbuf, u32 writelen, u32 * len, update_ota_target_hdr * pOtaTgtHdr, mbedtls_ssl_context *ssl)
{
	int read_bytes = 0;
	u32 TempLen;
	u8 * buf;
	update_file_hdr * pOtaFileHdr;
	update_file_img_hdr * pOtaFileImgHdr;

	buf = Recvbuf + writelen;
	/*receive the remaining OTA firmware file header info if needed*/
	if (writelen < 32) {
		TempLen = 32 - writelen;
		while(TempLen) {
			read_bytes = mbedtls_ssl_read(ssl, buf, TempLen);
			if(read_bytes < 0){
				printf("[%s] read socket failed [%d]\n", __FUNCTION__, read_bytes);
				goto error;
			}
			if(read_bytes == 0) {
				break;
			}
			TempLen -= read_bytes;
			buf += read_bytes;
		}
	}

	pOtaFileHdr = (update_file_hdr *)(Recvbuf);
	pOtaFileImgHdr = (update_file_img_hdr *)(Recvbuf + 8);
	
	pOtaTgtHdr->FileHdr.FwVer = pOtaFileHdr->FwVer;
	pOtaTgtHdr->FileHdr.HdrNum = pOtaFileHdr->HdrNum;

	*len = (pOtaFileHdr->HdrNum * pOtaFileImgHdr->ImgHdrLen) + 8;

	return 1;
error:
	return 0;
}


  /**
  * @brief  https read socket
  * @param  Recvbuf: pointer to buffer for receiving
  * @param  ssl: context for mbedtls 
  * @param  buf_len: read data length
  * @retval  >0:success;<0:error
  */   
int https_read_socket( mbedtls_ssl_context *ssl, u8 *recevie_buf, int buf_len )
{
	int bytes_rcvd = -1; 

	memset(recevie_buf, 0, buf_len);  

	bytes_rcvd = mbedtls_ssl_read(ssl, recevie_buf, buf_len );

	if(bytes_rcvd <= 0) {
		printf("[%s], ssl read failed [%d]\n",  __FUNCTION__, bytes_rcvd);
		return -2;
	}
	return bytes_rcvd;
}



/**
  * @brief	  download new firmware from https server and write it to flash.
  * @param     first_buf: point data already from https server
  * @param     firstbuf_len: the length of already read data
  * @param     ssl: context for mbedtls 
  * @param     pOtaTgtHdr: point to target image OTA  header
  * @param     targetIdx: target OTA index
  * @retval   	download size of OTA image
  */ 
u32 download_new_fw_from_server_https(u8* first_buf, unsigned int firstbuf_len, mbedtls_ssl_context *ssl, update_ota_target_hdr * pOtaTgtHdr, u8 targetIdx)
{
	/* To avoid gcc warnings */
	( void ) targetIdx;

	u8 * alloc;
	u8 * buf;
	s32 size = 0;
	int read_bytes;
	int read_bytes_buf;
	u32 TempLen;
	u32 ImageCnt;
	update_dw_info DownloadInfo[MAX_IMG_NUM];
	/*initialize the variables used in downloading procedure*/
	u32 OtaFg = 0;
	u32 IncFg = 0;
	u32 firstbufFg = 0;
	s32 RemainBytes;
	u32 SigCnt = 0;
	u32 TempCnt = 0;	
	u32 i;
	u8 res = _TRUE;
	u8 * signature;
	u32 write_sector = 0;
	u32 next_erase_sector = 0;

	/*acllocate buffer for downloading image from server*/
	alloc = ota_update_malloc(BUF_SIZE);

	buf = alloc;

	/*init download information buffer*/
	memset((u8 *)DownloadInfo, 0, MAX_IMG_NUM*sizeof(update_dw_info));

	ImageCnt = pOtaTgtHdr->ValidImgCnt;
	for(i = 0; i < ImageCnt; i++) {
		/* get OTA image and Write New Image to flash, skip the signature, 
			not write signature first for power down protection*/
		DownloadInfo[i].ImgId = OTA_IMAG;
		DownloadInfo[i].FlashAddr = pOtaTgtHdr->FileImgHdr[i].FlashAddr - SPI_FLASH_BASE + 8;
		DownloadInfo[i].ImageLen = pOtaTgtHdr->FileImgHdr[i].ImgLen - 8; /*skip the signature*/
		DownloadInfo[i].ImgOffset = pOtaTgtHdr->FileImgHdr[i].Offset;
	}

	/*initialize the reveiving counter*/
	TempLen = (pOtaTgtHdr->FileHdr.HdrNum * pOtaTgtHdr->FileImgHdr[0].ImgHdrLen) + sizeof(update_file_hdr);

	/*downloading parse the OTA and RDP image from the data stream sent by server*/
	for(i = 0; i < ImageCnt; i++) {

		/*the next image length*/
		RemainBytes = DownloadInfo[i].ImageLen;
		signature = &pOtaTgtHdr->Sign[i][0];

		if (i == 0) {
			if (firstbuf_len > DownloadInfo[i].ImgOffset) {
				firstbufFg = 1;
				TempLen += firstbuf_len -DownloadInfo[i].ImgOffset;
			}
		}

		/*download the new firmware from server*/
		while(RemainBytes > 0){
			buf = alloc;
			if(IncFg == 1) {
				IncFg = 0;
				read_bytes = read_bytes_buf;
			} else if(firstbufFg != 1) {
				read_bytes = https_read_socket(ssl, buf, BUF_SIZE);
				if(read_bytes == 0){
					break; // Read end			
				}
				if(read_bytes < 0){
					//OtaImgSize = -1;
					printf("\n\r[%s] Read socket failed", __FUNCTION__);
					res = _FALSE;
					goto exit;
				}
				read_bytes_buf = read_bytes;
				TempLen += read_bytes;
			}
			
			if(TempLen > DownloadInfo[i].ImgOffset) {
				if(!OtaFg) {
					/*reach the desired image, the first packet process*/
					OtaFg = 1;
					TempCnt = TempLen -DownloadInfo[i].ImgOffset;
					if(TempCnt < 8) {
						SigCnt = TempCnt;
					} else {
						SigCnt = 8;
					}

					if (firstbufFg == 1)
						_memcpy(signature, first_buf + DownloadInfo[i].ImgOffset, SigCnt);
					else
						_memcpy(signature, buf + read_bytes -TempCnt, SigCnt);
						
					if((SigCnt < 8) || (TempCnt -8 == 0)) {
						if (firstbufFg == 1) {							
							firstbufFg = 0;
						}	
						continue;
					}
					if (firstbufFg == 1) {
						buf = first_buf + DownloadInfo[i].ImgOffset + 8;
						firstbufFg = 0;
					} else
						buf = buf + (read_bytes -TempCnt + 8);
					read_bytes = TempCnt -8;
				} else {
					/*normal packet process*/
					if(SigCnt < 8) {
						if(read_bytes < (int)(8 -SigCnt)) {
							_memcpy(signature + SigCnt, buf, read_bytes);
							SigCnt += read_bytes;
							continue;
						} else {
							_memcpy(signature + SigCnt, buf, (8 -SigCnt));
							buf = buf + (8 - SigCnt);
							read_bytes -= (8 - SigCnt) ;
							SigCnt = 8;
							if(!read_bytes) {
								continue;
							}
						}
					}
				}
				
				RemainBytes -= read_bytes;
				if(RemainBytes < 0) {
					read_bytes = read_bytes -(-RemainBytes);
				}

				/* erase flash */
				write_sector = (DownloadInfo[i].ImageLen - RemainBytes-1+8)/4096;
				if (write_sector >= next_erase_sector){
					device_mutex_lock(RT_DEV_LOCK_FLASH);
					FLASH_EraseXIP(EraseSector, pOtaTgtHdr->FileImgHdr[i].FlashAddr -SPI_FLASH_BASE + write_sector * 4096);
					device_mutex_unlock(RT_DEV_LOCK_FLASH);
					next_erase_sector++;
				}
				
				device_mutex_lock(RT_DEV_LOCK_FLASH);
				if(ota_writestream_user(DownloadInfo[i].FlashAddr + size, read_bytes, buf) < 0){
					printf("\n\r[%s] Write sector failed", __FUNCTION__);
					device_mutex_unlock(RT_DEV_LOCK_FLASH);
					res = _FALSE;
					goto exit;
				}
				device_mutex_unlock(RT_DEV_LOCK_FLASH);
				size += read_bytes;
			}
		}

		printf("\n\rUpdate file size: %d bytes, start addr:%08x", size + 8, pOtaTgtHdr->FileImgHdr[i].FlashAddr);
		if((u32)size != (pOtaTgtHdr->FileImgHdr[i].ImgLen - 8)) {
			printf("\n\rdownload new firmware failed\n");
			goto exit;
		}
		
		/*update flag status*/
		size = 0;
		OtaFg = 0;
		IncFg = 1;
		next_erase_sector = 0;
	}

exit:
	ota_update_free(alloc);

	return res;
}



/**
  * @brief	 OTA update through https
  * @param     host: host addr of https server
  * @param     port: https server port
  * @param     resource: resource path
  * @retval      -1:fail;0:success
  */ 
int https_update_ota(char *host, int port, char *resource)
{
	unsigned char *alloc=NULL, *request=NULL;
	int alloc_buf_size = BUF_SIZE;
	int read_bytes = 0;
	int ret = -1;
	int writelen = 0;
	u32 RevHdrLen = 0;
	http_response_result_t rsp_result = {0};
	uint32_t ota_target_index = OTA_INDEX_2;
	update_ota_target_hdr OtaTargetHdr;

	mbedtls_net_context server_fd;
	mbedtls_ssl_context ssl;
	mbedtls_ssl_config conf;
	
restart_https_ota:
	redirect_server_port = 0;
	
	alloc = (unsigned char *)ota_update_malloc(alloc_buf_size);
	if(!alloc){
		printf("[%s] Alloc buffer failed\n", __FUNCTION__);
		goto update_ota_exit;
	}

	/*connect https server*/
	mbedtls_platform_set_calloc_free(my_calloc, vPortFree);

	mbedtls_net_init(&server_fd);
	mbedtls_ssl_init(&ssl);
	mbedtls_ssl_config_init(&conf);

	char *port_str = https_itoa (port);
	if((ret = mbedtls_net_connect(&server_fd, host, port_str, MBEDTLS_NET_PROTO_TCP)) != 0) {
		printf("ERROR: mbedtls_net_connect ret(%d)\n", ret);
		goto update_ota_exit;
	}

	mbedtls_ssl_set_bio(&ssl, &server_fd, mbedtls_net_send, mbedtls_net_recv, NULL);
	if((ret = mbedtls_ssl_config_defaults(&conf,
			MBEDTLS_SSL_IS_CLIENT,
			MBEDTLS_SSL_TRANSPORT_STREAM,
			MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {

		printf("ERRPR: mbedtls_ssl_config_defaults ret(%d)\n", ret);
		goto update_ota_exit;
	}

	mbedtls_ssl_conf_authmode(&conf, MBEDTLS_SSL_VERIFY_NONE);
	mbedtls_ssl_conf_rng(&conf, my_random, NULL);

	if((ret = mbedtls_ssl_setup(&ssl, &conf)) != 0) {
		printf("ERRPR: mbedtls_ssl_setup ret(%d)\n", ret);
		goto update_ota_exit;
	}

	if((ret = mbedtls_ssl_handshake(&ssl)) != 0) {
		printf("ERROR: mbedtls_ssl_handshake ret(-0x%x)", -ret);
		goto update_ota_exit;
	}

	printf("SSL ciphersuite %s\n", mbedtls_ssl_get_ciphersuite(&ssl));

	/*send https request*/
	uint32_t idx = 0;
	request = (unsigned char *) ota_update_malloc(strlen("GET /") + strlen(resource) + strlen(" HTTP/1.1\r\nHost: ") 
		+ strlen(host) + strlen("\r\n\r\n") + 1);
	sprintf((char*)request, "GET /%s HTTP/1.1\r\nHost: %s\r\n\r\n", resource, host);		
	ret = mbedtls_ssl_write(&ssl, request, strlen((char*)request));
	if(ret < 0){
		printf("[%s] Send HTTPS request failed\n", __FUNCTION__);
		goto update_ota_exit;
	}
			
	/* parse https response*/
	while (3 >= rsp_result.parse_status){//still read header
		if(0 == rsp_result.parse_status){//didn't get the http response
			memset(alloc, 0, alloc_buf_size);
			read_bytes = mbedtls_ssl_read(&ssl, alloc, alloc_buf_size);
			if(read_bytes <= 0){
				printf("[%s] Read socket failed\n", __FUNCTION__);
				goto update_ota_exit;
			}
			idx = read_bytes;
			memset(&rsp_result, 0, sizeof(rsp_result));
			if(parse_http_response(alloc, idx, &rsp_result) == -1){
				goto update_ota_exit;
			}
		} else if ((1 == rsp_result.parse_status) || (3 == rsp_result.parse_status)){//just get the status code
			memset(alloc, 0, alloc_buf_size);
			memcpy(alloc, rsp_result.header_bak, HEADER_BAK_LEN);
			ota_update_free(rsp_result.header_bak);
			rsp_result.header_bak = NULL;
			read_bytes = mbedtls_ssl_read(&ssl, alloc + HEADER_BAK_LEN, (alloc_buf_size - HEADER_BAK_LEN));
			if(read_bytes <= 0){
				printf("[%s] Read socket failed\n", __FUNCTION__);
				goto update_ota_exit;
			}
			idx = read_bytes + HEADER_BAK_LEN;
			if (parse_http_response(alloc, read_bytes + HEADER_BAK_LEN, &rsp_result) == -1){
				goto update_ota_exit;
			}
		}
	}
		
	if (0 == rsp_result.body_len) {
		printf("[%s] New firmware size = 0 !\n", __FUNCTION__);
		goto update_ota_exit;
	} else {
		printf("[%s] Download new firmware begin, total size : %d\n", __FUNCTION__, rsp_result.body_len);
	}	

	writelen = idx - rsp_result.header_len;
	/* remove https header_len from alloc*/
	memset(alloc, 0, rsp_result.header_len);
	_memcpy(alloc, alloc+rsp_result.header_len, writelen);
	memset(alloc+writelen, 0, rsp_result.header_len);

	/* check OTA index we should update */
	if (ota_get_cur_index() == OTA_INDEX_1) {
		ota_target_index = OTA_INDEX_2;
		printf("\n\r[%s] OTA2 address space will be upgraded \n", __FUNCTION__);
		
	} else {
		ota_target_index = OTA_INDEX_1;
		printf("\n\r[%s] OTA1 address space will be upgraded \n", __FUNCTION__);
	}
	
	/*----------------step2: receive firmware file header---------------------*/
	if(!recv_ota_file_hdr_https(alloc, writelen, &RevHdrLen, &OtaTargetHdr, &ssl)) {
		printf("\n\r[%s] recv firmware header failed", __FUNCTION__);
		goto update_ota_exit;
	}	
	
	/* -----step3: parse firmware file header and get the target OTA image header-----*/
	if(!get_ota_tartget_header(alloc, RevHdrLen, &OtaTargetHdr, ota_target_index)) {
		printf("\n\rget OTA header failed\n");
		goto update_ota_exit;
	}

	/* the upgrade space should be masked */
	//ota_rsip_mask(NewImg2Addr, OtaTargetHdr.FileImgHdr.ImgLen, ENABLE);

	/*-------------------step4: erase flash space for new firmware--------------*/
	/*erase flash space new OTA image */
	//printf("\n\rErase is ongoing...");
	//for(i = 0; i < OtaTargetHdr.ValidImgCnt; i++) {
	//	erase_ota_target_flash(OtaTargetHdr.FileImgHdr[i].FlashAddr, OtaTargetHdr.FileImgHdr[i].ImgLen);
	//}

	/*---------step5: download new firmware from server and write it to flash--------*/
	if(download_new_fw_from_server_https(alloc, writelen, &ssl, &OtaTargetHdr, ota_target_index) == _FALSE){
		goto update_ota_exit;
	}

	 /*-------------step6: verify checksum and update signature-----------------*/
	if(verify_ota_checksum(&OtaTargetHdr)){
		if(!change_ota_signature(&OtaTargetHdr, ota_target_index)) {
			printf("\n\rChange signature failed\n");
			goto update_ota_exit;
		}
		ret = 0;
	} 

update_ota_exit:
	if(alloc)
		ota_update_free(alloc);
	if(request)
		ota_update_free(request);
	
	mbedtls_net_free(&server_fd);
	mbedtls_ssl_free(&ssl);
	mbedtls_ssl_config_free(&conf);
	
	/* redirect_server_port != 0 means there is redirect URL can be downloaded*/
	if(redirect_server_port != 0){
		host = redirect_server_host;
		resource = redirect_resource;
		port = redirect_server_port;
		printf("OTA redirect host: %s, port: %d, resource: %s\n\r", host, port, resource);
		goto restart_https_ota;
	}
	
	ota_update_free(redirect);
	ota_update_free(redirect_server_host);
	ota_update_free(redirect_resource);

	return ret;
}

#endif
#endif
#endif
