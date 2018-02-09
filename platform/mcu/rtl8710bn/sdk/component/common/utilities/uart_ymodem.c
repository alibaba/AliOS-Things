/****************************************uart _ymodem.c**************************************************/

#include "uart_ymodem.h"
#include "osdep_service.h"
#include "PinNames.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#if defined(CONFIG_PLATFORM_8711B)
extern const update_file_img_id OtaImgId[2];
u8 uart_signature[9] = {0};
update_ota_target_hdr OtaTargetHdr;
static u32 flash_write_len = 0;
static u32 flash_offset = 0x0;
static u32 file_offset = 0;
static u32 IMAGE_OFFSET = 0;
static u32 IMAGE_LEN = 0;
static int hd_flags = 0;
static int sig_flags = 0;	
static int sig_cnt = 0;
#endif
/*****************************************************************************************
*                                uart basic functions                                    *
******************************************************************************************/
 void uarty_irq(uint32_t id, SerialIrq event)
{
	uart_ymodem_t *ptr = (uart_ymodem_t *)id;
	//u8 ch = 0;
	if(event == RxIrq) {
		if(ptr->uart_recv_index == 0){
			//RtlUpSemaFromISR(&ptr->uart_rx_sema);//up uart rx semaphore
			rtw_up_sema_from_isr(&ptr->uart_rx_sema);//up uart rx semaphore
		}
		if(ptr->uart_recv_index == RCV_BUF_SIZE)
			ptr->uart_recv_index = 0;
		//ch = serial_getc(&ptr->sobj);
	//	printf("[%d] 0x%x\r\n", ptr->uart_recv_index, ch);
		ptr->uart_irq_buf[ptr->uart_recv_index++] = serial_getc(&ptr->sobj);
		ptr->tick_last_update =  xTaskGetTickCountFromISR();	// update tick everytime recved data 
	}
	
	if(event == TxIrq){
//		uart_send_string(sobj, "\r\n8195a$");
//		rcv_ch = 0;
	}
}

void uart_init(uart_ymodem_t *ptr)
{
//	serial_t sobj;

	//uart init
	serial_init(&ptr->sobj,UART_TX,UART_RX);
	serial_baud(&ptr->sobj,UART_BAUDRATE);		//set baudrate 38400
	serial_format(&ptr->sobj, 8, ParityNone, 0);

	serial_irq_handler(&ptr->sobj, uarty_irq, (int)ptr);
	serial_irq_set(&ptr->sobj, RxIrq, 1);
	serial_irq_set(&ptr->sobj, TxIrq, 1);
	
	//RtlInitSema(&ptr->uart_rx_sema, 0);	
	rtw_init_sema(&ptr->uart_rx_sema, 0);
}
void uart_sendbyte(uart_ymodem_t *ptr,u8 sendc )
{

	serial_putc(&ptr->sobj, sendc);
//	printf(" uart send 0x%x\r\n",sendc);
}

int uart_recvbytetimeout(uart_ymodem_t *uart_ymodem,u8 *ptr)
{
	int ret = 0;
//	static int uart_recv_buf_index = 0;

//	printf(" [%d] = %x\r\n",uart_ymodem->uart_recv_buf_index,uart_ymodem->uart_irq_buf[uart_ymodem->uart_recv_buf_index]);
	*ptr = uart_ymodem->uart_irq_buf[uart_ymodem->uart_recv_buf_index];
	uart_ymodem->uart_recv_buf_index++;
	if(uart_ymodem->uart_recv_buf_index == RCV_BUF_SIZE)
		uart_ymodem->uart_recv_buf_index = 0;
	return ret;
}

void uart_rxempty(uart_ymodem_t *ptr)
{
	/*clean uart recv buf*/
//	printf("Uart_RxEmpty\r\n");
	memset(ptr->uart_irq_buf, 0, RCV_BUF_SIZE);
	memset(ptr->uart_rcv_buf, 0, RCV_BUF_SIZE);
	ptr->uart_recv_buf_index = 0;
	ptr->uart_recv_index = 0;
}
/*****************************************************************************************
*                                       flash function                                                   *
******************************************************************************************/
int ymodem_flashwrite(int flashadd, u8 *pbuf, int len)
{
	
	int ret = 0;
	flash_t flash;
	
//	if(!FLASH_ADDRESS_CHECK_WRITE_ERASE(flashadd)){
//		ret = -1;
//		return ret;
//		}
	if( len == 0){
		printf("input error,data length should not be null!\r\n");
		ret = -1;
		return ret;
		}
	else	//as 8711am only canbe r/w in words.so make len is 4-bytes aligmented.
		len += 4 - ((len%4)==0 ? 4 : (len%4));
	
	while(len){
		if(flash_write_word(&flash, flashadd, *(unsigned int *)pbuf) !=1 ){
			printf("write flash error!\r\n");
			ret = -1;
			return ret;
		}
		len -= 4;
		pbuf += 4;
		flashadd += 4;
	}

	return ret;
}
/****************************uart_ymodem_init**********************************/
void uart_ymodem_init(uart_ymodem_t *uart_ymodem_ptr)
{
//	u32 ret = 0;
#if CONFIG_CALC_FILE_SIZE
	u8 filename[33] = {0}; //file name: max 32 bytes+ '\0'=33
#endif
	//init uart struct 
	uart_ymodem_ptr->cur_num = 0;
	uart_ymodem_ptr->filelen = 0 ;
#if CONFIG_CALC_FILE_SIZE
	uart_ymodem_ptr->filename = &filename[0];
#endif
	uart_ymodem_ptr->len = 0;
	uart_ymodem_ptr->nxt_num = 0;
	uart_ymodem_ptr->modemtype = 2;	 //ymodem protocol 
	uart_ymodem_ptr->rec_err = 0;
	uart_ymodem_ptr->crc_mode = 1;	//crc check
	uart_ymodem_ptr->uart_recv_buf_index = 0;
	uart_ymodem_ptr->uart_recv_index = 0;
	uart_ymodem_ptr->image_address = IMAGE_TWO;
	
#if defined(CONFIG_PLATFORM_8711B)
	rtw_memset(uart_signature, 0, sizeof(uart_signature));
	rtw_memset(&OtaTargetHdr, 0, sizeof(OtaTargetHdr));
	flash_write_len = 0;
	flash_offset = 0x0;
	file_offset = 0;
	IMAGE_OFFSET = 0;
	IMAGE_LEN = 0;
	hd_flags = 0;
	sig_flags = 0;	
	sig_cnt = 0;
#endif
//	return uart_ymodem_ptr;
}

void uart_ymodem_deinit(uart_ymodem_t *ptr)
{

	/* Free uart_rx-sema */
	//RtlFreeSema(&ptr->uart_rx_sema);	
	rtw_free_sema(&ptr->uart_rx_sema);
	
	/* Free serial */
	serial_free(&ptr->sobj);

	/* Free uart_ymodem_t */
	//RtlMfree((u8 *)ptr,sizeof(uart_ymodem_t));	
	rtw_mfree((u8 *)ptr,sizeof(uart_ymodem_t));
}

#if CONFIG_CALC_FILE_SIZE
unsigned int buf_filelen(u8 *ptr)
{
	int datatype=10, result=0;

	if (ptr[0]=='0' && (ptr[1]=='x' && ptr[1]=='X'))
	{
		datatype = 16;
		ptr += 2;
	}

	for ( ; *ptr!='\0'; ptr++)
	{
		if (*ptr>= '0' && *ptr<='9')
		{
			result =result*datatype+*ptr-'0';
		}
		else
		{
			if (datatype == 10)
			{
				return result;
			}
			else
			{
				if (*ptr>='A' && *ptr<='F')
				{
					result = result*16 + *ptr-55;             //55 = 'A'-10
				}
				else if (*ptr>='a' && *ptr<='f')
				{
					result = result*16 + *ptr-87;             //87 = 'a'-10
				}
				else
				{
					return result;
				}
			}
		}
	}
	return result;
}
#endif
void modem_cancle(uart_ymodem_t *ptr)
{
	uart_sendbyte(ptr,0x18);
	uart_sendbyte(ptr,0x18);
	uart_sendbyte(ptr,0x18);
	uart_sendbyte(ptr,0x18);
	uart_sendbyte(ptr,0x18);
}

int start_next_round(uart_ymodem_t *ptr)
{
	int ret = 0;
	
//	printf(" uart ymodedm transfer %d block\r\n",ptr->nxt_num);
	//clean recv buf
	if(!ptr->rec_err){
		uart_rxempty(ptr);
	} 
	else{
		ret = -1;
		printf("\r\n recv data error!");
	}
	
	if (ptr->nxt_num == 0)
	{
		if (ptr->crc_mode)
		{
			uart_sendbyte(ptr,MODEM_C);	//first receiver send c
		}
		else
		{
			uart_sendbyte(ptr,MODEM_NAK);
		}
	}
	else
	{
		if (ptr->rec_err)
		{
			uart_sendbyte(ptr,MODEM_NAK);
		}
		else
		{
			if (ptr->nxt_num == 1)
			{
				if (ptr->crc_mode)
				{
					uart_sendbyte(ptr,MODEM_ACK);
					uart_sendbyte(ptr,MODEM_C);
				}
				else
				{
					uart_sendbyte(ptr,MODEM_NAK);
				}
			}
			else
			{
				uart_sendbyte(ptr,MODEM_ACK);
			}
		}
	}
	return ret;
}
int block_num_check(uart_ymodem_t *ptr)
{

	u8 blk,cblk;
	int stat, ret = 0;
	/**************** check blk and blk complement *********************/
	stat = uart_recvbytetimeout(ptr,&blk);	//blk num,bytes 2
	if (stat != 0)
	{
		ret = -1;
	}
	printf(" blk num = %x\r\n", blk);

	stat = uart_recvbytetimeout(ptr,&cblk);	//block num complement,bytes 3
	if (stat != 0)
	{
		ret = -1;
	}
//	printf(" block num cmpl = %x\r\n",cblk);

	if (blk+cblk != 0xff)
	{
		ret = -1;
	}
	return ret;

}

int calc_file_name_size(uart_ymodem_t *ptr,u8* bufptr)
{
	int ret = 0;
	u8* nameptr = ptr->filename;
	
	while (*bufptr != '\0'){
		*nameptr++ = *bufptr++;
	}
	*nameptr = '\0';
	bufptr++;
	while (*bufptr == ' ')
	{
		bufptr++;
	}
	//file length
	ptr->filelen = buf_filelen(bufptr);
	
	return ret;
}

int crc_check(uart_ymodem_t *ptr)
{
	u8 crch, crcl;
	u8 *in_ptr;
	int stat,i,ret = 0;
	u32 cksum = 0;

	stat = uart_recvbytetimeout(ptr,&crch);			//CRC byte 1
	if (stat != 0){
		ret = 1;
	}
//	printf(" char recved CRC byte 1 = %x\r\n", crch);
	if (ptr->crc_mode){
		stat = uart_recvbytetimeout(ptr,&crcl); 	//CRC byte 2
		if (stat != 0){
			ret = 1;
		}
	}
//	printf(" char recved CRC byte 2 = %x\r\n", crcl);
#if CRC_CHECK
	for (i=0; i<ptr->len; i++)			//sum check for last block
	{
		cksum += ptr->uart_rcv_buf[i];
	}
	if(cksum == 0)
	{ 
	 	ret = 2;
		return ret;
	}
	
	if (ptr->crc_mode)
	{
		in_ptr = ptr->uart_rcv_buf;
		cksum = 0;
		
		for (stat=ptr->len ; stat>0; stat--)
		{
			cksum = cksum^(int)(*in_ptr++) << 8;
			for (i=8; i !=0; i--)
			{
				if (cksum & 0x8000)
					cksum = cksum << 1 ^ 0x1021;
				else
					cksum = cksum << 1;
			}

		}
		cksum &= 0xffff;
		
		if (cksum != (crch<<8 | crcl))
		{
			ptr->rec_err = 1;
			ret = 1;
		}
	}
	else
	{
		for (i=0; i<ptr->len; i++)				//sum check
		{
			cksum += ptr->uart_rcv_buf[i];
		}
		if ((cksum&0xff)!=crch)
		{
			ptr->rec_err = 1;
			ret = 1;
		}
	}
#endif
	return ret;

}
#if DUMP_DATA
void flash_dump_data(uart_ymodem_t *ptr)
{
	int i,offset = 0;
	u32 data;
	printf("flash dump data");
	for(i = 0;i< ptr->filelen;i+=4){
		flash_read_word(&ptr->flash, ptr->image_address + 0x10 + offset, &data);
		offset += 4;
		printf("%x ",data);
	}
}
#endif

#if defined(CONFIG_PLATFORM_8711B)
int data_write_to_flash(uart_ymodem_t *ptr)
{
	int ret = 0;
	u8 *pImgId = NULL;
	u32 buf_offset = 0;
	u32 write_len = 0;
	uint32_t uart_ota_target_index = OTA_INDEX_2;

	if(!hd_flags){
		/* check OTA index we should update */
		if (ota_get_cur_index() == OTA_INDEX_1) {
			uart_ota_target_index = OTA_INDEX_2;
			printf("OTA2 address space will be upgraded\n");
		} else {
			uart_ota_target_index = OTA_INDEX_1;
			printf("OTA1 address space will be upgraded\n");
		}		
		pImgId = (u8 *)&OtaImgId[uart_ota_target_index];
		
		/* -----step3: parse firmware file header and get the target OTA image header-----*/
		/* parse firmware file header and get the target OTA image header-----*/
		if(!get_ota_tartget_header(ptr->uart_rcv_buf, ptr->len, &OtaTargetHdr, pImgId)){
			printf("\n\rget OTA header failed\n");
			return 1;
		}

		/*get new image addr and check new address validity*/
		if(!get_ota_address(uart_ota_target_index, &ptr->image_address, &OtaTargetHdr)){
			printf("\n\rget OTA address failed\n");
			return 1;
		}

		/*get new image length from the firmware header*/
		IMAGE_LEN = OtaTargetHdr.FileImgHdr.ImgLen;

		/*-------------------step4: erase flash space for new firmware--------------*/
		/*erase flash space new OTA image */
		erase_ota_target_flash(ptr->image_address, IMAGE_LEN);
		/*erase flash space for new RDP image*/
		if(OtaTargetHdr.RdpStatus == ENABLE) {
			device_mutex_lock(RT_DEV_LOCK_FLASH);
			flash_erase_sector(&ptr->flash, RDP_FLASH_ADDR - SPI_FLASH_BASE);
			device_mutex_unlock(RT_DEV_LOCK_FLASH);
		}

		IMAGE_OFFSET = OtaTargetHdr.FileImgHdr.Offset;
		hd_flags = 1;
	}
	//printf("\n\r file_offset = %d ptr->len = %d", file_offset, ptr->len);

	/*---------step5: download new firmware from server and write it to flash--------*/
	if(IMAGE_OFFSET >= file_offset && IMAGE_OFFSET < file_offset + ptr->len){
		buf_offset = IMAGE_OFFSET - file_offset;
		write_len = ptr->len - buf_offset;
		file_offset += ptr->len;
		if(!sig_flags){
			if(write_len < 8){
				sig_cnt = write_len;
			}else{
				sig_cnt = 8;
				sig_flags = 1;
			}
			_memcpy(uart_signature, ptr->uart_rcv_buf + buf_offset, sig_cnt);
			buf_offset += sig_cnt;
			flash_offset += sig_cnt;
			write_len -= sig_cnt;
			if(!write_len)
				return ret;
		}
	}else if(IMAGE_OFFSET < file_offset && IMAGE_OFFSET + IMAGE_LEN >= file_offset + ptr->len){
		buf_offset = 0;
		write_len = ptr->len;
		file_offset += ptr->len;
		if(!sig_flags){
			_memcpy(uart_signature + sig_cnt, ptr->uart_rcv_buf + buf_offset, 8 - sig_cnt);
			sig_cnt = 8 - sig_cnt;
			buf_offset += sig_cnt;
			flash_offset += sig_cnt;
			write_len -= sig_cnt;
			sig_flags = 1;
		}
	}else if(IMAGE_OFFSET + IMAGE_LEN > file_offset && IMAGE_OFFSET + IMAGE_LEN < file_offset + ptr->len){
		buf_offset = 0;
		write_len = IMAGE_OFFSET + IMAGE_LEN - file_offset;
		file_offset += ptr->len;
	}else{
		file_offset += ptr->len;
		return ret;	
	}

	device_mutex_lock(RT_DEV_LOCK_FLASH);
	if(flash_stream_write(&ptr->flash, ptr->image_address + flash_offset - SPI_FLASH_BASE, write_len, ptr->uart_rcv_buf + buf_offset) < 0){
		printf("\n\r[%s] Write sector failed", __FUNCTION__);
		device_mutex_unlock(RT_DEV_LOCK_FLASH);
		return 1;
	}
	device_mutex_unlock(RT_DEV_LOCK_FLASH);
	flash_offset += write_len;
	flash_write_len += write_len;
	return ret;	
}
int set_signature(uart_ymodem_t *ptr)
{
	int ret = 1 ;
	uint32_t uart_ota_target_index = OTA_INDEX_2;
	
	if(ptr->image_address == OTA1_ADDR)
		uart_ota_target_index = OTA_INDEX_1;
	else
		uart_ota_target_index = OTA_INDEX_2;
	
	if((flash_write_len <= 0) || (flash_write_len != (OtaTargetHdr.FileImgHdr.ImgLen - 8))) {
		printf("\n\rdownload new firmware failed\n");
		return 1;
	}
	printf("\n\r[%s] %s\n", __FUNCTION__, uart_signature);
	 /*-------------step6: verify checksum and update signature-----------------*/
	if(verify_ota_checksum(ptr->image_address, flash_write_len, uart_signature, &OtaTargetHdr)){
		if(!change_ota_signature(ptr->image_address, uart_signature, uart_ota_target_index)) {
			printf("\n%s: change signature failed\n", __FUNCTION__);
			return 1;
		}
		ret = 0;
	} else {
		/*if checksum error, clear the signature zone which has been 
		written in flash in case of boot from the wrong firmware*/
		#if 1
		device_mutex_lock(RT_DEV_LOCK_FLASH);
		flash_erase_sector(&ptr->flash, ptr->image_address - SPI_FLASH_BASE);
		device_mutex_unlock(RT_DEV_LOCK_FLASH);
		#endif
	}
	return ret;
}

#else
int data_write_to_flash(uart_ymodem_t *ptr)
{
	int ret = 0;
//	uint32_t update_image_address = IMAGE_TWO;
	static int offset = 0x0;
	u32 data;
	static int flags = 1;	//write update image header only once
//  int file_blk_size = 0
	
	flash_read_word(&ptr->flash, OFFSET_DATA, &data);
//	file_blk_size = ((ptr->filelen - 1)/4096) + 1;
	if(data == ~0x0){
		flash_write_word(&ptr->flash, OFFSET_DATA, ptr->image_address);
	}
//	printf("image_address get from flash =  0x%x\n\r",ptr->image_address);
	//erase flash where to be written,since ymodem blk size can be 128 or 1024,so, erase once when gather 4096
	if(offset ==0 || (offset % 4096)==0){
		flash_erase_sector(&ptr->flash, ptr->image_address + offset);
	}
	//write to flash
	//write back image size and address
	if(!flags){
		flash_write_word(&ptr->flash, ptr->image_address, ptr->filelen);
		flash_write_word(&ptr->flash, ptr->image_address+4,0x10004000);
		flags = 1;
	}
//	ymodem_flashwrite(update_image_address + offset, ptr->uart_rcv_buf, ptr->len);
	device_mutex_lock(RT_DEV_LOCK_FLASH);
	flash_stream_write(&ptr->flash, ptr->image_address+offset, ptr->len, ptr->uart_rcv_buf);
	device_mutex_unlock(RT_DEV_LOCK_FLASH);
	offset += ptr->len;
	
	return ret;
}

int set_signature(uart_ymodem_t *ptr)
{
	int ret = 0;
	uint32_t sig_readback0,sig_readback1;
	uint32_t oldimg2addr;

	//old image address
	flash_read_word(&ptr->flash, 0x18, &oldimg2addr);
	oldimg2addr = (oldimg2addr&0xFFFF)*1024;
	printf(" lod image  address 0x%x\n\r",oldimg2addr);
	//Set signature in New Image 2 addr + 8 and + 12
//	flash_write_word(&ptr->flash,ptr->image_address + 8, 0x35393138);//0x35393138
//	flash_write_word(&ptr->flash,ptr->image_address + 12, 0x31313738);
//	flash_read_word(&ptr->flash, ptr->image_address + 8, &sig_readback0);
//	flash_read_word(&ptr->flash, ptr->image_address + 12, &sig_readback1);
//	printf(" new signature %x,%x,\n\r",sig_readback0, sig_readback1);
#if 1
	flash_write_word(&ptr->flash,oldimg2addr + 8, 0x35393130);
	flash_write_word(&ptr->flash,oldimg2addr + 12, 0x31313738);
	flash_read_word(&ptr->flash, oldimg2addr + 8, &sig_readback0);
	flash_read_word(&ptr->flash, oldimg2addr + 12, &sig_readback1);
	printf(" old signature %x,%x\n\r",sig_readback0, sig_readback1);
#endif
	printf(" set signature success!\n\r");

	return ret;
}

#endif

#if AUTO_REBOOT
void auto_reboot(void)
{
#if defined(CONFIG_PLATFORM_8711B)
	//wifi_off();

	/*  Set processor clock to default before system reset */
	//HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_CLK_CTRL1, 0x00000021);
	CPU_ClkSet(CLK_31_25M);
	osDelay(100);

	/* CPU reset: Cortex-M3 SCB->AIRCR*/
	NVIC_SystemReset();
#else
	// Set processor clock to default before system reset
	HAL_WRITE32(SYSTEM_CTRL_BASE, 0x14, 0x00000021);
	osDelay(100);

	// Cortex-M3 SCB->AIRCR
	HAL_WRITE32(0xE000ED00, 0x0C, (0x5FA << 16) |                             // VECTKEY
	                              (HAL_READ32(0xE000ED00, 0x0C) & (7 << 8)) | // PRIGROUP
	                              (1 << 2));                                  // SYSRESETREQ
	while(1) osDelay(1000);

#endif
}
#endif

static void uart_ymodem_thread(void* param)
{
	u8 ch;
	u32 stat, error_bit = 0, transfer_over = 0;
	u32 can_counter = 0, eot_counter = 0;
	u32 i, send_count = 0 , ret = 0;
	static int first_time = 1;
	uart_ymodem_t *ymodem_ptr = (uart_ymodem_t *)param;
	printf(" ==>uart ymodem_task\r\n");
	while(1)
	{
		//wait 2min,2*60*1000/100
		if(send_count >= (2*60*10)){
			error_bit = 6;
			printf("no response after 2min\r\n");
			goto exit;
		}
		else{
			if (ymodem_ptr->crc_mode){
				uart_sendbyte(ymodem_ptr,MODEM_C);	//first receiver send c
			}
			else{
				uart_sendbyte(ymodem_ptr,MODEM_NAK);
			}
			send_count++; 
		}
		if(xSemaphoreTake(ymodem_ptr->uart_rx_sema, 0) == pdTRUE){
			//RtlUpSema(&ymodem_ptr->uart_rx_sema);			
			rtw_up_sema(&ymodem_ptr->uart_rx_sema);
			break;
		}
		else
			// send every 100ms
			vTaskDelay(100);
	}
start:
	while(xSemaphoreTake(ymodem_ptr->uart_rx_sema, portMAX_DELAY) == pdTRUE){
//		ymodem_ptr->tick_current = ymodem_ptr->tick_last_update = xTaskGetTickCount();
		ymodem_ptr->tick_current = xTaskGetTickCount();
		while((int)(ymodem_ptr->tick_current - ymodem_ptr->tick_last_update) < 50 ){
			ymodem_ptr->tick_current = xTaskGetTickCount();
			vTaskDelay(5);
		}
		printf("uart_recv_index = %d current=%d last=%d\r\n",ymodem_ptr->uart_recv_index, ymodem_ptr->tick_current, ymodem_ptr->tick_last_update);
		/*uart data recv done and process what we have recvied*/
		stat = uart_recvbytetimeout(ymodem_ptr,&ch);
		if (stat == 0)
		{
			switch (ch)
			{
				case MODEM_SOH :
					ymodem_ptr->len = 128;
//					printf(" char recved was MODEM_SOH!\r\n");
					break;
				case MODEM_STX :
					ymodem_ptr->len = 1024;
//					printf(" char recved was MODEM_STX!\r\n");
					break;
				case MODEM_CAN :
					if ((++can_counter) >= MODEM_CAN_COUNT)
					{
						error_bit = 1;
						goto exit;
					}
//					printf(" char recved was MODEM_CAN!\r\n");
					break;
				case MODEM_EOT :
//					printf(" char recved was MODEM_EOT!\r\n");
					if ((++eot_counter) >= MODEM_EOT_COUNT)
					{
						uart_sendbyte(ymodem_ptr,MODEM_ACK);
						if (ymodem_ptr->modemtype == 2)	//Ymodem protocol 
						{
							uart_sendbyte(ymodem_ptr,MODEM_C);			//first send a C
							uart_sendbyte(ymodem_ptr,MODEM_ACK);			//then send ack
							uart_sendbyte(ymodem_ptr,MODEM_C);			// and then send c
							modem_cancle(ymodem_ptr);					//cancel the transits
						}
						transfer_over = 1;
						goto exit;
					}
					else
					{
						uart_sendbyte(ymodem_ptr,MODEM_ACK);
						uart_sendbyte(ymodem_ptr,MODEM_C);
						goto start;
					}
					break;
				default:
					error_bit = 1;
					goto exit;
					break;
			}
		}

		//block num check
		if(block_num_check(ymodem_ptr)){
			error_bit = 2;
			goto exit;
		}
#if CONFIG_CALC_FILE_SIZE	
		// calculate file name and file size
		if(ymodem_ptr->nxt_num == 0 && first_time){
			error_bit = calc_file_name_size(ymodem_ptr,&ymodem_ptr->uart_irq_buf[3]);
//			first_time = 0;
		}
#endif	
		//copy data from uart irq buf to uart recv buf without header
		for (i=0; i<ymodem_ptr->len; i++)
		{
			stat = uart_recvbytetimeout(ymodem_ptr,&ymodem_ptr->uart_rcv_buf[i]);
//			printf(" data recv[%d] =%x\r\n",i,ymodem_ptr->uart_rcv_buf[i]);		
		}
		//write data to flash,but do not write first block data
		if(ymodem_ptr->nxt_num != 0 || !first_time){
			if(data_write_to_flash(ymodem_ptr)){
				error_bit = 3;
				goto exit;
			}
			first_time = 0;
		}
		//crc check
		ret = crc_check(ymodem_ptr);
		if(ret == 1){
			error_bit = 4;
			goto exit;
		}
		else if(ret == 2 && ymodem_ptr->nxt_num == 0xff){
			printf(" next num = %x\r\n",ymodem_ptr->nxt_num);
			transfer_over = 1;
			goto exit;
		}
	
#if 0 //avoid skip block
		uart_ymodem->cur_num = blk;
		if (blk != uart_ymodem->nxt_num)
		{
			error_bit = -1;
		}
#endif		
		ymodem_ptr->nxt_num++;
		ymodem_ptr->rec_err=0;
		//start another round
		if(start_next_round(ymodem_ptr)){
			error_bit = 5;
			printf(" start next round failed!\r\n");
			goto exit;		
		}
}
exit:
	//if anything goes wrong or transfer over,we kill ourself.
	if(error_bit || transfer_over){
		if(error_bit)
			printf("error!!! error bit = %d\r\n",error_bit);
		else{
			printf(" [%s, %d Bytes] transfer_over!\r\n",ymodem_ptr->filename,ymodem_ptr->filelen);	
			ret = set_signature(ymodem_ptr);
#if DUMP_DATA
			flash_dump_data(ymodem_ptr);
#endif
#if AUTO_REBOOT
			if(!ret){
				printf("\n\r[%s] Ready to reboot\r\n", __FUNCTION__);
				auto_reboot();
			}
#endif
		}
	}
	first_time = 1;
	uart_ymodem_deinit(ymodem_ptr);
	vTaskDelete(NULL);	
}

int uart_ymodem(void)
{
	int ret = 0;
	uart_ymodem_t *uart_ymodem_ptr;
	
	printf("uart ymodem update start\r\n");
	//uart_ymodem_ptr = (uart_ymodem_t *)RtlMalloc(sizeof(uart_ymodem_t));
	uart_ymodem_ptr = (uart_ymodem_t *)rtw_malloc(sizeof(uart_ymodem_t));
	if(!uart_ymodem_ptr){
		printf("uart ymodem malloc fail!\r\n");
		ret = -1;
		return ret;
	}
	uart_ymodem_init(uart_ymodem_ptr);
	if(ret == -1){
		ret = -1;
		return ret;
	}
	//uart initial
	uart_init(uart_ymodem_ptr);	
	if(xTaskCreate(uart_ymodem_thread, ((const char*)"uart_ymodem_thread"), UART_YMODEM_TASK_DEPTH, uart_ymodem_ptr, UART_YMODEM_TASK_PRIORITY, NULL) != pdPASS)
		printf("%s xTaskCreate(uart_thread) failed\r\n", __FUNCTION__);
	
	return ret;
}
